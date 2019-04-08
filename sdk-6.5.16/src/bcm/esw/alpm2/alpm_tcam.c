/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 * 
 * File:    alpm_tcam.c
 * Purpose: ALPM TCAM management. device independent implementation.
 * Requires:
 */

/* Implementation notes:
 */
#include <shared/bsl.h>

#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/lpm.h>
#include <soc/trident2.h>
#include <soc/tomahawk.h>
#include <shared/bsl.h>

#include <shared/util.h>
#include <shared/l3.h>

#if defined(ALPM_ENABLE)

#include <bcm/l3.h>

#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/alpm.h>
#include <bcm_int/esw/alpm_util.h>

/* Internal function declaration */

#define _TCAM_ZONE_CNT  4
#define _TCAM_ZID_CNT   32

typedef struct _tcam_zone_state_s {
    int start;  /* start index for this prefix length */
    int end;    /* End index for this prefix length */
    int prev;   /* Prev (Lo to Hi) prefix length with non zero entry count */
    int next;   /* Next (Hi to Lo) prefix length with non zero entry count */
    int vent;   /* valid entries */
    int fent;   /* free entries */
} _tcam_zone_state_t;

typedef enum _tz_db_e {
    _tcamZoneDBInvalid = 0,
    _tcamZoneDBV4All = 1,
    _tcamZoneDBV4Prv = 2,
    _tcamZoneDBV4Glb = 3,
    _tcamZoneDBV6All = 4,
    _tcamZoneDBV6Prv = 5,
    _tcamZoneDBV6Glb = 6,
    _tcamZoneDBMixAll = 7,
    _tcamZoneDBMixPrv = 8,
    _tcamZoneDBMixGlb = 9,
    _tcamZoneDBCnt = 10,
} _tz_db_t;

char *_tz_db_str[] = {
    "Invalid",
    "V4",
    "V4 Private",
    "V4 Global ",
    "V6",
    "V6 Private",
    "V6 Global ",
    "Mixed",
    "Mixed Private",
    "Mixed Global ",
    "Count"
};

typedef struct _tcam_zone_s {
    _tcam_zone_state_t   *state;

    /* TCAM Zone management */
    _tz_db_t        tz_db_type;
    uint16          tz_num_inst;
    uint16          tz_ent_per_inst;
    uint16          tz_min_v6;
    uint16          tz_base_idx;
    uint16          tz_tbl_sz;

    /* Calculated variables */
    uint16          _tz_ent_cnt;
    uint16          _tz_max_idx;
} _tcam_zone_t;

typedef struct _tcam_ctrl_s {
    _tcam_zone_t *tcam_zn[_TCAM_ZID_CNT];
    int          tcam_zn_cnt;
} _tcam_ctrl_t;

typedef struct _tcam_zone_st_s {
    int cnt_used;
    int cnt_free;
} _tcam_zone_st_t;

_tcam_ctrl_t *_tcam_control[SOC_MAX_NUM_DEVICES];

#define TCAMC(u)                         (_tcam_control[u])
#define _TCAM_INIT_CHECK(u)              (TCAMC(u) != NULL)
#define _TCAMZ_CNT(u)                    (TCAMC(u)->tcam_zn_cnt)
#define _TCAMZ(u, zid)                   (TCAMC(u)->tcam_zn[zid])
#define _TCAM_STATE(u, zid)              (_TCAMZ(u, zid)->state)
#define _TCAM_STATE_START(u, zid, pfx)   (_TCAMZ(u, zid)->state[pfx].start)
#define _TCAM_STATE_END(u, zid, pfx)     (_TCAMZ(u, zid)->state[pfx].end)
#define _TCAM_STATE_PREV(u, zid, pfx)    (_TCAMZ(u, zid)->state[pfx].prev)
#define _TCAM_STATE_NEXT(u, zid, pfx)    (_TCAMZ(u, zid)->state[pfx].next)
#define _TCAM_STATE_VENT(u, zid, pfx)    (_TCAMZ(u, zid)->state[pfx].vent)
#define _TCAM_STATE_FENT(u, zid, pfx)    (_TCAMZ(u, zid)->state[pfx].fent)

#define _TCAMZ_INITED(u, zid)            (_TCAMZ(u, zid) && _TCAMZ(u, zid)->tz_num_inst != 0)
#define _TCAMZ_DB_TYPE(u, zid)           (_TCAMZ(u, zid)->tz_db_type)
#define _TCAMZ_PFX_MIN_V6(u, zid)        (_TCAMZ(u, zid)->tz_min_v6)
#define _TCAMZ_PFX_ENT_PER_INST(u, zid)  (_TCAMZ(u, zid)->tz_ent_per_inst)
#define _TCAMZ_PFX_BASE_IDX(u, zid)      (_TCAMZ(u, zid)->tz_base_idx)
#define _TCAMZ_PFX_TBL_SZ(u, zid)        (_TCAMZ(u, zid)->tz_tbl_sz)
#define _TCAMZ_PFX_MAX_ENTS(u, zid)      (_TCAMZ(u, zid)->_tz_ent_cnt)
#define _TCAMZ_PFX_MAX_IDX(u, zid)       (_TCAMZ(u, zid)->_tz_max_idx)

#define _PFX_IS_PKM_64B(u, zid, pfx)     ((pfx) % _TCAMZ_PFX_ENT_PER_INST(u, zid) \
                                         >= _TCAMZ_PFX_MIN_V6(u, zid) && \
                                         (pfx) != _TCAMZ_PFX_MAX_IDX(u, zid))
/*
 * P: Paired, Z: Zoned, MC: IPMC, VRF 0: Global.
 * PKM: 32B(0) !32B(1)
 *
 * ------------------------------------------
 * | MC |  P | Z | VRF | PKM |  Zone | Inst |
 * ------------------------------------------
 * | 0  |  0 | 0 | 0   | 32B |  Mix  | 0,2  | 0b00000 0  ZoneA
 * | 0  |  0 | 0 | 0   | 64B |  Mix  | 0,2  | 0b00001 1  -->ZoneA
 * | 0  |  0 | 0 | 1   | 32B |  Mix  | 1    | 0b00010 2  -->ZoneA
 * | 0  |  0 | 0 | 1   | 64B |  Mix  | 1    | 0b00011 3  -->ZoneA
 * | 0  |  0 | 1 | 0   | 32B |  MxG  | 0,1  | 0b00100 4  B
 * | 0  |  0 | 1 | 0   | 64B |  MxG  | 0,1  | 0b00101 5  -->ZoneB
 * | 0  |  0 | 1 | 1   | 32B |  MxP  | 0    | 0b00110 6  C
 * | 0  |  0 | 1 | 1   | 64B |  MxP  | 0    | 0b00111 7  -->ZoneC
 * | 0  |  1 | 0 | 0   | 32B |  V4   | 0,2  | 0b01000 8  D
 * | 0  |  1 | 0 | 0   | 128 |  V6   | 0,2  | 0b01001 9  E
 * | 0  |  1 | 0 | 1   | 32B |  V4   | 1    | 0b01010 10 -->ZoneD
 * | 0  |  1 | 0 | 1   | 128 |  V6   | 1    | 0b01011 11 -->ZoneE
 * | 0  |  1 | 1 | 0   | 32B |  V4G  | 0,1  | 0b01100 12 F
 * | 0  |  1 | 1 | 0   | 128 |  V6G  | 0,1  | 0b01101 13 G
 * | 0  |  1 | 1 | 1   | 32B |  V4P  | 0    | 0b01110 14 H
 * | 0  |  1 | 1 | 1   | 128 |  V6P  | 0    | 0b01111 15 I
 * ------------------------------------------
 * ------------------------------------------
 * | MC |  P | Z | VRF | PKM |  Zone | Inst |
 * ------------------------------------------
 * | 1  |  0 | 0 | 0   | 32B |  Mix  | 3,5  | 0b10000 -->ZoneA
 * | 1  |  0 | 0 | 0   | 64B |  Mix  | 3,5  | 0b10001 -->ZoneA
 * | 1  |  0 | 0 | 1   | 32B |  Mix  | 4    | 0b10010 -->ZoneA
 * | 1  |  0 | 0 | 1   | 64B |  Mix  | 4    | 0b10011 -->ZoneA
 * | 1  |  0 | 1 | 0   | 32B |  MxP  | 1,3  | 0b10100 -->ZoneC
 * | 1  |  0 | 1 | 0   | 64B |  MxP  | 1,3  | 0b10101 -->ZoneC
 * | 1  |  0 | 1 | 1   | 32B |  MxP  | 2    | 0b10110 -->ZoneC
 * | 1  |  0 | 1 | 1   | 64B |  MxP  | 2    | 0b10111 -->ZoneC
 * | 1  |  1 | 0 | 0   | 32B |  V4   | 3,5  | 0b11000 -->ZoneD
 * | 1  |  1 | 0 | 0   | 128 |  V6   | 3,5  | 0b11001 -->ZoneE
 * | 1  |  1 | 0 | 1   | 32B |  V4   | 4    | 0b11010 -->ZoneD
 * | 1  |  1 | 0 | 1   | 128 |  V6   | 4    | 0b11011 -->ZoneE
 * | 1  |  1 | 1 | 0   | 32B |  V4P  | 1,3  | 0b11100 -->ZoneH
 * | 1  |  1 | 1 | 0   | 128 |  V6P  | 1,3  | 0b11101 -->ZoneI
 * | 1  |  1 | 1 | 1   | 32B |  V4P  | 2    | 0b11110 -->ZoneH
 * | 1  |  1 | 1 | 1   | 128 |  V6P  | 2    | 0b11111 -->ZoneI
 * ------------------------------------------
 */

/* Zone combination count */
typedef enum tz_comb_e {
    tZoneInvalid,
    tZoneAC64,       /* 1 */
    tZoneBP64Glb,    /* 2 */
    tZoneCP64Prv,    /* 3 */
    tZoneDC128V4,    /* 4 */
    tZoneEC128V6,    /* 5 */
    tZoneFP128V4Glb, /* 6 */
    tZoneGP128V6Glb, /* 7 */
    tZoneHP128V4Prv, /* 8 */
    tZoneIP128V6Prv, /* 9 */
    tZone__Count
} tz_comb_t;

static _tz_db_t tcam_zone_type[tZone__Count] = {
    _tcamZoneDBInvalid,
    _tcamZoneDBMixAll,  /* ZoneA, Combined-64 */
    _tcamZoneDBMixGlb,  /* ZoneB, Parallel-64 (Global) */
    _tcamZoneDBMixPrv,  /* ZoneC, Parallel-64 (Private) */
    _tcamZoneDBV4All,   /* ZoneD, Combined-128 (V4) */
    _tcamZoneDBV6All,   /* ZoneE, Combined-128 (V6) */
    _tcamZoneDBV4Glb,   /* ZoneF, Parallel-128 (V4 Global) */
    _tcamZoneDBV6Glb,   /* ZoneG, Parallel-128 (V6 Global) */
    _tcamZoneDBV4Prv,   /* ZoneH, Parallel-128 (V4 Private) */
    _tcamZoneDBV6Prv,   /* ZoneI, Parallel-128 (V6 Private) */
};

/* Number of instance in the zone */
static uint16 tcam_zone_num_inst[tZone__Count] = {
    0,
    3,                  /* ZoneA */
    2,                  /* ZoneB */
    1,                  /* ZoneC */
    3,                  /* ZoneD */
    3,                  /* ZoneE */
    2,                  /* ZoneF */
    2,                  /* ZoneG */
    1,                  /* ZoneH */
    1,                  /* ZoneI */
};

static uint16 tcam_zone_num_inst_ipmc_war[tZone__Count] = {
    0,
    6,                  /* ZoneA */
    2,                  /* ZoneB */
    4,                  /* ZoneC */
    6,                  /* ZoneD */
    6,                  /* ZoneE */
    2,                  /* ZoneF */
    2,                  /* ZoneG */
    4,                  /* ZoneH */
    4,                  /* ZoneI */
};

/* 4 means 4 combinations for Pair_128_mode + alpm_mode
 * _TCAM_ZONE_CNT:
 *    bit1 : 0 Glb, 1 Prv
 *    bit0 : 0 V4,  1 V6
 */
static tz_comb_t   tz_conf[4][_TCAM_ZONE_CNT] = {
    /* Combined-64 */
    {tZoneAC64, tZoneInvalid, tZoneInvalid, tZoneInvalid},
    /* Combined-128 bit0 V4|V6 */
    {tZoneDC128V4, tZoneEC128V6, tZoneInvalid, tZoneInvalid},
    /* Parallel-64  bit1 Glb|Prv */
    {tZoneBP64Glb, tZoneInvalid, tZoneCP64Prv, tZoneInvalid},
    /* Parallel-128 bit1 Glb|Prv bit0 V4|V6 */
    {tZoneFP128V4Glb, tZoneGP128V6Glb, tZoneHP128V4Prv, tZoneIP128V6Prv},
};

#define _TCAMZ_ZID_TO_ZN(zid)  (zid & 0x3)
#define _TCAMZ_ZID_MAKE_FROM_ZN(mc, p, z, zn)   \
    (!!(mc) << 4 | !!(p) << 3 | !!(z) << 2 | (zn & 0x3))
#define _TCAMZ_ZID_MAKE(mc, p, z, vrf, pkm)     \
    (!!(mc) << 4 | !!(p) << 3 | !!(z) << 2 | !!(vrf) << 1 | !!(pkm))
#define _TCAMZ_ZID_IS_V6_ONLY(zid)  ((zid) & 0x1)

static _tcam_zone_t tz_ctrl[SOC_MAX_NUM_DEVICES][_TCAM_ZONE_CNT];

/*
 *   TCAM (1 Zone)                  >>>> Combined + 64B.
 * --------------- index 0
 * |             |
 * |    Zone 0   |      ---- _tcamZoneDBMixAll
 * |             |
 * --------------- index size-1
 *
 *   TCAM (2 Zones)                 >>>> Parallel + 64B.
 * --------------- index 0
 * |    Zone 0   |      ---- _tcamZoneDBMixPrv
 * --------------- index size/2-1
 * |    Zone 1   |      ---- _tcamZoneDBMixGlb
 * --------------- index size-1
 *
 *   TCAM (2 Zones)                 >>>> Combined + 128B.
 * --------------- index 0
 * |    Zone 0   |      ---- _tcamZoneDBV4All
 * --------------- index size(L3_DEFIP_PAIR_128)-1
 * --------------- index 0
 * |    Zone 1   |      ---- _tcamZoneDBV6All
 * --------------- index size(L3_DEFIP)-1
 *
 *   TCAM (4 Zones)                 >>>> Parallel + 128B.
 * --------------- index 0
 * |    Zone 0   |      ---- _tcamZoneDBV4Prv
 * --------------- index size(L3_DEFIP_PAIR_128)/2-1
 * |    Zone 1   |      ---- _tcamZoneDBV4Glb
 * --------------- index size(L3_DEFIP_PAIR_128)-1
 * --------------- index 0
 * |    Zone 2   |      ---- _tcamZoneDBV6Prv
 * --------------- index size(L3_DEFIP)/2-1
 * |    Zone 3   |      ---- _tcamZoneDBV6Glb
 * --------------- index size(L3_DEFIP)-1
 */
void
_tcam_zone_ctrl_config(int u, int zn, _tz_db_t db_type, uint16 num_inst,
                       uint16 base_idx, uint16 tbl_sz)
{
    uint16 ent_per_inst = 0, v6_min = 0;
    _tcam_zone_t *tz;

    tz = &tz_ctrl[u][zn];

    switch (db_type) {
        case _tcamZoneDBV4All:
        case _tcamZoneDBV4Prv:
        case _tcamZoneDBV4Glb:
            ent_per_inst = 32 + 2 + 1;
            v6_min = ent_per_inst + 1;
            break;
        case _tcamZoneDBV6All:
        case _tcamZoneDBV6Prv:
        case _tcamZoneDBV6Glb:
            ent_per_inst = 128 + 2 + 1;
            v6_min = 0;
            break;
        case _tcamZoneDBMixAll:
        case _tcamZoneDBMixPrv:
        case _tcamZoneDBMixGlb:
            ent_per_inst = 64 + 32 + 2 + 1;
            v6_min = 32 + 1;
            break;
        default:
            assert(0);
            break;
    }

    tz->tz_db_type      = db_type;
    tz->tz_num_inst     = num_inst;
    tz->tz_ent_per_inst = ent_per_inst;
    tz->tz_min_v6       = v6_min;
    tz->tz_tbl_sz       = tbl_sz;
    tz->tz_base_idx     = base_idx;
    tz->_tz_ent_cnt     = num_inst * ent_per_inst;
    tz->_tz_max_idx     = tz->_tz_ent_cnt - 1;

    return ;
}

int
_tcam_zone_db_type_to_pkm(int u, _tz_db_t db_type)
{
    int pkm = ALPM_PKM_32B;
    switch (db_type) {
        case _tcamZoneDBV6All:
        case _tcamZoneDBV6Prv:
        case _tcamZoneDBV6Glb:
            pkm = ALPM_PKM_128;
            break;
        default:
            break;
    }

    return pkm;
}

int
_tcam_zone_init(int u)
{
    int rv = BCM_E_NONE, zn, zid, j, comb;
    int max_pfx_len, pfx_st_sz, zone_cnt = 0;

    int          pkm;
    _tz_db_t db_type;
    uint16  num_inst;
    uint16  base_idx;
    int       tbl_sz;

    comb = !!ALPM_TCAM_ZONED(u) << 1 | !!ALPM_128B(u);
    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        tz_comb_t tzc = tz_conf[comb][zn];
        if (tzc == tZoneInvalid) {
            continue;
        }

        db_type = tcam_zone_type[tzc];
        pkm = _tcam_zone_db_type_to_pkm(u, db_type);
        tbl_sz = tcam_table_size(u, pkm) >> !!ALPM_TCAM_ZONED(u);
        if (db_type == _tcamZoneDBV6Glb ||
            db_type == _tcamZoneDBV4Glb ||
            db_type == _tcamZoneDBMixGlb) {
            base_idx = tbl_sz;
        } else {
            base_idx = 0;
        }

        num_inst = (soc_feature(u, soc_feature_td3_lpm_ipmc_war)) ?
                    tcam_zone_num_inst_ipmc_war[tzc] :
                    tcam_zone_num_inst[tzc];
        _tcam_zone_ctrl_config(u, zn, db_type, num_inst,
                               base_idx, (uint16)tbl_sz);
        zone_cnt ++;
    }

    TCAMC(u)->tcam_zn_cnt = zone_cnt;

    /* Mapping to tz_ctrl(uc) */
    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int zn2 = zn;
        tz_comb_t tzc;

        tzc = tz_conf[comb][zn];
        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u), zn);
        if (tzc == tZoneInvalid) {
            /* IP type don't care */
            if (!ALPM_128B(u) && (zn & 0x1)) {
                zn2 = zn2 & 0x2;
            }
            /* VRF type don't care */
            if (!ALPM_TCAM_ZONED(u) && (zn & 0x2)) {
                zn2 = zn2 & 0x1;
            }
        }
        _TCAMZ(u, zid) = &tz_ctrl[u][zn2];
        ALPM_VERB(("uc zid %d -> zn %d\n", zid, zn2));
    }

    if (soc_feature(u, soc_feature_td3_lpm_ipmc_war)) {
        for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
            int zn2 = zn;

            zid = _TCAMZ_ZID_MAKE_FROM_ZN(1, ALPM_128B(u), ALPM_TCAM_ZONED(u), zn);

            /* IP type don't care */
            if (!ALPM_128B(u) && (zn & 0x1)) {
                zn2 = zn2 & 0x2;
            }

            if (ALPM_TCAM_ZONED(u)) {
                /* VRF type don't care */
                if (!(zn & 0x2)) {
                    zn2 = zn2 | 0x2;
                }
            } else {
                /* VRF type don't care */
                if (zn & 0x2) {
                    zn2 = zn2 & 0x1;
                }
            }
            _TCAMZ(u, zid) = &tz_ctrl[u][zn2];
            ALPM_VERB(("mc zid %d -> zn %d\n", zid, zn2));
        }
    }

    /* TCAM State init */
    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int max_idx, zid;

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u), zn);
        if (_TCAMZ(u, zid) == NULL) {
            continue;
        }

        max_idx = _TCAMZ_PFX_MAX_IDX(u, zid);
        max_pfx_len = _TCAMZ_PFX_MAX_ENTS(u, zid);
        pfx_st_sz = sizeof(_tcam_zone_state_t) * max_pfx_len;
        ALPM_REALLOC_EG(_TCAM_STATE(u, zid), pfx_st_sz, "LPM_pfx_info");
        for (j = 0; j < max_pfx_len; j++) {
            _TCAM_STATE_START(u, zid, j)  = -1;
            _TCAM_STATE_END(u, zid, j)    = -1;
            _TCAM_STATE_PREV(u, zid, j)   = -1;
            _TCAM_STATE_NEXT(u, zid, j)   = -1;
            _TCAM_STATE_VENT(u, zid, j)   = 0;
            _TCAM_STATE_FENT(u, zid, j)   = 0;
        }

        _TCAM_STATE_FENT(u, zid, max_idx) = _TCAMZ_PFX_TBL_SZ(u, zid);
        _TCAM_STATE_END(u, zid, max_idx) = _TCAMZ_PFX_BASE_IDX(u, zid) - 1;
    }

bad:
    return rv;
}

static int
_tcam_zone_zid_get(int u, int vrf, int ipt, int in_pair_mode, int mc)
{
    int zid;
    mc = soc_feature(u, soc_feature_td3_lpm_ipmc_war) && mc;
    zid = _TCAMZ_ZID_MAKE(mc, in_pair_mode, ALPM_TCAM_ZONED(u),
                          !ALPM_VRF_IS_GBL(u, vrf), ipt);
    return zid;
}

int
_tcam_zone_inst_id_get(int u, int vrf, int mc)
{
    int inst_id;

    if (ALPM_TCAM_ZONED(u)) {
        if (soc_feature(u, soc_feature_td3_lpm_ipmc_war) && mc) {
            switch (vrf) {
            case BCM_L3_VRF_GLOBAL:
                inst_id = 1;
                break;
            case BCM_L3_VRF_OVERRIDE:
                inst_id = 3;
                break;
            default:
                inst_id = 2;
                break;
            }
        } else {
            switch (vrf) {
            case BCM_L3_VRF_GLOBAL:
                inst_id = 0;
                break;
            case BCM_L3_VRF_OVERRIDE:
                inst_id = 1;
                break;
            default:
                inst_id = 0;
                break;
            }
        }
    } else {
        if (soc_feature(u, soc_feature_td3_lpm_ipmc_war) && mc) {
            switch (vrf) {
            case BCM_L3_VRF_GLOBAL:
                inst_id = 3;
                break;
            case BCM_L3_VRF_OVERRIDE:
                inst_id = 5;
                break;
            default:
                inst_id = 4;
                break;
            }
        } else {
            switch (vrf) {
            case BCM_L3_VRF_GLOBAL:
                inst_id = 0;
                break;
            case BCM_L3_VRF_OVERRIDE:
                inst_id = 2;
                break;
            default:
                inst_id = 1;
                break;
            }
        }
    }

    return inst_id;
}

static void
_tcam_zone_status(int u, int zid, _tcam_zone_st_t *tz_st)
{
    int i, max_pfx_len;

    sal_memset(tz_st, 0, sizeof(*tz_st));
    max_pfx_len = _TCAMZ_PFX_MAX_IDX(u, zid);
    for (i = max_pfx_len; i >= 0 ; i--) {
        if (i != _TCAMZ_PFX_MAX_IDX(u, zid) && (_TCAM_STATE_START(u, zid, i) == -1)) {
            continue;
        }

        tz_st->cnt_free += _TCAM_STATE_FENT(u, zid, i);
        tz_st->cnt_used += _TCAM_STATE_VENT(u, zid, i);
    }

    return ;
}

static int
_tcam_trie_lookup(int u, _bcm_defip_cfg_t *lpm_cfg, int *key_index)
{
    int              rv = BCM_E_NOT_FOUND;
    alpm_lib_trie_t  *pvt_trie;
    _alpm_pvt_node_t *pvt_node;

    int             ipt = ALPM_LPM_IPT(u, lpm_cfg);
    int             vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u), vrf_id, ipt);
    if (pvt_trie != NULL) {
        rv = alpm_lib_trie_search(pvt_trie, lpm_cfg->user_data,
                    lpm_cfg->defip_sub_len,
                    (alpm_lib_trie_node_t **)&pvt_node);
        if (BCM_SUCCESS(rv)) {
            *key_index = PVT_IDX(pvt_node);
        }
    }
    return rv;
}

static int
_tcam_trie_update(int u, uint32 *key, int len, int vrf_id, int ipt, int key_idx)
{
    int                 rv = BCM_E_NOT_FOUND;
    alpm_lib_trie_t     *pvt_trie;
    _alpm_pvt_node_t    *pvt_node;

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u), vrf_id, ipt);
    if (pvt_trie != NULL) {
        rv = alpm_lib_trie_search(pvt_trie, key, len, (alpm_lib_trie_node_t **)&pvt_node);
        if (BCM_SUCCESS(rv)) {
            PVT_IDX(pvt_node) = key_idx;
        }
    }

    return rv;
}

int
_tcam_trie_update_by_cfg(int u, _bcm_defip_cfg_t *lpm_cfg, int key_idx)
{
    int rv;
    int ipt = ALPM_LPM_IPT(u, lpm_cfg);
    int vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);

    rv = _tcam_trie_update(u, lpm_cfg->user_data, lpm_cfg->defip_sub_len,
                           vrf_id, ipt, key_idx);

    return rv;
}

int
tcam_entry_valid(int u, int pkm, void *e, int sub_idx)
{
    int vld = 0;

    vld = ALPM_DRV(u)->tcam_entry_valid(u, pkm, e, sub_idx);

    return vld;
}

static int
_tcam_entry_valid_set(int u, int pkm, void *e, int sub_idx, int val)
{
    int rv = BCM_E_NONE;

    rv = ALPM_DRV(u)->tcam_entry_valid_set(u, pkm, e, sub_idx, val);

    return rv;
}

static int
_tcam_trie_update_by_ent(int u, int pkm, void *e, int hw_idx)
{
    int         i, rv = BCM_E_NONE;
    uint32      pfx[5];
    int         pfx_len, vrf_id;

    if (pkm == ALPM_PKM_32B) {
        /* For IPv4(32B packing mode), check and update both 2 entries */
        for (i = 0; i < 2; i++) {
            if (tcam_entry_valid(u, pkm, e, i)) {
                ALPM_IER(tcam_entry_vrf_id_get(u, pkm, e, i, &vrf_id));
                ALPM_IER(tcam_entry_pfx_len_get(u, pkm, e, i, &pfx_len));
                ALPM_IER(alpm_trie_ent_to_pfx(u, pkm, e, i, pfx_len, pfx));
                rv = _tcam_trie_update(u, pfx, pfx_len, vrf_id, ALPM_PKM2IPT(pkm),
                                       hw_idx << 1 | i);
            }
        }
    } else {
        if (tcam_entry_valid(u, pkm, e, 0)) {
            ALPM_IER(tcam_entry_vrf_id_get(u, pkm, e, 0, &vrf_id));
            ALPM_IER(tcam_entry_pfx_len_get(u, pkm, e, 0, &pfx_len));
            ALPM_IER(alpm_trie_ent_to_pfx(u, pkm, e, 0, pfx_len, pfx));
            rv = _tcam_trie_update(u, pfx, pfx_len, vrf_id, ALPM_PKM2IPT(pkm), hw_idx);
        }
    }

    return rv;
}

/**********************************************
* TCAM Management functions *
 */

/*
 * TCAM based PIVOT implementation. Each table entry can hold two IPV4 PIVOTs or
 * one IPV6 PIVOT entry. VRF independent routes placed at the beginning or
 * at the end of table based on application provided entry vrf id
 * (BCM_L3_VRF_OVERRIDE/BCM_L3_VRF_GLOBAL).
 *
 *              _PFX_MAX_INDEX
 * lpm_prefix_index[98].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    0                      ==
 * lpm_prefix_index[98].end   ---> ===============================
 *
 * lpm_prefix_index[97].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV6  Prefix Len = 64  ==
 * lpm_prefix_index[97].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[x].begin --->  ===============================
 *                                 ==                           ==
 *                                 ==                           ==
 * lpm_prefix_index[x].end   --->  ===============================
 *
 *
 *              _PFX_MIN_V6
 * lpm_prefix_index[33].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV6  Prefix Len = 0   ==
 * lpm_prefix_index[33].end   ---> ===============================
 *
 *
 * lpm_prefix_index[32].begin ---> ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 32  ==
 * lpm_prefix_index[32].end   ---> ===============================
 *
 *
 *
 * lpm_prefix_index[0].begin --->  ===============================
 *                                 ==                           ==
 *                                 ==    IPV4  Prefix Len = 0   ==
 * lpm_prefix_index[0].end   --->  ===============================
 */

int
tcam_entry_vrf_id_get(int u, int pkm, void *e, int sub_idx, int *vrf_id)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_vrf_id_get(u, pkm, e, sub_idx, vrf_id);

    return rv;
}

int
tcam_entry_pfx_len_get(int u, int pkm, void *e, int sub_idx, int *pfx_len)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_pfx_len_get(u, pkm, e, sub_idx, pfx_len);

    return rv;
}

/*
 * x | y
 * --------------------
 * 0 | 0 : entry0_to_0
 * 0 | 1 : entry0_to_1
 * 1 | 0 : entry1_to_0
 * 1 | 1 : entry1_to_1
 * src and dst can be same
 */
static int
_tcam_entry_x_to_y(int u, int pkm, void *src, void *dst,
                   int copy_hit, int x, int y)
{
    int         rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_entry_x_to_y(u, pkm, src, dst, copy_hit, x, y);

    return rv;
}

static int
_tcam_entry_from_cfg(int u, int pkm, _bcm_defip_cfg_t *lpm_cfg, void *e, int x, uint32 write_flags)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_from_cfg(u, pkm, lpm_cfg, e, x, write_flags);
    return rv;
}

int
tcam_entry_to_cfg(int u, int pkm, void *e, int x, _bcm_defip_cfg_t *lpm_cfg)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_to_cfg(u, pkm, e, x, lpm_cfg);
    return rv;
}

int
tcam_entry_read(int u, int pkm, void *e, int index, int s_index)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u);
    rv = ALPM_DRV(u)->mem_entry_read(u, acb, acb->pvt_tbl[pkm], index, e, FALSE);

    return rv;
}

int
tcam_entry_read_no_cache(int u, int pkm, void *e, int index, int s_index)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u);
    rv = ALPM_DRV(u)->mem_entry_read(u, acb, acb->pvt_tbl[pkm], index, e, TRUE);

    return rv;
}

int
_tcam_entry_write(int u, int pkm, void *e, int index, int s_index)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u);
    rv = ALPM_DRV(u)->mem_entry_write(u, acb, NULL, acb->pvt_tbl[pkm], index, e);

    return rv;
}

/*
 *           key[4] = {key[0], key[1], key[2], key[3]}
 * ipv4:     key[4] =
 *               {IP_ADDR0f, 0, 0, 0}
 * ipv6-64:  key[4] =
 *               {0, 0, IP_ADDR0f, IP_ADDR1f}
 * ipv6-128: key[4] =
 *               {IP_ADDR0_LWRf, IP_ADDR1_LWRf, IP_ADDR0_UPRf, IP_ADDR1_UPRf}
 */
int
tcam_entry_to_key(int u, int pkm, void *e, int sub_idx, uint32 *key)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_to_key(u, pkm, e, sub_idx, key);

    return rv;
}

/*
 *      Create a slot for the new entry rippling the entries if required
 */
static int
_tcam_entry_shift(int u, int pkm, int from_ent, int to_ent)
{
    int         rv;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];

    ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
    rv = _tcam_trie_update_by_ent(u, pkm, e, to_ent);
    if (BCM_SUCCESS(rv)) {
        rv = _tcam_entry_write(u, pkm, e, to_ent, from_ent);
    }

    return rv;
}

/*
 * Lower index -> Higher index
 * Higher priority -> Lower priority
 * L3_DEFIP_PAIR_128 -> L3_DEFIP
 *  --------  0
 *  |      |
 *  |      |  |
 *  |      | \|/
 *  |      |  *
 *  |      |
 *  -------- 1023
 */
static int
_tcam_shift_pfx_up(int u, int pfx, int zid)
{
    int         pkm;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         from_ent;
    int         to_ent;
    uint32      v0, v1;
    int         rv = BCM_E_NONE;

    to_ent = _TCAM_STATE_END(u, zid, pfx) + 1;

    pkm = _tcam_zone_db_type_to_pkm(u, _TCAMZ_DB_TYPE(u, zid));
    if (pkm != ALPM_PKM_128) {
        pkm = _PFX_IS_PKM_64B(u, zid, pfx);
    }

    if (pkm == ALPM_PKM_32B) {
        from_ent = _TCAM_STATE_END(u, zid, pfx);
        ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, pkm, e, 0);
        v1 = tcam_entry_valid(u, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            _tcam_trie_update_by_ent(u, pkm, e, to_ent);
            ALPM_IER(_tcam_entry_write(u, pkm, e, to_ent, from_ent));
            to_ent--;
        }
    }

    from_ent = _TCAM_STATE_START(u, zid, pfx);
    if (from_ent != to_ent) {
        ALPM_IER(_tcam_entry_shift(u, pkm, from_ent, to_ent));
    }
    _TCAM_STATE_START(u, zid, pfx) += 1;
    _TCAM_STATE_END(u, zid, pfx) += 1;

    return rv;
}

/*
 * Higher index -> Lower index
 * Lower priority -> Higher priority
 * L3_DEFIP -> L3_DEFIP_PAIR_128
 *
 *  --------  0
 *  |      |
 *  |      |  *
 *  |      | /|\
 *  |      |  |
 *  |      |
 *  -------- 1023
 */
static int
_tcam_shift_pfx_down(int u, int pfx, int zid)
{
    int         pkm;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         from_ent;
    int         to_ent;
    int         prev_ent;
    uint32      v0, v1;
    int         rv = BCM_E_NONE;

    to_ent = _TCAM_STATE_START(u, zid, pfx) - 1;

    /* Don't move empty prefix . */
    if (_TCAM_STATE_VENT(u, zid, pfx) == 0) {
        _TCAM_STATE_START(u, zid, pfx) = to_ent;
        _TCAM_STATE_END(u, zid, pfx) = to_ent - 1;
        return BCM_E_NONE;
    }

    pkm = _tcam_zone_db_type_to_pkm(u, _TCAMZ_DB_TYPE(u, zid));
    if (pkm != ALPM_PKM_128) {
        pkm = _PFX_IS_PKM_64B(u, zid, pfx);
    }

    if (pkm == ALPM_PKM_32B &&
        _TCAM_STATE_END(u, zid, pfx) != _TCAM_STATE_START(u, zid, pfx)) {

        from_ent = _TCAM_STATE_END(u, zid, pfx);

        ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, pkm, e, 0);
        v1 = tcam_entry_valid(u, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            /* Shift entry before last to start - 1 position. */
            prev_ent = from_ent - 1;
            ALPM_IER(_tcam_entry_shift(u, pkm, prev_ent, to_ent));

            _tcam_trie_update_by_ent(u, pkm, e, prev_ent);
            ALPM_IER(_tcam_entry_write(u, pkm, e, prev_ent, from_ent));
        } else {
            /* Last entry is full -> just shift it to start - 1  position. */
            _tcam_trie_update_by_ent(u, pkm, e, to_ent);
            ALPM_IER(_tcam_entry_write(u, pkm, e, to_ent, from_ent));
        }

    } else  {
        from_ent = _TCAM_STATE_END(u, zid, pfx);
        ALPM_IER(_tcam_entry_shift(u, pkm, from_ent, to_ent));
    }
    _TCAM_STATE_START(u, zid, pfx) -= 1;
    _TCAM_STATE_END(u, zid, pfx) -= 1;

    return rv;
}

/*
 *      Create a slot for the new entry rippling the entries if required
 *      returned update=1 if half-free entry, update=0 if full-free entry.
 */
static int
_tcam_free_slot_create(int u, int pkm, int zid, int pfx, int *free_slot, int *update)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         prev_pfx;
    int         next_pfx;
    int         free_pfx;
    int         curr_pfx;
    int         from_ent;
    uint32      v0, v1;

    if (_TCAM_STATE_VENT(u, zid, pfx) == 0) {
        /*
         * Find the  prefix position. Only prefix with valid
         * entries are in the list.
         * next -> high to low prefix. low to high index
         * prev -> low to high prefix. high to low index
         * Unused prefix length _PFX_MAX_INDEX is the head of the
         * list and is node corresponding to this is always
         * present.
         */
        curr_pfx = _TCAMZ_PFX_MAX_IDX(u, zid);
        while (_TCAM_STATE_NEXT(u, zid, curr_pfx) > pfx) {
            curr_pfx = _TCAM_STATE_NEXT(u, zid, curr_pfx);
        }
        /* Insert the new prefix */
        next_pfx = _TCAM_STATE_NEXT(u, zid, curr_pfx);
        if (next_pfx != -1) {
            _TCAM_STATE_PREV(u, zid, next_pfx) = pfx;
        }
        _TCAM_STATE_NEXT(u, zid, pfx) = _TCAM_STATE_NEXT(u, zid, curr_pfx);
        _TCAM_STATE_PREV(u, zid, pfx) = curr_pfx;
        _TCAM_STATE_NEXT(u, zid, curr_pfx) = pfx;

        _TCAM_STATE_FENT(u, zid, pfx) =
            (_TCAM_STATE_FENT(u, zid, curr_pfx) + 1) / 2;
        _TCAM_STATE_FENT(u, zid, curr_pfx) -= _TCAM_STATE_FENT(u, zid, pfx);
        _TCAM_STATE_START(u, zid, pfx) = _TCAM_STATE_END(u, zid, curr_pfx) +
                                    _TCAM_STATE_FENT(u, zid, curr_pfx) + 1;
        _TCAM_STATE_END(u, zid, pfx) = _TCAM_STATE_START(u, zid, pfx) - 1;
        _TCAM_STATE_VENT(u, zid, pfx) = 0;
    } else if (pkm == ALPM_PKM_32B) {
        /* For IPv4 Check if alternate entry is free */
        from_ent = _TCAM_STATE_START(u, zid, pfx);
        ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, pkm, e, 0);
        v1 = tcam_entry_valid(u, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
            *update = 1; /* half-free entry */
            return BCM_E_NONE;
        }

        from_ent = _TCAM_STATE_END(u, zid, pfx);
        ALPM_IER(tcam_entry_read(u, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, pkm, e, 0);
        v1 = tcam_entry_valid(u, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
            *update = 1; /* half-free entry */
            return BCM_E_NONE;
        }
    }

    free_pfx = pfx;
    while (_TCAM_STATE_FENT(u, zid, free_pfx) == 0) {
        /* pkm <= ALPM_PKM_64B: starting from higher index.
         * else starting from lower index
         */
        free_pfx = _TCAM_STATE_NEXT(u, zid, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on this side try the other side */
            free_pfx = pfx;
            break;
        }
    }

    while (_TCAM_STATE_FENT(u, zid, free_pfx) == 0) {
        free_pfx = _TCAM_STATE_PREV(u, zid, free_pfx);
        if (free_pfx == -1) {
            if (_TCAM_STATE_VENT(u, zid, pfx) == 0) {
                /* We failed to allocate entries for a newly allocated prefix.*/
                prev_pfx = _TCAM_STATE_PREV(u, zid, pfx);
                next_pfx = _TCAM_STATE_NEXT(u, zid, pfx);
                if (-1 != prev_pfx) {
                    _TCAM_STATE_NEXT(u, zid, prev_pfx) = next_pfx;
                }
                if (-1 != next_pfx) {
                    _TCAM_STATE_PREV(u, zid, next_pfx) = prev_pfx;
                }
            }
            return BCM_E_FULL;
        }
    }

    /*
     * Ripple entries to create free space
     */
    while (free_pfx > pfx) {
        next_pfx = _TCAM_STATE_NEXT(u, zid, free_pfx);
        ALPM_IER(_tcam_shift_pfx_down(u, next_pfx, zid));
        _TCAM_STATE_FENT(u, zid, free_pfx) -= 1;
        _TCAM_STATE_FENT(u, zid, next_pfx) += 1;
        free_pfx = next_pfx;
    }

    while (free_pfx < pfx) {
        ALPM_IER(_tcam_shift_pfx_up(u, free_pfx, zid));
        _TCAM_STATE_FENT(u, zid, free_pfx) -= 1;
        prev_pfx = _TCAM_STATE_PREV(u, zid, free_pfx);
        _TCAM_STATE_FENT(u, zid, prev_pfx) += 1;
        free_pfx = prev_pfx;
    }

    _TCAM_STATE_VENT(u, zid, pfx) += 1;
    _TCAM_STATE_FENT(u, zid, pfx) -= 1;
    _TCAM_STATE_END(u, zid, pfx) += 1;

    *free_slot = _TCAM_STATE_END(u, zid, pfx) << ((pkm) ? 0 : 1);
    *update = 0; /* full-free entry */

    return BCM_E_NONE;
}

/*
 * Delete a slot and adjust entry pointers if required.
 */
static int
_tcam_free_slot_delete(int u, int pkm, int zid, int pfx, int slot)
{
    int         prev_pfx, next_pfx;
    int         fidx, tidx;
    uint32      ef[SOC_MAX_MEM_FIELD_WORDS];
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    void        *et;
    int         rv = BCM_E_NONE;

    fidx    = _TCAM_STATE_END(u, zid, pfx);
    tidx    = slot;

    if (pkm == ALPM_PKM_32B) {
        tidx >>= 1;
        ALPM_IER(tcam_entry_read(u, pkm, ef, fidx, fidx));
        if (fidx != tidx) {
            ALPM_IER(tcam_entry_read(u, pkm, e, tidx, tidx));
            et = e;
        } else {
            et = ef;
        }

        if (tcam_entry_valid(u, pkm, ef, 1)) {
            rv = _tcam_entry_x_to_y(u, pkm, ef, et, TRUE, 1, slot & 1);
            (void)_tcam_entry_valid_set(u, pkm, ef, 1, 0);
        } else {
            rv = _tcam_entry_x_to_y(u, pkm, ef, et, TRUE, 0, slot & 1);
            (void)_tcam_entry_valid_set(u, pkm, ef, 0, 0);
            _TCAM_STATE_VENT(u, zid, pfx) -= 1;
            _TCAM_STATE_FENT(u, zid, pfx) += 1;
            _TCAM_STATE_END(u, zid, pfx) -= 1;
        }

        /* not true for OVERRIDE routes */
        if (tidx != fidx) {
            ALPM_IER(_tcam_trie_update_by_ent(u, pkm, et, tidx));
            ALPM_IER(_tcam_entry_write(u, pkm, et, tidx, tidx));
        }
        ALPM_IER(_tcam_trie_update_by_ent(u, pkm, ef, fidx));
        ALPM_IER(_tcam_entry_write(u, pkm, ef, fidx, fidx));
    } else { /* IPV6 */
        _TCAM_STATE_VENT(u, zid, pfx) -= 1;
        _TCAM_STATE_FENT(u, zid, pfx) += 1;
        _TCAM_STATE_END(u, zid, pfx)  -= 1;
        if (tidx != fidx) {
            ALPM_IER(tcam_entry_read(u, pkm, ef, fidx, fidx));
            ALPM_IER(_tcam_trie_update_by_ent(u, pkm, ef, tidx));
            ALPM_IER(_tcam_entry_write(u, pkm, ef, tidx, fidx));
        }
        /* v6-64 indices are still raw in pivot tcam */
        sal_memset(ef, 0, sizeof(ef));
        ALPM_IER(_tcam_entry_write(u, pkm, ef, fidx, fidx));
    }

    if (_TCAM_STATE_VENT(u, zid, pfx) == 0) {
        /* remove from the list */
        prev_pfx = _TCAM_STATE_PREV(u, zid, pfx); /* Always present */
        assert(prev_pfx != -1);
        next_pfx = _TCAM_STATE_NEXT(u, zid, pfx);
        _TCAM_STATE_NEXT(u, zid, prev_pfx) = next_pfx;
        _TCAM_STATE_FENT(u, zid, prev_pfx) += _TCAM_STATE_FENT(u, zid, pfx);
        _TCAM_STATE_FENT(u, zid, pfx) = 0;
        if (next_pfx != -1) {
            _TCAM_STATE_PREV(u, zid, next_pfx) = prev_pfx;
        }
        _TCAM_STATE_NEXT(u, zid, pfx) = -1;
        _TCAM_STATE_PREV(u, zid, pfx) = -1;
        _TCAM_STATE_START(u, zid, pfx) = -1;
        _TCAM_STATE_END(u, zid, pfx) = -1;
    }

    return rv;
}

/*
 * Extract vrf weighted prefix lenght from vrf_id, pkm, key_len.
 */
static void
tcam_pfx_len_get(int u, int ipv6, int zid, int inst_id, int key_len, int *pfx_len)
{
    int ipv6_min;

    ipv6_min = _TCAMZ_PFX_MIN_V6(u, zid);

    if (ipv6) {
        key_len += ipv6_min;
    }

    /* In ALPM the arragnement of VRF is at he begining followed by VRF
       override and global */
    *pfx_len = key_len + inst_id * _TCAMZ_PFX_ENT_PER_INST(u, zid);
    return;
}

static void
_tcam_gpfx_len_get(
    int u, int defip_vrf, int ipv6, int zid,
    _bcm_defip_cfg_t *lpm_cfg, int *pfx_len)
{
    int inst_id;

    /* Calculate vrf weighted prefix lengh. */
    inst_id = _tcam_zone_inst_id_get(u, defip_vrf, lpm_cfg->defip_flags & BCM_L3_IPMC);
    tcam_pfx_len_get(u, ipv6, zid, inst_id, lpm_cfg->defip_sub_len, pfx_len);

    return;
}

static int
_tcam_cfg_write(int u, _bcm_defip_cfg_t *lpm_cfg, int index, int s_index,
                int update, uint32 write_flags)
{
    int         rv = BCM_E_NONE;
    int         hw_idx;
    int         pkm = ALPM_LPM_PKM(u, lpm_cfg);
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    _alpm_tcam_write_t *tcam_write = lpm_cfg->tcam_write;

    hw_idx = (pkm == ALPM_PKM_32B) ? index >> 1 : index;

    /* Entry already present. Update the entry */
    if (pkm == ALPM_PKM_32B) {
        if (update) {
            ALPM_IER(tcam_entry_read(u, pkm, e, hw_idx, s_index));
        } else {
            sal_memset(e, 0, sizeof(e));
        }
        /* Need to clear entry if both entry0 & 1 are invalid? */
        rv = _tcam_entry_from_cfg(u, pkm, lpm_cfg, e, index & 1, write_flags);
    } else {
        sal_memset(e, 0, sizeof(e));
        rv = _tcam_entry_from_cfg(u, pkm, lpm_cfg, e, 0, write_flags);
    }

    if (BCM_SUCCESS(rv)) {
        if (tcam_write != NULL) { /* case new L1 pvt add during L2 bkt split */
            tcam_write->hw_idx = hw_idx;
            sal_memcpy(tcam_write->ent, e, sizeof(e));
        } else {
            rv = _tcam_entry_write(u, pkm, e, hw_idx, s_index);
        }
    }

    return rv;
}

/*
 * _tcam_cfg_match (Exact match for the key. Will match both IP address
 * and mask)
 *
 * OUT index_ptr: return key location
 * OUT pfx_len: Key prefix length. vrf+32+pfx len for single wide(64b)
 */
static int
_tcam_cfg_match(int u, _bcm_defip_cfg_t *lpm_cfg, int *index_ptr)
{
    int         key_index = 0;
    int         rv = BCM_E_NONE;

    if (_tcam_trie_lookup(u, lpm_cfg, &key_index) == BCM_E_NONE) {
        *index_ptr = key_index;
    } else {
        rv = BCM_E_NOT_FOUND;
    }

    return rv;
}

static int
_tcam_cfg_insert(int u, int zid, _bcm_defip_cfg_t *lpm_cfg, uint32 write_flags)
{
    int rv;
    int index, pfx, pkm, update = 0; /* for 32B only */

    pkm = ALPM_LPM_PKM(u, lpm_cfg);
    _tcam_gpfx_len_get(u, lpm_cfg->defip_vrf, !!pkm, zid, lpm_cfg, &pfx);
    rv = _tcam_free_slot_create(u, pkm, zid, pfx, &index, &update);
    if (BCM_SUCCESS(rv)) {
        lpm_cfg->defip_index = index;
        rv = _tcam_cfg_write(u, lpm_cfg, index, index, update, write_flags);
    }

    return rv;
}

int
bcm_esw_alpm_tcam_avail(int u, int vrf_id, int ipt, int key_len, int mc)
{
    int pfx, zid, fent, cnt = 0;

    if (!TCAMC(u)) {
        return 0;
    }

    zid = _tcam_zone_zid_get(u, ALPM_VRF_ID_TO_VRF(u, vrf_id), ipt, ALPM_128B(u), mc);
    pfx = _TCAMZ_PFX_MAX_IDX(u, zid);
    while (pfx >= 0) {
        fent = _TCAM_STATE_FENT(u, zid, pfx);
        if (fent > 0) {
            cnt += fent;
        }
        pfx = _TCAM_STATE_NEXT(u, zid, pfx);
    }

    return cnt;
}

int
bcm_esw_alpm_tcam_state_free_get(int u, int zn, int *free_cnt, int *used_cnt)
{
    int i, zn_min, zn_max;
    int fcnt = 0, vcnt = 0;

    if (!TCAMC(u)) {
        return BCM_E_NONE;
    }

    if (zn < 0 || zn >= _TCAM_ZONE_CNT) {
        zn_min = 0;
        zn_max = _TCAM_ZONE_CNT - 1;
    } else {
        zn_min = zn_max = zn;
    }

    for (i = zn_min; i <= zn_max; i++) {
        int zid;
        _tcam_zone_st_t tz_st;

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u), i);
        if (!_TCAMZ_INITED(u, zid)) {
            continue;
        }
        _tcam_zone_status(u, zid, &tz_st);

        fcnt += tz_st.cnt_free;
        vcnt += tz_st.cnt_used;
    }

    if (free_cnt) {
        *free_cnt = fcnt;
    }
    if (used_cnt) {
        *used_cnt = vcnt;
    }

    return BCM_E_NONE;
}

void
bcm_esw_alpm_tcam_zone_state_dump(int u)
{
    int i, zn;
    int max_pfx_len;

    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int zid;

        if (tz_conf[!!ALPM_TCAM_ZONED(u) << 1 | !!ALPM_128B(u)][zn] == tZoneInvalid) {
            continue;
        }

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u), zn);
        if (!_TCAMZ_INITED(u, zid)) {
            continue;
        }

        cli_out("Zone %d (%s) :\n", zn, _tz_db_str[_TCAMZ_DB_TYPE(u, zid)]);
        max_pfx_len = _TCAMZ_PFX_MAX_IDX(u, zid);
        for (i = max_pfx_len; i >= 0 ; i--) {
            if ((i != _TCAMZ_PFX_MAX_IDX(u, zid)) &&
                _TCAM_STATE_START(u, zid, i) == -1 &&
                _TCAM_STATE_FENT(u, zid, i) == 0) {
                continue;
            }

            cli_out("\tPFX = %3d P = %3d N = %3d START = %5d END = %5d "
                    "VENT = %5d FENT = %5d\n", i,
                    _TCAM_STATE_PREV(u, zid, i),
                    _TCAM_STATE_NEXT(u, zid, i),
                    _TCAM_STATE_START(u, zid, i),
                    _TCAM_STATE_END(u, zid, i),
                    _TCAM_STATE_VENT(u, zid, i),
                    _TCAM_STATE_FENT(u, zid, i));
        }
        cli_out("\n");
    }
}

void
bcm_esw_alpm_tcam_zone_dump(int u, char *pfx_str)
{
    int zn;
    int free_tcam = 0, used_tcam = 0, tot_tcam;

    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int zid;

        if (tz_conf[!!ALPM_TCAM_ZONED(u) << 1 | !!ALPM_128B(u)][zn] == tZoneInvalid) {
            continue;
        }

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u), zn);
        if (!_TCAMZ_INITED(u, zid)) {
            continue;
        }

        (void)bcm_esw_alpm_tcam_state_free_get(u, zn, &free_tcam, &used_tcam);
        tot_tcam = used_tcam + free_tcam;
        if (tot_tcam == 0) {
            tot_tcam = 1; /* avoid divide by 0 */
        }
        cli_out("%sTCAM zone %d(%s): %4d (used), %4d (free), Usage: %d.%d%%\n",
                pfx_str, zn, _tz_db_str[_TCAMZ_DB_TYPE(u, zid)],
                used_tcam, free_tcam,
                used_tcam * 100 / tot_tcam,
                (used_tcam * 1000 / tot_tcam) % 10);
    }

    return ;
}

/*
 * De-initialize the start/end tracking pointers for each prefix length
 */
int
bcm_esw_alpm_tcam_deinit(int u)
{
    int i, zid;

    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return BCM_E_UNAVAIL;
    }

    for (zid = 0; zid < _TCAM_ZID_CNT; zid++) {
        if (_TCAMZ_INITED(u, zid) &&
            _TCAM_STATE(u, zid) != NULL) {
            alpm_util_free(_TCAM_STATE(u, zid));
            _TCAM_STATE(u, zid) = NULL;
        }
    }

    for (i = 0; i < _TCAM_ZID_CNT; i++) {
        _TCAMZ(u, i) = NULL;
    }

    if (_TCAM_INIT_CHECK(u)) {
        alpm_util_free(TCAMC(u));
        TCAMC(u) = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Initialize the start/end tracking pointers for each prefix length
 */
int
bcm_esw_alpm_tcam_init(int u)
{
    int rv = BCM_E_NONE;

    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return BCM_E_UNAVAIL;
    }

    if (_TCAM_INIT_CHECK(u)) {
        /* this is a reinit. clean up old state */
        if (bcm_esw_alpm_tcam_deinit(u) < 0) {
            return BCM_E_UNAVAIL;
        }
    }

    ALPM_ALLOC_EG(TCAMC(u), sizeof(_tcam_ctrl_t), "TCAMC");
    ALPM_IEG(_tcam_zone_init(u));

    return rv;

bad:
    (void)bcm_esw_alpm_tcam_deinit(u);
    return rv;
}

/*
 * ALPM TCAM cleanup
 */
int
bcm_esw_alpm_tcam_cleanup(int u)
{
    int rv = BCM_E_NONE;

    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return BCM_E_UNAVAIL;
    }

    ALPM_REALLOC_EG(TCAMC(u), sizeof(_tcam_ctrl_t), "TCAMC");
    ALPM_IEG(_tcam_zone_init(u));

bad:
    return rv;
}

/*
 * Implementation using _mem_read/write using entry rippling technique
 * Advantage: A completely sorted table is not required. Lookups can be slow
 * as it will perform a linear search on the entries for a given prefix length.
 * No device access necessary for the search if the table is cached. Auxiliary
 * Small number of entries need to be moved around (97 worst case)
 * for performing insert/update/delete. However CPU needs to do all
 * the work to move the entries.
 */

/*
 * bcm_esw_alpm_tcam_insert
 * For IPV4 assume only both IP_ADDR0 is valid
 * Moving multiple entries around in h/w vs  doing a linear search in s/w
 */
int
bcm_esw_alpm_tcam_insert(int u, _bcm_defip_cfg_t *lpm_cfg, uint32 write_flags)
{
    int         index;
    int         rv = BCM_E_NONE;

    rv = _tcam_cfg_match(u, lpm_cfg, &index);
    if (rv == BCM_E_NOT_FOUND) {
        int zid;
        zid = _tcam_zone_zid_get(u, lpm_cfg->defip_vrf,
                                 ALPM_LPM_IPT(u, lpm_cfg), ALPM_128B(u),
                                 lpm_cfg->defip_flags & BCM_L3_IPMC);
        rv = _tcam_cfg_insert(u, zid, lpm_cfg, write_flags);
    } else if (rv == BCM_E_NONE) {
        /* Found */
        rv = _tcam_cfg_write(u, lpm_cfg, index, index, 1, write_flags);
    }

    return rv;
}

/*
 * bcm_esw_alpm_tcam_delete
 */
int
bcm_esw_alpm_tcam_delete(int u, _bcm_defip_cfg_t *lpm_cfg)
{
    int         index, ipt;
    int         rv = BCM_E_NONE;

    rv = _tcam_cfg_match(u, lpm_cfg, &index);
    if (rv == BCM_E_NONE) {
        int zid, pkm, pfx, defip_vrf;
        defip_vrf = lpm_cfg->defip_vrf;

        pkm = ALPM_LPM_PKM(u, lpm_cfg);
        ipt = ALPM_PKM2IPT(pkm);
        zid = _tcam_zone_zid_get(u, defip_vrf, ipt, ALPM_128B(u),
                                 lpm_cfg->defip_flags & BCM_L3_IPMC);
        lpm_cfg->defip_index = index;
        _tcam_gpfx_len_get(u, defip_vrf, ipt, zid, lpm_cfg, &pfx);
        rv = _tcam_free_slot_delete(u, pkm, zid, pfx, index);
    }

    if (BCM_SUCCESS(rv)) {
        ; /* VRF_PIVOT_REF_DEC(u, vrf_id, vrf, pkm); */
    }

    return(rv);
}

/*
 * bcm_esw_alpm_tcam_match
 * (Exact match for the key. Will match both IP address and mask)
 */
int
bcm_esw_alpm_tcam_match(int u, _bcm_defip_cfg_t *lpm_cfg, int *index_ptr)
{
    int     rv;
    int     hw_idx;
    int     pkm = ALPM_LPM_PKM(u, lpm_cfg);
    uint32  e[SOC_MAX_MEM_FIELD_WORDS];

    rv = _tcam_cfg_match(u, lpm_cfg, index_ptr);
    if (BCM_SUCCESS(rv)) {
        /*
         * If entry is ipv4 copy to the "zero" half of the,
         * buffer, "zero" half of lpm_entry if the  original entry
         * is in the "one" half.
         */
        hw_idx = *index_ptr;
        if (pkm == ALPM_PKM_32B) {
            hw_idx >>= 1;
        }

        ALPM_IER(tcam_entry_read(u, pkm, e, hw_idx, hw_idx));

        if (pkm == ALPM_PKM_32B) {
            rv = tcam_entry_to_cfg(u, pkm, e, (*index_ptr & 0x1), lpm_cfg);
        } else {
            rv = tcam_entry_to_cfg(u, pkm, e, 0, lpm_cfg);
        }
    }
    return rv;
}

/* ********************* *
 * TCAM warmboot support *
 */

/*
 * Function:
 *      tcam_wb_reinit_done
 * Purpose:
 *      Update all TCAM state once entry reinit done from warmboot.
 * Parameters:
 *      u        - (IN)Device unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
tcam_wb_reinit_done(int u)
{
    int idx;
    int prev_idx;
    int defip_table_size;
    int zn;

    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int zid, pkm;

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u), zn);

        if (!_TCAMZ_INITED(u, zid)) {
            continue;
        }

        prev_idx = _TCAMZ_PFX_MAX_IDX(u, zid);
        pkm = _tcam_zone_db_type_to_pkm(u, _TCAMZ_DB_TYPE(u, zid));
        defip_table_size = tcam_table_size(u, pkm);

        if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED)) {
            /* Combined search mode */
            _TCAM_STATE_PREV(u, zid, _TCAMZ_PFX_MAX_IDX(u, zid)) = -1;

            for (idx = _TCAMZ_PFX_MAX_IDX(u, zid); idx > -1; idx--) {
                if (_TCAM_STATE_START(u, zid, idx) == -1) {
                    continue;
                }

                _TCAM_STATE_PREV(u, zid, idx) = prev_idx;
                _TCAM_STATE_NEXT(u, zid, prev_idx) = idx;

                _TCAM_STATE_FENT(u, zid, prev_idx) =                   \
                                 _TCAM_STATE_START(u, zid, idx) -      \
                                 _TCAM_STATE_END(u, zid, prev_idx) - 1;
                prev_idx = idx;

            }

            _TCAM_STATE_NEXT(u, zid, prev_idx) = -1;
            _TCAM_STATE_FENT(u, zid, prev_idx) =                       \
                             defip_table_size -                       \
                             _TCAM_STATE_END(u, zid, prev_idx) - 1;
        } else {
            int tab_idx_max = 0;
            /* Parallel search mode
             *
             *    Configured VRF Range
             *    {0 - (ALPM_MAX_PFX_INDEX/3)}
             *    Global VRF Range
             *    {((ALPM_MAX_PFX_INDEX/1) + 1) - (ALPM_MAX_PFX_INDEX/2) }
             *    Override VRF Range
             *    {((ALPM_MAX_PFX_INDEX/2) + 1) - (ALPM_MAX_PFX_INDEX) }
             */

            /*
             * Global and Overide VRF range are treated as single block and
             * both blocks will be linked
             */
            _TCAM_STATE_PREV(u, zid, _TCAMZ_PFX_MAX_IDX(u, zid)) = -1;
            for (idx = _TCAMZ_PFX_MAX_IDX(u, zid); idx > -1; idx--) {
                if (-1 == _TCAM_STATE_START(u, zid, idx)) {
                    continue;
                }

                _TCAM_STATE_PREV(u, zid, idx) = prev_idx;
                _TCAM_STATE_NEXT(u, zid, prev_idx) = idx;

                _TCAM_STATE_FENT(u, zid, prev_idx) =                    \
                                 _TCAM_STATE_START(u, zid, idx) -       \
                                 _TCAM_STATE_END(u, zid, prev_idx) - 1;

                prev_idx = idx;
            }
            /*
             * _TCAM_ZONE_CNT:
             *    bit1 : 0 Glb, 1 Prv
             *    bit0 : 0 V4,  1 V6
             */
            if (zn & 0x2) {
                tab_idx_max = defip_table_size / 2;
            } else {
                tab_idx_max = defip_table_size;
            }

            _TCAM_STATE_NEXT(u, zid, prev_idx) = -1;
            _TCAM_STATE_FENT(u, zid, prev_idx) =
                tab_idx_max - _TCAM_STATE_END(u, zid, prev_idx) - 1;
        }

    } /* for zn */

    return (BCM_E_NONE);
}

/*
 * Function:
 *      tcam_wb_reinit
 * Purpose:
 *      TCAM entry state reinit from warmboot.
 * Parameters:
 *      u        - (IN)Device unit number.
 *      pkm      - (In)Packing mode (32B, 64B, 128)
 *      vrf_id   - (In)VRF ID
 *      idx      - (In)Memory Index
 *      key_len  - (In)Key prefix length
 * Returns:
 *      BCM_E_XXX
 */
int
tcam_wb_reinit(int u, int vrf_id, int pkm, int idx, int key_len, int mc)
{
    int zid, inst_id;
    int pfx_len, ipt;

    ipt = ALPM_PKM2IPT(pkm);
    zid = _tcam_zone_zid_get(u, ALPM_VRF_ID_TO_VRF(u, vrf_id), ipt, ALPM_128B(u), mc);
    inst_id = _tcam_zone_inst_id_get(u, ALPM_VRF_ID_TO_VRF(u, vrf_id), mc);

    tcam_pfx_len_get(u, pkm, zid, inst_id, key_len, &pfx_len);

    if (_TCAM_STATE_VENT(u, zid, pfx_len) == 0) {
        _TCAM_STATE_START(u, zid, pfx_len) = idx;
        _TCAM_STATE_END(u, zid, pfx_len) = idx;
    } else {
        _TCAM_STATE_END(u, zid, pfx_len) = idx;
    }

    _TCAM_STATE_VENT(u, zid, pfx_len)++;

    return (BCM_E_NONE);
}

#endif /* ALPM_ENABLE */

