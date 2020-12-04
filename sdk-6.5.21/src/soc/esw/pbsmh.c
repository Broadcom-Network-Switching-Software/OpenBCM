/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Utility routines for managing pbsmh headers
 * These are mainly used for debug purposes
 */

#include <shared/bsl.h>

#include <assert.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/drv.h>
#include <soc/pbsmh.h>
#include <soc/cm.h>
#include <soc/dcb.h>
#include <soc/debug.h>

#ifdef BCM_XGS3_SWITCH_SUPPORT

/* PBSMH field macros */

#if defined(LE_HOST)
/* packet is in BE format */
#define SOC_PBSMH_SWAP32(val) \
        ((uint32)( \
                (((uint32)(val) & (uint32)0x000000ffUL) << 24) | \
                (((uint32)(val) & (uint32)0x0000ff00UL) <<  8) | \
                (((uint32)(val) & (uint32)0x00ff0000UL) >>  8) | \
                (((uint32)(val) & (uint32)0xff000000UL) >> 24) ))

#else /* !LE_HOST */

#define SOC_PBSMH_SWAP32(val) val

#endif

/* SOC_PBSMH_GET_BIT_POS - converts HW regsfile bitoffset to SW
 * assumption is that header is 4 word length - 128 bits )
 */
#define SOC_PBSMH_GET_BIT_POS(pos) (3-((pos)/32))*32+((pos)%32)

#define SOC_PBSMH_GET_WORD(array,word) SOC_PBSMH_SWAP32((array)[word])
#define SOC_PBSMH_SET_WORD(val,array,word) (array)[word]=SOC_PBSMH_SWAP32(val)

#define SOC_PBSMH_GET_MASK(size) ((1<<(size))-1)
#define SOC_PBSMH_SET_VALUE(val,array,word,pos,len)\
        val = SOC_PBSMH_GET_WORD (array,word);\
        val = (val) >> (pos);\
        val = (val) & SOC_PBSMH_GET_MASK ((len))

#define SOC_PBSMH_SET_VALUE_IN(val,array,word,pos,len){\
        uint32 tmp;\
        tmp = SOC_PBSMH_GET_WORD (array,word);\
        tmp = (tmp) & ~(SOC_PBSMH_GET_MASK (len)<<pos);\
        tmp |= (((val) & SOC_PBSMH_GET_MASK (len))<<pos);\
        SOC_PBSMH_SET_WORD(tmp,array,word);\
    }



static char *soc_pbsmh_field_names[] = {
    /* NOTE: strings must match soc_pbsmh_field_t */
    "start",
    "src_mod",
    "dst_port",
    "cos",
    "pri",
    "l3pbm_sel",
    "l2pbm_sel",
    "unicast",
    "tx_ts",
    "spid_override",
    "spid",
    "spap",
    "queue_num",
    "osts",
    "its_sign",
    "hdr_offset",
    "regen_udp_checksum",
    "int_pri",
    "nlf_port",
    "lm_ctr_index",
    "oam_replacement_type",
    "oam_replacement_offset",
    "ep_cpu_reasons",
    "header_type",
    "cell_error",
    "ipcf_ptr",
    "oam_ma_ptr",
    "ts_action",
    "sample_rdi",
    "ctr1_action",
    "lm_ctr1_index",
    "ctr2_action",
    "lm_ctr2_index",
    "pp_port",
   "dst_subport_num",
    "amp_ts_act",
    "sobmh_flex",
    NULL
};

/* The following array entries must have the same offset like soc_pbsmh_field_t */
static soc_pbsmh_field_attr_t soc_pbsmh_v10_field_attr [ PBSMH_COUNT ] = {
 /* PBSMH_start                   */{ SOC_PBSMH_GET_BIT_POS(126), 2 },                             
 /* PBSMH_src_mod,                */{ SOC_PBSMH_GET_BIT_POS(32), 8 },  
 /* PBSMH_dst_port,               */{ SOC_PBSMH_GET_BIT_POS(66), 7 },
 /* PBSMH_cos,                    */{ SOC_PBSMH_GET_BIT_POS(58), 4 }, 
 /* PBSMH_pri,                    */{ SOC_PBSMH_GET_BIT_POS(62), 4 },
 /* PBSMH_l3pbm_sel,              */{ SOC_PBSMH_GET_BIT_POS(82), 1 },
 /* PBSMH_l2pbm_sel,              */{ SOC_PBSMH_GET_BIT_POS(73), 1 },
 /* PBSMH_unicast,                */{ SOC_PBSMH_GET_BIT_POS(52), 1 },
 /* PBSMH_tx_ts,                  */{ SOC_PBSMH_GET_BIT_POS(83), 1 },
 /* PBSMH_spid_override,          */{ SOC_PBSMH_GET_BIT_POS(57), 1 },
 /* PBSMH_spid,                   */{ SOC_PBSMH_GET_BIT_POS(55), 2 },
 /* PBSMH_spap,                   */{ SOC_PBSMH_GET_BIT_POS(52), 2 },
 /* PBSMH_queue_num,              */{ SOC_PBSMH_GET_BIT_POS(40), 12 },
 /* PBSMH_osts,                   */{ SOC_PBSMH_GET_BIT_POS(86), 1 },
 /* PBSMH_its_sign,               */{ SOC_PBSMH_GET_BIT_POS(84), 1 },
 /* PBSMH_hdr_offset,             */{ SOC_PBSMH_GET_BIT_POS(74), 8 },
 /* PBSMH_regen_udp_checksum,     */{ SOC_PBSMH_GET_BIT_POS(85), 1 },
 /* PBSMH_int_pri,                */{ SOC_PBSMH_GET_BIT_POS(58), 4 },
 /* PBSMH_nlf_port,  NOT USED     */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_lm_ctr_index,           */{ SOC_PBSMH_GET_BIT_POS(81), 15 },
 /* PBSMH_oam_replacement_type, NOT USED  */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_oam_replacement_offset, */{ SOC_PBSMH_GET_BIT_POS(57), 6 },
 /* PBSMH_ep_cpu_reasons,NOT USED */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_header_type,            */{ SOC_PBSMH_GET_BIT_POS(120), 6 },
 /* PBSMH_cell_error,             */{ SOC_PBSMH_GET_BIT_POS(119), 1 },
 /* PBSMH_ipcf_ptr,  NOT USED     */{ SOC_PBSMH_GET_BIT_POS(24), 0 },
 /* PBSMH_oam_ma_ptr,NOT USED     */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_ts_action,              */{ SOC_PBSMH_GET_BIT_POS(99), 2 },
 /* PBSMH_sample_rdi, NOT USED    */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_ctr1_action,            */{ SOC_PBSMH_GET_BIT_POS(78), 2 },
 /* PBSMH_lm_ctr1_index,          */{ SOC_PBSMH_GET_BIT_POS(63), 15 },
 /* PBSMH_ctr2_action,            */{ SOC_PBSMH_GET_BIT_POS(96), 2 },
 /* PBSMH_lm_ctr2_index,          */{ SOC_PBSMH_GET_BIT_POS(81), 15 },
 /* PBSMH_pp_port,   NOT USED     */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
};

/* The following array entries must have the same offset like soc_pbsmh_field_t */
/* Used for Chips: Apache */
static soc_pbsmh_field_attr_t soc_pbsmh_v10_ap_field_attr [ PBSMH_COUNT ] = {
 /* PBSMH_start                   */{ SOC_PBSMH_GET_BIT_POS(126), 2 },
 /* PBSMH_src_mod,                */{ SOC_PBSMH_GET_BIT_POS(32), 8 },
 /* PBSMH_dst_port,               */{ SOC_PBSMH_GET_BIT_POS(68), 7 },
 /* PBSMH_cos,                    */{ SOC_PBSMH_GET_BIT_POS(60), 4 },
 /* PBSMH_pri,                    */{ SOC_PBSMH_GET_BIT_POS(64), 4 },
 /* PBSMH_l3pbm_sel,              */{ SOC_PBSMH_GET_BIT_POS(84), 1 },
 /* PBSMH_l2pbm_sel,              */{ SOC_PBSMH_GET_BIT_POS(75), 1 },
 /* PBSMH_unicast,                */{ SOC_PBSMH_GET_BIT_POS(54), 1 },
 /* PBSMH_tx_ts,                  */{ SOC_PBSMH_GET_BIT_POS(85), 1 },
 /* PBSMH_spid_override,          */{ SOC_PBSMH_GET_BIT_POS(59), 1 },
 /* PBSMH_spid,                   */{ SOC_PBSMH_GET_BIT_POS(57), 2 },
 /* PBSMH_spap,                   */{ SOC_PBSMH_GET_BIT_POS(55), 2 },
 /* PBSMH_queue_num,              */{ SOC_PBSMH_GET_BIT_POS(40), 14 },
 /* PBSMH_osts,                   */{ SOC_PBSMH_GET_BIT_POS(88), 1 },
 /* PBSMH_its_sign,               */{ SOC_PBSMH_GET_BIT_POS(86), 1 },
 /* PBSMH_hdr_offset,             */{ SOC_PBSMH_GET_BIT_POS(76), 8 },
 /* PBSMH_regen_udp_checksum,     */{ SOC_PBSMH_GET_BIT_POS(87), 1 },
 /* PBSMH_int_pri,                */{ SOC_PBSMH_GET_BIT_POS(60), 4 },
 /* PBSMH_nlf_port,  NOT USED     */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_lm_ctr_index, NOT USED  */{ SOC_PBSMH_GET_BIT_POS(81), 15 },
 /* PBSMH_oam_replacement_type, NOT USED  */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_oam_replacement_offset, */{ SOC_PBSMH_GET_BIT_POS(57), 6 },
 /* PBSMH_ep_cpu_reasons,NOT USED */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_header_type,            */{ SOC_PBSMH_GET_BIT_POS(120), 6 },
 /* PBSMH_cell_error,             */{ SOC_PBSMH_GET_BIT_POS(119), 1 },
 /* PBSMH_ipcf_ptr,  NOT USED     */{ SOC_PBSMH_GET_BIT_POS(24), 0 },
 /* PBSMH_oam_ma_ptr,NOT USED     */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_ts_action,              */{ SOC_PBSMH_GET_BIT_POS(99), 2 },
 /* PBSMH_sample_rdi, NOT USED    */{ SOC_PBSMH_GET_BIT_POS(0), 0 },
 /* PBSMH_ctr1_action,            */{ SOC_PBSMH_GET_BIT_POS(78), 2 },
 /* PBSMH_lm_ctr1_index,          */{ SOC_PBSMH_GET_BIT_POS(63), 15 },
 /* PBSMH_ctr2_action,            */{ SOC_PBSMH_GET_BIT_POS(96), 2 },
 /* PBSMH_lm_ctr2_index,          */{ SOC_PBSMH_GET_BIT_POS(81), 15 },
 /* PBSMH_pp_port,   NOT USED     */{ SOC_PBSMH_GET_BIT_POS(0), 0 },

};


soc_pbsmh_field_t
soc_pbsmh_name_to_field(int unit, char *name)
{
    soc_pbsmh_field_t           f;

    assert(COUNTOF(soc_pbsmh_field_names) - 1 == PBSMH_COUNT);

    COMPILER_REFERENCE(unit);

    for (f = 0; soc_pbsmh_field_names[f] != NULL; f++) {
        if (sal_strcmp(name, soc_pbsmh_field_names[f]) == 0) {
            return f;
        }
    }

    return PBSMH_invalid;
}

char *
soc_pbsmh_field_to_name(int unit, soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    assert(COUNTOF(soc_pbsmh_field_names) - 1 == PBSMH_COUNT);

    if (field < 0 || field >= PBSMH_COUNT) {
        return "??";
    } else {
        return soc_pbsmh_field_names[field];
    }
}

STATIC uint32 
soc_pbsmh_array_get(uint32 *array,int off,int len)
{
    int word = off / 32;
    int pos  = off % 32;
    uint32 val;
    uint32 valhi;
    uint32 wordhi, poshi, lenhi;
    uint32 lenlo;

    if ((pos + len) > 32) {
        /* two words */
        wordhi = word + 1;
        poshi  = 0;
        lenhi  = len - (32 - pos);
        lenlo = len - lenhi;

        SOC_PBSMH_SET_VALUE(valhi, array, wordhi, poshi, lenhi);
        SOC_PBSMH_SET_VALUE(val, array, word, pos, lenlo);

        val = (valhi << lenlo) | val;
    } else {
        SOC_PBSMH_SET_VALUE(val, array, word, pos, len);
    }

    return val;
}

STATIC void 
soc_pbsmh_array_set(uint32 value, uint32 *array,int off,int len)
{
    int word = off / 32;
    int pos  = off % 32;
    uint32 valhi;
    uint32 wordhi, poshi, lenhi;
    uint32 lenlo;

    if ((pos + len) > 32) {
        /* two words */
        wordhi = word + 1;
        poshi  = 0;
        lenhi  = len - (32 - pos);
        lenlo = len - lenhi;

        valhi = value >> lenlo;

        SOC_PBSMH_SET_VALUE_IN(valhi, array, wordhi, poshi, lenhi);
        SOC_PBSMH_SET_VALUE_IN(value, array, word, pos, lenlo);
    } else {
        SOC_PBSMH_SET_VALUE_IN(value, array, word, pos, len);
    }
}





STATIC void
soc_pbsmh_v1_field_set(int unit, soc_pbsmh_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:   mh->start = val;
                        mh->_rsvd0 = 0;
                        mh->_rsvd1 = 0;
                        mh->_rsvd2 = 0;
                        break;
    case PBSMH_src_mod: mh->src_mod = val; break;
    case PBSMH_dst_port:mh->dst_port = val;break;
    case PBSMH_cos:     mh->cos = val;break;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%s val=0x%x\n"),
                  unit, soc_pbsmh_field_to_name(unit, field), val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v1_field_get(int unit, soc_pbsmh_hdr_t *mh, soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:       return mh->start;
    case PBSMH_src_mod:     return mh->src_mod; 
    case PBSMH_dst_port:    return mh->dst_port;
    case PBSMH_cos:         return mh->cos;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC void
soc_pbsmh_v2_field_set(int unit, soc_pbsmh_v2_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->start = val;
                                mh->_rsvd0 = 0;
                                mh->_rsvd1 = 0;
                                mh->_rsvd2 = 0;
                                break;
    case PBSMH_src_mod:         mh->src_mod = val; break;
    case PBSMH_dst_port:        mh->dst_port = val; break;
    case PBSMH_cos:             mh->cos = val; break;
    case PBSMH_pri:             mh->pri = val; break;
    case PBSMH_l3pbm_sel:       mh->l3pbm_sel = val; break;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v2_field_get(int unit,
                       soc_pbsmh_v2_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:       return mh->start;
    case PBSMH_src_mod:     return mh->src_mod; 
    case PBSMH_dst_port:    return mh->dst_port;
    case PBSMH_cos:         return mh->cos;
    case PBSMH_pri:         return mh->pri;
    case PBSMH_l3pbm_sel:   return mh->l3pbm_sel;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC void
soc_pbsmh_v3_field_set(int unit, soc_pbsmh_v3_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->start = val;
                                mh->_rsvd0 = 0;
                                mh->_rsvd1 = 0;
                                mh->_rsvd2 = 0;
                                break;
    case PBSMH_src_mod:         mh->src_mod_hi = (val >> 4); 
                                mh->src_mod_lo = (val & 0xf); 
                                break;
    case PBSMH_dst_port:        mh->dst_port = val; break;
    case PBSMH_cos:             mh->cos = val; break;
    case PBSMH_pri:             mh->pri = val; break;
    case PBSMH_l3pbm_sel:       mh->l3pbm_sel = val; break;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v3_field_get(int unit,
                       soc_pbsmh_v3_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:       return mh->start;
    case PBSMH_src_mod:     return ((mh->src_mod_hi << 4) | (mh->src_mod_lo)); 
    case PBSMH_dst_port:    return mh->dst_port;
    case PBSMH_cos:         return mh->cos;
    case PBSMH_pri:         return mh->pri;
    case PBSMH_l3pbm_sel:   return mh->l3pbm_sel;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC void
soc_pbsmh_v4_field_set(int unit, soc_pbsmh_v4_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->start = val;
                                mh->_rsvd0 = 0;
                                mh->_rsvd1 = 0;
                                mh->_rsvd2 = 0;
                                break;
    case PBSMH_src_mod:         mh->src_mod_hi = (val >> 7); 
                                mh->src_mod_lo = (val & 0x7f); 
                                break;
    case PBSMH_dst_port:        mh->dst_port = val; break;
    case PBSMH_cos:             mh->cos = val; break;
    case PBSMH_pri:             mh->pri_hi = (val >> 3); 
                                mh->pri_lo = (val & 0x7); 
                                break;
    case PBSMH_l3pbm_sel:       mh->l3pbm_sel = val; break;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v4_field_get(int unit,
                       soc_pbsmh_v4_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:       return mh->start;
    case PBSMH_src_mod:     return ((mh->src_mod_hi << 7) | (mh->src_mod_lo)); 
    case PBSMH_dst_port:    return mh->dst_port;
    case PBSMH_cos:         return mh->cos;
    case PBSMH_pri:         return ((mh->pri_hi << 3) | (mh->pri_lo));
    case PBSMH_l3pbm_sel:   return mh->l3pbm_sel;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC void
soc_pbsmh_v5_field_set(int unit, soc_pbsmh_v5_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->start = val;
                                mh->_rsvd0 = 0;
                                mh->_rsvd1 = 0;
                                mh->_rsvd2 = 0;
                                mh->_rsvd3 = 0;
                                mh->_rsvd4 = 0;
                                mh->_rsvd5 = 0;
                                mh->_rsvd6 = 0;
                                mh->_rsvd7 = 0;
                                break;
    case PBSMH_src_mod:         mh->src_mod = val; break;
    case PBSMH_dst_port:        mh->dst_port = val; break;
    case PBSMH_cos:             mh->cos = val; break;
    case PBSMH_pri:             mh->input_pri = val; break;
    case PBSMH_l3pbm_sel:       mh->set_l3bm = val; break;
    case PBSMH_l2pbm_sel:       mh->set_l2bm = val; break;
    case PBSMH_unicast:         mh->unicast = val; break;
    case PBSMH_tx_ts:           mh->tx_ts = val; break;
    case PBSMH_spid_override:   mh->spid_override = val; break;
    case PBSMH_spid:            mh->spid = val; break;
    case PBSMH_spap:            mh->spap = val; break;
    case PBSMH_queue_num:       mh->queue_num = val; break;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v5_field_get(int unit,
                       soc_pbsmh_v5_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:         return mh->start;
    case PBSMH_src_mod:       return mh->src_mod;
    case PBSMH_dst_port:      return mh->dst_port;
    case PBSMH_cos:           return mh->cos;
    case PBSMH_pri:           return mh->input_pri;
    case PBSMH_l3pbm_sel:     return mh->set_l3bm;
    case PBSMH_l2pbm_sel:     return mh->set_l2bm;
    case PBSMH_unicast:       return mh->unicast;
    case PBSMH_tx_ts:         return mh->tx_ts;
    case PBSMH_spid_override: return mh->spid_override;
    case PBSMH_spid:          return mh->spid;
    case PBSMH_spap:          return mh->spap;
    case PBSMH_queue_num:     return mh->queue_num;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}


STATIC void
soc_pbsmh_v6_field_set(int unit, soc_pbsmh_v6_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->start = val;
                                mh->_rsvd0 = 0;
                                mh->_rsvd1 = 0;
                                mh->_rsvd2 = 0;
                                mh->_rsvd3 = 0;
                                mh->_rsvd4 = 0;
                                mh->_rsvd5 = 0;
                                break;
    case PBSMH_src_mod:         mh->src_mod = val; break;
    case PBSMH_dst_port:        mh->dst_port = val; break;
    case PBSMH_cos:             mh->cos = val; break;
    case PBSMH_pri:             mh->input_pri = val; break;
    case PBSMH_l3pbm_sel:       mh->set_l3bm = val; break;
    case PBSMH_l2pbm_sel:       mh->set_l2bm = val; break;
    case PBSMH_unicast:         mh->unicast = val; break;
    case PBSMH_tx_ts:           mh->tx_ts = val; break;
    case PBSMH_spid_override:   mh->spid_override = val; break;
    case PBSMH_spid:            mh->spid = val; break;
    case PBSMH_spap:            mh->spap = val; break;
    case PBSMH_queue_num:
        mh->queue_num_1 = val & 0x3;
        mh->queue_num_2 = (val >> 2) & 0xff;
        mh->queue_num_3 = (val >> 10) & 0x3;
        break;
    case PBSMH_osts:            mh->osts = val; break;
    case PBSMH_its_sign:        mh->its_sign = val; break;
    case PBSMH_hdr_offset:
        mh->hdr_offset_1 = val & 0x1f;
        mh->hdr_offset_2 = (val >> 5) & 0x7;
        break;
    case PBSMH_regen_udp_checksum: mh->regen_upd_chksum = val; break;
    case PBSMH_ipcf_ptr:        mh->ipcf_ptr = val; break;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v6_field_get(int unit,
                       soc_pbsmh_v6_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:         return mh->start;
    case PBSMH_src_mod:       return mh->src_mod;
    case PBSMH_dst_port:      return mh->dst_port;
    case PBSMH_cos:           return mh->cos;
    case PBSMH_pri:           return mh->input_pri;
    case PBSMH_l3pbm_sel:     return mh->set_l3bm;
    case PBSMH_l2pbm_sel:     return mh->set_l2bm;
    case PBSMH_unicast:       return mh->unicast;
    case PBSMH_tx_ts:         return mh->tx_ts;
    case PBSMH_spid_override: return mh->spid_override;
    case PBSMH_spid:          return mh->spid;
    case PBSMH_spap:          return mh->spap;
    case PBSMH_queue_num:
        return (mh->queue_num_1 | (mh->queue_num_2 << 2)  | (mh->queue_num_3 << 10));
    case PBSMH_osts:          return mh->osts;
    case PBSMH_its_sign:      return mh->its_sign;
    case PBSMH_hdr_offset:
        return (mh->hdr_offset_1 | (mh->hdr_offset_2 << 5));
    case PBSMH_regen_udp_checksum: return mh->regen_upd_chksum;
    case PBSMH_ipcf_ptr:      return mh->ipcf_ptr;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC void
soc_pbsmh_v7_field_set(int unit, soc_pbsmh_v7_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->overlay1.start = val;
              mh->overlay1.header_type = PBS_MH_V7_HDR_TYPE_FROM_CPU;
                                mh->overlay1.cell_error = 0;
                                mh->overlay1.oam_replacement_offset = 0;
                                mh->overlay1.oam_replacement_type = 0;
                                mh->overlay2._rsvd3 = 0;
                                break;
    case PBSMH_src_mod:         mh->overlay1.src_mod = val; break;
    case PBSMH_dst_port:        mh->overlay1.dst_port = val; break;
    case PBSMH_cos:             mh->overlay2.cos = val; break;
    case PBSMH_pri:             mh->overlay1.input_pri = val; break;
    case PBSMH_l3pbm_sel:       mh->overlay1.set_l3bm = val; break;
    case PBSMH_l2pbm_sel:       mh->overlay1.set_l2bm = val; break;
    case PBSMH_unicast:         mh->overlay2.unicast = val; break;
    case PBSMH_tx_ts:           mh->overlay1.tx_ts = val; break;
    case PBSMH_spid_override:   mh->overlay2.spid_override = val; break;
    case PBSMH_spid:            mh->overlay2.spid = val; break;
    case PBSMH_spap:            mh->overlay2.spap = val; break;
    case PBSMH_queue_num:
        mh->overlay1.queue_num_1 = val & 0xff;
        mh->overlay1.queue_num_2 = (val >> 8) & 0x3;
        if (SOC_DCB_TYPE(unit) == 26) {
            mh->overlay1.queue_num_3 = (val >> 10) & 0x3;
        }
        break;
    case PBSMH_osts:            mh->overlay1.osts = val; break;
    case PBSMH_its_sign:        mh->overlay1.its_sign = val; break;
    case PBSMH_hdr_offset:      mh->overlay1.hdr_offset = val; break;
    case PBSMH_regen_udp_checksum: mh->overlay1.regen_udp_checksum = val; break;
    case PBSMH_int_pri:         mh->overlay1.int_pri = val; break;
    case PBSMH_nlf_port:        mh->overlay1.nlf_port = val; break;
    case PBSMH_lm_ctr_index:
        mh->overlay2.lm_counter_index_1 = val & 0xff;
        mh->overlay2.lm_counter_index_2 = (val >> 8) & 0xff;
        mh->overlay2._rsvd3 = 0;
        break;
    case PBSMH_oam_replacement_type:
        mh->overlay1.oam_replacement_type = val;
        break;
    case PBSMH_oam_replacement_offset:
        mh->overlay1.oam_replacement_offset = val;
        break;
    case PBSMH_ep_cpu_reasons: 
        mh->overlay1._rsvd5 = 0;
        mh->overlay1.ep_cpu_reason_code_1 = val & 0xff;
        mh->overlay1.ep_cpu_reason_code_2 = (val >> 8) & 0xff;
        mh->overlay1.ep_cpu_reason_code_3 = (val >> 16) & 0xf;
        break;
    case PBSMH_header_type:     mh->overlay1.header_type = val; break;
    case PBSMH_cell_error:      mh->overlay1.cell_error = val; break;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v7_field_get(int unit,
                       soc_pbsmh_v7_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:         return mh->overlay1.start;
    case PBSMH_src_mod:       return mh->overlay1.src_mod;
    case PBSMH_dst_port:      return mh->overlay1.dst_port;
    case PBSMH_cos:           return mh->overlay2.cos;
    case PBSMH_pri:           return mh->overlay1.input_pri;
    case PBSMH_l3pbm_sel:     return mh->overlay1.set_l3bm;
    case PBSMH_l2pbm_sel:     return mh->overlay1.set_l2bm;
    case PBSMH_unicast:       return mh->overlay2.unicast;
    case PBSMH_tx_ts:         return mh->overlay1.tx_ts;
    case PBSMH_spid_override: return mh->overlay2.spid_override;
    case PBSMH_spid:          return mh->overlay2.spid;
    case PBSMH_spap:          return mh->overlay2.spap;
    case PBSMH_queue_num:
        if (SOC_DCB_TYPE(unit) == 26) {
            return (mh->overlay1.queue_num_1 |
                    (mh->overlay1.queue_num_2 << 8) |
                    (mh->overlay1.queue_num_3 << 10));
        } else {
            return (mh->overlay1.queue_num_1 |
                    (mh->overlay1.queue_num_2 << 8));
        }
    case PBSMH_osts:          return mh->overlay1.osts;
    case PBSMH_its_sign:      return mh->overlay1.its_sign;
    case PBSMH_hdr_offset:    return mh->overlay1.hdr_offset;
    case PBSMH_regen_udp_checksum: return mh->overlay1.regen_udp_checksum;
    case PBSMH_int_pri:       return mh->overlay1.int_pri;
    case PBSMH_nlf_port:      return mh->overlay1.nlf_port;
    case PBSMH_lm_ctr_index:
        return (mh->overlay2.lm_counter_index_1 |
                (mh->overlay2.lm_counter_index_2 << 8));
    case PBSMH_oam_replacement_type:
        return mh->overlay1.oam_replacement_type;
    case PBSMH_oam_replacement_offset:
        return mh->overlay1.oam_replacement_offset;
    case PBSMH_ep_cpu_reasons: 
        return (mh->overlay1.ep_cpu_reason_code_1 |
                (mh->overlay1.ep_cpu_reason_code_2 << 8) |
                (mh->overlay1.ep_cpu_reason_code_3 << 16));
    case PBSMH_header_type:   return mh->overlay1.header_type;
    case PBSMH_cell_error:    return mh->overlay1.cell_error;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC void
soc_pbsmh_v8_field_set(int unit, soc_pbsmh_v8_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:           mh->overlay1.start = val;
                                mh->overlay1.header_type = PBS_MH_V7_HDR_TYPE_FROM_CPU;
                                mh->overlay1.cell_error = 0;
                                mh->overlay1.oam_replacement_offset = 0;
                                mh->overlay1.oam_replacement_type = 0;
                                break;
    case PBSMH_src_mod:         mh->overlay1.src_mod = val; break;
    case PBSMH_dst_port:        mh->overlay1.dst_pp_port = val; break;
    case PBSMH_cos:
        mh->overlay1.cos_lsb = val & 0xf;
        mh->overlay1.cos_msb_1 = (val >> 4) & 0x3;
        mh->overlay1.cos_msb_2 = (val >> 6) & 0x3f;
        break;
    case PBSMH_pri:             mh->overlay1.input_pri = val; break;
    case PBSMH_l3pbm_sel:       mh->overlay1.set_l3bm = val; break;
    case PBSMH_l2pbm_sel:       mh->overlay1.set_l2bm = val; break;
    case PBSMH_unicast:         mh->overlay1.unicast = val; break;
    case PBSMH_tx_ts:           mh->overlay1.tx_ts = val; break;
    case PBSMH_spid_override:   mh->overlay1.spid_override = val; break;
    case PBSMH_spid:            mh->overlay1.spid = val; break;
    case PBSMH_spap:            mh->overlay1.spap = val; break;
    case PBSMH_queue_num:
        mh->overlay1.queue_num_1 = val & 0xff;
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            mh->overlay1.queue_num_2 = (val >> 8) & 0x1;
        } else
#endif
        {
            mh->overlay1.queue_num_2 = (val >> 8) & 0xf;
        }
        break;
    case PBSMH_osts:            mh->overlay1.osts = val; break;
    case PBSMH_its_sign:        mh->overlay1.its_sign = val; break;
    case PBSMH_hdr_offset:      mh->overlay1.hdr_offset = val; break;
    case PBSMH_regen_udp_checksum: mh->overlay1.regen_udp_checksum = val; break;
    case PBSMH_int_pri:         mh->overlay4.int_pri = val; break;
    case PBSMH_lm_ctr_index:
        mh->overlay1.lm_counter_index_1 = val & 0x3f;
        mh->overlay1.lm_counter_index_2 = (val >> 6) & 0x3f;
        break;
    case PBSMH_oam_replacement_type:
        mh->overlay1.oam_replacement_type = val;
        break;
    case PBSMH_oam_replacement_offset:
        mh->overlay1.oam_replacement_offset = val;
        break;
    case PBSMH_ep_cpu_reasons: 
        mh->overlay2.ep_cpu_reason_code_1 = val & 0x7f;
        mh->overlay2.ep_cpu_reason_code_2 = (val >> 7) & 0xff;
        mh->overlay2.ep_cpu_reason_code_3 = (val >> 15) & 0x1f;
        break;
    case PBSMH_header_type:     mh->overlay1.header_type = val; break;
    case PBSMH_cell_error:      mh->overlay1.cell_error = val; break;

    case PBSMH_oam_ma_ptr:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            mh->overlay3.ma_ptr_2 = (val >> 7) & 0x7;
        } else
#endif
        {
            mh->overlay3.ma_ptr_2 = (val >> 7) & 0xf;
        }
        mh->overlay3.ma_ptr_1 = val & 0x7f;
        break;

    case PBSMH_ts_action:
        mh->overlay3.ts_action = val & 0x3;
        break;

    case PBSMH_sample_rdi:
        mh->overlay3.sample_rdi = val & 0x1;
        break;

    case PBSMH_lm_ctr1_index:

#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            mh->overlay5.ctr1_id_2 = (val >> 8) & 0x1;
            mh->overlay5.ctr1_id_1 = val  & 0xff;
        }else
#endif
        {
            mh->overlay3.ctr1_id_2 = (val >> 8) & 0x7;
            mh->overlay3.ctr1_id_1 = val  & 0xff;
        }
        break;

    case PBSMH_lm_ctr2_index:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            mh->overlay5.ctr2_id_2 = (val >> 5) & 0xf;
            mh->overlay5.ctr2_id_1 = val  & 0x1f;
        }else
#endif
        {        
            mh->overlay3.ctr2_id_2 = (val >> 3) & 0xff;
            mh->overlay3.ctr2_id_1 = val  & 0x7;
        }
        break;

    case PBSMH_ctr1_action:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            mh->overlay5.ctr1_action = (val & 3);
        }else
#endif
        {
            mh->overlay3.ctr1_action = (val & 3);
        }
        break;

    case PBSMH_ctr2_action:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            mh->overlay5.ctr2_action = (val & 3);
        }else
#endif
        {
            mh->overlay3.ctr2_action = (val & 3);
        }
        break;

    case PBSMH_pp_port:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            mh->overlay5.pp_port = (val & 0x1f);
        }else
#endif
        {
            mh->overlay3.pp_port = (val & 0xff);
        }
        break;

    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v8_field_get(int unit,
                       soc_pbsmh_v8_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    COMPILER_REFERENCE(unit);

    switch (field) {
    case PBSMH_start:         return mh->overlay1.start;
    case PBSMH_src_mod:       return mh->overlay1.src_mod;
    case PBSMH_dst_port:      return mh->overlay1.dst_pp_port;
    case PBSMH_cos:
       return (mh->overlay1.cos_lsb |
           (mh->overlay1.cos_msb_1 << 4) |
           (mh->overlay1.cos_msb_2 << 6));
    case PBSMH_pri:           return mh->overlay1.input_pri;
    case PBSMH_l3pbm_sel:     return mh->overlay1.set_l3bm;
    case PBSMH_l2pbm_sel:     return mh->overlay1.set_l2bm;
    case PBSMH_unicast:       return mh->overlay1.unicast;
    case PBSMH_tx_ts:         return mh->overlay1.tx_ts;
    case PBSMH_spid_override: return mh->overlay1.spid_override;
    case PBSMH_spid:          return mh->overlay1.spid;
    case PBSMH_spap:          return mh->overlay1.spap;
    case PBSMH_queue_num:
            return (mh->overlay1.queue_num_1 |
                    (mh->overlay1.queue_num_2 << 8));
    case PBSMH_osts:          return mh->overlay1.osts;
    case PBSMH_its_sign:      return mh->overlay1.its_sign;
    case PBSMH_hdr_offset:    return mh->overlay1.hdr_offset;
    case PBSMH_regen_udp_checksum: return mh->overlay1.regen_udp_checksum;
    case PBSMH_int_pri:       return mh->overlay4.int_pri;
    case PBSMH_lm_ctr_index:
        return (mh->overlay1.lm_counter_index_1 |
                (mh->overlay1.lm_counter_index_2 << 6));
    case PBSMH_oam_replacement_type:
        return mh->overlay1.oam_replacement_type;
    case PBSMH_oam_replacement_offset:
        return mh->overlay1.oam_replacement_offset;
    case PBSMH_ep_cpu_reasons: 
        return (mh->overlay2.ep_cpu_reason_code_1 |
                (mh->overlay2.ep_cpu_reason_code_2 << 7) |
                (mh->overlay2.ep_cpu_reason_code_3 << 15));
    case PBSMH_header_type:   return mh->overlay1.header_type;
    case PBSMH_cell_error:    return mh->overlay1.cell_error;
    case PBSMH_oam_ma_ptr:    return (mh->overlay3.ma_ptr_1 |
                                      (mh->overlay3.ma_ptr_2 << 7));
    case PBSMH_ts_action:     return (mh->overlay3.ts_action);
    case PBSMH_sample_rdi:    return (mh->overlay3.sample_rdi);
    case PBSMH_lm_ctr1_index:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            return (mh->overlay5.ctr1_id_1 |
                    (mh->overlay5.ctr1_id_2 << 8));
        }else
#endif
        {
            return (mh->overlay3.ctr1_id_1 |
                    (mh->overlay3.ctr1_id_2 << 8));
        }
    case PBSMH_lm_ctr2_index:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            return (mh->overlay5.ctr2_id_1 |
                    (mh->overlay5.ctr2_id_2 << 5));
        }else
#endif
        {
            return (mh->overlay3.ctr2_id_1 |
                    (mh->overlay3.ctr2_id_2 << 3));
        }
    case PBSMH_ctr1_action:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            return mh->overlay5.ctr1_action;
        }else
#endif
        {
            return mh->overlay3.ctr1_action;
        }
    case PBSMH_ctr2_action:
#if defined (BCM_METROLITE_SUPPORT)
        if(SOC_IS_METROLITE(unit)) {
            return mh->overlay5.ctr2_action;
        }else
#endif
        {
            return mh->overlay3.ctr2_action;
        }
    case PBSMH_pp_port:       return mh->overlay3.pp_port;

    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC void
soc_pbsmh_v9_field_set(int unit, soc_pbsmh_v9_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{

    switch (field) {
    case PBSMH_start:           mh->start       = val;
                                break;
    case PBSMH_src_mod:         mh->src_mod     = val; break;
    case PBSMH_dst_port:        mh->dst_port    = val; break;
    case PBSMH_cos:             mh->cos         = val; break;
    case PBSMH_pri:             mh->input_pri   = val; break;
    case PBSMH_l2pbm_sel:       mh->set_l2bm    = val; break;
    case PBSMH_queue_num:       mh->rqe_q_num   = val; break;
    case PBSMH_unicast:         mh->unicast     = val; break;
    case PBSMH_tx_ts:           mh->tx_ts       = val; break;
    case PBSMH_spid_override:   mh->spid_override = val; break;
    case PBSMH_spid:            mh->spid        = val; break;
    case PBSMH_spap:            mh->spap        = val; break;
    case PBSMH_osts:            mh->osts        = val; break;
    case PBSMH_its_sign:        mh->its_sign    = val; break;
    case PBSMH_hdr_offset:      mh->hdr_offset  = val; break;
    case PBSMH_cell_error:      mh->cell_error  = val; break;
    case PBSMH_header_type:     mh->header_type = val; break;
    case PBSMH_regen_udp_checksum: mh->regen_udp_checksum = val; break;

    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }   
}

STATIC uint32
soc_pbsmh_v9_field_get(int unit,
                       soc_pbsmh_v9_hdr_t *mh,
                       soc_pbsmh_field_t field)
{

    switch (field) {
    case PBSMH_start:         return mh->start;
    case PBSMH_src_mod:       return mh->src_mod;
    case PBSMH_dst_port:      return mh->dst_port;
    case PBSMH_cos:           return mh->cos;
    case PBSMH_pri:           return mh->input_pri;
    case PBSMH_l2pbm_sel:     return mh->set_l2bm;
    case PBSMH_queue_num:     return mh->rqe_q_num;
    case PBSMH_unicast:       return mh->unicast;
    case PBSMH_tx_ts:         return mh->tx_ts;
    case PBSMH_spid_override: return mh->spid_override;
    case PBSMH_spid:          return mh->spid;
    case PBSMH_spap:          return mh->spap;
    case PBSMH_osts:          return mh->osts;
    case PBSMH_its_sign:      return mh->its_sign;
    case PBSMH_hdr_offset:    return mh->hdr_offset;
    case PBSMH_cell_error:    return mh->cell_error;
    case PBSMH_header_type:   return mh->header_type;
    case PBSMH_regen_udp_checksum: return mh->regen_udp_checksum;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC void
soc_pbsmh_v11_field_set(int unit, soc_pbsmh_v11_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{

    switch (field) {
    case PBSMH_start:           mh->start       = val;
                                break;
    case PBSMH_src_mod:
        mh->src_mod_1     = (val >> 7) & 0x1;
        mh->src_mod_7     = val & 0x3f;
        break;
    case PBSMH_dst_port:        mh->dst_port    = val; break;
    case PBSMH_cos:             mh->cos         = val; break;
    case PBSMH_pri:             mh->input_pri   = val; break;
    case PBSMH_l2pbm_sel:       mh->set_l2bm    = val; break;
    case PBSMH_unicast:         mh->unicast     = val; break;
    case PBSMH_tx_ts:           mh->tx_ts       = val; break;
    case PBSMH_spid_override:   mh->spid_override = val; break;
    case PBSMH_spid:
        mh->spid_0        = (val >> 1) & 0x1;
        mh->spid_1        = val & 0x1;
        break;
    case PBSMH_spap:            mh->spap        = val; break;
    case PBSMH_osts:            mh->osts        = val; break;
    case PBSMH_its_sign:        mh->its_sign    = val; break;
    case PBSMH_hdr_offset:      mh->hdr_offset  = val; break;
    case PBSMH_cell_error:      mh->cell_error  = val; break;
    case PBSMH_header_type:     mh->header_type = val; break;
    case PBSMH_regen_udp_checksum: mh->regen_udp_checksum = val; break;
    case PBSMH_dst_subport_num:
        mh->dst_subport_num_hi     = (val >> 8) & 0x1;
        mh->dst_subport_num_lo     = val & 0xff;
        break;
    case PBSMH_amp_ts_act:       mh->amp_ts_act = val; break;
    case PBSMH_sobmh_flex:       mh->sobmh_flex = val; break;

    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_set: unit %d: Unknown pbsmh field=%d val=0x%x\n"),
                  unit, field, val));
        break;
    }
}

STATIC uint32
soc_pbsmh_v11_field_get(int unit,
                       soc_pbsmh_v11_hdr_t *mh,
                       soc_pbsmh_field_t field)
{

    switch (field) {
    case PBSMH_start:         return mh->start;
    case PBSMH_src_mod:       return mh->src_mod_1 << 7 | mh->src_mod_7;
    case PBSMH_dst_port:      return mh->dst_port;
    case PBSMH_cos:           return mh->cos;
    case PBSMH_pri:           return mh->input_pri;
    case PBSMH_l2pbm_sel:     return mh->set_l2bm;
    case PBSMH_unicast:       return mh->unicast;
    case PBSMH_tx_ts:         return mh->tx_ts;
    case PBSMH_spid_override: return mh->spid_override;
    case PBSMH_spid:          return mh->spid_0 << 1 | mh->spid_1;
    case PBSMH_spap:          return mh->spap;
    case PBSMH_osts:          return mh->osts;
    case PBSMH_its_sign:      return mh->its_sign;
    case PBSMH_hdr_offset:    return mh->hdr_offset;
    case PBSMH_cell_error:    return mh->cell_error;
    case PBSMH_header_type:   return mh->header_type;
    case PBSMH_regen_udp_checksum: return mh->regen_udp_checksum;
    case PBSMH_dst_subport_num: return  mh->dst_subport_num_hi << 8 | mh->dst_subport_num_lo;
    case PBSMH_amp_ts_act:     return mh->amp_ts_act;
    case PBSMH_sobmh_flex:     return mh->sobmh_flex;
    default:
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "pbsmh_get: unit %d: Unknown pbsmh field=%d\n"),
                  unit, field));
        return 0;
    }
}

STATIC uint32
soc_pbsmh_common_field_get(int unit, soc_pbsmh_field_attr_t *field_attr, 
                       soc_pbsmh_v10_hdr_t *mh,
                       soc_pbsmh_field_t field)
{
    uint32 val = 0;

    COMPILER_REFERENCE(unit);

    if (field < PBSMH_COUNT) {
        if (field_attr [field].size) {
            val = soc_pbsmh_array_get(mh -> hdr,  
                    field_attr [field].off, 
                    field_attr [field].size);
        }
    }
    else {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit, 
                             "pbsmh_common_field_get: unit %d: "
                             "Unknown pbsmh field=%d\n"), 
                  unit, field));
    }

    return val;
}


STATIC void
soc_pbsmh_common_field_set(int unit, soc_pbsmh_field_attr_t *field_attr, 
                       soc_pbsmh_v10_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    COMPILER_REFERENCE(unit);

    if (field < PBSMH_COUNT) {
        if (field_attr [field].size) {
            soc_pbsmh_array_set(val, mh -> hdr,  
                    field_attr [field].off, 
                    field_attr [field].size);
        } else {
            LOG_WARN(BSL_LS_SOC_COMMON, 
                     (BSL_META_U(unit, 
                                 "pbsmh_common_field_set: unit %d:"
                                 " Unknown size for pbsmh field=%d(%s)\n"),
                      unit, field, soc_pbsmh_field_to_name(unit, field)));
        }
    } else {
        LOG_WARN(BSL_LS_SOC_COMMON, 
                 (BSL_META_U(unit, 
                             "pbsmh_common_field_set: unit %d:"
                             " Unknown pbsmh field=%d\n"),
                  unit, field));
    }
}


void
soc_pbsmh_field_set(int unit, soc_pbsmh_hdr_t *mh,
                       soc_pbsmh_field_t field, uint32 val)
{
    switch(SOC_DCB_TYPE(unit)) {
    case 9:
    case 10:
    case 13:
        soc_pbsmh_v1_field_set(unit, mh, field, val);
        break;
    case 11:
    case 12:
    case 15:
    case 17:
    case 18:
        {
            soc_pbsmh_v2_hdr_t *mhv2 = (soc_pbsmh_v2_hdr_t *)mh;
            soc_pbsmh_v2_field_set(unit, mhv2, field, val);
            break;
        }
    case 14:
    case 19:
    case 20:
        {
            soc_pbsmh_v3_hdr_t *mhv3 = (soc_pbsmh_v3_hdr_t *)mh;
            soc_pbsmh_v3_field_set(unit, mhv3, field, val);
            break;
        }
    case 16:
    case 22:
        {
            soc_pbsmh_v4_hdr_t *mhv4 = (soc_pbsmh_v4_hdr_t *)mh;
            soc_pbsmh_v4_field_set(unit, mhv4, field, val);
            break;
        }
    case 21:
        {
            soc_pbsmh_v5_hdr_t *mhv5 = (soc_pbsmh_v5_hdr_t *)mh;
            soc_pbsmh_v5_field_set(unit, mhv5, field, val);
            break;
        }
    case 24:
        {
            soc_pbsmh_v6_hdr_t *mhv6 = (soc_pbsmh_v6_hdr_t *)mh;
            soc_pbsmh_v6_field_set(unit, mhv6, field, val);
            break;
        }
    case 23:
    case 26:
    case 30:
    case 31:
    case 34:
    case 37:
    case 40:
        {
            soc_pbsmh_v7_hdr_t *mhv7 = (soc_pbsmh_v7_hdr_t *)mh;
            soc_pbsmh_v7_field_set(unit, mhv7, field, val);
            break;
        }
    case 29:
        {
            soc_pbsmh_v8_hdr_t *mhv8 = (soc_pbsmh_v8_hdr_t *)mh;
            soc_pbsmh_v8_field_set(unit, mhv8, field, val);
            break;
        }
    case 32:
    case 38:
        {
            soc_pbsmh_v9_hdr_t *mhv9 = (soc_pbsmh_v9_hdr_t *)mh;
            soc_pbsmh_v9_field_set(unit, mhv9, field, val);
            break;
        }
    case 36:
        {
            soc_pbsmh_v11_hdr_t *mhv11 = (soc_pbsmh_v11_hdr_t *)mh;
            soc_pbsmh_v11_field_set(unit, mhv11, field, val);
            break;
        }
    case 33:
        {
            soc_pbsmh_v10_hdr_t *mhv10 = (soc_pbsmh_v10_hdr_t *)mh;
            soc_pbsmh_common_field_set(unit,
                        soc_pbsmh_v10_field_attr, mhv10, field, val);
            break;
        }
    case 35:
        {
            soc_pbsmh_v10_hdr_t *mhv10 = (soc_pbsmh_v10_hdr_t *)mh;
            soc_pbsmh_common_field_set(unit,
                        soc_pbsmh_v10_ap_field_attr, mhv10, field, val);
            break;
        }
    default:
        break;
    }
}

uint32
soc_pbsmh_field_get(int unit, soc_pbsmh_hdr_t *mh, soc_pbsmh_field_t field)
{
    switch(SOC_DCB_TYPE(unit)) {
    case 9:
    case 10:
    case 13:
        return soc_pbsmh_v1_field_get(unit, mh, field);
        break;
    case 11:
    case 12:
    case 15:
    case 17:
    case 18:
        {
            soc_pbsmh_v2_hdr_t *mhv2 = (soc_pbsmh_v2_hdr_t *)mh;
            return soc_pbsmh_v2_field_get(unit, mhv2, field);
            break;
        }
    case 14:
    case 19:
    case 20:
        {
            soc_pbsmh_v3_hdr_t *mhv3 = (soc_pbsmh_v3_hdr_t *)mh;
            return soc_pbsmh_v3_field_get(unit, mhv3, field);
            break;
        }
    case 16:
    case 22:
        {
            soc_pbsmh_v4_hdr_t *mhv4 = (soc_pbsmh_v4_hdr_t *)mh;
            return soc_pbsmh_v4_field_get(unit, mhv4, field);
            break;
        }
    case 21:
        {
            soc_pbsmh_v5_hdr_t *mhv5 = (soc_pbsmh_v5_hdr_t *)mh;
            return soc_pbsmh_v5_field_get(unit, mhv5, field);
            break;
        }
    case 24:
        {
            soc_pbsmh_v6_hdr_t *mhv6 = (soc_pbsmh_v6_hdr_t *)mh;
            return soc_pbsmh_v6_field_get(unit, mhv6, field);
            break;
        }
    case 23:
    case 26:
    case 30:
    case 31:
    case 34:
    case 37:
    case 40:
        {
            soc_pbsmh_v7_hdr_t *mhv7 = (soc_pbsmh_v7_hdr_t *)mh;
            return soc_pbsmh_v7_field_get(unit, mhv7, field);
            break;
        }
    case 29:
        {
            soc_pbsmh_v8_hdr_t *mhv8 = (soc_pbsmh_v8_hdr_t *)mh;
            return soc_pbsmh_v8_field_get(unit, mhv8, field);
            break;
        }
    case 32:
    case 38:
        {
            soc_pbsmh_v9_hdr_t *mhv9 = (soc_pbsmh_v9_hdr_t *)mh;
            return soc_pbsmh_v9_field_get(unit, mhv9, field);
            break;
        }
    case 36:
        {
            soc_pbsmh_v11_hdr_t *mhv11 = (soc_pbsmh_v11_hdr_t *)mh;
            return soc_pbsmh_v11_field_get(unit, mhv11, field);
            break;
        }
    case 33:
        {
            soc_pbsmh_v10_hdr_t *mhv10 = (soc_pbsmh_v10_hdr_t *)mh;
            return soc_pbsmh_common_field_get(unit,
                        soc_pbsmh_v10_field_attr, mhv10, field);
            break;
        }
    case 35:
        {
            soc_pbsmh_v10_hdr_t *mhv10 = (soc_pbsmh_v10_hdr_t *)mh;
            return soc_pbsmh_common_field_get(unit,
                        soc_pbsmh_v10_ap_field_attr, mhv10, field);
            break;
        }
    default:
        break;
    }
    return 0;
}

void
soc_pbsmh_dump(int unit, char *pfx, soc_pbsmh_hdr_t *mh)
{
    int field;

    switch(SOC_DCB_TYPE(unit)) {
    default:
        LOG_CLI((BSL_META_U(unit,
                            "%s<START=0x%02x COS=%d PRI=%d L3PBM_SEL=%d "
                 "SRC_MODID=%d DST_PORT=%d>\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_start),
                 soc_pbsmh_field_get(unit, mh, PBSMH_cos),
                 soc_pbsmh_field_get(unit, mh, PBSMH_pri),
                 soc_pbsmh_field_get(unit, mh, PBSMH_l3pbm_sel),
                 soc_pbsmh_field_get(unit, mh, PBSMH_src_mod),
                 soc_pbsmh_field_get(unit, mh, PBSMH_dst_port)));
        break;
    case 21:
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<START=0x%02x>\n"),
                 pfx,
                 ((uint8 *)mh)[0],
                 ((uint8 *)mh)[1],
                 ((uint8 *)mh)[2],
                 ((uint8 *)mh)[3],
                 soc_pbsmh_field_get(unit, mh, PBSMH_start)));
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<TX_TX=%d SPID_OVERRIDE=%d SPID=%d SPAP=%d\n"),
                 pfx,
                 ((uint8 *)mh)[4],
                 ((uint8 *)mh)[5],
                 ((uint8 *)mh)[6],
                 ((uint8 *)mh)[7],
                 soc_pbsmh_field_get(unit, mh, PBSMH_tx_ts),
                 soc_pbsmh_field_get(unit, mh, PBSMH_spid_override),
                 soc_pbsmh_field_get(unit, mh, PBSMH_spid),
                 soc_pbsmh_field_get(unit, mh, PBSMH_spap)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            "
                 "SET_L3BM=%d SET_L2BM=%d UNICAST=%d SRC_MODID=%d>\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_l3pbm_sel),
                 soc_pbsmh_field_get(unit, mh, PBSMH_l2pbm_sel),
                 soc_pbsmh_field_get(unit, mh, PBSMH_unicast),
                 soc_pbsmh_field_get(unit, mh, PBSMH_src_mod)));
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<INPUT_PRI=%d QUEUE_NUM=%d COS=%d LOCAL_DEST_PORT=%d>\n"),
                 pfx,
                 ((uint8 *)mh)[8],
                 ((uint8 *)mh)[9],
                 ((uint8 *)mh)[10],
                 ((uint8 *)mh)[11],
                 soc_pbsmh_field_get(unit, mh, PBSMH_pri),
                 soc_pbsmh_field_get(unit, mh, PBSMH_queue_num),
                 soc_pbsmh_field_get(unit, mh, PBSMH_cos),
                 soc_pbsmh_field_get(unit, mh, PBSMH_dst_port)));
        break;
    case 23:
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<START=0x%02x HEADER_TYPE=0x%02x\n"),
                 pfx,
                 ((uint8 *)mh)[0],
                 ((uint8 *)mh)[1],
                 ((uint8 *)mh)[2],
                 ((uint8 *)mh)[3],
                 soc_pbsmh_field_get(unit, mh, PBSMH_start),
                 soc_pbsmh_field_get(unit, mh, PBSMH_header_type)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            "
                 "LM_COUNTER_INDEX=0x%02x EP_CPU_REASON_CODE=0x%03x>\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_lm_ctr_index),
                 soc_pbsmh_field_get(unit, mh, PBSMH_ep_cpu_reasons)));
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<OAM_REPLACEMENT_OFFSET=0x%02x "
                            "OAM_REPLACMENT_TYPE=%d\n"),
                 pfx,
                 ((uint8 *)mh)[4],
                 ((uint8 *)mh)[5],
                 ((uint8 *)mh)[6],
                 ((uint8 *)mh)[7],
                 soc_pbsmh_field_get(unit, mh,
                 PBSMH_oam_replacement_offset),
                 soc_pbsmh_field_get(unit, mh,
                 PBSMH_oam_replacement_type)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            "
                 "OSTS=%d REGEN_UDP_CHECKSUM=%d ITS_SIGN=%d TX_TS=%d\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_osts),
                 soc_pbsmh_field_get(unit, mh, PBSMH_regen_udp_checksum),
                 soc_pbsmh_field_get(unit, mh, PBSMH_its_sign),
                 soc_pbsmh_field_get(unit, mh, PBSMH_tx_ts)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            "
                 "SET_L3BM=%d TS_HDR_OFFSET=0x%02x "
                            "SET_L2BM=%d\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_l3pbm_sel),
                 soc_pbsmh_field_get(unit, mh, PBSMH_hdr_offset),
                 soc_pbsmh_field_get(unit, mh, PBSMH_l2pbm_sel)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            "
                 "LOCAL_DEST_PORT=%d CELL_ERROR=%d>\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_dst_port),
                 soc_pbsmh_field_get(unit, mh, PBSMH_cell_error)));

        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                 "<INPUT_PRI=%d COS=%d SPID_OVERRIDE=%d "
                            "SPID=%d SPAP=%d\n"),
                 pfx,
                 ((uint8 *)mh)[8],
                 ((uint8 *)mh)[9],
                 ((uint8 *)mh)[10],
                 ((uint8 *)mh)[11],
                 soc_pbsmh_field_get(unit, mh, PBSMH_pri),
                 soc_pbsmh_field_get(unit, mh, PBSMH_cos),
                 soc_pbsmh_field_get(unit, mh, PBSMH_spid_override),
                 soc_pbsmh_field_get(unit, mh, PBSMH_spid),
                 soc_pbsmh_field_get(unit, mh, PBSMH_spap)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            "
                 "UNICAST=%d QUEUE_NUM=%d SRC_MODID=%d\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_unicast),
                 soc_pbsmh_field_get(unit, mh, PBSMH_queue_num),
                 soc_pbsmh_field_get(unit, mh, PBSMH_src_mod)));
        LOG_CLI((BSL_META_U(unit,
                            "%s            "
                 "NLF_PORT_NUMBER=%d>\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_nlf_port)));
        break;
    case 32:
    case 36:
    case 38:
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                            "<START=0x%02x HEADER_TYPE=0x%02x>\n"),
                 pfx,
                 ((uint8 *)mh)[0],
                 ((uint8 *)mh)[1],
                 ((uint8 *)mh)[2],
                 ((uint8 *)mh)[3],
                 soc_pbsmh_field_get(unit, mh, PBSMH_start),
                 soc_pbsmh_field_get(unit, mh, PBSMH_header_type)));
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                            "<TX_TS=%d ITS_SIGN=%d REGEN_UDP_CHECKSUM=%d " 
                            "OSTS=%d\n"),
                 pfx,
                 ((uint8 *)mh)[4],
                 ((uint8 *)mh)[5],
                 ((uint8 *)mh)[6],
                 ((uint8 *)mh)[7],
                 soc_pbsmh_field_get(unit, mh, PBSMH_tx_ts),                      
                 soc_pbsmh_field_get(unit, mh, PBSMH_its_sign),
                 soc_pbsmh_field_get(unit, mh, PBSMH_regen_udp_checksum),
                 soc_pbsmh_field_get(unit, mh, PBSMH_osts)));
        LOG_CLI((BSL_META_U(unit,
                            "%s                   "
                            "CELL_ERROR=%d TS_HDR_OFFSET=0x%2x "
                            "LOCAL_DEST_PORT=%d>\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_cell_error),
                 soc_pbsmh_field_get(unit, mh, PBSMH_hdr_offset),
                 soc_pbsmh_field_get(unit, mh, PBSMH_dst_port)));
        LOG_CLI((BSL_META_U(unit,
                            "%s0x%02x%02x%02x%02x "
                            "<SPID_OVERRIDE=%d RQE_NUM=%d INPUT_PRI=%d SPAP=%d "
                            "SPID=%d COS=%d\n"),
                 pfx,
                 ((uint8 *)mh)[8],
                 ((uint8 *)mh)[9],
                 ((uint8 *)mh)[10],
                 ((uint8 *)mh)[11],
                 soc_pbsmh_field_get(unit, mh, PBSMH_spid_override),
                 soc_pbsmh_field_get(unit, mh, PBSMH_queue_num),
                 soc_pbsmh_field_get(unit, mh, PBSMH_pri),      
                 soc_pbsmh_field_get(unit, mh, PBSMH_spap),
                 soc_pbsmh_field_get(unit, mh, PBSMH_spid),
                 soc_pbsmh_field_get(unit, mh, PBSMH_cos)));
        LOG_CLI((BSL_META_U(unit,
                            "%s                   "
                            "UNICAST=%d SET_L2BM=%d SRC_MODID=%d>\n"),
                 pfx,
                 soc_pbsmh_field_get(unit, mh, PBSMH_unicast),
                 soc_pbsmh_field_get(unit, mh, PBSMH_l2pbm_sel),
                 soc_pbsmh_field_get(unit, mh, PBSMH_src_mod)));
        break;


    case 33:
    case 35:
        for (field = 0; field < PBSMH_COUNT; ++field) {
            uint32 val = soc_pbsmh_field_get(unit, mh, (soc_pbsmh_field_t) field);
            LOG_CLI((BSL_META_U(unit, 
                                "%-25s=0x%X (%d)\n"), 
                     soc_pbsmh_field_to_name(unit, field), 
                     val, val));
        }
        break;
    }
}




#endif /* BCM_XGS3_SWITCH_SUPPORT */
