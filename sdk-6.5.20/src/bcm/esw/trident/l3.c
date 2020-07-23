/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l3.c
 * Purpose:     Trident L3 function implementations
 */


#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIDENT_SUPPORT)  && defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>

#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/trident.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <bcm_int/esw/trident2plus.h>
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>

#include <bcm_int/esw_dispatch.h>

/* L3 ECMP DMA Threshold */
#define L3_ECMP_MEM_DMA_THRESHOLD    16

extern int ecmp_mode_hierarchical;
extern int ecmp_mode_single;

/*
 * Function:
 *      _bcm_td_l3_ecmp_grp_get
 * Purpose:
 *      Get ecmp group next hop members by index.
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      ecmp_grp - (IN)Ecmp group id to read. 
 *      ecmp_count - (IN)Maximum number of entries to read.
 *      nh_idx     - (OUT)Next hop indexes. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td_l3_ecmp_grp_get (int unit, int ecmp_grp, int ecmp_group_size, int *nh_idx)
{
    int idx; /* Iteration index. */
    int max_ent_count=0; /* Number of entries to read.*/
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to read hw entry. */
    int one_entry_grp = TRUE; /* Single next hop entry group.  */ 
    int rv = BCM_E_UNAVAIL; /* Operation return status.  */
    int ecmp_idx = 0;
    uint32 dest_type = SOC_MEM_FIF_DEST_INVALID;

    /* Input parameters sanity check. */
    if ((NULL == nh_idx) || (ecmp_group_size < 1)) {
         return (BCM_E_PARAM);
    }


    /* Zero all next hop indexes first. */
    sal_memset(nh_idx, 0, ecmp_group_size * sizeof(int));
    sal_memset(hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Calculate Base_ptr table index. */
    if (SOC_MEM_IS_VALID(unit, L3_ECMP_COUNTm)) {
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_l3_ecmp_grp_info_get(unit, ecmp_grp,
                                           &max_ent_count, &ecmp_idx));

    }

    /* Read all the indexes from hw. */
    for (idx = 0; idx < max_ent_count; idx++) {

         /* Read next hop index. */
         rv = soc_mem_read(unit, L3_ECMPm, MEM_BLOCK_ANY,
                           (ecmp_idx + idx), hw_buf);
         if (rv < 0) {
             break;
         }
         if (SOC_IS_TRIDENT3X(unit)) {
             nh_idx[idx] = soc_mem_field32_dest_get(unit, L3_ECMPm, hw_buf,
                                          DESTINATIONf, &dest_type);
         } else {
             nh_idx[idx] = soc_mem_field32_get(unit, L3_ECMPm,
                                          hw_buf, NEXT_HOP_INDEXf);
         }
#ifdef BCM_TOMAHAWK_SUPPORT
         /* Calculate the multipath intf index if ecmp member is a group id */
         if (soc_feature(unit, soc_feature_hierarchical_ecmp)) {
             /* Read ecmp flag and check if NH index is a group id */
             if (SOC_IS_TRIDENT3X(unit)) {
                 if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                     nh_idx[idx] += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                 }
             } else if (soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) {
                 if (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
                                         ecmp_mode_hierarchical) {
                     if ((nh_idx[idx]) &&
                         (ecmp_idx < (BCM_XGS3_L3_ECMP_TBL_SIZE(unit) / 2))) {
                         nh_idx[idx] += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                     }
                 }
             } else if (soc_mem_field32_get(unit, L3_ECMPm,
                                            hw_buf, ECMP_FLAGf)) {
                 nh_idx[idx] += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
             }
         }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_MULTI_LEVEL_ECMP_SUPPORT
         /* Calculate the multipath intf index if ecmp member is a group id */
         if (soc_feature(unit, soc_feature_multi_level_ecmp)) {
             /* Read ecmp flag and check if NH index is a group id */
#ifdef BCM_TRIDENT3_SUPPORT
             if (SOC_IS_TRIDENT3X(unit)) {
                 if (dest_type == SOC_MEM_FIF_DEST_ECMP) {
                     nh_idx[idx] += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                 }
             } else 
#endif
             if (soc_mem_field32_get(unit, L3_ECMPm,
                                            hw_buf, ECMPf)) {
                 nh_idx[idx] += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
             }
         }
#endif /* BCM_MULTI_LEVEL_ECMP_SUPPORT */


         /* Check if group contains . */ 
         if (idx && (nh_idx[idx] != nh_idx[0])) { 
              one_entry_grp = FALSE;
         }

        if (0 == soc_feature(unit, soc_feature_l3_dynamic_ecmp_group)) {
            /* Next hops popuplated in cycle,stop once you read first entry again */
            if (idx && (FALSE == one_entry_grp) && (nh_idx[idx] == nh_idx[0])) {
                nh_idx[idx] = 0;
                break;
            }
        } else {
             one_entry_grp = FALSE;
        }
    }
    /* Reset rest of the group if only 1 next hop is present. */
    if (one_entry_grp) {
         sal_memset(nh_idx + 1, 0, (ecmp_group_size - 1) * sizeof(int)); 
    }
    return rv;
}


/*
 * Function:
 *      _bcm_td_l3_ecmp_grp_add
 * Purpose:
 *      Add ecmp group next hop members, or reset ecmp group entry.  
 *      NOTE: Function always writes all the entries in ecmp group.
 *            If there is not enough nh indexes - next hops written
 *            in cycle. 
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      ecmp_grp   - (IN)ecmp group id to write.
 *      buf        - (IN)Next hop indexes or NULL for entry reset.
 *      info       - (IN)ECMP additional info
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td_l3_ecmp_grp_add (int unit, int ecmp_grp, void *buf, void *info)
{
    uint32 l3_ecmp[SOC_MAX_MEM_FIELD_WORDS];
    uint32 initial_l3_ecmp[SOC_MAX_MEM_FIELD_WORDS];
    uint32 l3_ecmp_count[SOC_MAX_MEM_FIELD_WORDS];
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS];
    initial_l3_ecmp_group_entry_t initial_l3_ecmp_group_entry;
    int max_grp_size = 0;  /* Maximum ecmp group size.*/
    int ecmpf = 0;
    int ecmp_idx; /* Ecmp table entry index. */
    int *nh_idx; /* Ecmp group nh indexes.  */
    int nh_cycle_idx;
    int idx = 0;  /* Iteration index.  */
    int rv = BCM_E_NONE; /* Operation return value. */
    int entry_type;
    int max_subgrp_cnt, subgrp_cnt, intf_count = 0;
    int l3_ecmp_oif_flds[8] = {  L3_OIF_0f, 
                             L3_OIF_1f, 
                             L3_OIF_2f, 
                             L3_OIF_3f, 
                             L3_OIF_4f, 
                             L3_OIF_5f, 
                             L3_OIF_6f, 
                             L3_OIF_7f }; 
    int l3_ecmp_oif_type_flds[8] = {  L3_OIF_0_TYPEf, 
                             L3_OIF_1_TYPEf, 
                             L3_OIF_2_TYPEf, 
                             L3_OIF_3_TYPEf, 
                             L3_OIF_4_TYPEf, 
                             L3_OIF_5_TYPEf, 
                             L3_OIF_6_TYPEf, 
                             L3_OIF_7_TYPEf };
    ing_l3_next_hop_entry_t ing_nh;
    uint32 reg_val, value;
    _bcm_l3_tbl_op_t data;
    soc_mem_t grp_mem = L3_ECMP_COUNTm;

#ifdef BCM_TOMAHAWK_SUPPORT
    soc_field_t _initial_l3_ecmp_flag = ECMP_FLAGf;
    int defragment_direction = 0;
#endif
    int ecmp_table_incr = 0;
    _bcm_l3_ecmp_group_info_t *ecmp_info = info;
    _bcm_l3_ecmp_group_buffer_t *ecmp_buffer = buf;
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT) 
    int dgm_zero_hit = 0;
#endif /* BCM_TOMAHAWK2_SUPPORT */

    char *l3_ecmp_tbl_ptr; /* Dma L3 ECMP table pointer. */
    uint32 *ecmp_entry_ptr = NULL; /* ECMP entry pointer.*/
    char *initial_l3_ecmp_tbl_ptr = NULL; /* Dma INITIAL L3 ECMP table pointer. */
    uint32 *initial_ecmp_entry_ptr = NULL; /* INITIAL ECMP entry pointer.*/
    void *ecmp_null_entry = NULL, *initial_ecmp_null_entry = NULL;

    /* Input parameters check. */
    if (NULL == ecmp_buffer || NULL == ecmp_info ||
        NULL == ecmp_buffer->group_buffer) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_TRIDENT3_SUPPORT
    if (SOC_IS_TRIDENT3X(unit)) {
       _initial_l3_ecmp_flag = ECMPf;
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
         (ecmp_info->ecmp_flags & BCM_L3_ECMP_OVERLAY)) {
        grp_mem = ECMP_GROUP_HIERARCHICALm;
    }
#endif
    /* Cast input buffer. */
    nh_idx = ecmp_buffer->group_buffer;
    max_grp_size = ecmp_info->max_paths;

    if (BCM_XGS3_L3_ENT_REF_CNT(BCM_XGS3_L3_TBL_PTR(unit,
                                ecmp_grp), ecmp_grp)
#ifdef BCM_TOMAHAWK3_SUPPORT
        || ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
         (ecmp_info->ecmp_flags & BCM_L3_ECMP_OVERLAY) &&
        BCM_XGS3_L3_ENT_REF_CNT(BCM_XGS3_L3_TBL_PTR(unit,
                                hecmp_grp), ecmp_grp))
#endif
      ) {
        /* Group already exists, get base ptr from group table */ 
        BCM_IF_ERROR_RETURN(
            _bcm_xgs3_l3_ecmp_grp_info_get(unit, ecmp_grp, NULL, &ecmp_idx));
    } else {
        /* Get index to the first slot in the ECMP table
         * that can accomodate max_grp_size */
        data.width = ecmp_info->max_paths;
        data.tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp); 
        data.oper_flags = _BCM_L3_SHR_TABLE_TRAVERSE_CONTROL; 
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
        /* 
         * we can look into the ecmp_grp index to know if this 
         * is overlay entry or underlay entry.
         */
        if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
#if defined(BCM_FIREBOLT6_SUPPORT)
            if (soc_feature(unit, soc_feature_fb6_ecmp_group_partitioning) &&
                    BCMI_L3_ECMP_GROUP_OVERLAY_IN_UPPER_RANGE(unit)) {
                if (ecmp_grp >= BCMI_L3_ECMP_OVERLAY_ENTRIES(unit)) {
                    data.oper_flags |= _BCM_L3_SHR_OVERLAY_OBJECT;
                }
            } else
#endif
                if (ecmp_grp < BCMI_L3_ECMP_OVERLAY_ENTRIES(unit)) {

                    data.oper_flags |= _BCM_L3_SHR_OVERLAY_OBJECT;
                }
        }
#endif
        data.entry_index = -1;
        rv = _bcm_xgs3_tbl_free_idx_get(unit, &data);
        if (rv == BCM_E_FULL) {
            /* Defragment ECMP table */
#ifdef BCM_TOMAHAWK_SUPPORT
            /* Pass ECMP level so the defragmentation can happen in the
               right part of the ECMP table */
            if (soc_feature(unit, soc_feature_hierarchical_ecmp) &&
                (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
                                     ecmp_mode_hierarchical)) {
                if (data.tbl_ptr->idx_min > 0) {
                    /* De-fragment Level 2 or Underlay ECMP table */
                    BCM_IF_ERROR_RETURN(bcm_tr2_l3_ecmp_defragment_no_lock(unit, 2));
                    /* Further De-fragment if possible */
                    defragment_direction = 0;
                } else if ((data.tbl_ptr->idx_min == 0) &&
                           (data.tbl_ptr->idx_max <
                            soc_mem_index_max(unit,L3_ECMPm))) {
                    /* De-fragment Level 1 or Overlay ECMP table */
                    BCM_IF_ERROR_RETURN(bcm_tr2_l3_ecmp_defragment_no_lock(unit, 1));
                    /* Further De-fragment if possible */
                    defragment_direction = 1;
                } else {
                    /* 
                     * The current entry can go in anywhere in the ECMP table. 
                     * De-fragment the entire ECMP table
                     */
                    BCM_IF_ERROR_RETURN(bcm_tr2_l3_ecmp_defragment_no_lock(unit, 2));
                    BCM_IF_ERROR_RETURN(bcm_tr2_l3_ecmp_defragment_no_lock(unit, 1));
                    /* Further De-fragment if possible */
                    defragment_direction = 0;
                }
            } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_RIOT_SUPPORT) || defined(BCM_MULTI_LEVEL_ECMP_SUPPORT)
            if (BCMI_L3_ECMP_IS_MULTI_LEVEL(unit)) {
                if (data.oper_flags & _BCM_L3_SHR_OVERLAY_OBJECT) {
                    BCM_IF_ERROR_RETURN(
                        bcm_tr2_l3_ecmp_defragment_no_lock(unit, 2));
                } else {
                    BCM_IF_ERROR_RETURN(
                        bcm_tr2_l3_ecmp_defragment_no_lock(unit, 1));
                }
            } else
#endif
            {
                BCM_IF_ERROR_RETURN(bcm_tr2_l3_ecmp_defragment_no_lock(unit, 0));
            }
            /* Attempt to get free index again */
            rv = _bcm_xgs3_tbl_free_idx_get(unit, &data);
#ifdef BCM_TOMAHAWK_SUPPORT
            if (soc_feature(unit, soc_feature_hierarchical_ecmp) &&
                (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
                     ecmp_mode_hierarchical) && (rv == BCM_E_FULL)) {
                /*
                 * Move the ECMP tables (which have flags 0)
                 * across two layers
                 */
                BCM_IF_ERROR_RETURN(
                    _bcm_th_l3_ecmp_across_layers_move(unit, max_grp_size,
                                                       defragment_direction));
                if (defragment_direction) {
                    /* L1 -> L2: further de-fragment on L1 */
                    BCM_IF_ERROR_RETURN(
                        bcm_tr2_l3_ecmp_defragment_no_lock(unit, 1));
                } else {
                    /* L2 -> L1: further de-fragment on L2*/
                    BCM_IF_ERROR_RETURN(
                        bcm_tr2_l3_ecmp_defragment_no_lock(unit, 2));
                }

                BCM_IF_ERROR_RETURN(_bcm_xgs3_tbl_free_idx_get(unit, &data));
            } else
#endif
            if (BCM_FAILURE(rv)) {
                return rv;
            }
        } else if (BCM_FAILURE(rv)) {
            return rv;
        }

        ecmp_idx = data.entry_index;
        BCM_XGS3_L3_ENT_REF_CNT_INC(data.tbl_ptr, ecmp_idx, max_grp_size);
        ecmp_table_incr = 1;

    }

    sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
    sal_memset (&initial_l3_ecmp_group_entry, 0,
            sizeof(initial_l3_ecmp_group_entry_t));

    if (max_grp_size > L3_ECMP_MEM_DMA_THRESHOLD) {
        rv = bcm_xgs3_l3_tbl_range_dma(unit, L3_ECMPm, sizeof(ecmp_entry_t),
                                       "l3_ecmp_tbl", ecmp_idx,
                                       ecmp_idx + max_grp_size - 1,
                                       &l3_ecmp_tbl_ptr);
        BCM_IF_ERROR_RETURN(rv);
        if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
            rv = bcm_xgs3_l3_tbl_range_dma(unit, INITIAL_L3_ECMPm,
                                           sizeof(initial_l3_ecmp_entry_t),
                                           "initial_l3_ecmp_tbl", ecmp_idx,
                                           ecmp_idx + max_grp_size - 1,
                                           &initial_l3_ecmp_tbl_ptr);
            if (BCM_FAILURE(rv)) {
                soc_cm_sfree(unit, l3_ecmp_tbl_ptr);
                return rv;
            }
            initial_ecmp_null_entry = soc_mem_entry_null(unit, INITIAL_L3_ECMPm);
        }

        ecmp_null_entry = soc_mem_entry_null(unit, L3_ECMPm);
    }

    /* Write all the indexes to hw. */
    for (idx = 0, nh_cycle_idx = 0; idx < max_grp_size; idx++, nh_cycle_idx++) {
         /* Set next hop index. */
         if (max_grp_size > L3_ECMP_MEM_DMA_THRESHOLD) {
             ecmp_entry_ptr =
                 soc_mem_table_idx_to_pointer(unit, L3_ECMPm, uint32 *,
                                              l3_ecmp_tbl_ptr, idx);
             if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                 initial_ecmp_entry_ptr =
                     soc_mem_table_idx_to_pointer(unit, INITIAL_L3_ECMPm, uint32 *,
                                                  initial_l3_ecmp_tbl_ptr, idx);
                     sal_memcpy(initial_ecmp_entry_ptr, initial_ecmp_null_entry,
                                sizeof(initial_l3_ecmp_entry_t));
             }
             sal_memcpy(ecmp_entry_ptr, ecmp_null_entry, sizeof(ecmp_entry_t));
         } else {
             sal_memset (l3_ecmp, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
             if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                 sal_memset (initial_l3_ecmp, 0,
                             SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
                 initial_ecmp_entry_ptr = initial_l3_ecmp;
             }
             ecmp_entry_ptr = l3_ecmp;
         }

        /* If this is the last nhop then program black-hole. */
#ifdef BCM_TOMAHAWK_SUPPORT
        if (!(soc_feature(unit, soc_feature_ecmp_resilient_hash_optimized) &&
              (ecmp_info->ecmp_flags & BCM_L3_ECMP_RH_OPT)))
#endif /* BCM_TOMAHAWK_SUPPORT */
        {
            if (!nh_idx[nh_cycle_idx]) {
                if (!idx) {
                  nh_cycle_idx = 0;
                } else {
#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
                    if (soc_feature(unit, soc_feature_dgm) &&
                        (ecmp_info->ecmp_flags & BCM_L3_ECMP_DGM_OPT)) {
                        /* primay entry exists or one more entry zeroed */
                        if (idx > ecmp_info->alternate_cnt || dgm_zero_hit) {
                            if (dgm_zero_hit) {
                            /* decrease idx to get real count */
                                idx--;
                            }
                            break;
                        } else {
                            /* If there is no primary entry, zero one more entry */
                            dgm_zero_hit = 1;
                        }
                    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
                    {
                        break;
                    }
                }
            }
        }
        ecmpf = 0;
#ifdef BCM_TOMAHAWK_SUPPORT
        /* Set ecmp flag if member is an ecmp group */
        if (soc_feature(unit, soc_feature_hierarchical_ecmp)) {
            if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                                           nh_idx[nh_cycle_idx])) {
                ecmpf = 1;
                nh_idx[nh_cycle_idx] -= BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
                if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                    soc_mem_field32_set(unit, L3_ECMPm, ecmp_entry_ptr,
                                        ECMP_FLAGf, 1);
                    soc_mem_field32_set(unit, INITIAL_L3_ECMPm,
                                        initial_ecmp_entry_ptr, _initial_l3_ecmp_flag, 1);
                }
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */

#ifdef BCM_MULTI_LEVEL_ECMP_SUPPORT
        /* Set ecmp flag if member is an ecmp group */
        if (soc_feature(unit, soc_feature_multi_level_ecmp)) {
            if (BCM_XGS3_L3_MPATH_EGRESS_IDX_VALID(unit,
                                           nh_idx[nh_cycle_idx])) {
                nh_idx[nh_cycle_idx] -= BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
#ifdef BCM_TRIDENT3_SUPPORT
                if (SOC_IS_TRIDENT3X(unit)) {
                    soc_mem_field32_set(unit, L3_ECMPm, ecmp_entry_ptr, ECMP_FLAGf, 1);
                    ecmpf = 1;
                    soc_mem_field32_set(unit, INITIAL_L3_ECMPm,
                                    initial_ecmp_entry_ptr, ECMPf, 1);
                } else 
#endif
                {
                    soc_mem_field32_set(unit, L3_ECMPm, ecmp_entry_ptr, ECMPf, 1);
                    if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                        soc_mem_field32_set(unit, INITIAL_L3_ECMPm,
                                            initial_ecmp_entry_ptr, ECMPf, 1);
                    }
                }
            }
        }
#endif /* BCM_MULTI_LEVEL_ECMP_SUPPORT */


         if (SOC_IS_TRIDENT3X(unit)) {
             soc_mem_field32_dest_set(unit, L3_ECMPm, ecmp_entry_ptr, DESTINATIONf, 
                                      ecmpf ? SOC_MEM_FIF_DEST_ECMP : SOC_MEM_FIF_DEST_NEXTHOP, 
                                      nh_idx[nh_cycle_idx]);
             soc_mem_field32_dest_set(unit, INITIAL_L3_ECMPm, initial_ecmp_entry_ptr, DESTINATIONf, 
                                      ecmpf ? SOC_MEM_FIF_DEST_ECMP : SOC_MEM_FIF_DEST_NEXTHOP, 
                                      nh_idx[nh_cycle_idx]);
         } else {

             soc_mem_field32_set(unit, L3_ECMPm, ecmp_entry_ptr, 
                                      NEXT_HOP_INDEXf, nh_idx[nh_cycle_idx]);
             if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                 soc_mem_field32_set(unit, INITIAL_L3_ECMPm, initial_ecmp_entry_ptr,
                                          NEXT_HOP_INDEXf, nh_idx[nh_cycle_idx]);
             }
         }

         /* Write buffer to hw L3_ECMPm table. */
         if (max_grp_size <= L3_ECMP_MEM_DMA_THRESHOLD) {
             rv = soc_mem_write(unit, L3_ECMPm, MEM_BLOCK_ALL,
                                    (ecmp_idx + idx), ecmp_entry_ptr);

             if (BCM_FAILURE(rv)) {
                  break;
             }

             if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                  /* Write buffer to hw INITIAL_L3_ECMPm table. */
                  rv = soc_mem_write(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL,
                                           (ecmp_idx + idx), initial_ecmp_entry_ptr);

                  if (BCM_FAILURE(rv)) {
                       break;
                  }
             }
         }

         if (soc_feature(unit, soc_feature_urpf)) {
              /* Check if URPF is enabled on device */
              rv = READ_L3_DEFIP_RPF_CONTROLr(unit, &reg_val);
              if (BCM_FAILURE(rv)) {
                  if (max_grp_size > L3_ECMP_MEM_DMA_THRESHOLD) {
                      soc_cm_sfree(unit, l3_ecmp_tbl_ptr);
                      soc_cm_sfree(unit, initial_l3_ecmp_tbl_ptr);
                  }
                  return rv;
              }

              if (reg_val) {
                   if (idx < 8) {
                       rv = soc_mem_read(unit, ING_L3_NEXT_HOPm, MEM_BLOCK_ANY,
                                         nh_idx[idx], &ing_nh);
                       if (BCM_FAILURE(rv)) {
                           if (max_grp_size > L3_ECMP_MEM_DMA_THRESHOLD) {
                               soc_cm_sfree(unit, l3_ecmp_tbl_ptr);
                               soc_cm_sfree(unit, initial_l3_ecmp_tbl_ptr);
                           }
                           return rv;
                       }

                        entry_type = 
                          soc_ING_L3_NEXT_HOPm_field32_get(unit, 
                               &ing_nh, ENTRY_TYPEf);

                        if (entry_type == 0x0) {
                            if (SOC_MEM_FIELD_VALID(unit, ING_L3_NEXT_HOPm,
                                                    L3_OIFf)) {
                                value = soc_ING_L3_NEXT_HOPm_field32_get(
                                            unit, &ing_nh, L3_OIFf);
                            } else {
                                value = soc_ING_L3_NEXT_HOPm_field32_get(
                                            unit, &ing_nh, VLAN_IDf);
                            }
                             soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                                            l3_ecmp_oif_type_flds[idx], entry_type);
                             soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                                            l3_ecmp_oif_flds[idx], value);
                        } else if (entry_type == 0x1) {
                             value  = soc_ING_L3_NEXT_HOPm_field32_get(unit, &ing_nh, L3_OIFf);
                             soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                                            l3_ecmp_oif_type_flds[idx], entry_type);
                             soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, 
                                            l3_ecmp_oif_flds[idx], value);
                        }
                        soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, URPF_COUNTf, idx);
                   }else {
                       /* Inorder to avoid TRAP_TO_CPU, urpf_mode on L3_IIF/PORT must be set 
                           to STRICT_MODE / LOOSE_MODE */
                        soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, ECMP_GT8f , 1);
                   }
              }
         }
    }
    intf_count = idx;
    if (max_grp_size > L3_ECMP_MEM_DMA_THRESHOLD) {
        rv = soc_mem_write_range(unit, L3_ECMPm, MEM_BLOCK_ALL, ecmp_idx,
                                 ecmp_idx + max_grp_size - 1, l3_ecmp_tbl_ptr);
        if (BCM_SUCCESS(rv)) {
            if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                rv = soc_mem_write_range(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL,
                                         ecmp_idx, ecmp_idx + max_grp_size - 1,
                                         initial_l3_ecmp_tbl_ptr);
                soc_cm_sfree(unit, initial_l3_ecmp_tbl_ptr);
            }
        }
        soc_cm_sfree(unit, l3_ecmp_tbl_ptr);
    }

    if (BCM_SUCCESS(rv)) {
        /* mode 0 = 512 ecmp groups */
        if (!BCM_XGS3_L3_MAX_ECMP_MODE(unit)) {
            /* Set Max Group Size. */
            sal_memset (l3_ecmp_count, 0,
                    SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));
            if (soc_feature(unit, soc_feature_l3_ecmp_1k_groups) ||
                    SOC_IS_MONTEREY(unit)) {
                rv = _bcm_xgs3_l3_ecmp_grp_info_set(unit, (ecmp_grp + 1),
                                                    l3_ecmp_count,
                                                    NULL, 0,
                                                    max_grp_size, -1, 0,
                                                    ecmp_info->ecmp_flags);
                if (BCM_FAILURE(rv)) {
                    if (ecmp_table_incr == 1) {
                        BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                                    ecmp_idx, max_grp_size);
                    }
                    return rv;
                }
            } else {
                if (soc_feature(unit, soc_feature_ecmp_1k_paths_4_subgroups) &&
                        (max_grp_size >= 256)) {
                    max_subgrp_cnt = ((max_grp_size + 3) / 4);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, COUNT_0f, (max_subgrp_cnt - 1));
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, COUNT_1f, (max_subgrp_cnt - 1));
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, COUNT_2f, (max_subgrp_cnt - 1));
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, COUNT_3f,
                            ((max_grp_size - (3 * max_subgrp_cnt) - 1)));

                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, BASE_PTR_0f, ecmp_idx);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, BASE_PTR_1f,
                            (ecmp_idx + max_subgrp_cnt));
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, BASE_PTR_2f,
                            (ecmp_idx + (2 * max_subgrp_cnt)));
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, BASE_PTR_3f,
                            (ecmp_idx + (3 * max_subgrp_cnt)));
                } else {
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, COUNT_0f, max_grp_size-1);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, COUNT_1f, max_grp_size-1);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, COUNT_2f, max_grp_size-1);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            l3_ecmp_count, COUNT_3f, max_grp_size-1);
                }
            }
            rv = soc_mem_write(unit, grp_mem, MEM_BLOCK_ALL,
                    (ecmp_grp+1), l3_ecmp_count);

            if (BCM_FAILURE(rv)) {
                if (ecmp_table_incr == 1) {
                    BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                                ecmp_idx, max_grp_size);
                }
                return rv;
            }
        }
        if (soc_feature(unit, soc_feature_l3_ecmp_1k_groups) ||
                SOC_IS_MONTEREY(unit)) {
            rv = _bcm_xgs3_l3_ecmp_grp_info_set(unit, ecmp_grp, hw_buf,
                                            &initial_l3_ecmp_group_entry, !soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp),
                                            idx, ecmp_idx, ecmp_info->alternate_cnt,
                                            ecmp_info->ecmp_flags);
            if (BCM_FAILURE(rv)) {
                if (ecmp_table_incr == 1) {
                    BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp),
                                                ecmp_idx, max_grp_size);
                }
                return rv;
            }
        } else {
            if (!idx) {
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNT_0f, idx );
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNT_1f, idx );
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNT_2f, idx );
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNT_3f, idx );

                if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_l3_ecmp_group_entry, COUNT_0f, idx);
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_l3_ecmp_group_entry, COUNT_1f, idx);
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_l3_ecmp_group_entry, COUNT_2f, idx);
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_l3_ecmp_group_entry, COUNT_3f, idx);
                }
            } else {
                if (soc_feature(unit, soc_feature_ecmp_1k_paths_4_subgroups) &&
                        (intf_count >= 256)) {
                    subgrp_cnt = ((intf_count + 3) / 4);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            hw_buf, COUNT_0f, (subgrp_cnt - 1));
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            hw_buf, COUNT_1f, (subgrp_cnt - 1));
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            hw_buf, COUNT_2f, (subgrp_cnt - 1));
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm,
                            hw_buf, COUNT_3f,
                            (intf_count - (3 * subgrp_cnt) - 1));

                    if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                &initial_l3_ecmp_group_entry, COUNT_0f,
                                (subgrp_cnt - 1));
                        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                &initial_l3_ecmp_group_entry, COUNT_1f,
                                (subgrp_cnt - 1));
                        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                &initial_l3_ecmp_group_entry, COUNT_2f,
                                (subgrp_cnt - 1));
                        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                &initial_l3_ecmp_group_entry, COUNT_3f,
                                (intf_count - (3 * subgrp_cnt) - 1));
                    }
                } else {

                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNT_0f, idx - 1);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNT_1f, idx - 1);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNT_2f, idx - 1);
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf, COUNT_3f, idx - 1);

                    if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                &initial_l3_ecmp_group_entry, COUNT_0f, idx - 1);
                        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                &initial_l3_ecmp_group_entry, COUNT_1f, idx - 1);
                        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                &initial_l3_ecmp_group_entry, COUNT_2f, idx - 1);
                        soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                                &initial_l3_ecmp_group_entry, COUNT_3f, idx - 1);
                    }
                }
            }

            if (soc_feature(unit, soc_feature_ecmp_1k_paths_4_subgroups) &&
                    (intf_count >= 256)) {
                subgrp_cnt = ((intf_count + 3) / 4);
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                        BASE_PTR_0f, ecmp_idx);
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                        BASE_PTR_1f, (ecmp_idx + subgrp_cnt));
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                        BASE_PTR_2f, (ecmp_idx + (2 * subgrp_cnt)));
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                        BASE_PTR_3f, (ecmp_idx + (3 * subgrp_cnt)));

                if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm, &initial_l3_ecmp_group_entry,
                            BASE_PTR_0f, ecmp_idx);
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm, &initial_l3_ecmp_group_entry,
                            BASE_PTR_1f, (ecmp_idx + subgrp_cnt));
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm, &initial_l3_ecmp_group_entry,
                            BASE_PTR_2f, (ecmp_idx + (2 * subgrp_cnt)));
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm, &initial_l3_ecmp_group_entry,
                            BASE_PTR_3f, (ecmp_idx + (3 * subgrp_cnt)));
                }
            } else {
                /* Set group base pointer. */
                if (SOC_MEM_FIELD_VALID(unit, L3_ECMP_COUNTm, BASE_PTR_0f)) {
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                            BASE_PTR_0f, ecmp_idx);
                }
                if (SOC_MEM_FIELD_VALID(unit, L3_ECMP_COUNTm, BASE_PTR_1f)) {
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                            BASE_PTR_1f, ecmp_idx);
                }
                if (SOC_MEM_FIELD_VALID(unit, L3_ECMP_COUNTm, BASE_PTR_2f)) {
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                            BASE_PTR_2f, ecmp_idx);
                }
                if (SOC_MEM_FIELD_VALID(unit, L3_ECMP_COUNTm, BASE_PTR_3f)) {
                    soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                            BASE_PTR_3f, ecmp_idx);
                }

                if (SOC_MEM_FIELD_VALID(unit, INITIAL_L3_ECMP_GROUPm, BASE_PTR_0f)) {
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_l3_ecmp_group_entry, BASE_PTR_0f, ecmp_idx);
                }
                if (SOC_MEM_FIELD_VALID(unit, INITIAL_L3_ECMP_GROUPm, BASE_PTR_1f)) {
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_l3_ecmp_group_entry, BASE_PTR_1f, ecmp_idx);
                }
                if (SOC_MEM_FIELD_VALID(unit, INITIAL_L3_ECMP_GROUPm, BASE_PTR_2f)) {
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_l3_ecmp_group_entry, BASE_PTR_2f, ecmp_idx);
                }
                if (SOC_MEM_FIELD_VALID(unit, INITIAL_L3_ECMP_GROUPm, BASE_PTR_3f)) {
                    soc_mem_field32_set(unit, INITIAL_L3_ECMP_GROUPm,
                            &initial_l3_ecmp_group_entry, BASE_PTR_3f, ecmp_idx);
                }
            }
        }

        if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
            rv = soc_mem_write(unit, INITIAL_L3_ECMP_GROUPm, MEM_BLOCK_ALL,
                    ecmp_grp, &initial_l3_ecmp_group_entry);
            if (BCM_FAILURE(rv) && (ecmp_table_incr == 1)) {
                BCM_XGS3_L3_ENT_REF_CNT_DEC
                    (BCM_XGS3_L3_TBL_PTR(unit, ecmp), ecmp_idx, max_grp_size);
                return rv;
            }
        }

#ifdef BCM_TRIDENT2_SUPPORT
        /* Preserve the fields related to resilient hashing */
        if (soc_feature(unit, soc_feature_ecmp_resilient_hash)) {
            ecmp_count_entry_t old_ecmp_count_entry;
            int enhanced_hashing_enable;
            int rh_flow_set_base;
            int rh_flow_set_size;

            SOC_IF_ERROR_RETURN(READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                        ecmp_grp, &old_ecmp_count_entry));
            if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
                enhanced_hashing_enable = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                        &old_ecmp_count_entry, LB_MODEf);
            } else {
                enhanced_hashing_enable = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                        &old_ecmp_count_entry, ENHANCED_HASHING_ENABLEf);
            }
            rh_flow_set_base = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                    &old_ecmp_count_entry, RH_FLOW_SET_BASEf);
            rh_flow_set_size = soc_mem_field32_get(unit, L3_ECMP_COUNTm,
                    &old_ecmp_count_entry, RH_FLOW_SET_SIZEf);
            if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                        LB_MODEf, enhanced_hashing_enable);
            } else {
                soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                        ENHANCED_HASHING_ENABLEf, enhanced_hashing_enable);
            }
            soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                    RH_FLOW_SET_BASEf, rh_flow_set_base);
            soc_mem_field32_set(unit, L3_ECMP_COUNTm, hw_buf,
                    RH_FLOW_SET_SIZEf, rh_flow_set_size);
        }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
/* Write Load Balancing Mode fields */
        if (soc_feature(unit, soc_feature_ecmp_resilient_hash_optimized)
            && !(ecmp_info->ecmp_flags & BCM_L3_ECMP_WEIGHTED)) {
            ecmp_count_entry_t old_ecmp_count_entry;
            int lb_mode;
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, grp_mem, MEM_BLOCK_ANY,
                         ecmp_grp, &old_ecmp_count_entry));
            lb_mode = soc_mem_field32_get(unit, grp_mem,
                     &old_ecmp_count_entry, LB_MODEf);
            soc_mem_field32_set(unit, grp_mem, hw_buf,
                     LB_MODEf, lb_mode);
        }
#endif /* BCM_TOMAHAWK_SUPPORT */

        rv = soc_mem_write(unit, grp_mem, MEM_BLOCK_ALL,
                ecmp_grp, hw_buf);

        /* mode 1 = max possible ecmp groups */
        if (BCM_XGS3_L3_MAX_ECMP_MODE(unit)) {
            /* Save the max possible paths for this ECMP group in s/w */ 
            BCM_XGS3_L3_MAX_PATHS_PERGROUP_PTR(unit)[ecmp_grp] = ecmp_info->max_paths;
        }
    }

    if (BCM_FAILURE(rv)) {
        if (ecmp_table_incr == 1) {
            BCM_XGS3_L3_ENT_REF_CNT_DEC(BCM_XGS3_L3_TBL_PTR(unit, ecmp), 
                                        ecmp_idx, max_grp_size);
        }
    }
    return rv;
}


/*
 * Function:
 *      _bcm_td_l3_ecmp_grp_del
 * Purpose:
 *      Reset ecmp group next hop members
 * Parameters:
 *      unit       - (IN)SOC unit number.
 *      ecmp_grp   - (IN)ecmp group id to write.
 *      info       - (IN)ECMP additional info
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_td_l3_ecmp_grp_del (int unit, int ecmp_grp, void *info)
{
    uint32 hw_buf[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to write hw entry.*/
    int ecmp_idx = 0; /* Ecmp table entry index. */
    int idx; /* Iteration index. */
    int rv = BCM_E_UNAVAIL; /* Operation return value. */
    _bcm_l3_tbl_op_t data;
    soc_mem_t mem = L3_ECMP_COUNTm;

    data.tbl_ptr = BCM_XGS3_L3_TBL_PTR(unit, ecmp); 

    if (NULL == info) {
        return BCM_E_INTERNAL;
    }
    /* Initialize ecmp entry. */
    sal_memset (hw_buf, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Calculate table index. */
#ifdef BCM_TOMAHAWK3_SUPPORT
    if ((soc_feature(unit, soc_feature_l3_ecmp_hier_tbl)) &&
        (BCM_XGS3_L3_TBL(unit, ecmp_info).ecmp_mode ==
                                         ecmp_mode_hierarchical) &&
        (ecmp_grp < (BCM_XGS3_L3_ECMP_MAX_GROUPS(unit) / 2))) {
        mem = ECMP_GROUP_HIERARCHICALm;
    }
#endif

    BCM_IF_ERROR_RETURN(
        _bcm_xgs3_l3_ecmp_grp_info_get(unit, ecmp_grp, NULL, &ecmp_idx));

    /* Write all the indexes to hw. */
    for (idx = 0; idx < ((_bcm_l3_ecmp_group_info_t*)info)->max_paths; idx++) {
        /* Write buffer to hw. */
        rv = soc_mem_write(unit, L3_ECMPm, MEM_BLOCK_ALL, 
                (ecmp_idx + idx), hw_buf);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        /* Write buffer to hw INITIAL_L3_ECMPm table. */
        if (!soc_feature(unit, soc_feature_post_ifp_single_stage_ecmp)) {
            rv = soc_mem_write(unit, INITIAL_L3_ECMPm, MEM_BLOCK_ALL,
                    (ecmp_idx + idx), hw_buf);

            if (BCM_FAILURE(rv)) {
                return rv;
            }
        }
    }

    /* Decrement ref count for the entries in ecmp table
     * Ref count for ecmp_group table is decremented in common table del func. */
    BCM_XGS3_L3_ENT_REF_CNT_DEC(data.tbl_ptr, ecmp_idx,
                                ((_bcm_l3_ecmp_group_info_t*)info)->max_paths);

    if (SOC_MEM_IS_VALID(unit, mem)) {
        /* Set group base pointer. */
        ecmp_idx = ecmp_grp;

        if (SOC_MEM_IS_VALID(unit, INITIAL_L3_ECMP_GROUPm)) {
            rv = soc_mem_write(unit, INITIAL_L3_ECMP_GROUPm, MEM_BLOCK_ALL,
                    ecmp_idx, hw_buf);
            BCM_IF_ERROR_RETURN(rv);
        }

        if (!BCM_XGS3_L3_MAX_ECMP_MODE(unit)) {
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                    (ecmp_idx+1), hw_buf);
            BCM_IF_ERROR_RETURN(rv);
        }

#ifdef BCM_TRIDENT2_SUPPORT
        /* Preserve the fields related to resilient hashing */
        if (soc_feature(unit, soc_feature_ecmp_resilient_hash)) {
            ecmp_count_entry_t old_ecmp_count_entry;
            int enhanced_hashing_enable;
            int rh_flow_set_base;
            int rh_flow_set_size;

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                        ecmp_grp, &old_ecmp_count_entry));
            if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
                enhanced_hashing_enable = soc_mem_field32_get(unit, mem,
                         &old_ecmp_count_entry, LB_MODEf);
            } else {
                enhanced_hashing_enable = soc_mem_field32_get(unit, mem,
                         &old_ecmp_count_entry, ENHANCED_HASHING_ENABLEf);
            }
            rh_flow_set_base = soc_mem_field32_get(unit, mem,
                    &old_ecmp_count_entry, RH_FLOW_SET_BASEf);
            rh_flow_set_size = soc_mem_field32_get(unit, mem,
                    &old_ecmp_count_entry, RH_FLOW_SET_SIZEf);
            if (soc_feature(unit, soc_feature_td3_style_dlb_rh)){
                soc_mem_field32_set(unit, mem, hw_buf,
                        LB_MODEf, enhanced_hashing_enable);
            } else {
                soc_mem_field32_set(unit, mem, hw_buf,
                        ENHANCED_HASHING_ENABLEf, enhanced_hashing_enable);
            }
            soc_mem_field32_set(unit, mem, hw_buf,
                    RH_FLOW_SET_BASEf, rh_flow_set_base);
            soc_mem_field32_set(unit, mem, hw_buf,
                    RH_FLOW_SET_SIZEf, rh_flow_set_size);
        }
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TOMAHAWK_SUPPORT
        /* LB field set */
        if (soc_feature(unit, soc_feature_ecmp_resilient_hash_optimized)) {
            ecmp_count_entry_t old_ecmp_count_entry;
            int lb_mode;

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                        ecmp_grp, &old_ecmp_count_entry));
            lb_mode = soc_mem_field32_get(unit, mem,
                    &old_ecmp_count_entry, LB_MODEf);
            soc_mem_field32_set(unit, mem, hw_buf,
                    LB_MODEf, lb_mode);
        }
#endif /* BCM_TOMAHAWK_SUPPORT */

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                ecmp_idx, hw_buf);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (BCM_XGS3_L3_MAX_ECMP_MODE(unit) &&
        BCM_XGS3_L3_MAX_PATHS_PERGROUP_PTR(unit)) {
        /* Reset max paths of the deleted group */
        BCM_XGS3_L3_MAX_PATHS_PERGROUP_PTR(unit)[ecmp_grp] = 0;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_td_l3_intf_qos_set
 * Purpose:
 *      Set L3 Interface QoS parameters
 * Parameters:
 *      unit      - (IN)SOC unit number.
 *      l3_if_entry_p - (IN) Pointer to buffer
 *      vid       - (IN) Intf_info
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td_l3_intf_qos_set(int unit, uint32 *l3_if_entry_p, _bcm_l3_intf_cfg_t *intf_info)
{
    int hw_map_idx=0;

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_COPY) {
         if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, OPRI_OCFI_SELf)) {
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, OPRI_OCFI_SELf, 0x0);
         }
    } else if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET) {
         if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, OPRI_OCFI_SELf)) {
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, OPRI_OCFI_SELf, 0x1);
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, OPRIf,
                                            intf_info->vlan_qos.pri);
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, OCFIf,
                                            intf_info->vlan_qos.cfi);
         }
    } else if (intf_info->vlan_qos.flags & BCM_L3_INTF_QOS_OUTER_VLAN_PRI_REMARK) {

         if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, OPRI_OCFI_SELf)) {
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, OPRI_OCFI_SELf, 0x2);
         }
         if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, OPRI_OCFI_MAPPING_PROFILEf)) {
              BCM_IF_ERROR_RETURN
               (_bcm_tr2_qos_id2idx(unit, intf_info->vlan_qos.qos_map_id, &hw_map_idx));
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, 
                             OPRI_OCFI_MAPPING_PROFILEf, hw_map_idx);
         }
    }

    if (intf_info->inner_vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_COPY) {
       if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, IPRI_ICFI_SELf)) {
           soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, IPRI_ICFI_SELf, 0x0);
       }
   } else if (intf_info->inner_vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_SET) {
       if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, IPRI_ICFI_SELf)) {
           soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, IPRI_ICFI_SELf, 0x1);
           soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, IPRIf,
                        intf_info->inner_vlan_qos.pri);
           soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, ICFIf,
                        intf_info->inner_vlan_qos.cfi);
       }
   } else if (intf_info->inner_vlan_qos.flags & BCM_L3_INTF_QOS_INNER_VLAN_PRI_REMARK) {
       if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, IPRI_ICFI_SELf)) {
           soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, IPRI_ICFI_SELf, 0x2);
       }
       if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, IPRI_ICFI_MAPPING_PROFILEf)) {
           BCM_IF_ERROR_RETURN
              (_bcm_tr2_qos_id2idx(unit, intf_info->inner_vlan_qos.qos_map_id, &hw_map_idx));
           soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, 
                        IPRI_ICFI_MAPPING_PROFILEf, hw_map_idx);
       }
   }

    if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_COPY) {
         if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, DSCP_SELf)) {
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, DSCP_SELf, 0x0);
         }
    } else if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_SET) {
         if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, DSCP_SELf)) {
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, DSCP_SELf, 0x1);
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, DSCPf,
                                            intf_info->dscp_qos.dscp);
         }
    } else if (intf_info->dscp_qos.flags & BCM_L3_INTF_QOS_DSCP_REMARK) {
         if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, DSCP_SELf)) {
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, DSCP_SELf, 0x2);
         }
         if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, DSCP_MAPPING_PTRf)) {
              BCM_IF_ERROR_RETURN
               (_bcm_tr2_qos_id2idx(unit, intf_info->dscp_qos.qos_map_id, &hw_map_idx));
              soc_mem_field32_set(unit, EGR_L3_INTFm, l3_if_entry_p, 
                             DSCP_MAPPING_PTRf, hw_map_idx);
         }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_l3_intf_qos_get
 * Purpose:
 *      Get L3 Interface QoS parameters
 * Parameters:
 *      unit      - (IN)SOC unit number.
*       l3_if_entry_p - entry buffer
 *      vid       - (IN) Intf_info
 * Returns:
 *      BCM_E_XXX.
 */
int
_bcm_td_l3_intf_qos_get(int unit, uint32 *l3_if_entry_p, _bcm_l3_intf_cfg_t *intf_info)
{

    /* Input parameters check */
    if (NULL == intf_info) {
        return (BCM_E_PARAM);
    }

    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, OPRI_OCFI_SELf)) {
       if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                l3_if_entry_p, OPRI_OCFI_SELf) == 0x0) {
              intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_COPY;
       } else if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                l3_if_entry_p, OPRI_OCFI_SELf) == 0x1) {
              intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_SET;
              intf_info->vlan_qos.pri = 
                   soc_mem_field32_get(unit, EGR_L3_INTFm, l3_if_entry_p, OPRIf);
              intf_info->vlan_qos.cfi = 
                   soc_mem_field32_get(unit, EGR_L3_INTFm, l3_if_entry_p, OCFIf);
       } else if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                l3_if_entry_p, OPRI_OCFI_SELf) == 0x2) {
              intf_info->vlan_qos.flags |= BCM_L3_INTF_QOS_OUTER_VLAN_PRI_REMARK;
              BCM_IF_ERROR_RETURN
                   (_bcm_tr2_qos_idx2id(unit, 
                        soc_mem_field32_get(unit, EGR_L3_INTFm, l3_if_entry_p,
                                OPRI_OCFI_MAPPING_PROFILEf), 0x2,
                        &intf_info->vlan_qos.qos_map_id));
        }
    }

    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, IPRI_ICFI_SELf)) {
       if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                l3_if_entry_p, IPRI_ICFI_SELf) == 0x0) {
              intf_info->inner_vlan_qos.flags |= BCM_L3_INTF_QOS_INNER_VLAN_PRI_COPY;
       } else if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                l3_if_entry_p, IPRI_ICFI_SELf) == 0x1) {
              intf_info->inner_vlan_qos.flags |= BCM_L3_INTF_QOS_INNER_VLAN_PRI_SET;
              intf_info->inner_vlan_qos.pri = 
                   soc_mem_field32_get(unit, EGR_L3_INTFm, l3_if_entry_p, IPRIf);
              intf_info->inner_vlan_qos.cfi = 
                   soc_mem_field32_get(unit, EGR_L3_INTFm, l3_if_entry_p, ICFIf);
       } else if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                l3_if_entry_p, IPRI_ICFI_SELf) == 0x2) {
              intf_info->inner_vlan_qos.flags |= BCM_L3_INTF_QOS_INNER_VLAN_PRI_REMARK;
              BCM_IF_ERROR_RETURN
                   (_bcm_tr2_qos_idx2id(unit, 
                        soc_mem_field32_get(unit, EGR_L3_INTFm, l3_if_entry_p,
                                IPRI_ICFI_MAPPING_PROFILEf), 0x2,
                        &intf_info->inner_vlan_qos.qos_map_id));
        }
    }


    if (SOC_MEM_FIELD_VALID(unit, EGR_L3_INTFm, DSCP_SELf)) {
         if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                  l3_if_entry_p, DSCP_SELf) == 0x0) {
              intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_COPY;
         } else if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                  l3_if_entry_p, DSCP_SELf) == 0x1) {
              intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_SET;
              intf_info->dscp_qos.dscp = 
                   soc_mem_field32_get(unit, EGR_L3_INTFm, l3_if_entry_p, DSCPf);
         } else if (soc_mem_field32_get(unit, EGR_L3_INTFm,
                  l3_if_entry_p, DSCP_SELf) == 0x2) {
              intf_info->dscp_qos.flags |= BCM_L3_INTF_QOS_DSCP_REMARK;
              BCM_IF_ERROR_RETURN
                   (_bcm_tr2_qos_idx2id(unit, 
                        soc_mem_field32_get(unit, EGR_L3_INTFm, l3_if_entry_p,
                                DSCP_MAPPING_PTRf), 0x4,
                        &intf_info->dscp_qos.qos_map_id));
         }
    } 
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td_l3_routed_int_pri_init
 * Purpose:
 *      Set L3 Routed Internal Priority
 * Parameters:
 *      unit      - (IN)SOC unit number.
 * Returns:
 *      BCM_E_XXX.
 */

int
_bcm_td_l3_routed_int_pri_init (int unit)
{
   int rv=BCM_E_NONE;
   int idx; /* Iteration index. */
   int max_ent_count=0; /* Number of entries to read.*/
   ing_routed_int_pri_mapping_entry_t   int_pri_entry;


    if(soc_mem_is_valid(unit, ING_ROUTED_INT_PRI_MAPPINGm)) {
       max_ent_count = soc_mem_index_count(unit, ING_ROUTED_INT_PRI_MAPPINGm);

       for(idx=0; idx<max_ent_count; idx++) {

            sal_memset(&int_pri_entry, 0, sizeof(ing_routed_int_pri_mapping_entry_t));

            soc_mem_field32_set(unit, ING_ROUTED_INT_PRI_MAPPINGm, &int_pri_entry,
                       NEW_INT_PRIf, idx);

            rv = soc_mem_write(unit, ING_ROUTED_INT_PRI_MAPPINGm,
                       MEM_BLOCK_ALL, idx, &int_pri_entry);

            if (rv < 0) {
                return rv;
            }
       }
    }
    return rv;
}

#else /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */
typedef int bcm_esw_trident_l3_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TRIDENT_SUPPORT && INCLUDE_L3 */

