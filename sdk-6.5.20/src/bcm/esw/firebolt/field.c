/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        field.c
 * Purpose:     BCM56504 Field Processor installation functions.
 */
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/scache.h>

#include <bcm/error.h>

#include <bcm/field.h>
#include <bcm/mirror.h>

#include <bcm_int/esw_dispatch.h>
#if defined(BCM_FIREBOLT_SUPPORT)
#include <bcm_int/esw/firebolt.h>
#endif
#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif
#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/trx.h>
#endif
#if defined(BCM_HURRICANE2_SUPPORT)
#include <bcm_int/esw/hurricane2.h>
#endif
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/mirror.h>
#if defined(BCM_KATANA_SUPPORT)
#include <bcm_int/esw/katana.h>
#endif
#if defined(BCM_KATANA2_SUPPORT)
#include <bcm_int/esw/katana2.h>
#endif
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <bcm_int/esw/tomahawk.h>
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
#include <bcm_int/esw/trident3.h>
#endif

#if defined(BCM_FIREBOLT_SUPPORT) && defined(BCM_FIELD_SUPPORT)

/* local/static function prototypes */
STATIC void _field_fb_functions_init(int unit, _field_funct_t *functions);
STATIC int _field_fb_detach(int unit, _field_control_t *fc);
STATIC int _field_fb_hw_clear(int unit, _field_stage_t *stage_fc);

STATIC int _field_fb_tcam_policy_install(int unit, _field_entry_t *f_ent,
                                         int tcam_idx);
STATIC int _field_fb_tcam_policy_reinstall(int unit, _field_entry_t *f_ent,
                                         int tcam_idx);
STATIC int _field_fb_tcam_get(int unit, _field_entry_t *f_ent,
                               soc_mem_t mem, uint32 *buf);
STATIC int _field_fb_action_get(int unit, soc_mem_t mem, 
                                _field_entry_t *f_ent, int tcam_idx, 
                                _field_action_t *fa, uint32 *buf);

#ifdef BROADCOM_DEBUG
STATIC char *_field_fb_action_name(bcm_field_action_t action);
#endif /* BROADCOM_DEBUG */

static soc_field_t _fb_ing_f4_reg[16] =   {
    SLICE_0_F4f, SLICE_1_F4f,
    SLICE_2_F4f, SLICE_3_F4f,
    SLICE_4_F4f, SLICE_5_F4f,
    SLICE_6_F4f, SLICE_7_F4f,
    SLICE_8_F4f, SLICE_9_F4f,
    SLICE_10_F4f, SLICE_11_F4f,
    SLICE_12_F4f, SLICE_13_F4f,
    SLICE_14_F4f, SLICE_15_F4f};

#if defined(BCM_FIREBOLT2_SUPPORT) 
static soc_field_t _fb2_ifp_double_wide_key[16] = {
    SLICE0_DOUBLE_WIDE_KEY_SELECTf,
    SLICE1_DOUBLE_WIDE_KEY_SELECTf,
    SLICE2_DOUBLE_WIDE_KEY_SELECTf,
    SLICE3_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE4_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE5_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE6_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE7_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE8_DOUBLE_WIDE_KEY_SELECTf,
    SLICE9_DOUBLE_WIDE_KEY_SELECTf,
    SLICE10_DOUBLE_WIDE_KEY_SELECTf,
    SLICE11_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE12_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE13_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE14_DOUBLE_WIDE_KEY_SELECTf, 
    SLICE15_DOUBLE_WIDE_KEY_SELECTf};      
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */

static soc_field_t _fb_field_tbl[16][3] = {
    {SLICE0_F1f, SLICE0_F2f, SLICE0_F3f},
    {SLICE1_F1f, SLICE1_F2f, SLICE1_F3f},
    {SLICE2_F1f, SLICE2_F2f, SLICE2_F3f},
    {SLICE3_F1f, SLICE3_F2f, SLICE3_F3f},
    {SLICE4_F1f, SLICE4_F2f, SLICE4_F3f},
    {SLICE5_F1f, SLICE5_F2f, SLICE5_F3f},
    {SLICE6_F1f, SLICE6_F2f, SLICE6_F3f},
    {SLICE7_F1f, SLICE7_F2f, SLICE7_F3f},
    {SLICE8_F1f, SLICE8_F2f, SLICE8_F3f},
    {SLICE9_F1f, SLICE9_F2f, SLICE9_F3f},
    {SLICE10_F1f, SLICE10_F2f, SLICE10_F3f},
    {SLICE11_F1f, SLICE11_F2f, SLICE11_F3f},
    {SLICE12_F1f, SLICE12_F2f, SLICE12_F3f},
    {SLICE13_F1f, SLICE13_F2f, SLICE13_F3f},
    {SLICE14_F1f, SLICE14_F2f, SLICE14_F3f},
    {SLICE15_F1f, SLICE15_F2f, SLICE15_F3f}};
static soc_field_t _fb_ing_slice_mode_field[16] = {
    SLICE_0_MODEf,   SLICE_1_MODEf,
    SLICE_2_MODEf,   SLICE_3_MODEf,
    SLICE_4_MODEf,   SLICE_5_MODEf,
    SLICE_6_MODEf,   SLICE_7_MODEf,
    SLICE_8_MODEf,   SLICE_9_MODEf,
    SLICE_10_MODEf,  SLICE_11_MODEf,
    SLICE_12_MODEf,  SLICE_13_MODEf,
    SLICE_14_MODEf,  SLICE_15_MODEf};
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
static soc_field_t _vfp_slice_wide_mode_field[4] = {
    SLICE_0_DOUBLE_WIDE_MODEf,
    SLICE_1_DOUBLE_WIDE_MODEf,
    SLICE_2_DOUBLE_WIDE_MODEf,
    SLICE_3_DOUBLE_WIDE_MODEf};
static soc_field_t _vfp_slice_pairing_field[2] = {
    SLICE1_0_PAIRINGf,   SLICE3_2_PAIRINGf};
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
#if defined (BCM_FIREBOLT2_SUPPORT)
static soc_field_t _fb2_slice_pairing_field[8] = {
    SLICE1_0_PAIRINGf,   SLICE3_2_PAIRINGf,
    SLICE5_4_PAIRINGf,   SLICE7_6_PAIRINGf,
    SLICE9_8_PAIRINGf,   SLICE11_10_PAIRINGf,
    SLICE13_12_PAIRINGf, SLICE15_14_PAIRINGf};
static soc_field_t _fb2_slice_wide_mode_field[16] = {
    SLICE0_DOUBLE_WIDE_MODEf, 
    SLICE1_DOUBLE_WIDE_MODEf,
    SLICE2_DOUBLE_WIDE_MODEf,
    SLICE3_DOUBLE_WIDE_MODEf,
    SLICE4_DOUBLE_WIDE_MODEf,
    SLICE5_DOUBLE_WIDE_MODEf,
    SLICE6_DOUBLE_WIDE_MODEf,
    SLICE7_DOUBLE_WIDE_MODEf,
    SLICE8_DOUBLE_WIDE_MODEf,
    SLICE9_DOUBLE_WIDE_MODEf,
    SLICE10_DOUBLE_WIDE_MODEf,
    SLICE11_DOUBLE_WIDE_MODEf,
    SLICE12_DOUBLE_WIDE_MODEf,
    SLICE13_DOUBLE_WIDE_MODEf,
    SLICE14_DOUBLE_WIDE_MODEf,
    SLICE15_DOUBLE_WIDE_MODEf};
static soc_field_t _efp_slice_mode[] = {
    SLICE_0_MODEf, SLICE_1_MODEf,
    SLICE_2_MODEf, SLICE_3_MODEf};
static soc_field_t _vfp_field_tbl[4][2] = {
    {SLICE_0_F2f, SLICE_0_F3f},
    {SLICE_1_F2f, SLICE_1_F3f},
    {SLICE_2_F2f, SLICE_2_F3f},
    {SLICE_3_F2f, SLICE_3_F3f}};
#endif /* BCM_FIREBOLT2_SUPPORT */

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)       \
    || defined(BCM_TRX_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)
static uint32 virtual_to_physical_map[] = {
        VIRTUAL_SLICE_0_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_1_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_2_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_3_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_4_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_5_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_6_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_7_PHYSICAL_SLICE_NUMBERf
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)        \
    || defined(BCM_TRX_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)
        ,
        VIRTUAL_SLICE_8_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_9_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_10_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_11_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_12_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_13_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_14_PHYSICAL_SLICE_NUMBERf,
        VIRTUAL_SLICE_15_PHYSICAL_SLICE_NUMBERf
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT
          || BCM_TRX_SUPPORT && BCM_WARM_BOOT_SUPPORT */
    };
static uint32 virtual_to_group_map[] = {
        VIRTUAL_SLICE_0_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_1_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_2_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_3_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_4_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_5_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_6_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_7_VIRTUAL_SLICE_GROUPf
#if defined(BCM_RAVEN_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)        \
    || defined(BCM_TRX_SUPPORT) && defined(BCM_WARM_BOOT_SUPPORT)
        ,
        VIRTUAL_SLICE_8_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_9_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_10_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_11_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_12_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_13_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_14_VIRTUAL_SLICE_GROUPf,
        VIRTUAL_SLICE_15_VIRTUAL_SLICE_GROUPf
#endif /* BCM_RAVEN_SUPPORT || BCM_FIREBOLT2_SUPPORT
          || BCM_TRX_SUPPORT && BCM_WARM_BOOT_SUPPORT */
    };
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT
          || BCM_TRX_SUPPORT && BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT

#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_TRX_SUPPORT)
int
_field_slice_expanded_status_get(int unit, _field_control_t *fc, _field_stage_t *stage_fc,
                                 int *expanded, int *slice_master_idx)
{
    int i, vslice, slice_idx = 0, vgroup = -1, max = -1, slice_num = 0;
    fp_slice_map_entry_t map_entry;
    soc_field_t fld;
#if defined(BCM_TRX_SUPPORT)
    uint32 map_val;
#endif
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 vmap;

    if (fc->l2warm) {
    }

    if (!soc_feature(unit, soc_feature_field_multi_pipe_support)) {
        BCM_IF_ERROR_RETURN(READ_FP_SLICE_MAPm(unit, MEM_BLOCK_ANY, 0, &map_entry));
        slice_num = SOC_IS_HAWKEYE(unit)? 4 : 16;
        if (SOC_IS_SC_CQ(unit)) {
            slice_num = 12;
        }
    }
    if ((stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) || 
        (stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP)) {
        slice_num = 4;
    }

    /* Iterate over virtual slices to get physical slices and virtual groups */
    for (vslice = 0; vslice < slice_num; vslice++) {
        switch (stage_fc->stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            fld = virtual_to_physical_map[vslice];
            slice_idx = soc_FP_SLICE_MAPm_field32_get(unit, &map_entry, fld);
            /* Now get virtual group for this virtual slice */
            fld = virtual_to_group_map[vslice];
            vgroup = soc_FP_SLICE_MAPm_field32_get(unit, &map_entry, fld);
            break;
#if defined(BCM_TRX_SUPPORT)
        case _BCM_FIELD_STAGE_EGRESS:
            BCM_IF_ERROR_RETURN(READ_EFP_SLICE_MAPr(unit, &map_val));
            fld = virtual_to_physical_map[vslice];
            slice_idx = soc_reg_field_get(unit, EFP_SLICE_MAPr, map_val, fld);
            /* Now get virtual group for this virtual slice */
            fld = virtual_to_group_map[vslice];
            vgroup = soc_reg_field_get(unit, EFP_SLICE_MAPr, map_val, fld);
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            BCM_IF_ERROR_RETURN(READ_VFP_SLICE_MAPr(unit, &map_val));
            fld = virtual_to_physical_map[vslice];
            slice_idx = soc_reg_field_get(unit, VFP_SLICE_MAPr, map_val, fld);
            /* Now get virtual group for this virtual slice */
            fld = virtual_to_group_map[vslice];
            vgroup = soc_reg_field_get(unit, VFP_SLICE_MAPr, map_val, fld);
            break;
#endif
        default:
            break; /* Should never happen */
        }
        /* Phys slice_idx <=> virtual vslice <=> vgroup */
        stage_fc->vmap[_FP_DEF_INST][0][vslice].vmap_key = slice_idx;
        stage_fc->vmap[_FP_DEF_INST][0][vslice].virtual_group = vgroup;
        stage_fc->vmap[_FP_DEF_INST][0][vslice].priority = vgroup;
        
    }

    /* See if virtual slice is the highest number in the virtual group */
    for (vslice = 0; vslice < slice_num; vslice++) {
        max = -1;
        for (i = 0; i < slice_num; i++) {
           if ((stage_fc->vmap[_FP_DEF_INST][0][vslice].virtual_group == 
                stage_fc->vmap[_FP_DEF_INST][0][i].virtual_group)) {
                if (i > max) {
                    max = i;
                }
            }
        }
        if (!((fc->l2warm)
              && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_8))) {
            if ((max != vslice) && (max != -1)) {
                expanded[stage_fc->vmap[_FP_DEF_INST][0][vslice].vmap_key] = 1;
            }
        }
    }

    /* Software mantains 3 MAPS, make them identical if only 1 map is present */
    if (stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) {
        for (vmap = 1; vmap < _FP_VMAP_CNT; ++vmap) {
            for (vslice = 0; vslice < slice_num; vslice++) {
                stage_fc->vmap[_FP_DEF_INST][vmap][vslice] =
                                        stage_fc->vmap[_FP_DEF_INST][0][vslice];
            }
        }
    }

    if ((fc->l2warm)
        && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_8)) {
        for (i = 0; i < slice_num; i++) {
            expanded[i] = buf[fc->scache_pos] & 0x1;
            slice_master_idx[i] = buf[fc->scache_pos] >> 1;
            fc->scache_pos++;
        }
    }

    return BCM_E_NONE;
}

int 
_field_physical_to_virtual(int unit, int inst, int slice_idx,
                           _field_stage_t *stage_fc)
{
    int vslice, slice_num;
    slice_num = SOC_IS_HAWKEYE(unit) ? 4 : (SOC_IS_KATANAX(unit) ? 12 : 16);
    for (vslice = 0; vslice < slice_num; vslice++) {
        if (stage_fc->vmap[inst][0][vslice].vmap_key == slice_idx) {
            return vslice;
        }
    }
    return -1;
}
#endif 

/* Function: _mask_is_set(_bcm_field_qual_offset_t *offset, uint32 *buf)
 * Returns:  1 if any bit of buf is set between offset->offset and 
 *             offset->offset + offset->width
 *           0 otherwise
 */
int
_mask_is_set(int unit, _bcm_field_qual_offset_t *offset, uint32 *buf, 
             _field_stage_id_t stage_id) 
{
    int len = offset->width[0], bp = offset->offset[0];
    uint32 temp;
    /*    coverity[forward_null : FALSE]    */
    soc_mem_info_t *meminfo = NULL;

    switch (stage_id) {
        case _BCM_FIELD_STAGE_INGRESS:
            meminfo = &SOC_MEM_INFO(unit, FP_TCAMm);
            break;
        case _BCM_FIELD_STAGE_EGRESS:
            meminfo = &SOC_MEM_INFO(unit, EFP_TCAMm);
            break;
        case _BCM_FIELD_STAGE_LOOKUP:
            meminfo = &SOC_MEM_INFO(unit, VFP_TCAMm);
            break;
        default:
            /* Should never happen */
            return (FALSE);
    }
#define FIX_MEM_ORDER_E(v,m) (((m)->flags & SOC_MEM_FLAG_BE) ? \
                                BYTES2WORDS((m)->bytes)-1-(v) : \
                                (v))
    while (len > 0) {
        temp = 0;
        do {
            /* coverity [forward_null] */
            temp =
                (temp << 1) |
                ((buf[FIX_MEM_ORDER_E(bp / 32, meminfo)] >>
                  (bp & (32 - 1))) & 1);
            if (temp > 0) {
                return TRUE;
            }
            len--;
            bp++;
        } while (len & (32 - 1));
    }
    return FALSE;
}

int
_field_fb_counter_recover(int unit, _field_entry_t *f_ent, uint32 ctr_mode, 
                          uint32 ctr_idx, int part, bcm_field_stat_t sid)
{
    _field_group_t *fg;
    _field_stage_t *stage_fc;
    _field_stage_id_t stage_id;
    _field_control_t *fc;
    _field_slice_t *fs;
    _field_stat_t *f_st = NULL;
    _field_entry_stat_t *f_ent_st;
    int rv, idx, stat_id, found;
    uint32 sub_mode = 0, ent_st_flags = 0;
    bcm_field_stat_t stat_arr[2];
    uint8 nstat = 1;
    uint8 hw_entry_count = 1;

    fg = f_ent->group;
    fs = f_ent[part].fs;

    sal_memset(stat_arr, 0, sizeof(stat_arr));

    /* Get field control and stage control */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    stage_id = f_ent->group->stage_id;
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);      
    BCM_IF_ERROR_RETURN(rv);

    /* Search if counter has already been detected */
    /* Go over the counter in the slice */
    if (stage_id == _BCM_FIELD_STAGE_INGRESS) {
        sub_mode = ctr_mode & ~BCM_FIELD_COUNTER_MODE_BYTES;
        if ((sub_mode == BCM_FIELD_COUNTER_MODE_RED) || 
            (sub_mode == BCM_FIELD_COUNTER_MODE_YELLOW) || 
            (sub_mode == BCM_FIELD_COUNTER_MODE_GREEN) ||
            (sub_mode == BCM_FIELD_COUNTER_MODE_YES_NO) ||
            (sub_mode == BCM_FIELD_COUNTER_MODE_DEFAULT)) {
            hw_entry_count = 1;
        } else {
            hw_entry_count = 2;
        }
    } else if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
        hw_entry_count = 1;
    }
    if (((_FP_COUNTER_BMP_TEST(fs->counter_bmp, 2 * ctr_idx)) && 
        (_FP_COUNTER_BMP_TEST(fs->counter_bmp, 2 * ctr_idx + 1)) && 
        (2 == hw_entry_count)) ||
        ((_FP_COUNTER_BMP_TEST(fs->counter_bmp, 2 * ctr_idx)) && 
        (hw_entry_count == 1) && (ctr_mode % 2 == 0)) ||
        ((_FP_COUNTER_BMP_TEST(fs->counter_bmp, 2 * ctr_idx + 1)) && 
        (hw_entry_count == 1) && (ctr_mode % 2))) {
        /* Counter has been detected - increment reference count */ 
        /* Happens when counter is shared by different entries */
        /* Search the hash to match against the HW index */
        found = 0;
        for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
            f_st = fc->stat_hash[idx];
            while (f_st != NULL) {
                if ((f_st->hw_index == ctr_idx) && 
                    (f_st->pool_index == fs->slice_number) &&
                    (f_st->hw_mode == ctr_mode) &&
                    (f_st->stage_id == stage_id)) {
                    found = 1;
                    break;
                }
                f_st = f_st->next;
            }
            if (found) {
                break;
            }
        }
        if (!found) {
            return BCM_E_INTERNAL;
        }
        f_st->hw_ref_count++;
        f_st->sw_ref_count++;
        f_ent_st = &f_ent->statistic;
    } else {
        /* Allocate new stat object */
        if (fc->l2warm) {
            stat_id = sid;
        } else {
            BCM_IF_ERROR_RETURN(_bcm_field_stat_id_alloc(unit, &stat_id));
        }
        _FP_XGS3_ALLOC(f_st, sizeof (_field_stat_t), "Field stat entity");
        if (NULL == f_st) {
            return (BCM_E_MEMORY);
        }
        ent_st_flags |= _FP_ENTRY_STAT_INSTALLED;
        f_st->sw_ref_count = 2;
        f_st->hw_ref_count = 1;
        f_st->pool_index = fs->slice_number;
        f_st->hw_index = ctr_idx;
        f_st->sid = stat_id;
        f_st->stage_id = fg->stage_id;
        f_st->gid = fg->gid;
        f_st->hw_mode = ctr_mode;
        f_ent_st = &f_ent->statistic;
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            switch (sub_mode) {
                case (0x1):
                case (0x2):
                    stat_arr[0] = bcmFieldStatPackets;
                    nstat = 1;
                    break;
                case (0x9):
                case (0xa):
                    stat_arr[0] = bcmFieldStatBytes;
                    nstat = 1;
                    break;
                case (0x3):
                    stat_arr[0] = bcmFieldStatRedPackets;
                    stat_arr[1] = bcmFieldStatNotRedPackets;
                    nstat = 2;
                    break;
                case (0xb):
                    stat_arr[0] = bcmFieldStatRedBytes;
                    stat_arr[1] = bcmFieldStatNotRedBytes;
                    nstat = 2;
                    break;
                case (0x4):                                                                     
                    stat_arr[0] = bcmFieldStatGreenPackets;
                    stat_arr[1] = bcmFieldStatNotGreenPackets;
                    nstat = 2;
                    break;
                case (0xc):
                    stat_arr[0] = bcmFieldStatGreenBytes;
                    stat_arr[1] = bcmFieldStatNotGreenBytes;
                    nstat = 2;
                    break;
                case (0x5):
                    stat_arr[0] = bcmFieldStatGreenPackets;
                    stat_arr[1] = bcmFieldStatRedPackets;
                    nstat = 2;
                    break;
                case (0xd):
                    stat_arr[0] = bcmFieldStatGreenBytes;
                    stat_arr[1] = bcmFieldStatRedBytes;
                    nstat = 2;
                    break;
                case (0x6):
                    stat_arr[0] = bcmFieldStatGreenPackets;
                    stat_arr[1] = bcmFieldStatYellowPackets;
                    nstat = 2;
                    break;
                case (0xe):
                    stat_arr[0] = bcmFieldStatGreenBytes;
                    stat_arr[1] = bcmFieldStatYellowBytes;
                    nstat = 2;
                    break;                                                                       
                case (0x7):
                    stat_arr[0] = bcmFieldStatRedPackets;
                    stat_arr[1] = bcmFieldStatYellowPackets;
                    nstat = 2;
                    break;
                case (0xf):
                    stat_arr[0] = bcmFieldStatRedBytes;
                    stat_arr[1] = bcmFieldStatYellowBytes;
                    nstat = 2;
                    break;                                                        
                default:
                    break;
            }
        } else if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
            stat_arr[0] = bcmFieldStatPackets;      
        }
        rv = _bcm_field_stat_array_init(unit, fc, f_st, nstat, stat_arr);
        if (BCM_FAILURE(rv)) {
            sal_free(f_st);
            return rv;
        }

        /* Assume counter was created using the stat_create API */
        if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                if ((part == 2) || (part == 3)) {
                    f_ent->flags |= _FP_ENTRY_STAT_IN_SECONDARY_SLICE;
                }
            } else {
                if (part == 1) {
                    f_ent->flags |= _FP_ENTRY_STAT_IN_SECONDARY_SLICE;
                }
            }
        }
        _FP_HASH_INSERT(fc->stat_hash, f_st, 
                        (stat_id & _FP_HASH_INDEX_MASK(fc)));
        fc->stat_count++;
        /* Mark as used in the slice */
        if (2 == f_st->hw_entry_count) {
            _FP_COUNTER_BMP_ADD(fs->counter_bmp, 2 * ctr_idx);
            _FP_COUNTER_BMP_ADD(fs->counter_bmp, 2 * ctr_idx + 1);
        } else {
            if (ctr_mode % 2) {
                _FP_COUNTER_BMP_ADD(fs->counter_bmp, 2 * ctr_idx + 1);
            } else {
                _FP_COUNTER_BMP_ADD(fs->counter_bmp, 2 * ctr_idx);
            }
        }
        fg->group_status.counter_count =
             fg->group_status.counter_count + f_st->hw_entry_count;
    }

    /* Associate the stat object with the entry */
    f_ent_st->flags |= (_FP_ENTRY_STAT_VALID | ent_st_flags);
    f_ent_st->sid = f_st->sid;

    return BCM_E_NONE;
}

/*
 * Function:
 *    _field_fb_meter_index_in_use
 * Purpose:
 *    Returns success if meter index/meter pair is in use.
 * Parameters:
 *     unit             - (IN) BCM device number.
 *     fs               - (IN) Slice where meter resides.
 *     meter_pair_mode  - (IN) Meter mode.
 *     meter_offset     - (IN) Odd/Even meter offset value.
 *     idx              - (IN) Meter pair index.
 * Returns:
 *     BCM_E_XXX
 */
STATIC
int _field_fb_meter_index_in_use(int unit,
                                 _field_slice_t *fs,
                                 uint32 meter_pair_mode,
                                 uint32 meter_offset,
                                 int idx)
{
    /* Input parameter check. */
    if (NULL == fs) {
        return (BCM_E_INTERNAL);
    }

    /* Meters are per-slice resource. */
    if (meter_pair_mode == BCM_FIELD_METER_MODE_FLOW
        && _FP_METER_BMP_TEST(fs->meter_bmp, ((idx * 2) + meter_offset))) {
        /* Flow mode meter index in use. */
        return (BCM_E_NONE);
    } else if (_FP_METER_BMP_TEST(fs->meter_bmp, (idx * 2))
               && _FP_METER_BMP_TEST(fs->meter_bmp, ((idx * 2) + 1))) {
        /* Non-Flow mode meter pair index in use. */
        return (BCM_E_NONE);
    }
    return (BCM_E_NOT_FOUND);
}

int
_field_fb_meter_recover(int unit, _field_entry_t *f_ent, 
                        _meter_config_t *meter_conf, int part, 
                        bcm_policer_t pid)
{
    _field_group_t *fg;
    _field_stage_t *stage_fc;
    _field_stage_id_t stage_id;
    _field_control_t *fc;
    _field_slice_t *fs;
    _field_policer_t *f_pl = NULL;
    _field_entry_policer_t *f_ent_pl;
    int rv, idx, policer_id, found, mem_idx, meter_offset = 0;
    uint32 ent[SOC_MAX_MEM_FIELD_WORDS], ent_pl_flags = 0;
    uint32 refresh_count, bucket_count;
#ifdef BCM_FIREBOLT2_SUPPORT
    uint32 bucket_size;
#endif /* BCM_FIREBOLT2_SUPPORT */

    if ((meter_conf->meter_mode == 0) && (meter_conf->meter_idx == 0)) {
        return BCM_E_NONE;
    }

    fg = f_ent->group;
    fs = f_ent[part].fs;

    /* Get field control and stage control */
    BCM_IF_ERROR_RETURN(_field_control_get(unit, &fc));
    stage_id = f_ent->group->stage_id;
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);      
    BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_FIREBOLT2_SUPPORT
    if ((meter_conf->meter_mode == 4) && 
        (stage_id == _BCM_FIELD_STAGE_EGRESS)) {
        return BCM_E_NONE;
    } 
#endif

    if (BCM_FIELD_METER_MODE_FLOW == meter_conf->meter_mode) {
        meter_offset = (meter_conf->meter_update_even
                        && meter_conf->meter_test_even) ? 0 : 1;
    }

    if (BCM_SUCCESS(_field_fb_meter_index_in_use(unit, fs,
            meter_conf->meter_mode, meter_offset, meter_conf->meter_idx))) {
        found = 0;
        for (idx = 0; idx < _FP_HASH_SZ(fc); idx++) {
            f_pl = fc->policer_hash[idx];
            while (f_pl != NULL) {
                if ((f_pl->hw_index == meter_conf->meter_idx) && 
                    (f_pl->pool_index == fs->slice_number) &&
                    (f_pl->stage_id == stage_id)) {
                        if (meter_conf->meter_mode ==
                             BCM_FIELD_METER_MODE_FLOW) {
                            if (((meter_offset == 1) &&
                                 (_FP_POLICER_EXCESS_HW_METER(f_pl))) ||
                                ((meter_offset == 0) &&
                                 !(_FP_POLICER_EXCESS_HW_METER(f_pl)))) {
                                f_pl = f_pl->next;
                                continue;
                            } else {
                               found =1;
                                break;
                            }
                        } else {
                            found = 1;
                            break;
                        }
                }
                f_pl = f_pl->next;
            }
            if (found) {
                break;
            }
        }
        if (!found) {
            return BCM_E_INTERNAL;
        }
        f_pl->hw_ref_count++;
        f_pl->sw_ref_count++;
    } else {
        /* Allocate new policer object */
        if (fc->l2warm) {
            policer_id = pid;
        } else {
            BCM_IF_ERROR_RETURN(_field_policer_id_alloc(unit, &policer_id));
        }
        _FP_XGS3_ALLOC(f_pl, sizeof (_field_policer_t), "Field policer entity");
        if (NULL == f_pl) {
            return (BCM_E_MEMORY);
        }
        ent_pl_flags |= _FP_POLICER_INSTALLED;
        f_pl->sw_ref_count = 2;
        f_pl->hw_ref_count = 1;
        f_pl->pool_index = fs->slice_number;
        f_pl->hw_index = meter_conf->meter_idx;
        f_pl->pid = policer_id;
        f_pl->stage_id = fg->stage_id;

        switch (meter_conf->meter_mode) {
            case BCM_FIELD_METER_MODE_DEFAULT: /* 0 */
                f_pl->cfg.mode = bcmPolicerModeGreen;
                break;
            case BCM_FIELD_METER_MODE_FLOW: /* 1 */
                f_pl->cfg.mode = bcmPolicerModeCommitted;

                if (meter_offset) {
                    /* Flow mode using committed hardware meter. */
                    _FP_POLICER_EXCESS_HW_METER_CLEAR(f_pl);
                } else {
                    /* Flow mode using excess hardware meter. */
                    _FP_POLICER_EXCESS_HW_METER_SET(f_pl);
                }

                mem_idx =
                stage_fc->slices[_FP_DEF_INST][f_pl->pool_index].start_tcam_idx
                            + (2 * f_pl->hw_index) + meter_offset;
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  mem_idx, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_pl);
                    return rv;
                }
                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                   BUCKETCOUNTf);
                
                f_pl->cfg.ckbits_sec = refresh_count * _FP_POLICER_REFRESH_RATE;
#ifdef BCM_FIREBOLT2_SUPPORT
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
                if (SOC_IS_FIREBOLT2(unit)) {
                    f_pl->cfg.ckbits_burst
                        = _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    f_pl->cfg.ckbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                break;
            case BCM_FIELD_METER_MODE_trTCM_COLOR_BLIND: /* 2 */
                f_pl->cfg.flags |= BCM_POLICER_COLOR_BLIND;
                /* Fall through */
            case BCM_FIELD_METER_MODE_trTCM_COLOR_AWARE: /* 3 */
                f_pl->cfg.mode = bcmPolicerModeTrTcm;
                /* Get the peak attributes */
                mem_idx =
                stage_fc->slices[_FP_DEF_INST][f_pl->pool_index].start_tcam_idx
                            + (2 * f_pl->hw_index);
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  mem_idx, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_pl);
                    return rv;
                }
                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                   BUCKETCOUNTf);
                f_pl->cfg.pkbits_sec = refresh_count * _FP_POLICER_REFRESH_RATE;
#ifdef BCM_FIREBOLT2_SUPPORT
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
                if (SOC_IS_FIREBOLT2(unit)) {
                    f_pl->cfg.pkbits_burst =
                       _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    f_pl->cfg.pkbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                /* Get the committed attributes */
                mem_idx =
                stage_fc->slices[_FP_DEF_INST][f_pl->pool_index].start_tcam_idx
                            + (2 * f_pl->hw_index) + 1;
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  mem_idx, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_pl);
                    return rv;
                }
                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                   BUCKETCOUNTf);
                f_pl->cfg.ckbits_sec = refresh_count * _FP_POLICER_REFRESH_RATE;
#ifdef BCM_FIREBOLT2_SUPPORT
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
                if (SOC_IS_FIREBOLT2(unit)) {
                    f_pl->cfg.ckbits_burst = 
                       _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    f_pl->cfg.ckbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                break;
            case 4:
                if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
                    f_pl->cfg.mode = bcmPolicerModePassThrough;
                }
                break;
            case 5:
                break;
            case BCM_FIELD_METER_MODE_srTCM_COLOR_BLIND: /* 6 */
                f_pl->cfg.flags |= BCM_POLICER_COLOR_BLIND;
                /* Fall through */
            case BCM_FIELD_METER_MODE_srTCM_COLOR_AWARE: /* 7 */
                f_pl->cfg.mode = bcmPolicerModeSrTcm;
                /* Get the peak attributes */
                mem_idx =
                stage_fc->slices[_FP_DEF_INST][f_pl->pool_index].start_tcam_idx
                            + (2 * f_pl->hw_index);
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  mem_idx, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_pl);
                    return rv;
                }
                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                   BUCKETCOUNTf);
                f_pl->cfg.pkbits_sec = refresh_count * _FP_POLICER_REFRESH_RATE;
#ifdef BCM_FIREBOLT2_SUPPORT
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
                if (SOC_IS_FIREBOLT2(unit)) {
                    f_pl->cfg.pkbits_burst = 
                       _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    f_pl->cfg.pkbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                /* Get the committed attributes */
                mem_idx =
                stage_fc->slices[_FP_DEF_INST][f_pl->pool_index].start_tcam_idx
                            + (2 * f_pl->hw_index) + 1;
                rv = soc_mem_read(unit, FP_METER_TABLEm, MEM_BLOCK_ANY,
                                  mem_idx, ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_pl);
                    return rv;
                }
                refresh_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                    REFRESHCOUNTf);
                bucket_count = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                   BUCKETCOUNTf);
                f_pl->cfg.ckbits_sec = refresh_count * _FP_POLICER_REFRESH_RATE;
#ifdef BCM_FIREBOLT2_SUPPORT
                bucket_size = soc_mem_field32_get(unit, FP_METER_TABLEm, ent,
                                                  BUCKETSIZEf);
                if (SOC_IS_FIREBOLT2(unit)) {
                    f_pl->cfg.ckbits_burst = 
                       _bcm_fb2_bucket_encoding_to_kbits(bucket_size, TRUE);
                } else
#endif
                {
                    f_pl->cfg.ckbits_burst = bucket_count / 2 / 1024;
                    /* Refer to _field_fb_bucket_calc */
                }
                break;
            default:
                break;
        }

        /* Assume policer was created using the policer_create API */
        if (fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                if ((part == 2) || (part == 3)) {
                    f_ent->flags |= _FP_ENTRY_POLICER_IN_SECONDARY_SLICE;
                }
            } else {
                if (part == 1) {
                    f_ent->flags |= _FP_ENTRY_POLICER_IN_SECONDARY_SLICE;
                }
            }
        }
        _FP_HASH_INSERT(fc->policer_hash, f_pl, 
                        (policer_id & _FP_HASH_INDEX_MASK(fc)));
        fc->policer_count++;
        /* Mark as used in the slice */
        if (!_FP_POLICER_IS_FLOW_MODE(f_pl)) {
            _FP_METER_BMP_ADD(fs->meter_bmp, (f_pl->hw_index * 2));
            _FP_METER_BMP_ADD(fs->meter_bmp, ((f_pl->hw_index * 2) + 1));
        } else {
            _FP_METER_BMP_ADD(fs->meter_bmp, ((f_pl->hw_index * 2) + meter_offset));
        }
        fg->group_status.meter_count++;
    }

    /* Associate the policer object with the entry */
    f_ent_pl = &f_ent->policer[0];
    f_ent_pl->flags |= (_FP_POLICER_VALID | ent_pl_flags);
    f_ent_pl->pid = f_pl->pid;

    return BCM_E_NONE;
}

STATIC int
_field_fb_actions_recover_action_add(int                unit,
                                      _field_entry_t     *f_ent,
                                      bcm_field_action_t action,
                                      uint32             param0,
                                      uint32             param1,
                                      uint32             param2,
                                      uint32             param3,
                                      uint32             hw_index
                                      )
{
    int             rv;
    _field_action_t *fa = NULL;

    rv = _field_action_alloc(unit, action,
             param0, param1, param2, param3, 0, 0, &fa);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    fa->hw_index = hw_index;

    fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */

    /* Add action to front of entry's linked-list. */
    fa->next = f_ent->actions;
    f_ent->actions  = fa;

    return (BCM_E_NONE);                                            
}

int
_field_fb_actions_recover(int unit, uint32 *policy_entry, 
                           _field_entry_t *f_ent, int part, soc_mem_t mem,
                           _field_entry_wb_info_t *f_ent_wb_info)
{
    soc_field_t fld;                    /* Policy table field */
    uint32 fldval, param0, param1, append; /* Field value and action params */
    uint32 ctr_mode, ctr_idx;           /* Counter mode and index */
    _field_action_t *fa = NULL;         /* Action details */
    bcm_field_action_t action;          /* Action type */
    int rv, i, num_fields;              /* Number of policy table fields */
    _meter_config_t meter_conf;         /* Meter config info */
    egr_l3_next_hop_entry_t egr_l3_next_hop_entry; /* Egress Next Hop table
                                                    * entry info */
    sal_mac_addr_t mac_addr;             /* MAC address info */
    uint32 mac_addr_words[2];            /* MAC address info for Param[0],
                                          * Param[1] */
    uint16 vlan_id;                      /* VLAN ID info */
    uint32 hw_index;                     /* Policy table index info */
    int action_sync = 1;

    if (f_ent_wb_info == NULL) {
        return BCM_E_INTERNAL;
    }

    num_fields = SOC_MEM_INFO(unit, mem).nFields;
    if (f_ent_wb_info->action_bmp == NULL || f_ent_wb_info->action_bmp->w == NULL) {
       action_sync = 0;
    }

    for (i = 0; i < num_fields; i++) {
        fld = SOC_MEM_INFO(unit, mem).fields[i].field;
        fldval = PolicyGet(unit, mem, policy_entry, fld);
        action = param0 = param1 = append = 0;
        switch (fld) {
        case GREEN_TO_PIDf:
            if (fldval) {
                f_ent->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
            }
            break;
        case ECN_CNGf:
            if (fldval) {
                action = bcmFieldActionEcnNew;
                append = 1;
            }
            break;
        case CHANGE_PRIORITYf:
            if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
                if (fldval) {
                    action = bcmFieldActionOuterVlanPrioNew;
                    param0 = PolicyGet(unit, mem, policy_entry, 
                                       NEW_PRIORITYf);
                    append = 1;
                }
            } else {
                switch (fldval) {
                    case 1:
                        action = bcmFieldActionCosQNew;
                        param0 = PolicyGet(unit, mem, policy_entry, 
                                           NEWPRIf);
                        append = 1;
                        break;
                    case 2:
                        action = bcmFieldActionCosQCpuNew;
                        param0 = PolicyGet(unit, mem, policy_entry, 
                                           NEWPRIf);
                        append = 1;
                        break;
                    case 4:
                        action = bcmFieldActionPrioPktAndIntCopy;
                        append = 1;
                        break;
                    case 5:
                        action = bcmFieldActionPrioPktAndIntNew;
                        param0 = PolicyGet(unit, mem, policy_entry, 
                                           NEWPRIf);
                        append = 1;
                        break;
                    case 6:
                        action = bcmFieldActionPrioPktAndIntTos;
                        append = 1;
                        break;
                    case 7:
                        action = bcmFieldActionPrioPktAndIntCancel;
                        append = 1;
                        break;
                    case 8:
                        action = bcmFieldActionPrioPktCopy;
                        append = 1;
                        break;
                    case 9:
                        action = bcmFieldActionPrioPktNew;
                        param0 = PolicyGet(unit, mem, policy_entry, 
                                           NEWPRIf);
                        append = 1;
                        break;
                    case 10:
                        action = bcmFieldActionPrioPktTos;
                        append = 1;
                        break;
                    case 11:
                        action = bcmFieldActionPrioPktCancel;
                        append = 1;
                        break;
                    case 12:
                        action = bcmFieldActionPrioIntCopy;
                        append = 1;
                        break;
                    case 13:
                        action = bcmFieldActionPrioIntNew;
                        param0 = PolicyGet(unit, mem, policy_entry, 
                                           NEWPRIf);
                        append = 1;
                        break;
                    case 14:
                        action = bcmFieldActionPrioIntTos;
                        append = 1;
                        break;
                    case 15:
                        action = bcmFieldActionPrioIntCancel;
                        append = 1;
                        break;
                    default:
                        break; /* Do nothing */
                }
            }
            break;
        case CHANGE_DSCP_TOSf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionTosNew;
                    param0 = PolicyGet(unit, mem, policy_entry, NEWDSCP_TOSf);
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionTosCopy;
                    append = 1;
                    break;
                case 3:
                    action = bcmFieldActionDscpNew;
                    param0 = PolicyGet(unit, mem, policy_entry, NEWDSCP_TOSf);
                    append = 1;
                    break;
                case 4:
                    action = bcmFieldActionDscpCancel;
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case RP_CHANGE_DSCPf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionRpDscpNew;
                    param0 = PolicyGet(unit, mem, policy_entry, RP_DSCPf);
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case YP_CHANGE_DSCPf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionYpDscpNew;
                    param0 = PolicyGet(unit, mem, policy_entry, YP_DSCPf);
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case COPY_TO_CPUf:
            if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
                if (fldval) {
                    action = bcmFieldActionCopyToCpu;
                    param0 = 1;
                    append = 1;
                }
            } else {
                switch (fldval) {
                    case 1:
                        action = bcmFieldActionCopyToCpu;
                        param1 = PolicyGet(unit, mem, policy_entry, 
                                           MATCHED_RULEf);
                        if (param1 != 0) {
                            param0 = 1;
                        }
                        append = 1;
                        break;
                    case 2:
                        action = bcmFieldActionCopyToCpuCancel;
                        append = 1;
                        break;
                    case 3:
                        action = bcmFieldActionSwitchToCpuCancel;
                        append = 1;
                        break;
                    default:
                        break; /* Do nothing */
                }
            }
            break;
        case YP_COPY_TO_CPUf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionYpCopyToCpu;
                    param1 = PolicyGet(unit, mem, policy_entry, MATCHED_RULEf);
                    if (param1 != 0) {
                        param0 = 1;
                    }
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionYpCopyToCpuCancel;
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case RP_COPY_TO_CPUf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionRpCopyToCpu;
                    param1 = PolicyGet(unit, mem, policy_entry, MATCHED_RULEf);
                    if (param1 != 0) {
                        param0 = 1;
                    }
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionRpCopyToCpuCancel;
                    append = 1;
                    break;
                case 3:
                    action = bcmFieldActionRpSwitchToCpuCancel;
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case PACKET_REDIRECTIONf:
            switch (fldval) {
                case 1:
                    if (PolicyGet(unit, mem, policy_entry, 
                                  REDIRECTIONf) & 0x20) {
                        action = bcmFieldActionRedirectTrunk;
                        param0 = PolicyGet(unit, mem, policy_entry, 
                                           REDIRECTIONf) & 0x1f;
                    } else {
                        action = bcmFieldActionRedirect;
                        param0 = PolicyGet(unit, mem, policy_entry, 
                                           REDIRECTIONf) >> 6;
                        param1 = PolicyGet(unit, mem, policy_entry, 
                                           REDIRECTIONf) & 0x3f;
                    }
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionRedirectCancel;
                    append = 1;
                    break;
                case 3:
                    action = bcmFieldActionRedirectPbmp;
                    param0 = PolicyGet(unit, mem, policy_entry, REDIRECTIONf);
                    append = 1;
                    break;
                case 4:
                    action = bcmFieldActionEgressMask;
                    param0 = PolicyGet(unit, mem, policy_entry, REDIRECTIONf);
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case DROPf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionDrop;
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionDropCancel;
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case YP_DROPf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionYpDrop;
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionYpDropCancel;
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case RP_DROPf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionRpDrop;
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionRpDropCancel;
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case GP_DROPf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionGpDrop;
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionGpDropCancel;
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case MIRROR_OVERRIDEf:
            if (fldval) {
                action = bcmFieldActionMirrorOverride;
                append = 1;
            }
            break;
        case MIRRORf:
            if (fldval & 0x1) {
                uint32 im_entry[SOC_MAX_MEM_FIELD_WORDS];
                action = bcmFieldActionMirrorIngress;
                rv = _field_action_alloc(unit, action,
                         param0, param1, 0, 0, 0, 0, &fa);
                fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
                fa->hw_index = PolicyGet(unit, mem, policy_entry, 
                                         IM_MTP_INDEXf);
                rv = soc_mem_read(unit, IM_MTP_INDEXm, MEM_BLOCK_ANY, 
                                  fa->hw_index, &im_entry);
                if (BCM_FAILURE(rv)) {
                    sal_free(fa);
                    return rv;
                }
                fa->param[0] = soc_IM_MTP_INDEXm_field32_get(unit, im_entry, 
                                                             MODULE_IDf);
                fa->param[1] = soc_IM_MTP_INDEXm_field32_get(unit, im_entry, 
                                                             PORT_TGIDf);
                /* Add action to front of entry's linked-list. */
                fa->next = f_ent->actions;
                f_ent->actions  = fa;
            }
            if (fldval & 0x2) {
                uint32 em_entry[SOC_MAX_MEM_FIELD_WORDS];
                action = bcmFieldActionMirrorEgress;
                rv = _field_action_alloc(unit, action,
                         param0, param1, 0, 0, 0, 0, &fa);
                fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */
                if (BCM_FAILURE(rv)) {
                    return rv;
                }
                fa->hw_index = PolicyGet(unit, mem, policy_entry, 
                                         EM_MTP_INDEXf);
                rv = soc_mem_read(unit, EM_MTP_INDEXm, MEM_BLOCK_ANY, 
                                  fa->hw_index, &em_entry);
                if (BCM_FAILURE(rv)) {
                    sal_free(fa);
                    return rv;
                }
                fa->param[0] = soc_EM_MTP_INDEXm_field32_get(unit, em_entry, 
                                                             MODULE_IDf);
                fa->param[1] = soc_EM_MTP_INDEXm_field32_get(unit, em_entry, 
                                                             PORT_TGIDf);
                /* Add action to front of entry's linked-list. */
                fa->next = f_ent->actions;
                f_ent->actions  = fa;
            }
            break;
        case L3SW_CHANGE_MACDA_OR_VLANf:
            switch (fldval) {
                int ecmp_paths, nh_ecmp;
                case 1:
                    action = bcmFieldActionL3ChangeVlan;
                    if (PolicyGet(unit, mem, policy_entry, ECMPf)) {
                        ecmp_paths = PolicyGet(unit, mem, 
                                               policy_entry, ECMP_COUNTf) + 1;
                        nh_ecmp = PolicyGet(unit, mem, 
                                            policy_entry, ECMP_PTRf);
                        param0 = _FP_L3_ACTION_PACK_ECMP(nh_ecmp, ecmp_paths);
                    } else {
                        nh_ecmp = PolicyGet(unit, mem, 
                                            policy_entry, NEXT_HOP_INDEXf);
                        param0 = _FP_L3_ACTION_PACK_NEXT_HOP(nh_ecmp);
                    }
                    append = 1;
                    
                    if (PolicyGet(unit, mem, policy_entry, ECMPf) == 0) {

                        /* Get the Next Hop Index value */
                        hw_index = PolicyGet(unit, mem, policy_entry,
                                        NEXT_HOP_INDEXf);

                        /* Read Next Hop information into entry */
                        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                                MEM_BLOCK_ANY, hw_index,
                                egr_l3_next_hop_entry.entry_data);
                        if (BCM_FAILURE(rv)) {
                            return (rv);
                        }

                        /* Get the VLAN ID infomation */
                        vlan_id = soc_mem_field32_get(unit, EGR_L3_NEXT_HOPm,
                                        egr_l3_next_hop_entry.entry_data,
                                        INTF_NUMf);

                        /* Re-Create bcmFieldActionOuterVlanNew action for the
                         * FP entry */
                        rv = _field_fb_actions_recover_action_add(unit,
                                f_ent, bcmFieldActionOuterVlanNew, vlan_id, 0,
                                0, 0, hw_index);
                        if (BCM_FAILURE(rv)) {
                            return (rv);
                        }
                    }
                    break;
                case 2:
                    action = bcmFieldActionL3ChangeVlanCancel;
                    append = 1;
                    break;
                case 3:
                /*recover fabricQueue action*/
                if ((action_sync == 1) &&
                     SHR_BITGET(f_ent_wb_info->action_bmp->w,
                                 _bcmFieldActionNoParamFabricQueue)) {
                    action = bcmFieldActionFabricQueue;
                    append = 1;
                    param0 = f_ent_wb_info->action_fabricQueue_wb.cosq_gport;
                }
                break;
                case 4:
                    action = bcmFieldActionL3ChangeMacDa;
                    if (PolicyGet(unit, mem, policy_entry, ECMPf)) {
                        ecmp_paths = PolicyGet(unit, mem, 
                                               policy_entry, ECMP_COUNTf) + 1;
                        nh_ecmp = PolicyGet(unit, mem, 
                                            policy_entry, ECMP_PTRf);
                        param0 = _FP_L3_ACTION_PACK_ECMP(nh_ecmp, ecmp_paths);
                    } else {
                        nh_ecmp = PolicyGet(unit, mem, 
                                            policy_entry, NEXT_HOP_INDEXf);
                        param0 = _FP_L3_ACTION_PACK_NEXT_HOP(nh_ecmp);
                    }
                    append = 1;

                    if (PolicyGet(unit, mem, policy_entry, ECMPf) == 0) {

                        /* Get the Next Hop Index value */
                        hw_index = PolicyGet(unit, mem, policy_entry,
                                        NEXT_HOP_INDEXf);

                        /* Read Next Hop information into entry */
                        rv = soc_mem_read(unit, EGR_L3_NEXT_HOPm,
                                MEM_BLOCK_ANY, hw_index,
                                egr_l3_next_hop_entry.entry_data);
                        if (BCM_FAILURE(rv)) {
                            return (rv);
                        }

                        /* Get the DA/DMAC infomation */
                        soc_mem_mac_addr_get(unit, EGR_L3_NEXT_HOPm,
                            egr_l3_next_hop_entry.entry_data,
                            MAC_ADDRESSf, mac_addr);
                        SAL_MAC_ADDR_TO_UINT32(mac_addr, mac_addr_words);

                        /* Re-Create bcmFieldActionDstMacNew action for the
                         * FP entry */
                        rv = _field_fb_actions_recover_action_add(unit,
                                f_ent, bcmFieldActionDstMacNew,
                                mac_addr_words[0], mac_addr_words[1], 0, 0,
                                hw_index);
                        if (BCM_FAILURE(rv)) {
                            return (rv);
                        }
                    }
                    break;
                case 5:
                    action = bcmFieldActionL3ChangeMacDaCancel;
                    append = 1;
                    break;
                case 6:
#ifdef INCLUDE_L3
                    action = bcmFieldActionL3Switch;
                    if (PolicyGet(unit, mem, policy_entry, ECMPf)) {
                        ecmp_paths = PolicyGet(unit, mem, 
                                               policy_entry, ECMP_COUNTf) + 1;
                        nh_ecmp = PolicyGet(unit, mem, 
                                            policy_entry, ECMP_PTRf);
                        param0 = (nh_ecmp) +
                            (BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit));
                    } else {
                        nh_ecmp = PolicyGet(unit, mem, 
                                            policy_entry, NEXT_HOP_INDEXf);
                        param0 = (nh_ecmp) +
                            (BCM_XGS3_EGRESS_IDX_MIN(unit));
                    }
                    append = 1;
#endif /* INCLUDE_L3 */
                    break;
                case 7:
                    action = bcmFieldActionL3SwitchCancel;
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case DROP_PRECEDENCEf:
            if (fldval) {
                action = bcmFieldActionDropPrecedence;
                param0 = fldval;
                append = 1;
            }
            break;
        case RP_DROP_PRECEDENCEf:
            if (fldval) {
                action = bcmFieldActionRpDropPrecedence;
                param0 = fldval;
                append = 1;
            }
            break;
        case YP_DROP_PRECEDENCEf:
            if (fldval) {
                action = bcmFieldActionYpDropPrecedence;
                param0 = fldval;
                append = 1;
            }
            break;
        case COUNTER_MODEf:
            if (fldval) {
                ctr_mode = fldval;
                ctr_idx = PolicyGet(unit, mem, policy_entry, 
                                    COUNTER_INDEXf);
                BCM_IF_ERROR_RETURN
                   (_field_fb_counter_recover(unit, f_ent, ctr_mode, 
                                               ctr_idx, part, f_ent_wb_info->sid));
            }
            break;
        case METER_PAIR_MODEf:
            meter_conf.meter_mode = fldval;
            meter_conf.meter_mode_modifier
                = soc_mem_field_valid(unit, mem, METER_PAIR_MODE_MODIFIERf)
                ? PolicyGet(unit, mem, policy_entry, METER_PAIR_MODE_MODIFIERf)
                : 0;
            meter_conf.meter_idx = PolicyGet(unit, mem, 
                                             policy_entry, 
                                             METER_INDEX_EVENf);
            meter_conf.meter_update_odd = PolicyGet(unit, mem, 
                                                    policy_entry, 
                                                    METER_UPDATE_ODDf);
            meter_conf.meter_test_odd = PolicyGet(unit, mem, 
                                                  policy_entry, 
                                                  METER_TEST_ODDf);
            meter_conf.meter_update_even = PolicyGet(unit, mem, 
                                                     policy_entry, 
                                                     METER_UPDATE_EVENf);
            meter_conf.meter_test_even = PolicyGet(unit, mem, 
                                                   policy_entry, 
                                                   METER_TEST_EVENf);
            BCM_IF_ERROR_RETURN
                (_field_fb_meter_recover(unit, f_ent, &meter_conf, part, f_ent_wb_info->pid));
            break;
        case PID_REPLACE_INNER_VIDf:
            if (fldval) {
                action = bcmFieldActionInnerVlanNew;
                param0 = PolicyGet(unit, mem, policy_entry, PID_NEW_INNER_VIDf);
                append = 1;
            }
            break;
        case PID_REPLACE_INNER_PRIf:
            if (fldval) {
                action = bcmFieldActionInnerVlanPrioNew;
                param0 = PolicyGet(unit, mem, policy_entry, PID_NEW_INNER_PRIf);
                append = 1;
            }
            break;
        case PID_REPLACE_OUTER_VIDf:
            if (fldval) {
                action = bcmFieldActionOuterVlanNew;
                param0 = PolicyGet(unit, mem, policy_entry, PID_NEW_OUTER_VIDf);
                append = 1;
            }
            break;
        case PID_REPLACE_OUTER_TPIDf:
            if (fldval) {
                uint32 mode;
                uint16 tpid;
                action = bcmFieldActionInnerVlanPrioNew;
                mode = PolicyGet(unit, mem, policy_entry, PID_TPID_INDEXf);
                BCM_IF_ERROR_RETURN
                    (_field_tpid_hw_decode(unit, mode, &tpid));
                param0 = tpid;
                append = 1;
            }
            break;
        case RP_CHANGE_DOT1Pf:
            if (fldval) {
                action = bcmFieldActionRpOuterVlanPrioNew;
                param0 = PolicyGet(unit, mem, policy_entry, RP_NEW_DOT1Pf);
                append = 1;
            }
            break;
        case YP_CHANGE_DOT1Pf:
            if (fldval) {
                action = bcmFieldActionYpOuterVlanPrioNew;
                param0 = PolicyGet(unit, mem, policy_entry, YP_NEW_DOT1Pf);
                append = 1;
            }
            break;
        case GP_CHANGE_DOT1Pf:
            if (fldval) {
                action = bcmFieldActionGpOuterVlanPrioNew;
                param0 = PolicyGet(unit, mem, policy_entry, GP_NEW_DOT1Pf);
                append = 1;
            }
            break;
        case PID_INCR_COUNTERf:
            if (fldval) {
                ctr_mode = fldval;
                ctr_idx = PolicyGet(unit, mem, policy_entry, 
                                    PID_COUNTER_INDEXf);
                BCM_IF_ERROR_RETURN
                   (_field_fb_counter_recover(unit, f_ent, ctr_mode, 
                                               ctr_idx, part, f_ent_wb_info->sid));
            }
            break;
        case CHANGE_VLANf:
            switch (fldval) {
                case 1:
                    action = bcmFieldActionVlanAdd;
                    param0 = PolicyGet(unit, mem, policy_entry, NEW_VLANf);
                    append = 1;
                    break;
                case 2:
                    action = bcmFieldActionOuterVlanNew;
                    param0 = PolicyGet(unit, mem, policy_entry, NEW_VLANf);
                    append = 1;
                    break;
                default:
                    break; /* Do nothing */
            }
            break;
        case USE_VFP_CLASS_IDf:
            if (fldval) {
                action = bcmFieldActionClassSet;
                param0 = PolicyGet(unit, mem, policy_entry, VFP_CLASS_IDf);
                append = 1;
            }
            break;
        case USE_VFP_VRF_IDf:
            if (fldval) {
                action = bcmFieldActionVrfSet;
                param0 = PolicyGet(unit, mem, policy_entry, VFP_VRF_IDf);
                append = 1;
            }
            break;
        default:
            break; /* Do nothing */
        }
        if (append) {
            rv = _field_action_alloc(unit, action,
                     param0, param1, 0, 0, 0, 0, &fa);
            fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            /* Add action to front of entry's linked-list. */
            fa->next = f_ent->actions;
            f_ent->actions  = fa;
        }
    }
    return BCM_E_NONE;
}

int 
_field_group_info_retrieve(int               unit,
                           bcm_port_t        port,
                           int               inst,
                           bcm_field_group_t *gid, 
                           int               *priority,
                           int               *action_res_id,
                           bcm_pbmp_t        *group_pbmp,
                           int8              *efp_key_info,
                           int               *group_flags,
                           bcm_field_qset_t  *qset,
                           _field_control_t  *fc
                           )
{

    _field_slice_group_info_t *current;
    current = fc->group_arr;
    while (current) {
        if (current->found) {
            *gid = -1; /* GID has already been copied */
            if(!(current = current->next)) {
                return BCM_E_NONE; /* Will detect it later */
            }
            continue;
        }
        if (((port == -1) || (BCM_PBMP_MEMBER(current->pbmp, port))) &&
            (current->instance == inst)) {
            *gid = current->gid;
            *priority = current->priority;
            *group_flags = current->flags;
            *action_res_id = current->action_res_id;
            if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_21) {
                SOC_PBMP_ASSIGN(*group_pbmp, current->pbmp);
            }

            if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit) ||
                        SOC_IS_TITAN(unit) || SOC_IS_TRIDENT(unit) ||
                        soc_feature(unit, soc_feature_field_multi_pipe_support)) &&
                    efp_key_info) {
                efp_key_info[0] = current->efp_pri_key;
                efp_key_info[1] = current->efp_sec_key;
            }
            sal_memcpy(qset, &current->qset, sizeof(bcm_field_qset_t));
            current->found = 1;
            return BCM_E_NONE;
        } else if (!current->next) {
            *gid = -1; /* Will detect it later */
            return BCM_E_NONE;
        }
        current = current->next;
    }
    return BCM_E_NOT_FOUND;
}

/* Retrieve all GIDs from a slice during Level 2 warm boot */
int
_field_scache_slice_group_recover(int unit, _field_control_t *fc, 
                                      int slice_num, int *multigroup)
{
    int i, slice_idx;
    int next_group_valid = 1, rv = BCM_E_NONE;
    int qset_count;
    int qset;
    bcm_field_group_t gid;
    bcm_port_t port;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    _field_slice_group_info_t *new_grp = NULL;
    int priority;
    int action_res_id;
    soc_pbmp_t group_pbmp;
    uint8  udf_count = 0;
    uint8 udf_idx = 0;

    /* Parse the scache buffer to recover GIDs and QSETs */
    /* To be used in stage reinit */
    slice_idx = buf[fc->scache_pos] >> 1;
    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "Read slice index %d @ byte %d\n"),
                 slice_idx, fc->scache_pos));
    if (multigroup) {
        *multigroup = buf[fc->scache_pos] & 0x1;
    }
    fc->scache_pos++;
    if (slice_idx != slice_num) {
        /* No groups stored for this slice */
        fc->scache_pos--;
        return BCM_E_NOT_FOUND;
    }
    fc->group_arr = NULL;

    /* Parse all the group data for this slice */
    while (next_group_valid) {
        SOC_PBMP_CLEAR(group_pbmp);
        gid = 0;
        if (fc->flags & _FP_STABLE_SAVE_LONG_IDS) {
            /* Read full GID */

            gid |= buf[fc->scache_pos];
            fc->scache_pos++;
            gid |= buf[fc->scache_pos] << 8;
            fc->scache_pos++;
            gid |= buf[fc->scache_pos] << 16;
            fc->scache_pos++;
            gid |= buf[fc->scache_pos] << 24;
            fc->scache_pos++;

            port = buf[fc->scache_pos];
            fc->scache_pos++;
        } else {
            /* Read compact GID */

            gid |= buf[fc->scache_pos]; /* GID */
            fc->scache_pos++;
            port = 0;
            port |= buf[fc->scache_pos] & 0xF; /* Rep_port_lo */
            fc->scache_pos++;
            port |= (buf[fc->scache_pos] & 0x3) << 4; /* Rep_port_hi */
            fc->scache_pos++;
        }

        LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "Read group id %d @ %d.\n"),
                     gid, fc->scache_pos - 3));
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_21) {
            int i = 0;
            for (i = 0; i < _SHR_PBMP_WORD_MAX; i++) {
                group_pbmp.pbits[i] = buf[fc->scache_pos];
                fc->scache_pos++;
                group_pbmp.pbits[i] |= buf[fc->scache_pos] << 8;
                fc->scache_pos++;
                group_pbmp.pbits[i] |= buf[fc->scache_pos] << 16;
                fc->scache_pos++;
                group_pbmp.pbits[i] |= buf[fc->scache_pos] << 24;
                fc->scache_pos++;
            }
        }
        /* Read group priority */
        priority = 0;
        priority |= buf[fc->scache_pos];
        fc->scache_pos++;
        priority |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        priority |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        priority |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;

        /* Read group action res id */
        action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_8) {
            action_res_id = 0;
            action_res_id |= buf[fc->scache_pos];
            fc->scache_pos++;
            action_res_id |= buf[fc->scache_pos] << 8;
            fc->scache_pos++;
            action_res_id |= buf[fc->scache_pos] << 16;
            fc->scache_pos++;
            action_res_id |= buf[fc->scache_pos] << 24;
            fc->scache_pos++;
        }

        /* Add group info to temp linked list */
        new_grp = NULL;
        _FP_XGS3_ALLOC(new_grp, sizeof(_field_slice_group_info_t), 
                       "Temp group info");
        new_grp->gid = gid;
        new_grp->priority = priority;
        new_grp->action_res_id = action_res_id;
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_21) {
            SOC_PBMP_ASSIGN(new_grp->pbmp, group_pbmp);
        }
        BCM_PBMP_PORT_ADD(new_grp->pbmp, port);
        new_grp->next = fc->group_arr;
        fc->group_arr = new_grp;
        qset_count = buf[fc->scache_pos]; /* QSET_count */
        fc->scache_pos++;

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "Before Read qset count %d @ s_pos=%d s_pos1=%d\n"),
                 qset_count, fc->scache_pos, fc->scache_pos1));

        for (i = 0; i < qset_count; i++) {
            qset = 0;
            qset |= buf[fc->scache_pos];
            fc->scache_pos++;
            if(NULL != buf1) {
                qset |= buf1[fc->scache_pos1] << 8;
                fc->scache_pos1++;
            } 
            BCM_FIELD_QSET_ADD(new_grp->qset, qset);

            LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "Read qualifier id %d @ %d.\n"),
                         qset, fc->scache_pos - 1));
        }

        LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                 "After Read qset count %d @ s_pos=%d s_pos1=%d\n"),
                 qset_count, fc->scache_pos, fc->scache_pos1));

        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_10) {
            qset_count = buf[fc->scache_pos]; /* Internal QSET_count */
            fc->scache_pos++;

            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "Internal Qset count %d @ s_pos=%d s_pos1=%d\n"),
                        qset_count, fc->scache_pos - 1, fc->scache_pos1));

            for (i = 0; i < qset_count; i++) {
                qset = 0;
                qset |= buf[fc->scache_pos];
                fc->scache_pos++;
                if(NULL != buf1) {
                    qset |= buf1[fc->scache_pos1] << 8;
                    fc->scache_pos1++;
                }
                qset += bcmFieldQualifyCount;
                BCM_FIELD_QSET_ADD(new_grp->qset, qset);

                LOG_VERBOSE(BSL_LS_BCM_FP,
                        (BSL_META_U(unit,
                                    "Read internal qualifier %d @ %d.\n"),
                         qset, fc->scache_pos - 1));
            }

            LOG_VERBOSE(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "After Read qset count %d @ s_pos=%d s_pos1=%d\n"),
                        qset_count, fc->scache_pos, fc->scache_pos1));

            udf_count = buf[fc->scache_pos];
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                            "Read udf count %d @ s_pos=%d\n"),
                        udf_count, fc->scache_pos));
            fc->scache_pos++;
            for (i = 0; i < udf_count; i++) {
                udf_idx = buf[fc->scache_pos];
                fc->scache_pos++;
                SHR_BITSET(new_grp->qset.udf_map, udf_idx);
                LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                                "Read UDF idx %d  @ byte %d.\n"),
                            udf_idx, fc->scache_pos - 1));
            }
        }

        /*
         * In this byte, 
         * bit 0 --  to indicate wheather next group 
         *           in this slice is valid or not 
         * bit 1,2 -- contains group's instance.
         * bit 5 --  to indicate group lookup enable status.
         * bit 6 --  to indicate group is WLAN.
         * bit 7 --  to indicate group auto expansion status.
         */

        next_group_valid =  buf[fc->scache_pos] & _FIELD_GROUP_NEXT_GROUP_VALID;

        if (buf[fc->scache_pos] & _FIELD_GROUP_AUTO_EXPANSION) {
            new_grp->flags = new_grp->flags | _FP_GROUP_SELECT_AUTO_EXPANSION;
        }

        if (buf[fc->scache_pos] & _FIELD_GROUP_LOOKUP_ENABLED) {
            new_grp->flags = new_grp->flags | _FP_GROUP_LOOKUP_ENABLED;
        }

        if (buf[fc->scache_pos] & _FIELD_GROUP_WLAN) {
            new_grp->flags = new_grp->flags | _FP_GROUP_WLAN;
        }

        new_grp->instance = ((buf[fc->scache_pos] >> 1) & 0x3);
        fc->scache_pos++;
    }
    /* The pointer has now advanced to the "entries" section */

    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "Done reading slice @ %d.\n"),
                 fc->scache_pos));
    return rv;
}

int
_field_entry_info_retrieve(int unit, _field_control_t *fc, _field_stage_t *stage_fc,
                           int multigroup, int *prev_prio,
                           _field_entry_wb_info_t *f_ent_wb_info)
{
    uint8 stat_present, pol_present, prio_ctrl;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8  part_index = 0;
    uint8  part_count = 0;
    _bcmFieldActionNoParam_t ingress_mirror_actions[] = {
                                    _bcmFieldActionNoParamMirrorIngress,
                                    _bcmFieldActionNoParamMirrorIngress1,
                                    _bcmFieldActionNoParamMirrorIngress2,
                                    _bcmFieldActionNoParamMirrorIngress3};
    _bcmFieldActionNoParam_t egress_mirror_actions[] = {
                                    _bcmFieldActionNoParamMirrorEgress,
                                    _bcmFieldActionNoParamMirrorEgress1,
                                    _bcmFieldActionNoParamMirrorEgress2,
                                    _bcmFieldActionNoParamMirrorEgress3};

    if (f_ent_wb_info == NULL) {
        return BCM_E_INTERNAL;
    }

    f_ent_wb_info->eid = f_ent_wb_info->sid = f_ent_wb_info->pid = 0;

    f_ent_wb_info->color_independent = (fc->flags & _FP_COLOR_INDEPENDENT) ? 1 : 0;

    if (fc->flags & _FP_STABLE_SAVE_LONG_IDS) {
        /* Read long EID and flags  */

        f_ent_wb_info->eid |=  buf[fc->scache_pos];
        fc->scache_pos++;
        f_ent_wb_info->eid |=  buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        f_ent_wb_info->eid |=  buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        f_ent_wb_info->eid |=  buf[fc->scache_pos] << 24;
        fc->scache_pos++;

        prio_ctrl    = buf[fc->scache_pos] & 1;
        stat_present = (buf[fc->scache_pos] >> 1) & 1;
        pol_present  = (buf[fc->scache_pos] >> 2) & 1;
        fc->scache_pos++;
    } else {
        /* Read compact EID and flags */

        f_ent_wb_info->eid |=  buf[fc->scache_pos]; /* EID_lo */
        fc->scache_pos++;
        f_ent_wb_info->eid |= (buf[fc->scache_pos] & 0xF) << 8; /* EID_hi */
        prio_ctrl = (buf[fc->scache_pos] >> 4) & 0x1;
        stat_present = (buf[fc->scache_pos] >> 5) & 0x1;
        pol_present = (buf[fc->scache_pos] >> 6) & 0x1;
        fc->scache_pos++;
    }

    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "Read entry id %d @ byte %d.\n"),
                 f_ent_wb_info->eid, fc->scache_pos - 2));

    if (multigroup) {
        fc->scache_pos += (fc->flags & _FP_STABLE_SAVE_LONG_IDS) ? 4 : 2; /* Skip over GID */
    }

    if (prio_ctrl) {
        f_ent_wb_info->prio = 0;
        f_ent_wb_info->prio |= buf[fc->scache_pos];
        fc->scache_pos++;
        f_ent_wb_info->prio |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        f_ent_wb_info->prio |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        f_ent_wb_info->prio |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        *prev_prio = f_ent_wb_info->prio;
    } else {
        f_ent_wb_info->prio = *prev_prio;
    }

    if (fc->flags & _FP_STABLE_SAVE_LONG_IDS) {
        /* Read long policer and stat ids, if present */

        if (pol_present) {
            f_ent_wb_info->pid |= buf[fc->scache_pos];
            fc->scache_pos++;
            f_ent_wb_info->pid |= buf[fc->scache_pos] << 8;
            fc->scache_pos++;
            f_ent_wb_info->pid |= buf[fc->scache_pos] << 16;
            fc->scache_pos++;
            f_ent_wb_info->pid |= buf[fc->scache_pos] << 24;
            fc->scache_pos++;
        }

        if (stat_present) {
            f_ent_wb_info->sid |= buf[fc->scache_pos];
            fc->scache_pos++;
            f_ent_wb_info->sid |= buf[fc->scache_pos] << 8;
            fc->scache_pos++;
            f_ent_wb_info->sid |= buf[fc->scache_pos] << 16;
            fc->scache_pos++;
            f_ent_wb_info->sid |= buf[fc->scache_pos] << 24;
            fc->scache_pos++;
        }
    } else {
        /* Read compact policer and stat ids, if present */

        if (pol_present) {
            f_ent_wb_info->pid |= buf[fc->scache_pos]; /* PID_lo */
            fc->scache_pos++;
        }
        if (stat_present) {
            f_ent_wb_info->sid |= buf[fc->scache_pos] & 0xF; /* SID_lo */
            if (pol_present) {
                f_ent_wb_info->pid |= (buf[fc->scache_pos] & 0xF0) << 4; /* PID_hi */
            }
            fc->scache_pos++;
            f_ent_wb_info->sid |= (buf[fc->scache_pos] << 4); /* SID_hi */
            fc->scache_pos++;
        } else if (pol_present) {
            f_ent_wb_info->pid |= (buf[fc->scache_pos] & 0xF0) << 4; /* PID_hi */
            fc->scache_pos++;
        }
    }

    /* Retrieve Entry part count */
    if (((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_18) &&
            ((stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) ||
             (stage_fc->stage_id == _BCM_FIELD_STAGE_EGRESS) ||
             (stage_fc->stage_id == _BCM_FIELD_STAGE_LOOKUP))) {
        part_count = buf[fc->scache_pos];
        fc->scache_pos++;
    } else {
        part_count = 0;
    }

    /* Retrieve DVP_tpe */
    if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_6) {
        f_ent_wb_info->dvp_type[0] =  buf[fc->scache_pos];
        fc->scache_pos++;
        part_index = 1;
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_18) {
            for (; part_index < part_count; part_index++) {
                f_ent_wb_info->dvp_type[part_index] =  buf[fc->scache_pos];
                fc->scache_pos++;
            }
        }
        for (; part_index < _FP_MAX_ENTRY_WIDTH; part_index++) {
            f_ent_wb_info->dvp_type[part_index] = f_ent_wb_info->dvp_type[0];
        }
    }

    /* Retrieve SVP_type */
    if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_16) {
        f_ent_wb_info->svp_type[0] =  buf[fc->scache_pos];
        fc->scache_pos++;
        part_index = 1;
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_18) {
            for (; part_index < part_count; part_index++) {
                f_ent_wb_info->svp_type[part_index] =  buf[fc->scache_pos];
                fc->scache_pos++;
            }
        }
        for (; part_index < _FP_MAX_ENTRY_WIDTH; part_index++) {
            f_ent_wb_info->svp_type[part_index] = f_ent_wb_info->svp_type[0];
        }
    }

    /*Retrieve Color Indpendent info*/
    if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_13) {
        f_ent_wb_info->color_independent = (buf[fc->scache_pos] >> 7) & 1;
        fc->scache_pos++;
    }

    /* Retrieve Action bitmap */
    if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
          if(f_ent_wb_info->action_bmp->w == NULL) {
             return BCM_E_PARAM;
          }

          LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
                "Read action bitmap[0x%x] @ byte %d.\n"),
                buf[fc->scache_pos], fc->scache_pos));

          f_ent_wb_info->action_bmp->w[0] = buf[fc->scache_pos];
          fc->scache_pos++;
          f_ent_wb_info->action_bmp->w[0] |= buf[fc->scache_pos] << 8;
          fc->scache_pos++;
          f_ent_wb_info->action_bmp->w[0] |= buf[fc->scache_pos] << 16;
          fc->scache_pos++;
          f_ent_wb_info->action_bmp->w[0] |= buf[fc->scache_pos] << 24;
          fc->scache_pos++;
          if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_19) {
             f_ent_wb_info->action_bmp->w[1] = buf[fc->scache_pos];
             fc->scache_pos++;
             f_ent_wb_info->action_bmp->w[1] |= buf[fc->scache_pos] << 8;
             fc->scache_pos++;
             f_ent_wb_info->action_bmp->w[1] |= buf[fc->scache_pos] << 16;
             fc->scache_pos++;
             f_ent_wb_info->action_bmp->w[1] |= buf[fc->scache_pos] << 24;
             fc->scache_pos++;
          }
    
          if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_16) {
             /* Recover IngressGportType Entry Action. */
             f_ent_wb_info->ing_gport_type = buf[fc->scache_pos];
             fc->scache_pos++;
          }

          if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_20) {
              uint8 index, max_index = 1;
              if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_21) {
                  max_index = 4;
              }
              for (index = 0 ; index < max_index ; index++) {
                  if (SHR_BITGET(f_ent_wb_info->action_bmp->w, ingress_mirror_actions[index])) {
                      f_ent_wb_info->ingress_mirror[index] = buf[fc->scache_pos];
                      fc->scache_pos++;
                      f_ent_wb_info->ingress_mirror[index] |= buf[fc->scache_pos] << 8;
                      fc->scache_pos++;
                      f_ent_wb_info->ingress_mirror[index] |= buf[fc->scache_pos] << 16;
                      fc->scache_pos++;
                      f_ent_wb_info->ingress_mirror[index] |= buf[fc->scache_pos] << 24;
                      fc->scache_pos++;
                  }
              }
              for (index = 0 ; index < max_index ; index++) {
                  if (SHR_BITGET(f_ent_wb_info->action_bmp->w, egress_mirror_actions[index])) {
                      f_ent_wb_info->egress_mirror[index] = buf[fc->scache_pos];
                      fc->scache_pos++;
                      f_ent_wb_info->egress_mirror[index] |= buf[fc->scache_pos] << 8;
                      fc->scache_pos++;
                      f_ent_wb_info->egress_mirror[index] |= buf[fc->scache_pos] << 16;
                      fc->scache_pos++;
                      f_ent_wb_info->egress_mirror[index] |= buf[fc->scache_pos] << 24;
                      fc->scache_pos++;
                  }
              }
              if ( SHR_BITGET(f_ent_wb_info->action_bmp->w,
                    _bcmFieldActionNoParamFabricQueue)) {
                  (f_ent_wb_info->action_fabricQueue_wb.cosq_gport) =
                        buf[fc->scache_pos];
                  fc->scache_pos++;
                  (f_ent_wb_info->action_fabricQueue_wb.cosq_gport) |=
                        buf[fc->scache_pos] << 8;
                  fc->scache_pos++;
                  (f_ent_wb_info->action_fabricQueue_wb.cosq_gport) |=
                        buf[fc->scache_pos] << 16;
                  fc->scache_pos++;
                  (f_ent_wb_info->action_fabricQueue_wb.cosq_gport) |=
                        buf[fc->scache_pos] << 24;
                  fc->scache_pos++;
              }
          }
          if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_31) {
             f_ent_wb_info->action_bmp->w[2] = buf[fc->scache_pos];
             fc->scache_pos++;
             f_ent_wb_info->action_bmp->w[2] |= buf[fc->scache_pos] << 8;
             fc->scache_pos++;
             f_ent_wb_info->action_bmp->w[2] |= buf[fc->scache_pos] << 16;
             fc->scache_pos++;
             f_ent_wb_info->action_bmp->w[2] |= buf[fc->scache_pos] << 24;
             fc->scache_pos++;
          }
    
    }  

    LOG_VERBOSE(BSL_LS_BCM_FP,
                (BSL_META_U(unit,
                            "Done reading entry @ %d.\n"),
                 fc->scache_pos));

    return BCM_E_NONE;
}

/* Free the slice-specific group information after we're done with it */
void
_field_scache_slice_group_free(int unit, _field_control_t *fc, 
                                   int slice_num)
{
    _field_slice_group_info_t *current = NULL;
    current = fc->group_arr;
    while (current) {
        fc->group_arr = current->next;
        sal_free(current);
        current = fc->group_arr;
    }
    fc->group_arr = NULL;
    return;
}

#if defined(BCM_FIREBOLT2_SUPPORT)
STATIC int 
_field_fb2_group_construct(int unit, _field_hw_qual_info_t *hw_sel, 
                           int intraslice, int paired, _field_control_t *fc,
                           bcm_port_t port, _field_stage_id_t stage_id,
                           int slice_idx)
{
    int i, j, k, idx, size, selector_match, parts_count = 1, rv = BCM_E_NONE;
    _field_group_t *fg_ptr = fc->groups;
    bcm_field_group_t gid;
    int               priority;
    int               group_flags;
    _bcm_field_qual_info_t *f_qual_arr = NULL;
    _field_stage_t *stage_fc;
    _bcm_field_qual_offset_t *q_offset_arr = NULL;
    uint16 *qid_arr = NULL;
    bcm_field_qset_t qset;
    int action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    bcm_pbmp_t group_pbmp ;

    bcm_field_qset_t_init(&qset);

    /* Get the stage control structure */
    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    /* Iterate over groups and compare all selectors - IFP */
    fg_ptr = fc->groups;
    while (fg_ptr != NULL) {
        if (intraslice && !(fg_ptr->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
            fg_ptr = fg_ptr->next;
            continue;
        }
        if (!intraslice && (fg_ptr->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
            fg_ptr = fg_ptr->next;
            continue;
        }
        if (paired && (fg_ptr->flags & _FP_GROUP_SPAN_SINGLE_SLICE)) {
            fg_ptr = fg_ptr->next;
            continue;
        } 
        if (!paired && (fg_ptr->flags & _FP_GROUP_SPAN_DOUBLE_SLICE)) {
            fg_ptr = fg_ptr->next;
            continue;
        }
        /* Check for primary slice numbers - there may be groups with the same
           QSET but residing in different slices */
        if (slice_idx != fg_ptr->slices[0].slice_number) {
            fg_ptr = fg_ptr->next;
            continue;
        }
        /* Compare primary slice selectors */
        if ((fg_ptr->sel_codes[0].fpf1 != hw_sel->pri_slice[0].fpf1) ||
            (fg_ptr->sel_codes[0].fpf2 != hw_sel->pri_slice[0].fpf2) ||
            (fg_ptr->sel_codes[0].fpf3 != hw_sel->pri_slice[0].fpf3) ||
            (fg_ptr->sel_codes[0].fpf4 != hw_sel->pri_slice[0].fpf4)) {
            fg_ptr = fg_ptr->next;
            continue;
        }
        if (intraslice && (fg_ptr->sel_codes[1].fpf2 != 
            hw_sel->pri_slice[1].fpf2)) {
            fg_ptr = fg_ptr->next;
            continue;
        }
        /* Compare secondary slice if paired */
        if (paired) {
            if ((fg_ptr->sel_codes[2].fpf1 != hw_sel->sec_slice[0].fpf1) ||
                (fg_ptr->sel_codes[2].fpf2 != hw_sel->sec_slice[0].fpf2) ||
                (fg_ptr->sel_codes[2].fpf3 != hw_sel->sec_slice[0].fpf3) ||
                (fg_ptr->sel_codes[2].fpf4 != hw_sel->sec_slice[0].fpf4)) {
                fg_ptr = fg_ptr->next;
                continue;
            }
            if (intraslice && (fg_ptr->sel_codes[3].fpf2 != 
                hw_sel->sec_slice[1].fpf2)) {
                fg_ptr = fg_ptr->next;
                continue;
            }
        }

        /* Found a match - add port to group pbmp */
        BCM_PBMP_PORT_ADD(fg_ptr->pbmp, port);
        BCM_PBMP_OR(fg_ptr->slices[0].pbmp, fg_ptr->pbmp);
        if (paired) {
            BCM_PBMP_OR(fg_ptr->slices[1].pbmp, fg_ptr->pbmp);
        }

        group_flags = 0;
        if (fc->l2warm) {
            /* Get stored group ID and QSET for Level 2 */
            BCM_IF_ERROR_RETURN
                (_field_group_info_retrieve(unit, port, _FP_DEF_INST, &gid,
                                            &priority, &action_res_id, &group_pbmp, NULL,
                                            &group_flags, &qset, fc));
            if (gid != -1) {
                sal_memcpy(&fg_ptr->qset, &qset, sizeof(bcm_field_qset_t));
                fg_ptr->gid      = gid;
                fg_ptr->priority = priority;
                fg_ptr->action_res_id = action_res_id;
                if (group_flags & _FP_GROUP_SELECT_AUTO_EXPANSION) {
                    fg_ptr->flags = fg_ptr->flags | 
                                         _FP_GROUP_SELECT_AUTO_EXPANSION;
                }

            }
        }
        break;
    }
    if (fg_ptr == NULL) {
        /* No match - create a new group and populate the group structure */
        _FP_XGS3_ALLOC(fg_ptr, sizeof(_field_group_t), "field group");
        if (NULL == fg_ptr) {
            return BCM_E_MEMORY;
        }

        /* Initialize action res id and VMAP group to default */
        action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
        for (idx = 0; idx < _FP_PAIR_MAX; idx++) {
            fg_ptr->vmap_group[idx] = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
        }

        group_flags = 0;
        if (fc->l2warm) {
            /* Get stored group ID and QSET for Level 2 */
            rv = _field_group_info_retrieve(unit, port, _FP_DEF_INST, &gid,
                                            &priority, &action_res_id, &group_pbmp, NULL,
                                            &group_flags, &qset, fc);
            if (gid != -1) {
                sal_memcpy(&fg_ptr->qset, &qset, sizeof(bcm_field_qset_t));
            }
            else {
                sal_free(fg_ptr);
                return rv;
            }
        } else { 
            /* Generate group ID (a ++ operation) for Level 1 */
            rv = _bcm_field_group_id_generate(unit, &gid);
            priority = slice_idx;
        }
        if (BCM_FAILURE(rv)) {
            sal_free(fg_ptr);
            return rv;
        }
        fg_ptr->gid = gid;
        fg_ptr->priority = priority;
        fg_ptr->action_res_id = action_res_id;
        if (group_flags & _FP_GROUP_SELECT_AUTO_EXPANSION) {
            fg_ptr->flags = fg_ptr->flags | _FP_GROUP_SELECT_AUTO_EXPANSION;
        }
        fg_ptr->stage_id = stage_id;
        fg_ptr->sel_codes[0].fpf1 = hw_sel->pri_slice[0].fpf1;
        fg_ptr->sel_codes[0].fpf2 = hw_sel->pri_slice[0].fpf2;
        fg_ptr->sel_codes[0].fpf3 = hw_sel->pri_slice[0].fpf3;
        fg_ptr->sel_codes[0].fpf4 = hw_sel->pri_slice[0].fpf4;
        fg_ptr->sel_codes[0].inner_vlan_overlay = 
            hw_sel->pri_slice[0].inner_vlan_overlay;
        if (port != -1) {
            BCM_PBMP_PORT_ADD(fg_ptr->pbmp, port);
        } else {
            SOC_PBMP_ASSIGN(fg_ptr->pbmp, PBMP_ALL(unit));
        }
        if (intraslice) {
            fg_ptr->flags |= _FP_GROUP_INTRASLICE_DOUBLEWIDE;
            fg_ptr->sel_codes[1].fpf2 = hw_sel->pri_slice[1].fpf2;
            fg_ptr->sel_codes[1].fpf4 = hw_sel->pri_slice[1].fpf4;
            fg_ptr->sel_codes[1].intraslice = 1;
            fg_ptr->sel_codes[1].inner_vlan_overlay = 
                hw_sel->pri_slice[1].inner_vlan_overlay;
        }
        if (paired) {
            parts_count = 2;
            fg_ptr->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
            fg_ptr->sel_codes[2].fpf1 = hw_sel->sec_slice[0].fpf1;
            fg_ptr->sel_codes[2].fpf2 = hw_sel->sec_slice[0].fpf2;
            fg_ptr->sel_codes[2].fpf3 = hw_sel->sec_slice[0].fpf3;
            fg_ptr->sel_codes[2].fpf4 = hw_sel->sec_slice[0].fpf4;
            fg_ptr->sel_codes[2].inner_vlan_overlay = 
                hw_sel->sec_slice[0].inner_vlan_overlay;
            if (intraslice) {
                parts_count = 4;
                fg_ptr->sel_codes[3].fpf2 = hw_sel->sec_slice[1].fpf2;
                fg_ptr->sel_codes[3].fpf4 = hw_sel->sec_slice[1].fpf4;
                fg_ptr->sel_codes[3].intraslice = 1;
                fg_ptr->sel_codes[3].inner_vlan_overlay = 
                    hw_sel->sec_slice[1].inner_vlan_overlay;
            }
        } else {
            fg_ptr->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
        }
        /* Determine the QSET based on selector values */
        for (i = 0; i < parts_count; i++) {
            for (idx = 0; idx < _bcmFieldQualifyCount; idx++) {
                f_qual_arr = stage_fc->f_qual_arr[idx];
                if (f_qual_arr == NULL) {
                    continue; /* Qualifier does not exist in this stage */
                }
                if (fc->l2warm && !BCM_FIELD_QSET_TEST(fg_ptr->qset, idx)) {
                    continue; /* Qualifier not present in the group */
                }
                /* Go over the API level qualifier's configuration info */
                for (j = 0; j < f_qual_arr->conf_sz; j++) {
                    selector_match = 0;
                    /* Check if it matches F1's value */
                    if ((f_qual_arr->conf_arr[j].selector.pri_sel == 
                         _bcmFieldSliceSelFpf1) && 
                        (f_qual_arr->conf_arr[j].selector.pri_sel_val == 
                         fg_ptr->sel_codes[i].fpf1)) {
                        /* Qualifier found - add to group's qset and group
                           qualifier info */
                        /* Tentatively add to fg->qual_arr[i] */
                        if (stage_id == _BCM_FIELD_STAGE_INGRESS) {
                            if ((fg_ptr->sel_codes[i].fpf1 != 3) && 
                                (fg_ptr->sel_codes[i].fpf1 != 6)) {
                                selector_match = 1; 
                            } else {
                                /* Deal with inner_vlan_overlay */
                                if ((idx == bcmFieldQualifyInnerVlan) || 
                                    (idx == bcmFieldQualifyInnerVlanId) ||
                                    (idx == bcmFieldQualifyInnerVlanPri) ||
                                    (idx == bcmFieldQualifyInnerVlanCfi)) {
                                    if (fg_ptr->sel_codes[i].inner_vlan_overlay
                                        == 0) {
                                        selector_match = 1; 
                                    }
                                } else if ((idx == bcmFieldQualifyVrf) ||
                                     (idx == bcmFieldQualifyDstClassField) ||
                                     (idx == bcmFieldQualifySrcClassL2)) {
                                    if (fg_ptr->sel_codes[i].inner_vlan_overlay
                                        == 1) {
                                        selector_match = 1; 
                                    }
                                } else {
                                    selector_match = 1; 
                                }
                            }
                        } else {
                            selector_match = 1; 
                        }
                    }
                    /* Check if it matches F2's value */
                    if ((f_qual_arr->conf_arr[j].selector.pri_sel == 
                         _bcmFieldSliceSelFpf2) && 
                        (f_qual_arr->conf_arr[j].selector.pri_sel_val == 
                         fg_ptr->sel_codes[i].fpf2)) {
                        /* Qualifier found - add to group's qset and group
                           qualifier info */
                        /* Tentatively add to fg->qual_arr[i] */
                        selector_match = 1; 
                    }
                    /* Check if it matches F3's value */
                    if ((f_qual_arr->conf_arr[j].selector.pri_sel == 
                         _bcmFieldSliceSelFpf3) && 
                        (f_qual_arr->conf_arr[j].selector.pri_sel_val == 
                         fg_ptr->sel_codes[i].fpf3)) {
                        /* Qualifier found - add to group's qset and group
                           qualifier info */
                        /* Tentatively add to fg->qual_arr[i] */
                        selector_match = 1; 
                    }
                    /* Check if it matches F4's value */
                    if ((f_qual_arr->conf_arr[j].selector.pri_sel == 
                         _bcmFieldSliceSelFpf4) && 
                        (f_qual_arr->conf_arr[j].selector.pri_sel_val == 
                         fg_ptr->sel_codes[i].fpf4)) {
                        /* Qualifier found - add to group's qset and group
                           qualifier info */
                        /* Tentatively add to fg->qual_arr[i] */
                        selector_match = 1; 
                    }
                    if (selector_match) {
                        BCM_FIELD_QSET_ADD(fg_ptr->qset, idx);
                        /* Allocate memory for this qualfier and its
                           associated offset information if not already added 
                           to fg->qual_arr[i] */
                        /* May change later depending on the mask */
                        for (k = 0; k < fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size; k++) {
                            if (fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[k] == idx) {
                                break; /* Qualifier already added */
                            }
                        }
                        if (k == fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size) {
                            size = fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size + 1;
                            qid_arr = NULL;
                            _FP_XGS3_ALLOC(qid_arr, size * sizeof (uint16), 
                                           "Group qual id");
                            if (NULL == qid_arr) {
                                sal_free (fg_ptr);
                                return (BCM_E_MEMORY);
                            }              
                            q_offset_arr = NULL;              
                            _FP_XGS3_ALLOC(q_offset_arr, size * 
                                           sizeof (_bcm_field_qual_offset_t), 
                                           "Group qual offset");
                            if (NULL == q_offset_arr) {
                                sal_free (fg_ptr);
                                sal_free (qid_arr);
                                return (BCM_E_MEMORY);
                            }
                            if (size) {
                                sal_memcpy(qid_arr, fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr,
                                           fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size * 
                                           sizeof (uint16));
                                sal_memcpy(q_offset_arr, 
                                           fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr, 
                                           fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size * 
                                           sizeof (_bcm_field_qual_offset_t));
                                if (fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size) {
                                    sal_free(fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr);
                                    sal_free(fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr);
                                }
                            }
                            fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr = qid_arr;
                            fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr = q_offset_arr;
                            fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[size - 1] = idx;
                            fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[size - 1].field = 
                                f_qual_arr->conf_arr[j].offset.field;

                            sal_memcpy(&fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[size - 1],
                                &f_qual_arr->conf_arr[j].offset, sizeof(f_qual_arr->conf_arr[j].offset));

                            fg_ptr->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size = size;

                            _field_qset_udf_bmap_reinit(unit,
                                                        stage_fc,
                                                        &fg_ptr->qset,
                                                        idx
                                                        );

                            break; /* Just added the qualifier */
                        }
                    }
                }
            }
        }
        /* Associate slice(s) to group */
        if (fg_ptr->slices == NULL) {
            fg_ptr->slices = stage_fc->slices[_FP_DEF_INST] + slice_idx;
            if (intraslice) {
                fg_ptr->slices->group_flags |= _FP_GROUP_INTRASLICE_DOUBLEWIDE;
                if (paired) {
                    fg_ptr->slices[1].group_flags |= 
                                               _FP_GROUP_INTRASLICE_DOUBLEWIDE;
                }
            }
        }

        BCM_PBMP_OR(fg_ptr->slices[0].pbmp, fg_ptr->pbmp);
        if (paired) {
            BCM_PBMP_OR(fg_ptr->slices[1].pbmp, fg_ptr->pbmp);
        }

        /* Initialize group default ASET list. */
        rv = _field_group_default_aset_set(unit, fg_ptr);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        if (fc->l2warm
                && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15)) {
            if (group_flags & _FP_GROUP_LOOKUP_ENABLED) {
                fg_ptr->flags = fg_ptr->flags | _FP_GROUP_LOOKUP_ENABLED;
            }
            if (group_flags & _FP_GROUP_WLAN) {
                fg_ptr->flags = fg_ptr->flags | _FP_GROUP_WLAN;
            }
        } else {
            fg_ptr->flags |= _FP_GROUP_LOOKUP_ENABLED;
        }

        fg_ptr->next = fc->groups;
        fc->groups = fg_ptr;
    }
    return rv;
}

int
_field_fb2_stage_ingress_reinit(int unit, _field_control_t *fc, 
                                _field_stage_t *stage_fc)
{
    int idx, idx1, slice_idx, vslice_idx,index_min, index_max, ratio, rv = BCM_E_NONE;
    int entries_per_slice, group_found, mem_sz, parts_count, qual_idx;
    int i, j, k, pri_tcam_idx, part_index, slice_number, expanded[16], slice_master_idx[16];
    int prev_prio, multigroup, max, master_slice;
    char *fp_tcam_buf = NULL; /* Buffer to read the FP_TCAM table */
    char *fp_pfs_buf = NULL; /* Buffer to read the FP_PORT_FIELD_SEL table */
    char *fp_policy_buf = NULL; /* Buffer to read the FP_POLICY table */
    uint32 pbm, rval, paired, intraslice, dbl_wide_key, dbl_wide_key_sec;
    uint32 f4_sel_val, qualified, inner_vlan_overlay;
    uint32 *tcam_entry_buf, temp;
    soc_field_t fld;
    bcm_port_t port;
    fp_port_field_sel_entry_t *pfs_entry;
    fp_tcam_entry_t *tcam_entry;
    fp_policy_table_entry_t *policy_entry;
    _field_hw_qual_info_t hw_sels;
    _field_slice_t *fs;
    _field_group_t *fg = NULL, *group = NULL;
    _field_entry_t *f_ent = NULL;
    bcm_pbmp_t entry_pbmp, temp_pbmp;
    _bcm_field_qual_info_t *f_qual_arr = NULL;
    _field_sel_t *sel_ptr;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    uint8 old_physical_slice, slice_num;
    uint32 entry_flags;
    _field_slice_t *fs_temp = NULL;
    _field_action_bmp_t action_bmp;
    _field_entry_wb_info_t f_ent_wb_info;

    soc_field_t en_flds[16] = 
                     {FP_SLICE_ENABLE_SLICE_0f, FP_SLICE_ENABLE_SLICE_1f,
                      FP_SLICE_ENABLE_SLICE_2f, FP_SLICE_ENABLE_SLICE_3f,
                      FP_SLICE_ENABLE_SLICE_4f, FP_SLICE_ENABLE_SLICE_5f,
                      FP_SLICE_ENABLE_SLICE_6f, FP_SLICE_ENABLE_SLICE_7f,
                      FP_SLICE_ENABLE_SLICE_8f, FP_SLICE_ENABLE_SLICE_9f,
                      FP_SLICE_ENABLE_SLICE_10f, FP_SLICE_ENABLE_SLICE_11f,
                      FP_SLICE_ENABLE_SLICE_12f, FP_SLICE_ENABLE_SLICE_13f,
                      FP_SLICE_ENABLE_SLICE_14f, FP_SLICE_ENABLE_SLICE_15f};
    soc_field_t lk_en_flds[16] = 
                     {FP_LOOKUP_ENABLE_SLICE_0f, FP_LOOKUP_ENABLE_SLICE_1f,
                      FP_LOOKUP_ENABLE_SLICE_2f, FP_LOOKUP_ENABLE_SLICE_3f,
                      FP_LOOKUP_ENABLE_SLICE_4f, FP_LOOKUP_ENABLE_SLICE_5f,
                      FP_LOOKUP_ENABLE_SLICE_6f, FP_LOOKUP_ENABLE_SLICE_7f,
                      FP_LOOKUP_ENABLE_SLICE_8f, FP_LOOKUP_ENABLE_SLICE_9f,
                      FP_LOOKUP_ENABLE_SLICE_10f, FP_LOOKUP_ENABLE_SLICE_11f,
                      FP_LOOKUP_ENABLE_SLICE_12f, FP_LOOKUP_ENABLE_SLICE_13f,
                      FP_LOOKUP_ENABLE_SLICE_14f, FP_LOOKUP_ENABLE_SLICE_15f};

    
    fc->scache_pos = 0;
    fc->scache_pos1 = 0;

    /* Reset Action bitmap to NULL. */
    action_bmp.w = NULL;

    if (fc->l2warm) {
        if ((fc->wb_recovered_version) > (fc->wb_current_version)) {
            /* Notify the application with an event */
            /* The application will then need to reconcile the 
               version differences using the documented behavioral
               differences on per module (handle) basis */
            SOC_IF_ERROR_RETURN
                (soc_event_generate(unit, SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE, 
                                    BCM_MODULE_FIELD, (fc->wb_recovered_version), 
                                    (fc->wb_current_version)));        
        }

        fc->scache_pos += SOC_WB_SCACHE_CONTROL_SIZE;

        if(NULL != fc->scache_ptr[_FIELD_SCACHE_PART_1]) {
            if ((fc->wb_recovered_version) > (fc->wb_current_version)) {
                /* Notify the application with an event */
                /* The application will then need to reconcile the
                   version differences using the documented behavioral
                   differences on per module (handle) basis */
                SOC_IF_ERROR_RETURN
                    (soc_event_generate(unit,
                                     SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
                                     BCM_MODULE_FIELD, (fc->wb_recovered_version),
                                     (fc->wb_current_version)));
            }

            fc->scache_pos1 += SOC_WB_SCACHE_CONTROL_SIZE;
        }


    }

    SOC_PBMP_CLEAR(entry_pbmp);
    sal_memset(expanded, 0, 16 * sizeof(int));
    sal_memset(slice_master_idx, 0, 16 * sizeof(int));

    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_IFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* DMA various tables */
    fp_tcam_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, FP_TCAMm), 
                                "FP TCAM buffer");
    if (NULL == fp_tcam_buf) {
        return BCM_E_MEMORY;
    }
    index_min = soc_mem_index_min(unit, FP_TCAMm);
    index_max = soc_mem_index_max(unit, FP_TCAMm);
    if ((rv = soc_mem_read_range(unit, FP_TCAMm, MEM_BLOCK_ALL,
                                 index_min, index_max, fp_tcam_buf)) < 0 ) {
        goto cleanup;
    }
    fp_pfs_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES(unit, 
                               FP_PORT_FIELD_SELm), 
                               "FP PORT_FIELD_SEL buffer");
    if (NULL == fp_pfs_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, FP_PORT_FIELD_SELm);
    index_max = soc_mem_index_max(unit, FP_PORT_FIELD_SELm);
    if ((rv = soc_mem_read_range(unit, FP_PORT_FIELD_SELm, MEM_BLOCK_ALL,
                                 index_min, index_max, fp_pfs_buf)) < 0 ) {
        goto cleanup;
    }
    fp_policy_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                  (unit, FP_POLICY_TABLEm), 
                                  "FP POLICY TABLE buffer");
    if (NULL == fp_policy_buf) {
        rv = BCM_E_MEMORY;
        goto cleanup;
    }
    index_min = soc_mem_index_min(unit, FP_POLICY_TABLEm);
    index_max = soc_mem_index_max(unit, FP_POLICY_TABLEm);
    if ((rv = soc_mem_read_range(unit, FP_POLICY_TABLEm, MEM_BLOCK_ALL,
                               index_min, index_max, fp_policy_buf)) < 0 ) {
        goto cleanup;
    }

    entries_per_slice = 256;

    if ((rv = READ_FP_F4_SELECTr(unit, &f4_sel_val)) < 0) {
        goto cleanup;
    }

    /* Recover range checkers */
    rv = _field_range_check_reinit(unit, stage_fc, fc);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Recover data qualifiers */
    rv = _field_data_qual_recover(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Get per-device selectors */
    if ((rv = READ_ING_MISC_CONFIG2r(unit, &rval)) < 0) {
        goto cleanup;
    }
    inner_vlan_overlay = soc_reg_field_get(unit, ING_MISC_CONFIG2r, rval, 
                                           FP_INNER_VLAN_OVERLAY_ENABLEf);

    /* Get slice expansion status and virtual map */
    if ((rv = _field_slice_expanded_status_get(unit, fc, stage_fc,
                                               expanded, slice_master_idx)) < 0) {
        goto cleanup;
    }

    /* Iterate over the slices */
    if ((rv = READ_FP_SLICE_ENABLEr(unit, &rval)) < 0) {
        goto cleanup;
    }
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        /* Ignore disabled slice */
        if ((soc_reg_field_get
             (unit, FP_SLICE_ENABLEr, rval, en_flds[slice_idx]) == 0) || 
            (soc_reg_field_get
             (unit, FP_SLICE_ENABLEr, rval, lk_en_flds[slice_idx]) == 0)) {
            continue;
        }
        /* Ignore secondary slice in paired mode */
        pfs_entry = soc_mem_table_idx_to_pointer(unit, FP_PORT_FIELD_SELm, 
                                                fp_port_field_sel_entry_t *, 
                                                fp_pfs_buf, 0);
        fld = _fb2_slice_pairing_field[slice_idx / 2];
        paired = soc_FP_PORT_FIELD_SELm_field32_get(unit, 
                                                    pfs_entry, fld);
        fld = _fb2_slice_wide_mode_field[slice_idx];
        intraslice = soc_FP_PORT_FIELD_SELm_field32_get(unit, 
                                                        pfs_entry, fld);
        if (paired && (slice_idx % 2)) {
            continue;
        }

        /* Don't need to read selectors for expanded slice */
        if (expanded[slice_idx]) {
            continue; 
        }

        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        for (idx = 0; idx < fs->entry_count; idx++) {
            tcam_entry = soc_mem_table_idx_to_pointer(unit, FP_TCAMm, 
                                                      fp_tcam_entry_t *, 
                                                      fp_tcam_buf, idx + 
                                                      entries_per_slice * 
                                                      slice_idx);
            if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) != 0) {
                break;
            }
        }
        if ((idx == fs->entry_count) && !fc->l2warm) {
            continue;
        }

        /* If Level 2, retrieve the GIDs in this slice */
        if (fc->l2warm) {
            rv = _field_scache_slice_group_recover(unit, fc, slice_idx, 
                                                       &multigroup);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }

        /* Iterate over FP_PORT_FIELD_SEL for all ports and this slice 
         * to identify bins for selectors (also groups for level 1) */
        PBMP_ALL_ITER(unit, port) {
            sal_memset(&hw_sels, 0, sizeof(_field_hw_qual_info_t));
            pfs_entry = soc_mem_table_idx_to_pointer
                            (unit, FP_PORT_FIELD_SELm, 
                             fp_port_field_sel_entry_t *, fp_pfs_buf, port);
            /* Get primary slice's selectors */
            hw_sels.pri_slice[0].fpf1 = soc_FP_PORT_FIELD_SELm_field32_get(unit,  
                                     pfs_entry, _fb_field_tbl[slice_idx][0]);
            hw_sels.pri_slice[0].fpf2 = soc_FP_PORT_FIELD_SELm_field32_get(unit,  
                                     pfs_entry, _fb_field_tbl[slice_idx][1]);
            hw_sels.pri_slice[0].fpf3 = soc_FP_PORT_FIELD_SELm_field32_get(unit,  
                                     pfs_entry, _fb_field_tbl[slice_idx][2]);
            hw_sels.pri_slice[0].fpf4 = soc_reg_field_get(unit, 
                                            FP_F4_SELECTr, f4_sel_val, 
                                            _fb_ing_f4_reg[slice_idx]);
            hw_sels.pri_slice[0].inner_vlan_overlay = inner_vlan_overlay;
            /* If intraslice, get double-wide key - only 2 options */
            if (intraslice) {
                dbl_wide_key = soc_FP_PORT_FIELD_SELm_field32_get(unit, 
                            pfs_entry, _fb2_ifp_double_wide_key[slice_idx]);
                hw_sels.pri_slice[1].intraslice = 1;
                hw_sels.pri_slice[1].fpf2 = dbl_wide_key;
                hw_sels.pri_slice[1].fpf4 = dbl_wide_key;
                hw_sels.pri_slice[1].inner_vlan_overlay = inner_vlan_overlay;
            }
            /* If paired, get secondary slice's selectors */
            if (paired) {
                hw_sels.sec_slice[0].fpf1 = soc_FP_PORT_FIELD_SELm_field32_get
                                         (unit, pfs_entry, 
                                          _fb_field_tbl[slice_idx + 1][0]);
                hw_sels.sec_slice[0].fpf2 = soc_FP_PORT_FIELD_SELm_field32_get
                                         (unit, pfs_entry, 
                                          _fb_field_tbl[slice_idx + 1][1]);
                hw_sels.sec_slice[0].fpf3 = soc_FP_PORT_FIELD_SELm_field32_get
                                         (unit, pfs_entry, 
                                          _fb_field_tbl[slice_idx + 1][2]);                                  hw_sels.sec_slice[0].fpf4 = soc_reg_field_get(unit, 
                                             FP_F4_SELECTr, f4_sel_val, 
                                             _fb_ing_f4_reg[slice_idx + 1]);
                hw_sels.sec_slice[0].inner_vlan_overlay = inner_vlan_overlay;
                if (intraslice) {
                    dbl_wide_key_sec = soc_FP_PORT_FIELD_SELm_field32_get
                     (unit, pfs_entry, _fb2_ifp_double_wide_key[slice_idx]);
                    hw_sels.sec_slice[1].intraslice = 1;
                    hw_sels.sec_slice[1].fpf2 = dbl_wide_key_sec;
                    hw_sels.sec_slice[1].fpf4 = dbl_wide_key_sec;
                    hw_sels.sec_slice[1].inner_vlan_overlay = inner_vlan_overlay;
                }
            }
            /* Create a group based on HW qualifiers (or find existing) */
            rv = _field_fb2_group_construct(unit, &hw_sels, intraslice, 
                                            paired, fc, port, 
                                            _BCM_FIELD_STAGE_INGRESS,
                                            slice_idx);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }
        /* Now go over the entries in this slice */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            ratio = 2;
            fs->free_count >>= 1;
        } else {
            ratio = 1;
        }
        prev_prio = -1;
        for (idx = 0; idx < fs->entry_count / ratio; idx++) {
            group_found = 0;
            tcam_entry = soc_mem_table_idx_to_pointer(unit, FP_TCAMm, 
                                                      fp_tcam_entry_t *, 
                                                      fp_tcam_buf, idx + 
                                                      entries_per_slice * 
                                                      slice_idx);
            if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) == 0) {
                continue;
            }
            /* Check which ports are applicable to this entry */
            SOC_PBMP_CLEAR(entry_pbmp);
            if ((soc_FP_TCAMm_field32_get(unit, tcam_entry, IPBM_SELf)) && 
                (soc_FP_TCAMm_field32_get(unit, tcam_entry, 
                                          IPBM_SEL_MASKf))) {
                pbm = soc_FP_TCAMm_field32_get(unit, tcam_entry, F1f) & 
                      soc_FP_TCAMm_field32_get(unit, tcam_entry, F1_MASKf);
                SOC_PBMP_WORD_SET(entry_pbmp, 0, pbm);
            } else if ((f4_sel_val == 0) && 
                       soc_FP_TCAMm_field32_get(unit, tcam_entry, F4_MASKf)) {
                pbm = soc_FP_TCAMm_field32_get(unit, tcam_entry, F4f) & 
                      soc_FP_TCAMm_field32_get(unit, tcam_entry, F4_MASKf);
                SOC_PBMP_PORT_ADD(entry_pbmp, pbm);
            } else {
                SOC_PBMP_ASSIGN(entry_pbmp, PBMP_ALL(unit));
            }
            /* Search groups to find match */
            fg = fc->groups;
            while (fg != NULL) {
                /* Check if group is in this slice */
                fs = &fg->slices[0];
                if (fs->slice_number != slice_idx) {
                    fg = fg->next;
                    continue;
                }
                /* Check if entry_pbmp is a subset of group pbmp */
                SOC_PBMP_ASSIGN(temp_pbmp, fg->pbmp);
                SOC_PBMP_OR(temp_pbmp, PBMP_LB(unit));
                SOC_PBMP_AND(temp_pbmp, entry_pbmp);
                if (SOC_PBMP_EQ(temp_pbmp, entry_pbmp)) {
                    group_found = 1;
                    break;
                }
                fg = fg->next;
            }
            if (!group_found) {
                return BCM_E_INTERNAL; /* Should never happen */
            }
            /* Allocate memory for the entry */
            rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                                   fg->flags, &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            mem_sz = parts_count * sizeof (_field_entry_t);
            _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = f_ent_wb_info.pid = -1;
            if (fc->l2warm) {
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
                   action_bmp.w = NULL;
                   _FP_XGS3_ALLOC(action_bmp.w,
                             SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                             "Action No Param Bitmap");
                   if (action_bmp.w == NULL) {
                       rv = BCM_E_MEMORY;
                       sal_free(f_ent);
                       goto cleanup;
                   }
                }
                f_ent_wb_info.action_bmp = &action_bmp;

                rv = _field_entry_info_retrieve(unit, fc, stage_fc,
                                                multigroup, &prev_prio,
                                                &f_ent_wb_info);
                
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr(unit);
            }
            vslice_idx = _field_physical_to_virtual(unit, _FP_DEF_INST,
                                                   slice_idx, stage_fc);
            if (vslice_idx == -1) {
                rv = BCM_E_INTERNAL;
                sal_free(f_ent);
                goto cleanup;
            }
            pri_tcam_idx = idx + entries_per_slice * slice_idx;
            for (i = 0; i < parts_count; i++) {
                if (fc->l2warm) {
                    /* Use retrieved EID */
                    f_ent[i].eid = f_ent_wb_info.eid;
                    /* Set retrieved dvp_type and svp_type */
                    f_ent[i].dvp_type = f_ent_wb_info.dvp_type[i];
                    f_ent[i].svp_type = f_ent_wb_info.svp_type[i];
                    f_ent[i].prio = f_ent_wb_info.prio;
                } else {
                    f_ent[i].eid = _bcm_field_last_alloc_eid_get(unit);
                    f_ent[i].prio = (vslice_idx << 10) | (entries_per_slice - idx);
                }
                f_ent[i].group = fg;
                rv = _bcm_field_tcam_part_to_entry_flags(unit, i, fg,
                                                         &f_ent[i].flags);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent, 
                                                        pri_tcam_idx,
                                                        i, &part_index);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                         _FP_DEF_INST,
                                                         part_index, 
                                                         &slice_number,
                                                (int *)&f_ent[i].slice_idx);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                BCM_PBMP_OR(f_ent[i].fs->pbmp, fg->pbmp);
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary 
                       entries. */
                    f_ent[i].fs->free_count--;
                }
                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;
    
                if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* tcam_entry_buf must point to the entry part */
                tcam_entry_buf = (uint32 *)soc_mem_table_idx_to_pointer
                                             (unit, FP_TCAMm, 
                                              fp_tcam_entry_t *, 
                                              fp_tcam_buf, part_index);

                /* Use this part to determine fg->qual_arr[i] */
                /* Iterate over the HW qualifiers and match offsets with
                   the non-zero masks */
                for (j = 0; j < fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size; j++) {
                    qual_idx = fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[j];
                    f_qual_arr = stage_fc->f_qual_arr[qual_idx];
                    /* Loop over configurations of this qualifier */
                    for (k = 0; k < f_qual_arr->conf_sz; k++) {
                        if (_mask_is_set(unit, &f_qual_arr->conf_arr[k].offset,
                                tcam_entry_buf, _BCM_FIELD_STAGE_INGRESS)) {
                            qualified = 0;
                            sel_ptr = NULL;
                            switch (i) {
                            case 0:
                                sel_ptr = &hw_sels.pri_slice[0];
                                break;
                            case 1:
                                if (fs->group_flags & 
                                    _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                                    sel_ptr = &hw_sels.pri_slice[1];
                                } else {
                                    sel_ptr = &hw_sels.sec_slice[0];
                                }
                                break;
                            case 2:
                                sel_ptr = &hw_sels.sec_slice[0];
                                break;
                            case 3:
                                sel_ptr = &hw_sels.sec_slice[1];
                                break;
                            default:
                                break;
                            }
                            if (((f_qual_arr->conf_arr[k].selector.pri_sel == 
                                 _bcmFieldSliceSelFpf1) && 
                                (f_qual_arr->conf_arr[k].selector.pri_sel_val 
                                 == sel_ptr->fpf1)) || 
                                ((f_qual_arr->conf_arr[k].selector.pri_sel == 
                                 _bcmFieldSliceSelFpf2) && 
                                (f_qual_arr->conf_arr[k].selector.pri_sel_val 
                                 == sel_ptr->fpf2)) ||
                                ((f_qual_arr->conf_arr[k].selector.pri_sel == 
                                 _bcmFieldSliceSelFpf3) && 
                                (f_qual_arr->conf_arr[k].selector.pri_sel_val 
                                 == sel_ptr->fpf3)) ||
                                ((f_qual_arr->conf_arr[k].selector.pri_sel == 
                                 _bcmFieldSliceSelFpf4) && 
                                (f_qual_arr->conf_arr[k].selector.pri_sel_val 
                                 == sel_ptr->fpf4))) {
                                qualified = 1;
                            }
                            if (qualified) {
                                sal_memcpy(&fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j],
                                           &f_qual_arr->conf_arr[k].offset,
                                           sizeof(f_qual_arr->conf_arr[k].offset));
                            }
                            break;
                        } 
                    }
                }
                /* Get the actions associated with this part of the entry */
                policy_entry = soc_mem_table_idx_to_pointer
                                                (unit, FP_POLICY_TABLEm, 
                                                 fp_policy_table_entry_t *, 
                                                 fp_policy_buf, part_index);
                rv = _field_fb_actions_recover(unit, (uint32 *)policy_entry,  
                                               f_ent, i, FP_POLICY_TABLEm,
                                               &f_ent_wb_info);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }
            }
            /* Add to the group */
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                goto cleanup;
            }
            f_ent = NULL;
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit, fc, slice_idx);
        }
    }

    /* Now go over the expanded slices */
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        if (!expanded[slice_idx]) {
            continue;
        }
        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        for (idx = 0; idx < fs->entry_count; idx++) {
            tcam_entry = soc_mem_table_idx_to_pointer(unit, FP_TCAMm, 
                                                      fp_tcam_entry_t *, 
                                                      fp_tcam_buf, idx + 
                                                      entries_per_slice * 
                                                      slice_idx);
            if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) != 0) {
                break;
            }
        }
        if (idx == fs->entry_count){
            continue;
        }
        /* If Level 2, retrieve the GIDs in this slice */
        if (fc->l2warm) {
            rv = _field_scache_slice_group_recover(unit, fc, slice_idx, 
                                                       &multigroup);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }
        /* Now find the master slice for this virtual group */
        vslice_idx = _field_physical_to_virtual(unit, _FP_DEF_INST,
                                                slice_idx, stage_fc);
        if (vslice_idx == -1) {
            /* vslice_idx - Must be a valid hardware slice index value */
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        if (fc->l2warm
                && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_8)) {
            master_slice = slice_master_idx[slice_idx];
        } else {
        max = -1;
        for (i = 0; i < stage_fc->tcam_slices; i++) {
            if ((stage_fc->vmap[_FP_DEF_INST][0][vslice_idx].virtual_group == 
                stage_fc->vmap[_FP_DEF_INST][0][i].virtual_group) && (i != vslice_idx)) {
                if (i > max) {
                    max = i;
                }
            } 
        }

        /* max - Must be a valid hardware slice index value */
        if (-1 == max) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        master_slice = stage_fc->vmap[_FP_DEF_INST][0][max].vmap_key;
        }
        /* See which group is in this slice - can be only one */
        fg = fc->groups;
        while (fg != NULL) {
            /* Check if group is in this slice */
            fs = &fg->slices[0];
            if ((fg->stage_id == stage_fc->stage_id)
                    && (fs->slice_number == master_slice)) {
                break;
            }
            fg = fg->next;
        }
        if (fg == 0) {
            rv = BCM_E_INTERNAL;
            goto cleanup;
        }

        /* Get number of entry parts for the group. */
        rv = _bcm_field_entry_tcam_parts_count (unit, fg->stage_id,
                                                fg->flags, &parts_count);
        BCM_IF_ERROR_RETURN(rv);

        old_physical_slice = fs->slice_number;

        /* Set up the new physical slice parameters in Software */
        for(part_index = parts_count - 1; part_index >= 0; part_index--) {
            /* Get entry flags. */
            rv = _bcm_field_tcam_part_to_entry_flags(unit, part_index, fg, &entry_flags);
            BCM_IF_ERROR_RETURN(rv);

            /* Get slice id for entry part */
            rv = _bcm_field_tcam_part_to_slice_number(unit, part_index,
                                                      fg, &slice_num);
            BCM_IF_ERROR_RETURN(rv);

            /* Get slice pointer. */
            fs = stage_fc->slices[_FP_DEF_INST] + slice_idx
                    + slice_num;

            if (0 == (entry_flags & _FP_ENTRY_SECOND_HALF)) {
                /* Set per slice configuration &  number of free entries in the slice.*/
                fs->free_count = fs->entry_count;
                if (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                    fs->free_count >>= 1;
                }
                /* Set group flags in in slice.*/
                fs->group_flags = fg->flags & _FP_GROUP_STATUS_MASK;

                /* Add slice to slices linked list . */
                fs_temp = stage_fc->slices[_FP_DEF_INST] + old_physical_slice + slice_num;
                /* To handle more than one auto expanded slice in a group */
                while (fs_temp->next != NULL) {
                    fs_temp = fs_temp->next;
                }
                fs_temp->next = fs;
                fs->prev = fs_temp;
            }
        }

        /* Now go over the entries in this slice */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            ratio = 2;
        } else {
            ratio = 1;
        }
        prev_prio = -1;
        for (idx = 0; idx < fs->entry_count / ratio; idx++) {
            group_found = 0;
            tcam_entry = soc_mem_table_idx_to_pointer(unit, FP_TCAMm, 
                                                      fp_tcam_entry_t *, 
                                                      fp_tcam_buf, idx + 
                                                      entries_per_slice * 
                                                      slice_idx);
            if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) == 0) {
                continue;
            }
            /* Check which ports are applicable to this entry */
            SOC_PBMP_CLEAR(entry_pbmp);
            if ((soc_FP_TCAMm_field32_get(unit, tcam_entry, IPBM_SELf)) && 
                (soc_FP_TCAMm_field32_get(unit, tcam_entry, 
                                          IPBM_SEL_MASKf))) {
                pbm = soc_FP_TCAMm_field32_get(unit, tcam_entry, F1f) & 
                      soc_FP_TCAMm_field32_get(unit, tcam_entry, F1_MASKf);
                SOC_PBMP_WORD_SET(entry_pbmp, 0, pbm);
            } else if ((f4_sel_val == 0) && 
                       soc_FP_TCAMm_field32_get(unit, tcam_entry, F4_MASKf)) {
                pbm = soc_FP_TCAMm_field32_get(unit, tcam_entry, F4f) & 
                      soc_FP_TCAMm_field32_get(unit, tcam_entry, F4_MASKf);
                SOC_PBMP_PORT_ADD(entry_pbmp, pbm);
            } else {
                SOC_PBMP_ASSIGN(entry_pbmp, PBMP_ALL(unit));
            }
            /* Search groups to find match */
            fg = fc->groups;
            while (fg != NULL) {
                /* Check if group is in this slice */
                fs = &fg->slices[0];
                if ((fg->stage_id != stage_fc->stage_id)
                        || (fs->slice_number != master_slice)) {
                    fg = fg->next;
                    continue;
                }
                /* Check if entry_pbmp is a subset of group pbmp */
                SOC_PBMP_ASSIGN(temp_pbmp, fg->pbmp);
                SOC_PBMP_OR(temp_pbmp, PBMP_LB(unit));
                SOC_PBMP_AND(temp_pbmp, entry_pbmp);
                if (SOC_PBMP_EQ(temp_pbmp, entry_pbmp)) {
                    group_found = 1;
                    break;
                }
                fg = fg->next;
            }
            if (!group_found) {
                return BCM_E_INTERNAL; /* Should never happen */
            }
            /* Allocate memory for the entry */
            /*    coverity[forward_null : FALSE]    */
            rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                                   fg->flags, &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            mem_sz = parts_count * sizeof (_field_entry_t);
            _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = f_ent_wb_info.pid = -1;
            if (fc->l2warm) {
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
                   action_bmp.w = NULL;
                   _FP_XGS3_ALLOC(action_bmp.w,
                             SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                             "Action No Param Bitmap");
                   if (action_bmp.w == NULL) {
                       rv = BCM_E_MEMORY;
                       sal_free(f_ent);
                       goto cleanup;
                   }
                }
                f_ent_wb_info.action_bmp = &action_bmp;

                rv = _field_entry_info_retrieve(unit, fc, stage_fc,
                                                multigroup, &prev_prio,
                                                &f_ent_wb_info);
                
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr(unit);
            }
            pri_tcam_idx = idx + entries_per_slice * slice_idx;
            for (i = 0; i < parts_count; i++) {
                if (fc->l2warm) {
                    /* Use retrieved EID */
                    f_ent[i].eid = f_ent_wb_info.eid;
                    /* Set retrieved dvp_type and svp_type */
                    f_ent[i].dvp_type = f_ent_wb_info.dvp_type[i];
                    f_ent[i].svp_type = f_ent_wb_info.svp_type[i];
                    f_ent[i].prio = f_ent_wb_info.prio;
                } else {
                    f_ent[i].eid = _bcm_field_last_alloc_eid_get(unit);
                    f_ent[i].prio = (vslice_idx << 10) | (entries_per_slice - idx);
                }
                f_ent[i].group = fg;
                rv = _bcm_field_tcam_part_to_entry_flags(unit, i, fg,
                                                         &f_ent[i].flags);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent, 
                                                        pri_tcam_idx,
                                                        i, &part_index);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                         _FP_DEF_INST,
                                                         part_index, 
                                                         &slice_number,
                                                (int *)&f_ent[i].slice_idx);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                BCM_PBMP_OR(f_ent[i].fs->pbmp, fg->pbmp);
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary 
                       entries. */
                    f_ent[i].fs->free_count--;
                }
                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                if (soc_FP_TCAMm_field32_get(unit, tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* tcam_entry_buf must point to the entry part */
                tcam_entry_buf = (uint32 *)soc_mem_table_idx_to_pointer
                                             (unit, FP_TCAMm, 
                                              fp_tcam_entry_t *, 
                                              fp_tcam_buf, part_index);

                /* Use this part to determine fg->qual_arr[i] */
                /* Iterate over the HW qualifiers and match offsets with
                   the non-zero masks */
                for (j = 0; j < fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size; j++) {
                    qual_idx = fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[j];
                    f_qual_arr = stage_fc->f_qual_arr[qual_idx];
                    /* Loop over configurations of this qualifier */
                    for (k = 0; k < f_qual_arr->conf_sz; k++) {
                        if (_mask_is_set(unit, &f_qual_arr->conf_arr[k].offset,
                                tcam_entry_buf, _BCM_FIELD_STAGE_INGRESS)) {
                            qualified = 0;
                            sel_ptr = NULL;
                            switch (i) {
                            case 0:
                                sel_ptr = &hw_sels.pri_slice[0];
                                break;
                            case 1:
                                if (fs->group_flags & 
                                    _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
                                    sel_ptr = &hw_sels.pri_slice[1];
                                } else {
                                    sel_ptr = &hw_sels.sec_slice[0];
                                }
                                break;
                            case 2:
                                sel_ptr = &hw_sels.sec_slice[0];
                                break;
                            case 3:
                                sel_ptr = &hw_sels.sec_slice[1];
                                break;
                            default:
                                break;
                            }
                            if (((f_qual_arr->conf_arr[k].selector.pri_sel == 
                                 _bcmFieldSliceSelFpf1) && 
                                (f_qual_arr->conf_arr[k].selector.pri_sel_val 
                                 == sel_ptr->fpf1)) || 
                                ((f_qual_arr->conf_arr[k].selector.pri_sel == 
                                 _bcmFieldSliceSelFpf2) && 
                                (f_qual_arr->conf_arr[k].selector.pri_sel_val 
                                 == sel_ptr->fpf2)) ||
                                ((f_qual_arr->conf_arr[k].selector.pri_sel == 
                                 _bcmFieldSliceSelFpf3) && 
                                (f_qual_arr->conf_arr[k].selector.pri_sel_val 
                                 == sel_ptr->fpf3)) ||
                                ((f_qual_arr->conf_arr[k].selector.pri_sel == 
                                 _bcmFieldSliceSelFpf4) && 
                                (f_qual_arr->conf_arr[k].selector.pri_sel_val 
                                 == sel_ptr->fpf4))) {
                                qualified = 1;
                            }
                            if (qualified) {
                                sal_memcpy(&fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j],
                                           &f_qual_arr->conf_arr[k].offset.field,
                                           sizeof(f_qual_arr->conf_arr[k].offset.field));
                            }
                            break;
                        } 
                    }
                }
                /* Get the actions associated with this part of the entry */
                policy_entry = soc_mem_table_idx_to_pointer
                                                (unit, FP_POLICY_TABLEm, 
                                                 fp_policy_table_entry_t *, 
                                                 fp_policy_buf, part_index);
                rv = _field_fb_actions_recover(unit, (uint32 *)policy_entry,  
                                               f_ent, i, FP_POLICY_TABLEm,
                                               &f_ent_wb_info);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }
            }
            /* Add to the group */
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                goto cleanup;
            }
            f_ent = NULL;
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit, fc, slice_idx);
        }
    }

    /* To set per port flags */
    group = fc->groups;
    while (group != NULL) {
           if (group->stage_id == _BCM_FIELD_STAGE_INGRESS) {
               if (!BCM_PBMP_EQ(PBMP_ALL(unit), group->pbmp)) {
                   /* Update Group Flags to denote Port based Group */
                   group->flags |= _FP_GROUP_PER_PORT_OR_PBMP;
               }
           }
           group = group->next;
    }

    if (fc->l2warm) {

        /* Mark end of Slice Info */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            fc->scache_pos++;
        }

        /* Group Slice Selectors */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) { 
            rv = _field_group_slice_selectors_recover(unit,
                        &buf[fc->scache_pos], stage_fc->stage_id);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        temp = 0;
        temp |= buf[fc->scache_pos];
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        if (temp != _FIELD_IFP_DATA_END) {
            fc->l2warm = 0;
            rv = BCM_E_INTERNAL;
        }
        if (NULL != buf1) {
            temp = 0;
            temp |= buf1[fc->scache_pos1];
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 8;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 16;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 24;
            fc->scache_pos1++;
            if (temp != _FIELD_IFP_DATA_END) {
                fc->l2warm = 0;
                rv = BCM_E_INTERNAL;
            }
        }

    }

    if (BCM_SUCCESS(rv)) {
        _field_group_slice_vmap_recovery(unit, fc, stage_fc);
    }

cleanup:
    if (fp_tcam_buf) {
        soc_cm_sfree(unit, fp_tcam_buf);
    }
    if (fp_pfs_buf) {
        soc_cm_sfree(unit, fp_pfs_buf);
    }
    if (fp_policy_buf) {
        soc_cm_sfree(unit, fp_policy_buf);
    }
    if (action_bmp.w != NULL) {
        _FP_ACTION_BMP_FREE(action_bmp);
        action_bmp.w = NULL;
    }

    return rv;
}

static uint16 list_key1[22] = {bcmFieldQualifyIpFrag, bcmFieldQualifyTcpControl,
                      bcmFieldQualifyL4DstPort, bcmFieldQualifyL4SrcPort,
                      bcmFieldQualifyTtl, bcmFieldQualifyIpProtocol,
                      bcmFieldQualifyDstIp, bcmFieldQualifySrcIp,
                      bcmFieldQualifyIpType, bcmFieldQualifyTos,
                      bcmFieldQualifyInPort, bcmFieldQualifyL3Routable,
                      bcmFieldQualifyMirrorCopy, bcmFieldQualifyOuterVlan,
                      bcmFieldQualifyOuterVlanId, bcmFieldQualifyOuterVlanCfi,
                      bcmFieldQualifyOuterVlanPri, bcmFieldQualifyVlanFormat,
                      bcmFieldQualifyDstHiGig, bcmFieldQualifyInterfaceClassPort,
                      bcmFieldQualifyOutPort, bcmFieldQualifyIp4};

static uint16 list_key2[16] = {bcmFieldQualifyIpProtocol, bcmFieldQualifySrcIp6,
                      bcmFieldQualifyIpType, bcmFieldQualifyDSCP,
                      bcmFieldQualifyInPort, bcmFieldQualifyL3Routable,
                      bcmFieldQualifyMirrorCopy, bcmFieldQualifyOuterVlan,
                      bcmFieldQualifyOuterVlanId, bcmFieldQualifyOuterVlanCfi,
                      bcmFieldQualifyOuterVlanPri, bcmFieldQualifyVlanFormat,
                      bcmFieldQualifyDstHiGig, bcmFieldQualifyInterfaceClassPort,
                      bcmFieldQualifyOutPort, bcmFieldQualifyIp6};

static uint16 list_key3[7] = {bcmFieldQualifyIpFrag, bcmFieldQualifyTcpControl,
                      bcmFieldQualifyL4DstPort, bcmFieldQualifyL4SrcPort,
                      bcmFieldQualifyTtl, bcmFieldQualifyDstIp6,
                      bcmFieldQualifyDstIp6};

static uint16 list_key4[19] = {bcmFieldQualifyL2Format, bcmFieldQualifyEtherType,
                      bcmFieldQualifySrcMac, bcmFieldQualifyDstMac,
                      bcmFieldQualifyInnerVlan, bcmFieldQualifyInnerVlanId,
                      bcmFieldQualifyInnerVlanCfi, bcmFieldQualifyInnerVlanPri,
                      bcmFieldQualifyInPort, bcmFieldQualifyL3Routable,
                      bcmFieldQualifyMirrorCopy, bcmFieldQualifyOuterVlan,
                      bcmFieldQualifyOuterVlanId, bcmFieldQualifyOuterVlanCfi,
                      bcmFieldQualifyOuterVlanPri, bcmFieldQualifyVlanFormat,
                      bcmFieldQualifyDstHiGig, bcmFieldQualifyInterfaceClassPort,
                      bcmFieldQualifyOutPort};

int
_field_fb2_stage_egress_reinit(int unit, _field_control_t *fc, 
                               _field_stage_t *stage_fc)
{
    int idx, idx1, slice_idx, index_min, index_max, rv = BCM_E_NONE;
    int entries_per_slice, mem_sz, qual_idx, parts_count = 1;
    int i, j, k, pri_tcam_idx, part_index, slice_number;
    int size, list_size[2], prev_prio;
    uint16 *list_arr[2];
    char *efp_tcam_buf = NULL; /* Buffer to read the EFP_TCAM table */
    char *efp_policy_buf = NULL; /* Buffer to read the EFP_POLICY table */
    uint32 rval, efp_slice_mode, key_match_type, temp;
    uint32 *efp_tcam_entry_buf;
    bcm_field_group_t gid;
    int               priority;
    int               group_flags;
    efp_tcam_entry_t *efp_tcam_entry;
    efp_policy_table_entry_t *efp_policy_entry;
    _field_slice_t *fs;
    _field_group_t *fg = NULL;
    _field_entry_t *f_ent = NULL;
    _bcm_field_qual_info_t *f_qual_arr = NULL;
    _bcm_field_qual_offset_t *q_offset_arr = NULL;
    uint16 *qid_arr = NULL;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    bcm_field_qset_t qset;
    int action_res_id;
    bcm_pbmp_t  group_pbmp;
    _field_entry_wb_info_t f_ent_wb_info;
    soc_field_t efp_en_flds[4] = {SLICE_ENABLE_SLICE_0f, SLICE_ENABLE_SLICE_1f,
                                  SLICE_ENABLE_SLICE_2f, SLICE_ENABLE_SLICE_3f};

    soc_field_t efp_lk_en_flds[4] = 
                     {LOOKUP_ENABLE_SLICE_0f, LOOKUP_ENABLE_SLICE_1f,
                      LOOKUP_ENABLE_SLICE_2f, LOOKUP_ENABLE_SLICE_3f};
    _field_action_bmp_t action_bmp;

    /* Reset Action bitmap to NULL. */
    action_bmp.w = NULL;

    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_EFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* DMA various tables */
    efp_tcam_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                 (unit, EFP_TCAMm), 
                                 "EFP TCAM buffer");
    if (NULL == efp_tcam_buf) {
        return BCM_E_MEMORY;
    }
    index_min = soc_mem_index_min(unit, EFP_TCAMm);
    index_max = soc_mem_index_max(unit, EFP_TCAMm);
    if ((rv = soc_mem_read_range(unit, EFP_TCAMm, MEM_BLOCK_ALL,
                                 index_min, index_max, 
                                 efp_tcam_buf)) < 0 ) {
        goto cleanup;
    }
    efp_policy_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                  (unit, EFP_POLICY_TABLEm), 
                                  "EFP POLICY TABLE buffer");
    if (NULL == efp_policy_buf) {
        return BCM_E_MEMORY;
    }
    index_min = soc_mem_index_min(unit, EFP_POLICY_TABLEm);
    index_max = soc_mem_index_max(unit, EFP_POLICY_TABLEm);
    if ((rv = soc_mem_read_range(unit, EFP_POLICY_TABLEm, MEM_BLOCK_ALL,
                                 index_min, index_max, 
                                 efp_policy_buf)) < 0 ) {
        goto cleanup;
    }
    entries_per_slice = 128;

    /* Iterate over the slices */
    if ((rv = READ_EFP_SLICE_CONTROLr(unit, &rval)) < 0) {
        goto cleanup;
    }
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        /* Ignore disabled slice */
        if ((soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval, 
                               efp_en_flds[slice_idx]) == 0) || 
            (soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval, 
                               efp_lk_en_flds[slice_idx]) == 0)) {
            continue;
        }

        efp_slice_mode = soc_reg_field_get(unit, EFP_SLICE_CONTROLr, rval, 
                                           _efp_slice_mode[slice_idx]);
        /* Skip if slice has no valid entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        for (idx = 0; idx < fs->entry_count; idx++) {
            efp_tcam_entry = soc_mem_table_idx_to_pointer
                                 (unit, EFP_TCAMm, efp_tcam_entry_t *, 
                                  efp_tcam_buf, idx + 
                                  entries_per_slice * slice_idx);
            if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, 
                                          VALIDf) != 0) {
                /* Get KEY_MATCH_TYPE from first valid entry */
                /* 
                 * COVERITY 
                 *
                 * Call to _field_extract is taken care in all the flows
                 * by not setting the width mrore than 32.
                 */
                 /* coverity[callee_ptr_arith] */
                _field_extract((uint32 *)efp_tcam_entry, 181 + 2, 3, 
                                &key_match_type); 
                break;
            }
        }
        if ((idx == fs->entry_count) && !fc->l2warm) {
            continue;
        }
        /* Skip second part of slice pair */
        if ((efp_slice_mode == _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE) &&
            (slice_idx % 2)) {
            continue;
        }
        fg = NULL;
        if (efp_slice_mode != 3) { /* Invalid value */
            /* If Level 2, retrieve the GIDs in this slice */
            if (fc->l2warm) {
                rv = _field_scache_slice_group_recover(unit, fc, slice_idx, 
                                                           NULL);
                if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                    fc->l2warm = 0;
                    goto cleanup;
                }
                if (rv == BCM_E_NOT_FOUND) {
                    rv = BCM_E_NONE;
                    continue;
                }
            }
            /* Create a new group - one group per slice */
            _FP_XGS3_ALLOC(fg, sizeof(_field_group_t), "field group");
            if (NULL == fg) {
                goto cleanup;
            } 

            /* Initialize action res id and VMAP group to default */
            action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
            for (idx = 0; idx < _FP_PAIR_MAX; idx++) {
                fg->vmap_group[idx] = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
            }

            group_flags = 0; 
            if (fc->l2warm) {
                /* Get stored group ID and QSET for Level 2 */
                rv = _field_group_info_retrieve(unit, -1, _FP_DEF_INST, &gid,
                                                &priority, &action_res_id, &group_pbmp, NULL,
                                                &group_flags, &qset, fc);
                if (gid != -1) {
                    sal_memcpy(&fg->qset, &qset, sizeof(bcm_field_qset_t));
                } else {
                    sal_free(fg);
                    goto cleanup;
                }
            } else {
                rv = _bcm_field_group_id_generate(unit, &gid);
                priority = slice_idx;
            }
            if (BCM_FAILURE(rv)) {
                sal_free(fg);
                goto cleanup;
            }
            fg->gid = gid;
            fg->priority = priority;
            fg->action_res_id = action_res_id;
            if (group_flags & _FP_GROUP_SELECT_AUTO_EXPANSION) {
                fg->flags = fg->flags | _FP_GROUP_SELECT_AUTO_EXPANSION;
            }

            fg->stage_id = stage_fc->stage_id;
            list_arr[0] = list_arr[1] = NULL;
            switch (efp_slice_mode) {
                case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2:
                    fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY4;
                    parts_count = 1;
                    list_size[0] = 19;
                    list_arr[0] = list_key4;
                    break;
                case _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3:
                    fg->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
                    fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                    parts_count = 1;
                    list_size[0] = 22;
                    list_arr[0] = list_key1;
                    break;
                case _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE:
                    fg->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
                    if (key_match_type == 
                        _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE) {
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY3;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY2;
                        list_size[0] = 7; 
                        list_size[1] = 16;    
                        list_arr[0] = list_key3;  
                        list_arr[1] = list_key2;            
                    } else { /* _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE */
                        fg->sel_codes[0].fpf3 = _BCM_FIELD_EFP_KEY1;
                        fg->sel_codes[1].fpf3 = _BCM_FIELD_EFP_KEY4;
                        list_size[0] = 22; 
                        list_size[1] = 19;
                        list_arr[0] = list_key1;  
                        list_arr[1] = list_key4; 
                    }
                    parts_count = 2;
                    break;
                default:
                    /* slice mode must be a valid value. */
                    sal_free(fg);
                    rv = BCM_E_INTERNAL;
                    goto cleanup;
            }
            /* Determine QSET based on selector values */
            /* Add the appropriate list of qualifiers */
            BCM_FIELD_QSET_ADD(fg->qset, bcmFieldQualifyStageEgress);
            for (i = 0; i < parts_count; i++) {
                for (j = 0; j < list_size[i]; j++) {
                    if (fc->l2warm && !BCM_FIELD_QSET_TEST(fg->qset, 
                                                       list_arr[i][j])) {  
                        continue;
                    }
                    BCM_FIELD_QSET_ADD(fg->qset, list_arr[i][j]);
                    f_qual_arr = stage_fc->f_qual_arr[list_arr[i][j]];
                    for (k = 0; k < f_qual_arr->conf_sz; k++) {
                        if ((f_qual_arr->conf_arr[k].selector.pri_sel_val == 
                             fg->sel_codes[i].fpf3)) {
                            break;
                        }
                    }
                    /* Allocate memory for this qualfier and its
                       associated ofset information if not already added 
                       to fg->qual_arr[i] */
                    size = fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size + 1;
                    qid_arr = NULL;
                    _FP_XGS3_ALLOC(qid_arr, size * sizeof (uint16), 
                                   "Group qual id");
                    if (NULL == qid_arr) {
                        goto cleanup;
                    }              
                    q_offset_arr = NULL;              
                    _FP_XGS3_ALLOC(q_offset_arr, size * 
                                   sizeof (_bcm_field_qual_offset_t), 
                                   "Group qual offset");
                    if (NULL == q_offset_arr) {
                        sal_free (qid_arr);
                        goto cleanup;
                    }
                    if (size) {
                        sal_memcpy(qid_arr, fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr,
                                   fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size * 
                                   sizeof (uint16));
                        sal_memcpy(q_offset_arr, 
                                   fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr, 
                                   fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size * 
                                   sizeof (_bcm_field_qual_offset_t));
                        if (fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size) {
                            sal_free(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr);
                            sal_free(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr);
                        }
                    }
                    fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr = qid_arr;
                    fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr = q_offset_arr;
                    fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[size - 1] = list_arr[i][j];

                    sal_memcpy(&fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[size - 1],
                               &f_qual_arr->conf_arr[k].offset,
                               sizeof(f_qual_arr->conf_arr[k].offset));

                    fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size = size;
                }
            }
            /* Associate slice(s) to group */
            fg->slices = stage_fc->slices[_FP_DEF_INST] + slice_idx;
            SOC_PBMP_ASSIGN(fg->pbmp, PBMP_ALL(unit));
            BCM_PBMP_OR(fs->pbmp, fg->pbmp);

            /* Initialize group default ASET list. */
            rv = _field_group_default_aset_set(unit, fg);
            if (BCM_FAILURE(rv)) {
                /* Free group's qualifier and offset array memories. */
                for (i = 0; i < parts_count; i++) {
                    if (NULL != fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr) {
                        sal_free(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr);
                        fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr = NULL;
                    }
                    if (NULL != fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr) {
                        sal_free(fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr);
                        fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr = NULL;
                    }
                }

                /* Free group's memory*/
                sal_free(fg);
                goto cleanup;
            }

            if (fc->l2warm
                    && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15)) {
                if (group_flags & _FP_GROUP_LOOKUP_ENABLED) {
                    fg->flags = fg->flags | _FP_GROUP_LOOKUP_ENABLED;
                }
                if (group_flags & _FP_GROUP_WLAN) {
                    fg->flags = fg->flags | _FP_GROUP_WLAN;
                }
            } else {
                fg->flags |= _FP_GROUP_LOOKUP_ENABLED;
            }

            fg->next = fc->groups;
            fc->groups = fg;

            /* Now go over the entries in this slice */
            prev_prio = -1;
            for (idx = 0; idx < fs->entry_count; idx++) {
                efp_tcam_entry = soc_mem_table_idx_to_pointer
                                     (unit, EFP_TCAMm, efp_tcam_entry_t *, 
                                      efp_tcam_buf, idx + 
                                      entries_per_slice * slice_idx);
                if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, 
                                              VALIDf) == 0) {
                    continue;
                }
                /* Allocate memory for this entry */
                mem_sz = parts_count * sizeof (_field_entry_t);
                _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
                if (f_ent == NULL) {
                    rv = BCM_E_MEMORY;
                    goto cleanup;
                }

                for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                     f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
                }

                sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
                f_ent_wb_info.sid = f_ent_wb_info.pid = -1;
                if (fc->l2warm) {
                    if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
                       action_bmp.w = NULL;
                       _FP_XGS3_ALLOC(action_bmp.w,
                             SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                             "Action No Param Bitmap");
                       if (action_bmp.w == NULL) {
                          rv = BCM_E_MEMORY;
                          sal_free(f_ent);
                          goto cleanup;
                       }
                    }
                    f_ent_wb_info.action_bmp = &action_bmp;

                    rv = _field_entry_info_retrieve(unit, fc, stage_fc,
                                                    0, &prev_prio, 
                                                    &f_ent_wb_info);
                    
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                } else {
                    _bcm_field_last_alloc_eid_incr(unit);
                }
                pri_tcam_idx = idx + entries_per_slice * slice_idx;
                for (i = 0; i < parts_count; i++) {
                    if (fc->l2warm) {
                        f_ent[i].eid = f_ent_wb_info.eid;
                        /* Set retrieved dvp_type and svp_type */
                        f_ent[i].dvp_type = f_ent_wb_info.dvp_type[i];
                        f_ent[i].svp_type = f_ent_wb_info.svp_type[i];
                    } else {
                        f_ent[i].eid = _bcm_field_last_alloc_eid_get(unit);
                    }
                    f_ent[i].group = fg;
                    rv = _bcm_field_tcam_part_to_entry_flags(unit, i, fg,
                                                             &f_ent[i].flags);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    if (f_ent_wb_info.color_independent) {
                        f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                    }
                    rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent, 
                                                            pri_tcam_idx,
                                                            i, &part_index);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                             _FP_DEF_INST,
                                                             part_index, 
                                                             &slice_number,
                                                (int *)&f_ent[i].slice_idx);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                    if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                        /* Decrement slice free entry count for primary 
                           entries. */
                        f_ent[i].fs->free_count--;
                    }
                    /* Assign entry to a slice */
                    f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                    f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                    if (soc_EFP_TCAMm_field32_get(unit, efp_tcam_entry, VALIDf) == 3) {
                        f_ent[i].flags |= _FP_ENTRY_ENABLED;
                    }

                    /* efp_tcam_entry_buf must point to the entry part */
                    efp_tcam_entry_buf = (uint32 *)
                                          soc_mem_table_idx_to_pointer
                                                 (unit, EFP_TCAMm, 
                                                  efp_tcam_entry_t *, 
                                                  efp_tcam_buf, part_index);

                    /* Use this part to determine fg->qual_arr[i] */
                    /* Iterate over the HW qualifiers and match offsets with
                       the non-zero masks */
                    for (j = 0; j < fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size; j++) {
                        qual_idx = fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[j];
                        f_qual_arr = stage_fc->f_qual_arr[qual_idx];
                        /* Loop over configurations of this qualifier */
                        for (k = 0; k < f_qual_arr->conf_sz; k++) {
                            if (_mask_is_set(unit, 
                                          &f_qual_arr->conf_arr[k].offset,
                                          efp_tcam_entry_buf, 
                                          _BCM_FIELD_STAGE_EGRESS)) {
                                sal_memcpy(&fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j].field,
                                           &f_qual_arr->conf_arr[k].offset,
                                           sizeof(f_qual_arr->conf_arr[k].offset));
                                break;
                            } 
                        }
                    }
                    /* Get the actions associated with this entry part */
                    efp_policy_entry = soc_mem_table_idx_to_pointer
                                               (unit, EFP_POLICY_TABLEm, 
                                                efp_policy_table_entry_t *, 
                                                efp_policy_buf, part_index);
                    rv = _field_fb_actions_recover(unit, (uint32 *)
                                                   efp_policy_entry, f_ent, 
                                                   i, EFP_POLICY_TABLEm,
                                                   &f_ent_wb_info);
                    if (BCM_FAILURE(rv)) {
                        sal_free(f_ent);
                        goto cleanup;
                    }
                    if (action_bmp.w != NULL) {
                       _FP_ACTION_BMP_FREE(action_bmp);
                       action_bmp.w = NULL;
                    }
                }
                /* Add to the group */
                rv = _field_group_entry_add(unit, fg, f_ent);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent = NULL;
            }
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit, fc, slice_idx);
        }
    }

    if (fc->l2warm) {

        /* Mark end of Slice Info */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            fc->scache_pos++;
        }

        /* Group Slice Selectors */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            rv = (_field_group_slice_selectors_recover(unit,
                        &buf[fc->scache_pos], stage_fc->stage_id));
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        temp = 0;
        temp |= buf[fc->scache_pos];
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        if (temp != _FIELD_EFP_DATA_END) {
            fc->l2warm = 0;
            rv = BCM_E_INTERNAL;
        }

        if (NULL != buf1) {
            temp = 0;
            temp |= buf1[fc->scache_pos1];
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 8;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 16;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 24;
            fc->scache_pos1++;
            if (temp != _FIELD_EFP_DATA_END) {
                fc->l2warm = 0;
                rv = BCM_E_INTERNAL;
            }
        }
    }
cleanup:
    if (efp_tcam_buf) {
        soc_cm_sfree(unit, efp_tcam_buf);
    }
    if (efp_policy_buf) {
        soc_cm_sfree(unit, efp_policy_buf);
    }
    if (fg != NULL) {
       sal_free(fg);
    }
    if (action_bmp.w != NULL) {
        _FP_ACTION_BMP_FREE(action_bmp);
        action_bmp.w = NULL;
    }
    return rv;
}

int
_field_fb2_stage_lookup_reinit(int unit, _field_control_t *fc, 
                               _field_stage_t *stage_fc)
{
    int idx, idx1, slice_idx, index_min, index_max, ratio, rv = BCM_E_NONE;
    int entries_per_slice, group_found, mem_sz, parts_count, qual_idx;
    int i, j, k, pri_tcam_idx, part_index, slice_number, prev_prio;
    char *vfp_tcam_buf = NULL; /* Buffer to read the VFP_TCAM table */
    char *vfp_policy_buf = NULL; /* Buffer to read the VFP_POLICY table */
    uint32 rval, paired, intraslice, dbl_wide_key, dbl_wide_key_sec;
    uint32 vfp_key, temp;
    uint32 *vfp_tcam_entry_buf;
    soc_field_t fld;
    vfp_tcam_entry_t *vfp_tcam_entry;
    vfp_policy_table_entry_t *vfp_policy_entry;
    _field_hw_qual_info_t hw_sels;
    _field_slice_t *fs;
    _field_group_t *fg;
    _field_entry_t *f_ent = NULL;
    bcm_pbmp_t entry_pbmp, temp_pbmp;
    _bcm_field_qual_info_t *f_qual_arr = NULL;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    _field_entry_wb_info_t f_ent_wb_info;

    soc_field_t vfp_en_flds[4] = {SLICE_ENABLE_SLICE_0f, SLICE_ENABLE_SLICE_1f,
                                  SLICE_ENABLE_SLICE_2f, SLICE_ENABLE_SLICE_3f};

    soc_field_t vfp_lk_en_flds[4] = 
                     {LOOKUP_ENABLE_SLICE_0f, LOOKUP_ENABLE_SLICE_1f,
                      LOOKUP_ENABLE_SLICE_2f, LOOKUP_ENABLE_SLICE_3f};
    _field_action_bmp_t action_bmp;

    /* Reset Action bitmap to NULL. */
    action_bmp.w = NULL;

    SOC_PBMP_CLEAR(entry_pbmp);

    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_VFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* DMA various tables */
    vfp_tcam_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                 (unit, VFP_TCAMm), 
                                 "VFP TCAM buffer");
    if (NULL == vfp_tcam_buf) {
        return BCM_E_MEMORY;
    }
    index_min = soc_mem_index_min(unit, VFP_TCAMm);
    index_max = soc_mem_index_max(unit, VFP_TCAMm);
    if ((rv = soc_mem_read_range(unit, VFP_TCAMm, MEM_BLOCK_ALL,
                                 index_min, index_max, 
                                 vfp_tcam_buf)) < 0 ) {
        goto cleanup;
    }
    vfp_policy_buf = soc_cm_salloc(unit, SOC_MEM_TABLE_BYTES
                                  (unit, VFP_POLICY_TABLEm), 
                                  "VFP POLICY TABLE buffer");
    if (NULL == vfp_policy_buf) {
        return BCM_E_MEMORY;
    }
    index_min = soc_mem_index_min(unit, VFP_POLICY_TABLEm);
    index_max = soc_mem_index_max(unit, VFP_POLICY_TABLEm);
    if ((rv = soc_mem_read_range(unit, VFP_POLICY_TABLEm, MEM_BLOCK_ALL,
                                 index_min, index_max, 
                                 vfp_policy_buf)) < 0 ) {
        goto cleanup;
    }
    entries_per_slice = 256;

    /* Iterate over the slices */
    if ((rv = READ_VFP_SLICE_CONTROLr(unit, &rval)) < 0) {
        goto cleanup;
    }
    if ((rv = READ_VFP_KEY_CONTROLr(unit, &vfp_key)) < 0) {
        goto cleanup;
    }
    for (slice_idx = 0; slice_idx < stage_fc->tcam_slices; slice_idx++) {
        /* Ignore disabled slice */
        if ((soc_reg_field_get(unit, VFP_SLICE_CONTROLr, rval, 
                               vfp_en_flds[slice_idx]) == 0) || 
            (soc_reg_field_get(unit, VFP_SLICE_CONTROLr, rval, 
                               vfp_lk_en_flds[slice_idx]) == 0)) {
            continue;
        }
        /* Ignore secondary slice in paired mode */
        fld = _vfp_slice_pairing_field[slice_idx / 2];
        paired = soc_reg_field_get(unit, VFP_KEY_CONTROLr, vfp_key, fld);
        fld = _vfp_slice_wide_mode_field[slice_idx];
        intraslice = soc_reg_field_get(unit, VFP_KEY_CONTROLr, vfp_key, fld);
        if (paired && (slice_idx % 2)) {
            continue;
        }
        /* Skip if slice has no valid groups and entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        for (idx = 0; idx < fs->entry_count; idx++) {
            vfp_tcam_entry = soc_mem_table_idx_to_pointer
                                 (unit, VFP_TCAMm, vfp_tcam_entry_t *, 
                                  vfp_tcam_buf, idx + 
                                  entries_per_slice * slice_idx);
            if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry, 
                                          VALIDf) != 0) {
                break;
            }
        }
        if ((idx == fs->entry_count) && !fc->l2warm) {
            continue;
        }
        /* If Level 2, retrieve the GIDs in this slice */
        if (fc->l2warm) {
            rv = _field_scache_slice_group_recover(unit, fc, slice_idx, 
                                                       NULL);
            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }
        /* Construct the group based on HW selector values */
        sal_memset(&hw_sels, 0, sizeof(_field_hw_qual_info_t));
        /* Get primary slice's selectors */
        hw_sels.pri_slice[0].fpf2 = soc_reg_field_get(unit, VFP_KEY_CONTROLr, 
                                    vfp_key, _vfp_field_tbl[slice_idx][0]);
        hw_sels.pri_slice[0].fpf3 = soc_reg_field_get(unit, VFP_KEY_CONTROLr, 
                                    vfp_key, _vfp_field_tbl[slice_idx][1]);
        /* If intraslice, get double-wide key - only 2 options */
        if (intraslice) {
            dbl_wide_key = soc_reg_field_get(unit, VFP_KEY_CONTROLr, vfp_key, 
                                     _vfp_slice_wide_mode_field[slice_idx]);
            hw_sels.pri_slice[1].intraslice = 1;
            hw_sels.pri_slice[1].fpf2 = dbl_wide_key;
            hw_sels.pri_slice[1].fpf3 = 0;
        }
        /* If paired, get secondary slice's selectors */
        if (paired) {
            hw_sels.sec_slice[0].fpf2 = soc_reg_field_get(unit, 
                                                          VFP_KEY_CONTROLr,
                                                          vfp_key, 
                                          _vfp_field_tbl[slice_idx + 1][0]);
            hw_sels.sec_slice[0].fpf3 = soc_reg_field_get(unit, 
                                                          VFP_KEY_CONTROLr, 
                                                          vfp_key, 
                                          _vfp_field_tbl[slice_idx + 1][1]); 
            if (intraslice) {
                dbl_wide_key_sec = soc_reg_field_get(unit, VFP_KEY_CONTROLr,
                            vfp_key, _vfp_slice_wide_mode_field[slice_idx + 1]);
                hw_sels.sec_slice[1].intraslice = 1;
                hw_sels.sec_slice[1].fpf2 = dbl_wide_key_sec;
                hw_sels.sec_slice[1].fpf3 = 0;                
            }
        }
        /* Create a group based on HW qualifiers (or find existing) */
        rv = _field_fb2_group_construct(unit, &hw_sels, intraslice, 
                                        paired, fc, -1, 
                                        _BCM_FIELD_STAGE_LOOKUP,
                                        slice_idx);
        if (BCM_FAILURE(rv)) {
            goto cleanup;
        }
        /* Now go over the entries */
        fs = stage_fc->slices[_FP_DEF_INST] + slice_idx;
        if (fs->group_flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) {
            ratio = 2;
        } else {
            ratio = 1;
        }
        prev_prio = -1;
        for (idx = 0; idx < fs->entry_count / ratio; idx++) {
            group_found = 0;
            vfp_tcam_entry = soc_mem_table_idx_to_pointer
                                 (unit, VFP_TCAMm, vfp_tcam_entry_t *, 
                                  vfp_tcam_buf, idx + 
                                  entries_per_slice * slice_idx);
            if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry, 
                                          VALIDf) == 0) {
                continue;
            }
            /* All ports are applicable to this entry */
            SOC_PBMP_ASSIGN(entry_pbmp, PBMP_ALL(unit));
            /* Search groups to find match */
            fg = fc->groups;
            while (fg != NULL) {
                /* Check if group is in this slice */
                fs = &fg->slices[0];
                if (fs->slice_number != slice_idx) {
                    fg = fg->next;
                    continue;
                }
                /* Check if entry_pbmp is a subset of group pbmp */
                SOC_PBMP_ASSIGN(temp_pbmp, fg->pbmp);
                SOC_PBMP_AND(temp_pbmp, entry_pbmp); 
                if (SOC_PBMP_EQ(temp_pbmp, entry_pbmp)) {
                    group_found = 1;
                    break;
                }
                fg = fg->next;
            }
            if (!group_found) {
                return BCM_E_INTERNAL; /* Should never happen */
            }
            /* Allocate memory for the entry */
            rv = _bcm_field_entry_tcam_parts_count(unit, fg->stage_id,
                                                   fg->flags, &parts_count);
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
            mem_sz = parts_count * sizeof (_field_entry_t);
            _FP_XGS3_ALLOC(f_ent, mem_sz, "field entry");
            if (f_ent == NULL) {
                rv = BCM_E_MEMORY;
                goto cleanup;
            }
            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 f_ent->policer[idx1].pid = _FP_INVALID_INDEX;
            }

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = f_ent_wb_info.pid = -1;

            if (fc->l2warm) {
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
                   action_bmp.w = NULL;
                   _FP_XGS3_ALLOC(action_bmp.w,
                             SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                             "Action No Param Bitmap");
                   if (action_bmp.w == NULL) {
                       rv = BCM_E_MEMORY;
                       sal_free(f_ent);
                       goto cleanup;
                   }
                }
                f_ent_wb_info.action_bmp = &action_bmp;
                rv = _field_entry_info_retrieve(unit, fc, stage_fc,
                                                0, &prev_prio,
                                                &f_ent_wb_info);
                
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
            } else {
                _bcm_field_last_alloc_eid_incr(unit);
            }
            pri_tcam_idx = idx + entries_per_slice * slice_idx;
            for (i = 0; i < parts_count; i++) {
                if (fc->l2warm) {
                    /* Use retrieved EID */
                    f_ent[i].eid = f_ent_wb_info.eid;
                    /* Set retrieved dvp_type and svp_type */
                    f_ent[i].dvp_type = f_ent_wb_info.dvp_type[i];
                    f_ent[i].svp_type = f_ent_wb_info.svp_type[i];
                    f_ent[i].prio = f_ent_wb_info.prio;
                } else {
                    f_ent[i].eid = _bcm_field_last_alloc_eid_get(unit);
                    f_ent[i].prio = (slice_idx << 10) | (entries_per_slice - idx);
                }
                f_ent[i].group = fg;
                rv = _bcm_field_tcam_part_to_entry_flags(unit, i, fg,
                                                         &f_ent[i].flags);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (f_ent_wb_info.color_independent) {
                    f_ent[i].flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }
                rv = _bcm_field_entry_part_tcam_idx_get(unit, f_ent, 
                                                        pri_tcam_idx,
                                                        i, &part_index);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                                                         _FP_DEF_INST,
                                                         part_index, 
                                                         &slice_number,
                                                (int *)&f_ent[i].slice_idx);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                f_ent[i].fs = stage_fc->slices[_FP_DEF_INST] + slice_number;
                if (0 == (f_ent[i].flags & _FP_ENTRY_SECOND_HALF)) {
                    /* Decrement slice free entry count for primary 
                       entries. */
                    f_ent[i].fs->free_count--;
                }
                /* Assign entry to a slice */
                f_ent[i].fs->entries[f_ent[i].slice_idx] = f_ent + i;
                BCM_PBMP_OR(f_ent[i].fs->pbmp, fg->pbmp);
                f_ent[i].flags |= _FP_ENTRY_INSTALLED;

                if (soc_VFP_TCAMm_field32_get(unit, vfp_tcam_entry, VALIDf) == 3) {
                    f_ent[i].flags |= _FP_ENTRY_ENABLED;
                }

                /* tcam_entry_buf must point to the entry part */
                vfp_tcam_entry_buf = (uint32 *)soc_mem_table_idx_to_pointer
                                             (unit, VFP_TCAMm, 
                                              vfp_tcam_entry_t *, 
                                              vfp_tcam_buf, part_index);

                /* Use this part to determine fg->qual_arr[i] */
                /* Iterate over the HW qualifiers and match offsets with
                   the non-zero masks */
                for (j = 0; j < fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].size; j++) {
                    qual_idx = fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].qid_arr[j];
                    f_qual_arr = stage_fc->f_qual_arr[qual_idx];
                    /* Loop over configurations of this qualifier */
                    for (k = 0; k < f_qual_arr->conf_sz; k++) {
                        if (_mask_is_set(unit, &f_qual_arr->conf_arr[k].offset,
                             vfp_tcam_entry_buf, _BCM_FIELD_STAGE_LOOKUP)) {
                            sal_memcpy(&fg->qual_arr[_FP_ENTRY_TYPE_DEFAULT][i].offset_arr[j],
                                       &f_qual_arr->conf_arr[k].offset,
                                       sizeof(f_qual_arr->conf_arr[k].offset));
                            break;
                        } 
                    }
                }
                /* Get the actions associated with this part of the entry */
                vfp_policy_entry = soc_mem_table_idx_to_pointer
                                                (unit, VFP_POLICY_TABLEm, 
                                                 vfp_policy_table_entry_t *, 
                                                 vfp_policy_buf, part_index);
                rv = _field_fb_actions_recover(unit, (uint32 *)vfp_policy_entry,  
                                               f_ent, i, VFP_POLICY_TABLEm,
                                               &f_ent_wb_info);
                if (BCM_FAILURE(rv)) {
                    sal_free(f_ent);
                    goto cleanup;
                }
                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }
            }
            /* Add to the group */
            rv = _field_group_entry_add(unit, fg, f_ent);
            if (BCM_FAILURE(rv)) {
                sal_free(f_ent);
                goto cleanup;
            }
            f_ent = NULL;
        }
        /* Free up the temporary slice group info */
        if (fc->l2warm) {
            _field_scache_slice_group_free(unit, fc, slice_idx);
        }
    }

    if (fc->l2warm) {

        /* Mark end of Slice Info */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            fc->scache_pos++;
        }

        /* Group Slice Selectors */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            rv = (_field_group_slice_selectors_recover(unit,
                        &buf[fc->scache_pos], stage_fc->stage_id));
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        temp = 0;
        temp |= buf[fc->scache_pos];
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        if (temp != _FIELD_VFP_DATA_END) {
            fc->l2warm = 0;
            rv = BCM_E_INTERNAL;
        }

        if (NULL != buf1) {
            temp = 0;
            temp |= buf1[fc->scache_pos1];
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 8;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 16;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 24;
            fc->scache_pos1++;
            if (temp != _FIELD_VFP_DATA_END) {
                fc->l2warm = 0;
                rv = BCM_E_INTERNAL;
            }
        }

    }
cleanup:
    if (vfp_tcam_buf) {
        soc_cm_sfree(unit, vfp_tcam_buf);
    }
    if (vfp_policy_buf) {
        soc_cm_sfree(unit, vfp_policy_buf);
    }
    if (action_bmp.w != NULL) {
        _FP_ACTION_BMP_FREE(action_bmp);
        action_bmp.w = NULL;
    }
    return rv;
}
#endif /* BCM_FIREBOLT2_SUPPORT */
#else
#define _field_fb_stage_reinit (NULL)
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT
typedef enum
{
    _fieldFBSliceModeSingle = 0,
    _fieldFBSliceModeDouble = 1,
    _fieldFBSliceModeTriple = 2,
    _fieldFBSliceModeCount
} _field_fb_slice_mode_t;

typedef struct
{
    int8 fpf1;
    int8 fpf2;
    int8 fpf3;
    int8 fpf4;
} _field_fb_sel_codes_t;

STATIC int _field_fb_group_construct(int unit, _field_control_t *fc,
    uint32 slice_mode, _field_stage_id_t stage_id, int slice_index,
    bcm_port_t port, _field_fb_sel_codes_t *hw_sel_codes)
{
    int rv;
    _field_stage_t *stage_fc;
    _field_group_t *fg_p;
    int part_index;
    bcm_field_group_t group_id;
    int               priority = 0;
    int               group_flags = 0;
    uint16 qualifier_id;
    _bcm_field_qual_info_t *stage_qualifier_p;
    int config_index;
    _bcm_field_qual_conf_t *config_p = NULL;
    _bcm_field_selector_t selector;
    int qualifier_id_index;
    _bcm_field_group_qual_t *part_qualifier_p;
    int old_qualifier_count;
    int new_qualifier_count;
    uint16 *qualifier_ids_p;
    _bcm_field_qual_offset_t *qualifier_offsets_p;
    _bcm_field_qual_offset_t *offset_p;
    bcm_field_qset_t qset;
    int action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    int idx;
    bcm_pbmp_t  group_pbmp;

    bcm_field_qset_t_init(&qset);

    BCM_IF_ERROR_RETURN(_field_stage_control_get(unit, stage_id, &stage_fc));

    for (fg_p = fc->groups; fg_p != NULL; fg_p = fg_p->next)
    {
        if (((slice_mode == _fieldFBSliceModeSingle) &&
                ((fg_p->flags & _FP_GROUP_SPAN_SINGLE_SLICE) == 0)) ||
            ((slice_mode == _fieldFBSliceModeDouble) &&
                ((fg_p->flags & _FP_GROUP_SPAN_DOUBLE_SLICE) == 0)) ||
            ((slice_mode == _fieldFBSliceModeTriple) &&
                ((fg_p->flags & _FP_GROUP_SPAN_TRIPLE_SLICE) == 0)))
        {
            /* This group isn't the same width as the one
               we're reconstructing */

            continue;
        }

        /* Check for primary slice numbers - there may be groups with the same
           QSET but residing in different slices */
        if (slice_index != fg_p->slices[0].slice_number) {
            continue;
        }

        /* See if selector codes match */

        for (part_index = 0; part_index <= slice_mode; ++part_index)
        {
            if ((fg_p->sel_codes[part_index].fpf1 !=
                    hw_sel_codes[part_index].fpf1) ||
                (fg_p->sel_codes[part_index].fpf2 !=
                    hw_sel_codes[part_index].fpf2) ||
                (fg_p->sel_codes[part_index].fpf3 !=
                    hw_sel_codes[part_index].fpf3) ||
                (fg_p->sel_codes[part_index].fpf4 !=
                    hw_sel_codes[part_index].fpf4))
            {
                /* Selectors don't match */

                break;
            }
        }

        if (part_index <= slice_mode)
        {
            /* Found a mismatch */

            continue;
        }

        /*
         * If we've gotten this far, everything matches, so add
         * the port to the group's PBMP
         */
        BCM_PBMP_PORT_ADD(fg_p->pbmp, port);
        BCM_PBMP_OR(fg_p->slices[0].pbmp, fg_p->pbmp);

            group_flags = 0;
        if (fc->l2warm)
        {
            /* Get stored group ID and qset for level 2 */

            BCM_IF_ERROR_RETURN(_field_group_info_retrieve(unit, port,
                                                           _FP_DEF_INST,
                                                           &group_id,
                                                           &priority,
                                                           &action_res_id,
                                                           &group_pbmp,
                                                           NULL,
                                                           &group_flags,
                                                           &qset,
                                                           fc
                                                           )
                                );

            if (group_id != -1)
            {
                sal_memcpy(&fg_p->qset, &qset, sizeof(bcm_field_qset_t));

                fg_p->gid      = group_id;
                fg_p->priority = priority;
                fg_p->action_res_id = action_res_id;
                if (group_flags & _FP_GROUP_SELECT_AUTO_EXPANSION) {
                    fg_p->flags = fg_p->flags | 
                                       _FP_GROUP_SELECT_AUTO_EXPANSION;
                }
            }
        }

        return BCM_E_NONE;
    }

    /* No existing groups match; create a new one */

    _FP_XGS3_ALLOC(fg_p, sizeof(_field_group_t), "field group");

    if (fg_p == NULL)
    {
        return BCM_E_MEMORY;
    }


    /* Initialize action res id and VMAP group to default */
    action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    for (idx = 0; idx < _FP_PAIR_MAX; idx++) {
        fg_p->vmap_group[idx] = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    }

        group_flags = 0;
        if (fc->l2warm)
        {
                /* Get stored group ID and QSET */

            rv = _field_group_info_retrieve(unit, port, _FP_DEF_INST,
                                            &group_id,
                                            &priority,
                                            &action_res_id,
                                            &group_pbmp,
                                            NULL,
                                            &group_flags,
                                            &qset,
                                            fc
                                            );

        if (group_id != -1)
        {
            sal_memcpy(&fg_p->qset, &qset, sizeof(bcm_field_qset_t));
        } else {
            sal_free(fg_p);
            return rv;
        }
        }
        else
        {
                rv = _bcm_field_group_id_generate(unit, &group_id);
                priority = slice_index;
        }

    if (BCM_FAILURE(rv))
    {
        sal_free(fg_p);

        return rv;
    }

    fg_p->gid = group_id;
    fg_p->priority = priority;
    fg_p->action_res_id = action_res_id;
    if (group_flags & _FP_GROUP_SELECT_AUTO_EXPANSION) {
        fg_p->flags = fg_p->flags | _FP_GROUP_SELECT_AUTO_EXPANSION;
    }
    fg_p->stage_id = stage_id;

    BCM_PBMP_PORT_ADD(fg_p->pbmp, port);

    for (part_index = 0; part_index <= slice_mode; ++part_index)
    {
        fg_p->sel_codes[part_index].fpf1 = hw_sel_codes[part_index].fpf1;
        fg_p->sel_codes[part_index].fpf2 = hw_sel_codes[part_index].fpf2;
        fg_p->sel_codes[part_index].fpf3 = hw_sel_codes[part_index].fpf3;
        fg_p->sel_codes[part_index].fpf4 = hw_sel_codes[part_index].fpf4;
    }

    switch (slice_mode)
    {
    case _fieldFBSliceModeSingle:
        fg_p->flags |= _FP_GROUP_SPAN_SINGLE_SLICE;
        break;

    case _fieldFBSliceModeDouble:
        fg_p->flags |= _FP_GROUP_SPAN_DOUBLE_SLICE;
        break;

    case _fieldFBSliceModeTriple:
        fg_p->flags |= _FP_GROUP_SPAN_TRIPLE_SLICE;
        break;

    }

    /* Build qset from selectors */

    for (part_index = 0; part_index <= slice_mode; ++part_index)
    {
        for (qualifier_id = 0; qualifier_id < _bcmFieldQualifyCount;
            ++qualifier_id)
        {
            stage_qualifier_p = stage_fc->f_qual_arr[qualifier_id];

            if (stage_qualifier_p == NULL)
            {
                /* Qualifier does not exist in this stage */

                continue;
            }

            for (config_index = 0; config_index < stage_qualifier_p->conf_sz;
                ++config_index)
            {
                config_p = stage_qualifier_p->conf_arr + config_index;

                selector = config_p->selector;

                /* Check for F1 match */

                if ((selector.pri_sel == _bcmFieldSliceSelFpf1) &&
                    (selector.pri_sel_val ==
                        fg_p->sel_codes[part_index].fpf1))
                {
                    break;
                }

                /* Check for F2 match */

                if ((selector.pri_sel == _bcmFieldSliceSelFpf2) &&
                    (selector.pri_sel_val ==
                        fg_p->sel_codes[part_index].fpf2))
                {
                    break;
                }

                /* Check for F3 match */

                if ((selector.pri_sel == _bcmFieldSliceSelFpf3) &&
                    (selector.pri_sel_val ==
                        fg_p->sel_codes[part_index].fpf3))
                {
                    break;
                }

                /* Check for F4 match */

                if ((selector.pri_sel == _bcmFieldSliceSelFpf4) &&
                    (selector.pri_sel_val ==
                        fg_p->sel_codes[part_index].fpf4))
                {
                    break;
                }
            }

            if (config_index < stage_qualifier_p->conf_sz)
            {
                /* Found a match; add the qualifier to the qset */

                BCM_FIELD_QSET_ADD(fg_p->qset, qualifier_id);

                part_qualifier_p = &(fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part_index]);

                for (qualifier_id_index = 0;
                    qualifier_id_index < part_qualifier_p->size;
                    ++qualifier_id_index)
                {
                    if (part_qualifier_p->qid_arr[qualifier_id_index] ==
                        qualifier_id)
                    {
                        /* Qualifier already in the qset */

                        break;
                    }
                }

                if (qualifier_id_index == part_qualifier_p->size)
                {
                    /* Qualifier not already in the qset; add it */

                    old_qualifier_count = part_qualifier_p->size;
                    new_qualifier_count = old_qualifier_count + 1;

                    qualifier_ids_p = NULL;
                    qualifier_offsets_p = NULL;

                    _FP_XGS3_ALLOC(qualifier_ids_p, new_qualifier_count *
                        sizeof (uint16), "Group qualifier ids");

                    if (qualifier_ids_p == NULL)
                    {
                        sal_free(fg_p);
                        return BCM_E_MEMORY;
                    }

                    _FP_XGS3_ALLOC(qualifier_offsets_p, new_qualifier_count *
                        sizeof (_bcm_field_qual_offset_t),
                        "Group qual offsets");

                    if (qualifier_offsets_p == NULL)
                    {
                        sal_free(fg_p);
                        sal_free(qualifier_ids_p);
                        return BCM_E_MEMORY;
                    }

                    if (old_qualifier_count > 0)
                    {
                        /* Copy existing data */

                        sal_memcpy(qualifier_ids_p, part_qualifier_p->qid_arr,
                            old_qualifier_count * sizeof (uint16));

                        sal_memcpy(qualifier_offsets_p,
                            part_qualifier_p->offset_arr,
                            old_qualifier_count *
                                sizeof (_bcm_field_qual_offset_t));

                        sal_free(part_qualifier_p->qid_arr);
                        sal_free(part_qualifier_p->offset_arr);
                    }

                    part_qualifier_p->qid_arr = qualifier_ids_p;
                    part_qualifier_p->offset_arr = qualifier_offsets_p;

                    part_qualifier_p->qid_arr[old_qualifier_count] =
                        qualifier_id;

                    offset_p = part_qualifier_p->offset_arr +
                        old_qualifier_count;

                    sal_memcpy(offset_p, &config_p->offset,
                               sizeof(config_p->offset));

                    part_qualifier_p->size = new_qualifier_count;

                    _field_qset_udf_bmap_reinit(unit,
                                                stage_fc,
                                                &fg_p->qset,
                                                qualifier_id
                                                );
                }
            }
        }
    }

    /* Associate slice with group */

    if (fg_p->slices == NULL)
    {
        fg_p->slices = stage_fc->slices[_FP_DEF_INST] + slice_index;
    }

    BCM_PBMP_OR(fg_p->slices[0].pbmp, fg_p->pbmp);

    if (fc->l2warm
            && ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15)) {
        if (group_flags & _FP_GROUP_LOOKUP_ENABLED) {
            fg_p->flags = fg_p->flags | _FP_GROUP_LOOKUP_ENABLED;
        }
        if (group_flags & _FP_GROUP_WLAN) {
            fg_p->flags = fg_p->flags | _FP_GROUP_WLAN;
        }
    } else {
        fg_p->flags |= _FP_GROUP_LOOKUP_ENABLED;
    }

    fg_p->next = fc->groups;
    fc->groups = fg_p;

    return BCM_E_NONE;
}

int _field_fb_slice_is_primary(int unit, int slice_index,
    int *is_primary_p, int *slice_mode_p)
{
    uint32 fp_slice_config_val;

    soc_field_t slice_mode_fields[16] =
    {
        SLICE_0_MODEf, SLICE_1_MODEf, SLICE_2_MODEf, SLICE_3_MODEf,
        SLICE_4_MODEf, SLICE_5_MODEf, SLICE_6_MODEf, SLICE_7_MODEf,
        SLICE_8_MODEf, SLICE_9_MODEf, SLICE_10_MODEf, SLICE_11_MODEf,
        SLICE_12_MODEf, SLICE_13_MODEf, SLICE_14_MODEf, SLICE_15_MODEf
    };

    BCM_IF_ERROR_RETURN(READ_FP_SLICE_CONFIGr(unit, &fp_slice_config_val));

    *slice_mode_p = soc_reg_field_get(unit, FP_SLICE_CONFIGr,
        fp_slice_config_val, slice_mode_fields[slice_index]);

    if ((*slice_mode_p == _fieldFBSliceModeDouble && slice_index % 2 != 0) ||
        (*slice_mode_p == _fieldFBSliceModeTriple && slice_index %4 != 0))
    {
        *is_primary_p = 0;
    }
    else
    {
        *is_primary_p = 1;
    }

    return BCM_E_NONE;
}

#define _FP_FB_IF_FAILURE_CLEANUP(op) \
    rv = (op); if (BCM_FAILURE(rv)) goto cleanup;

int _field_fb_stage_reinit(int unit, _field_control_t *fc,
    _field_stage_t *stage_fc)
{
    int rv, idx1;
    int temp;
    char *fp_tcam_buffer = NULL;
    char *fp_port_field_sel_buffer = NULL;
    int slice_index;
    int slice_mode;
    _field_slice_t *fs_p;
    int entry_index;
    bcm_port_t port;
    _field_fb_sel_codes_t sel_codes[_fieldFBSliceModeCount];
    _field_fb_sel_codes_t *sel_codes_p;
    uint32 fp_f4_select_value;
    char *fp_policy_table_buffer = NULL;
    fp_tcam_entry_t *fp_tcam_entry_p;
    bcm_pbmp_t entry_pbmp;
    _field_group_t *fg_p;
    _field_slice_t *group_slice_p;
    int part_count;
    _field_entry_t *entries_p;
    int primary_tcam_index;
    int part_slice_index;
    int part_tcam_index;
    int part_index;
    fp_port_field_sel_entry_t *port_field_sel_entry_p;
    _field_entry_t *entry_p = NULL;
    _bcm_field_group_qual_t *part_qualifier_p;
    int group_qualifier_index;
    int qualifier_id;
    _bcm_field_qual_info_t *stage_qualifier_p;
    int config_index;
    _bcm_field_qual_offset_t *config_offset_p;
    _bcm_field_qual_offset_t *group_qualifier_offset_p;
    fp_policy_table_entry_t *policy_entry_p;
    uint32 ipbm_sel_value, pbm;
    _bcm_field_selector_t config_selector;
    int selector;
    uint8 selector_value;
    int multigroup;
    int prev_prio;
    int slice_is_primary;
    uint8 *buf = fc->scache_ptr[_FIELD_SCACHE_PART_0];
    uint8 *buf1 = fc->scache_ptr[_FIELD_SCACHE_PART_1];
    bcm_pbmp_t temp_pbmp;
    _field_action_bmp_t action_bmp;
    _field_entry_wb_info_t f_ent_wb_info;

    soc_field_t slice_f1_fields[16] =
    {
        SLICE0_F1f, SLICE1_F1f, SLICE2_F1f, SLICE3_F1f,
        SLICE4_F1f, SLICE5_F1f, SLICE6_F1f, SLICE7_F1f,
        SLICE8_F1f, SLICE9_F1f, SLICE10_F1f, SLICE11_F1f,
        SLICE12_F1f, SLICE13_F1f, SLICE14_F1f, SLICE15_F1f,
    };

    soc_field_t slice_f2_fields[16] =
    {
        SLICE0_F2f, SLICE1_F2f, SLICE2_F2f, SLICE3_F2f,
        SLICE4_F2f, SLICE5_F2f, SLICE6_F2f, SLICE7_F2f,
        SLICE8_F2f, SLICE9_F2f, SLICE10_F2f, SLICE11_F2f,
        SLICE12_F2f, SLICE13_F2f, SLICE14_F2f, SLICE15_F2f,
    };

    soc_field_t slice_f3_fields[16] =
    {
        SLICE0_F3f, SLICE1_F3f, SLICE2_F3f, SLICE3_F3f,
        SLICE4_F3f, SLICE5_F3f, SLICE6_F3f, SLICE7_F3f,
        SLICE8_F3f, SLICE9_F3f, SLICE10_F3f, SLICE11_F3f,
        SLICE12_F3f, SLICE13_F3f, SLICE14_F3f, SLICE15_F3f,
    };

    soc_field_t slice_f4_fields[16] =
    {
        SLICE_0_F4f, SLICE_1_F4f, SLICE_2_F4f, SLICE_3_F4f,
        SLICE_4_F4f, SLICE_5_F4f, SLICE_6_F4f, SLICE_7_F4f,
        SLICE_8_F4f, SLICE_9_F4f, SLICE_10_F4f, SLICE_11_F4f,
        SLICE_12_F4f, SLICE_13_F4f, SLICE_14_F4f, SLICE_15_F4f,
    };

    
    /* Clear selector codes, set as invalid */
    sal_memset(&sel_codes, (int8)_FP_SELCODE_DONT_CARE,
        sizeof(_field_fb_sel_codes_t));

    /* Reset Action bitmap to NULL. */
    action_bmp.w = NULL;

    fc->scache_pos = 0;
    fc->scache_pos1 = 0;

    if (fc->l2warm) {
        if ((fc->wb_recovered_version) > (fc->wb_current_version)) {
            /* Notify the application with an event */
            /* The application will then need to reconcile the 
               version differences using the documented behavioral
               differences on per module (handle) basis */
            SOC_IF_ERROR_RETURN
                (soc_event_generate(unit, SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE, 
                                    BCM_MODULE_FIELD, (fc->wb_recovered_version), 
                                    (fc->wb_current_version)));        
        }

        fc->scache_pos += SOC_WB_SCACHE_CONTROL_SIZE;

        if(NULL != fc->scache_ptr[_FIELD_SCACHE_PART_1]) {
            if ((fc->wb_recovered_version) > (fc->wb_current_version)) {
                /* Notify the application with an event */
                /* The application will then need to reconcile the
                   version differences using the documented behavioral
                   differences on per module (handle) basis */
                SOC_IF_ERROR_RETURN
                    (soc_event_generate(unit, 
                                        SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
                                        BCM_MODULE_FIELD, (fc->wb_recovered_version),
                                        (fc->wb_current_version)));
            }

            fc->scache_pos1 += SOC_WB_SCACHE_CONTROL_SIZE;
        }
    }

    if (stage_fc->stage_id != _BCM_FIELD_STAGE_INGRESS)
    {
        return BCM_E_PARAM;
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "Beginning ingress stage recovery.\n")));

    if (fc->l2warm) {
        rv = _field_scache_stage_hdr_chk(fc, _FIELD_IFP_DATA_START);
        if (BCM_FAILURE(rv)) {
            return (rv);
        }
    }

    /* Read TCAM into memory */

    _FP_FB_IF_FAILURE_CLEANUP(_field_table_read(unit, FP_TCAMm,
        &fp_tcam_buffer, "FP_TCAM buffer"));

    /* Read FP_PORT_FIELD_SEL into memory */

    _FP_FB_IF_FAILURE_CLEANUP(_field_table_read(unit,
        FP_PORT_FIELD_SELm, &fp_port_field_sel_buffer,
        "FP_PORT_FIELD_SEL buffer"));

    /* Read FP_POLICY_TABLE table into memory */

    _FP_FB_IF_FAILURE_CLEANUP(_field_table_read(unit,
        FP_POLICY_TABLEm, &fp_policy_table_buffer,
        "FP_POLICY_TABLE"));

    /* Read F4 selectors */

    _FP_FB_IF_FAILURE_CLEANUP(READ_FP_F4_SELECTr(unit, &fp_f4_select_value));

    /* Recover range checkers */
    rv = _field_range_check_reinit(unit, stage_fc, fc);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Recover data qualifiers */
    rv = _field_data_qual_recover(unit, fc, stage_fc);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    for (slice_index = 0; slice_index < stage_fc->tcam_slices; ++slice_index)
    {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "  Checking slice %d...\n"),
                   slice_index));

        _FP_FB_IF_FAILURE_CLEANUP(_field_fb_slice_is_primary(unit,
            slice_index, &slice_is_primary, &slice_mode));

        if (!slice_is_primary)
        {
            /* Don't need selectors for second slice of double or for
               second and third slices of triple */

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "    Not a primary slice.\n")));

            continue;
        }

        /* Check if the slice has any valid entries */

        fs_p = stage_fc->slices[_FP_DEF_INST] + slice_index;

        for (entry_index = 0; entry_index < fs_p->entry_count; ++entry_index)
        {
            fp_tcam_entry_p = soc_mem_table_idx_to_pointer(unit, FP_TCAMm,
                fp_tcam_entry_t *, fp_tcam_buffer,
                entry_index + fs_p->entry_count * slice_index);

            if (soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, VALIDf) != 0)
            {
               /* There's at least one valid entry in the slice */

                break;
            }
        }

        if ((entry_index == fs_p->entry_count) && !fc->l2warm)
        {
            /* No valid groups and entries in the slice */

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "    No valid groups and entries in slice.\n")));

            continue;
        }

        if (fc->l2warm)
        {
            /* Retrieve the group IDs in this slice */

            rv = _field_scache_slice_group_recover(unit, fc,
                slice_index, &multigroup);

            if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
                fc->l2warm = 0;
                goto cleanup;
            }
            if (rv == BCM_E_NOT_FOUND) {
                rv = BCM_E_NONE;
                continue;
            }
        }

        PBMP_ALL_ITER(unit, port)
        {
            port_field_sel_entry_p = soc_mem_table_idx_to_pointer(
                unit, FP_PORT_FIELD_SELm, fp_port_field_sel_entry_t *,
                fp_port_field_sel_buffer, port);

            for (part_index = 0; part_index < COUNTOF(sel_codes); ++part_index) {
                sel_codes[part_index].fpf1 = _FP_SELCODE_DONT_CARE;
                sel_codes[part_index].fpf2 = _FP_SELCODE_DONT_CARE;
                sel_codes[part_index].fpf3 = _FP_SELCODE_DONT_CARE;
                sel_codes[part_index].fpf4 = _FP_SELCODE_DONT_CARE;
            }

            for (part_index = 0; part_index <= slice_mode; ++part_index)
            {
                sel_codes_p = sel_codes + part_index;

                sel_codes_p->fpf1 = soc_FP_PORT_FIELD_SELm_field32_get(
                    unit, port_field_sel_entry_p,
                        slice_f1_fields[slice_index + part_index]);

                sel_codes_p->fpf2 = soc_FP_PORT_FIELD_SELm_field32_get(
                    unit, port_field_sel_entry_p,
                        slice_f2_fields[slice_index + part_index]);

                sel_codes_p->fpf3 = soc_FP_PORT_FIELD_SELm_field32_get(
                    unit, port_field_sel_entry_p,
                        slice_f3_fields[slice_index + part_index]);

                sel_codes_p->fpf4 = soc_reg_field_get(unit, FP_F4_SELECTr,
                    fp_f4_select_value, slice_f4_fields[slice_index +
                        part_index]);
            }

            _FP_FB_IF_FAILURE_CLEANUP(_field_fb_group_construct(unit,
                fc, slice_mode, stage_fc->stage_id, slice_index, port,
                    sel_codes));
        }

        /* Recover entries in this slice */

        fs_p = stage_fc->slices[_FP_DEF_INST] + slice_index;
        prev_prio = -1;

        for (entry_index = 0; entry_index < fs_p->entry_count; ++entry_index)
        {
            fp_tcam_entry_p = soc_mem_table_idx_to_pointer(unit, FP_TCAMm,
                fp_tcam_entry_t *, fp_tcam_buffer, entry_index +
                    slice_index * fs_p->entry_count);

            if (soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, VALIDf) == 0)
            {
                /* Skip empty entry */

                continue;
            }

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "      Checking entry %d...\n"),
                       entry_index));

            /* Determine pbmp for this entry */

            ipbm_sel_value =
                soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, IPBM_SELf);
            SOC_PBMP_CLEAR(entry_pbmp);

            if ((soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p,
                IPBM_SEL_MASKf) != 0) && (ipbm_sel_value != 0))
            {
                SOC_PBMP_WORD_SET(entry_pbmp, 0, ipbm_sel_value);
            }
            else if ((fp_f4_select_value == 0) && 
                     soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, F4_MASKf)) 
            {
                pbm = soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, F4f) & 
                      soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, F4_MASKf);
                SOC_PBMP_PORT_ADD(entry_pbmp, pbm);
            } 
            else
            {
                SOC_PBMP_ASSIGN(entry_pbmp, PBMP_ALL(unit));
            }

            /* Find a matching group */

            for (fg_p = fc->groups; fg_p != NULL; fg_p = fg_p->next)
            {
                group_slice_p = fg_p->slices;

                if (group_slice_p->slice_number != slice_index)
                {
                    /* Not this slice */

                    continue;
                }

                SOC_PBMP_CLEAR(temp_pbmp);
                SOC_PBMP_ASSIGN(temp_pbmp, fg_p->pbmp);
                SOC_PBMP_OR(temp_pbmp, PBMP_LB(unit));
                SOC_PBMP_AND(temp_pbmp, entry_pbmp); 
                if (SOC_PBMP_EQ(temp_pbmp, entry_pbmp))
                {
                    /* Found a match */

                    break;
                }

                /* Check if entry pbmp is a subset of the group pbmp */

                SOC_PBMP_ITER(entry_pbmp, port)
                {
                    if (SOC_PBMP_MEMBER(fg_p->pbmp, port) == 0)
                    {
                        break;
                    }
                }

                if (port == SOC_PBMP_PORT_MAX)
                {
                    /* Got all the way through with no mismatches */

                    break;
                }
            }

            if (fg_p == NULL)
            {
                /* This should never happen */

                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "Could not find a matching group for entry %d.\n"),
                           entry_index));

                return BCM_E_INTERNAL;
            }

            /* Allocate the entry */

            _FP_FB_IF_FAILURE_CLEANUP(
                _bcm_field_entry_tcam_parts_count(unit, fg_p->stage_id,
                                                  fg_p->flags, &part_count));

            entries_p = NULL;

            _FP_XGS3_ALLOC(entries_p, part_count * sizeof (_field_entry_t),
                "field entry");

            if (entries_p == NULL)
            {
                rv = BCM_E_MEMORY;

                goto cleanup;
            }

            for (idx1 = 0; idx1 < _FP_POLICER_LEVEL_COUNT; idx1++) {
                 entries_p->policer[idx1].pid = _FP_INVALID_INDEX;
            }


            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "        Entry matches group %d.\n"),
                       fg_p->gid));

            sal_memset(&f_ent_wb_info, 0, sizeof(_field_entry_wb_info_t));
            f_ent_wb_info.sid = -1;
            f_ent_wb_info.pid = -1;

            if (fc->l2warm)
            {
                if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
                   action_bmp.w = NULL;
                   _FP_XGS3_ALLOC(action_bmp.w,
                             SHR_BITALLOCSIZE(_bcmFieldActionNoParamCount),
                             "Action No Param Bitmap");
                   if (action_bmp.w == NULL) {
                       rv = BCM_E_MEMORY;
                       sal_free(entries_p);
                       goto cleanup;
                   }
                }
                f_ent_wb_info.action_bmp = &action_bmp;

                rv = _field_entry_info_retrieve(unit, fc, stage_fc,
                    multigroup, &prev_prio, &f_ent_wb_info);

                if (BCM_FAILURE(rv))
                {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "Failed to retrieve entry info.")));

                    sal_free(entries_p);

                    goto cleanup;
                }
                if (action_bmp.w != NULL) {
                   _FP_ACTION_BMP_FREE(action_bmp);
                   action_bmp.w = NULL;
                }

            }
            else
            {
                _bcm_field_last_alloc_eid_incr(unit);

                f_ent_wb_info.eid = _bcm_field_last_alloc_eid_get(unit);
            }

            primary_tcam_index =
                slice_index * fs_p->entry_count + entry_index;

            for (part_index = 0; part_index < part_count; ++part_index)
            {
                entry_p = entries_p + part_index;

                entry_p->eid = f_ent_wb_info.eid;
                entry_p->group = fg_p;
                /* Set retrieved dvp_type and svp_type */
                entry_p->dvp_type = f_ent_wb_info.dvp_type[part_index];
                entry_p->svp_type = f_ent_wb_info.svp_type[part_index];

                /* Set entry flags */
                rv = _bcm_field_tcam_part_to_entry_flags(unit, part_index,
                        fg_p, &entry_p->flags);
                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                if (f_ent_wb_info.color_independent)
                {
                    entry_p->flags |= _FP_ENTRY_COLOR_INDEPENDENT;
                }

                /* Get index of entry in tcam */

                rv = _bcm_field_entry_part_tcam_idx_get(unit, entries_p,
                        primary_tcam_index, part_index, &part_tcam_index);
                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                /* Get slice and index within the slice */

                rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc,
                        _FP_DEF_INST, part_tcam_index, &part_slice_index,
                        (int *) (&entry_p->slice_idx));
                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                /* Point the entry to its slice */

                entry_p->fs = stage_fc->slices[_FP_DEF_INST] + part_slice_index;
                --entry_p->fs->free_count;

                /* Set the slice pbmp */

                BCM_PBMP_OR(entry_p->fs->pbmp, fg_p->pbmp);

                /* Assign entry to slice */

                entry_p->fs->entries[entry_p->slice_idx] = entry_p;
                entry_p->flags |= _FP_ENTRY_INSTALLED;

                if (soc_FP_TCAMm_field32_get(unit, fp_tcam_entry_p, VALIDf) == 3) {
                    entry_p->flags |= _FP_ENTRY_ENABLED;
                }
                /* Skip empty entry */
                fp_tcam_entry_p =
                    (fp_tcam_entry_t *) soc_mem_table_idx_to_pointer(unit,
                    FP_TCAMm, fp_tcam_entry_t *, fp_tcam_buffer,
                    part_tcam_index);

                part_qualifier_p = &(fg_p->qual_arr[_FP_ENTRY_TYPE_DEFAULT][part_index]);

                for (group_qualifier_index = 0;
                    group_qualifier_index < part_qualifier_p->size;
                    ++group_qualifier_index)
                {
                    qualifier_id =
                        part_qualifier_p->qid_arr[group_qualifier_index];

                    stage_qualifier_p = stage_fc->f_qual_arr[qualifier_id];

                    for (config_index = 0;
                        config_index < stage_qualifier_p->conf_sz;
                        ++config_index)
                    {
                        config_offset_p = &((stage_qualifier_p->conf_arr +
                            config_index)->offset);

                        group_qualifier_offset_p =
                            part_qualifier_p->offset_arr +
                                group_qualifier_index;

                        if (_mask_is_set(unit, config_offset_p,
                            (uint32 *) fp_tcam_entry_p,
                                stage_fc->stage_id) != 0)
                        {
                            config_selector = stage_qualifier_p->
                                conf_arr[config_index].selector;

                            selector = config_selector.pri_sel;
                            selector_value = config_selector.pri_sel_val;

                            if ((selector == _bcmFieldSliceSelFpf1 &&
                                selector_value == sel_codes[part_index].fpf1) ||
                                (selector == _bcmFieldSliceSelFpf2 &&
                                selector_value == sel_codes[part_index].fpf2) ||
                                (selector == _bcmFieldSliceSelFpf3 &&
                                selector_value == sel_codes[part_index].fpf3) ||
                                (selector == _bcmFieldSliceSelFpf4 &&
                                selector_value == sel_codes[part_index].fpf4))
                            {
                                /* This qualifier exists in this entry */

                                *group_qualifier_offset_p = *config_offset_p;

                                /* No need to check more configs */

                                break;
                            }
                        }
                    }
                }

                /* Get the actions for this part of the entry */

                policy_entry_p = soc_mem_table_idx_to_pointer(unit,
                    FP_POLICY_TABLEm, fp_policy_table_entry_t *,
                    fp_policy_table_buffer, part_tcam_index);

                rv = _field_fb_actions_recover(unit,
                    (uint32 *)policy_entry_p, entries_p, part_index,
                    FP_POLICY_TABLEm, &f_ent_wb_info);
                if (BCM_FAILURE(rv)) {
                    sal_free(entries_p);
                    goto cleanup;
                }

                entry_p->prio = fc->l2warm ? f_ent_wb_info.prio :
                    (slice_index << 10) | (fs_p->entry_count - entry_index);

            }
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "        Entry id = %d, priority = %d.\n"),
                       entries_p->eid,
                       entries_p->prio));

            /* Add the entry to the group */

            rv = _field_group_entry_add(unit, fg_p, entries_p);
            
            if (rv != BCM_E_NONE) {
                sal_free(entries_p);
            
                goto cleanup;
            }
        }

        if (fc->l2warm)
        {
            _field_scache_slice_group_free(unit, fc, slice_index);
        }
    }

    if (fc->l2warm) {

        /* Mark end of Slice Info */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            fc->scache_pos++;
        }

        /* Group Slice Selectors */
        if ((fc->wb_recovered_version) >= BCM_FIELD_WB_VERSION_1_15) {
            rv = (_field_group_slice_selectors_recover(unit,
                        &buf[fc->scache_pos], stage_fc->stage_id));
            if (BCM_FAILURE(rv)) {
                goto cleanup;
            }
        }

        temp = 0;
        temp |= buf[fc->scache_pos];
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 8;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 16;
        fc->scache_pos++;
        temp |= buf[fc->scache_pos] << 24;
        fc->scache_pos++;
        if (temp != _FIELD_IFP_DATA_END) {
            fc->l2warm = 0;
            rv = BCM_E_INTERNAL;
        }

        if (NULL != buf1) {
            temp = 0;
            temp |= buf1[fc->scache_pos1];
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 8;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 16;
            fc->scache_pos1++;
            temp |= buf1[fc->scache_pos1] << 24;
            fc->scache_pos1++;
            if (temp != _FIELD_IFP_DATA_END) {
                fc->l2warm = 0;
                rv = BCM_E_INTERNAL;
            }
        }
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "Ingress stage recovery complete.\n")));

cleanup:
    if (fp_tcam_buffer != NULL)
    {
        soc_cm_sfree(unit, fp_tcam_buffer);
    }
    if (fp_port_field_sel_buffer != NULL) 
    {
        soc_cm_sfree(unit, fp_port_field_sel_buffer);
    }
    if (fp_policy_table_buffer != NULL) 
    {
        soc_cm_sfree(unit, fp_policy_table_buffer);
    }
    if (action_bmp.w != NULL) {
        _FP_ACTION_BMP_FREE(action_bmp);
        action_bmp.w = NULL;
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "Leaving ingress stage recovery.\n")));

    return rv;
}
#else
#define _field_fb_stage_reinit (NULL)
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *     _field_fb_tcam_policy_mem_get
 *
 * Purpose:
 *     Get tcam & policy memories for a specific chip pipeline stage.
 *
 * Parameters:
 *     unit       -   (IN)BCM device number.
 *     stage_id   -   (IN)FP stage pipeline id. 
 *     tcam_mem   -   (IN)Tcam  memory id.
 *     policy_mem -   (IN)Policy memory id.
 *     
 * Returns:
 *     BCM_E_XXX
 */
int
_field_fb_tcam_policy_mem_get(int unit, _field_stage_id_t stage_id, 
                              soc_mem_t *tcam_mem, soc_mem_t *policy_mem)
{

    /* Input parameters check. */
    if ((NULL == tcam_mem) || (NULL == policy_mem)) {
        return (BCM_E_PARAM);
    }

    if (_BCM_FIELD_STAGE_INGRESS == stage_id) {
        *tcam_mem = FP_TCAMm;
        *policy_mem= FP_POLICY_TABLEm;
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    } else if (soc_feature(unit, soc_feature_field_multi_stage)) {
        if (_BCM_FIELD_STAGE_EGRESS == stage_id) {
            *tcam_mem = EFP_TCAMm;
            *policy_mem = EFP_POLICY_TABLEm;
        } else if (_BCM_FIELD_STAGE_LOOKUP== stage_id) {
            *tcam_mem = VFP_TCAMm;
            *policy_mem = VFP_POLICY_TABLEm;
        } else {
            /* Unknown stage specified */
            (*policy_mem) = (*tcam_mem) = INVALIDm; 
            return (BCM_E_PARAM);
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    } else {
        /* Not ingress stage specified while multi stage is not enabled */
        (*policy_mem) = (*tcam_mem) = INVALIDm; 
        return (BCM_E_UNAVAIL);
    }
    return (BCM_E_NONE);
}

#ifdef BCM_FIREBOLT2_SUPPORT 
/*
 * Function:
 *     _field_fb2_lookup_qualifiers_init
 * Purpose:
 *     Initialize device stage lookup qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_fb2_lookup_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    int           rv;              /* Operation return status.  */
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageLookup,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    /*
     *  Enable the overlay of Sender Ethernet Address onto 
     *  MACSA on ARP/RARP packets.
     */
    rv = soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                ARP_VALIDATION_ENf, 1);
    BCM_IF_ERROR_RETURN(rv);

    /* FPF3 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 0, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, 0, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, 0, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, 0, 8, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 1, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 1, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 1, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, 1, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, 1, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, 1, 8, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 2, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 2, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 2, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, 2, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, 2, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, 2, 8, 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerTpid,
                 _bcmFieldSliceSelFpf3, 3, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterTpid,
                 _bcmFieldSliceSelFpf3, 3, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 3, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGigProxy,
                 _bcmFieldSliceSelFpf3, 3, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelFpf3, 3, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, 3, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, 3, 8, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, 3, 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, 3, 12, 12);

    /* FPF2 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, 32, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf2, 0, 38, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 0, 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, 48, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, 64, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, 80, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, 88, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, 120, 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 1, 24, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 2, 24, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 3, 40, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 3, 56, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 3, 104, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 4, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 4, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 4, 40, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 4, 72, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 4, 104, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 5, 24, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 5, 32, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 5, 40, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 5, 72, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 5, 104, 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                 _bcmFieldSliceSelFpf2, 6, 24, 64);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 6, 88, 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySnap,
                 _bcmFieldSliceSelFpf2, 7, 88, 40);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyLlc,
                 _bcmFieldSliceSelFpf2, 7, 128, 24);
    /* FPF1 */
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocolCommon,
                 _bcmFieldSliceSelFpf1, 0, 152, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 0, 155, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 0, 155, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 0, 158, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 0, 159, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 0, 171, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf1, 0, 173, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf1, 0, 178, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf1, 0, 182, 2);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     0, 8, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     8, 16, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     24, 8, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     32, 6, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     38, 2, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     64, 16, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     80, 8, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     88, 32, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     120, 32, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldDevSelIntrasliceVfpKey, 0,
                     _bcmFieldSliceSelFpf2, 0, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     152, 16, 0, 0, 0, 0, 0);

    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     0, 8, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     8, 16, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     24, 128, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     152, 16, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     168, 8, 0, 0, 0, 0, 0);
    _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldDevSelIntrasliceVfpKey, 1,
                     _bcmFieldSliceSelFpf2, 1, 
                     _bcmFieldSliceSelDisable, 0, 1,
                     176, 8, 0, 0, 0, 0, 0);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb2_egress_qualifiers_init
 * Purpose:
 *     Initialize device stage egress qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_fb2_egress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    _FP_QUAL_DECL;
    _key_fld_ = KEYf;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageEgress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 2, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 18, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 66, 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 114, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 114, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 126, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 127, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 146, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 151, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 152, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 153, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 153, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 166, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 169, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 172, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY4, 176, 5);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 2, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 61, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 69, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 101, 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 133, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTos,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 134, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 146, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 151, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 152, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 153, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 153, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 166, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 169, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 172, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 176, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY1, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 8, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 136, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 137, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 146, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 151, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMirrorCopy,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 152, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 153, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 153, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 165, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 166, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 169, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstHiGig,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 171, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInterfaceClassPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 172, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOutPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 176, 5);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY2, 0, 0);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 2, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 8, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 24, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 40, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 48, 128);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf3, _BCM_FIELD_EFP_KEY3, 0, 0);

    return (BCM_E_NONE);
}
#endif /*BCM_FIREBOLT2_SUPPORT */


/*
 * Function:
 *     _field_fb_ingress_qualifiers_init
 * Purpose:
 *     Initialize device stage ingress qaualifiers 
 *     select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_fb_ingress_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    soc_mem_t     tcam_mem;        /* TCAM memory id.           */
    soc_mem_t     policy_mem;      /* Policy table memory id .  */
    int           offset;          /* Tcam field offset.        */
    int           rv;              /* Operation return status.  */
    _FP_QUAL_DECL;

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    rv = _field_fb_tcam_policy_mem_get(unit, stage_fc->stage_id, 
                                       &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    _key_fld_ = F4f;
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPort,
                 _bcmFieldSliceSelFpf4, 0, 0, 5);

    if (soc_feature(unit, soc_feature_field_qual_drop)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelFpf4, 1, 0, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                     _bcmFieldSliceSelFpf4, 1, 2, 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelFpf4, 1, 4, 1);
    }

    _key_fld_ = KEYf;
    offset = 0;

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStage,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyStageIngress,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp4,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyHiGig,
                 _bcmFieldSliceSelDisable, 0, 0, 0);
    if (SOC_IS_FIREBOLT2(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDrop,
                     _bcmFieldSliceSelDisable, 0, 0, 0);

    }


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf3, 0, 0, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf3, 0, 2, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf3, 0, 4, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf3, 0, 6, 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf3, 0, 9, 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf3, 0, 13, 3);

#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelFpf3, 1, 0, 14);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf3, 1, 14, 3);
        }
    } else 
#endif /* BCM_BRADLEY_SUPPORT */
    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelFpf3, 1, 0, 12);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf3, 1, 12, 3);
        }
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                 _bcmFieldSliceSelFpf3, 1, 0, 8);


#if defined(BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelFpf3, 2, 0, 14);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf3, 2, 14, 3);
        }
    } else 
#endif /* BCM_BRADLEY_SUPPORT */
    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelFpf3, 2, 0, 12);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf3, 2, 12, 3);
        }
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                 _bcmFieldSliceSelFpf3, 2, 0, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf3, 3, 0, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf3, 3, 1, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf3, 3, 2, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf3, 3, 3, 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf3, 3, 5, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf3, 3, 6, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf3, 3, 7, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf3, 3, 8, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf3, 3, 9, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf3, 3, 10, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf3, 3, 11, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf3, 3, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf3, 3, 13, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf3, 3, 14, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf3, 3, 15, 1);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf3, 4, 0, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf3, 4, 6, 8);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf3, 5, 0, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf3, 5, 0, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf3, 5, 12, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf3, 5, 13, 3);

    if (soc_feature(unit, soc_feature_field_wide)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf3, 6, 0, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf3, 6, 8, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelFpf3, 7, 0, 16);
    } 

    /* FPF2 */
    offset += _FIELD_MEM_FIELD_LENGTH(unit, tcam_mem, F3f);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 0, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 0, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 0, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 0, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 0, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 0, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 0, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 0, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 0, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 1, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 1, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 1, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 1, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf2, 1, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf2, 1, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 1, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 1, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 1, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 2, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 2, (offset + 8), 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFlags,
                 _bcmFieldSliceSelFpf2, 2, (offset + 14), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 2, (offset + 16), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf2, 2, (offset + 24), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                 _bcmFieldSliceSelFpf2, 2, (offset + 40), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 2, (offset + 56), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 2, (offset + 64), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 2, (offset + 96), 32);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                 _bcmFieldSliceSelFpf2, 3, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6,
                 _bcmFieldSliceSelFpf2, 4, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                 _bcmFieldSliceSelFpf2, 5, offset, 6);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                 _bcmFieldSliceSelFpf2, 5, (offset + 6), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIp6FlowLabel,
                 _bcmFieldSliceSelFpf2, 5, (offset + 14), 20);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                 _bcmFieldSliceSelFpf2, 5, (offset + 34), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf2, 5, (offset + 42), 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                 _bcmFieldSliceSelFpf2, 5, (offset + 50), 64);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 6, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 6, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 6, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 6, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 6, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 6, (offset + 32), 48);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 6, (offset + 80), 48);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 7, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 7, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 7, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 7, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 7, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                 _bcmFieldSliceSelFpf2, 7, (offset + 32), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcMac,
                 _bcmFieldSliceSelFpf2, 7, (offset + 64), 48);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf2, 8, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf2, 8, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf2, 8, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf2, 8, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf2, 8, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                 _bcmFieldSliceSelFpf2, 8, (offset + 32), 32);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstMac,
                 _bcmFieldSliceSelFpf2, 8, (offset + 64), 48);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData0,
                 _bcmFieldSliceSelFpf2, 9, offset, 128);

    _FP_QUAL_ADD(unit, stage_fc, _bcmFieldQualifyData1,
                 _bcmFieldSliceSelFpf2, 0xA, offset, 128);

    if (soc_feature(unit, soc_feature_field_qual_Ip6High)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp6High,
                     _bcmFieldSliceSelFpf2, 0xb, offset + 64, 64);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp6High,
                     _bcmFieldSliceSelFpf2, 0xb, offset, 64);
    }


#ifdef BCM_FIREBOLT2_SUPPORT
    if (SOC_IS_FIREBOLT2(unit)) { 
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelFpf2, 0xc, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 8), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 14), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 16), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 24), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 40), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 56), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 64), 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelFpf2, 0xc, (offset + 96), 32);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelFpf2, 0xd, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 8), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 14), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 16), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 24), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 40), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 56), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 64), 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelFpf2, 0xd, (offset + 96), 32);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                     _bcmFieldSliceSelFpf2, 0xe, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 8), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 14), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 16), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 24), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 40), 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 56), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 64), 32);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                     _bcmFieldSliceSelFpf2, 0xe, (offset + 96), 32);

    }

#endif /* BCM_FIREBOLT2_SUPPORT */

    /* FPF1 */
    offset += _FIELD_MEM_FIELD_LENGTH(unit, tcam_mem, F2f);

#if defined (BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                     _bcmFieldSliceSelFpf1, 0, offset, 21);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelFpf1, 0, (offset + 21), 2);
    } else 
#endif /* BCM_BRADLEY_SUPPORT */
    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInPorts,
                     _bcmFieldSliceSelFpf1, 0, offset, 29);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                         _bcmFieldSliceSelFpf1, 0, (offset + 29), 2);

        }
#ifdef BCM_FIREBOLT2_SUPPORT
        if (SOC_IS_FIREBOLT2(unit)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                         _bcmFieldSliceSelFpf1, 0, (offset + 31), 1);
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
    }

#if defined (BCM_BRADLEY_SUPPORT)
    if (SOC_IS_HB_GW(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldSliceSelFpf1, 1, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelFpf1, 1, offset, 14);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldSliceSelFpf1, 1, (offset + 14), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelFpf1, 1, (offset + 14), 14);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                     _bcmFieldSliceSelFpf1, 1, (offset + 28), 3);
    } else 
#endif /* BCM_BRADLEY_SUPPORT */

    {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstTrunk,
                     _bcmFieldSliceSelFpf1, 1, offset, 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstPort,
                     _bcmFieldSliceSelFpf1, 1, offset, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcTrunk,
                     _bcmFieldSliceSelFpf1, 1, (offset + 12), 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifySrcPort,
                     _bcmFieldSliceSelFpf1, 1, (offset + 12), 12);
        if (soc_feature(unit, soc_feature_field_wide)) {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                         _bcmFieldSliceSelFpf1, 1, (offset + 24), 3);
        }
    }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                 _bcmFieldSliceSelFpf1, 2, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                 _bcmFieldSliceSelFpf1, 2, (offset + 16), 16);

#if defined(BCM_FIREBOLT2_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit)) {
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 16, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 12, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 12), 1, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 13), 3, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         offset, 6, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 6), 5, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 3, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 11), 5, 0, 0, 0, 0, 0);
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        if (soc_feature(unit, soc_feature_src_mac_group)) {
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             offset, 16, 0, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             offset, 12, 0, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 12), 1, 0, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 13), 3, 0, 0, 0, 0, 0);

            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                             _bcmFieldDevSelInnerVlanOverlay, 1,
                             _bcmFieldSliceSelFpf1, 3, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 12), 4, 0, 0, 0, 0, 0);
        } else {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldSliceSelFpf1, 3, offset, 16);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldSliceSelFpf1, 3, offset, 12);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldSliceSelFpf1, 3, (offset + 12), 1);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldSliceSelFpf1, 3, (offset + 13), 3);

        }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 3, (offset + 16), 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 3, (offset + 16), 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 3, (offset + 28), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 3, (offset + 29), 3);


    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 4, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 4, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 4, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 4, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 4, (offset + 16), 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                 _bcmFieldSliceSelFpf1, 5, offset, 8);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyEtherType,
                 _bcmFieldSliceSelFpf1, 5, (offset + 8), 16);

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyTunnelTerminated,
                 _bcmFieldSliceSelFpf1, 6, offset, 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanTranslationHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 1), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyForwardingVlanValid,
                 _bcmFieldSliceSelFpf1, 6, (offset + 2), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIngressStpState,
                 _bcmFieldSliceSelFpf1, 6, (offset + 3), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 5), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                 _bcmFieldSliceSelFpf1, 6, (offset + 6), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 7), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                 _bcmFieldSliceSelFpf1, 6, (offset + 8), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 9), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3SrcHostHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 10), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestHostHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 11), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpmcStarGroupHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3DestRouteHit,
                 _bcmFieldSliceSelFpf1, 6, (offset + 13), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2StationMove,
                 _bcmFieldSliceSelFpf1, 6, (offset + 14), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                 _bcmFieldSliceSelFpf1, 6, (offset + 15), 1);
#if defined(BCM_FIREBOLT2_SUPPORT) 
    if (SOC_IS_FIREBOLT2(unit)) {
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 16, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 12, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 28), 1, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldDevSelInnerVlanOverlay, 0,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 29), 3, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 16), 6, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 22), 5, 0, 0, 0, 0, 0);
        _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                         _bcmFieldDevSelInnerVlanOverlay, 1,
                         _bcmFieldSliceSelFpf1, 6, 
                         _bcmFieldSliceSelDisable, 0, 0,
                         (offset + 27), 5, 0, 0, 0, 0, 0);
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        if (soc_feature(unit, soc_feature_src_mac_group)) {
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 16), 16, 0, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 16), 12, 0, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 28), 1, 0, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                             _bcmFieldDevSelInnerVlanOverlay, 0,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 29), 3, 0, 0, 0, 0, 0);
            _FP_QUAL_EXT_ADD(unit, stage_fc, bcmFieldQualifySrcClassL2,
                             _bcmFieldDevSelInnerVlanOverlay, 1,
                             _bcmFieldSliceSelFpf1, 6, 
                             _bcmFieldSliceSelDisable, 0, 0,
                             (offset + 28), 4, 0, 0, 0, 0, 0);
        } else {
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlan,
                         _bcmFieldSliceSelFpf1, 6, (offset + 16), 16);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanId,
                         _bcmFieldSliceSelFpf1, 6, (offset + 16), 12);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanCfi,
                         _bcmFieldSliceSelFpf1, 6, (offset + 28), 1);
            _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyInnerVlanPri,
                         _bcmFieldSliceSelFpf1, 6, (offset + 29), 3);
        }

    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                 _bcmFieldSliceSelFpf1, 7, offset, 16);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                 _bcmFieldSliceSelFpf1, 7, offset, 12);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                 _bcmFieldSliceSelFpf1, 7, (offset + 12), 1);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                 _bcmFieldSliceSelFpf1, 7, (offset + 13), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL2Format,
                 _bcmFieldSliceSelFpf1, 7, (offset + 16), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVlanFormat,
                 _bcmFieldSliceSelFpf1, 7, (offset + 18), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                 _bcmFieldSliceSelFpf1, 7, (offset + 20), 2);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyMHOpcode,
                 _bcmFieldSliceSelFpf1, 7, (offset + 22), 3);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyPacketRes,
                 _bcmFieldSliceSelFpf1, 7, (offset + 25), 4);
    _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpInfo,
                 _bcmFieldSliceSelFpf1, 7, (offset + 29), 3);


#ifdef BCM_FIREBOLT2_SUPPORT
    if (SOC_IS_FIREBOLT2(unit)) {
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                     _bcmFieldSliceSelFpf1, 0xc, offset, 16);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                     _bcmFieldSliceSelFpf1, 0xc, offset, 12);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 12), 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 13), 3);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 16), 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 22), 5);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 29), 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelFpf1, 0xc, (offset + 31), 1);

        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                     _bcmFieldSliceSelFpf1, 0xd, offset , 8);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyVrf,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 8) , 6);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDstClassField,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 14) , 5);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyIpType,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 21) , 2);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 23) , 1);
        _FP_QUAL_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                     _bcmFieldSliceSelFpf1, 0xd, (offset + 24) , 8);

        /* Intraslice. */
        _key_fld_ = DATAf;

        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 8, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 16, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTtl,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 24, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 32, 6);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpFrag,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 38, 2);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyTunnelTerminated,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 40, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyVlanTranslationHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 41, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyForwardingVlanValid,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 42, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyIngressStpState,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 43, 2);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 45, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2SrcStatic,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 46, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2DestHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 47, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL3Routable,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 48, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL2CacheHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 49, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyL3SrcHostHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 50, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyL3DestHostHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 51, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyIpmcStarGroupHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 52, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyL3DestRouteHit,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 53, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc,
                                bcmFieldQualifyL2StationMove,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 54, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDosAttack,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 55, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyOuterVlan,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 56, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanId,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 56, 12);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanCfi,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 68, 1);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyOuterVlanPri,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 69, 3);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyRangeCheck,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 72, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDstIp,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 88, 32);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 120, 32);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 152, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                                _bcmFieldSliceSelFpf2, 0,
                                _bcmFieldSliceSelDisable, 0, 168, 16);

        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyTcpControl,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 0, 6);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyDSCP,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 8, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyIpProtocol,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 16, 8);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifySrcIp6,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 24, 128);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4DstPort,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 152, 16);
        _FP_QUAL_INTRASLICE_ADD(unit, stage_fc, bcmFieldQualifyL4SrcPort,
                                _bcmFieldSliceSelFpf2, 1,
                                _bcmFieldSliceSelDisable, 0, 168, 16);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_qualifiers_init
 * Purpose:
 *     Initialize device qaualifiers select codes & offsets
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_fb_qualifiers_init(int unit, _field_stage_t *stage_fc)
{
    int rv;           /* Operation return status. */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    /* Allocated stage qualifiers configuration array. */
    _FP_XGS3_ALLOC(stage_fc->f_qual_arr, 
                   (_bcmFieldQualifyCount * sizeof(_bcm_field_qual_info_t *)), 
                   "Field qualifiers");
    if (NULL == stage_fc->f_qual_arr) {
        return (BCM_E_MEMORY);
    }
    
    switch (stage_fc->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _field_fb_ingress_qualifiers_init(unit, stage_fc);
          break;
#if defined (BCM_FIREBOLT2_SUPPORT)
      case _BCM_FIELD_STAGE_LOOKUP:
          if (SOC_IS_FIREBOLT2(unit)) {
              rv = _field_fb2_lookup_qualifiers_init(unit, stage_fc);
          } else {
              rv = (BCM_E_NONE);
          } 
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          if (SOC_IS_FIREBOLT2(unit)) {
              rv = _field_fb2_egress_qualifiers_init(unit, stage_fc);
          } else {
              rv = (BCM_E_NONE);
          } 
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default: 
          sal_free(stage_fc->f_qual_arr);
          return (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _field_fb_init
 * Purpose:
 *     Perform initializations that are specific to BCM56504. This
 *     includes initializing the FP field select bit offset tables for FPF[1-3]
 *     for every stage. 
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     fc         - (IN) Field Processor control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
int
_bcm_field_fb_init(int unit, _field_control_t *fc) 
{
    _field_stage_t *stage_p; /* Stages iteration pointer */

    /* Input parameters check. */
    if (NULL == fc) {
        return (BCM_E_PARAM);
    }

    stage_p = fc->stages;
    while (stage_p) {
        /* Clear hardware table */
        BCM_IF_ERROR_RETURN(_field_fb_hw_clear(unit, stage_p));

        /* Initialize qualifiers info. */
        BCM_IF_ERROR_RETURN(_field_fb_qualifiers_init(unit, stage_p));

        /* Goto next stage */
        stage_p = stage_p->next;
    }

    if (!SOC_WARM_BOOT(unit)) {
        /* Enable filter processor */
        BCM_IF_ERROR_RETURN(_field_port_filter_enable_set(unit, fc, TRUE));

        /* Enable meter refresh */
        BCM_IF_ERROR_RETURN(_field_meter_refresh_enable_set(unit, fc, TRUE));
        if (SOC_IS_FIREBOLT(unit) && 
            !soc_feature(unit, soc_feature_field_wide)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, FP_CAM_CONTROL_UPPERr,
                                                       REG_PORT_ANY, 
                                                       SECRET_CHAIN_MODEf,1));
        }
    }

    /* Initialize the function pointers */
    _field_fb_functions_init(unit, &fc->functions);;

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_hw_clear
 * Purpose:
 *     Clear hardware memory of requested stage. 
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     stage_fc   - (IN) Field Processor stage control structure. 
 *
 * Returns:
 *     BCM_E_NONE
 * Notes:
 */
STATIC int
_field_fb_hw_clear(int unit, _field_stage_t *stage_fc)
{
    if (SOC_WARM_BOOT(unit)) {
        return (BCM_E_NONE);
    }

    if (_BCM_FIELD_STAGE_INGRESS == stage_fc->stage_id) {
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_UDF_OFFSETm, COPYNO_ALL, TRUE));  
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_RANGE_CHECKm, COPYNO_ALL, TRUE));  
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_TCAMm, COPYNO_ALL, TRUE));  
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_POLICY_TABLEm, COPYNO_ALL, TRUE));  
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_METER_TABLEm, COPYNO_ALL, TRUE));  
        if (SOC_MEM_IS_VALID(unit, FP_COUNTER_TABLEm)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), FP_COUNTER_TABLEm, COPYNO_ALL, TRUE));
        }  
#if defined(BCM_SCORPION_SUPPORT)
        if (SOC_MEM_IS_VALID(unit, FP_COUNTER_TABLE_Xm)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), FP_COUNTER_TABLE_Xm, COPYNO_ALL, TRUE));
        }  
        if (SOC_MEM_IS_VALID(unit, FP_COUNTER_TABLE_Ym)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), FP_COUNTER_TABLE_Ym, COPYNO_ALL, TRUE));
        }
#endif  /* BCM_SCORPION_SUPPORT */
        BCM_IF_ERROR_RETURN
            (soc_mem_clear((unit), FP_PORT_FIELD_SELm, COPYNO_ALL, TRUE));  
        if (SOC_MEM_IS_VALID(unit, IFP_PORT_FIELD_SELm)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear(unit, IFP_PORT_FIELD_SELm, COPYNO_ALL, TRUE));
        } 
#if defined(BCM_TRX_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT) || \
    defined(BCM_RAPTOR_SUPPORT) 
        if (SOC_MEM_IS_VALID(unit, FP_SLICE_MAPm)) {              
            BCM_IF_ERROR_RETURN
                (soc_mem_clear(unit, FP_SLICE_MAPm, COPYNO_ALL, TRUE));
        }
#endif /* RAPTOR || TRX || FB2 */
#ifdef BCM_FIREBOLT2_SUPPORT 
    } else if (soc_feature(unit, soc_feature_field_multi_stage)) {

        if (_BCM_FIELD_STAGE_LOOKUP == stage_fc->stage_id) { 
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), VFP_TCAMm, COPYNO_ALL, TRUE));  
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), VFP_POLICY_TABLEm, COPYNO_ALL, TRUE));  
        
        } else if (_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id) {

            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), EFP_TCAMm, COPYNO_ALL, TRUE));  
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), EFP_POLICY_TABLEm, COPYNO_ALL, TRUE));  
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), EFP_METER_TABLEm, COPYNO_ALL, TRUE));  
            BCM_IF_ERROR_RETURN
                (soc_mem_clear((unit), EFP_COUNTER_TABLEm, COPYNO_ALL, TRUE));  
        }
#endif /*  BCM_FIREBOLT2_SUPPORT  */
    } else {
        return (BCM_E_UNAVAIL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_detach
 *
 * Purpose:
 *     Deallocates BCM5650x's field tables.
 *
 * Parameters:
 *     unit - BCM device number
 *
 * Returns:
 *     BCM_E_NONE
 */
STATIC int
_field_fb_detach(int unit, _field_control_t *fc)
{
    _field_stage_t   *stage_p;  /* Stage iteration pointer. */

    stage_p = fc->stages;

    while (stage_p) { 
       /* Clear the hardware tables */
        BCM_IF_ERROR_RETURN(_field_fb_hw_clear(unit, stage_p));
      
        /* Goto next stage */
        stage_p = stage_p->next; 
    }
    
    /* Clear udf match criteria registers. */
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ipprotocol_delete_all(unit));
    BCM_IF_ERROR_RETURN(_bcm_field_fb_udf_ethertype_delete_all(unit));

    /* Don't disable port controls during Warmboot or Reloading state */
    if (0 == SOC_HW_ACCESS_DISABLE(unit)) {
        /* Clear the Filter Enable flags in the port table */
        BCM_IF_ERROR_RETURN (_field_port_filter_enable_set(unit, fc, FALSE));
    }

    BCM_IF_ERROR_RETURN (_field_meter_refresh_enable_set(unit, fc, FALSE));
 
    return (BCM_E_NONE);
}

#ifdef BROADCOM_DEBUG
/*
 * Function:
 *     _field_fb_action_name
 * Purpose:
 *     Return text name of indicated action enum value.
 */
STATIC char *
_field_fb_action_name(bcm_field_action_t action)
{
    /* Text names of Actions. These are used for debugging output and CLIs.
     * Note that the order needs to match the bcm_field_action_t enum order.
     */
    static char *action_text[] = BCM_FIELD_ACTION_STRINGS;
    assert(COUNTOF(action_text) == bcmFieldActionCount);
    return (action >= bcmFieldActionCount ? "??" : action_text[action]);
}

#endif /* BROADCOM_DEBUG*/

#if defined(BCM_FIREBOLT2_SUPPORT) 
/*
 * Function:
 *     _field_fb_egress_selcode_get
 * Purpose:
 *     Finds a select encodings that will satisfy the
 *     requested qualifier set (Qset).
 * Parameters:
 *     unit      - (IN) BCM unit number.
 *     stage_fc  - (IN) Stage Field control structure.
 *     qset_req  - (IN) Client qualifier set.
 *     fg        - (IN/OUT)Select code information filled into the group.  
 * 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_egress_selcode_get(int unit, _field_stage_t *stage_fc, 
                               bcm_field_qset_t *qset_req,
                               _field_group_t *fg)
{
    int             rv;          /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    if  (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        /* Attempt _BCM_FIELD_EFP_KEY4  (L2 key). */
        rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                                           _BCM_FIELD_EFP_KEY4, 0, fg);
        if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
            return rv;
        }

        /* Attempt _BCM_FIELD_EFP_KEY1  (IPv4 key). */
        if (BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyIp4)) {
            rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                    _BCM_FIELD_EFP_KEY1, 0, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
                return rv;
            }
        }

        /* Attempt _BCM_FIELD_EFP_KEY2  (IPv6 key). */
        if (BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyIp6)) {
            rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                    _BCM_FIELD_EFP_KEY2, 0, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
                return rv;
            }
        }
    }  else  {
        /* IPv6 double wide predefined key. */
        if (BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyIp6)) {
            rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                    _BCM_FIELD_EFP_KEY3,
                    _BCM_FIELD_EFP_KEY2, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
                return rv;
            }
        }

        /* L2 + L3 double wide predefined key. */
        if (BCM_FIELD_QSET_TEST(*qset_req, bcmFieldQualifyIp4)) {
            rv = _bcm_field_egress_key_attempt(unit, stage_fc, qset_req, 
                    _BCM_FIELD_EFP_KEY1,
                    _BCM_FIELD_EFP_KEY4, fg);
            if (BCM_SUCCESS(rv) || (BCM_FAILURE(rv) && (rv != BCM_E_RESOURCE))) {
                return rv;
            }
        }
    }
    return (BCM_E_RESOURCE);
}
#endif /* BCM_FIREBOLT2_SUPPORT */


/*
 * Function:
 *     _bcm_field_fb_group_inner_vlan_overlay_get
 * Purpose:
 *     Returns inner vlan overlay for a group.
 * Parameters:
 *     uint    - (IN) BCM unit number.
 *     value   - (OUT) Inner vlan overlay value.
 * 
 * Returns:
 *     BCM_E_XXX
 * Note:
 */
int
_bcm_field_fb_group_inner_vlan_overlay_get (int unit, int *value)
{
    _field_control_t *fc;    /* field control structure. */
    _field_group_t   *fg;    /* field group structure.   */

    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    /* Get field control strucutre.*/
    BCM_IF_ERROR_RETURN(_field_control_get (unit, &fc));

    /* Iterate over all the groups to check for conflicts. */
    fg = fc->groups;

    while (NULL != fg) {
        if ((fg->stage_id == _BCM_FIELD_STAGE_INGRESS) && 
            (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].inner_vlan_overlay)) { 
            *value = fg->sel_codes[0].inner_vlan_overlay;
            break;
        }
        fg = fg->next;
    }

    if (NULL == fg) {
        *value = _FP_SELCODE_DONT_CARE;
    }
    return (BCM_E_NONE);
}
#if defined (BCM_FIREBOLT2_SUPPORT)
/*
 * Function:
 *     _bcm_field_vfp_doublewide_key_select_get
 * Purpose:
 *     Returns vfp doublewide key selection on FB2
 * Parameters:
 *     uint    - (IN) BCM unit number.
 *     value   - (OUT) Inner vlan overlay value.
 * 
 * Returns:
 *     BCM_E_RESOURCE - Conflict observed inside the group.
 *     BCM_E_NONE     - otherwise
 * Note:
 */
int
_bcm_field_vfp_doublewide_key_select_get(int unit, int *value)
{
    _field_control_t *fc;    /* field control structure. */
    _field_group_t   *fg;    /* field group structure.   */

    if (NULL == value) {
        return (BCM_E_PARAM);
    }

    if (0 == SOC_IS_FIREBOLT2(unit)) {
        *value = _FP_SELCODE_DONT_CARE; 
        return (BCM_E_NONE);
    }

    /* Get field control strucutre.*/
    BCM_IF_ERROR_RETURN(_field_control_get (unit, &fc));

    /* Iterate over all the groups to check for conflicts. */
    fg = fc->groups;

    while (NULL != fg) {
        if ((fg->stage_id == _BCM_FIELD_STAGE_LOOKUP) && 
            (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) &&
            (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].intraslice_vfp_sel)) { 
            *value = fg->sel_codes[0].intraslice_vfp_sel;
            break;
        }
        fg = fg->next;
    }

    if (NULL == fg) {
        *value = _FP_SELCODE_DONT_CARE;
    }
    return (BCM_E_NONE);
}
#endif /* BCM_FIREBOLT2_SUPPORT */


/*
 * Function:
 *     _field_fb_tcam_policy_clear
 *
 * Purpose:
 *     Writes a null entry into the TCAM plus Policy table.
 *
 * Parameters:
 *     unit      -   (IN)BCM device number.
 *     stage_id  -   (IN)FP stage pipeline id. 
 *     tcam_idx  -   (IN)Entry tcam index.
 *     
 * Returns:
 *     BCM_E_XXX
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_field_fb_tcam_policy_clear(int unit, _field_entry_t *f_ent,
                            _field_stage_id_t stage_id, int tcam_idx)
{
    soc_mem_t tcam_mem;                 /* TCAM memory id.                    */
    soc_mem_t policy_mem;               /* Policy table memory id .           */

    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_mem_get(unit, stage_id, &tcam_mem, &policy_mem));
    
    /* Memory index sanity check. */
    if (tcam_idx > soc_mem_index_max(unit, tcam_mem)) {
        return (BCM_E_PARAM);
    }  

    /* Write the TCAM & Policy Tables */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx,
                       soc_mem_entry_null(unit, tcam_mem)));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx,
                       soc_mem_entry_null(unit, policy_mem)));
#ifdef BCM_TRIDENT2_SUPPORT
    if (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2(unit) || SOC_IS_TD2P_TT2P(unit)) {
        if ((SOC_MEM_IS_VALID(unit, FP_GM_FIELDSm)) &&
            (stage_id == _BCM_FIELD_STAGE_INGRESS) &&
            (f_ent != NULL) && (f_ent->flags & _FP_ENTRY_SECONDARY)) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, FP_GM_FIELDSm,
                                   MEM_BLOCK_ALL, tcam_idx,
                                   soc_mem_entry_null(unit, FP_GM_FIELDSm)));
        } else if ((SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAMm)) &&
                   (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, FP_GLOBAL_MASK_TCAMm,
                               MEM_BLOCK_ALL, tcam_idx,
                               soc_mem_entry_null
                                   (unit, FP_GLOBAL_MASK_TCAMm)));
        }
    } else
#endif
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
        if ((SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAMm)) && 
            (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            SOC_IF_ERROR_RETURN (soc_mem_write(unit, FP_GLOBAL_MASK_TCAMm,
                                               MEM_BLOCK_ALL, tcam_idx,
                                               soc_mem_entry_null(unit, FP_GLOBAL_MASK_TCAMm)));
        }
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_SCORPION_SUPPORT)
        if ((SOC_IS_SC_CQ(unit)) && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, FP_TCAM_Ym,
                               MEM_BLOCK_ALL, tcam_idx,
                               soc_mem_entry_null(unit, FP_TCAM_Ym)));
        }
#endif /* BCM_SCORPION_SUPPORT */

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_field_fb_counter_adjust_wide_mode
 * Purpose:
 *     For wide mode entries, 
 *     if there is movement across slices,
 *     handle the counter mode and index
 * Parameters:
 *     unit - BCM unit
 *     policy_mem - policy memory
 *     f_st - Statistics collection entity. 
 *     f_ent_pri - PRI entry
 *     f_ent_sec - SEC entry
 *     p_entry_pri - policy table for PRI entry
 *     p_entry_sec - policy table for SEC entry
 * Returns:
 *     none
 * Notes:
 *     FOUR cases are to be handled 
 *     1. previously counter in PRI slice, now in PRI slice
 *     2. previously counter in PRI slice, now in SEC slice
 *     3. previously counter in SEC slice, now in PRI slice
 *     4. previously counter in SEC slice, now in SEC slice
 */
void
_bcm_field_fb_counter_adjust_wide_mode(int unit, 
                                   soc_mem_t policy_mem,
                                   _field_stat_t *f_st,
                                   _field_entry_t *f_ent_pri, 
                                   _field_entry_t *f_ent_sec, 
                                   int new_slice_numb,
                                   uint32 p_entry_pri[],
                                   uint32 p_entry_sec[])
{
    int mode;

    if ((mode = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, 
                                    COUNTER_MODEf)) != 0) {
        if (new_slice_numb == f_st->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_INDEXf, f_st->hw_index);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_MODEf, f_st->hw_mode);
        } else if ((new_slice_numb + 1) == f_st->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri, 
                                COUNTER_MODEf, 0);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec, 
                                COUNTER_MODEf, f_st->hw_mode);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                COUNTER_INDEXf, f_st->hw_index);
        }
    } else if ((mode = soc_mem_field32_get(unit, policy_mem, 
                (uint32 *) p_entry_sec, COUNTER_MODEf)) != 0) {
        if (new_slice_numb == f_st->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri, 
                                COUNTER_MODEf, f_st->hw_mode);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_INDEXf, f_st->hw_index);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec, 
                                COUNTER_MODEf, 0);
        } else if ((new_slice_numb + 1) == f_st->pool_index) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                COUNTER_INDEXf, f_st->hw_index);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec, 
                                COUNTER_MODEf, f_st->hw_mode);
        }
    }
}

/*
 * Function:
 *     _bcm_field_fb_meter_adjust_wide_mode
 * Purpose:
 *     For wide mode entries, 
 *     if there is movement across slices,
 *     handle the meter mode, index, update & valid bits
 * Parameters:
 *     unit - BCM unit
 *     policy_mem - policy memory
 *     f_ent_pri - PRI entry
 *     f_ent_sec - SEC entry
 *     p_entry_pri - policy table for PRI entry
 *     p_entry_sec - policy table for SEC entry
 * Returns:
 *     none
 * Notes:
 *     FOUR cases are to be handled 
 *     1. previously meter in PRI slice, now in PRI slice
 *     2. previously meter in PRI slice, now in SEC slice
 *     3. previously meter in SEC slice, now in PRI slice
 *     4. previously meter in SEC slice, now in SEC slice
 *     MUST BE CALLED AFTER COUNTER ADJUST.
 */
void
_bcm_field_fb_meter_adjust_wide_mode(int unit, 
                                     soc_mem_t policy_mem,
                                     _field_policer_t *f_pl,
                                     _field_entry_t *f_ent_pri, 
                                     _field_entry_t *f_ent_sec, 
                                     uint32 p_entry_pri[],
                                     uint32 p_entry_sec[])
{
    uint32 counter_mode_pri;
    uint32 counter_mode_sec;
    uint32 counter_idx_pri;
    uint32 counter_idx_sec;
    int mode; 
    int restore_counter = FALSE; 


    /* Preserve counter mode. */
    if (f_pl->stage_id == _BCM_FIELD_STAGE_EGRESS ) {
        counter_mode_pri = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, PID_COUNTER_MODEf);
        counter_idx_pri = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, PID_COUNTER_INDEXf);
        counter_mode_sec = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_sec, PID_COUNTER_MODEf);
        counter_idx_sec = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_sec, PID_COUNTER_INDEXf);
    } else if (f_pl->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
        counter_mode_pri = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, USE_VINTF_CTR_IDXf);
        counter_idx_pri = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, VINTF_CTR_IDXf);
        counter_mode_sec = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_sec, USE_VINTF_CTR_IDXf);
        counter_idx_sec = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_sec, VINTF_CTR_IDXf);
    } else {
        counter_mode_pri = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, COUNTER_MODEf);
        counter_idx_pri = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_pri, COUNTER_INDEXf);
        counter_mode_sec = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_sec, COUNTER_MODEf);
        counter_idx_sec = soc_mem_field32_get(unit, policy_mem, (uint32 *) p_entry_sec, COUNTER_INDEXf);
    }
    /* if meter to be moved is from primary slice*/
    if ((mode = soc_mem_field32_get(unit, policy_mem, 
         (uint32 *) p_entry_pri, METER_PAIR_MODEf)) != 0) {
        if ((f_pl->stage_id == _BCM_FIELD_STAGE_EGRESS) &&
            (SOC_IS_TRIDENT2PLUS(unit))) {
            /* if the destination entry is allocated from primar slice,
             * even or odd meter mode should be managed accordingly
             */
            if (!(f_ent_pri->flags & _FP_ENTRY_ALLOC_FROM_SECONDARY_SLICE)) {
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_ODDf, f_pl->hw_index);
                if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                   /* Excess meter - even index. */
                   soc_mem_field32_set(unit, policy_mem,
                                       (uint32 *) p_entry_pri,
                                        METER_TEST_ODDf, 0);
                   if (SOC_IS_KATANA2(unit) &&
                       (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                       soc_mem_field32_set(unit, policy_mem,
                                            (uint32 *) p_entry_pri,
                                            METER_TEST_EVENf, 0);
                   } else {
                       soc_mem_field32_set(unit, policy_mem,
                                            (uint32 *) p_entry_pri,
                                            METER_TEST_EVENf, 1);
                   }
                   soc_mem_field32_set(unit, policy_mem,
                                        (uint32 *) p_entry_pri,
                                         METER_UPDATE_ODDf, 0);
                   soc_mem_field32_set(unit, policy_mem,
                                        (uint32 *) p_entry_pri,
                                        METER_UPDATE_EVENf, 1);
                } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
                   /* Committed meter - odd index. */
                    if (SOC_IS_KATANA2(unit) &&
                        (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                        soc_mem_field32_set(unit, policy_mem,
                                             (uint32 *) p_entry_pri,
                                              METER_TEST_ODDf, 0);
                    } else {
                        soc_mem_field32_set(unit, policy_mem,
                                             (uint32 *) p_entry_pri,
                                             METER_TEST_ODDf, 1);
                    }
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_pri,
                                         METER_TEST_EVENf, 0);
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_pri,
                                          METER_UPDATE_ODDf, 1);
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_pri,
                                         METER_UPDATE_EVENf, 0);
                } else if (f_pl->cfg.mode == bcmPolicerModePassThrough) {
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_pri,
                                         METER_TEST_EVENf, 1);
                }
#if defined(BCM_TRIDENT2_SUPPORT)
                else if (f_pl->cfg.mode == bcmPolicerModeSrTcmModified
                        && (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
                    soc_mem_field32_set(unit, policy_mem,
                                    (uint32 *) p_entry_pri,
                                    METER_MODE_MODIFIERf, 1);
                }
#endif
            } else {
                sal_memcpy(p_entry_sec, p_entry_pri,
                      soc_mem_entry_words(unit, policy_mem)*sizeof(uint32));
                sal_memset(p_entry_pri, 0,
                      soc_mem_entry_words(unit, policy_mem)*sizeof(uint32));
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_PAIR_MODEf, mode);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_ODDf, f_pl->hw_index);
                restore_counter = TRUE;
                if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                   /* Excess meter - even index. */
                   soc_mem_field32_set(unit, policy_mem,
                                       (uint32 *) p_entry_sec,
                                        METER_TEST_ODDf, 0);
                   if (SOC_IS_KATANA2(unit) &&
                       (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                       soc_mem_field32_set(unit, policy_mem,
                                            (uint32 *) p_entry_sec,
                                            METER_TEST_EVENf, 0);
                   } else {
                       soc_mem_field32_set(unit, policy_mem,
                                            (uint32 *) p_entry_sec,
                                            METER_TEST_EVENf, 1);
                   }
                   soc_mem_field32_set(unit, policy_mem,
                                        (uint32 *) p_entry_sec,
                                         METER_UPDATE_ODDf, 0);
                   soc_mem_field32_set(unit, policy_mem,
                                        (uint32 *) p_entry_sec,
                                        METER_UPDATE_EVENf, 1);
                } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
                   /* Committed meter - odd index. */
                    if (SOC_IS_KATANA2(unit) &&
                        (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                        soc_mem_field32_set(unit, policy_mem,
                                             (uint32 *) p_entry_sec,
                                              METER_TEST_ODDf, 0);
                    } else {
                        soc_mem_field32_set(unit, policy_mem,
                                             (uint32 *) p_entry_sec,
                                             METER_TEST_ODDf, 1);
                    }
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_sec,
                                         METER_TEST_EVENf, 0);
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_sec,
                                          METER_UPDATE_ODDf, 1);
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_sec,
                                         METER_UPDATE_EVENf, 0);
                } else if (f_pl->cfg.mode == bcmPolicerModePassThrough) {
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_sec,
                                         METER_TEST_EVENf, 1);
                }
#if defined(BCM_TRIDENT2_SUPPORT)
                else if (f_pl->cfg.mode == bcmPolicerModeSrTcmModified
                        && (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
                    soc_mem_field32_set(unit, policy_mem,
                                    (uint32 *) p_entry_sec,
                                    METER_MODE_MODIFIERf, 1);
                }
#endif
            }
        } else {
            if (f_ent_pri->fs->slice_number == f_pl->pool_index) {
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                    METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_ODDf, f_pl->hw_index);
            
            } else if (f_ent_sec->fs->slice_number == f_pl->pool_index) {
                sal_memcpy(p_entry_sec, p_entry_pri,
                      soc_mem_entry_words(unit, policy_mem)*sizeof(uint32));
                sal_memset(p_entry_pri, 0,
                       soc_mem_entry_words(unit, policy_mem)*sizeof(uint32));
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_PAIR_MODEf, mode);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_ODDf, f_pl->hw_index);
                restore_counter = TRUE;
            }
        }
    /* if source entry is allocated from secondary slice */
    } else if ((mode = soc_mem_field32_get(unit, policy_mem, 
                (uint32 *) p_entry_sec, METER_PAIR_MODEf)) != 0) {

        if ((f_pl->stage_id == _BCM_FIELD_STAGE_EGRESS) &&
            (SOC_IS_TRIDENT2PLUS(unit))) {
            if (!(f_ent_pri->flags & _FP_ENTRY_ALLOC_FROM_SECONDARY_SLICE)) {
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_ODDf, f_pl->hw_index);
                if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                   /* Excess meter - even index. */
                   soc_mem_field32_set(unit, policy_mem,
                                       (uint32 *) p_entry_pri,
                                        METER_TEST_ODDf, 0);
                   if (SOC_IS_KATANA2(unit) &&
                       (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                       soc_mem_field32_set(unit, policy_mem,
                                            (uint32 *) p_entry_pri,
                                            METER_TEST_EVENf, 0);
                   } else {
                       soc_mem_field32_set(unit, policy_mem,
                                            (uint32 *) p_entry_pri,
                                            METER_TEST_EVENf, 1);
                   }
                   soc_mem_field32_set(unit, policy_mem,
                                        (uint32 *) p_entry_pri,
                                         METER_UPDATE_ODDf, 0);
                   soc_mem_field32_set(unit, policy_mem,
                                        (uint32 *) p_entry_pri,
                                        METER_UPDATE_EVENf, 1);
                } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
                   /* Committed meter - odd index. */
                    if (SOC_IS_KATANA2(unit) &&
                        (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                        soc_mem_field32_set(unit, policy_mem,
                                             (uint32 *) p_entry_pri,
                                              METER_TEST_ODDf, 0);
                    } else {
                        soc_mem_field32_set(unit, policy_mem,
                                             (uint32 *) p_entry_pri,
                                             METER_TEST_ODDf, 1);
                    }
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_pri,
                                         METER_TEST_EVENf, 0);
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_pri,
                                          METER_UPDATE_ODDf, 1);
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_pri,
                                         METER_UPDATE_EVENf, 0);
                } else if (f_pl->cfg.mode == bcmPolicerModePassThrough) {
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_pri,
                                         METER_TEST_EVENf, 1);
                }
#if defined(BCM_TRIDENT2_SUPPORT)
                else if (f_pl->cfg.mode == bcmPolicerModeSrTcmModified
                        && (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
                    soc_mem_field32_set(unit, policy_mem,
                                    (uint32 *) p_entry_pri,
                                    METER_MODE_MODIFIERf, 1);
                }
#endif
            } else {
                sal_memcpy(p_entry_sec, p_entry_pri,
                      soc_mem_entry_words(unit, policy_mem)*sizeof(uint32));
                sal_memset(p_entry_pri, 0,
                       soc_mem_entry_words(unit, policy_mem)*sizeof(uint32));
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_PAIR_MODEf, mode);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_ODDf, f_pl->hw_index);
                restore_counter = TRUE;
                if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
                   /* Excess meter - even index. */
                   soc_mem_field32_set(unit, policy_mem,
                                       (uint32 *) p_entry_sec,
                                        METER_TEST_ODDf, 0);
                   if (SOC_IS_KATANA2(unit) &&
                       (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                       soc_mem_field32_set(unit, policy_mem,
                                            (uint32 *) p_entry_sec,
                                            METER_TEST_EVENf, 0);
                   } else {
                       soc_mem_field32_set(unit, policy_mem,
                                            (uint32 *) p_entry_sec,
                                            METER_TEST_EVENf, 1);
                   }
                   soc_mem_field32_set(unit, policy_mem,
                                        (uint32 *) p_entry_sec,
                                         METER_UPDATE_ODDf, 0);
                   soc_mem_field32_set(unit, policy_mem,
                                        (uint32 *) p_entry_sec,
                                        METER_UPDATE_EVENf, 1);
                } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
                   /* Committed meter - odd index. */
                    if (SOC_IS_KATANA2(unit) &&
                        (f_pl->cfg.flags & BCM_POLICER_AS_PACKET_COUNTER)) {
                        soc_mem_field32_set(unit, policy_mem,
                                             (uint32 *) p_entry_sec,
                                              METER_TEST_ODDf, 0);
                    } else {
                        soc_mem_field32_set(unit, policy_mem,
                                             (uint32 *) p_entry_sec,
                                             METER_TEST_ODDf, 1);
                    }
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_sec,
                                         METER_TEST_EVENf, 0);
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_sec,
                                          METER_UPDATE_ODDf, 1);
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_sec,
                                         METER_UPDATE_EVENf, 0);
                } else if (f_pl->cfg.mode == bcmPolicerModePassThrough) {
                    soc_mem_field32_set(unit, policy_mem,
                                         (uint32 *) p_entry_sec,
                                         METER_TEST_EVENf, 1);
                }
#if defined(BCM_TRIDENT2_SUPPORT)
                else if (f_pl->cfg.mode == bcmPolicerModeSrTcmModified
                        && (SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
                    soc_mem_field32_set(unit, policy_mem,
                                    (uint32 *) p_entry_sec,
                                    METER_MODE_MODIFIERf, 1);
                }
#endif
            }
        } else {
            if (f_ent_pri->fs->slice_number == f_pl->pool_index) {
                sal_memcpy(p_entry_pri, p_entry_sec,
                     soc_mem_entry_words(unit, policy_mem)*sizeof(uint32));
                sal_memset(p_entry_sec, 0,
                      soc_mem_entry_words(unit, policy_mem)*sizeof(uint32));
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_PAIR_MODEf, mode);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                METER_INDEX_ODDf, f_pl->hw_index);
                restore_counter = TRUE;
            } else if (f_ent_sec->fs->slice_number == f_pl->pool_index) {
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                METER_INDEX_ODDf, f_pl->hw_index);
            }
        }
    }

    if (restore_counter) {
        if (f_pl->stage_id == _BCM_FIELD_STAGE_EGRESS ) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                PID_COUNTER_INDEXf, counter_idx_sec);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                PID_COUNTER_MODEf, counter_mode_sec);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                PID_COUNTER_INDEXf, counter_idx_pri);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                PID_COUNTER_MODEf, counter_mode_pri);
        } if (f_pl->stage_id == _BCM_FIELD_STAGE_LOOKUP ) {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                VINTF_CTR_IDXf, counter_idx_sec);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                USE_VINTF_CTR_IDXf, counter_mode_sec);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                VINTF_CTR_IDXf, counter_idx_pri);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                USE_VINTF_CTR_IDXf, counter_mode_pri);
        } else {
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                COUNTER_INDEXf, counter_idx_sec);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_sec,
                                COUNTER_MODEf, counter_mode_sec);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_INDEXf, counter_idx_pri);
            soc_mem_field32_set(unit, policy_mem, (uint32 *) p_entry_pri,
                                COUNTER_MODEf, counter_mode_pri);
        }
    }
}

/*
 * Function:
 *     _bcm_field_fb_entry_move
 * Purpose:
 *     Copy an entry from one TCAM index to another. It copies the values in
 *     hardware from the old index to the new index. 
 *     IT IS ASSUMED THAT THE NEW INDEX IS EMPTY (VALIDf=00) IN HARDWARE.
 *     The old Hardware index is cleared at the end.
 * Parameters:
 *     unit           - (IN) BCM device number. 
 *     f_ent          - (IN) Entry to move
 *     parts_count    - (IN) Field entry parts count.
 *     tcam_idx_old   - (IN) Source entry tcam index.
 *     tcam_idx_new   - (IN) Destination entry tcam index.
 *                          
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_entry_move(int unit, _field_entry_t *f_ent, int parts_count,
                         int *tcam_idx_old, int *tcam_idx_new)
{
    static uint32 e_buf[BCM_MAX_NUM_UNITS][25];
                                        /* For TCAM                           */
    static uint32 p_buf[BCM_MAX_NUM_UNITS][_FP_MAX_ENTRY_WIDTH][25];
                                        /* For policies                       */
    int new_slice_numb = 0;             /* Entry new slice number.            */
    int new_slice_idx = 0;              /* Entry new offset in the slice      */
    soc_mem_t tcam_mem;                 /* TCAM memory id.                    */
    soc_mem_t policy_mem;               /* Policy table memory id .           */
    int tcam_idx_max;                   /* TCAM memory max index.             */
    int tcam_idx_min;                   /* TCAM memory min index.             */
    _field_stage_t *stage_fc;           /* Stage field control structure.     */
    _field_stage_id_t stage_id;         /* Field pipeline stage id.           */
    int idx;                            /* Iteration index.                   */
    _field_policer_t *f_pl = NULL;      /* Field policer descriptor.          */
    _field_stat_t    *f_st = NULL;      /* Field statistics descriptor.       */
    _field_group_t   *fg;               /* Field group structure.             */
    int              rv;                /* Operation return status.           */
    int      is_backup_entry = 0;       /* Flag ,checks the entry is backup entry or not*/
#ifdef BCM_TRIUMPH_SUPPORT
    fp_global_mask_tcam_entry_t ipbm_entry[_FP_MAX_ENTRY_WIDTH]; /* Always in PRI entry, 1st half */
#endif /* BCM_TRIUMPH_SUPPORT */
#ifdef BCM_TRIDENT_SUPPORT
    fp_global_mask_tcam_x_entry_t fp_global_mask_x[_FP_MAX_ENTRY_WIDTH];
    fp_global_mask_tcam_y_entry_t fp_global_mask_y[_FP_MAX_ENTRY_WIDTH];
    fp_global_mask_tcam_entry_t fp_global_mask[_FP_MAX_ENTRY_WIDTH];
    fp_gm_fields_entry_t fp_gm_fields[_FP_MAX_ENTRY_WIDTH];
    bcm_pbmp_t pbmp_x, pbmp_y;
#endif /* !BCM_TRIDENT_SUPPORT */


    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == tcam_idx_old) || (NULL == tcam_idx_new)) {
        return (BCM_E_PARAM);
    }

    fg = f_ent->group;

    /* Get field stage control . */
    stage_id = f_ent->group->stage_id;
    rv = _field_stage_control_get(unit, stage_id, &stage_fc);      
    BCM_IF_ERROR_RETURN(rv);

    /* Get entry tcam and actions. */
    rv = _field_fb_tcam_policy_mem_get(unit, stage_id, &tcam_mem, &policy_mem);
    BCM_IF_ERROR_RETURN(rv);

    tcam_idx_max = soc_mem_index_max(unit, tcam_mem);
    tcam_idx_min = soc_mem_index_min(unit, tcam_mem);

    /* Update policy entry if moving across the slices. */
    if (((NULL != f_ent->ent_copy) && 
                (f_ent->ent_copy->eid ==  _FP_INTERNAL_RESERVED_ID))) {
        is_backup_entry = 1;
    }

    for (idx = 0; idx < parts_count; idx++) {
        /* Index sanity check. */
        if ((tcam_idx_old[idx] < tcam_idx_min) || (tcam_idx_old[idx] > tcam_idx_max) ||
            (tcam_idx_new[idx] < tcam_idx_min) || (tcam_idx_new[idx] > tcam_idx_max)) {
            LOG_DEBUG(BSL_LS_BCM_FP, (BSL_META_U(unit,
              "Invalid index range from %d to %d."),
              tcam_idx_old[idx], tcam_idx_new[idx]));

            rv = BCM_E_PARAM;
            /* coverity[dead_error_line:FALSE] */
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Read policy entry from current tcam index. */
        rv = soc_mem_read(unit, policy_mem, MEM_BLOCK_ANY, tcam_idx_old[idx],
                          p_buf[unit][idx]);
        BCM_IF_ERROR_RETURN(rv);

#ifdef BCM_TRIDENT_SUPPORT
        /*
         * On Trident, VALIDf of FP_GLOBAL_MASK_TCAMm must be set
         * for all parts of an wide/paired mode entry.
         * This bit would have been set during entry install.
         * Read this value from all parts of an entry while moving.
         * Otherwise, actions related to this entry would not work.
         */
        if (SOC_IS_TD_TT(unit) && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE)
                && (0x1 & idx)) {
                rv = READ_FP_GM_FIELDSm(unit, MEM_BLOCK_ANY,
                        tcam_idx_old[idx], &fp_gm_fields[idx]);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                rv = READ_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ANY,
                        tcam_idx_old[idx], &fp_global_mask[idx]);
                BCM_IF_ERROR_RETURN(rv);
            }
        } else
#endif /* !BCM_TRIDENT_SUPPORT */
#if defined(BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        if ((SOC_IS_KATANAX(unit)
            || SOC_IS_TRIUMPH3(unit))
            && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            rv = READ_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ANY,
                    tcam_idx_old[idx], &ipbm_entry[idx]);
            BCM_IF_ERROR_RETURN(rv);
        } else
#endif /* BCM_KATANA_SUPPORT */       
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_TRIUMPH2_SUPPORT)
        {
            if ((SOC_MEM_IS_VALID(unit, FP_GLOBAL_MASK_TCAMm))
                && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
                rv = READ_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ANY,
                        tcam_idx_old[idx], &ipbm_entry[idx]);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
#endif /* BCM_TRIUMPH_SUPPORT || BCM_TRIUMPH2_SUPPORT */
    }

    /* Calculate primary entry new slice & offset in the slice. */
    rv = _bcm_field_tcam_idx_to_slice_offset(unit, stage_fc, _FP_DEF_INST, 
                                             tcam_idx_new[0],
                                             &new_slice_numb, &new_slice_idx);
    BCM_IF_ERROR_RETURN(rv);

    /* Update policy entry if moving across the slices. */
    if ((1 != is_backup_entry) && (f_ent->fs->slice_number != new_slice_numb)) {
        /* Get policer associated with the entry. */
        if ((0 == (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS)) &&
            (f_ent->policer[0].flags & _FP_POLICER_INSTALLED)) {
             rv = _bcm_field_policer_get(unit,
                                         f_ent->policer[0].pid, 
                                         &f_pl);
             BCM_IF_ERROR_RETURN(rv);
        }
        /* Get statistics entity associated with the entry. */
        if ((0 == (stage_fc->flags & _FP_STAGE_GLOBAL_COUNTERS)) &&
            (0 == (stage_fc->flags & _FP_STAGE_GLOBAL_CNTR_POOLS)) &&
            (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) {
            rv = _bcm_field_stat_get(unit,
                                     f_ent->statistic.sid, 
                                     &f_st);
            BCM_IF_ERROR_RETURN(rv);
        }
        if (fg->flags & (_FP_GROUP_SPAN_SINGLE_SLICE | 
                         _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
            /*
             * For _FP_GROUP_INTRASLICE_DOUBLEWIDE, *even* if it is  
             *     _FP_GROUP_SPAN_DOUBLE_SLICE, we do this.
             *     This is because in intraslice double-wide, the PRI 
             *     slice has tcam_slice_sz/2 entries, and same number
             *     of counter/meter pairs.
             *         Thus, counter/meter will always be allocated in the 
             *         PRI slice.
             */
            if (NULL != f_st) {
                /* 
                 * Set the index of the counter for entry in new slice
                 * The new index has already been calculated in 
                 * _field_entry_move
                 */

                if (stage_id == _BCM_FIELD_STAGE_EGRESS ) {
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        PID_COUNTER_INDEXf, f_st->hw_index);
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        PID_COUNTER_MODEf, f_st->hw_mode);
                } else if (stage_id == _BCM_FIELD_STAGE_LOOKUP ) {
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        VINTF_CTR_IDXf, f_st->hw_index);
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        USE_VINTF_CTR_IDXf, f_st->hw_mode);
                } else {
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        COUNTER_INDEXf, f_st->hw_index);
                    soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                        COUNTER_MODEf, f_st->hw_mode);
                }
            }
            if (NULL != f_pl) {
                /* 
                 * Set the index of the meter for entry in new slice
                 * The new index has already been calculated in 
                 * _field_entry_move
                 */
                soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                    METER_INDEX_EVENf, f_pl->hw_index);
                soc_mem_field32_set(unit, policy_mem, p_buf[unit][0],
                                    METER_INDEX_ODDf, f_pl->hw_index);
            }
        } else {
            if (NULL != f_st) {
                _bcm_field_fb_counter_adjust_wide_mode(unit, policy_mem, 
                                                       f_st, f_ent, 
                                                       f_ent + 1, 
                                                       new_slice_numb,
                                                       p_buf[unit][0],
                                                       p_buf[unit][1]);
            }
            if (NULL != f_pl) {
                _bcm_field_fb_meter_adjust_wide_mode(unit, policy_mem,
                                                     f_pl, f_ent, f_ent + 1, 
                                                     p_buf[unit][0],
                                                     p_buf[unit][1]);
            }
        } 
    }

    /* 
     * Write entry to the destination
     * ORDER is important
     */ 
    for (idx = parts_count - 1; idx >= 0; idx--) {

        /* Write duplicate  policy entry to new tcam index. */
        rv = soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx_new[idx],
                           p_buf[unit][idx]);
        BCM_IF_ERROR_RETURN(rv);
#ifdef BCM_TRIDENT_SUPPORT
        if ((SOC_IS_TD_TT(unit)) && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            if ((fg->flags & _FP_GROUP_SPAN_DOUBLE_SLICE)
                && (0x1 & idx)) {
                rv = WRITE_FP_GM_FIELDSm(unit, MEM_BLOCK_ALL,
                        tcam_idx_new[idx], &fp_gm_fields[idx]);
                BCM_IF_ERROR_RETURN(rv);
            } else {
                rv = READ_FP_GLOBAL_MASK_TCAM_Xm(unit, MEM_BLOCK_ANY,
                        tcam_idx_old[idx], &fp_global_mask_x[idx]);
                BCM_IF_ERROR_RETURN(rv);
                rv = READ_FP_GLOBAL_MASK_TCAM_Ym(unit, MEM_BLOCK_ANY,
                        tcam_idx_old[idx], &fp_global_mask_y[idx]);
                BCM_IF_ERROR_RETURN(rv);

                soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Xm, &fp_global_mask_x[idx],
                                       IPBMf, &pbmp_x);
                soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Ym, &fp_global_mask_y[idx],
                                       IPBMf, &pbmp_y); 
                BCM_PBMP_OR(pbmp_x, pbmp_y); 

                soc_mem_pbmp_field_set(unit,
                                       FP_GLOBAL_MASK_TCAMm,
                                       &fp_global_mask[idx],
                                       IPBMf,
                                       &pbmp_x
                                       );

                soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Xm, &fp_global_mask_x[idx],
                                       IPBM_MASKf, &pbmp_x);
                soc_mem_pbmp_field_get(unit, FP_GLOBAL_MASK_TCAM_Ym, &fp_global_mask_y[idx],
                                       IPBM_MASKf, &pbmp_y);
                BCM_PBMP_OR(pbmp_x, pbmp_y);

                soc_mem_pbmp_field_set(unit,
                                       FP_GLOBAL_MASK_TCAMm,
                                       &fp_global_mask[idx],
                                       IPBM_MASKf,
                                       &pbmp_x
                                       );


                rv = WRITE_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ALL,
                        tcam_idx_new[idx], &fp_global_mask[idx]);
                BCM_IF_ERROR_RETURN(rv);
            }
        } else
#endif
#if defined (BCM_KATANA_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
        if ((SOC_IS_KATANAX(unit)
             || SOC_IS_TRIUMPH3(unit))
            && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            rv = WRITE_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ALL, 
                tcam_idx_new[idx], &ipbm_entry[idx]);
            BCM_IF_ERROR_RETURN(rv);
        } else
#endif /* BCM_KATANA_SUPPORT */            
#ifdef BCM_TRIUMPH_SUPPORT
        {
            if ((SOC_IS_TR_VL(unit)) && 
                (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
                rv = WRITE_FP_GLOBAL_MASK_TCAMm(unit, MEM_BLOCK_ALL, 
                        tcam_idx_new[idx], &ipbm_entry[idx]);
                BCM_IF_ERROR_RETURN(rv);
            }
        }
#endif /* BCM_TRIUMPH_SUPPORT */

#ifdef BCM_SCORPION_SUPPORT
        /*
         * Writing IPBMf value into FP_TCAMm sets IPBMf in FP_TCAM_Xm
         * and FP_TCAM_Ym but clears IPBM_MASKf in FP_TCAM_Ym unexpectedly. 
         * Similarly setting IPBM_MASKf in FP_TCAMm sets IPBM_MASKf in 
         * FP_TCAM_Xm and FP_TCAM_Ym but clears IPBMf in FP_TCAM_Ym unexpectedly.
         * On scorpion family, read and write entries only from FP_TCAM_Xm and 
         * FP_TCAM_Ym. Avoid Read/Write into FP_TCAMm while moving entries.
         * Read is also not good on FP_TCAMm, as Read operation returns values 
         * from FP_TCAM_Xm only.
         */
        if ((SOC_IS_SC_CQ(unit)) && (stage_id == _BCM_FIELD_STAGE_INGRESS)) {
            rv = soc_mem_read(unit, FP_TCAM_Xm, MEM_BLOCK_ANY,
                              tcam_idx_old[idx], e_buf[unit]);
            BCM_IF_ERROR_RETURN(rv);

            rv = soc_mem_write(unit, FP_TCAM_Xm, MEM_BLOCK_ALL,
                               tcam_idx_new[idx], e_buf[unit]);
            BCM_IF_ERROR_RETURN(rv);

            rv = soc_mem_read(unit, FP_TCAM_Ym, MEM_BLOCK_ANY,
                              tcam_idx_old[idx], e_buf[unit]);
            BCM_IF_ERROR_RETURN(rv);

            rv = soc_mem_write(unit, FP_TCAM_Ym, MEM_BLOCK_ALL,
                               tcam_idx_new[idx], e_buf[unit]);
            BCM_IF_ERROR_RETURN(rv);

        } else 
#endif /* BCM_SCORPION_SUPPORT */
        {
            /* Read tcam entry from current tcam index. */
            rv = soc_mem_read(unit, tcam_mem, MEM_BLOCK_ANY,
                              tcam_idx_old[idx], e_buf[unit]);
            BCM_IF_ERROR_RETURN(rv);

            /* Write duplicate  tcam entry to new tcam index. */
            rv = soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL,
                               tcam_idx_new[idx], e_buf[unit]);
            BCM_IF_ERROR_RETURN(rv);
        }
    }



    /*    
     * fp_entry_move is invoked even for backup entry create 
     * to copy TCAM and FP_POLICY_TABLE tables. In this case
     * TCAM and FP_POLICY_TABLEs should not be erased for
     * original entry.
     */
    if (1 != is_backup_entry) {
        /*
         * Clear old location 
         * ORDER is important
         */
        for (idx = 0; idx < parts_count; idx++) {
            rv = _field_fb_tcam_policy_clear(unit, NULL, stage_id, tcam_idx_old[idx]);
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_stat_action_set
 *
 * Purpose:
 *     Get counter portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number. 
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     mem       - (IN)Policy table memory. 
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_stat_action_set(int unit, _field_entry_t *f_ent,
                       soc_mem_t mem, uint32 *buf)
{
    _field_stat_t    *f_st;    /* Field policer descriptor. */  
    int mode;              /* Counter hw mode.          */                    
    int idx;               /* Counter index.            */

    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_FIREBOLT2_SUPPORT)
    /* VFP doesn't have counters. */
    if ((_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id)) {
        return (BCM_E_NONE);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */


    /* Disable counting if counter was not attached to the entry. */
    if ((0 == (f_ent->statistic.flags & _FP_ENTRY_STAT_INSTALLED)) ||
        ((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_STAT_IN_SECONDARY_SLICE))) {
        idx = 0;
        mode = 0;
    } else {
        /* Get statistics entity description structure. */
        BCM_IF_ERROR_RETURN(_bcm_field_stat_get(unit, f_ent->statistic.sid, &f_st));
        idx = f_st->hw_index;
        mode = f_st->hw_mode;
        /* Adjust counter hw mode for COUNTER_MODE_YES_NO/NO_YES */
        if (f_ent->statistic.flags & _FP_ENTRY_STAT_USE_ODD) {
            mode++;
        }
    }

#if defined(BCM_FIREBOLT2_SUPPORT)
    if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
        PolicySet(unit, mem, buf, PID_INCR_COUNTERf, (mode) ? 1 : 0);
        PolicySet(unit, mem, buf, PID_COUNTER_INDEXf, idx);
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
    { 
        PolicySet(unit, mem, buf, COUNTER_INDEXf, idx);
        PolicySet(unit, mem, buf, COUNTER_MODEf, mode);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_tcam_policy_reinstall
 *
 * Purpose:
 *     Write entry into the chip's memory. 
 *
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location in TCAM
 *
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_tcam_policy_reinstall(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to fill Policy & TCAM entry.*/
    soc_mem_t        tcam_mem;          /* tcAM memory id.                    */
    soc_mem_t        policy_mem;        /* Policy table memory id .           */
    _field_action_t  *fa;               /* Actions iterator.                  */


    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_mem_get(unit, f_ent->fs->stage_id, 
                                       &tcam_mem, &policy_mem));

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* Reset buffer. */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions;
            ((fa != NULL) && (_FP_ACTION_VALID & fa->flags)); fa = fa->next) {
        BCM_IF_ERROR_RETURN(
            _field_fb_action_get(unit, policy_mem, f_ent, tcam_idx, fa, e));
    }

    /* Handle color dependence/independence */
    if (soc_mem_field_valid(unit, policy_mem, GREEN_TO_PIDf)) {
        soc_mem_field32_set(unit, policy_mem, e, GREEN_TO_PIDf, 
                        (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) ? 1 : 0);
    }

    /* Install meter */
    BCM_IF_ERROR_RETURN(_bcm_field_fb_meter_action_set(unit, f_ent, 
                                                       policy_mem, e));

    /* Install counter */
    BCM_IF_ERROR_RETURN(_field_fb_stat_action_set(unit, f_ent, policy_mem, e));

    /* Write policy entry. */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx, e));
 
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_fb_tcam_policy_install
 *
 * Purpose:
 *     Write entry into the chip's memory. 
 *
 * Parameters:
 *     unit     -   BCM Unit
 *     f_ent    -   Physical entry structure to be installed 
 *     tcam_idx - location in TCAM
 *
 * Returns:
 *     BCM_E_XXX        On TCAM read/write errors
 *     BCM_E_NONE
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_tcam_policy_install(int unit, _field_entry_t *f_ent, int tcam_idx)
{
    uint32  e[SOC_MAX_MEM_FIELD_WORDS]; /* Buffer to fill Policy & TCAM entry.*/
    soc_mem_t        tcam_mem;          /* tcAM memory id.                    */
    soc_mem_t        policy_mem;        /* Policy table memory id .           */
    _field_action_t  *fa;               /* Actions iterator.                  */


    /* Input parameters check. */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_mem_get(unit, f_ent->fs->stage_id, 
                                       &tcam_mem, &policy_mem));

    if ((tcam_idx < soc_mem_index_min(unit, tcam_mem)) ||
        (tcam_idx > soc_mem_index_max(unit, tcam_mem))) {
        return (BCM_E_PARAM);
    }

    /* Reset buffer. */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Extract the policy info from the entry structure. */
    for (fa = f_ent->actions;
        ((fa != NULL) && (_FP_ACTION_VALID & fa->flags)); fa = fa->next) {
        BCM_IF_ERROR_RETURN(
            _field_fb_action_get(unit, policy_mem, f_ent, tcam_idx, fa, e));
    }

    /* Handle color dependence/independence */
    if (soc_mem_field_valid(unit, policy_mem, GREEN_TO_PIDf)) {
        soc_mem_field32_set(unit, policy_mem, e, GREEN_TO_PIDf, 
                        (f_ent->flags & _FP_ENTRY_COLOR_INDEPENDENT) ? 1 : 0);
    }

    /* Install meter */
    BCM_IF_ERROR_RETURN(_bcm_field_fb_meter_action_set(unit, f_ent, 
                                                       policy_mem, e));

    /* Install counter */
    BCM_IF_ERROR_RETURN(_field_fb_stat_action_set(unit, f_ent, policy_mem, e));

    /* Write policy entry. */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, policy_mem, MEM_BLOCK_ALL, tcam_idx, e));
 
    /* Reset buffer. */
    sal_memset(e, 0, SOC_MAX_MEM_FIELD_WORDS * sizeof(uint32));

    /* Extract the qualifier info from the entry structure. */
    BCM_IF_ERROR_RETURN(
        _field_fb_tcam_get(unit, f_ent, tcam_mem, e));

    /* Write tcam entry. */
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, tcam_mem, MEM_BLOCK_ALL, tcam_idx, e));
 
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_fb_tcam_get
 *
 * Purpose:
 *     Write entry into the chip's memory.
 *
 * Parameters:
 *     unit      -  (IN)BCM device number.  
 *     f_ent     -  (IN)Entry structure to get tcam info from.
 *     mem       -  (IN)Tcam memory.  
 *     buf       - (OUT) TCAM hardware entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_tcam_get(int unit, _field_entry_t *f_ent,
                   soc_mem_t mem, uint32 *buf)
{
    _field_tcam_t       *tcam; 
    _field_group_t      *fg;
    uint8               ent_part;
    int                 test_var;

    if ((NULL == f_ent) || (NULL == buf)) {
        return (BCM_E_PARAM);
    }

    tcam = &f_ent->tcam;
    fg = f_ent->group;

    if (_BCM_FIELD_STAGE_INGRESS == fg->stage_id) {
        soc_mem_field32_set(unit, mem, buf, VALIDf, 
                            (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 2);
        if (f_ent->flags & _FP_ENTRY_SECOND_HALF) {
            /* Intra-slice double wide key */
#ifdef BCM_FIREBOLT2_SUPPORT
            if (SOC_IS_FIREBOLT2(unit)) {
                soc_mem_field_set(unit, mem, buf, DATAf, tcam->key);
                soc_mem_field_set(unit, mem, buf, DATA_MASKf, tcam->mask);
            }
#endif /* BCM_FIREBOLT2_SUPPORT */
        } else {
            /* Single wide key */
            soc_mem_field_set(unit, mem, buf, KEYf, tcam->key);
            soc_mem_field_set(unit, mem, buf, MASKf, tcam->mask);

            /* 
             * Notes:
             *     1. SOURCE_PORT_NUMBER is same as F4.0
             *     2. DROP is part of F4.1
             *     3. PORT_FIELD_SEL_INDEX is same as F4.2 (not implemented
             *                                                  2006.01.18)
             */
            if ((BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyInPort)) &&
                (SOC_MEM_FIELD_VALID(unit, mem, SOURCE_PORT_NUMBERf))) {
                soc_mem_field32_set(unit, mem, buf, SOURCE_PORT_NUMBERf, 
                                    tcam->f4);
                soc_mem_field32_set(unit, mem, buf, SOURCE_PORT_NUMBER_MASKf, 
                                    tcam->f4_mask);
            } else if (SOC_MEM_FIELD_VALID(unit, mem, F4f)) {
                soc_mem_field32_set(unit, mem, buf, F4f, tcam->f4);
                soc_mem_field32_set(unit, mem, buf, F4_MASKf, tcam->f4_mask);
            }

            /* 
             * Check if IPBM_SEL needs to be set
             *     Will be set if fpf1.0 is selected
             * First find out the part of the group, entry resides in
             *     Then check the corresponding sel_codes
             */ 
            BCM_IF_ERROR_RETURN
                (_bcm_field_entry_flags_to_tcam_part (unit, f_ent->flags,
                                                      fg,
                                                      &ent_part));
            test_var = (fg->sel_codes[ent_part].fpf1 == 0) ? 1 : 0;

            soc_mem_field32_set(unit, mem, buf, IPBM_SELf, test_var);
            soc_mem_field32_set(unit, mem, buf, IPBM_SEL_MASKf, test_var);

            /*
             * Qualify on HiGig packets.
             */
            test_var =  
                (BCM_FIELD_QSET_TEST(fg->qset,
                                     bcmFieldQualifyHiGig) && tcam->higig) ? 1 : 0; 
            soc_mem_field32_set(unit, mem, buf, PKT_TYPEf, test_var);

            test_var =  
                (BCM_FIELD_QSET_TEST(fg->qset,
                                     bcmFieldQualifyHiGig) && tcam->higig_mask) ? 1 : 0;
            soc_mem_field32_set(unit, mem, buf, PKT_TYPE_MASKf, test_var);
#ifdef BCM_FIREBOLT2_SUPPORT 
            /*
             * Qualify on Drop.
             */
            if (SOC_IS_FIREBOLT2(unit)) {
                test_var =  
                    (BCM_FIELD_QSET_TEST(fg->qset,
                                         bcmFieldQualifyDrop) && tcam->drop) ? 1 : 0; 

                soc_mem_field32_set(unit, mem, buf, DROPf, test_var);

                test_var =  
                    (BCM_FIELD_QSET_TEST(fg->qset,
                                         bcmFieldQualifyDrop) && tcam->drop_mask) ? 1 : 0;
                soc_mem_field32_set(unit, mem, buf, DROP_MASKf, test_var);
            }
#endif
        }
#ifdef BCM_FIREBOLT2_SUPPORT
    } else if (soc_feature(unit, soc_feature_field_multi_stage)) {
        soc_field_t mask_field;
        if (_BCM_FIELD_STAGE_LOOKUP == fg->stage_id) {
            mask_field = MASKf;
        } else if (_BCM_FIELD_STAGE_EGRESS == fg->stage_id){
            mask_field = KEY_MASKf;
        } else {
            return (BCM_E_PARAM);
        }
        soc_mem_field_set(unit, mem, buf, KEYf, tcam->key);
        soc_mem_field_set(unit, mem, buf, mask_field, tcam->mask);
        soc_mem_field32_set(unit, mem, buf, VALIDf, 
                            (fg->flags & _FP_GROUP_LOOKUP_ENABLED) ? 3 : 2);
#endif
    } else {
        return (BCM_E_INTERNAL);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_meter_action_set
 *
 * Purpose:
 *     Get metering portion of Policy Table.
 *
 * Parameters:
 *     unit      - (IN)BCM device number. 
 *     f_ent     - (IN)Software entry structure to get tcam info from.
 *     mem       - (IN)Policy table memory. 
 *     buf       - (IN/OUT)Hardware policy entry
 *
 * Returns:
 *     BCM_E_NONE  - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_fb_meter_action_set(int unit, _field_entry_t *f_ent,
                               soc_mem_t mem, uint32 *buf)
{
    _field_policer_t *f_pl;             /* Field policer descriptor. */  
    uint32           meter_pair_mode;   /* Meter usage.              */ 

    /* Input parameter check. */
    if ((NULL == f_ent) || (NULL == buf))  {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

#if defined(BCM_FIREBOLT2_SUPPORT) 
    /* VFP doesn't have meters. */
    if ((_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id)) {
        return (BCM_E_NONE);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    

    if ((0 == (f_ent->policer[0].flags & _FP_POLICER_INSTALLED)) ||
        ((f_ent->flags & _FP_ENTRY_PRIMARY) && 
         (f_ent->flags & _FP_ENTRY_POLICER_IN_SECONDARY_SLICE))) {
        /* If no meter, install default meter pair mode.
         * Default meter pair mode doesn't need to burn a real meter.
         */
        soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, 0);
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);

#if defined(BCM_FIREBOLT2_SUPPORT)
        if ((SOC_IS_FIREBOLT2(unit) && 
             _BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id)) {
            soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, 4);
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
        return (BCM_E_NONE);
    }

    /* Get policer description structure. */
    BCM_IF_ERROR_RETURN(_bcm_field_policer_get(unit, f_ent->policer[0].pid, 
                                               &f_pl));
    /* Get the even and odd indexes from the entry. The even and odd
     * meter indices are the same.
     */
    soc_mem_field32_set(unit, mem, buf, METER_INDEX_EVENf, 
                        f_pl->hw_index);
    soc_mem_field32_set(unit, mem, buf, METER_INDEX_ODDf, 
                        f_pl->hw_index);


    /* Get hw encoding for meter mode. */
    BCM_IF_ERROR_RETURN
        (_bcm_field_meter_pair_mode_get(unit, f_pl, &meter_pair_mode));

    /* 
     * Flow mode is the only one that cares about the test and update bits.
     * Even = BCM_FIELD_METER_PEAK
     * Odd = BCM_FIELD_METER_COMMITTED
     */
    if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
        soc_mem_field32_set(unit, mem, buf, METER_TEST_ODDf, 0);
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
        soc_mem_field32_set(unit, mem, buf, METER_UPDATE_ODDf, 0);
        soc_mem_field32_set(unit, mem, buf, METER_UPDATE_EVENf, 1);
    } else if (_FP_POLICER_COMMITTED_HW_METER(f_pl)) {
        soc_mem_field32_set(unit, mem, buf, METER_TEST_ODDf, 1);
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 0);
        soc_mem_field32_set(unit, mem, buf, METER_UPDATE_ODDf, 1);
        soc_mem_field32_set(unit, mem, buf, METER_UPDATE_EVENf, 0);
    } else if (f_pl->cfg.mode == bcmPolicerModePassThrough) {
        if (SOC_MEM_FIELD_VALID(unit, mem, METER_PAIR_MODE_MODIFIERf)) {
            soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODE_MODIFIERf, 1);
        } 
        soc_mem_field32_set(unit, mem, buf, METER_TEST_EVENf, 1);
    }

    soc_mem_field32_set(unit, mem, buf, METER_PAIR_MODEf, meter_pair_mode);
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_ingress_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *
 * Parameters:
 *     unit          - (IN) BCM device number.
 *     fg            - (IN) Field group.
 *     slice_numb    - (IN) Slice number group installed in. 
 *     pbmp          - (IN) Group  active port bit map.
 *     selcode_index - (IN) Index into select codes array.
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_ingress_selcodes_install(int unit, _field_group_t *fg, 
                                  uint8 slice_numb, bcm_pbmp_t pbmp, 
                                  int selcode_index)
{
    bcm_port_t                  port;
    fp_port_field_sel_entry_t   pfs_entry;
    ifp_port_field_sel_entry_t  ipfs_entry;
    soc_field_t                 f1_field, f2_field, f3_field;
    uint32                      fpf4;
    int                         i_write_flag;
    _field_sel_t                *sel = NULL;
    int                         rv;
#if defined(BCM_FIREBOLT2_SUPPORT) 
    soc_field_t  dw_fld;
    bcm_pbmp_t   all_pbmp;
#endif /* BCM_FIREBOLT2_SUPPORT */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    } 

    sel = &fg->sel_codes[selcode_index];

    if (slice_numb >= 16) {
        return (BCM_E_PARAM);
    }


    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) && 
        ((selcode_index == 1) || (selcode_index == 3))) {
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (SOC_IS_FIREBOLT2(unit)) {

            /* Write appropriate values in FP_DOUBLE_WIDE_F4_SELECTr and */
            /* FP_PORT_FIELD_SELm for an intra-slice double wide group */
            /* V4 key => F4 should be FP_PORT_FIELD_SEL_INDEX: sel->fpf2 = 0 */
            /* V6 key => F4 should be TCP flags: sel->fpf2 = 1*/
            dw_fld = _fb_ing_f4_reg[slice_numb];
            if ((sel->fpf2 != _FP_SELCODE_DONT_CARE) &&
                SOC_REG_FIELD_VALID(unit, FP_DOUBLE_WIDE_F4_SELECTr, dw_fld)) { 
                rv = soc_reg_field32_modify(unit, FP_DOUBLE_WIDE_F4_SELECTr, 
                                            REG_PORT_ANY, dw_fld,  sel->fpf2);
                BCM_IF_ERROR_RETURN(rv);
            }

            /* Read device port configuration. */ 
            rv = _bcm_field_valid_pbmp_get(unit, &all_pbmp); 
            BCM_IF_ERROR_RETURN(rv);

            dw_fld = _fb2_ifp_double_wide_key[slice_numb]; 
            if ((sel->fpf2 != _FP_SELCODE_DONT_CARE) &&
                SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
                /* Do the same thing for each entry in FP_PORT_FIELD_SEL table */
                BCM_PBMP_ITER(all_pbmp, port) {
                    rv = READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, port, &pfs_entry); 
                    BCM_IF_ERROR_RETURN(rv);

                    soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, 
                                                       dw_fld, sel->fpf2);
                    rv = WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port, &pfs_entry);
                    BCM_IF_ERROR_RETURN(rv);

                    if (_FIELD_NEED_I_WRITE(unit, port, IFP_PORT_FIELD_SELm)) {
                        rv = READ_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, 
                                                      port, &pfs_entry);
                        BCM_IF_ERROR_RETURN(rv);

                        soc_IFP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, 
                                                            dw_fld, sel->fpf2);

                        rv = WRITE_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, 
                                                       port, &pfs_entry);
                        BCM_IF_ERROR_RETURN(rv);
                    }
                }
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
    } else {

        /* Determine which 3 fields will be modified */
        f1_field = _fb_field_tbl[slice_numb][0];
        f2_field = _fb_field_tbl[slice_numb][1];
        f3_field = _fb_field_tbl[slice_numb][2];

        /* Iterate over all ports */
        PBMP_ITER(pbmp, port) {
            i_write_flag = _FIELD_NEED_I_WRITE(unit, port,
                                               IFP_PORT_FIELD_SELm);
            /* Read Port's current entry in FP_PORT_FIELD_SEL table */
            rv = READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                         port, &pfs_entry);
            BCM_IF_ERROR_RETURN(rv);
            if (i_write_flag) {
                rv = READ_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                              port, &ipfs_entry);
                BCM_IF_ERROR_RETURN(rv);
            }

            /* modify 0-3 fields depending on state of SELCODE_INVALID */
            if (sel->fpf1 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                                   f1_field, sel->fpf1);
                if (i_write_flag) {
                    soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                        f1_field, sel->fpf1);
                }
            }

            if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                                   f2_field,
                                                   sel->fpf2);
                if (i_write_flag) {
                    soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry,
                                                        f2_field,
                                                        sel->fpf2);
                }
            }

            if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                                   f3_field,
                                                   sel->fpf3);
                if (i_write_flag) {
                    soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                        f3_field,
                                                        sel->fpf3);
                }
            }

            /* Write each port's new entry */
            rv = WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                          port, &pfs_entry);
            BCM_IF_ERROR_RETURN(rv);
            if (i_write_flag) {
                rv = WRITE_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                               port, &ipfs_entry);
                BCM_IF_ERROR_RETURN(rv);
            }
        }

        /* If the device supports it, write the F4 select code. */
        if (soc_feature(unit, soc_feature_field_qual_drop)) {
            fpf4 = (sel->fpf4 == _FP_SELCODE_DONT_CARE) ? 0 : sel->fpf4;
            rv = soc_reg_field32_modify(unit, FP_F4_SELECTr, REG_PORT_ANY, 
                                        _fb_ing_f4_reg[slice_numb], fpf4);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Set inner vlan overlay config. */
        if (_FP_SELCODE_DONT_CARE != fg->sel_codes[0].inner_vlan_overlay) {
#if defined(BCM_FIREBOLT2_SUPPORT)  || defined(BCM_RAVEN_SUPPORT)
            if (SOC_REG_FIELD_VALID(unit, ING_MISC_CONFIG2r, 
                                    FP_INNER_VLAN_OVERLAY_ENABLEf)) {
                rv = soc_reg_field32_modify(unit, ING_MISC_CONFIG2r, REG_PORT_ANY,
                                            FP_INNER_VLAN_OVERLAY_ENABLEf,
                                            fg->sel_codes[0].inner_vlan_overlay);
                BCM_IF_ERROR_RETURN(rv);
            } else 
#endif  /* BCM_FIREBOLT2_SUPPORT  || BCM_RAVEN_SUPPORT */
                if (SOC_REG_FIELD_VALID(unit, ING_CONFIGr, MAC_BLOCK_INDEX_OVERLAYf)) {
                    rv = soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                                MAC_BLOCK_INDEX_OVERLAYf,
                                                fg->sel_codes[0].inner_vlan_overlay);
                    BCM_IF_ERROR_RETURN(rv);
                }
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_ingress_slice_clear
 *
 * Purpose:
 *     Resets the IFP field slice configuration.
 *
 * Parameters:
 *     unit       - (IN) BCM device number.
 *     slice_numb - (IN) Field slice number.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_ingress_slice_clear(int unit, uint8 slice_numb)
{
    bcm_port_t                  port;
    fp_port_field_sel_entry_t   pfs_entry;
    ifp_port_field_sel_entry_t  ipfs_entry;
    bcm_pbmp_t                  all_pbmp;
    soc_field_t                 f1_field, f2_field, f3_field;
    int                         i_write_flag;
    int                         rv;
#if defined (BCM_FIREBOLT2_SUPPORT)
    soc_field_t                 dw_fld;
#endif /* BCM_FIREBOLT2_SUPPORT */

    f1_field = _fb_field_tbl[slice_numb][0];
    f2_field = _fb_field_tbl[slice_numb][1];
    f3_field = _fb_field_tbl[slice_numb][2];

    /* Read device port configuration. */ 
    BCM_PBMP_CLEAR(all_pbmp);
    BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

    /* Iterate over all ports */
    BCM_PBMP_ITER(all_pbmp, port) {
        i_write_flag = _FIELD_NEED_I_WRITE(unit, port, IFP_PORT_FIELD_SELm);
#if defined (BCM_RAVEN_SUPPORT)
        /* IFP_PORT_FIELD_SEL table will be automatically updated by the 
           hardware when FP_FIELD_PORT_SEL table is configured for hg ports.
           So configuring IFP_PORT_FIELD_SEL table again from software is  
           not required. However, when cpu port is configured in higig mode 
           (PORT_TAB.HIGIG_PACKET) software has to update this table */
        if (SOC_IS_RAVEN(unit) && !IS_CPU_PORT(unit, port)) {
           i_write_flag = 0;
        }
#endif
        /* Read Port's current entry in FP_PORT_FIELD_SEL table */
        rv = READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, 
                                     port, &pfs_entry);
        BCM_IF_ERROR_RETURN(rv);

        if (i_write_flag) {
            rv = READ_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL,
                                          port, &ipfs_entry);
            BCM_IF_ERROR_RETURN(rv);
        }

        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           f1_field, 0);
        if (i_write_flag) {
            soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                f1_field, 0);
        }

        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           f2_field, 0);
        if (i_write_flag) {
            soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry,
                                                f2_field, 0);
        }

        soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry,
                                           f3_field, 0);
        if (i_write_flag) {
            soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                f3_field, 0);
        }

#if defined (BCM_FIREBOLT2_SUPPORT)
        dw_fld = _fb2_ifp_double_wide_key[slice_numb]; 
        if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, 
                                               dw_fld, 0);
            if (i_write_flag) {
                soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry, 
                                                    dw_fld, 0);
            }
        }

        dw_fld = _fb2_slice_wide_mode_field[slice_numb];
        if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, dw_fld, 0);
            if (i_write_flag) {
                soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry,
                                                    dw_fld, 0);
            }
        }

        dw_fld = _fb2_slice_pairing_field[slice_numb / 2];
        if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, dw_fld)) {
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, dw_fld, 0);
            if (i_write_flag) {
                soc_IFP_PORT_FIELD_SELm_field32_set(unit, &ipfs_entry,
                                                    dw_fld, 0);
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */

        rv = WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port,
                                      &pfs_entry);
        BCM_IF_ERROR_RETURN(rv);

        if (i_write_flag) {
            rv = WRITE_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port,
                                          &ipfs_entry);
            BCM_IF_ERROR_RETURN(rv);
        }
    }

    if (SOC_REG_FIELD_VALID(unit, FP_SLICE_CONFIGr, 
                            _fb_ing_slice_mode_field[slice_numb])) {
        rv = soc_reg_field32_modify(unit, FP_SLICE_CONFIGr, REG_PORT_ANY,
                                    _fb_ing_slice_mode_field[slice_numb], 0);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* If the device supports it, write the F4 select code. */
    if (SOC_REG_FIELD_VALID(unit, FP_F4_SELECTr, _fb_ing_f4_reg[slice_numb])) {
        rv = soc_reg_field32_modify(unit, FP_F4_SELECTr, REG_PORT_ANY,
                                    _fb_ing_f4_reg[slice_numb],0);
        BCM_IF_ERROR_RETURN(rv);
    }
#if defined (BCM_FIREBOLT2_SUPPORT)
    dw_fld = _fb_ing_f4_reg[slice_numb];
    if (SOC_REG_FIELD_VALID(unit, FP_DOUBLE_WIDE_F4_SELECTr, dw_fld)) {
        rv = soc_reg_field32_modify(unit, FP_DOUBLE_WIDE_F4_SELECTr,
                                    REG_PORT_ANY, dw_fld,  0);
        BCM_IF_ERROR_RETURN(rv);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */
    return (BCM_E_NONE);
}

#if defined(BCM_FIREBOLT2_SUPPORT) 

/*
 * Function:
 *     _field_fb_lookup_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *     for VFP (_BCM_FIELD_STAGE_LOOKUP) lookup stage.
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_lookup_selcodes_install(int unit, _field_group_t *fg, 
                                  uint8 slice_numb, int selcode_index)
{
    uint32        reg_val;
    _field_sel_t  *sel;

    sel = &fg->sel_codes[selcode_index];


    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val)); 
    if ((fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) && 
        (selcode_index % 2)) {
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              DOUBLE_WIDE_KEY_SELECTf, sel->fpf2);
        }
    } else {
        /* FPF1 is fixed in Firebolt2 VFP */
        if (sel->fpf2 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              _vfp_field_tbl[slice_numb][0], sel->fpf2);
        }
        if (sel->fpf3 != _FP_SELCODE_DONT_CARE) {
            soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                              _vfp_field_tbl[slice_numb][1], sel->fpf3); 
        }
    }
    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_lookup_slice_clear
 *
 * Purpose:
 *     Reset slice configuraton on group deletion event. 
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_fb_lookup_slice_clear(int unit, uint8 slice_numb)
{
    uint32        reg_val;

    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val)); 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      DOUBLE_WIDE_KEY_SELECTf, 0);
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_field_tbl[slice_numb][0], 0);
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_field_tbl[slice_numb][1], 0); 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_slice_pairing_field[slice_numb / 2], 0);
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_slice_wide_mode_field[slice_numb], 0);
    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));
    return (BCM_E_NONE);
}
#endif /* BCM_FIREBOLT2_SUPPORT */

/*
 * Function:
 *     _field_fb_ingress_mode_set
 *
 * Purpose:
 *     Helper function to _bcm_field_fb_mode_install that sets the mode of a
 *     slice in a register value that is to be used for FP_SLICE_CONFIGr.
 *
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Installed group structure. 
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
int 
_field_fb_ingress_mode_set(int unit, uint8 slice_numb, 
                           _field_group_t *fg, uint8 flags)
{
    int     mode;
#if defined(BCM_FIREBOLT2_SUPPORT)
    bcm_pbmp_t  all_pbmp;
    int paired, dbl_wide;
    soc_field_t fld;
#endif /* BCM_FIREBOLT2_SUPPORT */

    if (slice_numb >= COUNTOF(_fb_ing_slice_mode_field)) {
        return BCM_E_PARAM;
    }


#if defined(BCM_FIREBOLT2_SUPPORT)
    if (soc_feature(unit, soc_feature_field_intraslice_double_wide)) {
        int port;
        fp_port_field_sel_entry_t pfs_entry;

        /* Read device port configuration. */ 
        BCM_PBMP_CLEAR(all_pbmp);
        BCM_IF_ERROR_RETURN(_bcm_field_valid_pbmp_get(unit, &all_pbmp));

        /* Do the same thing for each port (index) in FP_PORT_FIELD_SEL table */
        BCM_PBMP_ITER(all_pbmp, port) {

            BCM_IF_ERROR_RETURN
                (READ_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, port, &pfs_entry));

            fld = _fb2_slice_wide_mode_field[slice_numb];
            if (SOC_MEM_FIELD_VALID(unit, FP_PORT_FIELD_SELm, fld)) {
                dbl_wide = (flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ? 1 : 0;

                soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, fld,
                                                   dbl_wide);
            }

            paired = (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ? 1 : 0;
            fld = _fb2_slice_pairing_field[slice_numb / 2];
            soc_FP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, fld, paired);

            BCM_IF_ERROR_RETURN
                (WRITE_FP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port, &pfs_entry));

            if (_FIELD_NEED_I_WRITE(unit, port, IFP_PORT_FIELD_SELm)) {

                BCM_IF_ERROR_RETURN
                    (READ_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ANY, port, &pfs_entry));

                fld = _fb2_slice_wide_mode_field[slice_numb];
                if (SOC_MEM_FIELD_VALID(unit, IFP_PORT_FIELD_SELm, fld)) {
                    dbl_wide = (flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ? 1 : 0;
                    soc_IFP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, fld,
                                                        dbl_wide);
                }

                paired = (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ? 1 : 0;
                fld = _fb2_slice_pairing_field[slice_numb / 2];
                soc_IFP_PORT_FIELD_SELm_field32_set(unit, &pfs_entry, fld,
                                                    paired);

                BCM_IF_ERROR_RETURN
                    (WRITE_IFP_PORT_FIELD_SELm(unit, MEM_BLOCK_ALL, port,
                                               &pfs_entry));
            }

        }
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
    {
        if (flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
            mode = 0;
        } else if (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
            mode = 1;
        } else if (flags & _FP_GROUP_SPAN_TRIPLE_SLICE) {
            mode = 2; 
        } else {
            return (BCM_E_PARAM);
        }

        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, FP_SLICE_CONFIGr, REG_PORT_ANY,
                                    _fb_ing_slice_mode_field[slice_numb],
                                    mode));
    }
    return (BCM_E_NONE);
}

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
/*
 * Function:
 *     _bcm_field_fb_lookup_mode_set
 *
 * Purpose:
 *     Helper function to _bcm_field_fb_mode_install that sets the mode of a
 *     slice in a register value that is to be used for VFP_KEY_CONTROLr.
 *
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Filed group structure.
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
int 
_bcm_field_fb_lookup_mode_set(int unit, uint8 slice_numb, 
                              _field_group_t *fg, uint8 flags) 
{
    int paired,dbl_wide;
    uint32  reg_val;
    if (slice_numb >= COUNTOF(_vfp_slice_wide_mode_field)) {
        return (BCM_E_PARAM);
    }
    SOC_IF_ERROR_RETURN(READ_VFP_KEY_CONTROLr(unit, &reg_val));

    paired = (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) ? 1 : 0; 
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_slice_pairing_field[slice_numb / 2], paired);

    dbl_wide = (flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE) ? 1 : 0;
    soc_reg_field_set(unit, VFP_KEY_CONTROLr, &reg_val,
                      _vfp_slice_wide_mode_field[slice_numb], dbl_wide);
    
    SOC_IF_ERROR_RETURN(WRITE_VFP_KEY_CONTROLr(unit, reg_val));
    return (BCM_E_NONE);
}

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT*/

#if defined(BCM_FIREBOLT2_SUPPORT) 
/*
 * Function:
 *     _field_fb_egress_mode_set
 *
 * Purpose:
 *     Helper function to _bcm_field_fb_mode_install that sets the mode of a
 *     slice in a register value that is to be used for FP_SLICE_CONFIGr.
 *
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Installed group structure. 
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_egress_mode_set(int unit, uint8 slice_numb, 
                          _field_group_t *fg, uint8 flags)
{
    int                mode_val;

    /* Input parameters check. */
    if ((NULL == fg) || (slice_numb >= COUNTOF(_efp_slice_mode))) {
        return (BCM_E_PARAM);
    }

    if (flags & _FP_GROUP_SPAN_DOUBLE_SLICE) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY,
                                    _efp_slice_mode[slice_numb],
                                    _BCM_FIELD_EGRESS_SLICE_MODE_DOUBLE));
    } else {
        mode_val = (3 == fg->sel_codes[0].fpf3) ?
            _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L2 :
            _BCM_FIELD_EGRESS_SLICE_MODE_SINGLE_L3;
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY,
                                    _efp_slice_mode[slice_numb], mode_val));
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_fb_egress_slice_clear
 *
 * Purpose:
 *     Reset slice configuraton on group deletion event. 
 *
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_egress_slice_clear(int unit, uint8 slice_numb)
{
    int rv; 

    /* Input parameters check. */
    if (slice_numb >= COUNTOF(_efp_slice_mode)) {
        return (BCM_E_PARAM);
    }

    rv = soc_reg_field32_modify(unit, EFP_SLICE_CONTROLr, REG_PORT_ANY,
                                _efp_slice_mode[slice_numb], 0);
    return rv;
}
#endif /* BCM_FIREBOLT2_SUPPORT */


/*
 * Function:
 *     _bcm_field_fb_mode_set
 *
 * Purpose:
 *    Auxiliary routine used to set group pairing mode.
 * Parameters:
 *     unit       - (IN) BCM device number. 
 *     slice_numb - (IN) Slice number to set mode for.
 *     fg         - (IN) Installed group structure. 
 *     flags      - (IN) New group/slice mode.
 *
 * Returns:
 *     BCM_E_XXX
 */
int 
_bcm_field_fb_mode_set(int unit, uint8 slice_numb, _field_group_t *fg, uint8 flags) 
{
    int rv;     /* Operation return status. */

    /* Input parameter check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    if (!soc_feature(unit, soc_feature_field_wide)) {
        if (flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
            return (BCM_E_NONE);
        }
        return (BCM_E_UNAVAIL);
    }

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _field_fb_ingress_mode_set(unit, slice_numb, fg, flags);
          break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case _BCM_FIELD_STAGE_LOOKUP:
          rv  = _bcm_field_fb_lookup_mode_set(unit, slice_numb, fg, flags);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_fb_egress_mode_set(unit, slice_numb, fg, flags);
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default: 
          rv = BCM_E_PARAM;
    }
    return (rv);
}


/*
 * Function:
 *     _field_selcodes_install
 *
 * Purpose:
 *     Writes the field select codes (ie. FPFx).
 *
 * Parameters:
 *     unit  - BCM device number
 *     fs    - slice that needs its select codes written
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
STATIC int
_field_selcodes_install(int unit, _field_group_t *fg, 
                           uint8 slice_numb, bcm_pbmp_t pbmp, 
                           int selcode_index)
{
    int rv;    /* Operation return status. */

    /* Input parameters check. */
    if (NULL == fg) {
        return (BCM_E_PARAM);
    }
    

    /* Set slice mode. Single/Double/Triple, Intraslice */
    rv = _bcm_field_fb_mode_set(unit, slice_numb, fg, fg->flags);
    BCM_IF_ERROR_RETURN(rv);

    switch (fg->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS: 
          rv = _field_fb_ingress_selcodes_install(unit, fg, slice_numb,
                                                  pbmp, selcode_index);
          break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_fb_lookup_selcodes_install(unit, fg, slice_numb,
                                                 selcode_index);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = (BCM_E_NONE);
          break;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
      default:
          rv = (BCM_E_PARAM);
    }
    return (rv);
}

/*
 * Function:
 *     _bcm_field_fb_group_install
 *
 * Purpose:
 *     Writes the Group's mode and field select codes into hardware. This
 *     should be called both at group creation time and any time the select
 *     codes change (i.e. bcm_field_group_set calls).
 *
 * Parameters:
 *     unit  - BCM device number
 *     fg    - group to install
 *
 * Returns:
 *     BCM_E_INTERNAL - On read/write errors
 *     BCM_E_NONE     - Success
 *
 * Note:
 *     Unit lock should be held by calling function.
 */
int
_bcm_field_fb_group_install(int unit, _field_group_t *fg) 
{
    _field_slice_t *fs;        /* Slice pointer.           */
    uint8  slice_number;       /* Slices iterator.         */
    int    parts_count;        /* Number of entry parts.   */
    int    idx;                /* Iteration index.         */
    int    rv;                 /* Operation return status. */

    if (NULL == fg) {
        return (BCM_E_PARAM);
    }

    /* Get number of entry parts. */
    rv = _bcm_field_entry_tcam_parts_count (unit, fg->stage_id,
                                            fg->flags, &parts_count);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < parts_count;  idx++) {

        /* Get slice id for entry part */
        rv = _bcm_field_tcam_part_to_slice_number(unit, idx, fg,
                                                  &slice_number);
        BCM_IF_ERROR_RETURN(rv);

        /* Get slice pointer. */
        fs = fg->slices + slice_number;

#if defined(BCM_TOMAHAWK_SUPPORT)
        if (soc_feature(unit, soc_feature_field_multi_pipe_support)) {
            rv = _bcm_field_th_selcodes_install (unit, fg, fs->slice_number,
                                                  fg->pbmp, idx);
        } else
#endif 
#if defined(BCM_TRX_SUPPORT)
        if (SOC_IS_TRX(unit) && !(SOC_IS_HURRICANE2(unit) ||
            SOC_IS_GREYHOUND(unit) || SOC_IS_HURRICANE3(unit) ||
            SOC_IS_GREYHOUND2(unit))) {
            rv = _bcm_field_trx_selcodes_install (unit, fg, fs->slice_number, 
                                                  fg->pbmp, idx);
        } else
#if defined(BCM_HURRICANE2_SUPPORT)
        if (SOC_IS_HURRICANE2(unit) || SOC_IS_GREYHOUND(unit) ||
            SOC_IS_HURRICANE3(unit) || SOC_IS_GREYHOUND2(unit)) {
            rv = _bcm_field_hu2_selcodes_install (unit, fg, fs->slice_number,
                                                  fg->pbmp, idx);
        } else
#endif  /* BCM_HURRICANE2_SUPPORT */
#endif /* BCM_TRX_SUPPORT */
        {
            rv = _field_selcodes_install(unit, fg, fs->slice_number, fg->pbmp, idx);
        }
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_slice_clear
 *
 * Purpose:
 *     Clear slice configuration on group removal
 *
 * Parameters:
 *     unit  - BCM device number
 *     fg    - Field group slice belongs to
 *     fs    - Field slice structure.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_slice_clear(int unit, _field_group_t *fg, _field_slice_t *fs) 
{
    int rv;

    switch (fs->stage_id) {
      case _BCM_FIELD_STAGE_INGRESS:
          rv = _field_fb_ingress_slice_clear(unit, fs->slice_number);
          break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case _BCM_FIELD_STAGE_LOOKUP:
          rv = _field_fb_lookup_slice_clear(unit, fs->slice_number);
          break;
      case _BCM_FIELD_STAGE_EGRESS:
          rv = _field_fb_egress_slice_clear(unit, fs->slice_number);
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default: 
          rv = BCM_E_INTERNAL;
    }

    return (rv);
}

/*
 * Function:
 *     _bcm_field_fb_entry_remove
 * Purpose:
 *     Remove a previously installed physical entry.
 * Parameters:
 *     unit   - BCM device number
 *     f_ent  - Physical entry data
 * Returns:
 *     BCM_E_XXX
 *     BCM_E_NONE
 * Notes:
 *     FP unit lock should be held by calling function.
 */
int
_bcm_field_fb_entry_remove(int unit, _field_entry_t *f_ent, int tcam_idx)
{
#ifdef BCM_KATANA_SUPPORT
    uint32 param0 = 0, param1 = 0, in_flags = 0;
    int queue_id = 0;
    ing_queue_map_entry_t ing_queue_entry;
    int referenceCount = 0;
    _field_action_t *fa = NULL;
#endif
#ifdef BCM_KATANA2_SUPPORT
    int rv = BCM_E_NONE;
#endif

    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->fs) {
        return (BCM_E_PARAM);
    }


    BCM_IF_ERROR_RETURN
        (_field_fb_tcam_policy_clear(unit, f_ent, f_ent->fs->stage_id, tcam_idx));

#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANAX(unit)) {
        if ((f_ent->fs->stage_id == _BCM_FIELD_STAGE_INGRESS) &&
            (!(f_ent->flags & _FP_ENTRY_DIRTY)) ) {

            for (fa = f_ent->actions; fa != NULL; fa = fa->next) {
                if (fa->action == bcmFieldActionFabricQueue) {
                    sal_memset(&ing_queue_entry, 0,
                               sizeof(ing_queue_map_entry_t));
                    param0 = fa->param[0];
                    param1 = fa->param[1];
                    /* For Katana if param1 BCM_FABRIC_QUEUE_xxx
                     * flags are set then param0 is
                     * ucast/ucast_subscriber group cosq gport */
                    if ((param1 & BCM_FABRIC_QUEUE_DEST_OFFSET) ||
                        (param1 & BCM_FABRIC_QUEUE_CUSTOMER)) {
                        if(BCM_GPORT_IS_UCAST_QUEUE_GROUP(param0) ||
                           BCM_GPORT_IS_UCAST_SUBSCRIBER_QUEUE_GROUP(param0) ||
                           BCM_GPORT_IS_MCAST_SUBSCRIBER_QUEUE_GROUP(param0)) {
#if defined(BCM_KATANA2_SUPPORT)
                            if (SOC_IS_KATANA2(unit)) {
                                queue_id = -1;
                                rv = _bcm_kt2_cosq_index_resolve(
                                      unit, param0, 0,
                                      _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                                      NULL, &queue_id, NULL);
                                if (BCM_FAILURE(rv) &&
                                     (rv != BCM_E_NOT_FOUND)) {
                                    return rv;
                                }
                            } else
#endif
                            {
                                if (SOC_IS_KATANA(unit)) {
                                    BCM_IF_ERROR_RETURN(
                                        _bcm_kt_cosq_index_resolve(
                                          unit, param0, 0,
                                          _BCM_KT_COSQ_INDEX_STYLE_BUCKET,
                                          NULL, &queue_id, NULL));
                                }
                            }
                        } else {
                            queue_id = param0 & 0xffff;
                        }
                        in_flags = param1;
                    } else {
                        queue_id = param0 & 0xffff;
                        in_flags = param0;
                    }

                    if (in_flags & BCM_FABRIC_QUEUE_DEST_OFFSET) {
                        if (queue_id != -1) {
                            BCM_IF_ERROR_RETURN(
                              _bcm_field_fabricQueue_action_node_delete (unit,
                                  queue_id, param1, &referenceCount));
                            if(referenceCount == 0) {
                               BCM_IF_ERROR_RETURN(BCM_XGS3_MEM_WRITE(unit,
                                            ING_QUEUE_MAPm,
                                            queue_id, &ing_queue_entry));
                            }
                            fa->flags = fa->flags | _FP_ACTION_DIRTY;
                        }
                    }
                }
            }
        }
    }   
#endif
    f_ent->flags |= _FP_ENTRY_DIRTY; /* Mark entry as not installed */

    return (BCM_E_NONE);
}
#ifdef INCLUDE_L3
/*
 * Function:
 *     _bcm_field_policy_set_l3_info
 * Purpose:
 *     Install l3 forwarding policy entry.  
 * Parameters:
 *     unit      - (IN) BCM device number
 *     mem       - (IN) Policy table memory. 
 *     value     - (IN) Egress object id or combined next hop information.
 *     buf       - (IN/OUT) Hw entry buffer to write.
 * Returns:
 *     BCM_E_XXX
 */

int
_bcm_field_policy_set_l3_info(int unit, soc_mem_t mem, int value, uint32 *buf)
{
    uint32 flags;         /* L3 forwarding flags           */ 
    int nh_ecmp_id;       /* Next hop/Ecmp group id.       */
    int max_ecmp_paths;   /* Maximum number of ecmp paths. */
    int retval;           /* Operation return value.       */ 
    
    /* Resove next hop /ecmp group id. */
    retval = _bcm_field_policy_set_l3_nh_resolve(unit,  value,
                                                 &flags, &nh_ecmp_id);
    BCM_IF_ERROR_RETURN(retval);

    if (flags & BCM_L3_MULTIPATH) {              
        PolicySet(unit, mem, buf, ECMPf, 1);
        PolicySet(unit, mem, buf, ECMP_PTRf, nh_ecmp_id);
        BCM_IF_ERROR_RETURN(bcm_xgs3_max_ecmp_get(unit, &max_ecmp_paths));
        PolicySet(unit, mem, buf, ECMP_COUNTf, max_ecmp_paths - 1);
    } else {
        PolicySet(unit, mem, buf, ECMPf, 0);
        PolicySet(unit, mem, buf, NEXT_HOP_INDEXf, nh_ecmp_id);
    }
    return (BCM_E_NONE);
}
#endif /* INCLUDE_L3 */
/*
 * Function:
 *     _field_fb_action_copy_to_cpu
 * Purpose:
 *     Install copy to cpu action in policy table.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     mem      - (IN) Policy table memory
 *     f_ent    - (IN) Field entry structure to get policy info from
 *     fa       - (IN  Field action 
 *     buf      - (OUT) Field Policy table entry
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_action_copy_to_cpu(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                             _field_action_t *fa, uint32 *buf)
{
    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    if ((fa->param[0] != 0) && (fa->param[1] >= (1 << 8))) {
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) Error: param1=%d out of range for CopyToCpu.\n"),
                   unit, fa->param[1]));
        return (BCM_E_PARAM);
    }

    switch (fa->action) {
      case bcmFieldActionCopyToCpu:
#if defined(BCM_FIREBOLT2_SUPPORT)
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicySet(unit, mem, buf, COPY_TO_CPUf, 0x1);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 0x1);
              PolicySet(unit, mem, buf, YP_COPY_TO_CPUf, 0x1);
              PolicySet(unit, mem, buf, COPY_TO_CPUf, 0x1);
          }
          break;
      case bcmFieldActionRpCopyToCpu:
          PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 0x1);
          break;
      case bcmFieldActionYpCopyToCpu:
          PolicySet(unit, mem, buf, YP_COPY_TO_CPUf, 0x1);
          break;
      case bcmFieldActionGpCopyToCpu:
          PolicySet(unit, mem, buf, COPY_TO_CPUf, 0x1);
          break;

      default:
          return (BCM_E_INTERNAL);
    }

    if (fa->param[0] != 0) {
        if (SOC_MEM_FIELD_VALID(unit, mem, MATCHED_RULEf)) {
            PolicySet(unit, mem, buf, MATCHED_RULEf, fa->param[1]);
        }
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_fb_action_get
 * Purpose:
 *     Install an action into the hardware tables.
 * Parameters:
 *     unit     - BCM device number
 *     mem      - Policy table memory.
 *     tcam_idx - index into TCAM
 *     fa       - field action 
 *     buf      - (OUT) Field TCAM plus Policy table entry
 * Returns:
 *     BCM_E_NONE
 *     BCM_E_PARAM - Action parameter out-of-range or unrecognized action.
 * Notes:
 *     This is a simple read/modify/write pattern.
 *     FP unit lock should be held by calling function.
 */
STATIC int
_field_fb_action_get(int unit, soc_mem_t mem, _field_entry_t *f_ent, 
                     int tcam_idx, _field_action_t *fa, uint32 *buf)
{
#if defined (BCM_FIREBOLT2_SUPPORT)
    uint32                          mode;
#endif /* BCM_FIREBOLT2_SUPPORT */
    uint32                          redir_field = 0;

    if (NULL == f_ent || NULL == fa || NULL == buf) {
        return (BCM_E_PARAM);
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: BEGIN _field_fb_action_get(eid=%d, tcam_idx=0x%x, "),
               unit, f_ent->eid, tcam_idx));
#ifdef BROADCOM_DEBUG
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "action={%s,%d,%d})\n"),
               _field_fb_action_name(fa->action),
               fa->param[0], fa->param[1]));
#endif

    switch (fa->action) {
    case bcmFieldActionCosQNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 1);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionCosQCpuNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 2);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionPrioPktAndIntCopy:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 4);
        break;
    case bcmFieldActionPrioPktAndIntNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 5);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionPrioPktAndIntTos:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 6);
        break;
    case bcmFieldActionPrioPktAndIntCancel:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 7);
        break;
    case bcmFieldActionPrioPktCopy:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 8);
        break;
    case bcmFieldActionPrioPktNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 9);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionPrioPktTos:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 10);
        break;
    case bcmFieldActionPrioPktCancel:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 11);
        break;
    case bcmFieldActionPrioIntCopy:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 12);
        break;
    case bcmFieldActionPrioIntNew:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 13);
        PolicySet(unit, mem, buf, NEWPRIf, fa->param[0]);
        break;
    case bcmFieldActionPrioIntTos:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 14);
        break;
    case bcmFieldActionPrioIntCancel:
        PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 15);
        break;
    case bcmFieldActionTosNew:
        PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 1);
        PolicySet(unit, mem, buf, NEWDSCP_TOSf, fa->param[0]);
        break;
    case bcmFieldActionTosCopy:
        PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 2);
        break;
    case bcmFieldActionDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
                        PolicySet(unit, mem, buf, RP_CHANGE_DSCPf, 1);
                        PolicySet(unit, mem, buf, YP_CHANGE_DSCPf, 1);
                        PolicySet(unit, mem, buf, GP_CHANGE_DSCPf, 1);
            PolicySet(unit, mem, buf, RP_NEW_DSCPf, fa->param[0]);
            PolicySet(unit, mem, buf, YP_NEW_DSCPf, fa->param[0]);
            PolicySet(unit, mem, buf, GP_NEW_DSCPf, fa->param[0]);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 3);
            PolicySet(unit, mem, buf, NEWDSCP_TOSf, fa->param[0]);
            PolicySet(unit, mem, buf, RP_CHANGE_DSCPf, 1);
            PolicySet(unit, mem, buf, RP_DSCPf, fa->param[0]);
            PolicySet(unit, mem, buf, YP_CHANGE_DSCPf, 1);
            PolicySet(unit, mem, buf, YP_DSCPf, fa->param[0]);
        }
        break;
    case bcmFieldActionTosCancel:
        PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 4);
        break;
    case bcmFieldActionDscpCancel:
        PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 4);
        break;
    case bcmFieldActionCopyToCpu:
    case bcmFieldActionRpCopyToCpu:
    case bcmFieldActionYpCopyToCpu:
    case bcmFieldActionGpCopyToCpu:
        BCM_IF_ERROR_RETURN
            (_field_fb_action_copy_to_cpu(unit, mem, f_ent, fa, buf));
        break;
    case bcmFieldActionGpCopyToCpuCancel:
        PolicySet(unit, mem, buf, COPY_TO_CPUf, 2);
        break;
    case bcmFieldActionCopyToCpuCancel:
        PolicySet(unit, mem, buf, COPY_TO_CPUf, 2);
        PolicySet(unit, mem, buf, YP_COPY_TO_CPUf, 2);
        PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 2);
        break;
    case bcmFieldActionSwitchToCpuCancel:
        PolicySet(unit, mem, buf, COPY_TO_CPUf, 3);
        break;
    case bcmFieldActionRedirect:    /* param0=modid, param1=port/tgid */
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 1);
        PolicySet(unit, mem, buf, REDIRECTIONf, 
                  (fa->param[0] << 6) | fa->param[1]);
        break;
    case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
        if (soc_property_get(unit, spn_TRUNK_EXTEND, 1)) {
            redir_field = ((fa->param[0] & 0x60) << 1) | (fa->param[0] & 0x1f);
        } else {
            redir_field = fa->param[0] & 0x1f;
        }
        redir_field |= 0x20; 
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 1);
        PolicySet(unit, mem, buf, REDIRECTIONf, redir_field);
        break;
    case bcmFieldActionRedirectCancel:
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 2);
        break;
    case bcmFieldActionRedirectPbmp:
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 3);
        PolicySet(unit, mem, buf, REDIRECTIONf, fa->param[0]);
        break;
    case bcmFieldActionEgressMask:
        PolicySet(unit, mem, buf, PACKET_REDIRECTIONf, 4);
        PolicySet(unit, mem, buf, REDIRECTIONf, fa->param[0]);
        break;
    case bcmFieldActionDrop:
#if defined (BCM_FIREBOLT2_SUPPORT)
                if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
                        PolicySet(unit, mem, buf, RP_DROPf, 1);
                        PolicySet(unit, mem, buf, YP_DROPf, 1);
                        PolicySet(unit, mem, buf, GP_DROPf, 1);
                } else if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
                        PolicySet(unit, mem, buf, DROPf, 1);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
                {
                        PolicySet(unit, mem, buf, RP_DROPf, 1);
                        PolicySet(unit, mem, buf, YP_DROPf, 1);
                        PolicySet(unit, mem, buf, DROPf, 1);
                }
                break;
        case bcmFieldActionDropCancel:
#if defined (BCM_FIREBOLT2_SUPPORT)
                if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
                        PolicySet(unit, mem, buf, RP_DROPf, 2);
                        PolicySet(unit, mem, buf, YP_DROPf, 2);
                        PolicySet(unit, mem, buf, GP_DROPf, 2);
                } else if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
                        PolicySet(unit, mem, buf, DROPf, 2);
                } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
                {
                        PolicySet(unit, mem, buf, RP_DROPf, 2);
                        PolicySet(unit, mem, buf, YP_DROPf, 2);
                        PolicySet(unit, mem, buf, DROPf, 2);
                }
                break;
    case bcmFieldActionMirrorOverride:
        if (soc_feature(unit, soc_feature_field_mirror_ovr)) {
            PolicySet(unit, mem, buf, MIRROR_OVERRIDEf, 1);
        } else {
            return BCM_E_PARAM;
        }
        break;
    case bcmFieldActionMirrorIngress:    /* param0=modid, param1=port/tgid */
        PolicySet(unit, mem, buf, IM_MTP_INDEXf, fa->hw_index);
        PolicySet(unit, mem, buf, MIRRORf, 
                  PolicyGet(unit, mem, buf, MIRRORf) | 0x1);
        if (soc_feature(unit, soc_feature_field_mirror_pkts_ctl)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                        ENABLE_FP_FOR_MIRROR_PKTSf, 1));
        }
        break;
    case bcmFieldActionMirrorEgress:     /* param0=modid, param1=port/tgid */
        PolicySet(unit, mem, buf, EM_MTP_INDEXf, fa->hw_index);
        PolicySet(unit, mem, buf, MIRRORf, 
                  PolicyGet(unit, mem, buf, MIRRORf) | 2);
        if (soc_feature(unit, soc_feature_field_mirror_pkts_ctl)) {
            BCM_IF_ERROR_RETURN
                (soc_reg_field32_modify(unit, ING_CONFIGr, REG_PORT_ANY,
                                        ENABLE_FP_FOR_MIRROR_PKTSf, 1));
        }
        break;
#ifdef INCLUDE_L3 
    case bcmFieldActionL3ChangeVlan:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 1);
        BCM_IF_ERROR_RETURN
            (_bcm_field_policy_set_l3_info(unit, mem, fa->param[0], buf)); 
        break;
    case bcmFieldActionL3ChangeVlanCancel:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 2);
        break;
    case bcmFieldActionL3ChangeMacDa:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 4);
        BCM_IF_ERROR_RETURN
            (_bcm_field_policy_set_l3_info(unit, mem,  fa->param[0], buf)); 
        break;
    case bcmFieldActionSrcMacNew: 
    case bcmFieldActionDstMacNew:
        PolicySet(unit, mem, buf, ECMPf, 0);
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 0x4);
        PolicySet(unit, mem, buf, NEXT_HOP_INDEXf, fa->hw_index);
        break;
    case bcmFieldActionL3ChangeMacDaCancel:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 5);
        break;
    case bcmFieldActionL3Switch:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 6);
        BCM_IF_ERROR_RETURN
            (_bcm_field_policy_set_l3_info(unit, mem, fa->param[0], buf)); 
        break;
    case bcmFieldActionL3SwitchCancel:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 7);
        break;
#endif /* INCLUDE_L3 */
    case bcmFieldActionAddClassTag:
        PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 3);
        PolicySet(unit, mem, buf, CLASSIFICATION_TAGf, fa->param[0]);
        break;
    case bcmFieldActionEcnNew:
        /* Ecn in tos byte will be forced to a Drop Precedence value. */ 
        PolicySet(unit, mem, buf, ECN_CNGf, 0x1);
        /* coverity[MISSING_BREAK : FALSE] */
        /* passthru */
    case bcmFieldActionDropPrecedence:
        PolicySet(unit, mem, buf, RP_DROP_PRECEDENCEf, fa->param[0]);
        PolicySet(unit, mem, buf, YP_DROP_PRECEDENCEf, fa->param[0]);
        PolicySet(unit, mem, buf, DROP_PRECEDENCEf, fa->param[0]);
        break;
    case bcmFieldActionGpDropPrecedence:
        PolicySet(unit, mem, buf, DROP_PRECEDENCEf, fa->param[0]);
        break;
    case bcmFieldActionRpDrop:
        PolicySet(unit, mem, buf, RP_DROPf, 1);
        break;
    case bcmFieldActionRpDropCancel:
        PolicySet(unit, mem, buf, RP_DROPf, 2);
        break;
    case bcmFieldActionRpDropPrecedence:
        PolicySet(unit, mem, buf, RP_DROP_PRECEDENCEf, fa->param[0]);
        break;
    case bcmFieldActionRpCopyToCpuCancel:
        PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 2);
        break;
    case bcmFieldActionRpDscpNew:
        PolicySet(unit, mem, buf, RP_CHANGE_DSCPf, 1);
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, RP_NEW_DSCPf, fa->param[0]);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, RP_DSCPf, fa->param[0]);
        }
        break;
    case bcmFieldActionYpDrop:
        PolicySet(unit, mem, buf, YP_DROPf, 1);
        break;
    case bcmFieldActionYpDropCancel:
        PolicySet(unit, mem, buf, YP_DROPf, 2);
        break;
    case bcmFieldActionYpDropPrecedence:
        PolicySet(unit, mem, buf, YP_DROP_PRECEDENCEf, fa->param[0]);
        break;
    case bcmFieldActionYpCopyToCpuCancel:
        PolicySet(unit, mem, buf, YP_COPY_TO_CPUf, 2);
        break;
    case bcmFieldActionYpDscpNew:
        PolicySet(unit, mem, buf, YP_CHANGE_DSCPf, 1);
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, YP_NEW_DSCPf, fa->param[0]);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, YP_DSCPf, fa->param[0]);
        }
        break;
    case bcmFieldActionGpDrop:
#if defined (BCM_FIREBOLT2_SUPPORT)
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, GP_DROPf, 1);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, DROPf, 1);
        } 
        break;
    case bcmFieldActionGpDropCancel:
#if defined (BCM_FIREBOLT2_SUPPORT)
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, GP_DROPf, 2);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, DROPf, 2);
        } 
        break;
    case bcmFieldActionGpDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
                        PolicySet(unit, mem, buf, GP_CHANGE_DSCPf, 1);
            PolicySet(unit, mem, buf, GP_NEW_DSCPf, fa->param[0]);
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, CHANGE_DSCP_TOSf, 3);
            PolicySet(unit, mem, buf, NEWDSCP_TOSf, fa->param[0]);
        } 
        break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
    case bcmFieldActionRpOuterVlanPrioNew:
        PolicySet(unit, mem, buf, RP_CHANGE_DOT1Pf, 1);
        PolicySet(unit, mem, buf, RP_NEW_DOT1Pf, fa->param[0]);
        break;
    case bcmFieldActionYpOuterVlanPrioNew:
        PolicySet(unit, mem, buf, YP_CHANGE_DOT1Pf, 1);
        PolicySet(unit, mem, buf, YP_NEW_DOT1Pf, fa->param[0]);
        break;
    case bcmFieldActionGpOuterVlanPrioNew:
        PolicySet(unit, mem, buf, GP_CHANGE_DOT1Pf, 1);
        PolicySet(unit, mem, buf, GP_NEW_DOT1Pf, fa->param[0]);
        break;
    case bcmFieldActionClassSet:
        PolicySet(unit, mem, buf, USE_VFP_CLASS_IDf, 1);
        PolicySet(unit, mem, buf, VFP_CLASS_IDf, fa->param[0]); 
        break;
    case bcmFieldActionVrfSet:
        PolicySet(unit, mem, buf, USE_VFP_VRF_IDf, 1);
        PolicySet(unit, mem, buf, VFP_VRF_IDf, fa->param[0]); 
        break;
    case bcmFieldActionVlanAdd:
        PolicySet(unit, mem, buf, CHANGE_VLANf, 1);
        PolicySet(unit, mem, buf, NEW_VLANf, fa->param[0]);
        break;
    case bcmFieldActionInnerVlanNew:
        PolicySet(unit, mem, buf, PID_REPLACE_INNER_VIDf, 1);
        PolicySet(unit, mem, buf, PID_NEW_INNER_VIDf, fa->param[0]);
        break;
    case bcmFieldActionInnerVlanPrioNew:
        PolicySet(unit, mem, buf, PID_REPLACE_INNER_PRIf, 1);
        PolicySet(unit, mem, buf, PID_NEW_INNER_PRIf, fa->param[0]);
        break;
    case bcmFieldActionOuterVlanPrioNew:
        if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, CHANGE_PRIORITYf, 1);
            PolicySet(unit, mem, buf, NEW_PRIORITYf, fa->param[0]);
        } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) { 
            PolicySet(unit, mem, buf, RP_CHANGE_DOT1Pf, 1);
            PolicySet(unit, mem, buf, RP_NEW_DOT1Pf, fa->param[0]);
            PolicySet(unit, mem, buf, YP_CHANGE_DOT1Pf, 1);
            PolicySet(unit, mem, buf, YP_NEW_DOT1Pf, fa->param[0]);
            PolicySet(unit, mem, buf, GP_CHANGE_DOT1Pf, 1);
            PolicySet(unit, mem, buf, GP_NEW_DOT1Pf, fa->param[0]);
        }
        break;
    case bcmFieldActionOuterTpidNew:
        BCM_IF_ERROR_RETURN(_bcm_field_tpid_hw_encode(unit, fa->param[0], &mode));
        PolicySet(unit, mem, buf, PID_REPLACE_OUTER_TPIDf, 1);
        PolicySet(unit, mem, buf, PID_TPID_INDEXf, mode);
        break;
    case bcmFieldActionRpSwitchToCpuCancel:
        PolicySet(unit, mem, buf, RP_COPY_TO_CPUf, 0x3);
        break;
#endif /* BCM_FIREBOLT2_SUPPORT */
    case bcmFieldActionOuterVlanNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
        if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, CHANGE_VLANf, 2);
            PolicySet(unit, mem, buf, NEW_VLANf, fa->param[0]);
        }
        else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            PolicySet(unit, mem, buf, PID_REPLACE_OUTER_VIDf, 1);
            PolicySet(unit, mem, buf, PID_NEW_OUTER_VIDf, fa->param[0]);
        } else
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            PolicySet(unit, mem, buf, ECMPf, 0);
            PolicySet(unit, mem, buf, L3SW_CHANGE_MACDA_OR_VLANf, 0x1);
            PolicySet(unit, mem, buf, NEXT_HOP_INDEXf, fa->hw_index);
        }
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) Error: Unknown action (%d)\n"),
                   unit, (uint32)fa->action));
        return BCM_E_PARAM;
    }

    fa->flags &= ~_FP_ACTION_DIRTY; /* Mark action as installed. */

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: END _field_fb_action_get()\n"),
               unit));
    return BCM_E_NONE;
}

/*
 * Function:
 *     _field_fb_action_params_check
 * Purpose:
 *     Check field action parameters.
 * Parameters:
 *     unit     - BCM device number
 *     f_ent    - Field entry structure.
 *     fa       - field action 
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb_action_params_check(int unit,_field_entry_t *f_ent, 
                             _field_action_t *fa)
{
    uint32  redir_field = 0; /* Redirection field value.*/
#if defined(INCLUDE_L3)
    uint32 flags;            /* L3 forwarding flags.    */ 
    int nh_ecmp_id;          /* Next hop/Ecmp group id. */
#endif /* INCLUDE_L3 */
    soc_mem_t mem;           /* Policy table memory id. */
    soc_mem_t tcam_mem;      /* Tcam memory id.         */
    int rv;                  /* Operation return value. */ 


    /* Input parameters check. */
    if ((NULL == f_ent) || (fa == NULL)){
        return (BCM_E_PARAM);
    }

    rv = _field_fb_tcam_policy_mem_get(unit, f_ent->group->stage_id, 
                                       &tcam_mem, &mem);
    BCM_IF_ERROR_RETURN(rv);

    switch (fa->action) {
      case bcmFieldActionCosQNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionCosQCpuNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionPrioPktAndIntNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionPrioPktNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionPrioIntNew:
          PolicyCheck(unit, mem, NEWPRIf, fa->param[0]);
          break;
      case bcmFieldActionTosNew:
          PolicyCheck(unit, mem, NEWDSCP_TOSf, fa->param[0]);
          break;
      case bcmFieldActionDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, RP_NEW_DSCPf, fa->param[0]);
              PolicyCheck(unit, mem, YP_NEW_DSCPf, fa->param[0]);
              PolicyCheck(unit, mem, GP_NEW_DSCPf, fa->param[0]);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicyCheck(unit, mem, NEWDSCP_TOSf, fa->param[0]);
              PolicyCheck(unit, mem, RP_DSCPf, fa->param[0]);
              PolicyCheck(unit, mem, YP_DSCPf, fa->param[0]);
          }
          break;
      case bcmFieldActionCopyToCpu:
      case bcmFieldActionRpCopyToCpu:
      case bcmFieldActionYpCopyToCpu:
      case bcmFieldActionGpCopyToCpu:
          if (fa->param[0]) {
              PolicyCheck(unit, mem, MATCHED_RULEf, fa->param[1]);
          }
          break;
      case bcmFieldActionRedirect:    /* param0 = modid, param1 = port/tgid */
          rv = _bcm_field_action_dest_check(unit, fa);
          BCM_IF_ERROR_RETURN(rv);

          if (fa->param[0] > 0x3f || fa->param[1] > 0x3f) {
              return BCM_E_PARAM;
          }
          PolicyCheck(unit, mem, REDIRECTIONf, 
                      (fa->param[0] << 6) | fa->param[1]);
          break;
      case bcmFieldActionRedirectTrunk:    /* param0 = trunk ID */
          if (soc_property_get(unit, spn_TRUNK_EXTEND, 1)) {
              redir_field = ((fa->param[0] & 0x60) << 1) | (fa->param[0] & 0x1f);
          } else {
              redir_field = fa->param[0] & 0x1f;
          }
          redir_field |= 0x20; 
          PolicyCheck(unit, mem, REDIRECTIONf, redir_field);
          break;
      case bcmFieldActionRedirectPbmp:
          PolicyCheck(unit, mem, REDIRECTIONf, fa->param[0]);
          break;
      case bcmFieldActionEgressMask:
          PolicyCheck(unit, mem, REDIRECTIONf, fa->param[0]);
          break;
      case bcmFieldActionMirrorIngress:
      case bcmFieldActionMirrorEgress:
          rv = _bcm_field_action_dest_check(unit, fa);
          BCM_IF_ERROR_RETURN(rv);
          break;
#ifdef INCLUDE_L3 
      case bcmFieldActionL3ChangeVlan:
      case bcmFieldActionL3ChangeMacDa:
      case bcmFieldActionL3Switch:
          rv = _bcm_field_policy_set_l3_nh_resolve(unit,  fa->param[0],
                                                   &flags, &nh_ecmp_id);
          BCM_IF_ERROR_RETURN(rv);
          if (flags & BCM_L3_MULTIPATH) {
              PolicyCheck(unit, mem, ECMP_PTRf, nh_ecmp_id);
          } else {
              PolicyCheck(unit, mem, NEXT_HOP_INDEXf, nh_ecmp_id);
          }
          break;
#endif /* INCLUDE_L3 */
      case bcmFieldActionAddClassTag:
          PolicyCheck(unit, mem, CLASSIFICATION_TAGf, fa->param[0]);
          break;
      case bcmFieldActionDropPrecedence:
          PolicyCheck(unit, mem, RP_DROP_PRECEDENCEf, fa->param[0]);
          PolicyCheck(unit, mem, YP_DROP_PRECEDENCEf, fa->param[0]);
          PolicyCheck(unit, mem, DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionGpDropPrecedence:
          PolicyCheck(unit, mem, DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionRpDropPrecedence:
          PolicyCheck(unit, mem, RP_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionRpDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, RP_NEW_DSCPf, fa->param[0]);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicyCheck(unit, mem, RP_DSCPf, fa->param[0]);
          }
          break;
      case bcmFieldActionYpDropPrecedence:
          PolicyCheck(unit, mem, YP_DROP_PRECEDENCEf, fa->param[0]);
          break;
      case bcmFieldActionYpDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, YP_NEW_DSCPf, fa->param[0]);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicyCheck(unit, mem, YP_DSCPf, fa->param[0]);
          }
          break;
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case bcmFieldActionRpOuterVlanPrioNew:
          PolicyCheck(unit, mem, RP_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionYpOuterVlanPrioNew:
          PolicyCheck(unit, mem, YP_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionGpOuterVlanPrioNew:
          PolicyCheck(unit, mem, GP_NEW_DOT1Pf, fa->param[0]);
          break;
      case bcmFieldActionClassSet:
          PolicyCheck(unit, mem, VFP_CLASS_IDf, fa->param[0]); 
          break;
      case bcmFieldActionVrfSet:
          if (fa->param[0] > SOC_VRF_MAX(unit)) {
              return BCM_E_PARAM;
          }
          PolicyCheck(unit, mem, VFP_VRF_IDf, fa->param[0]);
          break;
      case bcmFieldActionVlanAdd:
          PolicyCheck(unit, mem, NEW_VLANf, fa->param[0]);
          break;
      case bcmFieldActionGpDscpNew:
#if defined(BCM_FIREBOLT2_SUPPORT) 
          if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, GP_NEW_DSCPf, fa->param[0]);
          } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
          {
              PolicyCheck(unit, mem, NEWDSCP_TOSf, fa->param[0]);
          }
          break;
      case bcmFieldActionInnerVlanNew:
          PolicyCheck(unit, mem, PID_NEW_INNER_VIDf, fa->param[0]);
          break;
      case bcmFieldActionInnerVlanPrioNew:
          PolicyCheck(unit, mem, PID_NEW_INNER_PRIf, fa->param[0]);
          break;
      case bcmFieldActionOuterVlanPrioNew:
          if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_PRIORITYf, fa->param[0]);
          } else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) { 
              PolicyCheck(unit, mem, RP_NEW_DOT1Pf, fa->param[0]);
              PolicyCheck(unit, mem, YP_NEW_DOT1Pf, fa->param[0]);
              PolicyCheck(unit, mem, GP_NEW_DOT1Pf, fa->param[0]);
          }
          break;
      case bcmFieldActionOuterVlanNew:
          if (_BCM_FIELD_STAGE_LOOKUP== f_ent->group->stage_id) {
              PolicyCheck(unit, mem, NEW_VLANf, fa->param[0]);
          }
          else if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
              PolicyCheck(unit, mem, PID_NEW_OUTER_VIDf, fa->param[0]);
          }
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default:
          return BCM_E_NONE;
    }
    return BCM_E_NONE;
}

#define BCM_FB_BITS_IN_4_KBYTES   (1 << 15) /* (4 * 1024 * 8) 4K bytes */
/*
 * Function: 
 *     _field_fb_bucket_calc
 * Purpose:
 *     Lookup the proper meter bucket size encoding.
 */

STATIC int 
_field_fb_bucket_calc(int unit, uint32 burst, 
                      uint32 *bucket_size, uint32 *bucket_count)
{
    uint32 b_size;

    /* Input  parameters check. */  
    if ((NULL == bucket_size) || (NULL == bucket_count)) {
        return (BCM_E_PARAM);
    }

    /* If requested rate is 0 -> set size & count to 0 */
    if (0 == burst) {
        *bucket_size = *bucket_count = 0;
        return (BCM_E_NONE);
    }

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    if (SOC_IS_FIREBOLT2(unit) || SOC_IS_SC_CQ(unit))
    {
        int linear_mode = TRUE;
        uint32 max_bucket_size;

        /* Bucket size is in 4 KBytes granularity */
        b_size = _bcm_fb2_kbits_to_bucket_encoding(burst, linear_mode);

        /* Verify that value fits into hw field. */
        max_bucket_size =  
            (1 << soc_mem_field_length(unit, FP_METER_TABLEm, BUCKETSIZEf)) - 1;
        if (b_size > max_bucket_size) {
            return (BCM_E_PARAM);
        }

        *bucket_size = b_size;
        /* Bucket count is in 1/2 bit granularity and bucket size is
         * in 4 KByte granularity.
         */ 
        *bucket_count = 2 * b_size * BCM_FB_BITS_IN_4_KBYTES;
    } else
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    {
        uint8               b_code;
       
        /* Bucket size is encoded as follow 
         *            Bucket Size
         * 4'd0     : 4 K - 1
         * 4'd1     : 8 K - 1
         * 4'd2     : 16 K - 1
         * 4'd3     : 32 K - 1
         * 4'd4     : 64 K - 1
         * 4'd5     : 128 K - 1
         * 4'd6     : 256 K - 1
         * 4'd7     : 512 K - 1
         * 4'd8     : 1 M - 1
         * 4'd9     : 2 M - 1
         * 4'd10    : 4 M - 1
         * 4'd11    : 8 M - 1
         * 4'd12    : 16 M - 1
         */
#define BCM_FB_BUCKET_SIZE_MAX (12)
        b_size = BCM_FB_BITS_IN_4_KBYTES;
        burst = burst * 1000; /* Convert KBits to bits */
        for (b_code = 0; b_code <= BCM_FB_BUCKET_SIZE_MAX; b_code++) {
            if (burst < b_size) {
                break;
            }
            b_size *= 2;
        }
        *bucket_size  = b_code;
        if (*bucket_size >  BCM_FB_BUCKET_SIZE_MAX) {
            return (BCM_E_PARAM);
        }
#undef BCM_FB_BUCKET_SIZE_MAX 

        /* Bucket count is in 1/2 of a bit granularity */
        *bucket_count = 2 * b_size;
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: _field_fb_bucket_calc  rate=%d, bucket_count=%d, "
                           "bucket_size=%d\n"), unit, burst, *bucket_count, *bucket_size));
    return (BCM_E_NONE);
}
#undef BCM_FB_BITS_IN_4_KBYTES

int
_field_fb_policer_mem_get(int unit, _field_entry_t *f_ent,
                          soc_mem_t *mem)
{
    _field_group_t  *fg;              /* Field group structure. */
    soc_mem_t meter_table = INVALIDm; /* Hw meter table.        */

    /* Input parameters check. */
    if ((NULL == f_ent) || (NULL == mem)) {
        return (BCM_E_PARAM);
    }

    fg = f_ent->group;
    if (NULL == fg) {
        return (BCM_E_INTERNAL);
    }

    if (_BCM_FIELD_STAGE_INGRESS == fg->stage_id) {
#ifdef BCM_HB_GW_SUPPORT
        if (SOC_IS_HB_GW(unit)) {
            meter_table = FP_METER_TABLE_Xm; 
        } else
#endif /* BCM_BRADLEY_SUPPORT */
        { 
            meter_table = FP_METER_TABLEm;
        }
#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRX_SUPPORT)
    } if (soc_feature(unit, soc_feature_field_multi_stage)) {
        if (_BCM_FIELD_STAGE_EGRESS == fg->stage_id) {
            meter_table = EFP_METER_TABLEm;
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRX_SUPPORT */
    }
    if (INVALIDm == meter_table) {
        return (BCM_E_INTERNAL);
    } 
    *mem = meter_table;
    return (BCM_E_NONE);
}

STATIC int
_field_fb_policer_hw_update(int unit, _field_entry_t  *f_ent,
                            _field_policer_t *f_pl, uint8 index_mtr,
                            uint32 bucket_size, uint32 bucket_count, 
                            uint32 refresh_count)
{
    _field_stage_t    *stage_fc;    /* Stage field control.       */
    int               meter_offset; /* HW buffer for meter entry. */
    uint32            meter_entry[SOC_MAX_MEM_FIELD_WORDS];
    int               mem_field_width;
    soc_mem_t         mem;          /* Meter table memory.        */
    int               meter_hw_idx; /* Meter memory entry index.  */

    if ((NULL == f_pl) || (NULL == f_ent)) {
        return (BCM_E_PARAM);
    }

    /* Get meter table memory. */
    BCM_IF_ERROR_RETURN(_field_fb_policer_mem_get(unit, f_ent, &mem));
    
    /* Get stage control structure. */
    BCM_IF_ERROR_RETURN
        (_field_stage_control_get(unit, f_ent->group->stage_id, &stage_fc));

    mem_field_width = soc_mem_field_length(unit, mem, REFRESHCOUNTf); 
    if (refresh_count > (uint32)((1 << mem_field_width) - 1)) {
        return (BCM_E_PARAM);
    }
 
    meter_offset = (BCM_FIELD_METER_PEAK == index_mtr) ? 0 : 1;

    if (stage_fc->flags & _FP_STAGE_GLOBAL_METER_POOLS) {
        meter_hw_idx = (2 * f_pl->pool_index *
            stage_fc->meter_pool[_FP_DEF_INST][f_pl->pool_index]->pool_size) +
                        (2 * f_pl->hw_index) + meter_offset;
    } else {
        meter_hw_idx = 
            stage_fc->slices[_FP_DEF_INST][f_pl->pool_index].start_tcam_idx + \
            (2 * f_pl->hw_index) + meter_offset;
    }

    if (meter_hw_idx < soc_mem_index_min(unit, mem) ||
        meter_hw_idx > soc_mem_index_max(unit, mem)) {
        return (BCM_E_INTERNAL);
    }

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY,
                      meter_hw_idx, &meter_entry));
 
    soc_mem_field32_set(unit, mem, &meter_entry, REFRESHCOUNTf,
                        refresh_count);
    soc_mem_field32_set(unit, mem, &meter_entry, BUCKETSIZEf,
                        bucket_size);
    soc_mem_field32_set(unit, mem, &meter_entry, BUCKETCOUNTf,
                        bucket_count);

    /* Refresh mode is only set to 1 for Single Rate. Other modes get 0 */
     
    if (f_pl->cfg.mode  == bcmPolicerModeSrTcm) {
        soc_mem_field32_set(unit, mem, &meter_entry, REFRESH_MODEf, 1);
    } else {
        soc_mem_field32_set(unit, mem, &meter_entry, REFRESH_MODEf, 0);
    }

#if defined(BCM_TRX_SUPPORT) 
    if (((stage_fc->stage_id == _BCM_FIELD_STAGE_INGRESS) ||
        (stage_fc->stage_id == _BCM_FIELD_STAGE_EXTERNAL)) &&
        (SOC_MEM_FIELD_VALID(unit, mem, METER_GRANf))) {
        soc_mem_field32_set(unit, mem, &meter_entry, METER_GRANf, 3);
    }
#endif /* BCM_TRX_SUPPORT) */

    BCM_IF_ERROR_RETURN
        (soc_mem_write(unit, mem, MEM_BLOCK_ALL,
                       meter_hw_idx, meter_entry));

#ifdef BCM_HB_GW_SUPPORT
    if (SOC_IS_HB_GW(unit)) {
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, FP_METER_TABLE_Ym, MEM_BLOCK_ALL,
                           meter_hw_idx, meter_entry));
    }
#endif /* BCM_BRADLEY_SUPPORT */ 

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_policer_install
 * Purpose:
 *     Install a policer pair into the hardware tables.
 * Parameters:
 *     unit   - (IN) BCM device number
 *     f_ent  - (IN) Field entry structure.
 *     f_pl   - (IN) Field policer descriptor.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_policer_install(int unit, _field_entry_t *f_ent, 
                              _field_policer_t *f_pl)
{
    uint32                       bucket_size = 0;
    uint32                       refresh_count = 0;
    uint32                       bucket_count = 0;

    if ((NULL == f_ent) || (NULL == f_pl)) {
        return (BCM_E_PARAM);
    }

    if (NULL == f_ent->group || NULL == f_ent->fs) {
        return (BCM_E_INTERNAL);
    }

    if (0 == (f_pl->hw_flags & _FP_POLICER_DIRTY)) {
        return (BCM_E_NONE);
    }

    /* lookup bucket size from tables */
    if (f_pl->hw_flags & _FP_POLICER_COMMITTED_DIRTY) {
        BCM_IF_ERROR_RETURN
            (_field_fb_bucket_calc(unit, f_pl->cfg.ckbits_burst,
                                   &bucket_size, &bucket_count));

        /* A value of one in the REFRESHCOUNT is 64 kbits/sec */
        refresh_count = f_pl->cfg.ckbits_sec / _FP_POLICER_REFRESH_RATE;

        BCM_IF_ERROR_RETURN
            (_field_fb_policer_hw_update(unit, f_ent, f_pl,   
                                         BCM_FIELD_METER_COMMITTED,
                                         bucket_size, bucket_count,
                                         refresh_count));

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: commited-> bucket_size=%u, bucket_count=%u, "
                               "refresh_count=%u\n"), unit, bucket_size, bucket_count, 
                   refresh_count)); 
        f_pl->hw_flags &= ~_FP_POLICER_COMMITTED_DIRTY;
    }

    if (f_pl->hw_flags & _FP_POLICER_PEAK_DIRTY) {
        if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
            /*
             * Rates are always set in committed variables, for flow meters.
             */
            f_pl->cfg.pkbits_sec = f_pl->cfg.ckbits_sec;
            f_pl->cfg.pkbits_burst = f_pl->cfg.ckbits_burst;
        }

        BCM_IF_ERROR_RETURN
            (_field_fb_bucket_calc(unit, f_pl->cfg.pkbits_burst, 
                                   &bucket_size, &bucket_count));

        refresh_count  = f_pl->cfg.pkbits_sec / _FP_POLICER_REFRESH_RATE;

        BCM_IF_ERROR_RETURN
            (_field_fb_policer_hw_update(unit, f_ent, f_pl,
                                         BCM_FIELD_METER_PEAK,
                                         bucket_size, bucket_count,
                                         refresh_count));

        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "FP(unit %d) vverb: commited-> bucket_size=%u, bucket_count=%u, "
                               "refresh_count=%u\n"), unit, bucket_size, bucket_count, 
                   refresh_count)); 

        f_pl->hw_flags &= ~_FP_POLICER_PEAK_DIRTY;

        if (_FP_POLICER_EXCESS_HW_METER(f_pl)) {
            /*
             * Reset rates in peak meter variables.
             */
            f_pl->cfg.pkbits_sec = 0;
            f_pl->cfg.pkbits_burst = 0;
        }
    }
    return BCM_E_NONE;
}
#undef _FP_POLICER_REFRESH_RATE

#if defined(BCM_FIREBOLT2_SUPPORT) 
/*
 * Function:
 *     _field_fb2_key_match_type_set
 *
 * Purpose:
 *     Set key match type based on entry group.
 *     NOTE: For double wide entries key type must be the same for 
 *           both parts of the entry.   
 * Parameters:
 *     unit   - (IN) BCM device number
 *     f_ent  - (IN) Slice number to enable
 * Returns:
 *     BCM_E_XXX
 */
STATIC int
_field_fb2_key_match_type_set(int unit, _field_entry_t *f_ent)
{
    _field_group_t           *fg;   /* Field group entry belongs to. */
    uint32                   data;  /* Key match type.               */         
    uint32                   mask;  /* Key match type mask.          */         
                                    /* Key match type offset.        */
    _bcm_field_qual_offset_t q_offset; 
    {
       sal_memset(&q_offset, 0, sizeof(q_offset));
       q_offset.field = KEYf;
       q_offset.num_offsets = 1;
       q_offset.offset[0] = 181;
       q_offset.width[0] = 3;    
    }

    /* Input parameters check. */ 
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    } 
    if (NULL == (fg = f_ent->group)) {
        return (BCM_E_PARAM);
    }
 
    if (fg->flags & _FP_GROUP_SPAN_SINGLE_SLICE) {
        switch (fg->sel_codes[0].fpf3) {
          case _BCM_FIELD_EFP_KEY1:
              data = _BCM_FIELD_EFP_KEY1_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY2:
              data = _BCM_FIELD_EFP_KEY2_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY4:
              data = _BCM_FIELD_EFP_KEY4_MATCH_TYPE;
              break;
          default:
              return (BCM_E_INTERNAL);
        } 
    } else {
        switch (fg->sel_codes[1].fpf3) {
          case _BCM_FIELD_EFP_KEY2:
              data = _BCM_FIELD_EFP_KEY2_KEY3_MATCH_TYPE;
              break;
          case _BCM_FIELD_EFP_KEY4:
              data = _BCM_FIELD_EFP_KEY1_KEY4_MATCH_TYPE;
              break;
          default:
              return (BCM_E_INTERNAL);
        }
    }
   mask = 0x7;
   /*
    * COVERITY 
    * 
    *This flow takes care of the  Out-of-bounds access issue as 
    * the offset that is being passed is defined.
    */ 
    /* coverity[callee_ptr_arith] */
   return _bcm_field_qual_value_set(unit, &q_offset, f_ent, &data, &mask);
}
#endif /* BCM_FIREBOLT2_SUPPORT */

/*
 * Function:
 *     _bcm_field_fb_write_slice_map
 *
 * Purpose:
 *     Write the FP_SLICE_MAP
 *
 * Parameters:
 *     unit
 *     stage_fc - pointer to stage control block
 *     fg       - Reference to Field Group Structure.
 *
 * Returns:
 *     nothing
 *     
 * Notes:
 */
int
_bcm_field_fb_write_slice_map(int unit, _field_stage_t *stage_fc,
                              _field_group_t *fg)
{
    int rv = BCM_E_NONE;
#if defined(BCM_RAPTOR_SUPPORT) || defined(BCM_FIREBOLT2_SUPPORT)
    int idx;
    fp_slice_map_entry_t map_entry;
    soc_field_t field;
    int vmap_size;
    uint32 value;

    /* Calculate virtual map size. */
    rv = _bcm_field_virtual_map_size_get(unit, stage_fc, &vmap_size); 
    BCM_IF_ERROR_RETURN(rv);

    rv = READ_FP_SLICE_MAPm(unit, MEM_BLOCK_ANY, 0, &map_entry);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < vmap_size; idx++) {
        value = (stage_fc->vmap[_FP_DEF_INST][0][idx]).vmap_key;
        field = virtual_to_physical_map[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);

        value = (stage_fc->vmap[_FP_DEF_INST][0][idx]).virtual_group;
        field = virtual_to_group_map[idx];
        soc_FP_SLICE_MAPm_field32_set(unit, &map_entry, field, value);
    }

    rv = WRITE_FP_SLICE_MAPm(unit, MEM_BLOCK_ALL, 0, &map_entry);
#endif /* BCM_RAPTOR_SUPPORT || BCM_FIREBOLT2_SUPPORT */
    return (rv);
}

/*
 * Function:
 *     _field_fb_qualify_ip_type
 * Purpose:
 *     Install ip type qualifier into TCAM
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry index
 *     type  - (IN) Ip Type. 
 *     qual  - (IN) Qualifier(IpType)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
#define BCM_FIELD_IPTYPE_BAD 0xff
STATIC int
_field_fb_qualify_ip_type(int unit, bcm_field_entry_t entry, 
                              bcm_field_IpType_t type, 
                              bcm_field_qualify_t qual) 
{
    _field_group_t      *fg;
    _field_entry_t      *f_ent;
    uint32              data = BCM_FIELD_IPTYPE_BAD,
                        mask = BCM_FIELD_IPTYPE_BAD;

    BCM_IF_ERROR_RETURN
         (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    fg = f_ent->group;
    if (NULL == fg) {
        return (BCM_E_INTERNAL);
    }

    /*
     * Devices with a separate PacketFormat and IpType use different hardware
     * encodings for IpType. 
     */
    if (soc_feature(unit, soc_feature_field_qual_IpType)) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            switch (type) {
              case bcmFieldIpTypeAny:
                  data = 0x0;
                  mask = 0x0;
                  break;
              case bcmFieldIpTypeNonIp:
                  data = 0x0;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIpv4Not:
                  data = 0x0;
                  mask = 0x1;
                  break;
              case bcmFieldIpTypeIpv4NoOpts:
                  data = 0x1;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIpv4WithOpts:
                  data = 0x3;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIpv4Any:
                  data = 0x1;
                  mask = 0x1;
                  break;
              case bcmFieldIpTypeIpv6:
                  data = 0x2;
                  mask = 0x3;
                  break;
              default:
                  break;
            }
        }
#ifdef BCM_FIREBOLT2_SUPPORT 
        else if (fg->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
            switch (type) {
              case bcmFieldIpTypeNonIp:
                  data = 0x2;
                  mask = 0x2;
                  break; 
              case bcmFieldIpTypeIpv4Any:
                  data = 0x0;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIpv6:
                  data = 0x1;
                  mask = 0x3;
                  break;
              case bcmFieldIpTypeIp:
                  data = 0x0;
                  mask = 0x2;
                  break;
              case bcmFieldIpTypeArp:
                  data = 0x2;
                  mask = 0x3;
                  break; 
              default:
                  break;
            }
        } else if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
            switch (type) {
              case bcmFieldIpTypeIpv4WithOpts:
                  data = 0x1;
                  mask = 0x1;
                  break; 
              case bcmFieldIpTypeIpv4NoOpts:
                  data = 0x0;
                  mask = 0x1;
                  break;
              case bcmFieldIpTypeIpv4Any:
                  if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
                      return (BCM_E_NONE);
                  } 
                  return (BCM_E_UNAVAIL);
              case bcmFieldIpTypeIpv6:
                  if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
                      return (BCM_E_NONE);
                  } 
                  return (BCM_E_UNAVAIL);
              default:
                  return (BCM_E_UNAVAIL);
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */ 
    } else { /* Devices without separate IpType field. */
        switch (type) {
          case bcmFieldIpTypeAny:
              data = 0x0;
              mask = 0x0;
              break;
          case bcmFieldIpTypeNonIp:
              data = 0x0;
              mask = 0x3;
              break;
          case bcmFieldIpTypeIpv4Not:
              data = 0x0;
              mask = 0x1;
              break;
          case bcmFieldIpTypeIpv4Any:
              data = 0x1;
              mask = 0x3;
              break;
          case bcmFieldIpTypeIpv6:
              data = 0x2;
              mask = 0x3;
              break;
          case bcmFieldIpTypeIpv6Not:
              data = 0x0;
              mask = 0x2;
              break;
          default:
              break;
        }
    }

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "FP(unit %d) vverb: entry=%d qualifying on Iptype, data=%#x, mask=%#x\n"),
               unit, f_ent->eid, data, mask));

    if ((data == BCM_FIELD_IPTYPE_BAD) ||
        (mask == BCM_FIELD_IPTYPE_BAD)) {
        return (BCM_E_UNAVAIL);
    }
    return _field_qualify32(unit, f_ent->eid, qual,
                            data, mask);
}
#undef BCM_FIELD_IPTYPE_BAD

/*
 * Function:
 *     _field_fb_qualify_ip_type_get
 * Purpose:
 *     Read ip type qualifier match criteria from the HW.
 * Parameters:
 *     unit  - (IN) BCM device number
 *     entry - (IN) Field entry index
 *     type  - (OUT) Ip Type. 
 *     qual  - (IN) Qualifier(IpType)
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
STATIC int
_field_fb_qualify_ip_type_get(int unit, bcm_field_entry_t entry, 
                              bcm_field_IpType_t *type,
                              bcm_field_qualify_t qual) 
{
    _field_group_t *fg;      /* Field group structure.      */
    _field_entry_t *f_ent;   /* Field Entry structure.      */
    uint32 hw_data;          /* HW encoded qualifier data.  */
    uint32 hw_mask;          /* HW encoding qualifier mask. */
    int rv;                  /* Operation return status.    */

    /* Input parameters checks. */
    if (NULL == type) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN
         (_field_entry_get(unit, entry, _FP_ENTRY_PRIMARY, &f_ent));

    fg = f_ent->group;
    if (NULL == fg) {
        return (BCM_E_INTERNAL);
    }

    /* Read qualifier match value and mask. */
    rv = _bcm_field_entry_qualifier_uint32_get(unit, f_ent->eid,
                                               qual,
                                               &hw_data, &hw_mask);
    BCM_IF_ERROR_RETURN(rv);

    /* Read IpType encoding for qualifier data and mask */
    rv = _bcm_field_fb_qualify_ip_type_encode_get(unit, fg, hw_data,
                                                  hw_mask, type);

    return rv;
}
/*
 * Function:
 *     _bcm_field_fb_qualify_ip_type_encode_get
 * Purpose:
 *     Get ip type qualifier encode value for given data and mask.
 * Parameters:
 *     unit     - (IN) BCM device number
 *     fg       - (IN) Field group
 *     data     - (IN) qualifier data.
 *     mask     - (IN) qualifier mask.
 *     type     - (OUT) Ip Type.
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */

int
_bcm_field_fb_qualify_ip_type_encode_get(int unit, _field_group_t *fg,
                                         uint32 hw_data, uint32 hw_mask,
                                         bcm_field_IpType_t *type)
{
    /*
     * Devices with a separate PacketFormat and IpType use different hardware
     * encodings for IpType.
     */
    if (soc_feature(unit, soc_feature_field_qual_IpType)) {
        if (fg->stage_id == _BCM_FIELD_STAGE_INGRESS) {
            if ((hw_data == 0) && (hw_mask == 0)) {
                *type = bcmFieldIpTypeAny;
            } else if ((hw_data == 0) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeNonIp;
            } else if ((hw_data == 0) && (hw_mask == 1)) {
                *type = bcmFieldIpTypeIpv4Not;
            } else if ((hw_data == 1) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv4NoOpts;
            } else if ((hw_data == 3) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv4WithOpts;
            } else if ((hw_data == 1) && (hw_mask == 1)) {
                *type = bcmFieldIpTypeIpv4Any;
            } else if ((hw_data == 2) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv6;
            } else {
                return (BCM_E_INTERNAL);
            }
        }
#ifdef BCM_FIREBOLT2_SUPPORT
        else if (fg->stage_id == _BCM_FIELD_STAGE_LOOKUP) {
            if ((hw_data == 2) && (hw_mask == 2)) {
                *type = bcmFieldIpTypeNonIp;
            } else if ((hw_data == 0) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv4Any;
            } else if ((hw_data == 1) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeIpv6;
            } else if ((hw_data == 0) && (hw_mask == 2)) {
                *type = bcmFieldIpTypeIp;
            } else if ((hw_data == 2) && (hw_mask == 3)) {
                *type = bcmFieldIpTypeArp;
            } else {
                return (BCM_E_INTERNAL);
            }
        } else if (fg->stage_id == _BCM_FIELD_STAGE_EGRESS) {
            if ((hw_data == 1) && (hw_mask == 1)) {
                *type = bcmFieldIpTypeIpv4WithOpts;
            } else if ((hw_data == 0) && (hw_mask == 1)) {
                *type = bcmFieldIpTypeIpv4NoOpts;
            } else if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp4)) {
                *type = bcmFieldIpTypeIpv4Any;
            } else if (BCM_FIELD_QSET_TEST(fg->qset, bcmFieldQualifyIp6)) {
                *type = bcmFieldIpTypeIpv6;
            } else {
                return (BCM_E_INTERNAL);
            }
        }
#endif /* BCM_FIREBOLT2_SUPPORT */
    } else { /* Devices without separate IpType field. */
        if ((hw_data == 0) && (hw_mask == 0)) {
            *type = bcmFieldIpTypeAny;
        } else if ((hw_data == 0) && (hw_mask == 3)) {
            *type = bcmFieldIpTypeNonIp;
        } else if ((hw_data == 0) && (hw_mask == 1)) {
            *type = bcmFieldIpTypeIpv4Not;
        } else if ((hw_data == 1) && (hw_mask == 3)) {
            *type = bcmFieldIpTypeIpv4Any;
        } else if ((hw_data == 2) && (hw_mask == 3)) {
            *type = bcmFieldIpTypeIpv6;
        } else if ((hw_data == 0) && (hw_mask == 2)) {
            *type = bcmFieldIpTypeIpv6Not;
        } else {
            return (BCM_E_INTERNAL);
        }
    }
    return (BCM_E_NONE);
 }

/*
 * Function:
 *     _field_fb_action_conflict_check
 *
 * Purpose:
 *     Check if two action conflict (occupy the same field in FP policy table)
 *
 * Parameters:
 *     unit    -(IN)BCM device number
 *     f_ent   -(IN)Field entry structure.  
 *     action -(IN) Action to check(bcmFieldActionXXX)
 *     action1 -(IN) Action to check(bcmFieldActionXXX)
 *
 * Returns:
 *     BCM_E_CONFIG - if actions do conflict
 *     BCM_E_NONE   - if there is no conflict
 */
STATIC int
_field_fb_action_conflict_check(int unit, _field_entry_t *f_ent,
                               bcm_field_action_t action1, 
                               bcm_field_action_t action)
{
    /* Input parameters check */
    if (NULL == f_ent) {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

    /* Two identical actions are forbidden. */
    _FP_ACTIONS_CONFLICT(action1);

#ifdef BCM_FIREBOLT2_SUPPORT 
    if (SOC_IS_FIREBOLT2(unit)) {
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            switch (action1) {
              case bcmFieldActionDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
                  break;
              case bcmFieldActionDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
                  break;
              case bcmFieldActionRpDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
                  break;
              case bcmFieldActionRpDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
                  break;
              case bcmFieldActionYpDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
                  break;
              case bcmFieldActionYpDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
                  break;
              case bcmFieldActionGpDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
                  break;
              case bcmFieldActionGpDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
                  break;
              case bcmFieldActionDscpNew:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
                  break;
              case bcmFieldActionRpDscpNew:
              case bcmFieldActionYpDscpNew:
              case bcmFieldActionGpDscpNew:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
                  break;
              case bcmFieldActionRpOuterVlanPrioNew:
              case bcmFieldActionYpOuterVlanPrioNew:
              case bcmFieldActionGpOuterVlanPrioNew:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanPrioNew);
                  break;
              case bcmFieldActionOuterVlanPrioNew:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionRpOuterVlanPrioNew);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionYpOuterVlanPrioNew);
                  _FP_ACTIONS_CONFLICT(bcmFieldActionGpOuterVlanPrioNew);
                  break;
              case bcmFieldActionInnerVlanPrioNew:
              case bcmFieldActionInnerVlanNew:
              case bcmFieldActionOuterVlanNew:
              case bcmFieldActionOuterTpidNew:
                  break;
              default:
                  break;
            }
            return (BCM_E_NONE);
        }
        if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
            switch (action1) {
              case bcmFieldActionDrop:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
                  break;
              case bcmFieldActionDropCancel:
                  _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
                  break;
              case bcmFieldActionOuterVlanNew:
              case bcmFieldActionVlanAdd:
              case bcmFieldActionOuterVlanPrioNew:
              case bcmFieldActionCopyToCpu:
              case bcmFieldActionVrfSet:
              case bcmFieldActionClassSet:
                  break;
              default:
                  break;
            }
            return (BCM_E_NONE);
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

    switch (action1) {
      case bcmFieldActionCosQNew:
      case bcmFieldActionCosQCpuNew:
      case bcmFieldActionPrioPktAndIntCopy:
      case bcmFieldActionPrioPktAndIntNew:
      case bcmFieldActionPrioPktAndIntTos:
      case bcmFieldActionPrioPktAndIntCancel:
      case bcmFieldActionPrioPktCopy:
      case bcmFieldActionPrioPktNew:
      case bcmFieldActionPrioPktTos:
      case bcmFieldActionPrioPktCancel:
      case bcmFieldActionPrioIntCopy:
      case bcmFieldActionPrioIntNew:
      case bcmFieldActionPrioIntTos:
      case bcmFieldActionPrioIntCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCosQCpuNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktAndIntCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioPktCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntTos);
          _FP_ACTIONS_CONFLICT(bcmFieldActionPrioIntCancel);
          break;
      case bcmFieldActionTosNew:
      case bcmFieldActionTosCopy:
      case bcmFieldActionTosCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionEcnNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          break;
      case bcmFieldActionDscpNew:
      case bcmFieldActionDscpCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          break;
      case bcmFieldActionRpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;
      case bcmFieldActionYpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;
      case bcmFieldActionGpDscpNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDscpCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;
      case bcmFieldActionEcnNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCopy);
          _FP_ACTIONS_CONFLICT(bcmFieldActionTosCancel);
          break;

      case bcmFieldActionCopyToCpu:
      case bcmFieldActionCopyToCpuCancel:
      case bcmFieldActionSwitchToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuCancel);
          break;
      case bcmFieldActionRpCopyToCpu:
      case bcmFieldActionRpCopyToCpuCancel:
      case bcmFieldActionRpSwitchToCpuCancel:  
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpSwitchToCpuCancel);
          break;
      case bcmFieldActionYpCopyToCpu:
      case bcmFieldActionYpCopyToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpCopyToCpuCancel);
          break;
      case bcmFieldActionGpCopyToCpu:
      case bcmFieldActionGpCopyToCpuCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionCopyToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSwitchToCpuCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpu);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpCopyToCpuCancel);
          break;
      case bcmFieldActionDrop:
      case bcmFieldActionDropCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
          break;
      case bcmFieldActionRpDrop:
      case bcmFieldActionRpDropCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropCancel);
          break;
      case bcmFieldActionYpDrop:
      case bcmFieldActionYpDropCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropCancel);
          break;
      case bcmFieldActionGpDrop:
      case bcmFieldActionGpDropCancel:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDrop);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropCancel);
          break;
      case bcmFieldActionRedirect:
      case bcmFieldActionRedirectTrunk:
      case bcmFieldActionRedirectCancel:
      case bcmFieldActionRedirectPbmp:
      case bcmFieldActionEgressMask:
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirect);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectTrunk);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRedirectPbmp);
          _FP_ACTIONS_CONFLICT(bcmFieldActionEgressMask);
          break;
      case bcmFieldActionMirrorIngress:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorIngress);
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
          break;
      case bcmFieldActionMirrorEgress:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorOverride);
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorEgress);
          break;
      case bcmFieldActionMirrorOverride:
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorIngress);
          _FP_ACTIONS_CONFLICT(bcmFieldActionMirrorEgress);
          break;
      case bcmFieldActionL3ChangeVlan:
      case bcmFieldActionL3ChangeVlanCancel:
      case bcmFieldActionL3ChangeMacDa:
      case bcmFieldActionL3ChangeMacDaCancel:
      case bcmFieldActionL3Switch:
      case bcmFieldActionL3SwitchCancel:
      case bcmFieldActionAddClassTag:
          _FP_ACTIONS_CONFLICT(bcmFieldActionAddClassTag);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionSrcMacNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionDstMacNew);
          _FP_ACTIONS_CONFLICT(bcmFieldActionOuterVlanNew);
          break;
      case bcmFieldActionOuterVlanNew:
      case bcmFieldActionSrcMacNew:
      case bcmFieldActionDstMacNew:
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlan);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeVlanCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDa);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3ChangeMacDaCancel);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3Switch);
          _FP_ACTIONS_CONFLICT(bcmFieldActionL3SwitchCancel);
          break;
      case bcmFieldActionDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropPrecedence);
          break;
      case bcmFieldActionRpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionRpDropPrecedence);
          break;
      case bcmFieldActionYpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionYpDropPrecedence);
          break;
      case bcmFieldActionGpDropPrecedence:
          _FP_ACTIONS_CONFLICT(bcmFieldActionDropPrecedence);
          _FP_ACTIONS_CONFLICT(bcmFieldActionGpDropPrecedence);
          break;
      case bcmFieldActionColorIndependent:
          break;
      default:
          break;
    }

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_fb_action_support_check
 *
 * Purpose:
 *     Check if action is supported by device.
 *
 * Parameters:
 *     unit   -(IN)BCM device number
 *     f_ent  -(IN)Field entry structure.  
 *     action -(IN) Action to check(bcmFieldActionXXX)
 *     result -(OUT)
 *               TRUE   - action is supported by device
 *               FALSE  - action is NOT supported by device
 *
 * Returns:
 *     BCM_E_XXX   
 */
STATIC int
_field_fb_action_support_check(int unit, _field_entry_t *f_ent,
                               bcm_field_action_t action, int *result)
{
    /* Input parameters check */
    if ((NULL == f_ent) || (NULL == result)) {
        return (BCM_E_PARAM);
    }
    if (NULL == f_ent->group) {
        return (BCM_E_PARAM);
    }

#ifdef BCM_FIREBOLT2_SUPPORT 
    if (SOC_IS_FIREBOLT2(unit)) {
        if (_BCM_FIELD_STAGE_EGRESS == f_ent->group->stage_id) {
            switch (action) {
              case bcmFieldActionDrop:
              case bcmFieldActionDropCancel:
              case bcmFieldActionDscpNew:
              case bcmFieldActionRpDrop:
              case bcmFieldActionYpDrop:
              case bcmFieldActionGpDrop:
              case bcmFieldActionRpDropCancel:
              case bcmFieldActionYpDropCancel:
              case bcmFieldActionGpDropCancel:
              case bcmFieldActionRpDscpNew:
              case bcmFieldActionYpDscpNew:
              case bcmFieldActionGpDscpNew:
              case bcmFieldActionRpOuterVlanPrioNew:
              case bcmFieldActionYpOuterVlanPrioNew:
              case bcmFieldActionGpOuterVlanPrioNew:
              case bcmFieldActionInnerVlanPrioNew:
              case bcmFieldActionOuterVlanPrioNew:
              case bcmFieldActionInnerVlanNew:
              case bcmFieldActionOuterVlanNew:
              case bcmFieldActionOuterTpidNew:
                  *result = TRUE;
                  break;
              default:
                  *result = FALSE;
            }
            return (BCM_E_NONE);
        }
        if (_BCM_FIELD_STAGE_LOOKUP == f_ent->group->stage_id) {
            switch (action) {
              case bcmFieldActionOuterVlanNew:
              case bcmFieldActionVlanAdd:
              case bcmFieldActionOuterVlanPrioNew:
              case bcmFieldActionDrop:
              case bcmFieldActionDropCancel:
              case bcmFieldActionCopyToCpu:
              case bcmFieldActionVrfSet:
              case bcmFieldActionClassSet:
                  *result = TRUE;
                  break;
              default:
                  *result = FALSE;
            }
            return (BCM_E_NONE);
        }
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

    switch (action) {
        case bcmFieldActionCosQNew:
        case bcmFieldActionCosQCpuNew:
        case bcmFieldActionColorIndependent:
            *result = 
                (soc_feature(unit, soc_feature_field_wide)) ? TRUE : FALSE;
            break;
        case bcmFieldActionMirrorOverride:
            *result = (soc_feature(unit, soc_feature_field_mirror_ovr)) ? \
                                                TRUE : FALSE;
            break;
        case bcmFieldActionSrcMacNew: 
            *result = FALSE;
            break;
        case bcmFieldActionDstMacNew: 
        case bcmFieldActionOuterVlanNew: 
#if defined(INCLUDE_L3) /* INCLUDE_L3 */
            *result = (soc_feature(unit, soc_feature_field_action_l2_change) &&
                       soc_feature(unit, soc_feature_l3)) ? \
                            TRUE : FALSE;
#else 
            *result = FALSE;
#endif /* !INCLUDE_L3 */
            break;
        case bcmFieldActionPrioPktAndIntCopy:
        case bcmFieldActionPrioPktAndIntNew:
        case bcmFieldActionPrioPktAndIntTos:
        case bcmFieldActionPrioPktAndIntCancel:
        case bcmFieldActionPrioPktCopy:
        case bcmFieldActionPrioPktNew:
        case bcmFieldActionPrioPktTos:
        case bcmFieldActionPrioPktCancel:
        case bcmFieldActionPrioIntCopy:
        case bcmFieldActionPrioIntNew:
        case bcmFieldActionPrioIntTos:
        case bcmFieldActionPrioIntCancel:
        case bcmFieldActionTosNew:
        case bcmFieldActionTosCopy:
        case bcmFieldActionTosCancel:
        case bcmFieldActionDscpNew:
        case bcmFieldActionDscpCancel:
        case bcmFieldActionCopyToCpu:
        case bcmFieldActionCopyToCpuCancel:
        case bcmFieldActionSwitchToCpuCancel:
        case bcmFieldActionRedirect:
        case bcmFieldActionRedirectTrunk:
        case bcmFieldActionRedirectCancel:
        case bcmFieldActionRedirectPbmp:
        case bcmFieldActionEgressMask:
        case bcmFieldActionDrop:
        case bcmFieldActionDropCancel:
        case bcmFieldActionMirrorIngress:
        case bcmFieldActionMirrorEgress:
        case bcmFieldActionL3ChangeVlan:
        case bcmFieldActionL3ChangeVlanCancel:
        case bcmFieldActionL3ChangeMacDa:
        case bcmFieldActionL3ChangeMacDaCancel:
        case bcmFieldActionL3Switch:
        case bcmFieldActionL3SwitchCancel:
        case bcmFieldActionAddClassTag:
        case bcmFieldActionDropPrecedence:
        case bcmFieldActionRpDrop:
        case bcmFieldActionRpDropCancel:
        case bcmFieldActionRpDropPrecedence:
        case bcmFieldActionRpCopyToCpu:
        case bcmFieldActionRpCopyToCpuCancel:
        case bcmFieldActionRpDscpNew:
        case bcmFieldActionYpDrop:
        case bcmFieldActionYpDropCancel:
        case bcmFieldActionYpDropPrecedence:
        case bcmFieldActionYpCopyToCpu:
        case bcmFieldActionYpCopyToCpuCancel:
        case bcmFieldActionYpDscpNew:
        case bcmFieldActionGpDrop:
        case bcmFieldActionGpDropCancel:
        case bcmFieldActionGpDropPrecedence:
        case bcmFieldActionGpCopyToCpu:
        case bcmFieldActionGpCopyToCpuCancel:
        case bcmFieldActionGpDscpNew:
            *result = TRUE;
            break;
#if defined(BCM_FIREBOLT2_SUPPORT)
        case bcmFieldActionRpSwitchToCpuCancel:
            *result = (SOC_IS_FIREBOLT2(unit)) ? TRUE: FALSE;
            break;
#endif
        case bcmFieldActionEcnNew:
            *result = (SOC_MEM_FIELD_VALID(unit, FP_POLICY_TABLEm, ECN_CNGf)) ?
                TRUE : FALSE;
            break;
        default:
            *result = FALSE;
    }

    return (BCM_E_NONE);
}

/*
 * Function: 
 *   _bcm_field_fb_stat_hw_mode_to_bmap 
 *
 * Description:
 *      HW counter mode to statistics bitmap. 
 * Parameters:
 *      unit           - (IN) BCM device number.
 *      mode           - (IN) HW counter mode.
 *      stage_id       - (IN) Stage id.
 *      hw_bmap        - (OUT) Statistics bitmap. 
 *      hw_entry_count - (OUT) Number of counters required.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_field_fb_stat_hw_mode_to_bmap(int unit, uint16 mode, 
                                   _field_stage_id_t stage_id, 
                                   uint32 *hw_bmap, uint8 *hw_entry_count)
{
    uint32 value = 0;
    /* Input parameters check. */
    if ((NULL == hw_bmap) || (NULL == hw_entry_count)) {
        return (BCM_E_PARAM);
    }

#if defined (BCM_FIREBOLT2_SUPPORT)
    if (_BCM_FIELD_STAGE_EGRESS == stage_id) {
        if (1 == mode) {
            value |= (1 << (int)bcmFieldStatPackets);
            *hw_entry_count = 1;
        }
        *hw_bmap = value;
        return (BCM_E_NONE);
    }
#endif /* BCM_FIREBOLT2_SUPPORT */

    /*
     *   4 -bit value h/w encoding
     *   The msb indicates if the counter should be incremented by one or be
     *   increased by the packet byte count.
     *
     *   The next 3 bits indicate how and when to update one of the two counters
     *   in the pair.
     *   MODE    UPPER_COUNTER_UPDATE    LOWER_COUNTER_UPDATE
     *   3b000    NO                        NO
     *   3b001    NO                        YES
     *   3b010    YES                       NO
     *   3b011    IF RED                    IF NOT RED
     *   3b100    IF GREEN                  IF NOT GREEN
     *   3b101    IF GREEN                  IF RED
     *   3b110    IF GREEN                  IF YELLOW
     *   3b111    IF RED                    IF YELLOW
     */
    switch (mode) {
      case (0x1):
      case (0x2):
          value |= (1 << (int)bcmFieldStatPackets);
          *hw_entry_count = 1;
          break;
      case (0x9):
      case (0xa):
          value |= (1 << (int)bcmFieldStatBytes);
          *hw_entry_count = 1;
          break;
      case (0x3):
          value |= (1 << (int)bcmFieldStatRedPackets);
          value |= (1 << (int)bcmFieldStatNotRedPackets);
          *hw_entry_count = 2;
          break;
      case (0xb):
          value |= (1 << (int)bcmFieldStatRedBytes);
          value |= (1 << (int)bcmFieldStatNotRedBytes);
          *hw_entry_count = 2;
          break;
      case (0x4):
          value |= (1 << (int)bcmFieldStatGreenPackets);
          value |= (1 << (int)bcmFieldStatNotGreenPackets);
          *hw_entry_count = 2;
          break;
      case (0xc):
          value |= (1 << (int)bcmFieldStatGreenBytes);
          value |= (1 << (int)bcmFieldStatNotGreenBytes);
          *hw_entry_count = 2;
          break;
      case (0x5):
          value |= (1 << (int)bcmFieldStatGreenPackets);
          value |= (1 << (int)bcmFieldStatRedPackets);
          value |= (1 << (int)bcmFieldStatNotYellowPackets);
          *hw_entry_count = 2;
          break;
      case (0xd):
          value |= (1 << (int)bcmFieldStatGreenBytes);
          value |= (1 << (int)bcmFieldStatRedBytes);
          value |= (1 << (int)bcmFieldStatNotYellowBytes);
          *hw_entry_count = 2;
          break;
      case (0x6):
          value |= (1 << (int)bcmFieldStatGreenPackets);
          value |= (1 << (int)bcmFieldStatYellowPackets);
          value |= (1 << (int)bcmFieldStatNotRedPackets);
          *hw_entry_count = 2;
          break;
      case (0xe):
          value |= (1 << (int)bcmFieldStatGreenBytes);
          value |= (1 << (int)bcmFieldStatYellowBytes);
          value |= (1 << (int)bcmFieldStatNotRedBytes);
          *hw_entry_count = 2;
          break;
      case (0x7):
          value |= (1 << (int)bcmFieldStatRedPackets);
          value |= (1 << (int)bcmFieldStatYellowPackets);
          value |= (1 << (int)bcmFieldStatNotGreenPackets);
          *hw_entry_count = 2;
          break;
      case (0xf):
          value |= (1 << (int)bcmFieldStatRedBytes);
          value |= (1 << (int)bcmFieldStatYellowBytes);
          value |= (1 << (int)bcmFieldStatNotGreenBytes);
          *hw_entry_count = 2;
          break;
      default:
          *hw_entry_count = 0;
    }
    *hw_bmap = value;
    return (BCM_E_NONE);
}

#if defined(BCM_BRADLEY_SUPPORT)
/*
 * Function:
 *     _field_br_counter_write
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_br_counter_write(int unit, soc_mem_t mem, int idx, uint32 *buf,
                      _field_counter64_collect_t *cntrs64_buf,
                      uint64 *value)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    /* Set accumulated SW counter. */
    COMPILER_64_ZERO(cntrs64_buf->accumulated_counter);
    COMPILER_64_OR(cntrs64_buf->accumulated_counter, (*value));
    COMPILER_64_SET(cntrs64_buf->last_hw_value, 
                    COMPILER_64_HI(*value) & 0xf, COMPILER_64_LO(*value));

    /* 36 LSB set as a last hw value. */
    hw_val[0] = COMPILER_64_LO(cntrs64_buf->last_hw_value);
    hw_val[1] = COMPILER_64_HI(cntrs64_buf->last_hw_value);
    soc_mem_field_set(unit, mem, buf, COUNTERf, hw_val);
    return soc_mem_write(unit, mem, MEM_BLOCK_ALL, idx, buf);
}


/*
 * Function:
 *     _field_br_counter_set
 * Purpose:
 *     Set accumulated sw & hw counters. 
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_br_counter_set(int unit, _field_stage_t *stage_fc,
                      soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                      soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                      int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter64_collect_t *cntrs64_buf;  /* Sw packet counter value    */
    uint64                     *value;        /* Counter accumulated value. */
    int                        rv;            /* Operation return status.   */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    value = (NULL == packet_count) ? byte_count : packet_count;

    /* Write X pipeline counter value. */
    if (NULL != mem_x_buf) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        rv = _field_br_counter_write(unit, counter_x_mem, idx, mem_x_buf,
                                     cntrs64_buf, value);
        BCM_IF_ERROR_RETURN(rv);
    }

    /* Extract Y pipeline counter value. */
    if (NULL != mem_y_buf) {
        COMPILER_64_ZERO(*value);
        cntrs64_buf = &stage_fc->_field_y64_counters[idx];
        rv = _field_br_counter_write(unit, counter_y_mem, idx, mem_y_buf,
                                     cntrs64_buf, value);
        BCM_IF_ERROR_RETURN(rv);
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *     _field_br_counter_read
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_br_counter_read(int unit, soc_memacc_t *memacc, uint32 *buf,
                      _field_counter64_collect_t *cntrs64_buf,
                      uint64 *count)
{
    uint32  hw_val[2];     /* Parsed field counter value.*/

    /* Byte counter. */
    hw_val[0] = hw_val[1] = 0;
    soc_memacc_field_get(memacc, buf, hw_val);

    _bcm_field_36bit_counter_update(unit, hw_val, cntrs64_buf);
    if (NULL != count) {
        COMPILER_64_ADD_64(*count, cntrs64_buf->accumulated_counter);
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _field_br_counter_get
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
STATIC int
_field_br_counter_get(int unit, _field_stage_t *stage_fc,
                      soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                      soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                      int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter64_collect_t *cntrs64_buf;  /* Sw packet counter value    */
    uint64                     value;         /* Counter accumulated value. */
    soc_memacc_t *memacc_x, *memacc_y;        /* Memory access cache.   */

    /* Input parameters check. */
    if (NULL == stage_fc) {
        return (BCM_E_PARAM);
    }

    sal_memset(&value, 0, sizeof(uint64));

    /* Extract X pipeline counter value. */
    if (NULL != mem_x_buf) {
        cntrs64_buf = &stage_fc->_field_x64_counters[idx];
        memacc_x =
            &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_COUNTER]);
        SOC_IF_ERROR_RETURN
            (_field_br_counter_read(unit, memacc_x, mem_x_buf,
                                    cntrs64_buf, &value));
    }

    /* Extract Y pipeline counter value. */
    if (NULL != mem_y_buf) {
        cntrs64_buf = &stage_fc->_field_y64_counters[idx];
        memacc_y =
            &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_COUNTER_Y]);
        SOC_IF_ERROR_RETURN
            (_field_br_counter_read(unit, memacc_y, mem_y_buf,
                                    cntrs64_buf, &value));
    }

    /* Return accumulated value to the caller routine. */
    if (NULL != packet_count) {
        COMPILER_64_OR(*packet_count, value);
    }
    if (NULL != byte_count) {
        COMPILER_64_OR(*byte_count, value);
    }
    return (BCM_E_NONE);
}
#endif /* BCM_BRADLEY_SUPPORT */


/*
 * Function:
 *     _bcm_field_fb_counter_set
 * Purpose:
 *     Set accumulated sw & hw counters.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb_counter_set(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    uint32                     hw_val[2];     /* Parsed field counter value.*/
    uint64                     *value;        /* Programmed value.          */

    /* Input parameters check. */
    if ((NULL == mem_x_buf) || (INVALIDm == counter_x_mem) || \
        (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    hw_val[0] = hw_val[1] = 0;
    /* Set accumulated sw counter. */
    value = (NULL == packet_count) ? byte_count : packet_count;
    cntrs32_buf = &stage_fc->_field_x32_counters[idx];
    COMPILER_64_ZERO(cntrs32_buf->accumulated_counter);
    COMPILER_64_OR(cntrs32_buf->accumulated_counter, *value);
    COMPILER_64_TO_32_LO(cntrs32_buf->last_hw_value, *value);
    cntrs32_buf->last_hw_value &= 0x1fffffff; /* 29 bits */

    /* Set lsb hw counter. */
    hw_val[0] = cntrs32_buf->last_hw_value;
    hw_val[1] = 0;
    soc_mem_field_set(unit, counter_x_mem, mem_x_buf, COUNTERf, hw_val);
    return soc_mem_write(unit, counter_x_mem, MEM_BLOCK_ALL, idx, mem_x_buf);
}


/*
 * Function:
 *     _bcm_field_fb_counter_get
 * Purpose:
 *     Update accumulated sw counter and return current counter value.
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   stage_fc      - (IN)  Stage field control. 
 *   counter_x_mem - (IN)  HW memory name.
 *   mem_x_buf     - (IN)  Memory table entry.
 *   counter_y_mem - (IN)  HW memory name.
 *   mem_y_buf     - (IN)  Memory table entry.
 *   idx           - (IN)  Counter index.
 *   packet_count  - (OUT) Packet Counter value
 *   byte_count    - (OUT) Byte Counter value
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb_counter_get(int unit, _field_stage_t *stage_fc,
                          soc_mem_t counter_x_mem, uint32 *mem_x_buf,
                          soc_mem_t counter_y_mem, uint32 *mem_y_buf,
                          int idx, uint64 *packet_count, uint64 *byte_count)
{
    _field_counter32_collect_t *cntrs32_buf;  /* Sw packet counter value    */
    soc_memacc_t *memacc;                     /* Memory access cache.   */
    uint32                     hw_val[2];     /* Parsed field counter value.*/

    /* Input parameters check. */
    if ((NULL == mem_x_buf) || (INVALIDm == counter_x_mem) || \
        (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    hw_val[0] = hw_val[1] = 0;
    cntrs32_buf = &stage_fc->_field_x32_counters[idx];
    memacc =
        &(stage_fc->_field_memacc_counters[_FIELD_COUNTER_MEMACC_COUNTER]);
    soc_memacc_field_get(memacc, mem_x_buf, hw_val);
    _bcm_field_32bit_counter_update(unit, hw_val, cntrs32_buf);
    /* Return counter value to caller if (out) pointer was provided. */ 
    if (NULL != byte_count) {
        COMPILER_64_OR(*byte_count, cntrs32_buf->accumulated_counter);
    }
    if (NULL != packet_count) {
        COMPILER_64_OR(*packet_count, cntrs32_buf->accumulated_counter);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_field_fb_counter_get
 * Purpose:
 *      Get hw indexes and flags needed to compose requested statistic.
 *     
 * Parameters:
 *   unit          - (IN)  BCM device number.
 *   f_st          - (IN)  Field statistics entity.
 *   stat          - (IN)  Counter type.
 *   idx1          - (OUT)  Primary counter index.
 *   idx2          - (OUT)  Secondary counter index.
 *   out_flags     - (OUT)  Counter flags.
 * Returns:
 *    BCM_E_XXX
 */
int
_bcm_field_fb_stat_index_get(int unit, _field_stat_t *f_st, 
                             bcm_field_stat_t stat,
                             int *idx1, int *idx2, int *idx3, uint32 *out_flags)
{
    _field_stage_t *stage_fc;        /* Stage field control structure. */
    int            rv;               /* Operation return status.       */
    uint32         flags = 0;        /* _FP_STAT_XXX flags.            */
    uint32         counter_tbl_idx;  /* Counter table index.           */
    int            index1  = _FP_INVALID_INDEX;
    int            index2  = _FP_INVALID_INDEX;

    /* Input parameters check. */
    if ((NULL == idx1) || (NULL == idx2) || (NULL == idx3) ||
        (NULL == out_flags) || (NULL == f_st)) {
        return (BCM_E_PARAM);
    }

    /* Stat entity indexes are adjusted for policy table counter pairs. */
#if defined (BCM_FIREBOLT2_SUPPORT) 
    if (SOC_IS_FIREBOLT2(unit) && (_BCM_FIELD_STAGE_EGRESS == f_st->stage_id)) {
        /* 5651x doesn't have counter pairs on stage egress. */
        counter_tbl_idx = f_st->hw_index;
    } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
    {
        counter_tbl_idx = f_st->hw_index << 1;
    }

    switch ((int)stat) {
      case bcmFieldStatBytes:
          flags |= _FP_STAT_BYTES;
          /* passthru */
          /* coverity[MISSING_BREAK: FALSE] */
      case bcmFieldStatPackets:
          switch (f_st->hw_mode) {
            case (0x2):
            case (0xa):
                index1 = counter_tbl_idx + 1;
                break;
            case (0x1):
            case (0x9):
                index1 = counter_tbl_idx; 
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatGreenBytes:
          switch (f_st->hw_mode) {
            case (0xc):
            case (0xd):
            case (0xe):
                index1 = counter_tbl_idx + 1;   /* Green bytes. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatGreenPackets:
          switch (f_st->hw_mode) {
            case (0x4):
            case (0x5):
            case (0x6):
                index1 = counter_tbl_idx + 1;   /* Green packets. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatYellowBytes:
          switch (f_st->hw_mode) {
            case (0xe):
            case (0xf):
                index1 = counter_tbl_idx;   /* Yellow bytes. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatYellowPackets:
          switch (f_st->hw_mode) {
            case (0x6):
            case (0x7):
                index1 = counter_tbl_idx;   /* Yellow packets. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatRedBytes:
          switch (f_st->hw_mode) {
            case (0xb):
            case (0xf):
                index1 = counter_tbl_idx + 1;     /* Red bytes. */
                break;
            case (0xd):
                index1 = counter_tbl_idx;         /* Red bytes. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatRedPackets:
          switch (f_st->hw_mode) {
            case (0x3):
            case (0x7):
                index1 = counter_tbl_idx + 1;     /* Red packets. */
                break;
            case (0x5):
                index1 = counter_tbl_idx;         /* Red packets. */
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatNotGreenBytes:
          switch (f_st->hw_mode) {
            case (0xc):
                index1 = counter_tbl_idx;   /* Not Green bytes. */
                break;
            case (0xf):
                index1 = counter_tbl_idx + 1;    /* Red bytes.    */
                index2 = counter_tbl_idx;        /* Yellow bytes. */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatNotGreenPackets:
          switch (f_st->hw_mode) {
            case (0x4):
                index1 = counter_tbl_idx;        /* Green packets. */
                break;
            case (0x7):
                index1 = counter_tbl_idx + 1;   /*  Red packets.    */
                index2 = counter_tbl_idx;       /*  Yellow packets. */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatNotYellowBytes:
          switch (f_st->hw_mode) {
            case (0xd):
                index1 = counter_tbl_idx;           /* Green bytes. */
                index2 = counter_tbl_idx + 1;       /* Red bytes.   */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatNotYellowPackets:
          switch (f_st->hw_mode) {
            case (0x5):
                index1 = counter_tbl_idx + 1;      /* Green packets. */
                index2 = counter_tbl_idx;          /* Red packets.   */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      case bcmFieldStatNotRedBytes:
          switch (f_st->hw_mode) {
            case (0xb):
                index1 = counter_tbl_idx;         /* Not red bytes. */
                break;
            case (0xe):
                index1 = counter_tbl_idx + 1;     /* Yellow bytes.  */
                index2 = counter_tbl_idx;         /* Green bytes.   */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          flags |= _FP_STAT_BYTES;
          break;
      case bcmFieldStatNotRedPackets:
          switch (f_st->hw_mode) {
            case (0x3):
                index1 = counter_tbl_idx;         /* Not red packets. */
                break;
            case (0x6):
                index1 = counter_tbl_idx;         /* Yellow packets.  */
                index2 = counter_tbl_idx + 1;     /* Green packets.   */
                flags |= _FP_STAT_ADD;
                break;
            default:
                return (BCM_E_INTERNAL);
          }
          break;
      default:
          return (BCM_E_INTERNAL);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, f_st->stage_id, &stage_fc);
    BCM_IF_ERROR_RETURN(rv);

    /* Calculate  counter table entry index. */
    if (_FP_INVALID_INDEX != index1) {
#if defined (BCM_FIREBOLT2_SUPPORT)
        if (SOC_IS_FIREBOLT2(unit) && (_BCM_FIELD_STAGE_EGRESS == stage_fc->stage_id)) {
            *idx1 = index1;
        } else 
#endif /* BCM_FIREBOLT2_SUPPORT */
        {
            *idx1 =
            stage_fc->slices[_FP_DEF_INST][f_st->pool_index].start_tcam_idx
                + index1;
        }
    } else {
        *idx1 = _FP_INVALID_INDEX;
    }

    if (_FP_INVALID_INDEX != index2) {
        *idx2 =
        stage_fc->slices[_FP_DEF_INST][f_st->pool_index].start_tcam_idx
            + index2;
    } else {
        *idx2 = _FP_INVALID_INDEX;
    }
    *idx3 = _FP_INVALID_INDEX;
    *out_flags = flags;
    return (BCM_E_NONE);
}

#if defined(BCM_FIREBOLT2_SUPPORT) 
/*
 * Function: _bcm_field_fb2_qualify_ttl
 *
 * Purpose:
 *     Add a field qualification to a filter entry.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Field entry array.
 *     data - Data to qualify with 
 *     mask - Mask to qualify with
 *
 * Returns:
 *     BCM_E_XXX        Other errors
 */
int
_bcm_field_fb2_qualify_ttl(int unit, bcm_field_entry_t entry,
                           uint8 data, uint8 mask)
{
    _field_entry_t *f_ent;
    _field_group_t *fg = NULL;
    uint8 result;
    int rv;   

    rv = _bcm_field_entry_get_by_id(unit, entry, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    fg = f_ent->group;   
    /* Check if ttl is part of group qset. */
    rv = _bcm_field_qset_test(bcmFieldQualifyTtl, &fg->qset, &result);
    BCM_IF_ERROR_RETURN(rv);

    if (result) {
        return _field_qualify32(unit, entry, bcmFieldQualifyTtl, data, mask);
    }

    /* Only intraslice groups support encoded TTL values. */
    if (0 == (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        return BCM_E_PARAM;
    }

    /* bcmFieldQualifyTtl not present, use TTL_ENCODE (if present) */
    switch (data & mask) {
      case 0:
          data = 0x0;
          mask &= 0x3;
          break;
      case 1:
          data = 0x1;
          mask &= 0x3;
          break;
      case 255:
          data = 0x3;
          mask &= 0x3;
          break;
      default:
          data = 0x2;
          mask &= 0x3;    
    }
    /* Write the TTL_ENCODE data and mask - only for FB 2 */
    rv = _bcm_field_qual_tcam_key_mask_get(unit, f_ent + 1);
    BCM_IF_ERROR_RETURN(rv);

    f_ent[1].tcam.key[0] &= ~(mask << 6);
    f_ent[1].tcam.key[0] |= data << 6;

    f_ent[1].tcam.mask[0] &= ~(mask << 6);
    f_ent[1].tcam.mask[0] |= mask << 6;
    f_ent[1].flags |= _FP_ENTRY_DIRTY;
    return BCM_E_NONE;
}

/*
 * Function: _bcm_field_fb2_qualify_ttl_get
 *
 * Purpose:
 *     Add a field qualification to a filter entry.
 *
 * Parameters:
 *     unit  - BCM device number
 *     entry - Field entry array.
 *     data - Data to qualify with 
 *     mask - Mask to qualify with
 *
 * Returns:
 *     BCM_E_XXX        Other errors
 */
int
_bcm_field_fb2_qualify_ttl_get(int unit, bcm_field_entry_t entry,
                           uint8 *data, uint8 *mask)
{
    _field_entry_t *f_ent;
    _field_group_t *fg = NULL;
    uint8 free_tcam = FALSE;
    uint8 result;
    int rv;   

    rv = _bcm_field_entry_get_by_id(unit, entry, &f_ent);
    BCM_IF_ERROR_RETURN(rv);

    fg = f_ent->group;   
    /* Check if ttl is part of group qset. */
    rv = _bcm_field_qset_test(bcmFieldQualifyTtl, &fg->qset, &result);
    BCM_IF_ERROR_RETURN(rv);

    if (result) {
        return _bcm_field_entry_qualifier_uint8_get(unit, entry, 
                                                    bcmFieldQualifyTtl, 
                                                    data, mask);
    }

    /* Only intraslice groups support encoded TTL values. */
    if (0 == (fg->flags & _FP_GROUP_INTRASLICE_DOUBLEWIDE)) {
        return BCM_E_PARAM;
    }

    if (NULL == f_ent[1].tcam.key) {
        free_tcam = TRUE;
    }

    /* Read  the TTL_ENCODE data and mask - only for FB 2 */
    rv = _bcm_field_qual_tcam_key_mask_get(unit, f_ent + 1);
    BCM_IF_ERROR_RETURN(rv);

    /* bcmFieldQualifyTtl not present, use TTL_ENCODE (if present) */
    switch ((f_ent[1].tcam.key[0] >> 6) & 0x3) {
      case 0:
          *data = 0x0;
          *mask = 0xff;
          break;
      case 1:
          *data = 0x1;
          *mask &= 0xff;
          break;
      case 3:
          *data = 0xff;
          *mask &= 0xff;
          break;
      default:
          return (BCM_E_UNAVAIL);
    }
    if (free_tcam) {
        _bcm_field_qual_tcam_key_mask_free(unit, f_ent + 1);
    }
    return BCM_E_NONE;
}
#endif /* BCM_FIREBOLT2_SUPPORT */

/*
 * Function:
 *     _bcm_field_fb_selcode_get
 * Purpose:
 *     Finds a 4-tuple of select encodings that will satisfy the
 *     requested qualifier set (Qset).
 * Parameters:
 *     unit      - (IN) BCM unit number.
 *     stage_fc  - (IN) Stage Field control structure.
 *     qset_req  - (IN) Client qualifier set.
 *     fg        - (IN/OUT)Select code information filled into the group.  
 * 
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_selcode_get(int unit, _field_stage_t *stage_fc, 
                      bcm_field_qset_t *qset_req,
                      _field_group_t *fg)
{
    int  rv;                     /* Operation return status. */

    /* Input parameters check. */
    if ((NULL == fg) || (NULL == qset_req) || (NULL == stage_fc)) {
        return (BCM_E_PARAM);
    }

    /* Egress qualifiers are selected based on Key. */
    switch (stage_fc->stage_id) {
#if defined(BCM_FIREBOLT2_SUPPORT) 
      case _BCM_FIELD_STAGE_EGRESS:
          rv =  _field_fb_egress_selcode_get(unit, stage_fc, qset_req, fg);
          break;
#endif /* BCM_FIREBOLT2_SUPPORT */
      default:
          rv =  _bcm_field_selcode_get(unit, stage_fc, qset_req, fg);
    }
    return (rv);
}

/*
 * Function: _bcm_field_fb_udf_ipprotocol_delete_all
 *
 * Purpose:
 *     Delete all the UDF ip protocol match values.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ipprotocol_delete_all(int unit)
{
    int index;

    if (0 == SOC_REG_IS_VALID(unit, UDF_IPPROTO_MATCHr)) {
        return (BCM_E_NONE);
    }

    for (index = 0; index < _FP_DATA_IP_PROTOCOL_MAX; index++) {
        SOC_IF_ERROR_RETURN(WRITE_UDF_IPPROTO_MATCHr(unit, index, 0));
    }
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ip_protocol_set
 *
 * Purpose:
 *     Set the UDF Ip Protocol match value.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 *     index     - (IN) 0->7, Ethertype match index
 *     flags     - (IN) IP version match is enabled
 *     proto     - (IN) IP protocol id.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ipprotocol_set(int unit, int index, uint32 flags, uint8 proto)
{
    uint32 reg_val = 0;
    uint32 ip4_enable = 0;
    uint32 ip6_enable = 0;
    uint32 protocol = 0;

    if (0 == SOC_REG_IS_VALID(unit, UDF_IPPROTO_MATCHr)) {
        return (BCM_E_UNAVAIL); 
    }

    /* Range check index. */
    if (index < 0 || (_FP_DATA_IP_PROTOCOL_MAX <= index)) {
        return (BCM_E_PARAM);
    }

    protocol = proto;
    switch (flags) {
      case 0:
          break;
      case BCM_FIELD_DATA_FORMAT_IP4:
          ip4_enable = TRUE;
          break;
      case BCM_FIELD_DATA_FORMAT_IP6:
          ip6_enable = TRUE;
          break;
      case (BCM_FIELD_DATA_FORMAT_IP4 | BCM_FIELD_DATA_FORMAT_IP6):
          ip4_enable = ip6_enable = TRUE;
          break;
      default: 
          return (BCM_E_PARAM);
    }

    /*  Update UDF_IPPROTO_MATCHr register. */
    SOC_IF_ERROR_RETURN(READ_UDF_IPPROTO_MATCHr(unit, index, &reg_val));
    soc_reg_field_set(unit, UDF_IPPROTO_MATCHr, &reg_val, IPV4ENABLEf, ip4_enable);
    soc_reg_field_set(unit, UDF_IPPROTO_MATCHr, &reg_val, IPV6ENABLEf, ip6_enable);
    soc_reg_field_set(unit, UDF_IPPROTO_MATCHr, &reg_val, PROTOCOLf, protocol); 
    SOC_IF_ERROR_RETURN(WRITE_UDF_IPPROTO_MATCHr(unit, index, reg_val));

    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ip_protocol_get
 *
 * Purpose:
 *     Get the UDF Ip Protocol match value.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 *     index     - (IN) 0->7, Ethertype match index
 *     flags     - (OUT) IP version match is enabled
 *     proto     - (OUT) IP protocol id.
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ipprotocol_get(int unit, int index, uint32 *flags, uint8 *proto)
{
    uint32              reg_val = 0;

    /* Input parameters check. */
    if ((NULL == flags) || (NULL == proto)) {
        return (BCM_E_PARAM);
    }

    if (0 == SOC_REG_IS_VALID(unit, UDF_IPPROTO_MATCHr)) {
        return (BCM_E_UNAVAIL); 
    }

    /* Range check index. */
    if (index < 0 || (_FP_DATA_IP_PROTOCOL_MAX <= index)) {
        return (BCM_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_UDF_IPPROTO_MATCHr(unit, index, &reg_val));

    *flags = 0;
    if (soc_reg_field_get(unit, UDF_IPPROTO_MATCHr, reg_val, IPV4ENABLEf)) {
        *flags |= BCM_FIELD_DATA_FORMAT_IP4;
    }
    if (soc_reg_field_get(unit, UDF_IPPROTO_MATCHr, reg_val, IPV6ENABLEf)) {
        *flags |= BCM_FIELD_DATA_FORMAT_IP6;
    }

    *proto = soc_reg_field_get(unit, UDF_IPPROTO_MATCHr, reg_val, PROTOCOLf);

    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ethertype_delete_all
 *
 * Purpose:
 *     Delete all the UDF ethertype match values.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ethertype_delete_all(int unit)
{
    int index;

    if (0 == SOC_REG_IS_VALID(unit, UDF_ETHERTYPE_MATCHr)) {
        return (BCM_E_NONE);
    }

    for (index = 0; index < _FP_DATA_ETHERTYPE_MAX; index++) {
        SOC_IF_ERROR_RETURN(WRITE_UDF_ETHERTYPE_MATCHr(unit, index, 0));
    }
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ethertype_set
 *
 * Purpose:
 *     Set the UDF Ethertype match value.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 *     index     - (IN) 0->7, Ethertype match index
 *     frametype - (IN) BCM_FIELD_DATA_FORMAT_L2_XXX
 *     ethertype - (IN) Ethertype value to match
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ethertype_set(int unit, int index,
                         bcm_port_frametype_t frametype,
                         bcm_port_ethertype_t ethertype) 
{
    uint32 reg_val;
    uint32 enable;
    uint32 frame_type;

    if (0 == SOC_REG_IS_VALID(unit, UDF_ETHERTYPE_MATCHr)) {
        return (BCM_E_UNAVAIL);
    }

    /* Range check index. */
    if (index < 0 || (_FP_DATA_ETHERTYPE_MAX  <= index)) {
        return (BCM_E_PARAM);
    }

    switch (frametype) {
      case 0:
          enable = FALSE;
          ethertype = 0;
          frame_type = 0;
          break;
      case BCM_PORT_FRAMETYPE_ETHER2:
          frame_type = 0;
          enable = TRUE;
          break;
      case BCM_PORT_FRAMETYPE_SNAP:
          frame_type = 1;
          enable = TRUE;
          break;
      case BCM_PORT_FRAMETYPE_LLC:
          frame_type = 2;
          enable = TRUE;
          break;
      default:
          return (BCM_E_PARAM);
    }

    /*  Update UDF_ETHERTYPE_MATCHr register. */
    SOC_IF_ERROR_RETURN(READ_UDF_ETHERTYPE_MATCHr(unit, index, &reg_val));
    soc_reg_field_set(unit, UDF_ETHERTYPE_MATCHr, &reg_val, ENABLEf, enable);
    soc_reg_field_set(unit, UDF_ETHERTYPE_MATCHr, &reg_val, L2_PACKET_FORMATf, 
                      frame_type);
    soc_reg_field_set(unit, UDF_ETHERTYPE_MATCHr, &reg_val, ETHERTYPEf, 
                      (uint32)ethertype);
    SOC_IF_ERROR_RETURN(WRITE_UDF_ETHERTYPE_MATCHr(unit, index, reg_val));
    return (BCM_E_NONE);
}

/*
 * Function: _bcm_field_fb_udf_ethertype_get
 *
 * Purpose:
 *     Set the UDF Ethertype match value.
 *
 * Parameters:
 *     unit      - (IN) BCM device number
 *     index     - (IN) 0->7, Ethertype match index
 *     frametype - (OUT) BCM_FIELD_DATA_FORMAT_L2_XXX
 *     ethertype - (OUT) Ethertype value to match
 *
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_field_fb_udf_ethertype_get(int unit, int index,
                         bcm_port_frametype_t *frametype,
                         bcm_port_ethertype_t *ethertype) 
{
    uint32  reg_val = 0;
    uint32  enable;
    uint32  frame_type;     

    if (0 == SOC_REG_IS_VALID(unit, UDF_ETHERTYPE_MATCHr)) {
        return (BCM_E_UNAVAIL);
    }

    /* Range check index. */
    if (index < 0 || (_FP_DATA_ETHERTYPE_MAX  <= index)) {
        return (BCM_E_PARAM);
    }

    /* Input parameters check */
    if ((NULL == frametype) || (NULL == ethertype)) {
        return (BCM_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_UDF_ETHERTYPE_MATCHr(unit, index, &reg_val));

    enable = soc_reg_field_get(unit, UDF_ETHERTYPE_MATCHr, reg_val,
                               ENABLEf);
    if (enable) {
        frame_type = soc_reg_field_get(unit, UDF_ETHERTYPE_MATCHr, reg_val,
                                       L2_PACKET_FORMATf);

        switch (frame_type) {
          case 0:
              *frametype = BCM_PORT_FRAMETYPE_ETHER2;
              break;
          case 1:
              *frametype = BCM_PORT_FRAMETYPE_SNAP;
              break;
          case 2:
              *frametype = BCM_PORT_FRAMETYPE_LLC;
              break;
          default:
              return (BCM_E_PARAM);
        }

        *ethertype = soc_reg_field_get(unit, UDF_ETHERTYPE_MATCHr, reg_val,
                                       ETHERTYPEf);
    } else {
        *frametype = 0;
        *ethertype = 0;
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_udf_idx_to_l2_format
 * Purpose:
 *      Translate udf offset index l2 portion to packet l2 format.
 * Parameters:
 *      unit     - (IN) Bcm device number.
 *      idx      - (IN) Udf offset table index.
 *      l2       - (OUT)Packet L2 format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_udf_idx_to_l2_format(int unit, int idx, uint16 *l2)
{
    int l2_idx;    /* L2 portion of udf offset table index. */

    /* Input parameters check. */
    if (NULL == l2) {
        return (BCM_E_PARAM);
    }

    if (SOC_IS_TR_VL(unit)) {
        l2_idx = idx >> 5; 
    } else if  (SOC_IS_SC_CQ(unit)) {
        l2_idx = idx >> 4; 
    } else {
        l2_idx = idx >> 3; 
    }

    switch (l2_idx & 0x3) {
      case 0:
          *l2 = BCM_FIELD_DATA_FORMAT_L2_ETH_II;
          break;
      case 1:
          *l2 = BCM_FIELD_DATA_FORMAT_L2_SNAP;
          break;
      case 2:
          *l2 = BCM_FIELD_DATA_FORMAT_L2_LLC;
          break;
      default:
	  /* for case 3 */
          *l2 = _BCM_FIELD_DATA_FORMAT_ETHERTYPE;
          break;
    }
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _field_fb_udf_idx_to_vlan_format
 * Purpose:
 *      Translate udf offset index vlan portion to packet vlan tag format.
 * Parameters:
 *      unit     - (IN) Bcm device number.
 *      idx      - (IN) Udf offset table index.
 *      vlan     - (OUT)Packet vlan format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_udf_idx_to_vlan_format(int unit, int idx, uint16 *vlan)
{
    int vlan_idx;    /* Vlan portion of udf offset table index. */

    /* Input parameters check. */
    if (NULL == vlan) {
        return (BCM_E_PARAM);
    }

    if (SOC_IS_TR_VL(unit)) {
        vlan_idx = idx >> 7; 
    } else if  (SOC_IS_SC_CQ(unit)) {
        vlan_idx = idx >> 6; 
    } else {
        vlan_idx = idx >> 5; 
    }

    switch (vlan_idx & 0x3) {
      case 0:
          *vlan = BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG;
          break;
      case 1:
          *vlan = BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED;
          break;
      case 2:
          *vlan = BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED;
          break;
      default:        
          return (BCM_E_EMPTY);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_udf_idx_to_ip_format
 * Purpose:
 *      Translate udf offset index ip portion to packet ip format.
 * Parameters:
 *      unit     - (IN) Bcm device number.
 *      idx      - (IN) Udf offset table index.
 *      pkt_fmt  - (OUT)Packet format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_udf_idx_to_ip_format(int unit, int idx, 
                            bcm_field_data_packet_format_t *pkt_fmt)
{
    int ip_idx;    /* Ip portion of udf offset table index. */

    /* Input parameters check. */
    if (NULL == pkt_fmt) {
        return (BCM_E_PARAM);
    }

    if (SOC_IS_TR_VL(unit)) {
        ip_idx = idx & 0x1f;  
    } else if  (SOC_IS_SC_CQ(unit)) {
        ip_idx = idx & 0xf; 
    } else {
        ip_idx = idx & 0x7; 
    }

    /* Init tunnel/ip header version to none. */
    pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
    pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
    pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;

    switch (ip_idx) {
      case 0:
          pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
          pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
          break;
      case 1:
          pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_NONE;
          pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
          pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP_NONE;
          break;
      case 2:
          pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
          pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          break;
      case 3:
          pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
          pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
          pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          break;
      case 4:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          }
          break;
      case 5:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_IP_IN_IP;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          } else {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0;
          }
          break;
      case 6:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          } else {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1;
          }
          break;
      case 7:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP4;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 8:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP4;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 9:
          if (SOC_IS_TRX(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_GRE;
              pkt_fmt->outer_ip = BCM_FIELD_DATA_FORMAT_IP6;
              pkt_fmt->inner_ip = BCM_FIELD_DATA_FORMAT_IP6;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xa:
          if (SOC_IS_SC_CQ(unit)) {
                /* _BCM_FIELD_USER_IP_NOTUSED */
          } else if (SOC_IS_TR_VL(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS;
              pkt_fmt->mpls = BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xb:
          if (SOC_IS_SC_CQ(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0;
          } else if (SOC_IS_TR_VL(unit)) {
              pkt_fmt->tunnel = BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS;
              pkt_fmt->mpls = BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xc:
          if (SOC_IS_SC_CQ(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1;
          } 
          /* SOC_IS_TR_VL: _BCM_FIELD_USER_IP_NOTUSED */
          break;
      case 0xd:
          if (SOC_IS_SC_CQ(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0;
          } else if (SOC_IS_TR_VL(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xe:
          if (SOC_IS_SC_CQ(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1;
          } else if (SOC_IS_TR_VL(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0xf:
          if (SOC_IS_TR_VL(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      case 0x10:
          if (SOC_IS_TR_VL(unit)) {
              pkt_fmt->outer_ip = _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1;
          } else {
              return (BCM_E_EMPTY);
          }
          break;
      default:
          return (BCM_E_EMPTY);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_udf_offset_idx_to_packet_format
 * Purpose:
 *      Translate udf offset table index to packet format.
 * Parameters:
 *      unit     - (IN) Bcm device number.
 *      idx      - (IN) Udf offset table index.
 *      pkt_fmt  - (OUT)Packet format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_udf_offset_idx_to_packet_format(int unit, int idx,
                                bcm_field_data_packet_format_t *pkt_fmt)
{
    int rv;               /* Operation return status. */

    /* Input parameters check. */
    if (NULL == pkt_fmt) {
        return (BCM_E_PARAM);
    }

    /* parse L2 format */
    rv = _field_fb_udf_idx_to_l2_format(unit, idx, &pkt_fmt->l2);
    BCM_IF_ERROR_RETURN(rv);

    /* Ignore ethertype based indexes. */
    if (pkt_fmt->l2 == _BCM_FIELD_DATA_FORMAT_ETHERTYPE) {
        return (BCM_E_EMPTY);
    }

    /* parse vlan tag format */
    rv = _field_fb_udf_idx_to_vlan_format(unit, idx, &pkt_fmt->vlan_tag);
    BCM_IF_ERROR_RETURN(rv);

    /* parse L3 tunnel type/ Ip headers  format */
    rv = _field_fb_udf_idx_to_ip_format(unit, idx, pkt_fmt);
    BCM_IF_ERROR_RETURN(rv);

    /* Ignore ip protcol based indexes. */
    if ((pkt_fmt->outer_ip == _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0) ||
        (pkt_fmt->outer_ip == _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1) ||
        (pkt_fmt->outer_ip == _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0) ||
        (pkt_fmt->outer_ip == _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1)) {
        return (BCM_E_EMPTY);
    }

    return (BCM_E_NONE);
}

/*
 * Function:
 *     _field_fb_data_offset_install
 * Purpose:
 *     Write the info in udf_spec to hardware
 * Parameters:
 *     unit     - (IN) BCM device number. 
 *     f_dq     - (IN) Data qualifier structure.
 *     tbl_idx  - (IN) FP_UDF_OFFSETm table index.
 *     offset   - (IN) Word offset value.
 * Returns:
 *     BCM_E_PARAM - UDF number or User number out of range
 *     BCM_E_NONE
 * Notes:
 *     Calling function should have unit's lock.
 */
STATIC int
_field_fb_data_offset_install(int unit, _field_data_qualifier_t *f_dq, 
                           int tbl_idx, int offset)
{
    fp_udf_offset_entry_t  tbl_entry;   /* HW table buffer.               */ 
    uint32                 word_offset; /* Offset iterator.               */      
    int                    idx;         /* Data qualifier words iterator. */
    int                    offset_idx;  /* Swapped udf_offsetX number.    */
    uint32                 value;       /* HW buffer value.               */

    static soc_field_t  off_field[] = {UDF1_OFFSET0f, UDF1_OFFSET1f, 
                                       UDF1_OFFSET2f, UDF1_OFFSET3f,
                                       UDF2_OFFSET0f, UDF2_OFFSET1f,
                                       UDF2_OFFSET2f, UDF2_OFFSET3f};

    static soc_field_t  option_field[] = {UDF1_ADD_IPV4_OPTIONS0f,
                                          UDF1_ADD_IPV4_OPTIONS1f,
                                          UDF1_ADD_IPV4_OPTIONS2f,
                                          UDF1_ADD_IPV4_OPTIONS3f,
                                          UDF2_ADD_IPV4_OPTIONS0f,
                                          UDF2_ADD_IPV4_OPTIONS1f,
                                          UDF2_ADD_IPV4_OPTIONS2f,
                                          UDF2_ADD_IPV4_OPTIONS3f};

    static soc_field_t  gre_option_field[] = {UDF1_ADD_GRE_OPTIONS0f,
                                           UDF1_ADD_GRE_OPTIONS1f,
                                           UDF1_ADD_GRE_OPTIONS2f,
                                           UDF1_ADD_GRE_OPTIONS3f,
                                           UDF2_ADD_GRE_OPTIONS0f,
                                           UDF2_ADD_GRE_OPTIONS1f,
                                           UDF2_ADD_GRE_OPTIONS2f,
                                           UDF2_ADD_GRE_OPTIONS3f};

    /* Input parameters check. */
    if (NULL == f_dq) {
        return (BCM_E_PARAM);
    }

    /* index sanity check. */
    if ((tbl_idx < soc_mem_index_min(unit, FP_UDF_OFFSETm)) || 
        (tbl_idx > soc_mem_index_max(unit, FP_UDF_OFFSETm))) {
        return (BCM_E_PARAM);
    }

    /* Read udf offset table enty.*/
    BCM_IF_ERROR_RETURN
        (READ_FP_UDF_OFFSETm(unit, MEM_BLOCK_ANY, tbl_idx, &tbl_entry));

    word_offset = (offset < 0) ? 0 : (uint32)offset;

    /* Find a proper word to insert the data. */
    for (idx = 0; idx <= _FP_DATA_DATA1_WORD_MAX; idx++) {
        if (0 == (f_dq->hw_bmap & (1 << idx))) {
            continue;
        }

        if (word_offset > 31) {
            return BCM_E_PARAM;
        }

        offset_idx = idx;
        soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, off_field[offset_idx], 
                                       word_offset);

        /* Set the UDFn_ADD_IPV4_OPTIONSm bit accordingly */ 
        value = (f_dq->flags &
            BCM_FIELD_DATA_QUALIFIER_OFFSET_IP4_OPTIONS_ADJUST) ? TRUE : FALSE;
        soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                       option_field[offset_idx], value);

        /* Set the UDFn_ADD_IPV4_OPTIONSm bit accordingly */ 
        if (SOC_MEM_FIELD_VALID(unit, FP_UDF_OFFSETm, UDF1_ADD_GRE_OPTIONS0f)) {
            value = (f_dq->flags &
                     BCM_FIELD_DATA_QUALIFIER_OFFSET_GRE_OPTIONS_ADJUST) ? TRUE : FALSE;
            soc_FP_UDF_OFFSETm_field32_set(unit, &tbl_entry, 
                                           gre_option_field[offset_idx], value);
        }

        if (offset >= 0) {
            /*
             * If word_offset is 31, reset the offset to 0 for 126th and 127th byte
             * of the incoming packet 
             */
            if (word_offset == 31) {
               word_offset = 0;
            } else { 
               word_offset++;
            }
        }
    }

    /* Write back udf offset table enty.*/
    BCM_IF_ERROR_RETURN
        (WRITE_FP_UDF_OFFSETm(unit, MEM_BLOCK_ALL, tbl_idx, &tbl_entry));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _field_fb_data_qualifier_ethertype_install
 * Purpose:
 *      Add ethertype based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      f_dq       - (IN) Data qualifier descriptor.
 *      etype_idx  - (IN) Ethertype index. 
 *      etype      - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_data_qualifier_ethertype_install (int unit,
                                         _field_data_qualifier_t *f_dq,
                                         uint8 idx,
                                         _field_data_ethertype_t *etype)
{
    bcm_port_frametype_t       frametype; /* HW register encoding.       */ 
    int                        idx_mask;  /* Ethertype id index mask.    */ 
    int                        offset;    /* Adjusted word offset.       */ 
    int                        tbl_idx;   /* Udf offset table iterator.  */
    uint16                     l2;        /* L2 packet format.           */
    uint16                     vlan_tag;  /* Vlan packet format.         */
    
    int                        rv;        /* Operation return status.    */
    uint8                      avail = 0;


    /* Input parameters check. */
    if ((NULL == f_dq) || (NULL == etype)) {
        return (BCM_E_PARAM);
    }
    /* Verify UDF offset memory is valid. */
    if (0 == SOC_MEM_IS_VALID(unit, FP_UDF_OFFSETm)){
        return (BCM_E_UNAVAIL);
    }

    switch (etype->l2) {
      case BCM_FIELD_DATA_FORMAT_L2_ETH_II:
          frametype = BCM_PORT_FRAMETYPE_ETHER2;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_SNAP:
          frametype = BCM_PORT_FRAMETYPE_SNAP;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_LLC:
          frametype = BCM_PORT_FRAMETYPE_LLC;
          break;
      default: 
          frametype = 0;
    } 


    /* Get ethertype index mask. */
    if (SOC_IS_TR_VL(unit)) {
        idx_mask = 0x1f;  /* Ethertype is 5 lower bits of udf index. */
    } else {
        idx_mask = 0x7;   /* Ethertype is 3 lower bits of udf index. */
    }

    /* Calculate offset */
    offset = f_dq->offset + etype->relative_offset;
    if ((offset < 0) || ((offset  + f_dq->length) > _FP_DATA_OFFSET_MAX)) {
        return (BCM_E_PARAM);
    }
    offset = ((offset + 2) % 128) / 4; 

    /* Update ethertype match register. */
    rv = _bcm_field_fb_udf_ethertype_set(unit, idx, frametype, etype->ethertype);
    BCM_IF_ERROR_RETURN(rv);

    for (tbl_idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
         tbl_idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); tbl_idx++) {

        if ((tbl_idx & idx_mask) != idx) {
            continue;
        }

        /* parse L2 format */
        rv = _field_fb_udf_idx_to_l2_format(unit, tbl_idx, &l2);
        BCM_IF_ERROR_RETURN(rv);

        /* Ignore not ethertype based indexes. */
        if (l2 != _BCM_FIELD_DATA_FORMAT_ETHERTYPE) {
            continue;
        }

        /* parse vlan tag format */
        rv = _field_fb_udf_idx_to_vlan_format(unit, tbl_idx, &vlan_tag);
        BCM_IF_ERROR_RETURN(rv);
        /* Compare packet vlan tag format. */
        if ((etype->vlan_tag != BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY) &&
            (etype->vlan_tag != vlan_tag)) {
            continue;
        }

        /* Write offset to udf offset table . */
        rv = _field_fb_data_offset_install(unit, f_dq, tbl_idx, offset);
        BCM_IF_ERROR_RETURN(rv);
        avail = 1;
    }

    if (avail) {
        return (BCM_E_NONE);
    } else {
        return (BCM_E_UNAVAIL);
    }
}

/*
 * Function:
 *      _field_data_qualifier_ip_protocol_install
 * Purpose:
 *      Add ip_protocol based offset to data qualifier object.
 * Parameters:
 *      unit        - (IN) bcm device.
 *      f_dq        - (IN) Data qualifier descriptor.
 *      idx         - (IN) Ip protocol index. 
 *      proto       - (IN) Ip protocol based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_data_qualifier_ip_protocol_install(int unit,
                                         _field_data_qualifier_t *f_dq,
                                         uint8 idx,
                                         _field_data_protocol_t *proto)
{
    bcm_field_data_packet_format_t pkt_fmt;
    int                        offset;    /* Adjusted word offset.       */ 
    int                        tbl_idx;   /* Udf offset table iterator.  */
    uint16                     l2;        /* L2 packet format.           */
    uint16                     vlan_tag;  /* Vlan packet format.         */
    int                        rv;        /* Operation return status.    */
    uint8                      avail = 0;

     /* Input parameters check. */
     if ((NULL == f_dq) || (NULL == proto)) {
         return (BCM_E_PARAM);
     }

     /* Verify UDF offset memory is valid. */
     if (0 == SOC_MEM_IS_VALID(unit, FP_UDF_OFFSETm)){
         return (BCM_E_UNAVAIL);
     }

     /* Calculate offset */
     offset = f_dq->offset + proto->relative_offset;
     if ((offset < 0) || ((offset  + f_dq->length) > _FP_DATA_OFFSET_MAX)) {
         return (BCM_E_PARAM);
     }
     offset = ((offset + 2) % 128) / 4; 

     /* Update ip_protocol match register. */
     rv = _bcm_field_fb_udf_ipprotocol_set(unit, idx, proto->flags, proto->ip);
     BCM_IF_ERROR_RETURN(rv);

     for (tbl_idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
          tbl_idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); tbl_idx++) {
         /* parse L3 tunnel type/ Ip headers  format */
         rv = _field_fb_udf_idx_to_ip_format(unit, tbl_idx, &pkt_fmt);
         if (rv == BCM_E_EMPTY) {
             continue;
         }
         BCM_IF_ERROR_RETURN(rv);

         /* Ignore not ip protcol based indexes. */
         if (idx == 0) {
             if (((pkt_fmt.outer_ip != _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL0) ||
                  (0 == (proto->flags & BCM_FIELD_DATA_FORMAT_IP4))) &&  
                 (((pkt_fmt.outer_ip != _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL0) ||
                   (0 == (proto->flags & BCM_FIELD_DATA_FORMAT_IP6))))) {
                 continue;
             }
         } else {
             if (((pkt_fmt.outer_ip != _BCM_FIELD_DATA_FORMAT_IP4_PROTOCOL1) ||
                  (0 == (proto->flags & BCM_FIELD_DATA_FORMAT_IP4))) &&  
                 (((pkt_fmt.outer_ip != _BCM_FIELD_DATA_FORMAT_IP6_PROTOCOL1) ||
                   (0 == (proto->flags & BCM_FIELD_DATA_FORMAT_IP6))))) {
                 continue;
             }
         }

        /* parse l2 format */
        rv = _field_fb_udf_idx_to_l2_format(unit, tbl_idx, &l2);
        if (rv == BCM_E_EMPTY) {
            continue;
        }
        BCM_IF_ERROR_RETURN(rv);
        /* Compare packet l2 format. */
        if ((proto->l2  != BCM_FIELD_DATA_FORMAT_L2_ANY) &&
            (proto->l2  != l2)) {
            continue;
        }

        /* parse vlan tag format */
        rv = _field_fb_udf_idx_to_vlan_format(unit, tbl_idx, &vlan_tag);
        if (rv == BCM_E_EMPTY) {
            continue;
        }
        BCM_IF_ERROR_RETURN(rv);
        /* Compare packet vlan tag format. */
        if ((proto->vlan_tag != BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY) &&
            (proto->vlan_tag != vlan_tag)) {
            continue;
        }

        /* Write offset to udf offset table . */
        rv = _field_fb_data_offset_install(unit, f_dq, tbl_idx, offset);
        BCM_IF_ERROR_RETURN(rv);
        avail = 1;
    }

    if (avail) {
        return (BCM_E_NONE);
    } else {
        return (BCM_E_UNAVAIL);
    }
}


/*
 * Function:
 *      _field_fb_packet_format_is_subset
 * Purpose:
 *      Check if master packet format is superset of 
 *      subset packet format.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      master     - (IN) Superset packet format.
 *      subset     - (IN) Subset packet format.
 *      cmp_result - (OUT)Comparison result.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_packet_format_is_subset(int unit, 
                                  bcm_field_data_packet_format_t *master, 
                                  bcm_field_data_packet_format_t *subset, 
                                  uint8 *cmp_result) 
{
    /* Input parameters check. */
    if ((NULL == master) || (NULL == subset) || (NULL == cmp_result)) {
        return (BCM_E_PARAM);
    }

    /* Compare packet l2 format. */
    if ((master->l2 != BCM_FIELD_DATA_FORMAT_L2_ANY) &&
        (master->l2 != subset->l2)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare packet vlan tag format. */
    if ((master->vlan_tag  != BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY) &&
        (master->vlan_tag != subset->vlan_tag)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare packet tunnel type. */
    if ((master->tunnel != BCM_FIELD_DATA_FORMAT_TUNNEL_ANY) &&
        (master->tunnel != subset->tunnel)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare outer ip header. */
    if ((master->tunnel != BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS) &&  
        (master->outer_ip != BCM_FIELD_DATA_FORMAT_IP_ANY) && 
        (master->outer_ip != subset->outer_ip)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare inner ip header. */
    if ((master->tunnel != BCM_FIELD_DATA_FORMAT_TUNNEL_NONE) && 
        (master->tunnel != BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS) &&
        (master->inner_ip != BCM_FIELD_DATA_FORMAT_IP_ANY) &&
        (master->inner_ip != subset->inner_ip)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }

    /* Compare number of mpls labels. */
    if ((master->tunnel == BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS) && 
        (master->mpls != BCM_FIELD_DATA_FORMAT_MPLS_ANY) &&
        (master->mpls != subset->mpls)) {
        *cmp_result = FALSE;
        return (BCM_E_NONE);
    }
    *cmp_result = TRUE;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_tunnel_shim_size_get
 * Purpose:
 *      Get  tunnel shim size  based on packet
 *      format descriptor.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      pkt_fmt    - (IN) Packet format.
 *      *size      - (OUT)Outer L3 + tunnel shim size.
 * Returns:
 *      BCM_E_XXX
 * NOTE: 
 *      This api calculates basic header size. 
 *      Gre Options... are not handled here.
 *      Please set proper FIELD_DATA_QUALIFIER_XXX options adjust flag.
 */
STATIC int
_field_fb_packet_format_tunnel_shim_size_get(int unit, 
                               bcm_field_data_packet_format_t *pkt_fmt, 
                               uint8 *size) 
{
    /* Input parameters check. */
    if ((NULL == pkt_fmt) || (NULL == size)) {
        return (BCM_E_PARAM);
    }

    if (BCM_FIELD_DATA_FORMAT_TUNNEL_GRE == pkt_fmt->tunnel) { 
        *size = 4;  /* Basic (no key/no checksum/no options). */
    } else {
        *size = 0;  /* IPv4 header size. */
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_ip_size_get
 * Purpose:
 *      Get outer l3 header + tunnel shim size  based on packet
 *      format descriptor.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      ip         - (IN) BCM_FIELD_DATA_FORMAT_IP_XXX.
 *      *size      - (OUT)Outer L3 + tunnel shim size.
 * Returns:
 *      BCM_E_XXX
 * NOTE: 
 *      This api calculates basic header size. 
 *      IP options/Extension headers  ... are not handled here.
 *      Please set proper FIELD_DATA_QUALIFIER_XXX options adjust flag.
 */
STATIC int
_field_fb_packet_format_ip_size_get(int unit, uint16 ip, uint8 *size) 
{
    /* Input parameters check. */
    if (NULL == size) {
        return (BCM_E_PARAM);
    }

    if (BCM_FIELD_DATA_FORMAT_IP6 == ip) {
        *size = 40;  /* Ipv6 header size */
    } else if (BCM_FIELD_DATA_FORMAT_IP4 == ip) {
        *size = 20;  /* IPv4 header size. */
    } else if (BCM_FIELD_DATA_FORMAT_IP_NONE == ip) {
        *size = 0;
    } else {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_mpls_labels_size_get
 * Purpose:
 *      Get mpls labels size based on packet format descriptor.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      pkt_fmt    - (IN) Packet format.
 *      *size      - (OUT)mpls labels size.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_packet_format_mpls_labels_size_get(int unit, 
                               bcm_field_data_packet_format_t *pkt_fmt, 
                               uint8 *size) 
{
    /* Input parameters check. */
    if ((NULL == pkt_fmt) || (NULL == size)) {
        return (BCM_E_PARAM);
    }

    if (BCM_FIELD_DATA_FORMAT_TUNNEL_MPLS != pkt_fmt->tunnel) {
        *size = 0;
    } else if (BCM_FIELD_DATA_FORMAT_MPLS_ONE_LABEL == pkt_fmt->mpls) {
        *size = 4;
    } else if (BCM_FIELD_DATA_FORMAT_MPLS_TWO_LABELS == pkt_fmt->mpls) {
        *size = 8;
    }  else {
        return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_l2_header_size_get
 * Purpose:
 *      Get l2 header size based on packet format descriptor.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      pkt_fmt    - (IN) Packet format.
 *      *size      - (OUT)L2 header size.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_packet_format_l2_header_size_get(int unit, 
                               bcm_field_data_packet_format_t *pkt_fmt, 
                               uint8 *size) 
{
    uint8 tmp;

    /* Input parameters check. */
    if ((NULL == pkt_fmt) || (NULL == size)) {
        return (BCM_E_PARAM);
    }

    switch(pkt_fmt->l2) {
      case BCM_FIELD_DATA_FORMAT_L2_ETH_II:
      case BCM_FIELD_DATA_FORMAT_L2_LLC:
          tmp = 14;
          break;
      case BCM_FIELD_DATA_FORMAT_L2_SNAP:
          tmp = 22;
          break;
      default:
          return (BCM_E_INTERNAL);
    }

    switch(pkt_fmt->vlan_tag) {
      case BCM_FIELD_DATA_FORMAT_VLAN_NO_TAG:
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_SINGLE_TAGGED:
          tmp += 4;  
          break;
      case BCM_FIELD_DATA_FORMAT_VLAN_DOUBLE_TAGGED:
          tmp += 8;
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    *size = tmp;
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_packet_format_offset_adjust
 * Purpose:
 *      Calculate word offset for data qualifier 
 *      based on offset based, relative offset & master offset.
 * Parameters:
 *      unit       - (IN) Bcm device number.
 *      f_dq       - (IN) Data qualifier descriptor.
 *      pkt_fmt    - (IN) Installed packet format.
 *      offset     - (OUT)Final word offset.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_field_fb_packet_format_offset_adjust(int unit, 
                               _field_data_qualifier_t *f_dq, 
                               bcm_field_data_packet_format_t *pkt_fmt, 
                               int *offset) 
{
    int tmp_offset;         /* Local calculation variable. */
    uint8 size;             /* Various headers sizes.      */
    int rv;                 /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == f_dq) || (NULL == pkt_fmt) || (NULL == offset)) {
        return (BCM_E_PARAM);
    }

    /* Calculate offset */
    tmp_offset = f_dq->offset + pkt_fmt->relative_offset;

    if (tmp_offset == _FP_DATA_OFFSET_MAX - 2) {
        if (f_dq->length > 4) {
            return (BCM_E_PARAM);
        }
    } else if ((tmp_offset < 0) || 
               (tmp_offset + f_dq->length) >= _FP_DATA_OFFSET_MAX) {
        return (BCM_E_PARAM);
    }

    switch (f_dq->offset_base) {
      case bcmFieldDataOffsetBasePacketStart:
          break;

      case bcmFieldDataOffsetBaseOuterL3Header:
          /* a) Add L2 header size. */  
          rv = _field_fb_packet_format_l2_header_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* b) Add size of mpls labels. */  
          rv = _field_fb_packet_format_mpls_labels_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;
          break;

      case bcmFieldDataOffsetBaseInnerL3Header:
          /* a) Add L2 header size. */  
          rv = _field_fb_packet_format_l2_header_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;


          /* b) Add size of outer l3 header . */  
          rv =  _field_fb_packet_format_ip_size_get(unit, pkt_fmt->outer_ip, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* c) Add size of GRE shim. */  
          rv =  _field_fb_packet_format_tunnel_shim_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;
          break;

      case bcmFieldDataOffsetBaseOuterL4Header:
          /* a) Add L2 header size. */  
          rv = _field_fb_packet_format_l2_header_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* b) Add size of outer l3 header . */  
          rv =  _field_fb_packet_format_ip_size_get(unit, pkt_fmt->outer_ip, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;
          break;

      case bcmFieldDataOffsetBaseInnerL4Header:
          /* a) Add L2 header size. */  
          rv = _field_fb_packet_format_l2_header_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;


          /* b) Add size of outer l3 header . */  
          rv =  _field_fb_packet_format_ip_size_get(unit, pkt_fmt->outer_ip, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* c) Add size of GRE shim. */  
          rv =  _field_fb_packet_format_tunnel_shim_size_get(unit, pkt_fmt, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;

          /* d) Add size of inner l3 header . */  
          rv =  _field_fb_packet_format_ip_size_get(unit, pkt_fmt->inner_ip, &size); 
          BCM_IF_ERROR_RETURN(rv);
          tmp_offset += size;
          break;

      case bcmFieldDataOffsetBaseHigigHeader:
          if (SOC_IS_FB(unit) ||
              SOC_IS_HB_GW(unit) ||
              SOC_IS_SC_CQ(unit) ||
              SOC_IS_ENDURO(unit) ||
              soc_feature(unit, soc_feature_field_udf_offset_hg_114B)) {
              /* Hardcoded Higig header offset is 114 bytes. */  
              tmp_offset += 0x72;
          } else {
              /* Hardcoded Higig header length is 12 bytes. */  
              tmp_offset += 0xc;
          }
          break;

      case bcmFieldDataOffsetBaseHigig2Header:
          if (SOC_IS_FB(unit) ||
              SOC_IS_HB_GW(unit) ||
              SOC_IS_SC_CQ(unit) ||
              SOC_IS_ENDURO(unit) ||
              soc_feature(unit, soc_feature_field_udf_offset_hg2_110B)) {
              /* Hardcoded Higig2 header offset is 110 bytes. */  
              tmp_offset += 0x6e;
          } else {
              /* Hardcoded Higig2 header length is 16 bytes. */  
              tmp_offset += 0x10;
          }  
          break;

      default:
          return (BCM_E_PARAM);
    }

    *offset = ((tmp_offset + 2) % 128) / 4; 
    return (BCM_E_NONE);
}


/*
 * Function:
 *      _bcm_field_fb_data_qualifier_packet_format_add
 * Purpose:
 *      Add packet format based offset to data qualifier object.
 * Parameters:
 *      unit          - (IN) Bcm device number.
 *      qual_id       - (IN) Data qualifier id.
 *      packet_format - (IN) Packet format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_packet_format_add(int unit,  int qual_id,
                                bcm_field_data_packet_format_t *packet_format)
{
    bcm_field_data_packet_format_t pkt_fmt;   /* Packet format iterator.     */
    _field_data_qualifier_t        *f_dq;     /* Data qualifier descriptor.  */
    _field_stage_t                 *stage_fc; /* Stage field control.        */
    uint8                          cmp_result;/* Packet format compare result*/ 
    int                            offset = 0;/* Adjusted word offset.       */ 
    int                            idx;       /* Udf offset table iterator.  */
    int                            rv;        /* Operation return status.    */
    uint8                          avail = 0;

    /* Input parameters check. */
    if ((NULL == packet_format)) {
        return (BCM_E_PARAM);
    }

    /* Verify UDF offset memory is valid. */
    if (0 == SOC_MEM_IS_VALID(unit, FP_UDF_OFFSETm)){
        return (BCM_E_UNAVAIL);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id, &f_dq);
    BCM_IF_ERROR_RETURN(rv);


    for (idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
         idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); idx++) {
        sal_memset(&pkt_fmt, 0, sizeof (bcm_field_data_packet_format_t));

        /* Get packet format associated with an index. */
        rv = _field_fb_udf_offset_idx_to_packet_format(unit, idx, &pkt_fmt);
        if (BCM_FAILURE(rv)) {
            if (rv == BCM_E_EMPTY) {
                /* Unused/ethertype/ip protocol  table index. */
                continue;
            }
            return (rv);
        }

        /* Check that idx packet format is subset of installed packet format. */
        rv = _field_fb_packet_format_is_subset(unit, packet_format, 
                                               &pkt_fmt, &cmp_result);
        BCM_IF_ERROR_RETURN(rv);
        if (FALSE == cmp_result) {
            continue;
        }

        /* Calculate base offset adjusted offset. */
        rv = _field_fb_packet_format_offset_adjust(unit, f_dq, 
                                                   packet_format, &offset);
        BCM_IF_ERROR_RETURN(rv);


        /* Write offset to udf offset table . */
        rv = _field_fb_data_offset_install(unit, f_dq, idx, offset);
        BCM_IF_ERROR_RETURN(rv);
        avail = 1;

    }

    if (avail) {
        return (BCM_E_NONE);
    } else {
        return (BCM_E_UNAVAIL);
    }
}

/*
 * Function:
 *      _bcm_field_fb_data_qualifier_packet_format_delete
 * Purpose:
 *      Remove packet format based offset to data qualifier object.
 * Parameters:
 *      unit          - (IN) Bcm device number.
 *      qual_id       - (IN) Data qualifier id.
 *      packet_format - (IN) Packet format specification.                 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_packet_format_delete(int unit,  int qual_id,
                                bcm_field_data_packet_format_t *packet_format)
{
    bcm_field_data_packet_format_t pkt_fmt;   /* Packet format iterator.     */
    _field_data_qualifier_t        *f_dq;     /* Data qualifier descriptor.  */
    _field_stage_t                 *stage_fc; /* Stage field control.        */
    uint8                          cmp_result;/* Packet format compare result*/ 
    int                            idx;       /* Udf offset table iterator.  */
    int                            rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == packet_format)) {
        return (BCM_E_PARAM);
    }

    /* Verify UDF offset memory is valid. */
    if (0 == SOC_MEM_IS_VALID(unit, FP_UDF_OFFSETm)){
        return (BCM_E_UNAVAIL);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id, &f_dq);
    BCM_IF_ERROR_RETURN(rv);


    for (idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
         idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); idx++) {
        sal_memset(&pkt_fmt, 0, sizeof (bcm_field_data_packet_format_t));

        /* Get packet format associated with an index. */
        rv = _field_fb_udf_offset_idx_to_packet_format(unit, idx, &pkt_fmt);
        if (BCM_FAILURE(rv)) {
            if (rv == BCM_E_EMPTY) {
                /* Unused/ethertype/ip protocol  table index. */
                continue;
            }
            return (rv);
        }

        /* Check that idx packet format is subset of installed packet format. */
        rv = _field_fb_packet_format_is_subset(unit, packet_format, &pkt_fmt,
                                            &cmp_result);
        BCM_IF_ERROR_RETURN(rv);
        if (FALSE == cmp_result) {
            continue;
        }

        /* Write offset to udf offset table . */
        rv = _field_fb_data_offset_install(unit, f_dq, idx, -1);
        BCM_IF_ERROR_RETURN(rv);

    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _field_fb_data_qualifier_ethertype_add
 * Purpose:
 *      Add ethertype based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_field_fb_data_qualifier_ethertype_add(int unit,  int qual_id,
                                           bcm_field_data_ethertype_t *etype)
{
    _field_data_ethertype_t    *etype_ptr;/* Ether type pointer.         */
    _field_stage_t             *stage_fc; /* Stage field control.        */
    _field_data_qualifier_t    *f_dq;     /* Data qualifier descriptor.  */
    int                        idx;       /* Ethertype allocation index. */
    int                        unused_idx;/* Unused ethertype index.     */
    int                        rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == etype)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    /*
     *  Ethertype based qualifier offset must be 
     *  from the beginning of the packet. 
     */
    if (f_dq->offset_base != bcmFieldDataOffsetBasePacketStart) {
        return (BCM_E_UNAVAIL);
    }

    unused_idx = -1;
    for (idx = 0; idx < _FP_DATA_ETHERTYPE_MAX; idx++) {
        etype_ptr = stage_fc->data_ctrl->etype + idx;
        /* Find an entry with identical ethertype/l2format. */
            if ((etype->l2 == etype_ptr->l2) &&
                (etype->ethertype == etype_ptr->ethertype) &&
                (etype_ptr->ref_count > 0)) {
                etype_ptr->ref_count++;
                rv = BCM_E_NONE;
                break;
            }

        /* Find unused entry. */
        if ((-1 == unused_idx) && (0 == etype_ptr->ref_count)) {
            unused_idx = idx;
        }
    }

        if (_FP_DATA_ETHERTYPE_MAX == idx) {
            /* No match found -> check for unused entry. */
            if (-1 == unused_idx) {
                return (BCM_E_RESOURCE);
            }
            idx = unused_idx;
            etype_ptr = stage_fc->data_ctrl->etype + idx;
            etype_ptr->ref_count = 1;
        }
        etype_ptr->ethertype = etype->ethertype;
        etype_ptr->l2 = etype->l2;
        etype_ptr->vlan_tag  = etype->vlan_tag;
        etype_ptr->relative_offset = etype->relative_offset;

        rv = _field_fb_data_qualifier_ethertype_install(unit, f_dq, 
                                                        idx, etype_ptr);

    return (rv);
}

/*
 * Function:
 *      _field_fb_data_qualifier_ethertype_delete
 * Purpose:
 *      Remove ethertype based offset from data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification. 
 * Returns:
 *      BCM_E_XXX
 */
int
_field_fb_data_qualifier_ethertype_delete(int unit,  int qual_id,
                                          bcm_field_data_ethertype_t *etype)
{
    _field_stage_t             *stage_fc; /* Stage field control.        */
    _field_data_qualifier_t    *f_dq;     /* Data qualifier descriptor.  */
    int                        idx;       /* Ethertype allocation index. */
    int                        offset_idx;/* UDF offset. */
    int                        rv;        /* Operation return status.    */
    _field_data_ethertype_t    *etype_ptr = NULL;/* Ether type pointer.  */
    int                        tbl_idx;   /* Udf offset table iterator.  */
    int                        idx_mask;  /* Ethertype id index mask.    */
    uint16                     l2 = 0;    /* L2 packet format.           */
    uint16                     vlan_tag;  /* Vlan packet format.         */
    fp_udf_offset_entry_t      tbl_entry;   /* HW table buffer.           */
    int                        match_found = 0;

    static soc_field_t  off_field[] = {UDF1_OFFSET0f, UDF1_OFFSET1f,
                                       UDF1_OFFSET2f, UDF1_OFFSET3f,
                                       UDF2_OFFSET0f, UDF2_OFFSET1f,
                                       UDF2_OFFSET2f, UDF2_OFFSET3f};

    /* Input parameters check. */
    if ((NULL == etype)) {
        return (BCM_E_PARAM);
    }

    /* Get ethertype index mask. */
    if (SOC_IS_TR_VL(unit)) {
        idx_mask = 0x1f;  /* Ethertype is 5 lower bits of udf index. */
    } else {
        idx_mask = 0x7;   /* Ethertype is 3 lower bits of udf index. */
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id,  &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    for (idx = 0; idx < _FP_DATA_ETHERTYPE_MAX; idx++) {
        etype_ptr = stage_fc->data_ctrl->etype + idx;

        if (etype_ptr->ref_count <= 0) {
            continue;
        }

        /* Find an entry with identical ethertype/l2format. */
            if ((etype->l2 == etype_ptr->l2) &&
                (etype->ethertype == etype_ptr->ethertype)) {
                for (tbl_idx = soc_mem_index_min(unit, FP_UDF_OFFSETm); 
                    tbl_idx <= soc_mem_index_max(unit, FP_UDF_OFFSETm); tbl_idx++) {
            
                if ((tbl_idx & idx_mask) != idx) {
                        continue;
                    }
            
                    /* parse L2 format */
                    rv = _field_fb_udf_idx_to_l2_format(unit, tbl_idx, &l2);
                    BCM_IF_ERROR_RETURN(rv);
            
                    /* Ignore not ethertype based indexes. */
                    if (l2 != _BCM_FIELD_DATA_FORMAT_ETHERTYPE) {
                        continue;
                    }
            
                    /* parse vlan tag format */
                    rv = _field_fb_udf_idx_to_vlan_format(unit, tbl_idx, &vlan_tag);
                    BCM_IF_ERROR_RETURN(rv);
                    /* Compare packet vlan tag format. */
                    if (etype->vlan_tag != vlan_tag) {
                        continue;
                    }

                    /* Read udf offset table enty.*/
                    BCM_IF_ERROR_RETURN
                        (READ_FP_UDF_OFFSETm(unit, MEM_BLOCK_ANY, tbl_idx, &tbl_entry));

                /* Check whether the entry has valid data or not.*/
                for (offset_idx = 0; offset_idx < _FP_DATA_DATA1_WORD_MAX ;
                                                              offset_idx++) {
                    if ((soc_FP_UDF_OFFSETm_field32_get(
                         unit, &tbl_entry, off_field[offset_idx])) != 0) { 
                       match_found = 1;
                       break;
                    }
                }

                if (!match_found) {
                        continue;
                    }

                    etype_ptr->ref_count--;            
                    if (etype_ptr->ref_count >= 0) { 
                        /* Write offset to udf offset table . */
                        rv = _field_fb_data_offset_install(unit, f_dq, tbl_idx, -1);
                        BCM_IF_ERROR_RETURN(rv);
                        if (etype_ptr->ref_count == 0) {
                            /* Update ethertype match register. */
                            rv = _bcm_field_fb_udf_ethertype_set(unit, idx, 0, 0);
                            BCM_IF_ERROR_RETURN(rv);
                        }
                        break;
                    }
                }
            if (match_found) {
                break;
            }
        }
    }

    if (_FP_DATA_ETHERTYPE_MAX == idx) {
        return (BCM_E_NOT_FOUND);
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_field_fb_data_qualifier_ip_protocol_add
 * Purpose:
 *      Add ip_protocol based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) Bcm device number. 
 *      qual_id    - (IN) Data qualifier id.
 *      ip_protocol- (IN) Ip protocol based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_ip_protocol_add(int unit,  int qual_id,
                                bcm_field_data_ip_protocol_t *ip_protocol)
{
    _field_data_protocol_t     *proto_ptr;/* Ip protocol pointer.        */
    _field_data_qualifier_t    *f_dq;     /* Data qualifier descriptor.  */
    _field_stage_t             *stage_fc; /* Stage field control.        */
    int                        unused_idx;/* Unused ip_protocol index.   */
    int                        idx;       /* Ethertype allocation index. */
    int                        flags;     /* Ip protocol version flags.  */
    int                        rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == ip_protocol)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id, &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    unused_idx = -1;
    flags = ip_protocol->flags & 
        (BCM_FIELD_DATA_FORMAT_IP4 | BCM_FIELD_DATA_FORMAT_IP6);
    for (idx = 0; idx < _FP_DATA_IP_PROTOCOL_MAX; idx++) {
        proto_ptr = stage_fc->data_ctrl->ip + idx;

        /* Find an entry with identical ip_protocol. */
        if ((ip_protocol->ip == proto_ptr->ip) &&
            ((proto_ptr->ip4_ref_count > 0) || 
             ((proto_ptr->ip6_ref_count > 0)))) {
            break; 
        }
        /* Find unused entry. */
        if ((-1 == unused_idx) && 
            (0 == proto_ptr->ip4_ref_count) && 
            (0 == proto_ptr->ip6_ref_count)) {
            unused_idx = idx;
        }
    }
    if (_FP_DATA_IP_PROTOCOL_MAX == idx) {
        /* No match found -> check for unused entry. */
        if (-1 == unused_idx) {
            return (BCM_E_RESOURCE);
        }
        idx = unused_idx;
    }

    proto_ptr = stage_fc->data_ctrl->ip + idx;
    /* Increment reference count. */
    if (flags & BCM_FIELD_DATA_FORMAT_IP4) {
        proto_ptr->ip4_ref_count++;
    } 
    if (flags & BCM_FIELD_DATA_FORMAT_IP6) {
        proto_ptr->ip6_ref_count++;
    } 

    /* Check if reinstall is required. */
    proto_ptr->ip = ip_protocol->ip;
    proto_ptr->l2 = ip_protocol->l2;
    proto_ptr->vlan_tag = ip_protocol->vlan_tag;
    proto_ptr->flags = flags;
    proto_ptr->relative_offset = ip_protocol->relative_offset;

    rv =  _field_fb_data_qualifier_ip_protocol_install(unit, f_dq,
                                                       idx, proto_ptr);
    return (rv);
}

/*
 * Function:
 *      _bcm_field_fb_data_qualifier_ip_protocol_delete
 * Purpose:
 *      Remove ip protocol based offset from data qualifier object.
 * Parameters:
 *      unit        - (IN) bcm device.
 *      qual_id     - (IN) Data qualifier id.
 *      ip_protocol - (IN) Ip Protocol based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_ip_protocol_delete(int unit,  int qual_id,
                          bcm_field_data_ip_protocol_t *ip_protocol)
{
    _field_data_protocol_t     *proto_ptr;/* Ip protocol pointer.        */
    _field_data_qualifier_t    *f_dq;     /* Data qualifier descriptor.  */
    _field_stage_t             *stage_fc; /* Stage field control.        */
    int                        idx;       /* Ethertype allocation index. */
    int                        flags;     /* Ip protocol version flags.  */
    int                        rv;        /* Operation return status.    */

    /* Input parameters check. */
    if ((NULL == ip_protocol)) {
        return (BCM_E_PARAM);
    }

    /* Get stage field control structure. */
    rv = _field_stage_control_get(unit, _BCM_FIELD_STAGE_INGRESS, &stage_fc); 
    BCM_IF_ERROR_RETURN(rv);

    /* Get data qualifier info. */
    rv = _bcm_field_data_qualifier_get(unit, stage_fc, qual_id, &f_dq);
    BCM_IF_ERROR_RETURN(rv);

    flags = ip_protocol->flags & 
        (BCM_FIELD_DATA_FORMAT_IP4 | BCM_FIELD_DATA_FORMAT_IP6);
    for (idx = 0; idx < _FP_DATA_IP_PROTOCOL_MAX; idx++) {
        proto_ptr = stage_fc->data_ctrl->ip + idx;

        /* Find an entry with identical ip_protocol. */
        if ((ip_protocol->ip == proto_ptr->ip) &&
            ((proto_ptr->ip4_ref_count > 0) || 
             ((proto_ptr->ip6_ref_count > 0)))) {
            break; 
        }
    }
    if (_FP_DATA_IP_PROTOCOL_MAX == idx) {
        /* No match found. */
        return (BCM_E_NOT_FOUND);
    }

    proto_ptr = stage_fc->data_ctrl->ip + idx;
    /* Increment reference count. */
    if ((flags & BCM_FIELD_DATA_FORMAT_IP4) &&
        (proto_ptr->ip4_ref_count > 0)) {
        proto_ptr->ip4_ref_count--;
        if (0 == proto_ptr->ip4_ref_count) {
            proto_ptr->flags &= ~BCM_FIELD_DATA_FORMAT_IP4;
        } 
    } 
    if ((flags & BCM_FIELD_DATA_FORMAT_IP6) &&
        (proto_ptr->ip6_ref_count > 0)) {
        proto_ptr->ip6_ref_count--;
        if (0 == proto_ptr->ip6_ref_count) {
            proto_ptr->flags &= ~BCM_FIELD_DATA_FORMAT_IP6;
        }
    } 

    /* Mark entry as unused. */
    if ((0 == proto_ptr->ip4_ref_count) && (0 == proto_ptr->ip6_ref_count)) {
        proto_ptr->ip = 0;
        proto_ptr->flags = 0;
    } 

    /* Install protocol match register. */
    rv = _bcm_field_fb_udf_ipprotocol_set(unit, idx, proto_ptr->flags, 
                                          proto_ptr->ip);
    return (rv);
}


#define _FP_L2_FORMAT_MIN   (0)
#define _FP_L2_FORMAT_MAX   (2)
/*
 * Function:
 *      _bcm_field_fb_data_qualifier_ethertype_add
 * Purpose:
 *      Add ethertype based offset to data qualifier object.
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_ethertype_add(int unit,  int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    int               idx;             /* L2 format iteration index.*/
    uint16            l2;              /* Installed L2 format.      */
    int               rv = BCM_E_NONE; /* Operation return status.  */

    /* Input parameters check. */
    if (NULL == etype) {
        return (BCM_E_PARAM);
    }

    l2 = etype->l2;
    for (idx = _FP_L2_FORMAT_MIN; idx <= _FP_L2_FORMAT_MAX; idx++) {
        if (0 == (l2 & (1 << idx))) {
            continue;
        }
        etype->l2 = (1 << idx);
        rv = _field_fb_data_qualifier_ethertype_add(unit, qual_id, etype);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    return (rv);
}

/*
 * Function:
 *      _bcm_field_fb_data_qualifier_ethertype_delete
 * Purpose:
 *      Remove ethertype based offset from data qualifier object. 
 * Parameters:
 *      unit       - (IN) bcm device.
 *      qual_id    - (IN) Data qualifier id.
 *      etype      - (IN) Ethertype based offset specification.                 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_field_fb_data_qualifier_ethertype_delete(int unit, int qual_id,
                                 bcm_field_data_ethertype_t *etype)
{
    int               idx;             /* L2 format iteration index.*/
    uint16            l2;              /* Installed L2 format.      */
    int               rv = BCM_E_NONE; /* Operation return status.  */

    /* Input parameters check. */
    if (NULL == etype) {
        return (BCM_E_PARAM);
    }

    l2 = etype->l2;
    for (idx = _FP_L2_FORMAT_MIN; idx <= _FP_L2_FORMAT_MAX; idx++) {
        if (0 == (l2 & (1 << idx))) {
            continue;
        }
        etype->l2 = (1 << idx);
        rv = _field_fb_data_qualifier_ethertype_delete(unit, qual_id, etype);
        if (BCM_FAILURE(rv)) {
            break;
        }
    }
    return (rv);
}
#undef _FP_L2_FORMAT_MIN
#undef _FP_L2_FORMAT_MAX


/*
 * Function:
 *     _field_fb_functions_init
 *
 * Purpose:
 *     Set up functions pointers 
 *
 * Parameters:
 *     unit     - (IN) BCM device number.
 *     stage_fc - (IN/OUT) pointers to stage control block whe the device 
 *                         and stage specific functions will be registered.
 *
 * Returns:
 *     nothing
 * Notes:
 */
STATIC void
_field_fb_functions_init(int unit, _field_funct_t *functions)
{
    functions->fp_detach               = _field_fb_detach;
    functions->fp_group_install        = _bcm_field_fb_group_install;
    functions->fp_selcodes_install     = _field_selcodes_install;
    functions->fp_slice_clear          = _bcm_field_fb_slice_clear;
    functions->fp_entry_remove         = _bcm_field_fb_entry_remove;
    functions->fp_entry_move           = _bcm_field_fb_entry_move;
    functions->fp_selcode_get          = _bcm_field_fb_selcode_get;
    functions->fp_selcode_to_qset      = _bcm_field_selcode_to_qset;
    functions->fp_qual_list_get        = _bcm_field_qual_lists_get;
    functions->fp_tcam_policy_clear    = _field_fb_tcam_policy_clear;
    functions->fp_tcam_policy_install  = _field_fb_tcam_policy_install;
    functions->fp_tcam_policy_reinstall = _field_fb_tcam_policy_reinstall;
    functions->fp_policer_install      = _bcm_field_fb_policer_install;
    functions->fp_write_slice_map      = _bcm_field_fb_write_slice_map;
    functions->fp_qualify_ip_type      = _field_fb_qualify_ip_type;
    functions->fp_qualify_ip_type_get  = _field_fb_qualify_ip_type_get;
    functions->fp_action_support_check = _field_fb_action_support_check;
    functions->fp_action_conflict_check = _field_fb_action_conflict_check;
    functions->fp_stat_index_get       = _bcm_field_fb_stat_index_get;
    functions->fp_action_params_check  = _field_fb_action_params_check;
    functions->fp_data_qualifier_ethertype_add = 
                       _bcm_field_fb_data_qualifier_ethertype_add;
    functions->fp_data_qualifier_ethertype_delete= 
                       _bcm_field_fb_data_qualifier_ethertype_delete;
    functions->fp_data_qualifier_ip_protocol_add = 
                       _bcm_field_fb_data_qualifier_ip_protocol_add;
    functions->fp_data_qualifier_ip_protocol_delete= 
                       _bcm_field_fb_data_qualifier_ip_protocol_delete;
    functions->fp_data_qualifier_packet_format_add= 
                       _bcm_field_fb_data_qualifier_packet_format_add;
    functions->fp_data_qualifier_packet_format_delete=
                       _bcm_field_fb_data_qualifier_packet_format_delete;

#if defined(BCM_BRADLEY_SUPPORT) 
    if (soc_feature(unit, soc_feature_two_ingress_pipes)) { 
        functions->fp_counter_get    = _field_br_counter_get;
        functions->fp_counter_set    = _field_br_counter_set;
    } else 
#endif /* BCM_BRADLEY_SUPPORT */
    {
        functions->fp_counter_get    = _bcm_field_fb_counter_get;
        functions->fp_counter_set    = _bcm_field_fb_counter_set;
    }

#if defined(BCM_FIREBOLT2_SUPPORT) 
    functions->fp_egress_key_match_type_set = _field_fb2_key_match_type_set;
#else  /* BCM_FIREBOLT2_SUPPORT */
    functions->fp_egress_key_match_type_set = NULL;
#endif  /* BCM_FIREBOLT2_SUPPORT */
    functions->fp_external_entry_install = NULL;
    functions->fp_external_entry_reinstall = NULL;
    functions->fp_external_entry_remove = NULL;
    functions->fp_external_entry_prio_set = NULL;
    functions->fp_stat_value_get = NULL;                          
    functions->fp_stat_value_set = NULL;
    functions->fp_control_set = _bcm_field_control_set;
    functions->fp_control_get = _bcm_field_control_get;
    functions->fp_stat_hw_mode_get = _bcm_field_stat_hw_mode_get;
    functions->fp_stat_hw_alloc = _bcm_field_stat_hw_alloc;
    functions->fp_stat_hw_free = _bcm_field_stat_hw_free;
    functions->fp_group_add = NULL;
    functions->fp_qualify_trunk = _bcm_field_qualify_trunk;
    functions->fp_qualify_trunk_get = _bcm_field_qualify_trunk_get;
    functions->fp_qualify_inports = _bcm_field_qualify_InPorts;
    functions->fp_entry_stat_extended_attach = NULL;
    functions->fp_entry_stat_extended_get = NULL;
    functions->fp_entry_stat_detach = _bcm_field_entry_stat_detach;
    functions->fp_class_size_get = NULL;
    functions->fp_qualify_packet_res = _field_qualify_PacketRes;
    functions->fp_qualify_packet_res_get = _field_qualify_PacketRes_get;
}
#else /*BCM_FIREBOLT_SUPPORT && BCM_FIELD_SUPPORT */ 
typedef int _firebolt_field_not_empty; /* Make ISO compilers happy. */
#endif  /* BCM_FIREBOLT_SUPPORT && BCM_FIELD_SUPPORT */
