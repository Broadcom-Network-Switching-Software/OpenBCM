/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mem.h
 * Purpose:     Include file for memory access functions: ARL, L2X, L3X.
 */

#ifndef _PCID_MEM_H
#define _PCID_MEM_H

#include "pcid.h"
#include <sys/types.h>
#include <soc/mcm/memregs.h>


#ifdef	BCM_XGS_SWITCH_SUPPORT
extern int soc_internal_l2x_read(pcid_info_t *pcid_info, uint32 addr,
                                 l2x_entry_t *entry);

extern int soc_internal_l2x_write(pcid_info_t *pcid_info, uint32 addr, 
                                  l2x_entry_t *entry);
extern int soc_internal_l2x_lkup(pcid_info_t *pcid_info, 
                                 l2x_entry_t *entry_lookup, uint32 *result);
extern int soc_internal_l2x_init(pcid_info_t * pcid_info) ;
extern int soc_internal_l2x_del(pcid_info_t *pcid_info, l2x_entry_t *entry);
extern int soc_internal_l2x_ins(pcid_info_t *pcid_info, l2x_entry_t *entry);
#endif	/* BCM_XGS_SWITCH_SUPPORT */
#ifdef	BCM_FIREBOLT_SUPPORT
extern int soc_internal_l2x2_entry_ins(pcid_info_t *pcid_info, uint8 banks,
                                       l2x_entry_t *entry,
                                       uint32 *result);
extern int soc_internal_l2x2_entry_del(pcid_info_t *pcid_info, uint8 banks,
                                       l2x_entry_t *entry,
                                       uint32 *result);
extern int soc_internal_l2x2_entry_lkup(pcid_info_t * pcid_info, uint8 banks,
                                        l2x_entry_t *entry_lookup,
                                        uint32 *result);
extern int soc_internal_l3x2_read(pcid_info_t * pcid_info, soc_mem_t mem,
                                  uint32 addr, uint32 *entry);
extern int soc_internal_l3x2_write(pcid_info_t * pcid_info, soc_mem_t mem,
                                   uint32 addr, uint32 *entry);
extern int soc_internal_l3x2_entry_ins(pcid_info_t *pcid_info,
                                       uint32 inv_bank_map, void *entry,
                                       uint32 *result);
extern int soc_internal_l3x2_entry_del(pcid_info_t *pcid_info,
                                       uint32 inv_bank_map, void *entry,
                                       uint32 *result);
extern int soc_internal_l3x2_entry_lkup(pcid_info_t * pcid_info,
                                       uint32 inv_bank_map, void *entry,
                                       uint32 *result);
#endif	/* BCM_FIREBOLT_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
extern int soc_internal_exact_match_read(pcid_info_t * pcid_info,
                                         soc_mem_t mem, uint32 addr,
                                         uint32 *entry);
extern int soc_internal_exact_match_write(pcid_info_t * pcid_info,
                                          soc_mem_t mem, uint32 addr,
                                          uint32 *entry);
extern int soc_internal_exact_match_ins(pcid_info_t *pcid_info,
                                        uint32 inv_bank_map, void *entry,
                                        uint32 *result);
extern int soc_internal_exact_match_del(pcid_info_t *pcid_info,
                                        uint32 inv_bank_map, void *entry,
                                        uint32 *result);
extern int soc_internal_exact_match_lkup(pcid_info_t * pcid_info,
                                         uint32 inv_bank_map, void *entry,
                                         uint32 *result);
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef	BCM_EASYRIDER_SUPPORT
extern int soc_internal_l2_er_entry_ins(pcid_info_t *pcid_info,
                                        uint32 *entry);
extern int soc_internal_l2_er_entry_del(pcid_info_t *pcid_info,
                                        uint32 *entry);
extern int soc_internal_l2_er_entry_lkup(pcid_info_t * pcid_info,
                                         uint32 *entry);
extern int soc_internal_l3_er_entry_ins(pcid_info_t *pcid_info,
                                        uint32 *entry, int l3v6);
extern int soc_internal_l3_er_entry_del(pcid_info_t *pcid_info,
                                        uint32 *entry, int l3v6);
extern int soc_internal_l3_er_entry_lkup(pcid_info_t * pcid_info,
                                         uint32 *entry, int l3v6);
extern int soc_internal_mcmd_write(pcid_info_t * pcid_info,
                                   soc_mem_t mem, uint32 *data, int cmd);
#endif	/* BCM_EASYRIDER_SUPPORT */
#ifdef BCM_TRX_SUPPORT
extern int soc_internal_l2_tr_entry_ins(pcid_info_t *pcid_info,
                                        uint32 inv_bank_map, void *entry,
                                        uint32 *result);
extern int soc_internal_l2_tr_entry_del(pcid_info_t *pcid_info,
                                        uint32 inv_bank_map, void *entry,
                                        uint32 *result);
extern int soc_internal_l2_tr_entry_lkup(pcid_info_t *pcid_info,
                                         uint32 inv_bank_map, void *entry,
                                         uint32 *result);
extern int soc_internal_vlan_xlate_hash(pcid_info_t * pcid_info,
                                        vlan_xlate_entry_t *entry, int dual);

extern int soc_internal_vlan_xlate_entry_read(pcid_info_t * pcid_info, 
                                              uint32 addr,
                                              vlan_xlate_entry_t *entry);
extern int soc_internal_vlan_xlate_extended_entry_read(pcid_info_t * pcid_info, 
                                                       uint32 block, uint32 addr,
                                                       vlan_xlate_entry_t *entry);
extern int soc_internal_vlan_xlate_entry_write(pcid_info_t * pcid_info, 
                                              uint32 addr,
                                              vlan_xlate_entry_t *entry);
extern int soc_internal_vlan_xlate_extended_entry_write(pcid_info_t * pcid_info, 
                                                        uint32 block, uint32 addr,
                                                        vlan_xlate_entry_t *entry);
extern int soc_internal_vlan_xlate_entry_ins(pcid_info_t *pcid_info, 
                                             uint8 banks,
                                             vlan_xlate_entry_t *entry, 
                                             uint32 *result);
extern int soc_internal_vlan_xlate_entry_del(pcid_info_t * pcid_info, 
                                             uint8 banks,
                                             vlan_xlate_entry_t *entry, 
                                             uint32 *result);
extern int soc_internal_vlan_xlate_entry_lkup(pcid_info_t * pcid_info, 
                                              uint8 banks,
                                              vlan_xlate_entry_t *entry_lookup, 
                                              uint32 *result);

extern int soc_internal_egr_vlan_xlate_entry_ins(pcid_info_t *pcid_info,
                                                 uint8 banks,
                                                 egr_vlan_xlate_entry_t *entry,
                                                 uint32 *result);
extern int soc_internal_egr_vlan_xlate_entry_del(pcid_info_t * pcid_info,
                                                 uint8 banks,
                                                 egr_vlan_xlate_entry_t *entry,
                                                 uint32 *result);
extern int soc_internal_egr_vlan_xlate_entry_lkup(pcid_info_t * pcid_info,
                                                  uint8 banks,
                                                  egr_vlan_xlate_entry_t *entry_lookup,
                                                  uint32 *result);
#endif /* BCM_TRX_SUPPORT */

#ifdef BCM_TRIDENT2_SUPPORT
extern int soc_internal_ing_vp_vlan_member_ins(pcid_info_t *pcid_info,
                                               uint8 banks, void *entry,
                                               uint32 *result);
extern int soc_internal_ing_vp_vlan_member_del(pcid_info_t *pcid_info,
                                               uint8 banks, void *entry,
                                               uint32 *result);
extern int soc_internal_ing_vp_vlan_member_lkup(pcid_info_t *pcid_info,
                                                uint8 banks, void *entry,
                                                uint32 *result);

extern int soc_internal_egr_vp_vlan_member_ins(pcid_info_t *pcid_info,
                                               uint8 banks, void *entry,
                                               uint32 *result);
extern int soc_internal_egr_vp_vlan_member_del(pcid_info_t *pcid_info,
                                               uint8 banks, void *entry,
                                               uint32 *result);
extern int soc_internal_egr_vp_vlan_member_lkup(pcid_info_t *pcid_info,
                                                uint8 banks, void *entry,
                                                uint32 *result);

extern int soc_internal_ing_dnat_address_type_ins(pcid_info_t *pcid_info,
                                               uint8 banks, void *entry,
                                               uint32 *result);
extern int soc_internal_ing_dnat_address_type_del(pcid_info_t *pcid_info,
                                               uint8 banks, void *entry,
                                               uint32 *result);
extern int soc_internal_ing_dnat_address_type_lkup(pcid_info_t *pcid_info,
                                                uint8 banks, void *entry,
                                                uint32 *result);

extern int soc_internal_l2_endpoint_id_ins(pcid_info_t *pcid_info, uint8 banks,
                                           void *entry, uint32 *result);
extern int soc_internal_l2_endpoint_id_del(pcid_info_t *pcid_info, uint8 banks,
                                           void *entry, uint32 *result);
extern int soc_internal_l2_endpoint_id_lkup(pcid_info_t *pcid_info,
                                            uint8 banks, void *entry,
                                            uint32 *result);

extern int soc_internal_endpoint_queue_map_ins(pcid_info_t *pcid_info,
                                               uint8 banks,
                                               void *entry, uint32 *result);
extern int soc_internal_endpoint_queue_map_del(pcid_info_t *pcid_info,
                                               uint8 banks,
                                               void *entry, uint32 *result);
extern int soc_internal_endpoint_queue_map_lkup(pcid_info_t *pcid_info,
                                                uint8 banks, void *entry,
                                                uint32 *result);
#endif /* BCM_TRIDNET2_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
extern int soc_internal_mpls_hash(pcid_info_t * pcid_info,
                                  mpls_entry_entry_t *entry, int dual);

extern int soc_internal_mpls_entry_read(pcid_info_t * pcid_info,
                                        uint32 addr,
                                        mpls_entry_entry_t *entry);
extern int soc_internal_mpls_entry_write(pcid_info_t * pcid_info,
                                         uint32 addr,
                                         mpls_entry_entry_t *entry);
extern int soc_internal_mpls_entry_ins(pcid_info_t *pcid_info,
                                       uint8 banks,
                                       mpls_entry_entry_t *entry,
                                       uint32 *result);
extern int soc_internal_mpls_entry_del(pcid_info_t * pcid_info,
                                       uint8 banks,
                                       mpls_entry_entry_t *entry,
                                       uint32 *result);
extern int soc_internal_mpls_entry_lkup(pcid_info_t * pcid_info,
                                        uint8 banks,
                                        mpls_entry_entry_t *entry_lookup,
                                        uint32 *result);

#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TOMAHAWK3_SUPPORT
extern int soc_internal_tunnel_entry_ins(pcid_info_t *pcid_info,
                                         uint8 inv_bank_map,
                                         void *entry,
                                         uint32 *result);
#endif /* BCM_TOMAHAWK3_SUPPORT */

#endif	/* _PCID_MEM_H */
