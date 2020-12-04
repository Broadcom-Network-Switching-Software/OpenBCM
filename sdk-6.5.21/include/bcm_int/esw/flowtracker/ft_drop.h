/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ft_drop.h
 * Purpose: Function declarations for flowtracker drop reason groups.
 */
#ifndef _BCM_INT_FT_DROP_REASON_H_
#define _BCM_INT_FT_DROP_REASON_H_

#if defined(BCM_FLOWTRACKER_SUPPORT) && defined(BCM_FLOWTRACKER_V2_SUPPORT)

/* Flowtracker drop reason global enum */
#define BCMI_FT_DROP_REASON_GROUP_ID_ING_MMU_MAX(unit)    16
#define BCMI_FT_DROP_REASON_GROUP_ID_EGR_MAX(unit)        16

/* Drop reason bits in bs2.0 hardware */
#define BCMI_FT_DROP_BIT_ING_MMU_MAX                      192
#define BCMI_FT_DROP_BIT_EGR_MAX                          64

/* Flowtracker drop reason index generation */
#define BCMI_FT_DROP_REASON_GROUP_ID_TYPE_ING_MMU         1
#define BCMI_FT_DROP_REASON_GROUP_ID_TYPE_EGR             2
#define BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_ING_MMU     3
#define BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_EGR         4
#define BCMI_FT_DROP_REASON_GROUP_ID_TYPE_SHIFT           16
#define BCMI_FT_DROP_REASON_GROUP_ID_TYPE_MASK            0xffff

#define BCMI_FT_DROP_REASON_GROUP_ID_SET(index, type)             \
  (((type) << BCMI_FT_DROP_REASON_GROUP_ID_TYPE_SHIFT) | (index))

#define BCMI_FT_DROP_REASON_GROUP_INDEX_GET(id)                   \
            ((id) & BCMI_FT_DROP_REASON_GROUP_ID_TYPE_MASK)

#define BCMI_FT_DROP_REASON_GROUP_ID_IS_ING_MMU(id)               \
           (((id) >> BCMI_FT_DROP_REASON_GROUP_ID_TYPE_SHIFT) ==  \
                       BCMI_FT_DROP_REASON_GROUP_ID_TYPE_ING_MMU)

#define BCMI_FT_DROP_REASON_GROUP_ID_IS_EGR(id)                   \
           (((id) >> BCMI_FT_DROP_REASON_GROUP_ID_TYPE_SHIFT) ==  \
                           BCMI_FT_DROP_REASON_GROUP_ID_TYPE_EGR)

#define BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_ING_MMU(id)           \
           (((id) >> BCMI_FT_DROP_REASON_GROUP_ID_TYPE_SHIFT) ==  \
                       BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_ING_MMU)

#define BCMI_FT_DROP_REASON_GROUP_ID_IS_AGG_EGR(id)               \
           (((id) >> BCMI_FT_DROP_REASON_GROUP_ID_TYPE_SHIFT) ==  \
                           BCMI_FT_DROP_REASON_GROUP_ID_TYPE_AGG_EGR)

/* Flowtracker software check slice state */
typedef struct bcmi_ft_drop_reason_group_state_s {

    /* Ingress/Mmu drop reason group index bitmap. */
    SHR_BITDCL *drop_ing_mmu_idx_bitmap;

    /* Egress drop reason group index bitmap. */
    SHR_BITDCL *drop_egr_idx_bitmap;

    /* Ingress/Mmu drop reason group index reference count. */
    uint16 *drop_ing_mmu_idx_refcount;

    /* Egress drop reason group index reference count. */
    uint16 *drop_egr_idx_refcount;

    /* Ingress/Mmu drop reason group maximum index. */
    uint16 drop_ing_mmu_max_idx;

    /* Egress drop reason group maximum index. */
    uint16 drop_egr_max_idx;

    /* Software drop enum to hardware bit value mapping. */
    int16 *drop_reason_sw_to_hw;

    /* Ingress/Mmu drop hardware bit value to sw enum mapping. */
    int16 *drop_reason_ing_mmu_hw_to_sw;

    /* Egress drop hardware bit value to sw enum mapping. */
    int16 *drop_reason_egr_hw_to_sw;

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    /* Aggregate Ingress/Mmu drop reason group index bitmap. */
    SHR_BITDCL *drop_agg_ing_mmu_idx_bitmap;

    /* Aggregate Egress drop reason group index bitmap. */
    SHR_BITDCL *drop_agg_egr_idx_bitmap;

    /* Aggregate Ingress/Mmu drop reason group index reference count. */
    uint16 *drop_agg_ing_mmu_idx_refcount;

    /* Aggregate Egress drop reason group index reference count. */
    uint16 *drop_agg_egr_idx_refcount;
#endif

} bcmi_ft_drop_reason_group_state_t;

extern bcmi_ft_drop_reason_group_state_t
        bcmi_ft_drop_reason_group_state[BCM_MAX_NUM_UNITS];

#define BCMI_FT_DROP_REASON_ING_MMU_BITMAP(unit)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_ing_mmu_idx_bitmap)

#define BCMI_FT_DROP_REASON_EGR_BITMAP(unit)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_egr_idx_bitmap)

#define BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_STATE(unit)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_ing_mmu_idx_refcount)

#define BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT(unit, index)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_ing_mmu_idx_refcount[index])

#define BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_DEC(unit, index)  \
   ((bcmi_ft_drop_reason_group_state[unit].drop_ing_mmu_idx_refcount[index])--)

#define BCMI_FT_DROP_REASON_GROUP_ING_MMU_REFCOUNT_INC(unit, index)  \
   ((bcmi_ft_drop_reason_group_state[unit].drop_ing_mmu_idx_refcount[index])++)

#define BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_STATE(unit)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_egr_idx_refcount)

#define BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT(unit, index)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_egr_idx_refcount[index])

#define BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_DEC(unit, index)  \
   ((bcmi_ft_drop_reason_group_state[unit].drop_egr_idx_refcount[index])--)

#define BCMI_FT_DROP_REASON_GROUP_EGR_REFCOUNT_INC(unit, index)  \
   ((bcmi_ft_drop_reason_group_state[unit].drop_egr_idx_refcount[index])++)

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
#define BCMI_FT_DROP_REASON_AGG_ING_MMU_BITMAP(unit)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_agg_ing_mmu_idx_bitmap)

#define BCMI_FT_DROP_REASON_AGG_EGR_BITMAP(unit)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_agg_egr_idx_bitmap)

#define BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_STATE(unit)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_agg_ing_mmu_idx_refcount)

#define BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT(unit, index)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_agg_ing_mmu_idx_refcount[index])

#define BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_DEC(unit, index)  \
   ((bcmi_ft_drop_reason_group_state[unit].drop_agg_ing_mmu_idx_refcount[index])--)

#define BCMI_FT_DROP_REASON_GROUP_AGG_ING_MMU_REFCOUNT_INC(unit, index)  \
   ((bcmi_ft_drop_reason_group_state[unit].drop_agg_ing_mmu_idx_refcount[index])++)

#define BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_STATE(unit)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_agg_egr_idx_refcount)

#define BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT(unit, index)  \
    (bcmi_ft_drop_reason_group_state[unit].drop_agg_egr_idx_refcount[index])

#define BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_DEC(unit, index)  \
   ((bcmi_ft_drop_reason_group_state[unit].drop_agg_egr_idx_refcount[index])--)

#define BCMI_FT_DROP_REASON_GROUP_AGG_EGR_REFCOUNT_INC(unit, index)  \
   ((bcmi_ft_drop_reason_group_state[unit].drop_agg_egr_idx_refcount[index])++)

#endif


#define BCMI_FT_DROP_REASON_ING_MMU_MAX_IDX(unit) \
   (bcmi_ft_drop_reason_group_state[unit].drop_ing_mmu_max_idx)

#define BCMI_FT_DROP_REASON_EGR_MAX_IDX(unit) \
   (bcmi_ft_drop_reason_group_state[unit].drop_egr_max_idx)

#define BCMI_FT_DROP_REASON_SW_TO_HW_STATE(unit) \
   (bcmi_ft_drop_reason_group_state[unit].drop_reason_sw_to_hw)

#define BCMI_FT_DROP_REASON_ING_MMU_HW_TO_SW_STATE(unit) \
   (bcmi_ft_drop_reason_group_state[unit].drop_reason_ing_mmu_hw_to_sw)

#define BCMI_FT_DROP_REASON_EGR_HW_TO_SW_STATE(unit) \
   (bcmi_ft_drop_reason_group_state[unit].drop_reason_egr_hw_to_sw)

#define BCMI_FT_DROP_REASON_SW_TO_HW(unit, sw_enum) \
    (bcmi_ft_drop_reason_group_state[unit].drop_reason_sw_to_hw[sw_enum])

#define BCMI_FT_DROP_REASON_ING_MMU_HW_TO_SW(unit, hw_bit) \
    (bcmi_ft_drop_reason_group_state[unit].drop_reason_ing_mmu_hw_to_sw[hw_bit])

#define BCMI_FT_DROP_REASON_EGR_HW_TO_SW(unit, hw_bit) \
    (bcmi_ft_drop_reason_group_state[unit].drop_reason_egr_hw_to_sw[hw_bit])

extern void
bcmi_ftv2_drop_reason_group_incr_ref_count(int unit,
                bcm_flowtracker_drop_reason_group_t id);

extern void
bcmi_ftv2_drop_reason_group_decr_ref_count(int unit,
                bcm_flowtracker_drop_reason_group_t id);

extern int
bcmi_ftv2_drop_reason_group_id_is_invalid(int unit,
                bcm_flowtracker_drop_reason_group_t id);

extern int
bcmi_ftv2_drop_reason_group_vector_get(int unit,
                          bcmi_flowtracker_flowchecker_info_t *fc_info,
                          int *dg_check_pri_vector_value,
                          int *dg_check_sec_vector_value);

extern int
bcmi_ftv2_drop_reason_group_init(int unit);

extern void
bcmi_ftv2_drop_reason_group_cleanup(int unit);

extern int
bcmi_ftv2_drop_reason_group_create(int unit,
                                   uint32 flags,
                                   bcm_flowtracker_drop_reasons_t drop_reasons,
                                   bcm_flowtracker_drop_reason_group_t *id);

extern int
bcmi_ftv2_drop_reason_group_get(int unit,
                                bcm_flowtracker_drop_reason_group_t id,
                                bcm_flowtracker_drop_reasons_t *drop_reasons);

extern int
bcmi_ftv2_drop_reason_group_delete(int unit,
                                   bcm_flowtracker_drop_reason_group_t id);

#endif /* BCM_FLOWTRACKER_SUPPORT && BCM_FLOWTRACKER_V2_SUPPORT */
#endif /* _BCM_INT_FT_DROP_REASON_H_ */
