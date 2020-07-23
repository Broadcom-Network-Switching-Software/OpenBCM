/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        l2.c
 * Purpose:     Triumph L2 function implementations
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <assert.h>

#include <sal/core/libc.h>

#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/hash.h>
#include <soc/l2x.h>
#include <soc/tomahawk3.h>
#include <soc/triumph.h>
#ifdef BCM_TRIDENT2_SUPPORT
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#ifdef BCM_TOMAHAWK_SUPPORT
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */

#include <bcm/l2.h>
#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l2.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/mpls.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/switch.h>
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_TRIDENT2_SUPPORT)
#include <bcm_int/esw/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <bcm_int/esw/tomahawk3.h>

#if defined(BCM_TRIUMPH_SUPPORT) /* BCM_TRIUMPH_SUPPORT */

#define GPORT_TYPE(_gport) (((_gport) >> _SHR_GPORT_TYPE_SHIFT) & _SHR_GPORT_TYPE_MASK)

#endif /* !BCM_TRIUMPH_SUPPORT */
#define DEFAULT_L2DELETE_CHUNKS		64	/* 8k entries / 64 = 512 */

typedef struct _bcm_mac_block_info_s {
    bcm_pbmp_t mb_pbmp;
    int ref_count;
} _bcm_mac_block_info_t;

static _bcm_mac_block_info_t *_mbi_entries[BCM_MAX_NUM_UNITS];
static int _mbi_num[BCM_MAX_NUM_UNITS];

#if defined(INCLUDE_L3)
extern int16 * _sc_subport_group_index[BCM_MAX_NUM_UNITS];
#define _SC_SUBPORT_NUM_PORT  (4096)
#define _SC_SUBPORT_NUM_GROUP (4096/8)
#define _SC_SUBPORT_VPG_FIND(unit, vp, grp) \
    do { \
         int ix; \
         grp = -1; \
         for (ix = 0; ix < _SC_SUBPORT_NUM_GROUP; ix++) { \
              if (_sc_subport_group_index[unit][ix] == vp) { \
                  grp = ix * 8; \
                  break;  \
              } \
         } \
       } while ((0))
#endif /* INCLUDE_L3 */

typedef enum _bcm_th3_l2x_memacc_field_e {
    _BCM_TH3_L2X_MEMACC_BASE_VALID_f,
    _BCM_TH3_L2X_MEMACC_KEY_TYPE_f,
    _BCM_TH3_L2X_MEMACC_MAC_ADDR_f,
    _BCM_TH3_L2X_MEMACC_VLAN_ID_f,
    _BCM_TH3_L2X_MEMACC_L2MC_PTR_f,
    _BCM_TH3_L2X_MEMACC_DEST_TYPE_f,
    _BCM_TH3_L2X_MEMACC_TGID_f,
    _BCM_TH3_L2X_MEMACC_PORT_NUM_f,
    _BCM_TH3_L2X_MEMACC_CLASS_ID_f,
    _BCM_TH3_L2X_MEMACC_MAC_BLOCK_INDEX_f,
    _BCM_TH3_L2X_MEMACC_PRI_f,
    _BCM_TH3_L2X_MEMACC_CPU_f,
    _BCM_TH3_L2X_MEMACC_DST_DISCARD_f,
    _BCM_TH3_L2X_MEMACC_SRC_DISCARD_f,
    _BCM_TH3_L2X_MEMACC_SCP_f,
    _BCM_TH3_L2X_MEMACC_STATIC_BIT_f,
    _BCM_TH3_L2X_MEMACC_HITDA_f,
    _BCM_TH3_L2X_MEMACC_HITSA_f,
    _BCM_TH3_L2X_MEMACC_LOCAL_SA_f,
    _BCM_TH3_L2X_MEMACC_DESTINATION_f,
    _BCM_TH3_L2X_MEMACC_T_f,
    _BCM_TH3_L2X_MEMACC_RPE_f,
    _BCM_TH3_L2X_MEMACC_RESERVED_KEY_PADDING_f,
    _BCM_TH3_L2X_MEMACC_NUM
} _bcm_th3_l2x_memacc_field_t;

static soc_memacc_t *_bcm_th3_l2x_memacc[BCM_UNITS_MAX];
static soc_field_t   _bcm_th3_l2x_fields[] = {
    BASE_VALIDf,
    KEY_TYPEf,
    MAC_ADDRf,
    VLAN_IDf,
    L2MC_PTRf,
    DEST_TYPEf,
    TGIDf,
    PORT_NUMf,
    CLASS_IDf,
    MAC_BLOCK_INDEXf,
    PRIf,
    CPUf,
    DST_DISCARDf,
    SRC_DISCARDf,
    SCPf,
    STATIC_BITf,
    HITDAf,
    HITSAf,
    LOCAL_SAf,
    DESTINATIONf,
    Tf,
    RPEf,
    RESERVED_KEY_PADDINGf 
};

#define _BCM_L2X_MEMACC_FIELD(_u_, _f_) \
            (&_bcm_th3_l2x_memacc[_u_][_BCM_TH3_L2X_MEMACC_##_f_])

#define _BCM_L2X_MEMACC_FIELD_VALID(_u_, _f_) \
            SOC_MEMACC_VALID(_BCM_L2X_MEMACC_FIELD(_u_, _f_))

#define _BCM_L2X_MEMACC_FIELD32_GET(_u_, _entBuf_, _f_) \
            soc_memacc_field32_get(_BCM_L2X_MEMACC_FIELD(_u_, _f_), _entBuf_)

#define _BCM_L2X_MEMACC_FIELD32_SET(_u_, _entBuf_, _f_, _v_) \
            soc_memacc_field32_set(_BCM_L2X_MEMACC_FIELD(_u_, _f_), _entBuf_, \
                                   _v_)

typedef enum _bcm_th3_l2hit_mem_e {
    _BCM_TH3_L2HITDA,
    _BCM_TH3_L2HITSA,
    _BCM_TH3_L2HIT_MEM_NUM
} _bcm_th3_l2hit_mem_t;

typedef enum _bcm_th3_l2hit_field_e {
    _BCM_TH3_L2HITDA_HITDA_f_0,
    _BCM_TH3_L2HITDA_HITDA_f_1,
    _BCM_TH3_L2HITDA_HITDA_f_2,
    _BCM_TH3_L2HITDA_HITDA_f_3,
    _BCM_TH3_L2HITSA_HITSA_f_0,
    _BCM_TH3_L2HITSA_HITSA_f_1,
    _BCM_TH3_L2HITSA_HITSA_f_2,
    _BCM_TH3_L2HITSA_HITSA_f_3,
    _BCM_TH3_L2HITSA_LOCAL_SA_f_0,
    _BCM_TH3_L2HITSA_LOCAL_SA_f_1,
    _BCM_TH3_L2HITSA_LOCAL_SA_f_2,
    _BCM_TH3_L2HITSA_LOCAL_SA_f_3,
    _BCM_TH3_L2HIT_FIELD_NUM
} _bcm_th3_l2hit_field_t;


static soc_memacc_t *_th3_l2hit_memacc[BCM_UNITS_MAX][_BCM_TH3_L2HIT_MEM_NUM];

#define _TH3_L2HIT_MEM(_u_, _m_, _p_) \
            (_BCM_TH3_##_m_ + (_p_))

#define _TH3_L2HIT_FIELD(_u_, _m_,_f_, _i_) \
            (_BCM_TH3_##_m_##_##_f_##_0 + (_i_))

#define _TH3_L2HIT_MEMACC_FIELD(_u_, _mem_, _fld_) \
            (&_th3_l2hit_memacc[_u_][_mem_][_fld_])

#define _TH3_L2HIT_MEMACC_FIELD32_GET(_u_, _mem_, _entBuf_, _fld_) \
            soc_memacc_field32_get(_TH3_L2HIT_MEMACC_FIELD(_u_, _mem_, _fld_), \
                                   _entBuf_)

STATIC int
_bcm_th3_l2_memacc_init(int unit)
{
    int rv, i;
    int alloc_size;
    int m, f;
    static soc_mem_t   _bcm_th3_l2hit_mems[] = {
        L2_HITDA_ONLYm,
        L2_HITSA_ONLYm,
    };
    static soc_field_t   _bcm_th3_l2hit_fields[] = {
        HITDA_0f, HITDA_1f, HITDA_2f, HITDA_3f,
        HITSA_0f, HITSA_1f, HITSA_2f, HITSA_3f,
        LOCAL_SA_0f, LOCAL_SA_1f, LOCAL_SA_2f, LOCAL_SA_3f,
    };

    if (_bcm_th3_l2x_memacc[unit] != NULL) {
        sal_free(_bcm_th3_l2x_memacc[unit]);
    }
    alloc_size = sizeof(soc_memacc_t) * _BCM_TH3_L2X_MEMACC_NUM;
    _bcm_th3_l2x_memacc[unit] = sal_alloc(alloc_size, "L2 memacc");
    if (_bcm_th3_l2x_memacc[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(_bcm_th3_l2x_memacc[unit], 0, alloc_size);

    for (i = 0; i < _BCM_TH3_L2X_MEMACC_NUM; i++) {
        rv = soc_memacc_init(unit, L2Xm, _bcm_th3_l2x_fields[i], 
                             &_bcm_th3_l2x_memacc[unit][i]);
        if (BCM_FAILURE(rv)) {
            SOC_MEMACC_INVALID_SET(&_bcm_th3_l2x_memacc[unit][i]);
        }
    }

    for (m = 0; m < _BCM_TH3_L2HIT_MEM_NUM; m++) {
        if (_th3_l2hit_memacc[unit][m] != NULL) {
            sal_free(_th3_l2hit_memacc[unit][m]);
        }
        alloc_size = sizeof(soc_memacc_t) * _BCM_TH3_L2HIT_FIELD_NUM;
        _th3_l2hit_memacc[unit][m] = sal_alloc(alloc_size, "L2hit memacc");
        if (_th3_l2hit_memacc[unit][m] == NULL) {
            sal_free(_bcm_th3_l2x_memacc[unit]);
            _bcm_th3_l2x_memacc[unit] = NULL;
            return BCM_E_MEMORY;
        }
        
        for (f = 0; f < _BCM_TH3_L2HIT_FIELD_NUM; f++) {
            rv = soc_memacc_init(unit, _bcm_th3_l2hit_mems[m], 
                                 _bcm_th3_l2hit_fields[f], 
                                 &_th3_l2hit_memacc[unit][m][f]);
            if (BCM_FAILURE(rv)) {
                SOC_MEMACC_INVALID_SET(&_th3_l2hit_memacc[unit][m][f]);
            }
        }
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_l2_memacc_deinit(int unit)
{
    int m;

    if (_bcm_th3_l2x_memacc[unit] != NULL) {
        sal_free(_bcm_th3_l2x_memacc[unit]);
        _bcm_th3_l2x_memacc[unit] = NULL;
    }

    for (m = 0; m < _BCM_TH3_L2HIT_MEM_NUM; m++) {
        if (_th3_l2hit_memacc[unit][m] != NULL) {
            sal_free(_th3_l2hit_memacc[unit][m]);
            _th3_l2hit_memacc[unit][m] = NULL;
        }
    }
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_mac_block_delete
 * Purpose:
 *      Remove reference to MAC_BLOCK table entry matching the given bitmap
 * Parameters:
 *      unit - Unit number
 *      mb_index - Index of MAC_BLOCK table with bitmap
 */
STATIC void
_bcm_th3_mac_block_delete(int unit, int mb_index)
{
    if (_mbi_entries[unit][mb_index].ref_count > 0) {
        _mbi_entries[unit][mb_index].ref_count--;
    } else if (mb_index) {
        /* Someone reran init without flushing L2 table */
    } /* else mb_index = 0, as expected for learning */
}

/*
 * Function:
 *      _bcm_th3_mac_block_insert
 * Purpose:
 *      Find or create a MAC_BLOCK table entry matching the given bitmap
 * Parameters:
 *      unit - Unit number
 *      mb_pbmp - egress port bitmap for source MAC blocking
 *      mb_index - (OUT) Index of MAC_BLOCK table with bitmap
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No more MAC_BLOCK entries available
 *      BCM_E_PARAM             Bad bitmap supplied
 */
STATIC int
_bcm_th3_mac_block_insert(int unit, bcm_pbmp_t mb_pbmp, int *mb_index)
{
    int cur_index = 0;
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    mac_block_entry_t mbe;
    bcm_pbmp_t temp_pbmp;

    /* Check for reasonable pbmp */
    BCM_PBMP_ASSIGN(temp_pbmp, mb_pbmp);
    BCM_PBMP_AND(temp_pbmp, PBMP_ALL(unit));
    if (BCM_PBMP_NEQ(mb_pbmp, temp_pbmp)) {
        return BCM_E_PARAM;
    }

    for (cur_index = 0; cur_index < _mbi_num[unit]; cur_index++) {
        if (BCM_PBMP_EQ(mbi[cur_index].mb_pbmp, mb_pbmp)) {
            mbi[cur_index].ref_count++;
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Not in table already, see if any space free */
    for (cur_index = 1; cur_index < _mbi_num[unit]; cur_index++) {
        if (mbi[cur_index].ref_count == 0) {
            /* Attempt insert */
            sal_memset(&mbe, 0, sizeof(mac_block_entry_t));

            soc_mem_pbmp_field_set(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                                   &mb_pbmp);

            SOC_IF_ERROR_RETURN(WRITE_MAC_BLOCKm(unit, MEM_BLOCK_ALL,
                                                 cur_index, &mbe));
            mbi[cur_index].ref_count++;
            BCM_PBMP_ASSIGN(mbi[cur_index].mb_pbmp, mb_pbmp);
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Didn't find a free slot, out of table space */
    return BCM_E_RESOURCE;
}


#if 0
/*
 * function:
 *     _bcm_tr_l2_traverse_mem
 * Description:
 *      Helper function to _bcm_esw_l2_traverse to itterate over given memory
 *      and actually read the table and parse entries for Triumph external
 *      memory
 * Parameters:
 *     unit         device number
 *      mem         External L2 memory to read
 *     trav_st      Traverse structure with all the data.
 * Return:
 *     BCM_E_XXX
 */
#ifdef BCM_TRIUMPH_SUPPORT
int
_bcm_tr_l2_traverse_mem(int unit, soc_mem_t mem, _bcm_l2_traverse_t *trav_st)
{
    _soc_tr_l2e_ppa_info_t    *ppa_info;
    ext_l2_entry_entry_t      ext_l2_entry;
    ext_l2_entry_tcam_entry_t tcam_entry;
    ext_l2_entry_data_entry_t data_entry;
    ext_src_hit_bits_l2_entry_t src_hit_entry;
    ext_dst_hit_bits_l2_entry_t dst_hit_entry;
    int                       src_hit, dst_hit;
    int                       idx, idx_max;

    if (mem != EXT_L2_ENTRYm) {
        return BCM_E_UNAVAIL;
    }

    if (!soc_mem_index_count(unit, mem)) {
        return BCM_E_NONE;
    }

    ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
    if (ppa_info == NULL) {
        return BCM_E_NONE;
    }

    idx_max = soc_mem_index_max(unit, mem);
    for (idx = soc_mem_index_min(unit, mem); idx <= idx_max; idx++ ) {
        if (!(ppa_info[idx].data & _SOC_TR_L2E_VALID)) {
            continue;
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EXT_L2_ENTRY_TCAMm, MEM_BLOCK_ANY, idx,
                          &tcam_entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EXT_L2_ENTRY_DATAm, MEM_BLOCK_ANY, idx,
                          &data_entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EXT_SRC_HIT_BITS_L2m, MEM_BLOCK_ANY, idx >> 5,
                          &src_hit_entry));
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, EXT_DST_HIT_BITS_L2m, MEM_BLOCK_ANY, idx >> 5,
                          &dst_hit_entry));
        src_hit = (soc_mem_field32_get
                   (unit, EXT_SRC_HIT_BITS_L2m, &src_hit_entry, SRC_HITf) >>
                   (idx & 0x1f)) & 1;
        dst_hit = (soc_mem_field32_get
                   (unit, EXT_DST_HIT_BITS_L2m, &dst_hit_entry, DST_HITf) >>
                   (idx & 0x1f)) & 1;
        BCM_IF_ERROR_RETURN
            (_bcm_tr_compose_ext_l2_entry(unit, &tcam_entry, &data_entry,
                                          src_hit, dst_hit, &ext_l2_entry));
        trav_st->data = (uint32 *)&ext_l2_entry;
        trav_st->mem = mem;

        BCM_IF_ERROR_RETURN(trav_st->int_cb(unit, trav_st));
    }

    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      _bcm_mac_block_insert
 * Purpose:
 *      Find or create a MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_pbmp - egress port bitmap for source MAC blocking
 *      mb_index - (OUT) Index of MAC_BLOCK table with bitmap.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No more MAC_BLOCK entries available
 *      BCM_E_PARAM             Bad bitmap supplied
 */
static int
_bcm_mac_block_insert(int unit, bcm_pbmp_t mb_pbmp, int *mb_index)
{
    int cur_index = 0;
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    mac_block_entry_t mbe;
    bcm_pbmp_t temp_pbmp;
#if defined(BCM_KATANA2_SUPPORT)
    uint32     fldbuf[SOC_PBMP_WORD_MAX];
    bcm_pbmp_t mb_pbmp0, mb_pbmp1;
    int        mask_w0_width = 0;
    int        mask_w1_width = 0;
    int        i = 0;

    SOC_PBMP_CLEAR(mb_pbmp0);
    SOC_PBMP_CLEAR(mb_pbmp1);
#endif

    /* Check for reasonable pbmp */
    BCM_PBMP_ASSIGN(temp_pbmp, mb_pbmp);
    BCM_PBMP_AND(temp_pbmp, PBMP_ALL(unit));
    if (BCM_PBMP_NEQ(mb_pbmp, temp_pbmp)) {
        return BCM_E_PARAM;
    }

    for (cur_index = 0; cur_index < _mbi_num[unit]; cur_index++) {
        if (BCM_PBMP_EQ(mbi[cur_index].mb_pbmp, mb_pbmp)) {
            mbi[cur_index].ref_count++;
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Not in table already, see if any space free */
    for (cur_index = 1; cur_index < _mbi_num[unit]; cur_index++) {
        if (mbi[cur_index].ref_count == 0) {
            /* Attempt insert */
            sal_memset(&mbe, 0, sizeof(mac_block_entry_t));

            if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_LOf)) {
                soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_LOf,
                                           SOC_PBMP_WORD_GET(mb_pbmp, 0));
            } else if (soc_mem_field_valid(unit, MAC_BLOCKm,
                                           MAC_BLOCK_MASK_W0f)) {
#if defined(BCM_KATANA2_SUPPORT)
                if (SOC_IS_KATANA2(unit)) {
                    mask_w0_width = soc_mem_field_length(unit, MAC_BLOCKm,
                                                         MAC_BLOCK_MASK_W0f);
                    for (i = 0; i < mask_w0_width; i++) {
                        if (SOC_PBMP_MEMBER(mb_pbmp, i)) {
                            SOC_PBMP_PORT_ADD(mb_pbmp0, i);
                        }
                    }

                    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
                    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                        fldbuf[i] = SOC_PBMP_WORD_GET(mb_pbmp0, i);
                    }
                    soc_MAC_BLOCKm_field_set(unit, &mbe,
                                             MAC_BLOCK_MASK_W0f, fldbuf);
                } else
#endif /* BCM_KATANA2_SUPPORT */
                {
                    soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_W0f,
                                           SOC_PBMP_WORD_GET(mb_pbmp, 0));
                }
            } else if (soc_mem_field_valid(unit, MAC_BLOCKm,
                                           MAC_BLOCK_MASKf)) {
                soc_mem_pbmp_field_set(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                                       &mb_pbmp); 
            } else {
                return BCM_E_INTERNAL;
            }
            if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_HIf)) {
                soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_HIf,
                                           SOC_PBMP_WORD_GET(mb_pbmp, 1));
            } else if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_W1f)) {
#if defined(BCM_KATANA2_SUPPORT)
                if (SOC_IS_KATANA2(unit)) {
                    mask_w1_width = soc_mem_field_length(unit, MAC_BLOCKm,
                                                         MAC_BLOCK_MASK_W1f);

                    for (i = mask_w0_width;
                         i < (mask_w0_width + mask_w1_width); i++) {
                        if (SOC_PBMP_MEMBER(mb_pbmp, i)) {
                            SOC_PBMP_PORT_ADD(mb_pbmp1, (i - mask_w0_width));
                        }
                    }

                    sal_memset(fldbuf, 0, SOC_PBMP_WORD_MAX * sizeof(uint32));
                    for (i = 0; i < SOC_PBMP_WORD_MAX; i++) {
                        fldbuf[i] = SOC_PBMP_WORD_GET(mb_pbmp1, i);
                    }
                    soc_MAC_BLOCKm_field_set(unit, &mbe,
                                             MAC_BLOCK_MASK_W1f, fldbuf);
                } else
#endif /* BCM_KATANA2_SUPPORT */
                {
                    soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_W1f,
                                           SOC_PBMP_WORD_GET(mb_pbmp, 1));
                }
            }
            if (soc_mem_field_valid(unit, MAC_BLOCKm, MAC_BLOCK_MASK_W2f)) {
#if defined(BCM_TRIDENT_SUPPORT)
                if (SOC_IS_TRIDENT(unit)) {
                    soc_MAC_BLOCKm_field32_set(unit, &mbe, MAC_BLOCK_MASK_W2f,
                                               SOC_PBMP_WORD_GET(mb_pbmp, 2));
                }
#endif
            }

            SOC_IF_ERROR_RETURN(WRITE_MAC_BLOCKm(unit, MEM_BLOCK_ALL,
                                                 cur_index, &mbe));
            mbi[cur_index].ref_count++;
            BCM_PBMP_ASSIGN(mbi[cur_index].mb_pbmp, mb_pbmp);
            *mb_index = cur_index;
            return BCM_E_NONE;
        }
    }

    /* Didn't find a free slot, out of table space */
    return BCM_E_RESOURCE;
}

/*
 * Function:
 *      _bcm_mac_block_delete
 * Purpose:
 *      Remove reference to MAC_BLOCK table entry matching the given bitmap.
 * Parameters:
 *      unit - Unit number
 *      mb_index - Index of MAC_BLOCK table with bitmap.
 */
static void
_bcm_mac_block_delete(int unit, int mb_index)
{
    if (_mbi_entries[unit][mb_index].ref_count > 0) {
        _mbi_entries[unit][mb_index].ref_count--;
    } else if (mb_index) {
        
        /* Someone reran init without flushing the L2 table */
    } /* else mb_index = 0, as expected for learning */
}

#endif /* 0 */


#if 0
#ifdef BCM_TRIUMPH_SUPPORT
STATIC int
_bcm_t_l2e_ppa_match(int unit, _bcm_l2_replace_t *rep_st)
{
    _soc_tr_l2e_ppa_info_t      *ppa_info;
    _soc_tr_l2e_ppa_vlan_t      *ppa_vlan;
    int                         i, imin, imax, rv, nmatches;
    soc_mem_t                   mem;
    uint32                      entdata, entmask, entvalue, newdata, newmask;
    ext_l2_entry_entry_t        l2entry, old_l2entry;
    int                         same_dest;

    ppa_info = SOC_CONTROL(unit)->ext_l2_ppa_info;
    ppa_vlan = SOC_CONTROL(unit)->ext_l2_ppa_vlan;
    if (ppa_info == NULL) {
        return BCM_E_NONE;
    }

    mem = EXT_L2_ENTRYm;
    imin = soc_mem_index_min(unit, mem);
    imax = soc_mem_index_max(unit, mem);

    /* convert match data */
    entdata = _SOC_TR_L2E_VALID;
    entmask = _SOC_TR_L2E_VALID;
    if (!(rep_st->flags & BCM_L2_REPLACE_MATCH_STATIC)) {
        entdata |= 0x00000000;
        entmask |= _SOC_TR_L2E_STATIC;
    }
    if (rep_st->flags & BCM_L2_REPLACE_MATCH_VLAN) {
        entdata |= (rep_st->key_vlan & _SOC_TR_L2E_VLAN_MASK) <<
            _SOC_TR_L2E_VLAN_SHIFT;
        entmask |= _SOC_TR_L2E_VLAN_MASK << _SOC_TR_L2E_VLAN_SHIFT;
        imin = ppa_vlan->vlan_min[rep_st->key_vlan];
        imax = ppa_vlan->vlan_max[rep_st->key_vlan];
    }
    if (rep_st->flags & BCM_L2_REPLACE_MATCH_DEST) {
        if (rep_st->match_dest.trunk != -1) {
            entdata |= _SOC_TR_L2E_T |
                ((rep_st->match_dest.trunk & _SOC_TR_L2E_TRUNK_MASK)
                 << _SOC_TR_L2E_TRUNK_SHIFT);
            entmask |= _SOC_TR_L2E_T |
                (_SOC_TR_L2E_TRUNK_MASK << _SOC_TR_L2E_TRUNK_SHIFT);
        } else {
            entdata |= 
                ((rep_st->match_dest.module & _SOC_TR_L2E_MOD_MASK) <<
                 _SOC_TR_L2E_MOD_SHIFT) |
                ((rep_st->match_dest.port & _SOC_TR_L2E_PORT_MASK) <<
                 _SOC_TR_L2E_PORT_SHIFT);
            entmask |= _SOC_TR_L2E_T |
                (_SOC_TR_L2E_MOD_MASK << _SOC_TR_L2E_MOD_SHIFT) |
                (_SOC_TR_L2E_PORT_MASK << _SOC_TR_L2E_PORT_SHIFT);
        }
    }

    nmatches = 0;

    if (imin >= 0) {
        soc_mem_lock(unit, mem);
        for (i = imin; i <= imax; i++) {
            entvalue = ppa_info[i].data;
            if ((entvalue & entmask) != entdata) {
                continue;
            }
            if (rep_st->flags & BCM_L2_REPLACE_MATCH_MAC) {
                if (ENET_CMP_MACADDR(rep_st->key_mac, ppa_info[i].mac)) {
                    continue;
                }
            }
            nmatches += 1;

            /* lookup the matched entry */
            sal_memset(&l2entry, 0, sizeof(l2entry));
            soc_mem_field32_set(unit, mem, &l2entry, VLAN_IDf,
                                (entvalue >> 16) & 0xfff);
            soc_mem_mac_addr_set(unit, mem, &l2entry, MAC_ADDRf,
                                 ppa_info[i].mac);

            /* operate on matched entry */
            if (rep_st->flags & BCM_L2_REPLACE_DELETE) {
                int             mb_index;

                /* coverity[value_overwrite] */
                rv = soc_mem_generic_delete(unit, mem, MEM_BLOCK_ANY, 0,
                                            &l2entry, &l2entry, NULL);
                if (rv < 0) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index = soc_mem_field32_get(unit, mem, &l2entry,
                                                   MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (entvalue & _SOC_TR_L2E_LIMIT_COUNTED) {
                    /* coverity[returned_value] */
                    rv = soc_triumph_learn_count_update(unit, &l2entry,
                                                        TRUE, -1);
                }
            } else {
                /* replace destination fields */
                /* coverity[value_overwrite] */
                rv = soc_mem_generic_lookup(unit, mem, MEM_BLOCK_ANY, 0,
                                            &l2entry, &l2entry, NULL);
                if (rep_st->flags & BCM_L2_REPLACE_NEW_TRUNK) {
                    newdata = _SOC_TR_L2E_T |
                        ((rep_st->new_dest.trunk & _SOC_TR_L2E_TRUNK_MASK) <<
                         _SOC_TR_L2E_TRUNK_SHIFT);
                    newmask = _SOC_TR_L2E_T |
                        (_SOC_TR_L2E_TRUNK_MASK << _SOC_TR_L2E_TRUNK_SHIFT);
                    soc_mem_field32_set(unit, mem, &l2entry, Tf, 1);
                    soc_mem_field32_set(unit, mem, &l2entry, TGIDf,
                                        rep_st->new_dest.trunk);
                } else {
                    newdata =
                        (rep_st->new_dest.module << _SOC_TR_L2E_MOD_SHIFT) |
                        (rep_st->new_dest.port << _SOC_TR_L2E_PORT_SHIFT);
                    newmask = _SOC_TR_L2E_T |
                        (_SOC_TR_L2E_MOD_MASK << _SOC_TR_L2E_MOD_SHIFT) |
                        (_SOC_TR_L2E_PORT_MASK << _SOC_TR_L2E_PORT_SHIFT);
                    soc_mem_field32_set(unit, mem, &l2entry, MODULE_IDf,
                                        rep_st->new_dest.module);
                    soc_mem_field32_set(unit, mem, &l2entry, PORT_NUMf,
                                        rep_st->new_dest.port);
                }
                same_dest = !((entvalue ^ newdata) & newmask);

                if ((entvalue & _SOC_TR_L2E_LIMIT_COUNTED) && !same_dest) {
                    rv = soc_triumph_learn_count_update(unit, &l2entry, FALSE,
                                                        1);
                    if (SOC_FAILURE(rv)) {
                        soc_mem_unlock(unit, mem);
                        return rv;
                    }
                }

                /* re-insert entry */
                rv = soc_mem_generic_insert(unit, mem, MEM_BLOCK_ANY, 0,
                                            &l2entry, &old_l2entry, NULL);
                if (rv == BCM_E_EXISTS) {
                    rv = BCM_E_NONE;
                }
                if (rv < 0) {
                    soc_mem_unlock(unit, mem);
                    return rv;
                }
                if ((entvalue & _SOC_TR_L2E_LIMIT_COUNTED) && !same_dest) {
                    rv = soc_triumph_learn_count_update(unit, &old_l2entry,
                                                        FALSE, -1);
                    if (SOC_FAILURE(rv)) {
                        soc_mem_unlock(unit, mem);
                        return rv;
                    }
                }
            }
        }
        soc_mem_unlock(unit, EXT_L2_ENTRYm);
    }
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "tr_l2e_ppa_match: imin=%d imax=%d nmatches=%d flags=0x%x\n"),
                 imin, imax, nmatches, rep_st->flags));
    return BCM_E_NONE;
}
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * Function:
 *      _bcm_l2_hash_dynamic_replace
 * Purpose:
 *      Replace dynamic L2 entries in a dual hash.
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - Entry to insert instead of dynamic entry.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX          		Error 
 */

int _bcm_l2_hash_dynamic_replace(int unit, l2x_entry_t *l2x_entry)
{
    l2x_entry_t     l2ent;
    int             rv;
    uint32          fval;
    bcm_mac_t       mac;
    int             cf_hit, cf_unhit;
    int             bank, bucket, slot, index;
    int             num_banks;
    int             entries_per_bank, entries_per_row, entries_per_bucket;
    int             bank_base, bucket_offset;


    
    BCM_IF_ERROR_RETURN(soc_trident2_hash_bank_count_get(unit, L2Xm, &num_banks));

    BCM_IF_ERROR_RETURN(soc_l2x_freeze(unit));

    cf_hit = cf_unhit = -1;
    for (bank = 0; bank < num_banks; bank++) {
        {
            
            rv = soc_trident2_hash_bank_info_get(unit, L2Xm, bank,
                                                 &entries_per_bank,
                                                 &entries_per_row,
                                                 &entries_per_bucket,
                                                 &bank_base, &bucket_offset);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }
            bucket = soc_td2_l2x_bank_entry_hash(unit, bank,
                                                 (uint32 *)l2x_entry);
        }


        for (slot = 0; slot < entries_per_bucket; slot++) {
            uint32 valid = 0;
            index = bank_base + bucket * entries_per_row + bucket_offset +
                slot;
            rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, index, &l2ent);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }

            if (soc_feature(unit, soc_feature_base_valid)) {
                valid = soc_L2Xm_field32_get(unit, &l2ent, BASE_VALIDf);
            } else {
                valid = soc_L2Xm_field32_get(unit, &l2ent, VALIDf);
            }
            if (!valid) {
                /* Found invalid entry - stop the search victim found */
                cf_unhit = index; 
                break;
            }

            fval = soc_mem_field32_get(unit, L2Xm, &l2ent, KEY_TYPEf);
#ifdef BCM_TRIDENT2_SUPPORT
            if (SOC_IS_TD2_TT2(unit)) {
                if (fval != TD2_L2_HASH_KEY_TYPE_BRIDGE &&
                    fval != TD2_L2_HASH_KEY_TYPE_VFI) {
                    continue;
                }
            } else
#endif /* BCM_TRIDENT2_SUPPORT */
            if (fval != TR_L2_HASH_KEY_TYPE_BRIDGE &&
                fval != TR_L2_HASH_KEY_TYPE_VFI) {
                continue;
            }

            soc_L2Xm_mac_addr_get(unit, &l2ent, MAC_ADDRf, mac);
            /* Skip static entries */
            if ((soc_L2Xm_field32_get(unit, &l2ent, STATIC_BITf)) ||
                (BCM_MAC_IS_MCAST(mac)) ||
                (soc_mem_field_valid(unit, L2Xm, L3f) && 
                 soc_L2Xm_field32_get(unit, &l2ent, L3f))) {
                continue;
            }

            if (soc_L2Xm_field32_get(unit, &l2ent, HITDAf) || 
                soc_L2Xm_field32_get(unit, &l2ent, HITSAf) ) {
                cf_hit =  index;
            } else {
                /* Found unhit entry - stop search victim found */
                cf_unhit = index;
                break;
            }
        }
        if (cf_unhit != -1) {
            break;
        }
    }

    COMPILER_REFERENCE(entries_per_bank);

    if (cf_unhit >= 0) {
        index = cf_unhit;   /* take last unhit dynamic */
    } else if (cf_hit >= 0) {
        index = cf_hit;     /* or last hit dynamic */
    } else {
        rv = BCM_E_FULL;     /* no dynamics to delete */
        (void) soc_l2x_thaw(unit);
         return rv;
    }

    rv = soc_mem_delete_index(unit, L2Xm, MEM_BLOCK_ALL, index);
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_generic_insert(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                    l2x_entry, NULL, NULL);
    }
    if (SOC_FAILURE(rv)) {
        (void) soc_l2x_thaw(unit);
        return rv;
    }

    return soc_l2x_thaw(unit);
}

/*
 * Function:
 *      bcm_bfd_l2_hash_dynamic_replace
 * Purpose:
 *      Replace dynamic L2 entries in a dual hash.
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - Entry to insert instead of dynamic entry.
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_XXX               Error
 */

int bcm_bfd_l2_hash_dynamic_replace(int unit, l2x_entry_t *l2x_entry)
{
    int rv;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        rv = bcm_tr3_bfd_l2_hash_dynamic_replace(unit, (void *)l2x_entry);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        rv = _bcm_l2_hash_dynamic_replace(unit, l2x_entry);
    }

    return rv;
}

#endif

#if defined(BCM_TRIDENT2_SUPPORT)
/*
 * Function:
 *      _bcm_th3_l2_hit_retrieve
 * Purpose:
 *      Retrieve the hit bit from both x and y pipe, then "or" the hitbit values and assign it to 
 *      L2_ENTRY data structure.
 * Parameters:
 *      unit        Unit number
 *      l2addr      (OUT) L2 API data structure
 *      l2x_entry   L2X entry
 *      l2_hw_index   L2X entry hardware index
 */
int
_bcm_th3_l2_hit_retrieve(int unit, l2x_entry_t *l2x_entry,
                                 int l2_hw_index)
{ 
    uint32 hit_sa, hit_da, hit_local_sa;          /* composite hit = hit_x|hit_y */    
    int idx_offset, hit_idx_shift;
    l2_hitsa_only_x_entry_t hit_sa_x;
    l2_hitsa_only_y_entry_t hit_sa_y;
    l2_hitda_only_x_entry_t hit_da_x;
    l2_hitda_only_y_entry_t hit_da_y;  
    soc_field_t hit_saf[] = { HITSA_0f, HITSA_1f, HITSA_2f, HITSA_3f};
    soc_field_t hit_daf[] = { HITDA_0f, HITDA_1f, HITDA_2f, HITDA_3f};   
    soc_field_t hit_local_saf[] = { LOCAL_SA_0f, LOCAL_SA_1f, 
                                    LOCAL_SA_2f, LOCAL_SA_3f};     

    idx_offset = (l2_hw_index & 0x3);
    hit_idx_shift = 2;

    /* Retrieve DA HIT */
    BCM_IF_ERROR_RETURN(
        BCM_XGS3_MEM_READ(unit, L2_HITDA_ONLY_Xm,
          (l2_hw_index >> hit_idx_shift), &hit_da_x));
    
    BCM_IF_ERROR_RETURN(
        BCM_XGS3_MEM_READ(unit, L2_HITDA_ONLY_Ym,
          (l2_hw_index >> hit_idx_shift), &hit_da_y)); 

    hit_da = 0;
    hit_da |= soc_mem_field32_get(unit, L2_HITDA_ONLY_Xm,
                                   &hit_da_x, hit_daf[idx_offset]);
    
    hit_da |= soc_mem_field32_get(unit, L2_HITDA_ONLY_Ym,
                                   &hit_da_y, hit_daf[idx_offset]);
    
    _BCM_L2X_MEMACC_FIELD32_SET(unit, l2x_entry, HITDA_f, hit_da);  

    /* Retrieve SA HIT */
    BCM_IF_ERROR_RETURN(
        BCM_XGS3_MEM_READ(unit, L2_HITSA_ONLY_Xm,
          (l2_hw_index >> hit_idx_shift), &hit_sa_x));
    
    BCM_IF_ERROR_RETURN(
        BCM_XGS3_MEM_READ(unit, L2_HITSA_ONLY_Ym,
          (l2_hw_index >> hit_idx_shift), &hit_sa_y)); 

    hit_sa = 0;
    hit_sa |= soc_mem_field32_get(unit, L2_HITSA_ONLY_Xm,
                                   &hit_sa_x, hit_saf[idx_offset]);
    
    hit_sa |= soc_mem_field32_get(unit, L2_HITSA_ONLY_Ym,
                                   &hit_sa_y, hit_saf[idx_offset]);
    
    _BCM_L2X_MEMACC_FIELD32_SET(unit, l2x_entry, HITSA_f, hit_sa);        
    
    /* Retrieve LOCAL_SA HIT */
    hit_local_sa = 0;
    hit_local_sa |= soc_mem_field32_get(unit, L2_HITSA_ONLY_Xm,
                                   &hit_sa_x, hit_local_saf[idx_offset]);
    
    hit_local_sa |= soc_mem_field32_get(unit, L2_HITSA_ONLY_Ym,
                                   &hit_sa_y, hit_local_saf[idx_offset]);
    
    _BCM_L2X_MEMACC_FIELD32_SET(unit, l2x_entry, LOCAL_SA_f, hit_local_sa);        
    
    return BCM_E_NONE;    
}

#endif /* BCM_TRIDENT2_SUPPORT */


#if 0
/*
 * Function:
 *      bcm_tr_l2_addr_add
 * Description:
 *      Add a MAC address to the Switch Address Resolution Logic (ARL)
 *      port with the given VLAN ID and parameters.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No MAC_BLOCK entries available
 * Notes:
 *      Use CMIC_PORT(unit) to associate the entry with the CPU.
 *      Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 *      In case the L2X table is full (e.g. bucket full), an attempt
 *      will be made to store the entry in the L2_USER_ENTRY table.
 */
int
bcm_tr_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    l2x_entry_t  l2x_entry, l2x_lookup;
#ifdef BCM_TRIUMPH_SUPPORT
    _soc_tr_l2e_ppa_info_t *ppa_info = NULL;
    ext_l2_entry_entry_t ext_l2_entry, ext_l2_lookup;
    int          exist_in_ext_l2 =0, limit_counted;
    int          rv1, ext_l2_index;
#endif /* BCM_TRIUMPH_SUPPORT */
    int          enable_ppa_war;
    int          rv, l2_index, mb_index = 0;

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit) || SOC_IS_KATANAX(unit) ||
         SOC_IS_TD2_TT2(unit)) {       
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm) &&
            (l2addr->flags & BCM_L2_L3LOOKUP)) {
            BCM_IF_ERROR_RETURN(bcm_td_l2_myStation_add(unit, l2addr));
        }
        else if (soc_mem_is_valid(unit, MY_STATION_TCAMm) &&
                !(l2addr->flags & BCM_L2_L3LOOKUP)) {
            /* If VPN specified do not perform myStation delete */
            if (!_BCM_VPN_VFI_IS_SET(l2addr->vid)) {
                rv = bcm_td_l2_myStation_delete(unit, 
                            l2addr->mac, l2addr->vid, &l2_index);
                if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
                    return rv;
                }
            }
        }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_l2x(unit, &l2x_entry, l2addr, FALSE));

    rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                &l2x_lookup, &l2_index);
    if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        /* Mac blocking, attempt to associate with bitmap entry */
        BCM_IF_ERROR_RETURN
            (_bcm_mac_block_insert(unit, l2addr->block_bitmap, &mb_index));
        soc_mem_field32_set(unit, L2Xm, &l2x_entry, MAC_BLOCK_INDEXf,
                            mb_index);
    }

    enable_ppa_war = FALSE;
    if (SOC_CONTROL(unit)->l2x_ppa_bypass == FALSE &&
        soc_feature(unit, soc_feature_ppa_bypass) &&
        soc_mem_field32_get(unit, L2Xm, &l2x_entry, KEY_TYPEf) !=
        TR_L2_HASH_KEY_TYPE_BRIDGE) {
        enable_ppa_war = TRUE;
    }

    rv = soc_mem_insert_return_old(unit, L2Xm, MEM_BLOCK_ANY, 
                                   (void *)&l2x_entry, NULL);
    if ((rv == BCM_E_FULL) && (l2addr->flags & BCM_L2_REPLACE_DYNAMIC)) {
        rv = _bcm_l2_hash_dynamic_replace( unit, &l2x_entry);
        if (rv < 0 ) {
            goto done;
        }
    } else if (rv == BCM_E_EXISTS) {
        if (!SOC_CONTROL(unit)->l2x_group_enable) {
            mb_index = soc_mem_field32_get(unit, L2Xm, &l2x_lookup,
                                           MAC_BLOCK_INDEXf);
            _bcm_mac_block_delete(unit, mb_index);
        }
        rv = BCM_E_NONE;
    }

    if (BCM_SUCCESS(rv) && enable_ppa_war) {
        SOC_CONTROL(unit)->l2x_ppa_bypass = TRUE;
    }

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_SUCCESS(rv)) {
        if (exist_in_ext_l2) {
            soc_mem_lock(unit, EXT_L2_ENTRYm);
            limit_counted =
                ppa_info[ext_l2_index].data & _SOC_TR_L2E_LIMIT_COUNTED;
            rv1 = soc_mem_generic_delete(unit, EXT_L2_ENTRYm, MEM_BLOCK_ANY,
                                         0, &ext_l2_entry, &ext_l2_lookup,
                                         &ext_l2_index);
            if (SOC_SUCCESS(rv1)) {
                if (!SOC_CONTROL(unit)->l2x_group_enable) {
                    mb_index =
                        soc_mem_field32_get(unit, EXT_L2_ENTRYm,
                                            &ext_l2_lookup, MAC_BLOCK_INDEXf);
                    _bcm_mac_block_delete(unit, mb_index);
                }
                if (limit_counted) {
                    (void)soc_triumph_learn_count_update(unit, &ext_l2_lookup,
                                                         TRUE, -1);
                }
            }
            soc_mem_unlock(unit, EXT_L2_ENTRYm);
        }
    }
#endif

done:
    if (rv < 0) {
        _bcm_mac_block_delete(unit, mb_index);
    }

    return rv;
}

/*
 * Function:
 *      bcm_tr_l2_addr_delete
 * Description:
 *      Delete an L2 address (MAC+VLAN) from the device
 * Parameters:
 *      unit - device unit
 *      mac  - MAC address to delete
 *      vid  - VLAN id
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tr_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_addr_t  l2addr;
    l2x_entry_t    l2x_entry, l2x_lookup;
    int            l2_index, mb_index;
    int            rv;
    soc_control_t  *soc = SOC_CONTROL(unit);

    bcm_l2_addr_t_init(&l2addr, mac, vid);

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit) || (SOC_IS_KATANAX(unit)) ||
        SOC_IS_TD2_TT2(unit)) {
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm) && BCM_VLAN_VALID(vid)) {
              rv = bcm_td_l2_myStation_delete (unit, mac, vid, &l2_index);
            if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FULL)) {
                   if (rv != BCM_E_NONE) {
                        return rv;
                   }
              }
         }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    BCM_IF_ERROR_RETURN(_bcm_tr_l2_to_l2x(unit, &l2x_entry, &l2addr, TRUE));

    soc_mem_lock(unit, L2Xm);

    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index,
                       (void *)&l2x_entry, (void *)&l2x_lookup, 0);
    if (BCM_E_NONE != rv) {
        soc_mem_unlock(unit, L2Xm);
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        mb_index = soc_L2Xm_field32_get(unit, &l2x_lookup, MAC_BLOCK_INDEXf);
        _bcm_mac_block_delete(unit, mb_index);
    }

    if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
        soc_mem_unlock(unit, L2Xm);
        return BCM_E_RESOURCE;
    }
    rv = soc_mem_delete_return_old(unit, L2Xm, MEM_BLOCK_ANY,
                                   (void *)&l2x_entry, (void *)&l2x_entry);
    if (rv >= 0) {
        rv = soc_l2x_sync_delete(unit, (uint32 *) &l2x_lookup, l2_index, 0);
    }
    SOC_L2_DEL_SYNC_UNLOCK(soc);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *      bcm_tr_l2_addr_get
 * Description:
 *      Given a MAC address and VLAN ID, check if the entry is present
 *      in the L2 table, and if so, return all associated information.
 * Parameters:
 *      unit - Device unit number
 *      mac - input MAC address to search
 *      vid - input VLAN ID to search
 *      check_l2_only - only check if the target exist in L2 table.
 *      l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *      BCM_E_NONE              Success (l2addr filled in)
 *      BCM_E_PARAM             Illegal parameter (NULL pointer)
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_NOT_FOUND Address not found (l2addr not filled in)
 */

int
_bcm_tr_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
    int check_l2_only, bcm_l2_addr_t *l2addr)
{
    bcm_l2_addr_t l2addr_key;
    l2x_entry_t  l2x_entry, l2x_lookup;
    int          l2_hw_index;
    int          rv;

    bcm_l2_addr_t_init(&l2addr_key, mac, vid);

#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TRIDENT(unit) || SOC_IS_KATANAX(unit) ||       
        SOC_IS_TD2_TT2(unit)) {
        if (soc_mem_is_valid(unit, MY_STATION_TCAMm) && (!check_l2_only)) {
              rv = bcm_td_l2_myStation_get (unit, mac, vid, l2addr);
              if (BCM_SUCCESS(rv)) {
                   return rv;
              }
         }
    }
#endif /* BCM_TRIDENT_SUPPORT */

    BCM_IF_ERROR_RETURN(
        _bcm_tr_l2_to_l2x(unit, &l2x_entry, &l2addr_key, TRUE));

    soc_mem_lock(unit, L2Xm);

    rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                &l2x_lookup, &l2_hw_index);

    soc_mem_unlock(unit, L2Xm);

    if (check_l2_only) {
        return rv;
    }

    if (SOC_SUCCESS(rv)) {
        /* Retrieve the hit bit for TD2/TT2 */
#ifdef BCM_TRIDENT2_SUPPORT
        if (!SOC_IS_TOMAHAWK(unit) && !SOC_IS_APACHE(unit) &&
            !SOC_IS_TRIDENT3(unit) &&
                SOC_IS_TD2_TT2(unit)) {
            BCM_IF_ERROR_RETURN(
                _bcm_th3_l2_hit_retrieve(unit, &l2x_lookup, l2_hw_index));
        }
#endif /* BCM_TRIDENT2_SUPPORT */
        rv = _bcm_tr_l2_from_l2x(unit, l2addr, &l2x_lookup);
    }

    return rv;
}

int
bcm_tr_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
                   bcm_l2_addr_t *l2addr)
{
    return _bcm_tr_l2_addr_get(unit, mac, vid, FALSE, l2addr);
}

#endif

#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      _bcm_th3_l2_reload_mbi
 * Description:
 *      Load MAC block info from hardware into software data structures
 * Parameters:
 *      unit - StrataSwitch unit number
 */
int
_bcm_th3_l2_reload_mbi(int unit)
{
    _bcm_mac_block_info_t *mbi = _mbi_entries[unit];
    l2x_entry_t         *l2x_entry, *l2x_table;
    mac_block_entry_t   mbe;
    int                 index, mb_index, l2x_size;
    pbmp_t              mb_pbmp;

    /*
     * Refresh MAC Block information from the hardware tables
     */

    for (mb_index = 0; mb_index < _mbi_num[unit]; mb_index++) {
        SOC_IF_ERROR_RETURN
            (READ_MAC_BLOCKm(unit, MEM_BLOCK_ANY, mb_index, &mbe));

        SOC_PBMP_CLEAR(mb_pbmp);

        soc_mem_pbmp_field_set(unit, MAC_BLOCKm, &mbe, MAC_BLOCK_MASKf,
                               &mb_pbmp); 

        BCM_PBMP_ASSIGN(mbi[mb_index].mb_pbmp, mb_pbmp);
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        l2x_size = sizeof(l2x_entry_t) * soc_mem_index_count(unit, L2Xm);
        l2x_table = soc_cm_salloc(unit, l2x_size, "l2 reload");
        if (l2x_table == NULL) {
            return BCM_E_MEMORY;
        }

        memset((void *)l2x_table, 0, l2x_size);
        if (soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                               soc_mem_index_min(unit, L2Xm),
                               soc_mem_index_max(unit, L2Xm),
                               l2x_table) < 0) {
            soc_cm_sfree(unit, l2x_table);
            return SOC_E_INTERNAL;
        }

        for (index = soc_mem_index_min(unit, L2Xm);
             index <= soc_mem_index_max(unit, L2Xm); index++) {

             l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm,
                                                      l2x_entry_t *,
                                                      l2x_table, index);

             if (!soc_L2Xm_field32_get(unit, l2x_entry, BASE_VALIDf)) {
                 continue;
             }
  
             mb_index = soc_L2Xm_field32_get(unit, l2x_entry, MAC_BLOCK_INDEXf);
             mbi[mb_index].ref_count++;
        }
        soc_cm_sfree(unit, l2x_table);
    }

    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _bcm_th3_l2x_delete_all
 * Purpose:
 *      Clear L2 table by invalidating entries
 * Parameters:
 *      unit - StrataSwitch unit #
 * Returns:
 *      SOC_E_XXX
 */

STATIC int
_bcm_th3_l2x_delete_all(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int copyno;
    int index_min, index_max, index, mem_max;
    l2_entry_only_single_entry_t *l2x_entry;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    int mem_size, idx;
    int modified;
    uint32 key_type;

    index_min = soc_mem_index_min(unit, L2Xm);
    mem_max = soc_mem_index_max(unit, L2Xm);
    mem_size =  DEFAULT_L2DELETE_CHUNKS * sizeof(l2_entry_only_single_entry_t);
    
    buffer = soc_cm_salloc(unit, mem_size, "L2_ENTRY_ONLY_SINGLE_delete");
    if (buffer == NULL) {
        return SOC_E_MEMORY;
    }

    /* Lock on aggregate entry */
    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += DEFAULT_L2DELETE_CHUNKS) {
        index_max = idx + DEFAULT_L2DELETE_CHUNKS - 1;
        if (index_max > mem_max) {
            index_max = mem_max;
        }

        if ((rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }

        modified = FALSE;
        for (index = 0; index < DEFAULT_L2DELETE_CHUNKS; index++) {
            l2x_entry =
                soc_mem_table_idx_to_pointer(unit, L2Xm,
                                             l2_entry_only_single_entry_t *,
                                             buffer, index);
            if (!soc_L2Xm_field32_get(unit, l2x_entry, BASE_VALIDf)) {
                continue;
            }

            key_type = soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf);
            if (key_type == TH3_L2_HASH_KEY_TYPE_BRIDGE) {
                sal_memcpy(l2x_entry, soc_mem_entry_null(unit, L2Xm),
                           sizeof(l2_entry_only_single_entry_t));
                modified = TRUE;
            }
        }

        if (!modified) {
            continue;
        }

        if ((rv = soc_mem_write_range(unit, L2Xm,
                                      MEM_BLOCK_ALL, idx, index_max, buffer))
                                      < 0) {
            soc_cm_sfree(unit, buffer);
            soc_mem_unlock(unit, L2Xm);
            return rv;
        }
    }

    soc_cm_sfree(unit, buffer);

    /* Clear all shadow table entries */
    if (soc->arlShadow != NULL) {
        sal_mutex_take(soc->arlShadowMutex, sal_mutex_FOREVER);
        (void) shr_avl_delete_all(soc->arlShadow);
        sal_mutex_give(soc->arlShadowMutex);
    }

    /* Free AVL table used for L2 learning */
    if (soc->l2x_lrn_shadow != NULL) {
        sal_mutex_take(soc->l2x_lrn_shadow_mutex, sal_mutex_FOREVER);
        shr_avl_delete_all(soc->l2x_lrn_shadow);
        sal_mutex_give(soc->l2x_lrn_shadow_mutex);
    }

    copyno = SOC_MEM_BLOCK_ANY(unit, L2Xm);
    if (SOC_MEM_BLOCK_VALID(unit, L2Xm, copyno)) {
        SOP_MEM_STATE(unit, L2Xm).count[copyno] = 0;
    }

    soc_mem_unlock(unit, L2Xm);

    (void)soc_th3_l2x_appl_callback_entry_delete_all(unit);

    return rv;
}



/*
 * Function:
 *      bcm_th3_l2_init
 * Description:
 *      Initialize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number
 */

int
bcm_th3_l2_init(int unit)
{
    int         rv, was_running = FALSE;
    uint32      flags;
    sal_usecs_t interval = 0;
    sal_usecs_t lrn_interval;
    int lrn_thread_running = FALSE;

    if (soc_l2x_running(unit, &flags, &interval)) {
        was_running = TRUE;
        BCM_IF_ERROR_RETURN(soc_l2x_stop(unit));
    }

    if (soc_th3_l2_learn_thread_running(unit, &lrn_interval)) {
        lrn_thread_running = TRUE;
        BCM_IF_ERROR_RETURN(soc_th3_l2_learn_thread_stop(unit));
    }

    if (!SOC_WARM_BOOT(unit) && !SOC_IS_RCPU_ONLY(unit)) {

        if (!(SAL_BOOT_QUICKTURN || 
             ((SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM || SAL_BOOT_XGSSIM) &&
              !SOC_IS_TOMAHAWKX(unit)))) {
            _bcm_th3_l2x_delete_all(unit);
        }
    }

    if (_mbi_entries[unit] != NULL) {
        sal_free(_mbi_entries[unit]);
        _mbi_entries[unit] = NULL;
    }

    _mbi_num[unit] = (SOC_MEM_INFO(unit, MAC_BLOCKm).index_max -
                      SOC_MEM_INFO(unit, MAC_BLOCKm).index_min + 1);
    _mbi_entries[unit] = sal_alloc(_mbi_num[unit] *
                                   sizeof(_bcm_mac_block_info_t),
                                   "BCM L2X MAC blocking info");
    if (!_mbi_entries[unit]) {
        return BCM_E_MEMORY;
    }
    sal_memset(_mbi_entries[unit], 0, _mbi_num[unit] * sizeof(_bcm_mac_block_info_t));

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_init(unit));

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        BCM_IF_ERROR_RETURN(_bcm_th3_l2_reload_mbi(unit));

#if defined(BCM_TRIUMPH_SUPPORT)
        BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_reload(unit));
#endif
    }
#endif

    BCM_IF_ERROR_RETURN(_bcm_l2_match_ctrl_init(unit));

    rv = _bcm_th3_l2_memacc_init(unit);
    if (BCM_FAILURE(rv)) {
        _bcm_l2_match_ctrl_deinit(unit);
        return rv;
    }

    /* bcm_l2_register clients */
    soc_l2x_register(unit, _bcm_th3_l2_register_callback, NULL);

    if (was_running) {
        /* Use value saved in soc_control[]. Original code running on
         * bcmsim sets it back to default value
         * 'interval' should be non-0 if the task was running originally
         */
        soc_l2x_start(unit, flags, interval);
    }

    if (lrn_thread_running) {
        /* Use value saved in soc_control[], no point setting it back to
         * default value
         * 'lrn_interval' should be non-0 if the task was running originally
         */
        soc_th3_l2_learn_thread_start(unit, lrn_interval);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_l2_term
 * Description:
 *      Finalize chip-dependent parts of L2 module
 * Parameters:
 *      unit - StrataSwitch unit number.
 */

int
bcm_th3_l2_term(int unit)
{
    if (_mbi_entries[unit] != NULL) {
        sal_free(_mbi_entries[unit]);
        _mbi_entries[unit] = NULL;
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_station_control_deinit(unit));

    /* Free resources allocated for match control data */
    _bcm_l2_match_ctrl_deinit(unit);

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_memacc_deinit(unit));

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_dual_l2_conflict_get(int unit, bcm_l2_addr_t *addr,
                              bcm_l2_addr_t *cf_array, int cf_max,
                              int *cf_count)
{
    l2x_entry_t     match_entry, entry;
    uint32          fval;
    int             bank, bucket, slot, index;
    int             num_banks;
    int             entries_per_bank, entries_per_row, entries_per_bucket;
    int             bank_base, bucket_offset;


    BCM_IF_ERROR_RETURN(soc_tomahawk3_hash_bank_count_get(unit, L2Xm,
                                                              &num_banks));

    *cf_count = 0;

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_to_l2x(unit, &match_entry, addr, TRUE));
    for (bank = 0; bank < num_banks; bank++) {

        BCM_IF_ERROR_RETURN(soc_tomahawk3_hash_bank_info_get(unit,
                                L2Xm,
                                bank,
                                &entries_per_bank,
                                &entries_per_row,
                                &entries_per_bucket,
                                &bank_base, &bucket_offset));

        bucket = soc_tomahawk3_l2x_bank_entry_hash(unit, bank,
                                            (uint32 *)&match_entry);

        for (slot = 0; slot < entries_per_bucket; slot++) {
            index = bank_base + bucket * entries_per_row + bucket_offset +
                slot;

            LOG_DEBUG(BSL_LS_BCM_L2,
                      (BSL_META_U(unit,
                      "bucket=%d, index=%d) \n"), bucket, index));

            SOC_IF_ERROR_RETURN(soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, index,
                                             &entry));

            if (!soc_L2Xm_field32_get(unit, &entry, BASE_VALIDf)) {
                continue;
            }

            fval = soc_mem_field32_get(unit, L2Xm, &entry, KEY_TYPEf);

            if (fval != TH3_L2_HASH_KEY_TYPE_BRIDGE) {
                continue;
            }

            BCM_IF_ERROR_RETURN(_bcm_th3_l2_from_l2x(unit,
                                    &cf_array[*cf_count],
                                    &entry));

            *cf_count += 1;
            if (*cf_count >= cf_max) {
                return BCM_E_NONE;
            }
        }
    }

    COMPILER_REFERENCE(entries_per_bank);

    return BCM_E_NONE;
}

int
bcm_th3_l2_conflict_get(int unit, bcm_l2_addr_t *addr, bcm_l2_addr_t *cf_array,
                        int cf_max, int *cf_count)
{

    if (soc_feature(unit, soc_feature_dual_hash)) {
        return _bcm_th3_dual_l2_conflict_get(unit, addr, cf_array,
                                             cf_max, cf_count);
    }

    return BCM_E_UNAVAIL;
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_th3_l2_sw_dump
 * Purpose:
 *     Displays L2 information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_th3_l2_sw_dump(int unit)
{
    _bcm_mac_block_info_t *mbi;
     char                 pfmt[SOC_PBMP_FMT_LEN];
     int                  i;

    LOG_CLI((BSL_META_U(unit,
                        "\n")));
    LOG_CLI((BSL_META_U(unit,
                        "  TR L2 MAC Blocking Info -\n")));
    LOG_CLI((BSL_META_U(unit,
                        "      Number : %d\n"), _mbi_num[unit]));

    mbi = _mbi_entries[unit];
    LOG_CLI((BSL_META_U(unit,
                        "      Entries (index: pbmp-count) :\n")));
    if (mbi != NULL) {
        for (i = 0; i < _mbi_num[unit]; i++) {
            SOC_PBMP_FMT(mbi[i].mb_pbmp, pfmt);
            LOG_CLI((BSL_META_U(unit,
                                "          %5d: %s-%d\n"), i, pfmt, mbi[i].ref_count));
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "\n")));

    LOG_CLI((BSL_META_U(unit,
                        "\n  TR L2 PPA bypass - %s\n"),
             SOC_CONTROL(unit)->l2x_ppa_bypass ? "TRUE" : "FALSE"));
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/*
 * Function:
 *	_bcm_th3_l2_gport_parse
 * Description:
 *	Parse gport in the l2 table
 * Parameters:
 *	unit -      [IN] StrataSwitch PCI device unit number (driver internal).
 *  l2addr -    [IN/OUT] l2 addr structure to parse and fill
 *  sub_port -  [OUT] Virtual port group (-1 if not supported)
 *  mpls_port - [OUT] MPLS port (-1 if not supported)
 * Returns:
 *	BCM_E_XXX
 */
int
_bcm_th3_l2_gport_parse(int unit, bcm_l2_addr_t *l2addr,
                        _bcm_l2_gport_params_t *params)
{
    int             id;
    bcm_port_t      _port;
    bcm_trunk_t     _trunk;
    bcm_module_t    _modid, _mymodid;

    if ((NULL == l2addr) || (NULL == params)){
        return BCM_E_PARAM;
    }

    params->param0 = -1;
    params->param1 = -1;
    params->type = 0;

    
    BCM_IF_ERROR_RETURN(
        _bcm_esw_gport_resolve(unit, l2addr->port, &_modid, &_port,
                               &_trunk, &id));

    _mymodid = 0;

    if ((BCM_TRUNK_INVALID != _trunk) && (-1 == id)) {
        params->param0 = _trunk;
        params->type = _SHR_GPORT_TYPE_TRUNK;
        return BCM_E_NONE;
    }

    if ((-1 != _port) && (-1 == id)) {
        params->param0 = _port;
        if ((_port == CMIC_PORT(unit)) && (_modid == _mymodid)) {
            params->type = _SHR_GPORT_TYPE_LOCAL_CPU;
            return BCM_E_NONE;
        }
        params->param1 = _modid;
        params->type = _SHR_GPORT_TYPE_MODPORT;
        return BCM_E_NONE;
    }

    if (-1 != id) {
        params->param0 = id;
        /* Check if the settings below apply to TH3 L2 */
        if (BCM_GPORT_IS_MPLS_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_MPLS_PORT;
        } else if (BCM_GPORT_IS_VLAN_PORT(l2addr->port)){
            params->type = _SHR_GPORT_TYPE_VLAN_PORT;
        }
        return BCM_E_NONE;
    } else if (BCM_GPORT_IS_BLACK_HOLE(l2addr->port)) {
       params->type = _SHR_GPORT_TYPE_BLACK_HOLE;
       params->param0 = 0;
       params->param1 = id;

       return BCM_E_NONE;
    }

    return BCM_E_PORT;
}

/*
 * Function:
 *      _bcm_th3_l2_to_l2x
 * Purpose:
 *      Convert an L2 API data structure to a Triumph L2X entry
 * Parameters:
 *      unit        Unit number
 *      l2x_entry   (OUT) Triumph L2X entry
 *      l2addr      L2 API data structure
 *      key_only    Only construct key portion
 */
int
_bcm_th3_l2_to_l2x(int unit, l2x_entry_t *l2x_entry, bcm_l2_addr_t *l2addr,
                   int key_only)
{
    bcm_cos_t cos_dst = l2addr->cos_dst;
    int group = l2addr->group;

    if (l2addr->cos_dst < 0 || l2addr->cos_dst > 15) {
        return BCM_E_PARAM;
    }

    /*  BCM_L2_MIRROR is not supported starting from Triumph */
    if (l2addr->flags & BCM_L2_MIRROR) {
        return BCM_E_PARAM;
    }

    /* Setting both flags is not a valid configuration. */
    if (l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT &&
        l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL) {
        return BCM_E_PARAM;
    }

    sal_memset(l2x_entry, 0, sizeof (*l2x_entry));

    VLAN_CHK_ID(unit, l2addr->vid);
    soc_L2Xm_field32_set(unit, l2x_entry, VLAN_IDf, l2addr->vid);
    soc_L2Xm_field32_set(unit, l2x_entry, KEY_TYPEf,
                             TH3_L2_HASH_KEY_TYPE_BRIDGE);

    soc_L2Xm_mac_addr_set(unit, l2x_entry, MAC_ADDRf, l2addr->mac);

    if (key_only) {
        return BCM_E_NONE;
    }

    if (BCM_MAC_IS_MCAST(l2addr->mac)) {
        if (l2addr->l2mc_group < 0) {
            return BCM_E_PARAM;
        }

        if (_BCM_MULTICAST_IS_SET(l2addr->l2mc_group)) {
            soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf,
                _BCM_MULTICAST_ID_GET(l2addr->l2mc_group));
        } else {
            soc_L2Xm_field32_set(unit, l2x_entry, L2MC_PTRf,
                                 l2addr->l2mc_group);
        }
    } else {
        bcm_port_t      port = -1;
        bcm_trunk_t     tgid = BCM_TRUNK_INVALID;
        bcm_module_t    modid = -1;
        int             gport_id = -1;

        if (BCM_GPORT_IS_SET(l2addr->port)) {
            _bcm_l2_gport_params_t  g_params;

            if (BCM_GPORT_IS_BLACK_HOLE(l2addr->port)) {
                soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 1);
            } else {
                soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 0);
                BCM_IF_ERROR_RETURN(
                        _bcm_th3_l2_gport_parse(unit, l2addr, &g_params));

                switch (g_params.type) {
                    case _SHR_GPORT_TYPE_TRUNK:
                        tgid = g_params.param0;
                        break;
                    case  _SHR_GPORT_TYPE_MODPORT:
                        port = g_params.param0;
                        modid = g_params.param1;
                        break;
                    case _SHR_GPORT_TYPE_LOCAL_CPU:
                        port = g_params.param0;
                        BCM_IF_ERROR_RETURN(
                                bcm_esw_stk_my_modid_get(unit, &modid));
                        break;
                    case _SHR_GPORT_TYPE_MPLS_PORT:
                        gport_id = g_params.param0;
                        break;
                    case _SHR_GPORT_TYPE_VLAN_PORT:
                        gport_id = g_params.param0;
                        break;
                    default:
                        return BCM_E_PORT;
                }
            }
        } else if (l2addr->flags & BCM_L2_TRUNK_MEMBER) {
            tgid = l2addr->tgid;
        } else {
            /* Check parameters */
            if (!SOC_MODID_ADDRESSABLE(unit, l2addr->modid)) {
                return BCM_E_BADID;
            }
            modid = l2addr->modid; /* Should be zero */
            if (!SOC_PORT_ADDRESSABLE(unit, l2addr->port)) {
                return BCM_E_PORT;
            }
            port = l2addr->port;
        }

        /* Setting l2x_entry fields according to parameters */
        if (BCM_TRUNK_INVALID != tgid) {
            soc_L2Xm_field32_set(unit, l2x_entry, Tf, 1);
            soc_L2Xm_field32_set(unit, l2x_entry, TGIDf, tgid);
            soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x1);

            if (l2addr->flags & BCM_L2_REMOTE_TRUNK) {
                /* Note REMOTE_TRUNK is not supported.
                 * Remove this if after top-level check
                 */
            }
        } else if (-1 != port) {
            soc_L2Xm_field32_set(unit, l2x_entry, PORT_NUMf, port);
            soc_L2Xm_field32_set(unit, l2x_entry, DEST_TYPEf, 0x0);
        } else if (-1 != gport_id) {
                
                soc_L2Xm_field32_set(unit, l2x_entry, DESTINATIONf, gport_id);
        }
    }

    if (SOC_L2X_GROUP_ENABLE_GET(unit)) {
        if (soc_feature(unit, soc_feature_overlaid_address_class) &&
            !BCM_L2_PRE_SET(l2addr->flags)) {
            if ((l2addr->group > SOC_OVERLAID_ADDR_CLASS_MAX(unit)) ||
                (l2addr->group < 0)) {
                return BCM_E_PARAM;
            }
            cos_dst = (l2addr->group & 0x3C0) >> 6;
            group = l2addr->group & 0x3F;
        }
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        soc_L2Xm_field32_set(unit, l2x_entry, CLASS_IDf, group);
    } /* else MAC_BLOCK_INDEXf is handled in the add/remove functions below */

    if (l2addr->flags & BCM_L2_SETPRI) {
        soc_L2Xm_field32_set(unit, l2x_entry, RPEf, 1);
    }

    if (!SOC_IS_TRIUMPH3(unit)) {
        soc_L2Xm_field32_set(unit, l2x_entry, PRIf, cos_dst);
    }

    if (l2addr->flags & BCM_L2_COPY_TO_CPU) {
        soc_L2Xm_field32_set(unit, l2x_entry, CPUf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_DST) {
        soc_L2Xm_field32_set(unit, l2x_entry, DST_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_DISCARD_SRC) {
        soc_L2Xm_field32_set(unit, l2x_entry, SRC_DISCARDf, 1);
    }

    if (l2addr->flags & BCM_L2_COS_SRC_PRI) {
        soc_L2Xm_field32_set(unit, l2x_entry, SCPf, 1);
    }

    if (l2addr->flags & BCM_L2_PENDING) {
        /* Not supported - relevant h/w learn support is absent in TH3 */
    }

    if (l2addr->flags & BCM_L2_STATIC) {
        soc_L2Xm_field32_set(unit, l2x_entry, STATIC_BITf, 1);
    }

    soc_L2Xm_field32_set(unit, l2x_entry, BASE_VALIDf, 1);

    if ((l2addr->flags & BCM_L2_DES_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        soc_L2Xm_field32_set(unit, l2x_entry, HITDAf, 1);
    }

    if ((l2addr->flags & BCM_L2_SRC_HIT) ||
        (l2addr->flags & BCM_L2_HIT)) {
        soc_L2Xm_field32_set(unit, l2x_entry, HITSAf, 1);
    }

    if (SOC_MEM_FIELD_VALID(unit, L2Xm, LOCAL_SAf)) {
        if (l2addr->flags & BCM_L2_NATIVE) {
            soc_L2Xm_field32_set(unit, l2x_entry, LOCAL_SAf, 1);
        }
    }

    if (l2addr->flags & BCM_L2_LEARN_LIMIT_EXEMPT_LOCAL) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_l2_from_l2x
 * Purpose:
 *      Convert a L2X entry to an L2 API data structure
 * Parameters:
 *      unit        Unit number
 *      l2addr      (OUT) L2 API data structure
 *      l2x_entry   Triumph L2X entry
 */
int
_bcm_th3_l2_from_l2x(int unit, bcm_l2_addr_t *l2addr, l2x_entry_t *l2x_entry)
{
    int l2mc_index, mb_index, rval;
    soc_memacc_t *memacc;

    sal_memset(l2addr, 0, sizeof(*l2addr));

    /* _BCM_L2X_MEMACC_FIELD and related macros defined for TH3(l2_helpers.c) */
    memacc = _BCM_L2X_MEMACC_FIELD(unit, MAC_ADDR_f);
    soc_memacc_mac_addr_get(memacc, l2x_entry, l2addr->mac);

    l2addr->vid = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, VLAN_ID_f);

    if (BCM_MAC_IS_MCAST(l2addr->mac)) {
        l2addr->flags |= BCM_L2_MCAST;
        l2mc_index = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, L2MC_PTR_f);

        l2addr->l2mc_group = l2mc_index;
        /* Translate l2mc index */
        BCM_IF_ERROR_RETURN(
            bcm_esw_switch_control_get(unit, bcmSwitchL2McIdxRetType, &rval));
        if (rval) {
            _BCM_MULTICAST_GROUP_SET(l2addr->l2mc_group, _BCM_MULTICAST_TYPE_L2,
                l2addr->l2mc_group);
        }
    } else {
        _bcm_gport_dest_t       dest;
        int                     dest_type_is_trunk = 0;
        int                     isGport = 0;

        _bcm_gport_dest_t_init(&dest);
#if defined(INCLUDE_L3)
        dest_type_is_trunk = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, T_f);
#endif /* INCLUDE_L3 */
        if (dest_type_is_trunk) {
            /* Trunk group */
            l2addr->flags |= BCM_L2_TRUNK_MEMBER;
            l2addr->tgid = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, TGID_f);

            dest.tgid = l2addr->tgid;
            dest.gport_type = _SHR_GPORT_TYPE_TRUNK;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        } else {
            bcm_module_t    mod_in;
            bcm_port_t      port_in;

            port_in = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, PORT_NUM_f);
            mod_in  = 0;

            l2addr->modid = mod_in;
            l2addr->port = port_in;
            dest.port = l2addr->port;
            dest.modid = l2addr->modid;
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            BCM_IF_ERROR_RETURN(
                bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
        }

        if (isGport) {
            
            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_construct(unit, &dest, &(l2addr->port)));
        }
    }

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        l2addr->group = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry,
                                                    CLASS_ID_f);
    } else {
        mb_index = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry,
                                               MAC_BLOCK_INDEX_f);
        if (mb_index) {
            BCM_PBMP_ASSIGN(l2addr->block_bitmap,
                            _mbi_entries[unit][mb_index].mb_pbmp);
        }
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, RPE_f)) {
        l2addr->flags |= BCM_L2_SETPRI;
    }

    l2addr->cos_dst = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, PRI_f);
    l2addr->cos_src = _BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, PRI_f);

    if (SOC_CONTROL(unit)->l2x_group_enable) {
        if (!BCM_L2_PRE_SET(l2addr->flags)) {
            l2addr->group |= (l2addr->cos_dst & 0xF) << 6;
        }
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, CPU_f)) {
        l2addr->flags |= BCM_L2_COPY_TO_CPU;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, DST_DISCARD_f)) {
        l2addr->flags |= BCM_L2_DISCARD_DST;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, SRC_DISCARD_f)) {
        l2addr->flags |= BCM_L2_DISCARD_SRC;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, SCP_f)) {
        l2addr->flags |= BCM_L2_COS_SRC_PRI;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, STATIC_BIT_f)) {
        l2addr->flags |= BCM_L2_STATIC;
    }

    /* No 'pending' support in h/w, so do not set BCM_L2_PENDING */

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, HITDA_f)) {
        l2addr->flags |= BCM_L2_DES_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, HITSA_f)) {
        l2addr->flags |= BCM_L2_SRC_HIT;
        l2addr->flags |= BCM_L2_HIT;
    }

    if (_BCM_L2X_MEMACC_FIELD_VALID(unit, LOCAL_SA_f)) {
        if (_BCM_L2X_MEMACC_FIELD32_GET(unit, l2x_entry, LOCAL_SA_f)) {
            l2addr->flags |= BCM_L2_NATIVE;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_l2_addr_get
 * Description:
 *      Given a MAC address and VLAN ID, check if the entry is present
 *      in the L2 table, and if so, return all associated information
 * Parameters:
 *      unit - Device unit number
 *      mac - input MAC address to search
 *      vid - input VLAN ID to search
 *      check_l2_only - only check if the target exist in L2 table
 *      l2addr - Pointer to bcm_l2_addr_t structure to receive results
 * Returns:
 *      BCM_E_NONE              Success (l2addr filled in)
 *      BCM_E_PARAM             Illegal parameter (NULL pointer)
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_NOT_FOUND Address not found (l2addr not filled in)
 */

int
_bcm_th3_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
                     int check_l2_only, bcm_l2_addr_t *l2addr)
{
    bcm_l2_addr_t l2addr_key;
    l2x_entry_t  l2x_entry, l2x_lookup;
    int          l2_hw_index;
    int          rv;

    bcm_l2_addr_t_init(&l2addr_key, mac, vid);

    if (!check_l2_only) {
        /* Check if function below has all l2 my stn mem fields */
        rv = bcm_td_l2_myStation_get (unit, mac, vid, l2addr);
        if (BCM_SUCCESS(rv)) {
            return rv;
        }
    }

    BCM_IF_ERROR_RETURN(
        _bcm_th3_l2_to_l2x(unit, &l2x_entry, &l2addr_key, TRUE));

    soc_mem_lock(unit, L2Xm);

    rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                &l2x_lookup, &l2_hw_index);

    soc_mem_unlock(unit, L2Xm);

    if (check_l2_only) {
        return rv;
    }

    if (SOC_SUCCESS(rv)) {
        rv = _bcm_th3_l2_from_l2x(unit, l2addr, &l2x_lookup);
    }

    return rv;
}

int
bcm_th3_l2_addr_get(int unit, sal_mac_addr_t mac, bcm_vlan_t vid,
                   bcm_l2_addr_t *l2addr)
{
    return _bcm_th3_l2_addr_get(unit, mac, vid, FALSE, l2addr);
}

/*
 * Function:
 *      bcm_th3_l2_addr_delete
 * Description:
 *      Delete an L2 address (MAC+VLAN) from the device
 * Parameters:
 *      unit - device unit
 *      mac  - MAC address to delete
 *      vid  - VLAN id
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_l2_addr_delete(int unit, bcm_mac_t mac, bcm_vlan_t vid)
{
    bcm_l2_addr_t  l2addr;
    l2x_entry_t    l2x_entry, l2x_lookup;
    int            l2_index, mb_index;
    int            rv;
    soc_control_t  *soc = SOC_CONTROL(unit);

    bcm_l2_addr_t_init(&l2addr, mac, vid);

    if (BCM_VLAN_VALID(vid)) {
        rv = bcm_td_l2_myStation_delete (unit, mac, vid, &l2_index);
        if ((rv != BCM_E_NOT_FOUND) && (rv != BCM_E_FULL)) {
            if (rv != BCM_E_NONE) {
                return rv;
            }
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_to_l2x(unit, &l2x_entry, &l2addr, TRUE));

    soc_mem_lock(unit, L2Xm);

    rv = soc_mem_search(unit, L2Xm, MEM_BLOCK_ANY, &l2_index,
                       (void *)&l2x_entry, (void *)&l2x_lookup, 0);
    if (BCM_E_NONE != rv) {
        soc_mem_unlock(unit, L2Xm);
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        mb_index = soc_L2Xm_field32_get(unit, &l2x_lookup, MAC_BLOCK_INDEXf);
        _bcm_th3_mac_block_delete(unit, mb_index);
    }

    if (SOC_L2_DEL_SYNC_LOCK(soc) < 0) {
        soc_mem_unlock(unit, L2Xm);
        return BCM_E_RESOURCE;
    }
    rv = soc_mem_delete_return_old(unit, L2Xm, MEM_BLOCK_ANY,
                                   (void *)&l2x_entry, (void *)&l2x_entry);
    if (rv >= 0) {
        rv = soc_l2x_sync_delete(unit, (uint32 *) &l2x_lookup, l2_index, 0);
    }
    SOC_L2_DEL_SYNC_UNLOCK(soc);
    soc_mem_unlock(unit, L2Xm);

    return rv;
}

/*
 * Function:
 *      _bcm_th3_l2_hash_dynamic_replace
 * Purpose:
 *      Replace dynamic L2 entries in a dual hash.
 * Parameters:
 *      unit - Unit number
 *      l2x_entry - Entry to insert instead of dynamic entry.
 * Returns:
 *      BCM_E_NONE Success
 *      BCM_E_XXX  Error
 */

int _bcm_th3_l2_hash_dynamic_replace(int unit, l2x_entry_t *l2x_entry)
{
    l2x_entry_t     l2ent;
    int             rv;
    uint32          fval;
    bcm_mac_t       mac;
    int             cf_hit, cf_unhit;
    int             bank, bucket, slot, index;
    int             num_banks;
    int             entries_per_bank, entries_per_row, entries_per_bucket;
    int             bank_base, bucket_offset;

    BCM_IF_ERROR_RETURN
        (soc_tomahawk3_hash_bank_count_get(unit, L2Xm, &num_banks));

    BCM_IF_ERROR_RETURN(soc_l2x_freeze(unit));

    cf_hit = cf_unhit = -1;
    for (bank = 0; bank < num_banks; bank++) {

        rv = soc_tomahawk3_hash_bank_info_get(unit, L2Xm, bank,
                                              &entries_per_bank,
                                              &entries_per_row,
                                              &entries_per_bucket,
                                              &bank_base, &bucket_offset);
        if (SOC_FAILURE(rv)) {
            (void)soc_l2x_thaw(unit);
            return rv;
        }

        bucket = soc_tomahawk3_l2x_bank_entry_hash(unit, bank, (uint32 *)l2x_entry);

        for (slot = 0; slot < entries_per_bucket; slot++) {
            uint32 valid = 0;
            index = bank_base + bucket * entries_per_row + bucket_offset +
                slot;
            rv = soc_mem_read(unit, L2Xm, MEM_BLOCK_ANY, index, &l2ent);
            if (SOC_FAILURE(rv)) {
                (void)soc_l2x_thaw(unit);
                return rv;
            }

            valid = soc_L2Xm_field32_get(unit, &l2ent, BASE_VALIDf);

            if (!valid) {
                /* Found invalid entry - stop the search victim found */
                cf_unhit = index;
                break;
            }

            fval = soc_mem_field32_get(unit, L2Xm, &l2ent, KEY_TYPEf);

            if (fval != TH3_L2_HASH_KEY_TYPE_BRIDGE) {
                continue;
            }

            soc_L2Xm_mac_addr_get(unit, &l2ent, MAC_ADDRf, mac);

            /* Skip static entries */
            if ((soc_L2Xm_field32_get(unit, &l2ent, STATIC_BITf)) ||
                (BCM_MAC_IS_MCAST(mac))) {
                continue;
            }

            if (soc_L2Xm_field32_get(unit, &l2ent, HITDAf) ||
                soc_L2Xm_field32_get(unit, &l2ent, HITSAf) ) {
                cf_hit =  index;
            } else {
                /* Found unhit entry - stop search victim found */
                cf_unhit = index;
                break;
            }
        }
        if (cf_unhit != -1) {
            break;
        }
    }

    COMPILER_REFERENCE(entries_per_bank);

    if (cf_unhit >= 0) {
        index = cf_unhit;   /* take last unhit dynamic */
    } else if (cf_hit >= 0) {
        index = cf_hit;     /* or last hit dynamic */
    } else {
        rv = BCM_E_FULL;     /* no dynamics to delete */
        (void) soc_l2x_thaw(unit);
         return rv;
    }

    rv = soc_mem_delete_index(unit, L2Xm, MEM_BLOCK_ALL, index);
    if (SOC_SUCCESS(rv)) {
        rv = soc_mem_generic_insert(unit, L2Xm, MEM_BLOCK_ANY, 0,
                                    l2x_entry, NULL, NULL);
    }
    if (SOC_FAILURE(rv)) {
        (void) soc_l2x_thaw(unit);
        return rv;
    }

    return soc_l2x_thaw(unit);
}

/*
 * Function:
 *      bcm_th3_l2_addr_add
 * Description:
 *      Add a MAC address to the Switch Address Resolution Logic (ARL)
 *      port with the given VLAN ID and parameters.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      l2addr - Pointer to bcm_l2_addr_t containing all valid fields
 * Returns:
 *      BCM_E_NONE              Success
 *      BCM_E_INTERNAL          Chip access failure
 *      BCM_E_RESOURCE          No MAC_BLOCK entries available
 * Notes:
 *      Use CMIC_PORT(unit) to associate the entry with the CPU.
 *      Use flag of BCM_L2_COPY_TO_CPU to send a copy to the CPU.
 *      Use flag of BCM_L2_TRUNK_MEMBER to set trunking (TGID must be
 *      passed as well with non-zero trunk group ID)
 *      In case the L2X table is full (e.g. bucket full), an attempt
 *      will be made to store the entry in the L2_USER_ENTRY table.
 */
int
bcm_th3_l2_addr_add(int unit, bcm_l2_addr_t *l2addr)
{
    l2x_entry_t  l2x_entry, l2x_lookup;
    int rv, l2_index, mb_index = 0;

    if (l2addr->flags & BCM_L2_L3LOOKUP) {
        BCM_IF_ERROR_RETURN(bcm_td_l2_myStation_add(unit, l2addr));
    } else {
        rv = bcm_td_l2_myStation_delete(unit, l2addr->mac, l2addr->vid,
                                        &l2_index);
        if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
            return rv;
        }
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_l2_to_l2x(unit, &l2x_entry, l2addr, FALSE));

    rv = soc_mem_generic_lookup(unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry,
                                &l2x_lookup, &l2_index);
    if (BCM_FAILURE(rv) && rv != BCM_E_NOT_FOUND) {
        return rv;
    }

    if (!SOC_CONTROL(unit)->l2x_group_enable) {
        /* Mac blocking, attempt to associate with bitmap entry */
        BCM_IF_ERROR_RETURN
            (_bcm_th3_mac_block_insert(unit, l2addr->block_bitmap, &mb_index));
        soc_mem_field32_set(unit, L2Xm, &l2x_entry, MAC_BLOCK_INDEXf,
                            mb_index);
    }

    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_insert_return_old(unit, L2Xm, MEM_BLOCK_ANY,
                                   (void *)&l2x_entry, NULL);
    if ((rv == BCM_E_FULL) && (l2addr->flags & BCM_L2_REPLACE_DYNAMIC)) {
        rv = _bcm_th3_l2_hash_dynamic_replace( unit, &l2x_entry);
        if (rv < 0 ) {
            goto done;
        }
    } else if (rv == BCM_E_EXISTS) {
        if (!SOC_CONTROL(unit)->l2x_group_enable) {
            mb_index = soc_mem_field32_get(unit, L2Xm, &l2x_lookup,
                                           MAC_BLOCK_INDEXf);
            _bcm_th3_mac_block_delete(unit, mb_index);
        }
        rv = BCM_E_NONE;
    }

    if (rv == BCM_E_NONE) {
        if (soc_th3_l2x_appl_callback_ready(unit)) {
            /* Record the entry in internal database for use by l2x sync thread;
             * the thread will suppress callback when it sees the entry in the
             * internal database
             */
            rv = soc_th3_l2x_appl_callback_entry_add(unit, l2addr->mac,
                                                     l2addr->vid, TRUE);
        }
    }

done:
    soc_mem_unlock(unit, L2Xm);
    if (rv < 0) {
        _bcm_th3_mac_block_delete(unit, mb_index);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_th3_l2_cross_connect_add
 * Description:
 *      Add a VLAN cross connect entry
 * Parameters:
 *      unit       - Device unit number
 *      outer_vlan - Outer vlan ID
 *      destt      - array of 2, specifies the corresponding dst is TGID or Port
 *      destv      - array of 2, TGID or Port corresponding to two gports
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 * Notes:
 *      All incoming paramaters are assumed to be valid
 *      and hence no further checks are performed on them
 */
int
_bcm_th3_l2_cross_connect_add(
    int unit, bcm_vlan_t outer_vlan, uint8 *destt, int *destv)
{
    int rv;
    int l2_index;
    l2x_entry_t l2x_entry;
    l2x_entry_t l2x_lookup;

    sal_memset(&l2x_entry, 0, sizeof (l2x_entry));

    soc_L2Xm_field32_set(unit, &l2x_entry, BASE_VALIDf, 1);
    soc_L2Xm_field32_set(unit, &l2x_entry, STATIC_BITf, 1);
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN__OVIDf, outer_vlan);
    soc_L2Xm_field32_set(unit, &l2x_entry, KEY_TYPEf,
                             TH3_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT);

    /* See if the entry already exists */
    rv = soc_mem_generic_lookup(
            unit, L2Xm, MEM_BLOCK_ANY, 0, &l2x_entry, &l2x_lookup, &l2_index);

    if ((rv < 0) && (rv != BCM_E_NOT_FOUND)) {
         return rv;
    }

    /* Fill in first port */
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN__DEST_TYPEf, !!destt[0]);
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN__DESTINATIONf, destv[0]);
    /* Fill in second port */
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN__DEST_TYPE_1f, !!destt[1]);
    soc_L2Xm_field32_set(unit, &l2x_entry, VLAN__DESTINATION_1f, destv[1]);

    rv = soc_mem_insert_return_old(unit, L2Xm, MEM_BLOCK_ANY,
                                   (void *)&l2x_entry, (void *)&l2x_entry);
    if (rv == BCM_E_FULL) {
        rv = _bcm_th3_l2_hash_dynamic_replace(unit, &l2x_entry);
    }

    return rv;
}

#if defined(BCM_WARM_BOOT_SUPPORT)
/*
 * Function:
 *      _bcm_th3_l2_cross_connect_entry_add_all
 * Description:
 *      Walks through the valid cross connect entries
 *      in L2X and adds the entry in VLAN CC Database.
 * Parameters:
 *      unit       - (IN) bcm device.
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_XXX
 */
int
_bcm_th3_l2_cross_connect_entry_add_all(int unit)
{
    int idx, index;
    int index_min, index_max, mem_max;
    int rv = SOC_E_NONE;
    int *buffer = NULL;
    l2x_entry_t *l2x_entry;
    bcm_vlan_t outer_vlan;
    soc_mem_t mem = L2Xm;
    uint8 destt[2];
    int destv[2];
    int l2delete_chunks = DEFAULT_L2DELETE_CHUNKS;

    index_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);

    buffer = soc_cm_salloc( unit,
                            (l2delete_chunks * sizeof(l2x_entry_t)),
                            "cross connect traverse");
    if (NULL == buffer) {
        return SOC_E_MEMORY;
    }

    soc_mem_lock(unit, L2Xm);
    for (idx = index_min; idx < mem_max; idx += l2delete_chunks) {
        index_max = idx + l2delete_chunks - 1;
        if ( index_max > mem_max) {
            index_max = mem_max;
        }
        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                                     idx, index_max, buffer)) < 0 ) {
            break;
        }
        for (index = 0; index < l2delete_chunks; index++) {
            l2x_entry =
                soc_mem_table_idx_to_pointer(unit, mem,
                                             l2x_entry_t *, buffer, index);
            if (soc_L2Xm_field32_get(unit, l2x_entry, BASE_VALIDf)) {
                if (TH3_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT !=
                        soc_L2Xm_field32_get(unit, l2x_entry, KEY_TYPEf)) {
                    continue;
                }

                /* Extract the outer vlan */
                outer_vlan = soc_L2Xm_field32_get(unit, l2x_entry, VLAN__OVIDf);

                /* Deduce the first destination */
                destv[0] =
                    soc_L2Xm_field32_get(unit, l2x_entry, VLAN__DESTINATIONf);
                destt[0] = (uint8)
                    soc_L2Xm_field32_get(unit, l2x_entry, VLAN__DEST_TYPEf);

                /* Deduce the second destination */
                destv[1] =
                    soc_L2Xm_field32_get(unit, l2x_entry, VLAN__DESTINATION_1f);
                destt[1] = (uint8)
                    soc_L2Xm_field32_get(unit, l2x_entry, VLAN__DEST_TYPE_1f);

                rv =  _bcm_th3_vlan_xconnect_db_entry_add(
                                               unit, outer_vlan, destt, destv);
                if (BCM_FAILURE(rv)) {
                    soc_mem_unlock(unit, L2Xm);
                    soc_cm_sfree(unit, buffer);
                    return rv;
                }
            }
        }
    }
    soc_mem_unlock(unit, L2Xm);
    soc_cm_sfree(unit, buffer);

    return rv;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif /* BCM_TOMAHAWK3_SUPPORT */
