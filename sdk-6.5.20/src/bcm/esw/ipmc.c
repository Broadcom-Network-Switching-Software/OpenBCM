/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IP Multicast API
 */

#ifdef INCLUDE_L3

#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/l3x.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#if defined BCM_METROLITE_SUPPORT
#include <soc/katana2.h>
#endif

#include <bcm/error.h>
#include <bcm/ipmc.h>

#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/proxy.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/firebolt.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <bcm_int/esw/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/katana.h>
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif /* BCM_KATANA2_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_HGPROXY_COE_SUPPORT)
#include <bcm_int/esw/xgs5.h>
#endif /* BCM_HGPROXY_COE_SUPPORT */

#include <bcm_int/esw/ipmc.h>
#include <bcm_int/esw/switch.h>

#include <bcm_int/esw/multicast.h>
#ifndef BCM_SW_STATE_DUMP_DISABLE
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */
#endif /* BCM_SW_STATE_DUMP_DISABLE */

#include <bcm_int/common/multicast.h>
#include <bcm_int/esw_dispatch.h>

#ifdef BCM_TRIDENT3_SUPPORT
#include <soc/esw/cancun.h>
#include <soc/esw/cancun_enums.h>
#endif /* BCM_TRIDENT3_SUPPORT */

_bcm_esw_ipmc_t esw_ipmc_info[BCM_MAX_NUM_UNITS] = {{ 0 }};

#define	IPMC_REPL_UNIT(unit) \
	if (!soc_feature(unit, soc_feature_ip_mcast_repl)) \
        { return BCM_E_UNAVAIL; }

static int _bcm_ipmc_init[BCM_MAX_NUM_UNITS];
static int _bcm_ipmc_idx_ret_type[BCM_MAX_NUM_UNITS];
static int _bcm_ipmc_repl_threshold[BCM_MAX_NUM_UNITS];

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0     SOC_SCACHE_VERSION(1,0)
#define BCM_WB_VERSION_1_1     SOC_SCACHE_VERSION(1,1)
#define BCM_WB_VERSION_1_2     SOC_SCACHE_VERSION(1,2)
#define BCM_WB_VERSION_1_3     SOC_SCACHE_VERSION(1,3)
#define BCM_WB_VERSION_1_4     SOC_SCACHE_VERSION(1,4)
#define BCM_WB_VERSION_1_5     SOC_SCACHE_VERSION(1,5)
#define BCM_WB_DEFAULT_VERSION BCM_WB_VERSION_1_5

/* Entry 0 of the MMU_IPMC_VLAN_TBL is reserved from replication use.
 * We can use the MSB_VLAN bits as long as the rest is not marked
 * for use. This allows us to store a few IPMC-related flags for
 * recovery during Warm Boot.
 * Note that we do not check for IPMC init, because this is also used
 * to store the L2MC multicast group type encoding marker.  Although
 * this means we can only recover the L2MC group type choice with L3
 * support, the entire multicast module requires that, so there is no
 * point in using the L2MC group type multicast encoding without
 * L3 support.
 */

int
_bcm_esw_ipmc_repl_wb_flags_set(int unit, uint8 flags, uint8 flags_mask)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) { 
        mmu_ipmc_vlan_tbl_entry_t vlan_entry;

#ifdef BCM_KATANA2_SUPPORT
        if (SOC_IS_KATANA2(unit)) {
            if (SOC_MEM_IS_VALID(unit, MMU_REPL_LIST_TBLm)) {
                mmu_repl_list_tbl_entry_t repl_list_entry;       
                uint8 oms_bits, ms_bits;

                /* Entry 0 is reserved from replication use.  We can use the
                 * MSB_VLANf bits as long as the rest is not marked for use. */

                SOC_IF_ERROR_RETURN
                   (READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY, 0,
                                         &repl_list_entry));

                oms_bits = ms_bits =
                      soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                        &repl_list_entry, MSB_VLANf);
                ms_bits &= ~flags_mask;
                ms_bits |= (flags & flags_mask);
                ms_bits &= 0xff;
                if (ms_bits != oms_bits) {
                    soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                        MSB_VLANf, ms_bits);
                    SOC_IF_ERROR_RETURN
                    (WRITE_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ALL,
                                              0, &repl_list_entry));
                }
                return BCM_E_NONE;
            }
        } else 
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            uint32 repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
            soc_mem_t repl_list_tbl;
            uint16 next_hop_index_0_field;

            if(SOC_IS_TOMAHAWK3(unit)) {
                repl_list_tbl = MMU_REPL_LIST_TBLm;
            } else if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
                repl_list_tbl = MMU_REPL_LIST_TBL_SPLIT0_PIPE0m;
            } else {
                repl_list_tbl = MMU_REPL_LIST_TBL_PIPE0m;
            }

            /* Entry 0 is reserved from replication use.  We can use the
             * NEXT_HOP_INDEX_0, 1, 2, and 3 fields as long as MODE = 1 and
             * MODE_1_BITMAP = 0.
             */
             SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, repl_list_tbl, MEM_BLOCK_ALL,
                                            0, repl_list_entry));

            if (soc_mem_field_valid(unit, repl_list_tbl,
                                    NEXT_HOP_INDEX_0f)) {
                next_hop_index_0_field = soc_mem_field32_get(
                                             unit, repl_list_tbl, 
                                             repl_list_entry, NEXT_HOP_INDEX_0f);
                next_hop_index_0_field &= ~flags_mask;
                next_hop_index_0_field |= (flags & flags_mask);
                next_hop_index_0_field &= 0xffff;
                soc_mem_field32_set(unit, repl_list_tbl, repl_list_entry,
                            NEXT_HOP_INDEX_0f, next_hop_index_0_field);
           }
           soc_mem_field32_set(unit, repl_list_tbl, repl_list_entry,
                                           MODEf, 1);
           soc_mem_field32_set(unit, repl_list_tbl, repl_list_entry,
                                            MODE_1_BITMAPf, 0);

           SOC_IF_ERROR_RETURN(
               soc_mem_write(unit, repl_list_tbl, MEM_BLOCK_ALL,
               0, repl_list_entry));

           return BCM_E_NONE;
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT) 
        if (SOC_MEM_IS_VALID(unit, MMU_REPL_LIST_TBLm)) {
            mmu_repl_list_tbl_entry_t repl_list_entry;
            uint16 next_hop_index_0_field;

            /* Entry 0 is reserved from replication use.  We can use the
             * NEXT_HOP_INDEX_0, 1, 2, and 3 fields as long as MODE = 1 and
             * MODE_1_BITMAP = 0.
             */
            SOC_IF_ERROR_RETURN
                (READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY, 0,
                                             &repl_list_entry));
            if (soc_mem_field_valid(unit,
                                       MMU_REPL_LIST_TBLm, NEXT_HOP_INDEX_0f)) {
                next_hop_index_0_field = soc_MMU_REPL_LIST_TBLm_field32_get(
                                             unit, 
                                             &repl_list_entry, NEXT_HOP_INDEX_0f);
                next_hop_index_0_field &= ~flags_mask;
                next_hop_index_0_field |= (flags & flags_mask);
                next_hop_index_0_field &= 0xffff;
                soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                            NEXT_HOP_INDEX_0f, next_hop_index_0_field);
           }
           soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                                              MODEf, 1);
           soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                                              MODE_1_BITMAPf, 0);
           SOC_IF_ERROR_RETURN
                 (WRITE_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ALL,
                                       0, &repl_list_entry));
           return BCM_E_NONE;
        } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            uint16 intf_num_0_bits;

            /* Entry 0 is reserved from replication use.  We can use the
             * INTF_NUM_0, 1, 2, and 3 fields as long as MODE = 1 and
             * MODE_1_BITMAP = 0.
             */
            SOC_IF_ERROR_RETURN
                (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL, 0, &vlan_entry));
            intf_num_0_bits = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                    &vlan_entry, INTF_NUM_0f);
            intf_num_0_bits &= ~flags_mask;
            intf_num_0_bits |= (flags & flags_mask);
            intf_num_0_bits &= 0x3fff;
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                    MODEf, 1);
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                    MODE_1_BITMAPf, 0);
            soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                    INTF_NUM_0f, intf_num_0_bits);
            SOC_IF_ERROR_RETURN
                (WRITE_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                          0, &vlan_entry));
            return BCM_E_NONE;

        } else
#endif /* BCM_TRIDENT_SUPPORT */
        if (soc_mem_field_valid(unit, MMU_IPMC_VLAN_TBLm, MSB_VLANf)) {
            uint8 oms_bits, ms_bits, flen;
            uint32 f_bmp;

            /* Entry 0 is reserved from replication use.  We can use the
             * MSB_VLANf bits as long as the rest is not marked for use. */
            SOC_IF_ERROR_RETURN
                (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL, 0, &vlan_entry));
            oms_bits = ms_bits =
                soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                        &vlan_entry, MSB_VLANf);
            ms_bits &= ~flags_mask;
            ms_bits |= (flags & flags_mask);
            flen = soc_mem_field_length(unit, MMU_IPMC_VLAN_TBLm, MSB_VLANf);
            /* Set the mask for MSB_VLANf */
            f_bmp = ((1 << flen) - 1);
            ms_bits &= f_bmp;
            if (ms_bits != oms_bits) {
                soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                        MSB_VLANf, ms_bits);
                SOC_IF_ERROR_RETURN
                    (WRITE_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                              0, &vlan_entry));
            }
            return BCM_E_NONE;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return BCM_E_UNAVAIL;
}

int
_bcm_esw_ipmc_repl_wb_flags_get(int unit, uint8 flags_mask, uint8 *flags)
{
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        mmu_ipmc_vlan_tbl_entry_t vlan_entry;
#ifdef BCM_KATANA2_SUPPORT 
        if( SOC_IS_KATANA2(unit)) {
            if (SOC_MEM_IS_VALID(unit, MMU_REPL_LIST_TBLm)) {
                mmu_repl_list_tbl_entry_t repl_list_entry;
                uint8 ms_bits;

                SOC_IF_ERROR_RETURN
                  (READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ALL, 0, &repl_list_entry));
                ms_bits = soc_MMU_REPL_LIST_TBLm_field32_get(unit, &repl_list_entry,
                          MSB_VLANf);
                ms_bits &= flags_mask;
                *flags = ms_bits;
                return BCM_E_NONE;
            }
        } else 
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            mmu_repl_list_tbl_entry_t repl_list_entry;
            int mode;
            uint16 next_hop_index_0_field;
#ifdef BCM_TOMAHAWK3_SUPPORT
           if(SOC_IS_TOMAHAWK3(unit)) {
                SOC_IF_ERROR_RETURN
                    (READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ALL,
                                                    0, &repl_list_entry));

           } else
#endif
           {
               if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
                   SOC_IF_ERROR_RETURN
                       (READ_MMU_REPL_LIST_TBL_SPLIT0_PIPE0m(unit, MEM_BLOCK_ANY, 0,
                                           &repl_list_entry));
               } else {
                   SOC_IF_ERROR_RETURN
                       (READ_MMU_REPL_LIST_TBL_PIPE0m(unit, MEM_BLOCK_ANY, 0,
                                                      &repl_list_entry));
               }
           }

           if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
               mode = soc_MMU_REPL_LIST_TBL_SPLIT0_PIPE0m_field32_get(unit, &repl_list_entry,
                            MODEf);
                if (0 == mode) {
                    /* No flags have been set */
                    *flags = 0;
                    return BCM_E_NONE;
                }
                next_hop_index_0_field = soc_MMU_REPL_LIST_TBL_SPLIT0_PIPE0m_field32_get(unit,
                                         &repl_list_entry, NEXT_HOP_INDEX_0f);
                next_hop_index_0_field &= flags_mask;
                *flags = next_hop_index_0_field;
           } else {
               mode = soc_MMU_REPL_LIST_TBLm_field32_get(unit, &repl_list_entry,
                            MODEf);
                if (0 == mode) {
                    /* No flags have been set */
                    *flags = 0;
                    return BCM_E_NONE;
                }
                next_hop_index_0_field = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                                         &repl_list_entry, NEXT_HOP_INDEX_0f);
                next_hop_index_0_field &= flags_mask;
                *flags = next_hop_index_0_field;
           }
            return BCM_E_NONE;
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, MMU_REPL_LIST_TBLm)) {
            mmu_repl_list_tbl_entry_t repl_list_entry;
            int mode;
            uint16 next_hop_index_0_field;

            SOC_IF_ERROR_RETURN
            (READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY, 0,
                                      &repl_list_entry));
            mode = soc_MMU_REPL_LIST_TBLm_field32_get(unit, &repl_list_entry,
                        MODEf);
            if (0 == mode) {
                /* No flags have been set */
                *flags = 0;
                return BCM_E_NONE;
            }
            next_hop_index_0_field = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                                     &repl_list_entry, NEXT_HOP_INDEX_0f);
            next_hop_index_0_field &= flags_mask;
            *flags = next_hop_index_0_field;
            return BCM_E_NONE;
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
        if (SOC_IS_TD_TT(unit)) {
            int mode;
            uint16 intf_num_0_bits;

            SOC_IF_ERROR_RETURN
                (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL, 0, &vlan_entry));
            mode = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit, &vlan_entry,
                    MODEf);
            if (0 == mode) {
                /* No flags have been set */
                *flags = 0;
                return BCM_E_NONE;
            }
            intf_num_0_bits = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                    &vlan_entry, INTF_NUM_0f);
            intf_num_0_bits &= flags_mask;
            *flags = intf_num_0_bits;
            return BCM_E_NONE;

        } else
#endif /* BCM_TRIDENT_SUPPORT */
        if (soc_mem_field_valid(unit, MMU_IPMC_VLAN_TBLm, MSB_VLANf)) {
            uint8 ms_bits;

            SOC_IF_ERROR_RETURN
                (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL, 0, &vlan_entry));
            ms_bits = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit, &vlan_entry,
                    MSB_VLANf);
            ms_bits &= flags_mask;
            *flags = ms_bits;
            return BCM_E_NONE;
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_esw_ipmc_repl_wb_threshold_set(int unit, int threshold)
{
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit))
    {
       /* To be fixed for Katana2/Katana */
       return BCM_E_UNAVAIL;
    } else
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        uint32 repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
        soc_mem_t repl_list_tbl;

        if(SOC_IS_TOMAHAWK3(unit)) {
            repl_list_tbl = MMU_REPL_LIST_TBLm;
        } else if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
            repl_list_tbl = MMU_REPL_LIST_TBL_SPLIT0_PIPE0m;
        } else {
            repl_list_tbl = MMU_REPL_LIST_TBL_PIPE0m;
        }

        /* Entry 0 is reserved from replication use.  We can use the
         * NEXT_HOP_INDEX_0, 1, 2, and 3 fields as long as MODE = 1 and
         * MODE_1_BITMAP = 0.
         */
         SOC_IF_ERROR_RETURN
                (soc_mem_read(unit, repl_list_tbl, MEM_BLOCK_ALL,
                                            0, repl_list_entry));

            soc_mem_field32_set(unit, repl_list_tbl, repl_list_entry,
                                           MODEf, 1);
            soc_mem_field32_set(unit, repl_list_tbl, repl_list_entry,
                                            MODE_1_BITMAPf, 0);
        if (soc_mem_field_valid(unit, repl_list_tbl, NEXT_HOP_INDEX_1f)) {
            threshold &= 0xffff;
            soc_mem_field32_set(unit, repl_list_tbl, repl_list_entry,
                                            NEXT_HOP_INDEX_1f, threshold);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, repl_list_tbl,
                                            MEM_BLOCK_ALL, 0, repl_list_entry));

        }
        return BCM_E_NONE;
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)     
    if (SOC_MEM_IS_VALID(unit, MMU_REPL_LIST_TBLm)) {
        mmu_repl_list_tbl_entry_t repl_list_entry;

        /* Entry 0 is reserved from replication use.  We can use the
         * NEXT_HOP_INDEX_0, 1, 2, and 3 fields as long as MODE = 1 and
         * MODE_1_BITMAP = 0.
         */
        SOC_IF_ERROR_RETURN
            (READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY, 0,
                                         &repl_list_entry));
        soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                    MODEf, 1);
        soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                    MODE_1_BITMAPf, 0);
        if (soc_mem_field_valid(unit,MMU_REPL_LIST_TBLm, NEXT_HOP_INDEX_1f)) {
            threshold &= 0xffff;
            soc_MMU_REPL_LIST_TBLm_field32_set(unit, &repl_list_entry,
                    NEXT_HOP_INDEX_1f, threshold);
            SOC_IF_ERROR_RETURN
                (WRITE_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ALL,
                                          0, &repl_list_entry));
        }
        return BCM_E_NONE;
     } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        mmu_ipmc_vlan_tbl_entry_t vlan_entry;

        /* Entry 0 is reserved from replication use.  We can use the
         * INTF_NUM_0, 1, 2, and 3 fields as long as MODE = 1 and
         * MODE_1_BITMAP = 0.
         */
        SOC_IF_ERROR_RETURN
            (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL, 0, &vlan_entry));
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                MODEf, 1);
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                MODE_1_BITMAPf, 0);
        threshold &= 0x3fff;
        soc_MMU_IPMC_VLAN_TBLm_field32_set(unit, &vlan_entry,
                INTF_NUM_1f, threshold);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL,
                                      0, &vlan_entry));
        return BCM_E_NONE;

    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_UNAVAIL;
}

STATIC int
_bcm_esw_ipmc_repl_wb_threshold_get(int unit, int *threshold)
{
#if defined (BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(unit))
    {
       /* To be fixed for Katana2/Katana */
       return BCM_E_UNAVAIL;
    } else
#endif /* BCM_KATANA2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        uint32 repl_list_entry[SOC_MAX_MEM_FIELD_WORDS];
        int mode;
        soc_mem_t repl_list_tbl = MMU_REPL_LIST_TBL_PIPE0m;

#ifdef BCM_TOMAHAWK3_SUPPORT
        if(SOC_IS_TOMAHAWK3(unit)) {
            repl_list_tbl = MMU_REPL_LIST_TBLm;
        } else
#endif
        if (soc_feature(unit, soc_feature_split_2_repl_list_table)) {
            repl_list_tbl = MMU_REPL_LIST_TBL_SPLIT0_PIPE0m;
        } else {
            repl_list_tbl = MMU_REPL_LIST_TBL_PIPE0m;
        }

        SOC_IF_ERROR_RETURN
               (soc_mem_read(unit, repl_list_tbl, MEM_BLOCK_ALL,
                                           0, repl_list_entry));
        mode = soc_mem_field32_get(unit, repl_list_tbl, &repl_list_entry,
                                                  MODEf);
        if (0 == mode) {
            /* Threshold has not been set */
            *threshold = 0;
            return BCM_E_NONE;
        }
        *threshold = soc_mem_field32_get(unit, repl_list_tbl, &repl_list_entry,
                                         NEXT_HOP_INDEX_1f);
        return BCM_E_NONE;
    }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT) 
    if (SOC_MEM_IS_VALID(unit, MMU_REPL_LIST_TBLm)) {
        mmu_repl_list_tbl_entry_t repl_list_entry;
        int mode;

        SOC_IF_ERROR_RETURN
                (READ_MMU_REPL_LIST_TBLm(unit, MEM_BLOCK_ANY, 0,
                                         &repl_list_entry));
        mode = soc_MMU_REPL_LIST_TBLm_field32_get(unit, &repl_list_entry,
                    MODEf);
        if (0 == mode) {
            /* Threshold has not been set */
            *threshold = 0;
            return BCM_E_NONE;
        }
        *threshold = soc_MMU_REPL_LIST_TBLm_field32_get(unit,
                    &repl_list_entry, NEXT_HOP_INDEX_1f);
        return BCM_E_NONE;
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        mmu_ipmc_vlan_tbl_entry_t vlan_entry;
        int mode;

        SOC_IF_ERROR_RETURN
            (READ_MMU_IPMC_VLAN_TBLm(unit, MEM_BLOCK_ALL, 0, &vlan_entry));
        mode = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit, &vlan_entry,
                MODEf);
        if (0 == mode) {
            /* Threshold has not been set */
            *threshold = 0;
            return BCM_E_NONE;
        }
        *threshold = soc_MMU_IPMC_VLAN_TBLm_field32_get(unit,
                &vlan_entry, INTF_NUM_1f);
        return BCM_E_NONE;

    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_required_scache_size_get
 * Purpose:
 *      Get required IPMC module scache size for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 *      size - (OUT) Required scache size.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_esw_ipmc_required_scache_size_get(int unit, uint32 *size)
{
    *size = 0;

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        uint32 temp_size;

        BCM_IF_ERROR_RETURN
            (bcm_td2_ipmc_pim_bidir_scache_size_get(unit, &temp_size));
        *size += temp_size;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
        uint32 temp_size;

        BCM_IF_ERROR_RETURN
            (bcm_tr3_ipmc_repl_l3_intf_scache_size_get(unit, &temp_size));
        *size += temp_size;
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
        uint32 aggid_info_size;
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_th_ipmc_aggid_info_scache_size_get(unit, &aggid_info_size));
            *size += aggid_info_size;
        }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_td2p_ipmc_aggid_info_scache_size_get(unit, &aggid_info_size));
            *size += aggid_info_size;
        }
#endif
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
        if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
            uint32 temp_size;
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th_ipmc_repl_l3_intf_nh_map_scache_size_get(unit,
                                                               &temp_size));
                *size += temp_size;
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_ipmc_repl_l3_intf_nh_map_scache_size_get(unit,
                                                               &temp_size));
                *size += temp_size;
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
                uint32 temp_size;
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_size_get(
                            unit, &temp_size));
                    *size += temp_size;
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_scache_size_get(
                            unit, &temp_size));
                    *size += temp_size;
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_KATANA_SUPPORT
    if ((SOC_IS_KATANA(unit)) ||
        (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit))) {
        uint32 temp_size;
        BCM_IF_ERROR_RETURN
            (_bcm_kt_ipmc_hw_queue_info_scache_size_get(unit, &temp_size));
        *size += temp_size;
    }
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_GREYHOUND_SUPPORT)
            if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                uint32 temp_size;
                BCM_IF_ERROR_RETURN
                   (_bcm_fb_ipmc_scahce_size_get(unit, &temp_size));
                *size += temp_size;
            }
#endif /* BCM_GREYHOUND_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_ipmc_sync
 * Purpose:
 *      Record IPMC module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_esw_ipmc_sync(int unit)
{
    int rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    uint8 *ipmc_scache_ptr;
    uint32 required_scache_size;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_ipmc_required_scache_size_get(unit, &required_scache_size));
    if (required_scache_size == 0) {
        /* Nothing to sync */
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IPMC, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &ipmc_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (rv == BCM_E_NOT_FOUND) {
        /* For Triumph3, BCM_WB_VERSION_1_0 contains no data,
         * while BCM_WB_VERSION_1_1 contains the L3 interface bitmap.
         * When upgrading SDK from VERSION_1_0 to VERSION_1_1 on Triumph3,
         * _bcm_esw_ipmc_reinit will not read any data from IPMC scache since
         * it does not exist. Subsequently, during _bcm_esw_ipmc_sync,
         * _bcm_esw_scache_ptr_get will return BCM_E_NOT_FOUND. In this case,
         * IPMC scache needs to be created.
         */
        rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                required_scache_size, &ipmc_scache_ptr,
                BCM_WB_DEFAULT_VERSION, NULL);
        BCM_IF_ERROR_RETURN(rv);
    }

#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        BCM_IF_ERROR_RETURN
            (bcm_td2_ipmc_pim_bidir_sync(unit, &ipmc_scache_ptr));
    }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
    if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
#ifdef BCM_TOMAHAWK_SUPPORT
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th_ipmc_repl_l3_intf_sync(unit, &ipmc_scache_ptr));
        } else
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            BCM_IF_ERROR_RETURN
                (bcm_tr3_ipmc_repl_l3_intf_sync(unit, &ipmc_scache_ptr));
        }
    }
#endif /* BCM_TRIUMPH3_SUPPORT */

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_th_ipmc_aggid_info_sync(unit, &ipmc_scache_ptr));
        }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
            BCM_IF_ERROR_RETURN
                (bcm_td2p_ipmc_aggid_info_sync(unit, &ipmc_scache_ptr));
        }
#endif
    }
#endif /* BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
        if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th_ipmc_repl_l3_intf_nh_map_sync(unit, &ipmc_scache_ptr));
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_ipmc_repl_l3_intf_nh_map_sync(unit, &ipmc_scache_ptr));
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH3_SUPPORT
        if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
#ifdef BCM_TOMAHAWK_SUPPORT
            if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th_ipmc_repl_l3_intf_trill_nh_map_sync(
                        unit, &ipmc_scache_ptr));
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
            {
                BCM_IF_ERROR_RETURN
                    (bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_sync(
                         unit, &ipmc_scache_ptr));
            }
        }
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
    if ((SOC_IS_KATANA(unit)) ||
        (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit))) {
        BCM_IF_ERROR_RETURN
           (_bcm_kt_ipmc_hw_queue_info_sync(unit, &ipmc_scache_ptr));
    }
#endif /* BCM_KATANA_SUPPORT */

#if defined(BCM_GREYHOUND_SUPPORT)
        if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
            BCM_IF_ERROR_RETURN
               (_bcm_fb_ipmc_sync(unit, &ipmc_scache_ptr));
        }
#endif /* BCM_GREYHOUND_SUPPORT */


    return BCM_E_NONE;
}

STATIC int _bcm_esw_ipmc_reinit(int unit)
{
    int rv = BCM_E_UNAVAIL;
    uint32 required_scache_size;
    soc_scache_handle_t scache_handle;
    uint8               *ipmc_scache_ptr;
    uint16              recovered_ver;
    uint32 additional_scache_size;
#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
    uint32 aggid_info_size = 0;
#endif /* BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */
#if defined BCM_KATANA_SUPPORT
    uint32 sw_hw_queue_info_size = 0;
#endif
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
#ifdef BCM_TRIUMPH_SUPPORT
        if (SOC_IS_TR_VL(unit)) {
            rv = _bcm_tr_ipmc_reinit(unit);
        } else
#endif /* BCM_TRIUMPH_SUPPORT */
        if (!SOC_IS_RAPTOR(unit)) {
            rv = _bcm_xgs3_ipmc_reinit(unit);
        }
        if (BCM_SUCCESS(rv)) {
            uint8 flags;
            int threshold;

            _bcm_ipmc_init[unit] = TRUE;
            rv = _bcm_esw_ipmc_repl_wb_flags_get(unit,
                           _BCM_IPMC_WB_IPMC_GROUP_TYPE_MULTICAST, &flags);
            if (BCM_SUCCESS(rv)) {
                _bcm_ipmc_idx_ret_type[unit] = (0 != flags);
            } else {
                _bcm_ipmc_idx_ret_type[unit] = 0;
            }

            rv = _bcm_esw_ipmc_repl_wb_threshold_get(unit, &threshold);
            if (BCM_SUCCESS(rv)) {
                _bcm_ipmc_repl_threshold[unit] = threshold;
            } else {
                _bcm_ipmc_repl_threshold[unit] = 0;
                rv = BCM_E_NONE;
            }
        }
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    if (SOC_IS_XGS12_FABRIC(unit)) {
        rv = BCM_E_NONE;
    }

    /* Recover from IPMC scache */
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_ipmc_required_scache_size_get(unit,
                                                &required_scache_size));
    if (required_scache_size <= 0) {
        return rv;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IPMC, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
            0, &ipmc_scache_ptr,
            BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_E_NOT_FOUND == rv) {
        /* level 1 warmboot */
        rv = BCM_E_NONE;
        ipmc_scache_ptr = NULL;
    } else if (BCM_FAILURE(rv)) {
        return rv;
    } else {
#ifdef BCM_TRIDENT2_SUPPORT
        if (soc_feature(unit, soc_feature_pim_bidir)) {
            /* Recovery of PIM-BIDIR info from scache needs to be done
             * after initialization of PIM-BIDIR data structures, which
             * is performed in _bcm_tr_ipmc_reinit.
             */
            BCM_IF_ERROR_RETURN
                (bcm_td2_ipmc_pim_bidir_scache_recover(unit,
                                                       &ipmc_scache_ptr));
        }
#endif /* BCM_TRIDENT2_SUPPORT */

        additional_scache_size = 0;
        if (recovered_ver >= BCM_WB_VERSION_1_1) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    (_bcm_th_ipmc_repl_l3_intf_scache_recover(unit,
                                                              &ipmc_scache_ptr));
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_ipmc_repl_l3_intf_scache_recover(unit,
                                                                  &ipmc_scache_ptr));
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
                uint32 temp_size;
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_th_ipmc_repl_l3_intf_scache_size_get(unit,
                                                                   &temp_size));
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN
                        (bcm_tr3_ipmc_repl_l3_intf_scache_size_get(unit,
                                                                   &temp_size));
                }
                additional_scache_size += temp_size;
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        }

#if defined(BCM_TRIDENT2PLUS_SUPPORT) || defined(BCM_TOMAHAWK_SUPPORT)
        if (recovered_ver >= BCM_WB_VERSION_1_2) {
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
#if defined(BCM_TOMAHAWK_SUPPORT)
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_th_ipmc_aggid_info_recover(
                            unit, &ipmc_scache_ptr));
                }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_td2p_ipmc_aggid_info_recover(
                            unit, &ipmc_scache_ptr));
                }
#endif
            }
        } else {
            if (BCM_MC_PER_TRUNK_REPL_MODE(unit)) {
#if defined(BCM_TOMAHAWK_SUPPORT)
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_th_ipmc_aggid_info_scache_size_get(
                            unit, &aggid_info_size));
                }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
                if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
                    BCM_IF_ERROR_RETURN(
                        bcm_td2p_ipmc_aggid_info_scache_size_get(
                            unit, &aggid_info_size));
                }
#endif
                additional_scache_size += aggid_info_size;
            }
        }
#endif /* BCM_TRIDENT2PLUS_SUPPORT || BCM_TOMAHAWK_SUPPORT */

        if (recovered_ver >= BCM_WB_VERSION_1_3) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_th_ipmc_repl_l3_intf_nh_map_scache_recover(
                            unit, &ipmc_scache_ptr));
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN(
                        bcm_tr3_ipmc_repl_l3_intf_nh_map_scache_recover(
                            unit, &ipmc_scache_ptr));
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
                uint32 temp_size;
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_th_ipmc_repl_l3_intf_nh_map_scache_size_get(
                            unit, &temp_size));
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN(
                        bcm_tr3_ipmc_repl_l3_intf_nh_map_scache_size_get(
                            unit, &temp_size));
                }
                additional_scache_size += temp_size;
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        }

        if (recovered_ver >= BCM_WB_VERSION_1_4) {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_recover(
                            unit, &ipmc_scache_ptr));
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN(
                        bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_scache_recover(
                            unit, &ipmc_scache_ptr));
                }
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        } else {
#ifdef BCM_TRIUMPH3_SUPPORT
            if (soc_feature(unit, soc_feature_repl_l3_intf_use_next_hop)) {
                uint32 temp_size;
#ifdef BCM_TOMAHAWK_SUPPORT
                if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_th_ipmc_repl_l3_intf_trill_nh_map_scache_size_get(
                            unit, &temp_size));
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
                {
                    BCM_IF_ERROR_RETURN(
                        bcm_tr3_ipmc_repl_l3_intf_trill_nh_map_scache_size_get(
                            unit, &temp_size));
                }
                additional_scache_size += temp_size;
            }
#endif /* BCM_TRIUMPH3_SUPPORT */
        }

        if (recovered_ver >= BCM_WB_VERSION_1_5) {
#if defined(BCM_KATANA_SUPPORT)
            if ((SOC_IS_KATANA(unit)) ||
                (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit))) {
                BCM_IF_ERROR_RETURN(
                    _bcm_kt_ipmc_hw_queue_info_recover(
                        unit, &ipmc_scache_ptr));
            }
#endif /* BCM_KATANA_SUPPORT */
        } else {
#if defined(BCM_KATANA_SUPPORT)
            if ((SOC_IS_KATANA(unit)) ||
                (SOC_IS_KATANA2(unit) && !SOC_IS_SABER2(unit))) {
                BCM_IF_ERROR_RETURN(
                    _bcm_kt_ipmc_hw_queue_info_scache_size_get(
                        unit, &sw_hw_queue_info_size));
            }
            additional_scache_size += sw_hw_queue_info_size;
#endif /* BCM_KATANA_SUPPORT */
        }
#if defined(BCM_GREYHOUND_SUPPORT)
            if (soc_feature(unit, soc_feature_l3mc_use_egress_next_hop)) {
                uint32 temp_size;
                BCM_IF_ERROR_RETURN
                   (_bcm_fb_ipmc_repl_scache_recover(unit, &ipmc_scache_ptr));
                BCM_IF_ERROR_RETURN
                   (_bcm_fb_ipmc_scahce_size_get(unit, &temp_size));
                additional_scache_size += temp_size;
            }
#endif /* BCM_GREYHOUND_SUPPORT */


        if (additional_scache_size > 0) {
            SOC_IF_ERROR_RETURN(soc_scache_realloc(unit, scache_handle,
                        additional_scache_size));
        }
    }

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */



/*
 * Function:
 *	    _bcm_esw_ipmc_gport_construct
 * Description:
 *	    Constructs gport from port, modid or trunk given in the data structure
 *	    if bcmSwitchUseGport is set. 
 * Parameters:
 *	    unit - StrataSwitch PCI device unit number (driver internal).
 *      data - (IN/OUT) data structure to use in order to construct a gport
 * Returns:
 *	    BCM_E_XXX
 */

STATIC int 
_bcm_esw_ipmc_gport_construct(int unit, bcm_ipmc_addr_t *data)
{
    int                 isGport, rv;
    bcm_gport_t         gport;
    _bcm_gport_dest_t   dest;
    bcm_module_t        mymodid;
#if defined(BCM_KATANA2_SUPPORT)
    bcm_port_t          pp_port;
    int                 min_subport = SOC_INFO(unit).pp_port_index_min;
    int                 max_subport = SOC_INFO(unit).pp_port_index_max;
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int                 is_local_subport = 0;
#endif

    if (NULL == data) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));

    /* if output should not be gport then do nothing*/
    if (!isGport) {
        return BCM_E_NONE;
    }

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching)))  {

        if(_bcm_xgs5_subport_coe_mod_port_local(unit, data->mod_id,
                                                data->port_tgid)) {
            is_local_subport = 1;

            /* Encode parameter as GPORT_SUBPORT type and fill in the
               associated (mod,port) */
            _BCM_SUBPORT_COE_PORT_ID_SET(gport, data->mod_id, data->port_tgid);

            /* Set the bit on the outgoing param indicating this is subtag type */
            _BCM_SUBPORT_COE_PORT_TYPE_SET(gport, _BCM_SUBPORT_COE_TYPE_SUBTAG);
        }
    }
#endif
#if defined(BCM_KATANA2_SUPPORT)
    if (soc_feature(unit, soc_feature_linkphy_coe) ||
        soc_feature(unit, soc_feature_subtag_coe)) {
        /* For LinkPHY/SubportPktTag pp_ports, check if the mod is valid */
        if ((_bcm_kt2_mod_is_coe_mod_check(unit, data->mod_id) == BCM_E_NONE)
            && !(data->ts)) {
            BCM_IF_ERROR_RETURN(_bcm_kt2_modport_to_pp_port_get(unit,
                data->mod_id, data->port_tgid, &pp_port));
            /*For KT2, pp_port >=42 are used for LinkPHY/SubportPktTag subport.
            * Get the subport info associated with the pp_port and form the
            * subport_gport */
#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
                    is_local_subport = 1;
                    /* construct LinkPHY/SubportPkttag subport gport */
                    _BCM_KT2_SUBPORT_PORT_ID_SET(gport, pp_port);
                    if (BCM_PBMP_MEMBER(
                                SOC_INFO(unit).linkphy_pp_port_pbm, pp_port)) {
                        _BCM_KT2_SUBPORT_PORT_TYPE_SET(gport,
                                _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                    } else if (BCM_PBMP_MEMBER(
                                SOC_INFO(unit).subtag_pp_port_pbm, pp_port)) {
                        _BCM_KT2_SUBPORT_PORT_TYPE_SET(gport,
                                _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                    } else {
                        return BCM_E_PORT;
                    }
                }
            } else
#endif
            {            
                if ((pp_port >= min_subport) &&
                    (pp_port <= max_subport)) {
                    is_local_subport = 1;
                    /* construct LinkPHY/SubportPkttag subport gport */
                    _BCM_KT2_SUBPORT_PORT_ID_SET(gport, pp_port);
                    if (BCM_PBMP_MEMBER(
                                SOC_INFO(unit).linkphy_pp_port_pbm, pp_port)) {
                        _BCM_KT2_SUBPORT_PORT_TYPE_SET(gport,
                                _BCM_KT2_SUBPORT_TYPE_LINKPHY);
                    } else if (BCM_PBMP_MEMBER(
                                SOC_INFO(unit).subtag_pp_port_pbm, pp_port)) {
                        _BCM_KT2_SUBPORT_PORT_TYPE_SET(gport,
                                _BCM_KT2_SUBPORT_TYPE_SUBTAG);
                    } else {
                        return BCM_E_PORT;
                    }
                }
            }
        }
    }
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    if (is_local_subport) {
        /* gport assigned above */
    } else
#endif
    {
        _bcm_gport_dest_t_init(&dest);
    
        if (data->ts) {
            dest.tgid = data->port_tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
        } else {
            /* Stacking ports should be encoded as devport */
            if (IS_ST_PORT(unit, data->port_tgid)) {
                rv = bcm_esw_stk_my_modid_get(unit, &mymodid);
                if (BCM_E_UNAVAIL == rv) {
                    dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
                } else {
                    dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                    dest.modid = data->mod_id;
                }
            } else {
                dest.modid = data->mod_id;
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            }
            dest.port = data->port_tgid;
        }
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_construct(unit, &dest, &gport));
    }
    data->port_tgid = gport;
    return BCM_E_NONE;
}


/*
 * Function:
 *	    _bcm_esw_ipmc_gport_resolve
 * Description:
 *	    Resolve port, modid or trunk from given gport. 
 * Parameters:
 *	    unit - StrataSwitch PCI device unit number (driver internal).
 *      gport - given gport
 *      port  - (Out) physical port encoded in gport
 *      modid - (Out) module id encoded in gport 
 *      tgid  - (Out) Trunk group ID encoded in gport
 *      tbit  - (Out) Trunk indicator 1 if trunk encoded in gport 0 otherwise
 *      isPortLocal - Indicator if port encoded in gport must be only local physical port
 * Returns:
 *	    BCM_E_XXX
 */

STATIC int 
_bcm_esw_ipmc_gport_resolve(int unit, bcm_gport_t gport, bcm_port_t *port, 
                            bcm_module_t *modid, bcm_trunk_t *tgid, int *tbit,
                            int isPortLocal)
{
    int id;
    bcm_trunk_t     local_tgid;
    bcm_module_t    local_modid;
    bcm_port_t      local_port;
#if defined(BCM_KATANA2_SUPPORT)
    int             pp_port = -1;
    int             my_modid;
    int             min_subport = SOC_INFO(unit).pp_port_index_min;
    int             max_subport = SOC_INFO(unit).pp_port_index_max;
#endif
#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int             is_local_subport = 0;
#endif

    if ((NULL == port) || (NULL == modid) || (NULL == tgid) ||
        (NULL == tbit) ) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, gport,
            &local_modid, &local_port, &local_tgid, &id));


#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, gport)) {
        if(_bcm_xgs5_subport_coe_mod_port_local(unit, local_modid, local_port)) {
            is_local_subport = 1;
        }

        *port = local_port;
        *modid = local_modid;
        *tbit = 0;
        if (isPortLocal) {
            if (is_local_subport == 0) {
                return BCM_E_PARAM;
            }
        }
        return BCM_E_NONE;
    } else
#endif
#if defined(BCM_KATANA2_SUPPORT)
    if (BCM_GPORT_IS_SET(gport) &&
        _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, gport)) {
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        if (_bcm_kt2_mod_is_coe_mod_check(unit, local_modid) == BCM_E_NONE) {
            pp_port = BCM_GPORT_SUBPORT_PORT_GET(gport);
#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
                    is_local_subport = 1;
                }
            } else
#endif
            {
                if ((pp_port >= min_subport) &&
                    (pp_port <= max_subport)) {
                    is_local_subport = 1;
                }
            }
        }
        *port = local_port;
        *modid = local_modid;
        *tbit = 0;
        if (isPortLocal) {
            if (is_local_subport == 0) {
                return BCM_E_PARAM;
            }
        }
        return BCM_E_NONE;
    } else
#endif
    {
        if (-1 != id) {
            return BCM_E_PORT;
        }
    }

    if (BCM_TRUNK_INVALID != local_tgid) {
        *tgid = local_tgid;
        *tbit = 1;
    } else {
        *port = local_port;
        *modid = local_modid;
        *tbit = 0;
        if (isPortLocal) {
            int    ismymodid;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, local_modid, &ismymodid));
            if (ismymodid != TRUE) {
                return BCM_E_PARAM;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_esw_multicast_ipmc_group_type_get
 * Description:
 *      Get multicast group type associated with given IPMC group
 *
 * Parameters:
 *      unit                - (IN) unit number
 *      ipmc_id             - (IN) IPMC entry id
 *      group               - (IN) group type
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_esw_multicast_ipmc_group_type_get(
            int unit,
            uint32 ipmc_id,
            bcm_multicast_t *group)
{
    int rv = BCM_E_NOT_FOUND;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if(soc_feature(unit, soc_feature_ipmc_l2_use_vlan_vpn) &&
            soc_property_get(unit, spn_IPMC_L2_USE_VLAN_VPN, 0)){
        rv = _bcm_tr_multicast_ipmc_group_type_get(unit, ipmc_id, group);
    }
#endif
    if (rv == BCM_E_NOT_FOUND)
    {
        /* default to _BCM_MULTICAST_TYPE_L3 */
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3, ipmc_id);
        return BCM_E_NONE;
    } else {
        return rv;
    }
}

/*
 * Function:
 *      _bcm_esw_ipmc_convert_mcindex_m2h
 * Purpose:
 *      Convert a bcm_multicast_t ipmc index to a hardware ipmc index.
 * Parameters:
 *	mcindex - (IN/OUT) Pointer to a multicast index 
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_PARAM   If bcm_multicast_t is not L3 type 
 */

STATIC int
_bcm_esw_ipmc_convert_mcindex_m2h(int *mcindex)
{
    /* If the input parameter mcindex is a L3 type bcm_multicast_t, 
     * convert it to a hardware ipmc index.
     */
    if (_BCM_MULTICAST_IS_SET(*mcindex)) {
        if (_BCM_MULTICAST_IS_L3(*mcindex)) {
            *mcindex = _BCM_MULTICAST_ID_GET(*mcindex);
        } else {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_esw_ipmc_convert_mcindex_h2m
 * Purpose:
 *      Convert a hardware ipmc index to a bcm_multicast_t ipmc index,
 *      if the global property _bcm_ipmc_idx_ret_type[unit] is set.
 * Parameters:
 *      unit - unit number.
 *	mcindex - (IN/OUT) Pointer to a multicast index 
 * Returns:
 *      None.
 */

STATIC void
_bcm_esw_ipmc_convert_mcindex_h2m(int unit, int *mcindex)
{
    if (_bcm_ipmc_idx_ret_type[unit]) {
        /* Convert hardware ipmc index to a L3 type bcm_multicast_t */
        _BCM_MULTICAST_GROUP_SET(*mcindex, _BCM_MULTICAST_TYPE_L3, *mcindex);
    }
}

/*
 * Function:
 *	bcm_esw_ipmc_init
 * Description:
 *	Initialize the IPMC module and enable IPMC support. This
 *	function will enable the source port checking and the
 *	search rule to include source IP checking.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	This function has to be called before any other IPMC functions.
 */

int
bcm_esw_ipmc_init(int unit)
{
    int         rv;

#ifdef BCM_WARM_BOOT_SUPPORT
    soc_scache_handle_t scache_handle;
    uint32              required_scache_size;
    uint8               *ipmc_scache_ptr;
#endif /* BCM_WARM_BOOT_SUPPORT */

    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        return BCM_E_UNAVAIL;
    }

    LOG_INFO(BSL_LS_BCM_IPMC,
             (BSL_META_U(unit,
                         "IPMC %d: Init\n"),
              unit));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        return (_bcm_esw_ipmc_reinit(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
    rv = mbcm_driver[unit]->mbcm_ipmc_init(unit);

    if (rv >= 0 && soc_feature(unit, soc_feature_ip_mcast_repl)) {
        rv = mbcm_driver[unit]->mbcm_ipmc_repl_init(unit);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (rv >= 0 && !SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_ipmc_required_scache_size_get(unit, &required_scache_size));
        if (required_scache_size > 0) {
            SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_IPMC, 0);
            rv = _bcm_esw_scache_ptr_get(unit, scache_handle,
                    TRUE,
                    required_scache_size, &ipmc_scache_ptr,
                    BCM_WB_DEFAULT_VERSION, NULL);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                return rv;
            }
            rv = BCM_E_NONE;
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /*
     * Note: for Lynx, egress_port_init requires ipmc_repl_init to have
     * been done already.
     */

    if (rv >= 0) {
        rv = bcm_esw_ipmc_egress_port_init(unit);
    }

    if (rv >= 0) {
        _bcm_ipmc_init[unit] = TRUE;
        _bcm_ipmc_idx_ret_type[unit] = 0;
        _bcm_ipmc_repl_threshold[unit] = 0;
#ifdef BCM_WARM_BOOT_SUPPORT
        rv = _bcm_esw_ipmc_repl_wb_threshold_set(unit, 0);
        if (BCM_E_UNAVAIL == rv) {
            rv = BCM_E_NONE;
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }
#endif /* BCM_WARM_BOOT_SUPPORT */
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)
    if (SOC_IS_XGS3_SWITCH(unit) && !(SOC_IS_KATANAX(unit))) {
        int ipmc_id = 0;
        /* reserve L3_IPMC entry 0 for default behavior */
        /* for Katana and Katana2 0 and 1 get reserved from inside 
         * bcm_tr2_ipmc_repl_init() and
         * bcm_kt2_ipmc_repl_init
         */ 
        BCM_IF_ERROR_RETURN(bcm_xgs3_ipmc_create(unit, &ipmc_id));
    }
#endif

    return rv;
}

/*
 * Function:
 *	bcm_esw_ipmc_detach
 * Description:
 *	Delete all entries of the IPMC table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_detach(int unit)
{
    int         rv = BCM_E_NONE;

    if (!_bcm_ipmc_init[unit]) {
        return (rv);
    }

    LOG_INFO(BSL_LS_BCM_IPMC,
             (BSL_META_U(unit,
                         "IPMC %d: Detach\n"),
              unit));
    if (soc_feature(unit, soc_feature_ip_mcast_repl)) {
        rv = mbcm_driver[unit]->mbcm_ipmc_repl_detach(unit);
    }

    if (rv >= 0) {
        rv = mbcm_driver[unit]->mbcm_ipmc_detach(unit);
    }

    if (rv >= 0) {
	_bcm_ipmc_init[unit] = FALSE;
    }

    return rv;
}

/*
 * Function:
 *	bcm_esw_ipmc_enable
 * Description:
 *	Enable/disable IPMC support.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	enable - 1: enable IPMC support.
 *		0: disable IPMC support.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_enable(int unit, int enable)
{
    IPMC_INIT(unit);

    LOG_INFO(BSL_LS_BCM_IPMC,
             (BSL_META_U(unit,
                         "IPMC %d: %sable\n"),
              unit, enable ? "En" : "Dis"));
    return mbcm_driver[unit]->mbcm_ipmc_enable(unit, enable);
}

/*
 * Function:
 *      _bcm_esw_ipmc_key_validate
 * Description:
 *      Validate ipmc lookup key 
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *	    data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_ipmc_key_validate(int unit, bcm_ipmc_addr_t *data)
{
    bcm_if_t l3iif = 0;
    uint8 l3iif_validate = 0;
    /* Make sure ipmc was initialized on the device. */
    IPMC_INIT(unit);

    /* Input parameters check. */
    if (NULL == data) {
        return (BCM_E_PARAM);
    }

    /* Validate vrf range. */
    if ((data->vrf > SOC_VRF_MAX(unit)) ||
        (data->vrf < BCM_L3_VRF_DEFAULT)) {
        return (BCM_E_PARAM);
    }

    /* IPv6 support check. */ 
    if ((data->flags & BCM_IPMC_IP6) && !SOC_IS_XGS3_SWITCH(unit)) {
        if (!_bcm_proxy_client_enabled(unit)) {
            return (BCM_E_UNAVAIL);
        } else {
            data->flags |= BCM_IPMC_PROXY_IP6;
        }
    }

     if (!(data->flags & BCM_IPMC_L2) && (data->ing_intf != BCM_IF_INVALID)) {
        /* if BCM_IPMC_L2 is not set then validate ing_intf */
           l3iif_validate = 1;
           l3iif = data->ing_intf;
     } else if (!BCM_VLAN_VALID(data->vid) && BCM_VLAN_NONE != data->vid
                                           && !(data->flags & BCM_IPMC_L2)) {
        /* Vlan id range check. Allow BCM_VLAN_NONE for non-vlan matching entries
         * data->vid can also carry L3 incoming interface, which may be
         * greater than 4k.
         */
         l3iif_validate = 1;
         l3iif = data->vid;
     }
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
     else if (soc_feature(unit, soc_feature_ipmc_l2_use_vlan_vpn)
                  && soc_property_get(unit, spn_IPMC_L2_USE_VLAN_VPN, 0)
                  && (data->flags & BCM_IPMC_L2)) {
        /* data->vid carries VFI or VID */
        if (_BCM_VPN_VFI_IS_SET(data->vid)) {
            int vfi;
            _BCM_VPN_GET(vfi, _BCM_VPN_TYPE_VFI, data->vid);
            if (vfi >= soc_mem_index_count(unit, VFIm)) {
                return BCM_E_PARAM;
            }
        } else if (!BCM_VLAN_VALID(data->vid)) {
            return BCM_E_PARAM;
        }
     }
#endif /* BCM_TRIDENT2PLUS_SUPPORT */

     if(l3iif_validate) {
        soc_mem_t l3_entry_mem;
        soc_field_t l3_iif_field;
        if (data->flags & BCM_IPMC_IP6) {
            if (soc_mem_is_valid(unit, L3_ENTRY_4m)) {
                l3_entry_mem = L3_ENTRY_4m;
                l3_iif_field = IPV6MC__L3_IIFf;
            } else if (soc_mem_is_valid(unit, L3_ENTRY_QUADm)) {
                l3_entry_mem = L3_ENTRY_QUADm;
                l3_iif_field = IPV6MC__L3_IIFf;
            } else {
                l3_entry_mem = L3_ENTRY_IPV6_MULTICASTm;
                l3_iif_field = L3_IIFf;
            }
        } else {
            if (soc_mem_is_valid(unit, L3_ENTRY_2m)) {
                l3_entry_mem = L3_ENTRY_2m;
                l3_iif_field = IPV4MC__L3_IIFf;
            } else if (soc_mem_is_valid(unit, L3_ENTRY_DOUBLEm)) {
                l3_entry_mem = L3_ENTRY_DOUBLEm;
                l3_iif_field = IPV4MC__L3_IIFf;
            } else {
                l3_entry_mem = L3_ENTRY_IPV4_MULTICASTm;
                l3_iif_field = L3_IIFf;
            }
        }
        if (soc_mem_field_valid(unit, l3_entry_mem, l3_iif_field)) {
            /* HR/HR2/GH has no L3_IIFm */
            if (soc_mem_is_valid(unit, L3_IIFm)) {
                if (l3iif >= soc_mem_index_count(unit, L3_IIFm)) {
                    return (BCM_E_PARAM);
                }
            }
        } else {
            return (BCM_E_PARAM);
        }
    } 

    /* Destination address must be multicast */
    /* Source address must be unicast        */
    if (data->flags & BCM_IPMC_IP6) {
        if (!BCM_IP6_MULTICAST(data->mc_ip6_addr)) {
            return (BCM_E_PARAM);
        } 
        if (BCM_IP6_MULTICAST(data->s_ip6_addr)) {
            return (BCM_E_PARAM);
        } 
    } else {
        if (!BCM_IP4_MULTICAST(data->mc_ip_addr)) {
            return (BCM_E_PARAM);
        } 
        if (BCM_IP4_MULTICAST(data->s_ip_addr)) {
            return (BCM_E_PARAM);
        } 
    }

    /* Expected L3 IIF range check */
    if (data->flags & BCM_IPMC_POST_LOOKUP_RPF_CHECK) {
        /* IIF = 0 disables RPF checking */
        if ((data->l3a_intf <= 0) ||
            (data->l3a_intf >= BCM_XGS3_L3_ING_IF_TBL_SIZE(unit))) {
            return (BCM_E_PARAM);
        }
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *	bcm_esw_ipmc_add
 * Description:
 *	Adds a new entry to the IPMC table.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	data - IPMC entry information.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_add(int unit, bcm_ipmc_addr_t *data)
{
    bcm_gport_t     gport;
    bcm_port_t      local_port;
    int             is_local_mod;
    int             rv;
    bcm_ipmc_addr_t l_data;
    int             check_port_dualmodid_valid;
    soc_mem_t       v4mc_mem;
    soc_field_t     pri_field;

    /* Input parameters validation. */
    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, data));

    /* Copy input to local storage to enable modifying */
    sal_memcpy(&l_data, data, sizeof(bcm_ipmc_addr_t));

    if (!(l_data.flags & BCM_IPMC_SOURCE_PORT_NOCHECK)) {
        /* Validate source port/trunk. */ 
        if (BCM_GPORT_IS_SET(l_data.port_tgid)) {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_ipmc_gport_resolve(unit, l_data.port_tgid,
                                             &(l_data.port_tgid),
                                             &(l_data.mod_id),
                                             &(l_data.port_tgid),
                                             &(l_data.ts), 0)); 

            /* In multiple modid mode, port number should be less than 32.
             * This check has already been done when resolving gport.
             */
            check_port_dualmodid_valid = 0;
        } else {
            check_port_dualmodid_valid = 1;
        } 

        if(l_data.ts) { 
            if (BCM_FAILURE(_bcm_trunk_id_validate(unit, l_data.port_tgid))) {
                return (BCM_E_PARAM);
            }
        } else  {
            /* Check port validity only on local module. */
            BCM_IF_ERROR_RETURN
                (_bcm_esw_modid_is_local(unit, l_data.mod_id, &is_local_mod));
            if (is_local_mod) {
                if (check_port_dualmodid_valid) {
                    PORT_DUALMODID_VALID(unit, l_data.port_tgid);
                }
                BCM_GPORT_MODPORT_SET(gport, l_data.mod_id, l_data.port_tgid);
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, gport, &local_port));
            }
        }
    }

    /* Validate lookup class range. */
    if (soc_feature(unit, soc_feature_extended_address_class)) {
        if ((l_data.lookup_class > SOC_EXT_ADDR_CLASS_MAX(unit)) ||
            (l_data.lookup_class < 0)) {
            return (BCM_E_PARAM);
        }
    } else if ((l_data.lookup_class > SOC_ADDR_CLASS_MAX(unit)) || 
        (l_data.lookup_class < 0)) {
        return (BCM_E_PARAM);
    }


    /* Validate cos range. */ 
    if (l_data.flags & BCM_IPMC_SETPRI) {
        if (SOC_MEM_IS_VALID(unit, L3_ENTRY_DOUBLEm)) {
            v4mc_mem = L3_ENTRY_DOUBLEm;
            pri_field = IPV4MC__PRIf;
        } else
        {
            v4mc_mem = L3_ENTRY_IPV4_MULTICASTm;
            pri_field = PRIf;
        }
        if(SOC_MEM_IS_VALID(unit, v4mc_mem)) {
            if(soc_mem_field_length(unit, v4mc_mem, pri_field) > 3) {
                if (!BCM_RPE_PRIORITY_VALID(l_data.cos)) {
                    return (BCM_E_PARAM);
                }
            }
        } else {
            if (!BCM_PRIORITY_VALID(l_data.cos)) {
                return (BCM_E_PARAM);
            }
        }
    }

    LOG_INFO(BSL_LS_BCM_IPMC,
             (BSL_META_U(unit,
                         "IPMC %d: Add mc 0x%x, sip 0x%x, vid %d. cos %d, %s %d\n"),
              unit,
              l_data.mc_ip_addr,
              l_data.s_ip_addr,
              l_data.vid,
              l_data.cos,
              l_data.ts ? "trunk" : "port",
              l_data.port_tgid));

    if (l_data.group_l2 &&
        (_BCM_MULTICAST_IS_L3(l_data.group_l2) ||
         _BCM_MULTICAST_IS_VPLS(l_data.group_l2) ||
         _BCM_MULTICAST_IS_FLOW(l_data.group_l2))) {
        l_data.group_l2 = _BCM_MULTICAST_ID_GET(l_data.group_l2);
    }

    if (_BCM_MULTICAST_IS_L3(l_data.group) ||
        _BCM_MULTICAST_IS_VPLS(l_data.group) ||
        _BCM_MULTICAST_IS_FLOW(l_data.group)) {
        l_data.group = _BCM_MULTICAST_ID_GET(l_data.group);
    } else {
        return BCM_E_PARAM;
    }

    L3_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_ipmc_add(unit, &l_data);
    L3_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_esw_ipmc_remove
 * Description:
 *      Delete an entry from the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *	data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_ipmc_remove(int unit, bcm_ipmc_addr_t *data)
{
    int rv;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, data));

    L3_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_ipmc_delete(unit, data);
    L3_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      bcm_esw_ipmc_remove_all
 * Description:
 *      Remove all entries of the IPMC table.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_ipmc_remove_all(int unit)
{
    int rv;

    IPMC_INIT(unit);

    LOG_INFO(BSL_LS_BCM_IPMC,
             (BSL_META_U(unit,
                         "IPMC %d: Del all\n"),
              unit));

    L3_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_ipmc_delete_all(unit);
    L3_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *	bcm_esw_ipmc_get_by_index
 * Description:
 *	Get an IPMC entry.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	index - The index number.
 *	data - (OUTPUT) IPMC entry information.
 * Returns:
 *	BCM_E_XXX
 */
int
bcm_esw_ipmc_get_by_index(int unit, int index, bcm_ipmc_addr_t *data)
{
    int rv;
    int flags;

    IPMC_INIT(unit);

    if(NULL == data) {
        return (BCM_E_PARAM);
    } 

    flags = data->flags;
    bcm_ipmc_addr_t_init(data);
    data->flags = flags;

    BCM_IF_ERROR_RETURN(_bcm_esw_ipmc_convert_mcindex_m2h(&index));

    L3_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_ipmc_get(unit, index, data);
    L3_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        _bcm_esw_ipmc_convert_mcindex_h2m(unit, &data->group);
        if (data->group_l2 > 0) {
            _bcm_esw_ipmc_convert_mcindex_h2m(unit, &data->group_l2);
        }
        rv = _bcm_esw_ipmc_gport_construct(unit, data);
    }

    return rv;
}

/*
 * Function:
 *      bcm_esw_ipmc_find
 * Description:
 *      Find an IPMC entry.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      data - IPMC entry information.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_esw_ipmc_find(int unit, bcm_ipmc_addr_t *data)
{
    int index, rv;

    BCM_IF_ERROR_RETURN(_bcm_ipmc_key_validate(unit, data));

    L3_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_ipmc_lookup(unit, &index, data);
    L3_UNLOCK(unit);

    if (BCM_SUCCESS(rv)) {
        BCM_IF_ERROR_RETURN(bcm_esw_multicast_ipmc_group_type_get(unit,
            data->group, &(data->group)));
        if (data->group_l2 > 0) {
            BCM_IF_ERROR_RETURN(bcm_esw_multicast_ipmc_group_type_get(unit,
                data->group_l2, &(data->group_l2)));
        }
        rv = _bcm_esw_ipmc_gport_construct(unit, data);
    }

    return rv;
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_port_init
 * Description:
 *	Reinitialize/clear the egress IP multicast configuration
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_port_init(int unit)
{

    if (soc_feature(unit, soc_feature_ip_mcast)) {
        int        port;
        bcm_pbmp_t e_pbmp;

        BCM_PBMP_CLEAR(e_pbmp);
        BCM_PBMP_ASSIGN(e_pbmp, PBMP_E_ALL(unit));

#if defined(BCM_KATANA2_SUPPORT)
        if (soc_feature(unit, soc_feature_linkphy_coe) ||
            soc_feature(unit, soc_feature_subtag_coe)) {
            _bcm_kt2_subport_pbmp_update(unit, &e_pbmp);
        }
        if (SOC_IS_KATANA2(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_kt2_flexio_pbmp_update (unit, &e_pbmp));
        }
#endif

        if (!soc_feature(unit, soc_feature_no_egr_ipmc_cfg)) {
            PBMP_ITER(e_pbmp, port) {
                BCM_IF_ERROR_RETURN(bcm_esw_ipmc_egress_port_set(unit, port,
                    _soc_mac_all_zeroes, 0, 0, 0));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_port_set
 * Description:
 *	Config the egress IP multicast Configuration Register.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - port to config.
 *	mac - MAC address.
 *	untag - 1: The IP multicast packet is transmitted as untagged packet.
 *		0: The IP multicast packet is transmitted as tagged packet
 *		with VLAN tag vid.
 *	vid  - VLAN ID.
 *	ttl_thresh - Drop IPMC packets if TTL of IP packet <= ttl_thresh.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_port_set(int unit, bcm_port_t port, const bcm_mac_t mac,
                             int untag, bcm_vlan_t vid, int ttl_thresh)
{
    /*
     * Can't check IPMC_INIT here because this function is used as part
     * of the bcm_esw_ipmc_init function (via bcm_ipmc_egress_port_init).
     * Just check that IPMC feature is enabled.
     */
    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        return BCM_E_UNAVAIL;
    }

    if (!BCM_VLAN_VALID(vid) && vid != BCM_VLAN_NONE) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate (unit, port, &port));

    if (!BCM_TTL_VALID(ttl_thresh)) {
        return (BCM_E_PARAM);
    }

    return mbcm_driver[unit]->mbcm_ipmc_egress_port_set(unit, port, mac,
                                                        untag, vid, ttl_thresh);
}

/*
 * Function:
 *	bcm_esw_ipmc_egress_port_get
 * Description:
 *    Get the egress IP multicast Configuration Register.
 * Parameters:
 *    unit - StrataSwitch PCI device unit number (driver internal).
 *    port - port to config.
 *    mac - (OUT) MAC address.
 *    untag - (OUT) 1: The IP multicast packet is transmitted as untagged packet.
 *                  0: The IP multicast packet is transmitted as tagged
 *                     packet with VLAN tag vid.
 *    vid - (OUT) VLAN ID.
 *    ttl_thresh - (OUT) Drop IPMC packets if TTL <= ttl_thresh.
 * Returns:
 *    BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_port_get(int unit, bcm_port_t port, sal_mac_addr_t mac,
                             int *untag, bcm_vlan_t *vid, int *ttl_thresh)
{
    /* Make sure ipmc was initialized on the device. */
    IPMC_INIT(unit);

    if ((NULL == untag) || (NULL == vid) || (NULL == ttl_thresh)) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate (unit, port, &port));

    return mbcm_driver[unit]->mbcm_ipmc_egress_port_get(unit, port, mac,
                                                        untag, vid, ttl_thresh);
}

/*
 * Function:
 *	bcm_esw_ipmc_counters_get
 * Description:
 *	Get IPMC counters of hardware.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *	port - port to get information.
 *	counters - (OUT) IPMC entry information.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_counters_get(int unit, bcm_port_t port,
                                      bcm_ipmc_counters_t *counters)
{
#if defined(BCM_KATANA2_SUPPORT)
    int my_modid = -1, pp_port = -1;
    int max_subport = SOC_INFO(unit).pp_port_index_max; 
    int min_subport = SOC_INFO(unit).pp_port_index_min;
#endif

#if defined(BCM_KATANA2_SUPPORT) || defined(BCM_HGPROXY_COE_SUPPORT)
    int is_local_subport = 0;
    bcm_module_t mod_out = -1;
    bcm_port_t   port_out = -1;
    bcm_trunk_t  trunk_id = -1;
    int          id = -1;
#endif

    IPMC_INIT(unit);

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if ((soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &mod_out, 
                                    &port_out, &trunk_id, &id));
        if(_bcm_xgs5_subport_coe_mod_port_local(unit, mod_out, port_out)) {
            is_local_subport = 1;
			BCM_IF_ERROR_RETURN(_bcmi_coe_subport_physical_port_get(unit,
																	port,
																	&port));
        }

        if (is_local_subport == 0) {
            return BCM_E_PORT;
        }
    } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
    if (BCM_GPORT_IS_SET(port) &&
        _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port)) {
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port, &mod_out, 
                                    &port_out, &trunk_id, &id));
        BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));
        if (_bcm_kt2_mod_is_coe_mod_check(unit, mod_out) == BCM_E_NONE) {
            pp_port = BCM_GPORT_SUBPORT_PORT_GET(port);
#if defined BCM_METROLITE_SUPPORT
            if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                if (_SOC_IS_PP_PORT_LINKPHY_SUBTAG(unit, pp_port)) {
                    is_local_subport = 1;
                    port = pp_port;
                }
            } else
#endif
            {
                if ((pp_port >= min_subport) &&
                    (pp_port <= max_subport)) {
                    is_local_subport = 1;
				    port = pp_port;
                }
            }
        }
        if (is_local_subport == 0) {
            return BCM_E_PORT;
        }
    }
#endif

#if defined(BCM_HGPROXY_COE_SUPPORT)
    if (BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port) &&
        (soc_feature(unit, soc_feature_hgproxy_subtag_coe) ||
        (soc_feature(unit, soc_feature_channelized_switching))) &&
        is_local_subport) {
    } else
#endif

#if defined(BCM_KATANA2_SUPPORT)
    if (BCM_GPORT_IS_SET(port) &&
        _BCM_KT2_GPORT_IS_LINKPHY_OR_SUBTAG_SUBPORT_GPORT(unit, port) &&
        is_local_subport) {
    } else
#endif
    {
        if (BCM_GPORT_IS_SET(port)) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_local_get(unit, port, &port)); 
        } 
    
        if (!SOC_PBMP_PORT_VALID(port) || !IS_E_PORT(unit, port)) {
            return BCM_E_PORT;
        }
    }

    if (NULL == counters) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_stat_get(unit, port, snmpIfHCInMulticastPkts,
                          &counters->rmca));
    BCM_IF_ERROR_RETURN
        (bcm_esw_stat_get(unit, port, snmpIfHCOutMulticastPkts,
                          &counters->tmca));
    BCM_IF_ERROR_RETURN
        (bcm_esw_stat_get(unit, port, snmpBcmIPMCBridgedPckts,
                          &counters->imbp));
    BCM_IF_ERROR_RETURN
        (bcm_esw_stat_get(unit, port, snmpBcmIPMCRoutedPckts,
                          &counters->imrp));
    BCM_IF_ERROR_RETURN
        (bcm_esw_stat_get(unit, port, snmpBcmIPMCInDroppedPckts,
                          &counters->rimdr));
    BCM_IF_ERROR_RETURN
        (bcm_esw_stat_get(unit, port, snmpBcmIPMCOutDroppedPckts,
                          &counters->timdr));

    return BCM_E_NONE;
}

#if defined(BCM_XGS2_FABRIC_SUPPORT)
STATIC int
_bcm5675_ipmc_bitmap_set(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    mem_ipmc_entry_t  entry;
    int             blk;
    int             rv;

    /* Make sure packet never goes back */
    BCM_PBMP_PORT_REMOVE(pbmp, in_port);

    blk = SOC_PORT_BLOCK(unit, in_port);

    sal_memset(&entry, 0, sizeof(entry));

    soc_mem_lock(unit, MEM_IPMCm);

    if ((rv = READ_MEM_IPMCm(unit, blk, ipmc_idx, &entry)) >= 0)
    {
        soc_MEM_IPMCm_field32_set(unit, &entry, IPMCBITMAPf,
                                  SOC_PBMP_WORD_GET(pbmp, 0));
        rv = WRITE_MEM_IPMCm(unit, blk, ipmc_idx, &entry);
    }

    soc_mem_unlock(unit, MEM_IPMCm);

    return rv;
}

STATIC int
_bcm5675_ipmc_bitmap_get(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t *pbmp)
{
    mem_ipmc_entry_t  entry;
    int             blk;

    sal_memset(&entry, 0, sizeof(entry));
    blk = SOC_PORT_BLOCK(unit, in_port);

    SOC_IF_ERROR_RETURN
        (READ_MEM_IPMCm(unit, blk, ipmc_idx, &entry));

    BCM_PBMP_CLEAR(*pbmp);
    SOC_PBMP_WORD_SET(
        *pbmp, 0, soc_MEM_IPMCm_field32_get(unit, &entry, IPMCBITMAPf));

    return BCM_E_NONE;
}

STATIC int
_bcm5675_ipmc_bitmap_del(int unit, int ipmc_idx,
			 bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    mem_ipmc_entry_t  entry;
    uint32          fval;
    int             blk;
    int             rv;

    blk = SOC_PORT_BLOCK(unit, in_port);

    sal_memset(&entry, 0, sizeof(entry));

    soc_mem_lock(unit, MEM_IPMCm);

    if ((rv = READ_MEM_IPMCm(unit, blk, ipmc_idx, &entry)) >= 0)
    {
        soc_MEM_IPMCm_field_get(unit, &entry, IPMCBITMAPf, &fval);
        fval &= ~SOC_PBMP_WORD_GET(pbmp, 0);
        soc_MEM_IPMCm_field32_set(unit, &entry, IPMCBITMAPf, fval);
        rv = WRITE_MEM_IPMCm(unit, blk, ipmc_idx, &entry);

    }
    soc_mem_unlock(unit, MEM_IPMCm);

    return rv;
}
#endif /* BCM_XGS2_FABRIC_SUPPORT */

#ifdef BCM_BRADLEY_SUPPORT
int
_bcm_xgs3_ipmc_bitmap_set(int unit, int ipmc_idx, bcm_pbmp_t pbmp)
{
    int		rv, mc_base, mc_size, mc_index;
    l2mc_entry_t	l2mc;

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
    if (ipmc_idx < 0 || ipmc_idx > mc_size) {
        return BCM_E_PARAM;
    }
    mc_index = ipmc_idx + mc_base;

    soc_mem_lock(unit, L2MCm);
    rv = READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &l2mc);
    if (rv >= 0) {
        soc_mem_pbmp_field_set(unit, L2MCm, &l2mc, PORT_BITMAPf, &pbmp);
        soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
        rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index, &l2mc);
    }
    soc_mem_unlock(unit, L2MCm);

    /* set the l3_ipmc table entry valid but empty */
    if (rv >= 0 && ipmc_idx <= soc_mem_index_max(unit, L3_IPMCm)) {
        ipmc_entry_t	ipmc_e;

        sal_memset(&ipmc_e, 0, sizeof(ipmc_e));
        soc_mem_field32_set(unit, L3_IPMCm, &ipmc_e, VALIDf, 1);
        rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, ipmc_idx, &ipmc_e);
    }
    return rv;
}

int
_bcm_xgs3_ipmc_bitmap_get(int unit, int ipmc_idx, bcm_pbmp_t *pbmp)
{
    int		mc_base, mc_size, mc_index;
    l2mc_entry_t	l2mc;

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
    if (ipmc_idx < 0 || ipmc_idx > mc_size) {
        return BCM_E_PARAM;
    }
    mc_index = ipmc_idx + mc_base;

    SOC_IF_ERROR_RETURN(READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &l2mc));
    soc_mem_pbmp_field_get(unit, L2MCm, &l2mc, PORT_BITMAPf, pbmp);
    return BCM_E_NONE;
}

int
_bcm_xgs3_ipmc_bitmap_del(int unit, int ipmc_idx, bcm_pbmp_t pbmp)
{
    int		rv, mc_base, mc_size, mc_index;
    l2mc_entry_t	l2mc;
    bcm_pbmp_t          tmp_pbmp;

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
    if (ipmc_idx < 0 || ipmc_idx > mc_size) {
        return BCM_E_PARAM;
    }
    mc_index = ipmc_idx + mc_base;

    soc_mem_lock(unit, L2MCm);
    rv = READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &l2mc);
    if (rv >= 0) {
        soc_mem_pbmp_field_get(unit, L2MCm, &l2mc, PORT_BITMAPf, &tmp_pbmp);
        SOC_PBMP_REMOVE(tmp_pbmp, pbmp);
        soc_mem_pbmp_field_set(unit, L2MCm, &l2mc, PORT_BITMAPf, &tmp_pbmp);
        soc_mem_field32_set(unit, L2MCm, &l2mc, VALIDf, 1);
        rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index, &l2mc);
    }
    soc_mem_unlock(unit, L2MCm);

    /* set the l3_ipmc table entry valid but empty */
    if (rv >= 0 && ipmc_idx <= soc_mem_index_max(unit, L3_IPMCm)) {
        ipmc_entry_t	ipmc_e;

        sal_memset(&ipmc_e, 0, sizeof(ipmc_e));
        soc_mem_field32_set(unit, L3_IPMCm, &ipmc_e, VALIDf, 1);
        rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, ipmc_idx, &ipmc_e);
    }
    return rv;
}

int
_bcm_xgs3_ipmc_bitmap_clear(int unit, int ipmc_idx)
{
    int		mc_base, mc_size, mc_index, rv;
    l2mc_entry_t	l2mc;

    SOC_IF_ERROR_RETURN
        (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
    if (ipmc_idx < 0 || ipmc_idx > mc_size) {
        return BCM_E_PARAM;
    }
    mc_index = ipmc_idx + mc_base;
    sal_memset(&l2mc, 0, sizeof(l2mc_entry_t));

    soc_mem_lock(unit, L2MCm);
    rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index, &l2mc);
    soc_mem_unlock(unit, L2MCm);

    return rv;
}

#endif /* BCM_BRADLEY_SUPPORT */

/*
 * Function:
 *	bcm_esw_ipmc_bitmap_max_get
 * Purpose:
 *	Provides maximum IPMC index that this fabric can handle
 * Parameters:
 *	unit - device
 *	max_index - (OUT) returned number of entries
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *	Fabric only
 */

int
bcm_esw_ipmc_bitmap_max_get(int unit, int *max_index)
{
#ifdef	BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
	*max_index = soc_mem_index_count(unit, MEM_IPMCm);
	return BCM_E_NONE;
    }
#endif	/* BCM_HERCULES_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
	int	mc_base, mc_size;

	SOC_IF_ERROR_RETURN
            (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
	*max_index = mc_size;
	return BCM_E_NONE;
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    *max_index = 0;
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_bitmap_set
 * Purpose:
 *	Set the IPMC forwarding port bitmap
 * Parameters:
 *	unit     - SOC unit #
 *	index    - the IPMC index carried in HiGig header
 *	in_port  - ingress port number
 *	pbmp     - IPMC forwarding port bitmap
 * Returns:
 *	BCM_E_XXX
 * Note:
 *	Fabric only
 */

int
bcm_esw_ipmc_bitmap_set(int unit, int index,
			bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, in_port, &in_port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, in_port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_ipmc_convert_mcindex_m2h(&index));

#ifdef	BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        return _bcm5675_ipmc_bitmap_set(unit, index, in_port, pbmp);
    }
#endif	/* BCM_HERCULES_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        COMPILER_REFERENCE(in_port);
        return _bcm_xgs3_ipmc_bitmap_set(unit, index, pbmp);
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_bitmap_get
 * Purpose:
 *	Get the IPMC forwarding port bitmap
 * Parameters:
 *	unit     - SOC unit #
 *	index    - the IPMC index carried in HiGig header
 *	in_port  - ingress port number
 *	pbmp     - IPMC forwarding port bitmap
 * Returns:
 *	BCM_E_XXX
 * Note:
 *	Fabric only
 */

int
bcm_esw_ipmc_bitmap_get(int unit, int index,
			bcm_port_t in_port, bcm_pbmp_t *pbmp)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, in_port, &in_port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, in_port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_ipmc_convert_mcindex_m2h(&index));

#ifdef	BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        return _bcm5675_ipmc_bitmap_get(unit, index, in_port, pbmp);
    }
#endif	/* BCM_HERCULES_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        COMPILER_REFERENCE(in_port);
        return _bcm_xgs3_ipmc_bitmap_get(unit, index, pbmp);
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *	bcm_esw_ipmc_bitmap_del
 * Purpose:
 *	Remove IPMC forwarding port bitmap
 * Parameters:
 *	unit     - SOC unit #
 *	index - the IPMC index carried in HiGig header
 *	in_port  - ingress port number
 *	pbmp     - IPMC forwarding port bitmap
 * Returns:
 *	BCM_E_XXX
 * Note:
 * 	Fabric only
 */

int
bcm_esw_ipmc_bitmap_del(int unit, int index,
			bcm_port_t in_port, bcm_pbmp_t pbmp)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             t;

    if (BCM_GPORT_IS_SET(in_port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_ipmc_gport_resolve(unit, in_port, &in_port, &modid, &tgid, &t, 1)); 
    } 
    if (!SOC_PORT_VALID(unit, in_port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(_bcm_esw_ipmc_convert_mcindex_m2h(&index));

#ifdef	BCM_HERCULES_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        return _bcm5675_ipmc_bitmap_del(unit, index, in_port, pbmp);
    }
#endif	/* BCM_HERCULES_SUPPORT */

#ifdef	BCM_XGS3_FABRIC_SUPPORT
    if (SOC_IS_XGS3_FABRIC(unit)) {
        COMPILER_REFERENCE(in_port);
        return _bcm_xgs3_ipmc_bitmap_del(unit, index, pbmp);
    }
#endif	/* BCM_XGS3_FABRIC_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_age
 * Purpose:
 *      Age out the ipmc entry by clearing the HIT bit when appropriate,
 *      the ipmc entry itself is removed if HIT bit is not set.
 * Parameters:
 *      unit       -  (IN) BCM device number.
 *      flags      -  (IN) The criteria used to age out ipmc table.
 *      age_cb     -  (IN) Call back routine.
 *      user_data  -  (IN) User provided cookie for callback.
 * Returns:
 *      BCM_E_XXX 
 */
int
bcm_esw_ipmc_age(int unit, uint32 flags, bcm_ipmc_traverse_cb age_cb, 
                 void *user_data)
{
    int rv;

    IPMC_INIT(unit);
    
    L3_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_ipmc_age(unit, flags, age_cb, user_data);
    L3_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *      bcm_esw_ipmc_traverse
 * Purpose:
 *      Go through all valid ipmc entries, and call the callback function
 *      at each entry
 * Parameters:
 *      unit      - (IN) BCM device number.
 *      flags     - (IN) The criteria used to age out ipmc table.
 *      cb        - (IN) User supplied callback function.
 *      user_data - (IN) User supplied cookie used in parameter 
 *                       in callback function.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_ipmc_traverse(int unit, uint32 flags,
                      bcm_ipmc_traverse_cb cb, void *user_data)
{
    int rv;

    IPMC_INIT(unit);

    /* Input parameters check. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    L3_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_ipmc_traverse(unit, flags, cb, user_data);
    L3_UNLOCK(unit);
    return rv;
}

/*
 * Function:
 *	_bcm_esw_ipmc_egress_intf_set
 * Purpose:
 *	    Helper function to call per device specific routines
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	mc_index - The index number.
 *	port     - port to list.
 *      if_count - number of interfaces in list.
 *	if_array - (IN) array of bcm_if_t interfaces, size if_count.
 *      is_l3    - Indicates if multicast group type is IPMC.
 *      check_port - indication if the port should be checked 
 * Returns:
 *	BCM_E_XXX
 */

int
_bcm_esw_ipmc_egress_intf_set(int unit, int mc_index, bcm_port_t port,
                              int if_count, bcm_if_t *if_array, int is_l3, 
                              int check_port)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return bcm_th_ipmc_egress_intf_set(unit, mc_index, port, if_count,
                if_array, is_l3, check_port);
    }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit) || SOC_IS_KATANA2(unit)) {
        return bcm_tr3_ipmc_egress_intf_set(unit, mc_index, port, if_count,
                if_array, is_l3, check_port);
    }
#endif
#ifdef	BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_TRIDENT(unit) || SOC_IS_KATANA(unit)) {
        return bcm_tr2_ipmc_egress_intf_set(unit, mc_index, port,
                                            if_count, if_array, check_port);
    }
#endif
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        return bcm_fb_ipmc_egress_intf_set(unit, mc_index, port, if_count, 
                                           if_array, is_l3, check_port);
    }
#endif
#endif  /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;

}

int
_bcm_esw_ipmc_port_trunkid_get(int unit, bcm_port_t port, int *tgid)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            return bcm_th_port_trunkid_get(unit, port, tgid);
        }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
            return bcm_td2p_port_trunkid_get(unit, port, tgid);
        }
#endif

#endif  /* INCLUDE_L3 */
        return BCM_E_UNAVAIL;
}


int
_bcm_esw_ipmc_egress_intf_set_for_trunk_first_member(
    int unit, int mc_index, bcm_port_t port,
    int if_count, bcm_if_t *if_array, int is_l3,
    int check_port, bcm_trunk_t tgid)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            return bcm_th_ipmc_egress_intf_set_for_trunk_first_member(
                       unit, mc_index, port, if_count, if_array,
                       is_l3, check_port, tgid);
        }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
            return bcm_td2p_ipmc_egress_intf_set_for_trunk_first_member(
                       unit, mc_index, port, if_count, if_array,
                       is_l3, check_port, tgid);
        }
#endif

#endif  /* INCLUDE_L3 */
        return BCM_E_UNAVAIL;
}


int
_bcm_esw_ipmc_egress_intf_set_for_same_pipe_member(
    int unit, int mc_index, bcm_port_t port,
    bcm_port_t first_port, bcm_trunk_t tgid)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
            return bcm_th_ipmc_egress_intf_set_for_same_pipe_member(
                       unit, mc_index, port, first_port, tgid);

        }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
            return bcm_td2p_ipmc_egress_intf_set_for_same_pipe_member(
                       unit, mc_index, port, first_port, tgid);
        }
#endif

#endif  /* INCLUDE_L3 */
        return BCM_E_UNAVAIL;
}

int
_bcm_esw_ipmc_egress_intf_add_trunk_member(int unit, int mc_index,
                                           bcm_port_t port)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return bcm_th_ipmc_egress_intf_add_trunk_member(unit, mc_index, port);
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        return bcm_td2p_ipmc_egress_intf_add_trunk_member(unit, mc_index, port);
    }
#endif

#endif  /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

int
_bcm_esw_ipmc_egress_intf_del_trunk_member(int unit, int mc_index,
                                           bcm_port_t port)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return bcm_th_ipmc_egress_intf_del_trunk_member(unit, mc_index, port);
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        return bcm_td2p_ipmc_egress_intf_del_trunk_member(unit, mc_index, port);
    }
#endif

#endif  /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}


int
_bcm_esw_ipmc_egress_intf_add_for_trunk(int unit, int mc_index, bcm_trunk_t tgid,
                                       int encap_id, int is_l3)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return bcm_th_ipmc_egress_intf_add_for_trunk(unit, mc_index, tgid,
                                                       encap_id, is_l3);
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        return bcm_td2p_ipmc_egress_intf_add_for_trunk(unit, mc_index, tgid,
                                                       encap_id, is_l3);
    }
#endif

#endif  /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

int
_bcm_esw_ipmc_egress_intf_del_for_trunk(int unit, int mc_index, bcm_trunk_t tgid,
                                        int if_max, int encap_id, int is_l3)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return bcm_th_ipmc_egress_intf_del_for_trunk(unit, mc_index, tgid,
                                                       if_max, encap_id, is_l3);
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        return bcm_td2p_ipmc_egress_intf_del_for_trunk(unit, mc_index, tgid,
                                                       if_max, encap_id, is_l3);
    }
#endif

#endif  /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

int
_bcm_esw_ipmc_egress_intf_add_in_per_trunk_mode(int unit, int mc_index,
                                       bcm_port_t port,
                                       int encap_id, int is_l3)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return bcm_th_ipmc_egress_intf_add_in_per_trunk_mode(unit, mc_index,
                                                             port, encap_id,
                                                             is_l3);
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        return bcm_td2p_ipmc_egress_intf_add_in_per_trunk_mode(unit, mc_index,
                                                               port, encap_id,
                                                               is_l3);
    }
#endif

#endif  /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

int
_bcm_esw_ipmc_egress_intf_del_in_per_trunk_mode(int unit, int mc_index,
                                       bcm_port_t port, int if_max,
                                       int encap_id, int is_l3)
{
#if defined(INCLUDE_L3)

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return bcm_th_ipmc_egress_intf_del_in_per_trunk_mode(unit, mc_index,
                                                             port, if_max,
                                                             encap_id, is_l3);
    }
#endif
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_APACHE(unit)) {
        return bcm_td2p_ipmc_egress_intf_del_in_per_trunk_mode(unit, mc_index,
                                                               port, if_max,
                                                               encap_id, is_l3);
    }
#endif

#endif  /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *	bcm_esw_ipmc_egress_intf_get
 * Purpose:
 *	Retrieve list of interfaces in port's replication list
 *      for chosen IPMC group.
 * Parameters:
 *	unit     - StrataSwitch PCI device unit number.
 *	mc_index - The index number.
 *	port     - port to list.
 *      if_max   - maximum size of returned interface array.
 *      if_count - (OUT) number of interfaces in list.
 *	if_array - (OUT) array of bcm_if_t interfaces, size if_count.
 * Returns:
 *	BCM_E_XXX
 */

int
bcm_esw_ipmc_egress_intf_get(int unit, int mc_index, bcm_port_t port,
                             int if_max, bcm_if_t *if_array, int *if_count)
{
    bcm_module_t    modid;
    bcm_trunk_t     tgid;
    int             id, t, ismymodid;
    int             port_out;
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    int             port_in = port;
#endif
#if defined(BCM_HGPROXY_COE_SUPPORT)
    int             is_local_subport;
#endif

    IPMC_INIT(unit);
    IPMC_REPL_UNIT(unit);

    if (BCM_GPORT_IS_SET(port)) {
        if (BCM_GPORT_IS_WLAN_PORT(port)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            BCM_IF_ERROR_RETURN(
                _bcm_esw_modid_is_local(unit, modid, &ismymodid));
            if (ismymodid != TRUE) {
                return BCM_E_PORT;
            }
        } else {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_ipmc_gport_resolve(unit, port, &port_out, &modid,
                                            &tgid, &t, 1));
#if defined(BCM_HGPROXY_COE_SUPPORT)
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_is_local_subport(unit, port, modid, port_out,
                                           &is_local_subport, &port_out));
#endif
            port = port_out;
        }
    }

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        port = port_in;
    }
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_esw_ipmc_convert_mcindex_m2h(&mc_index));


#if defined(INCLUDE_L3)
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) || SOC_IS_TRIDENT3X(unit)) {
        return bcm_th_ipmc_egress_intf_get(unit, mc_index, port,
                                            if_max, if_array, if_count);
    }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_TD2_TT2(unit) || SOC_IS_KATANA2(unit))     {
        return bcm_tr3_ipmc_egress_intf_get(unit, mc_index, port,
                                            if_max, if_array, if_count);
    }
#endif
#ifdef	BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit) || SOC_IS_TRIDENT(unit) || SOC_IS_KATANA(unit)) {
        return bcm_tr2_ipmc_egress_intf_get(unit, mc_index, port,
                                            if_max, if_array, if_count);
    }
#endif
#if defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_IS_FBX(unit)) {
        return bcm_fb_ipmc_egress_intf_get(unit, mc_index, port,
                                           if_max, if_array, if_count);
    }
#endif
#endif /* INCLUDE_L3 */
    return BCM_E_UNAVAIL;
}

/****************************************************************
 *
 * Stack port updating of IPMC tables
 */

STATIC void
_xgs_ipmc_bitmaps_get(int unit, ipmc_entry_t *ipmc_entry,
                      bcm_pbmp_t *l2_ports, bcm_pbmp_t *l3_ports)
{
    BCM_PBMP_CLEAR(*l2_ports);
    BCM_PBMP_CLEAR(*l3_ports);

    soc_mem_pbmp_field_get(unit, L3_IPMCm, ipmc_entry, L3_BITMAPf, l3_ports);
    soc_mem_pbmp_field_get(unit, L3_IPMCm, ipmc_entry, L2_BITMAPf, l2_ports);
}

STATIC void
_xgs_ipmc_bitmaps_set(int unit, ipmc_entry_t *ipmc_entry,
                      bcm_pbmp_t l2_ports, bcm_pbmp_t l3_ports)
{
#if defined(BCM_TRIDENT3_SUPPORT)
    /* Set new bitmaps and write back entry */
    if (SOC_MEM_FIELD_VALID(unit, L3_IPMCm, L2_L3f) &&
        BCM_PBMP_NOT_NULL(l3_ports)) {
        uint32 cancun_ver;
        int32 rv = SOC_E_UNAVAIL;

        rv =  soc_cancun_version_get(unit, &cancun_ver);
        if ((SOC_SUCCESS(rv)) &&
                SOC_IS_TRIDENT3(unit) &&
                (cancun_ver < SOC_CANCUN_VERSION_DEF_5_3_2)) {
            soc_mem_field32_set(unit, L3_IPMCm, ipmc_entry, L2_L3f, 1);
        }
    }
#endif

    soc_mem_pbmp_field_set(unit, L3_IPMCm, ipmc_entry, L3_BITMAPf, &l3_ports);
    soc_mem_pbmp_field_set(unit, L3_IPMCm, ipmc_entry, L2_BITMAPf, &l2_ports);
}


STATIC int
_xgs_ipmc_stk_update(int unit, bcm_pbmp_t add_ports,
                     bcm_pbmp_t remove_ports)
{
    int rv = BCM_E_NONE;  /* Ignore if unavailable */

#if defined(BCM_XGS_SUPPORT)
    int i;
    ipmc_entry_t	ipmc_entry;
    bcm_pbmp_t new_l2_ports, old_l2_ports;
    bcm_pbmp_t new_l3_ports, old_l3_ports;
    int changed = 0;

    soc_mem_lock(unit, L3_IPMCm);
    for (i = soc_mem_index_min(unit, L3_IPMCm);
             i <= soc_mem_index_max(unit, L3_IPMCm);
             i++) {
        rv = soc_mem_read(unit, L3_IPMCm, MEM_BLOCK_ANY, i, &ipmc_entry);
        if (rv < 0) {
            break;
        }
        if (soc_L3_IPMCm_field32_get(unit, &ipmc_entry, VALIDf) == 0) {
            continue;
        }

        /* Get bitmaps, update, write back if changed */
        _xgs_ipmc_bitmaps_get(unit, &ipmc_entry,
                              &old_l2_ports, &old_l3_ports);
        SOC_PBMP_ASSIGN(new_l3_ports, old_l3_ports);
        SOC_PBMP_ASSIGN(new_l2_ports, old_l2_ports);

        BCM_PBMP_OR(new_l2_ports, add_ports);
        BCM_PBMP_REMOVE(new_l2_ports, remove_ports);
        BCM_PBMP_REMOVE(new_l3_ports, SOC_PBMP_STACK_CURRENT(unit));

        if (BCM_PBMP_NEQ(new_l2_ports, old_l2_ports) ||
                BCM_PBMP_NEQ(new_l3_ports, old_l3_ports)) {
            ++changed;
            _xgs_ipmc_bitmaps_set(unit, &ipmc_entry,
                                  new_l2_ports, new_l3_ports);
            rv = soc_mem_write(unit, L3_IPMCm, MEM_BLOCK_ALL, i, &ipmc_entry);
            if (rv < 0) {
                break;
            }
        }
    }
    soc_mem_unlock(unit, L3_IPMCm);

    if (changed) {
        LOG_VERBOSE(BSL_LS_BCM_IPMC,
                    (BSL_META_U(unit,
                                "IPMC %d: xgs stk update changed %d entries\n"),
                     unit, changed));
    }
#endif

    return rv;
}


/*
 * Function:
 *      _bcm_esw_ipmc_stk_update
 * Purpose:
 *      Update port bitmaps after stack change
 * Parameters:
 *
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */


int
_bcm_esw_ipmc_stk_update(int unit, uint32 flags)
{
    bcm_pbmp_t add_ports, remove_ports;
    int rv = BCM_E_NONE;

    if (soc_feature(unit, soc_feature_ip_mcast)) {
        /* Calculate ports that should and should not be in bitmap */
        SOC_PBMP_ASSIGN(add_ports, SOC_PBMP_STACK_CURRENT(unit));
        SOC_PBMP_REMOVE(add_ports, SOC_PBMP_STACK_INACTIVE(unit));

        /* Remove ports no longer stacking, or explicitly inactive */
        SOC_PBMP_ASSIGN(remove_ports, SOC_PBMP_STACK_PREVIOUS(unit));
        SOC_PBMP_REMOVE(remove_ports, SOC_PBMP_STACK_CURRENT(unit));
        SOC_PBMP_OR(remove_ports, SOC_PBMP_STACK_INACTIVE(unit));

        if (SOC_IS_XGS_SWITCH(unit)) {
            rv = _xgs_ipmc_stk_update(unit, add_ports, remove_ports);
        } else { 
            rv = BCM_E_NONE;  /* Ignore if unavailable */
        }
    }

    return rv;
}

#ifndef BCM_SW_STATE_DUMP_DISABLE
/*
 * Function:
 *     _bcm_ipmc_sw_dump
 * Purpose:
 *     Displays IPMC information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_ipmc_sw_dump(int unit)
{
    LOG_CLI((BSL_META_U(unit,
                        "\nSW Information IPMC - Unit %d\n"), unit));
    LOG_CLI((BSL_META_U(unit,
                        "  Init : %d\n"), _bcm_ipmc_init[unit]));
    LOG_CLI((BSL_META_U(unit,
                        "  Multicast Index Return Type : %d\n"), _bcm_ipmc_idx_ret_type[unit]));
#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        int rv;
        uint8 flags;
        int threshold;

        rv = _bcm_esw_ipmc_repl_wb_flags_get(unit, _BCM_IPMC_WB_FLAGS_ALL,
                                             &flags);
        if (BCM_SUCCESS(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "  IPMC SW flags:")));
            if (flags & _BCM_IPMC_WB_REPL_LIST) {
                LOG_CLI((BSL_META_U(unit,
                                    "  ReplicationSharing")));
            }
            if (flags & _BCM_IPMC_WB_MULTICAST_MODE) {
                LOG_CLI((BSL_META_U(unit,
                                    "  MulticastAPIMode")));
            }
            if (flags & _BCM_IPMC_WB_IPMC_GROUP_TYPE_MULTICAST) {
                LOG_CLI((BSL_META_U(unit,
                                    "  L3McIdxRetType")));
            }
            if (flags & _BCM_IPMC_WB_L2MC_GROUP_TYPE_MULTICAST) {
                LOG_CLI((BSL_META_U(unit,
                                    "  L2McIdxRetType")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "Unable to retrieve unit %d IPMC flags - %d\n"),
                     unit, rv));
        }

        LOG_CLI((BSL_META_U(unit,
                            "  IPMC replication availablity threshold: ")));
        rv = _bcm_esw_ipmc_repl_wb_threshold_get(unit, &threshold);
        if (BCM_SUCCESS(rv)) {
            LOG_CLI((BSL_META_U(unit,
                                "%d percent\n"), threshold));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "0 percent\n")));
        }
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
#if defined(BCM_TRIUMPH_SUPPORT) 
    if (SOC_IS_TR_VL(unit)) {
         _bcm_tr_ipmc_sw_dump(unit); 
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_FBX(unit)) {
        _bcm_xgs3_ipmc_sw_dump(unit);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return;
}
#endif /* BCM_SW_STATE_DUMP_DISABLE */

/*
 * Function:
 *      _bcm_esw_ipmc_idx_ret_type_set
 * Purpose:
 *      Set multicast index return type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      arg - 0 means hardware multicast index, 1 means bcm_multicast_t
 * Returns:
 *      BCM_E_NONE 
 *      BCM_E_UNAVAIL
 */

int
_bcm_esw_ipmc_idx_ret_type_set(int unit, int arg)
{
    if (SOC_IS_XGS3_SWITCH(unit)) {
        IPMC_INIT(unit);
        _bcm_ipmc_idx_ret_type[unit] = arg;

#ifdef BCM_WARM_BOOT_SUPPORT
        /* Record this value in HW for Warm Boot recovery. */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_ipmc_repl_wb_flags_set(unit,
                  (arg ? _BCM_IPMC_WB_IPMC_GROUP_TYPE_MULTICAST : 0),
                         _BCM_IPMC_WB_IPMC_GROUP_TYPE_MULTICAST));
#endif /* BCM_WARM_BOOT_SUPPORT */
        
        return BCM_E_NONE;
    } 

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_idx_ret_type_get
 * Purpose:
 *      Get multicast index return type.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      arg - Pointer to where retrieved value will be written
 * Returns:
 *      BCM_E_NONE 
 *      BCM_E_UNAVAIL
 */

int
_bcm_esw_ipmc_idx_ret_type_get(int unit, int *arg)
{
    if (SOC_IS_XGS3_SWITCH(unit)) {
        IPMC_INIT(unit);
        *arg = _bcm_ipmc_idx_ret_type[unit];
        return BCM_E_NONE;
    }

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_repl_threshold_set
 * Purpose:
 *      Set IPMC replication table available space threshold.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      arg - Available space threshold
 * Returns:
 *      BCM_E_NONE 
 *      BCM_E_UNAVAIL
 */
int
_bcm_esw_ipmc_repl_threshold_set(int unit, int arg)
{
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        IPMC_INIT(unit);
        if ((arg < 0) || (arg > 100)) {
            return BCM_E_PARAM;
        }
        _bcm_ipmc_repl_threshold[unit] = arg;

#ifdef BCM_WARM_BOOT_SUPPORT
        /* Record this value in HW for Warm Boot recovery. */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_ipmc_repl_wb_threshold_set(unit, arg));
#endif /* BCM_WARM_BOOT_SUPPORT */
        
        return BCM_E_NONE;
    } 
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcm_esw_ipmc_repl_threshold_get
 * Purpose:
 *      Get IPMC replication table available space threshold.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      arg - Available space threshold
 * Returns:
 *      BCM_E_NONE 
 *      BCM_E_UNAVAIL
 */
int
_bcm_esw_ipmc_repl_threshold_get(int unit, int *arg)
{
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        IPMC_INIT(unit);
        *arg = _bcm_ipmc_repl_threshold[unit];
        return BCM_E_NONE;
    }
#endif /* BCM_TRIDENT_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_rp_create
 * Purpose:
 *      Create a rendezvous point.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      flags - BCM_IPMC_RP_xxx flags.
 *      rp_id - (IN/OUT) Rendezvous point ID.
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_rp_create(
    int unit, 
    uint32 flags, 
    int *rp_id)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_rp_create(unit, flags, rp_id);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_rp_destroy
 * Purpose:
 *      Destroy a rendezvous point.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      rp_id - Rendezvous point ID.
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_rp_destroy(
    int unit, 
    int rp_id)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_rp_destroy(unit, rp_id);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_rp_set
 * Purpose:
 *      Set active L3 interfaces for a rendezvous point.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      rp_id - Rendezvous point ID
 *      intf_count - Number of elements in intf_array
 *      intf_array - Array of active L3 interfaces
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_rp_set(
    int unit, 
    int rp_id, 
    int intf_count, 
    bcm_if_t *intf_array)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_rp_set(unit, rp_id, intf_count, intf_array);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_rp_get
 * Purpose:
 *      Get active L3 interfaces for a rendezvous point.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      rp_id - Rendezvous point ID
 *      intf_max - Number of elements in intf_array
 *      intf_array - (OUT) Array of active L3 interfaces
 *      intf_count - (OUT) Number of elements returned in intf_array
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_rp_get(
    int unit, 
    int rp_id, 
    int intf_max, 
    bcm_if_t *intf_array, 
    int *intf_count)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_rp_get(unit, rp_id, intf_max, intf_array, intf_count);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_rp_add
 * Purpose:
 *      Add an active L3 interface to a rendezvous point.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      rp_id - Rendezvous point ID
 *      intf_id - L3 interface ID
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_rp_add(
    int unit, 
    int rp_id, 
    bcm_if_t intf_id)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_rp_add(unit, rp_id, intf_id);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_rp_delete
 * Purpose:
 *      Delete an active L3 interface from a rendezvous point.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      rp_id - Rendezvous point ID
 *      intf_id - L3 interface ID
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_rp_delete(
    int unit, 
    int rp_id, 
    bcm_if_t intf_id)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_rp_delete(unit, rp_id, intf_id);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_rp_delete_all
 * Purpose:
 *      Delete all active L3 interfaces from a rendezvous point.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      rp_id - Rendezvous point ID
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_rp_delete_all(
    int unit, 
    int rp_id)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_rp_delete_all(unit, rp_id);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_range_add
 * Purpose:
 *      Add a range of PIM-BIDIR IPMC addresses.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      range_id - (IN/OUT) Range ID
 *      range - IPMC address range info
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_range_add(
    int unit, 
    int *range_id, 
    bcm_ipmc_range_t *range)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_range_add(unit, range_id, range);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_range_delete
 * Purpose:
 *      Delete a range of PIM-BIDIR IPMC addresses.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      range_id - Range ID
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_range_delete(
    int unit, 
    int range_id)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_range_delete(unit, range_id);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_range_delete_all
 * Purpose:
 *      Delete all ranges of PIM-BIDIR IPMC addresses.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      range_id - Range ID
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_range_delete_all(
    int unit)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_range_delete_all(unit);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

   return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_range_get
 * Purpose:
 *      Get a range of PIM-BIDIR IPMC addresses.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      range_id - Range ID
 *      range - (OUT) Range info
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_range_get(
    int unit, 
    int range_id, 
    bcm_ipmc_range_t *range)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_range_get(unit, range_id, range);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_range_size_get
 * Purpose:
 *      Get the number of PIM-BIDIR IPMC address ranges supported by the device.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      size - (OUT) Number of ranges 
 * Returns:
 *      BCM_E_xxx
 */
int 
bcm_esw_ipmc_range_size_get(
    int unit, 
    int *size)
{
#ifdef BCM_TRIDENT2_SUPPORT
    if (soc_feature(unit, soc_feature_pim_bidir)) {
        int rv;

        L3_LOCK(unit);
        rv = bcm_td2_ipmc_range_size_get(unit, size);
        L3_UNLOCK(unit);
        return rv;
    }
#endif /* BCM_TRIDENT2_SUPPORT */

    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_attach
 * Description:
 *      Attach counters entries to the given IPMC group
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) IPMC entry information  
 *      stat_counter_id  - (IN) Stat Counter ID.
 *
 * Return Value:
 *      BCM_E_XXX
 */
bcm_error_t bcm_esw_ipmc_stat_attach(
            int unit,
            bcm_ipmc_addr_t *info,
            uint32 stat_counter_id)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        return _bcm_td2_ipmc_stat_attach(unit, info, stat_counter_id);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_detach
 * Description:
 *      Detach counters entries to the given IPMC group
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) IPMC entry information
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_esw_ipmc_stat_detach(
            int unit,
            bcm_ipmc_addr_t *info)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_ipmc_stat_detach(unit, info);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      _bcm_esw_ipmc_stat_counter_get
 * Description:
 *      Get counter statistic values for specific IPMC group
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      sync_mode        - (IN) if 1 sync sw count value with hw count
 *                              before retieving count else get
 *                              sw accumulated count
 *      info             - (IN) IPMC entry information
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t
_bcm_esw_ipmc_stat_counter_get(int              unit,
                               int              sync_mode,
                               bcm_ipmc_addr_t  *info,
                               bcm_ipmc_stat_t  stat,
                               uint32           num_entries,
                               uint32           *counter_indexes,
                               bcm_stat_value_t *counter_values)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) { 
        return _bcm_td2_ipmc_stat_counter_get(unit, sync_mode, info, stat, 
                    num_entries, counter_indexes, counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_counter_get
 * Description:
 *      Get sw accumulated counter statistic values for i
 *      specific IPMC group
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) IPMC entry information
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t
bcm_esw_ipmc_stat_counter_get(int              unit,
                              bcm_ipmc_addr_t  *info,
                              bcm_ipmc_stat_t  stat,
                              uint32           num_entries,
                              uint32           *counter_indexes,
                              bcm_stat_value_t *counter_values)
{
        return _bcm_esw_ipmc_stat_counter_get(unit, 0, info, stat, 
                    num_entries, counter_indexes, counter_values);
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_counter_sync_get
 * Description:
 *      Get counter statistic values for specific IPMC group
 *      but sync the software accumulated count with hw count 
 *      
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) IPMC entry information
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t
bcm_esw_ipmc_stat_counter_sync_get(int              unit,
                                   bcm_ipmc_addr_t  *info,
                                   bcm_ipmc_stat_t  stat,
                                   uint32           num_entries,
                                   uint32           *counter_indexes,
                                   bcm_stat_value_t *counter_values)
{
        return _bcm_esw_ipmc_stat_counter_get(unit, 1, info, stat, 
                    num_entries, counter_indexes, counter_values);
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_counter_set
 * Description:
 *      Set counter statistic values for specific IPMC group
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) IPMC entry information
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (IN) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_esw_ipmc_stat_counter_set(
            int unit,
            bcm_ipmc_addr_t *info,
            bcm_ipmc_stat_t stat,
            uint32 num_entries,
            uint32 *counter_indexes,
            bcm_stat_value_t *counter_values)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {
        return _bcm_td2_ipmc_stat_counter_set(unit, info, stat,
                    num_entries, counter_indexes, counter_values);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_multi_get
 *
 * Description:
 *  Get Multiple IPMC entry counter value for specified IPMC entry index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) IPMC entry information
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

    
 bcm_error_t  bcm_esw_ipmc_stat_multi_get(
                            int                 unit, 
                            bcm_ipmc_addr_t	    *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t *stat_arr,
                            uint64              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_ipmc_stat_multi_get (
                    unit, info, nstat, stat_arr, value_arr);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_multi_get32
 *
 * Description:
 *  Get 32bit IPMC entry counter value for specified IPMC entry index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info		 	 - (IN) IPMC entry information
 *      nstat            - (IN) Number of elements in stat array
 *      stat_arr         - (IN) Collected statistics descriptors array
 *      value_arr        - (OUT) Collected counters values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
 bcm_error_t  bcm_esw_ipmc_stat_multi_get32(
                            int                 unit, 
                            bcm_ipmc_addr_t	    *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t *stat_arr,
                            uint32              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_ipmc_stat_multi_get32 (
                     unit, info, nstat, stat_arr, value_arr);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *     bcm_esw_ipmc_stat_multi_set
 *
 * Description:
 *  Set IPMC entry counter value for specified IPMC entry index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) IPMC entry information
 *      stat             - (IN) IPMC entry counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t  bcm_esw_ipmc_stat_multi_set(
                            int                 unit, 
                            bcm_ipmc_addr_t	    *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t *stat_arr,
                            uint64              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_ipmc_stat_multi_set (
                    unit, info, nstat, stat_arr, value_arr);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_multi_set32
 *
 * Description:
 *  Set 32bit IPMC entry counter value for specified IPMC entry index for multiple stat
 *  types
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info			 - (IN) IPMC entry information
 *      stat             - (IN) IPMC entry counter stat types
 *      num_entries      - (IN) Number of counter Entries
 *      counter_indexes  - (IN) Pointer to Counter indexes entries
 *      counter_values   - (OUT) Pointer to counter values
 *
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */

 bcm_error_t  bcm_esw_ipmc_stat_multi_set32(
                            int                 unit, 
                            bcm_ipmc_addr_t	    *info,
                            int                 nstat, 
                            bcm_ipmc_stat_t *stat_arr,
                            uint32              *value_arr)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) {    
        return _bcm_td2_ipmc_stat_multi_set32 (
                    unit, info, nstat, stat_arr, value_arr);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

/*
 * Function:
 *      bcm_esw_ipmc_stat_id_get
 * Description:
 *      Get stat counter id associated with given IPMC group
 *
 * Parameters:
 *      unit             - (IN) unit number
 *      info             - (IN) IPMC entry information
 *      stat             - (IN) Type of the counter to retrieve
 *                              I.e. ingress/egress byte/packet)
 *      Stat_counter_id  - (OUT) Stat Counter ID
 * Return Value:
 *      BCM_E_XXX
 * Notes:
 */
bcm_error_t bcm_esw_ipmc_stat_id_get(
            int unit,
            bcm_ipmc_addr_t *info,
            bcm_ipmc_stat_t stat,
            uint32 *stat_counter_id)
{
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2_TT2(unit)) { 
        return _bcm_td2_ipmc_stat_id_get(unit, info, stat, stat_counter_id);
    } else
#endif
    {
        return BCM_E_UNAVAIL;
    }
}

#else /* INCLUDE_L3 */
typedef int _bcm_esw_ipmc_not_empty; /* Make ISO compilers happy. */
#endif	/* INCLUDE_L3 */
