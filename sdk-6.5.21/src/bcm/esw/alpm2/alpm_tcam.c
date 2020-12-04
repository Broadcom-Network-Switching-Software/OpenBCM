/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File:    alpm_tcam.c
 * Purpose: ALPM TCAM management. device independent implementation.
 * Requires:
 */

/* Implementation notes:
 */

#if defined(ALPM_ENABLE)

#include <bcm_int/esw/alpm_internal.h>

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
    _tcamZoneDBMixAllAclSrc = 10,   /* Combined-64 ACL SRC */
    _tcamZoneDBV4AllAclSrc = 11,    /* Combined-128 (V4) ACL SRC */
    _tcamZoneDBV6AllAclSrc = 12,    /* Combined-128 (V6) ACL SRC */

    _tcamZoneDBCnt = 13,

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
    "Mixed ACL SRC ",
    "V4 ACL SRC",
    "V6 ACL SRC",
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
    _tcam_zone_t *tcam_zn[_TCAM_ZID_CNT][ALPM_APPS];
    int          tcam_zn_cnt[ALPM_APPS];
} _tcam_ctrl_t;

typedef struct _tcam_zone_st_s {
    int cnt_used;
    int cnt_free;
} _tcam_zone_st_t;

_tcam_ctrl_t *_tcam_control[SOC_MAX_NUM_DEVICES];

#define TCAMC(u)                              (_tcam_control[u])
#define _TCAM_INIT_CHECK(u)                   (TCAMC(u) != NULL)
#define _TCAMZ_CNT(u)                         (TCAMC(u)->tcam_zn_cnt)
#define _TCAMZ(u, zid, app)                   (TCAMC(u)->tcam_zn[zid][app])
#define _TCAM_STATE(u, zid, app)              (_TCAMZ(u, zid, app)->state)
#define _TCAM_STATE_START(u, zid, app, pfx)   (_TCAMZ(u, zid, app)->state[pfx].start)
#define _TCAM_STATE_END(u, zid, app, pfx)     (_TCAMZ(u, zid, app)->state[pfx].end)
#define _TCAM_STATE_PREV(u, zid, app, pfx)    (_TCAMZ(u, zid, app)->state[pfx].prev)
#define _TCAM_STATE_NEXT(u, zid, app, pfx)    (_TCAMZ(u, zid, app)->state[pfx].next)
#define _TCAM_STATE_VENT(u, zid, app, pfx)    (_TCAMZ(u, zid, app)->state[pfx].vent)
#define _TCAM_STATE_FENT(u, zid, app, pfx)    (_TCAMZ(u, zid, app)->state[pfx].fent)

#define _TCAMZ_INITED(u, zid, app)            (_TCAMZ(u, zid, app) && _TCAMZ(u, zid, app)->tz_num_inst != 0)
#define _TCAMZ_DB_TYPE(u, zid, app)           (_TCAMZ(u, zid, app)->tz_db_type)
#define _TCAMZ_PFX_MIN_V6(u, zid, app)        (_TCAMZ(u, zid, app)->tz_min_v6)
#define _TCAMZ_PFX_ENT_PER_INST(u, zid, app)  (_TCAMZ(u, zid, app)->tz_ent_per_inst)
#define _TCAMZ_PFX_BASE_IDX(u, zid, app)      (_TCAMZ(u, zid, app)->tz_base_idx)
#define _TCAMZ_PFX_TBL_SZ(u, zid, app)        (_TCAMZ(u, zid, app)->tz_tbl_sz)
#define _TCAMZ_PFX_MAX_ENTS(u, zid, app)      (_TCAMZ(u, zid, app)->_tz_ent_cnt)
#define _TCAMZ_PFX_MAX_IDX(u, zid, app)       (_TCAMZ(u, zid, app)->_tz_max_idx)

#define _PFX_IS_PKM_64B(u, zid, app, pfx)     ((pfx) % _TCAMZ_PFX_ENT_PER_INST(u, zid, app) \
                                         >= _TCAMZ_PFX_MIN_V6(u, zid, app) && \
                                         (pfx) != _TCAMZ_PFX_MAX_IDX(u, zid, app))
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
    tZoneJC64AclSrc,       /* 10 */
    tZoneKC128V4AclSrc,    /* 11 */
    tZoneKC128V6AclSrc,    /* 12 */
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
    _tcamZoneDBMixAllAclSrc,  /* ZoneJ, Combined-64 */
    _tcamZoneDBV4AllAclSrc,   /* ZoneK, Combined-128 (V4) */
    _tcamZoneDBV6AllAclSrc,   /* ZoneL, Combined-128 (V6) */
};

/* Number of instance in the zone */
static uint16 tcam_zone_num_inst[tZone__Count] = {
    0,
    3,                  /* ZoneA */
    3,                  /* ZoneB */
    3,                  /* ZoneC */
    3,                  /* ZoneD */
    3,                  /* ZoneE */
    3,                  /* ZoneF */
    3,                  /* ZoneG */
    3,                  /* ZoneH */
    3,                  /* ZoneI */
    3,                  /* ZoneJ */
    3,                  /* ZoneK */
    3,                  /* ZoneL */
};

static uint16 tcam_zone_num_inst_ipmc_war[tZone__Count] = {
    0,
    6,                  /* ZoneA */
    6,                  /* ZoneB */
    6,                  /* ZoneC */
    6,                  /* ZoneD */
    6,                  /* ZoneE */
    6,                  /* ZoneF */
    6,                  /* ZoneG */
    6,                  /* ZoneH */
    6,                  /* ZoneI */
    6,                  /* ZoneJ */
    6,                  /* ZoneK */
    6,                  /* ZoneL */
};

/* 4 means 4 combinations for Pair_128_mode + alpm_mode
 * _TCAM_ZONE_CNT:
 *    bit1 : 0 Glb, 1 Prv
 *    bit0 : 0 V4,  1 V6
 */
static tz_comb_t   tz_conf[6][_TCAM_ZONE_CNT] = {
    /* Combined-64 */
    {tZoneAC64, tZoneInvalid, tZoneInvalid, tZoneInvalid},
    /* Combined-128 bit0 V4|V6 */
    {tZoneDC128V4, tZoneEC128V6, tZoneInvalid, tZoneInvalid},
    /* Parallel-64  bit1 Glb|Prv */
    {tZoneBP64Glb, tZoneInvalid, tZoneCP64Prv, tZoneInvalid},
    /* Parallel-128 bit1 Glb|Prv bit0 V4|V6 */
    {tZoneFP128V4Glb, tZoneGP128V6Glb, tZoneHP128V4Prv, tZoneIP128V6Prv},
    /* Combined-64 (ACL DST + SRC ) */
    {tZoneAC64, tZoneInvalid, tZoneJC64AclSrc, tZoneInvalid},
    /* Combined-128 bit0 V4|V6 (ACL DST + SRC ) */
    {tZoneDC128V4, tZoneEC128V6, tZoneKC128V4AclSrc, tZoneKC128V6AclSrc},

};

#define _TCAMZ_ZID_TO_ZN(zid)  (zid & 0x3)
#define _TCAMZ_ZN_MAKE(pri, pkm)  (!!(pri) << 1 | !!(pkm))
#define _TCAMZ_ZN_MAKE_FOR_ACL(src, lpm128)  (!!(src) << 1 | !!(lpm128))
#define _TCAMZ_ZID_MAKE_FROM_ZN(mc, p, z, zn)   \
    (!!(mc) << 4 | !!(p) << 3 | !!(z) << 2 | (zn & 0x3))
#define _TCAMZ_ZID_MAKE(mc, p, z, vrf, pkm)     \
    (!!(mc) << 4 | !!(p) << 3 | !!(z) << 2 | !!(vrf) << 1 | !!(pkm))
#define _TCAMZ_ZID_MAKE_FOR_ACL(mc, p, z, src, lpm128)     \
    _TCAMZ_ZID_MAKE_FROM_ZN(mc, p, z, _TCAMZ_ZN_MAKE_FOR_ACL(src, lpm128))

#define _TCAMZ_ZID_IS_V6_ONLY(zid)  ((zid) & 0x1)

static _tcam_zone_t tz_ctrl[SOC_MAX_NUM_DEVICES][_TCAM_ZONE_CNT][ALPM_APPS];

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
                       uint16 base_idx, uint16 tbl_sz, int app)
{
    uint16 ent_per_inst = 0, v6_min = 0;
    _tcam_zone_t *tz;

    tz = &tz_ctrl[u][zn][app];

    switch (db_type) {
        case _tcamZoneDBV4All:
        case _tcamZoneDBV4Prv:
        case _tcamZoneDBV4Glb:
        case _tcamZoneDBV4AllAclSrc:
            ent_per_inst = 32 + 2 + 1;
            v6_min = ent_per_inst + 1;
            break;
        case _tcamZoneDBV6All:
        case _tcamZoneDBV6Prv:
        case _tcamZoneDBV6Glb:
        case _tcamZoneDBV6AllAclSrc:
            ent_per_inst = 128 + 2 + 1;
            v6_min = 0;
            break;
        case _tcamZoneDBMixAll:
        case _tcamZoneDBMixPrv:
        case _tcamZoneDBMixGlb:
        case _tcamZoneDBMixAllAclSrc:
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
        case _tcamZoneDBV6AllAclSrc:
            pkm = ALPM_PKM_128;
            break;
        default:
            break;
    }

    return pkm;
}

int
_tcam_zone_init(int u, int app)
{
    int rv = BCM_E_NONE, zn, zid, j, comb;
    int max_pfx_len, pfx_st_sz, zone_cnt = 0;

    int          pkm;
    _tz_db_t db_type;
    uint16  num_inst;
    uint16  base_idx;
    int       tbl_sz;

    comb = !!ALPM_TCAM_ZONED(u, app) << 1 | !!ALPM_128B(u);
    if (alpm_acl_app(app)) {
        comb = 4 + !!ALPM_128B(u); /* ACL base */
    }

    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        tz_comb_t tzc = tz_conf[comb][zn];
        if (tzc == tZoneInvalid) {
            continue;
        }

        db_type = tcam_zone_type[tzc];
        pkm = _tcam_zone_db_type_to_pkm(u, db_type);
        tbl_sz = tcam_table_size(u, app, pkm) >> !!ALPM_TCAM_ZONED(u, app);
        if (db_type == _tcamZoneDBV6Glb ||
            db_type == _tcamZoneDBV4Glb ||
            db_type == _tcamZoneDBMixGlb) {
            base_idx = tbl_sz;
        } else {
            base_idx = 0;
        }
        if (ALPM_ACL_EN(u) && alpm_acl_app(app)) {
            base_idx += tcam_table_size(u, APP0, pkm);
            if (db_type == _tcamZoneDBMixAllAclSrc ||
                db_type == _tcamZoneDBV4AllAclSrc ||
                db_type == _tcamZoneDBV6AllAclSrc) {
                base_idx += tcam_table_size(u, app, pkm);
            }
        }

        num_inst = (soc_feature(u, soc_feature_td3_lpm_ipmc_war)) ?
                    tcam_zone_num_inst_ipmc_war[tzc] :
                    tcam_zone_num_inst[tzc];
        _tcam_zone_ctrl_config(u, zn, db_type, num_inst,
                               base_idx, (uint16)tbl_sz, app);
        zone_cnt ++;
    }

    TCAMC(u)->tcam_zn_cnt[app] = zone_cnt;

    /* Mapping to tz_ctrl(uc) */
    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int zn2 = zn;
        tz_comb_t tzc;

        tzc = tz_conf[comb][zn];
        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u, app), zn);
        if (tzc == tZoneInvalid) {
            /* IP type don't care */
            if (!ALPM_128B(u) && (zn & 0x1)) {
                zn2 = zn2 & 0x2;
            }
            /* VRF type don't care */
            if (!ALPM_TCAM_ZONED(u, app) && (zn & 0x2)) {
                zn2 = zn2 & 0x1;
            }
        }
        _TCAMZ(u, zid, app) = &tz_ctrl[u][zn2][app];
        ALPM_VERB(("uc zid %d -> zn %d\n", zid, zn2));
    }

    if (soc_feature(u, soc_feature_td3_lpm_ipmc_war)) {
        for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
            int zn2 = zn;

            zid = _TCAMZ_ZID_MAKE_FROM_ZN(1, ALPM_128B(u), ALPM_TCAM_ZONED(u, app), zn);

            /* IP type don't care */
            if (!ALPM_128B(u) && (zn & 0x1)) {
                zn2 = zn2 & 0x2;
            }

            if (ALPM_TCAM_ZONED(u, app)) {
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
            _TCAMZ(u, zid, app) = &tz_ctrl[u][zn2][app];
            ALPM_VERB(("mc zid %d -> zn %d\n", zid, zn2));
        }
    }

    /* TCAM State init */
    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int max_idx, zid;

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u, app), zn);
        if (_TCAMZ(u, zid, app) == NULL) {
            continue;
        }

        max_idx = _TCAMZ_PFX_MAX_IDX(u, zid, app);
        max_pfx_len = _TCAMZ_PFX_MAX_ENTS(u, zid, app);
        pfx_st_sz = sizeof(_tcam_zone_state_t) * max_pfx_len;
        ALPM_REALLOC_EG(_TCAM_STATE(u, zid, app), pfx_st_sz, "LPM_pfx_info");
        for (j = 0; j < max_pfx_len; j++) {
            _TCAM_STATE_START(u, zid, app, j)  = -1;
            _TCAM_STATE_END(u, zid, app, j)    = -1;
            _TCAM_STATE_PREV(u, zid, app, j)   = -1;
            _TCAM_STATE_NEXT(u, zid, app, j)   = -1;
            _TCAM_STATE_VENT(u, zid, app, j)   = 0;
            _TCAM_STATE_FENT(u, zid, app, j)   = 0;
        }

        _TCAM_STATE_FENT(u, zid, app, max_idx) = _TCAMZ_PFX_TBL_SZ(u, zid, app);
        _TCAM_STATE_END(u, zid, app, max_idx) = _TCAMZ_PFX_BASE_IDX(u, zid, app) - 1;
    }

bad:
    return rv;
}

static int
_tcam_zone_zid_get(int u, int app, int vrf, int ipt, int in_pair_mode, int mc)
{
    int zid, src;
    mc = soc_feature(u, soc_feature_td3_lpm_ipmc_war) && mc;
    if (app == APP0) {
        zid = _TCAMZ_ZID_MAKE(mc, in_pair_mode, ALPM_TCAM_ZONED(u, app),
                              !ALPM_VRF_IS_GBL(u, vrf), ipt);
    } else {
        src = (app == ALPM_APP1) ? 0 : 1;
        zid = _TCAMZ_ZID_MAKE_FOR_ACL(mc, in_pair_mode, ALPM_TCAM_ZONED(u, app),
                                      src, ipt);
    }
    return zid;
}

int
_tcam_zone_inst_id_get(int u, int vrf, int mc)
{
    int inst_id;

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

    return inst_id;
}

static void
_tcam_zone_status(int u, int zid, int app, _tcam_zone_st_t *tz_st)
{
    int i, max_pfx_len;

    sal_memset(tz_st, 0, sizeof(*tz_st));
    max_pfx_len = _TCAMZ_PFX_MAX_IDX(u, zid, app);
    for (i = max_pfx_len; i >= 0 ; i--) {
        if (i != _TCAMZ_PFX_MAX_IDX(u, zid, app) && (_TCAM_STATE_START(u, zid, app, i) == -1)) {
            continue;
        }

        tz_st->cnt_free += _TCAM_STATE_FENT(u, zid, app, i);
        tz_st->cnt_used += _TCAM_STATE_VENT(u, zid, app, i);
    }

    return ;
}

int
tcam_resource_info_get(int u, int app, int vrf, int ipt, int mc,
                       bcm_l3_alpm_lvl_usage_t *lvl_info)
{
    int zid;
    _tcam_zone_st_t tz_st;

    if (!TCAMC(u)) {
        return BCM_E_INIT;
    }

    zid = _tcam_zone_zid_get(u, app, vrf, ipt, ALPM_128B(u), mc);
    _tcam_zone_status(u, zid, app, &tz_st);

    lvl_info->cnt_used = tz_st.cnt_used;
    lvl_info->cnt_total = tz_st.cnt_free + tz_st.cnt_used;

    return BCM_E_NONE;
}

static int
_tcam_trie_lookup(int u, _bcm_defip_cfg_t *lpm_cfg, int *key_index)
{
    int              rv = BCM_E_NOT_FOUND;
    alpm_lib_trie_t  *pvt_trie;
    _alpm_pvt_node_t *pvt_node;

    int             ipt = ALPM_LPM_IPT(u, lpm_cfg);
    int             vrf_id = ALPM_LPM_VRF_ID(u, lpm_cfg);
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u, app), vrf_id, ipt);
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

int
_tcam_pkm_uses_half_wide(int u, int pkm, int mc)
{
    int use_half_wide = 0;

    if (pkm == ALPM_PKM_32B) {
        if (mc && soc_feature(u, soc_feature_separate_key_for_ipmc_route)) {
            /* V4 Multicast */
            use_half_wide = 0;
        } else {
            use_half_wide = 1;
        }
    }

    return use_half_wide;
}

/* VALID bit is not checked here */
int
tcam_entry_pvt_node_get(int u, int app, int pkm, void *e, int sub_idx, _alpm_pvt_node_t **pvt_node)
{
    int rv = BCM_E_NOT_FOUND;
    uint32 pfx[5];
    int pfx_len, vrf_id;
    alpm_lib_trie_t     *pvt_trie;

    if (sub_idx < 0 || sub_idx > 1) {
        sub_idx = 0;
    }

    ALPM_IER(tcam_entry_vrf_id_get(u, app, pkm, e, sub_idx, &vrf_id));
    ALPM_IER(tcam_entry_pfx_len_get(u, app, pkm, e, sub_idx, &pfx_len));
    ALPM_IER(alpm_trie_ent_to_pfx(u, app, pkm, e, sub_idx, pfx_len, pfx));

    pvt_trie = ACB_PVT_TRIE(ACB_TOP(u, app), vrf_id, ALPM_PKM2IPT(pkm));
    if (pvt_trie != NULL) {
        rv = alpm_lib_trie_search(pvt_trie, pfx, pfx_len, (alpm_lib_trie_node_t **)pvt_node);
    }
    return rv;
}

int
tcam_entry_valid(int u, int app, int pkm, void *e, int sub_idx)
{
    int vld = 0;

    vld = ALPM_DRV(u)->tcam_entry_valid(u, app, pkm, e, sub_idx);

    return vld;
}

static int
_tcam_entry_valid_set(int u, int app, int pkm, void *e, int sub_idx, int val)
{
    int rv = BCM_E_NONE;

    rv = ALPM_DRV(u)->tcam_entry_valid_set(u, app, pkm, e, sub_idx, val);

    return rv;
}

static int
_tcam_trie_update_by_ent(int u, int app, int pkm, void *e, int hw_idx)
{
    int         i, rv = BCM_E_NONE;
    _alpm_pvt_node_t *pvt_node;
    int         ip4mc = 0;

    if (soc_feature(u, soc_feature_separate_key_for_ipmc_route)) {
        rv = tcam_entry_key_type_get(u, app, pkm, e, 0, &ip4mc);
        if (BCM_SUCCESS(rv)) {
            ip4mc = (ip4mc == ALPM_SEPARATE_IPV4MC_KEY_TYPE) ? 1 : 0;
        }
    }

    if (_tcam_pkm_uses_half_wide(u, pkm, ip4mc)) {
        /* For IPv4(32B packing mode), check and update both 2 entries */
        for (i = 0; i < 2; i++) {
            if (tcam_entry_valid(u, app, pkm, e, i)) {
                rv = tcam_entry_pvt_node_get(u, app, pkm, e, i, &pvt_node);
                if (BCM_SUCCESS(rv)) {
                    PVT_IDX(pvt_node) = hw_idx << 1 | i;
                }
            }
        }
    } else {
        if (tcam_entry_valid(u, app, pkm, e, 0)) {
            rv = tcam_entry_pvt_node_get(u, app, pkm, e, 0, &pvt_node);
            if (BCM_SUCCESS(rv)) {
                PVT_IDX(pvt_node) = hw_idx;
            }
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
tcam_entry_vrf_id_get(int u, int app, int pkm, void *e, int sub_idx, int *vrf_id)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_vrf_id_get(u, app, pkm, e, sub_idx, vrf_id);

    return rv;
}

int
tcam_entry_key_type_get(int u, int app, int pkm, void *e, int sub_idx, int *key_type)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_key_type_get(u, app, pkm, e, sub_idx, key_type);

    return rv;
}

int
tcam_entry_pfx_len_get(int u, int app, int pkm, void *e, int sub_idx, int *pfx_len)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_pfx_len_get(u, app, pkm, e, sub_idx, pfx_len);

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
_tcam_entry_x_to_y(int u, int app, int pkm, void *src, void *dst,
                   int copy_hit, int x, int y)
{
    int         rv = BCM_E_UNAVAIL;
    rv = ALPM_DRV(u)->tcam_entry_x_to_y(u, app, pkm, src, dst, copy_hit, x, y);

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
tcam_entry_read(int u, int app, int pkm, void *e, int index, int s_index)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u, app);
    rv = ALPM_DRV(u)->mem_entry_read(u, acb, acb->pvt_tbl[pkm], index, e, FALSE);

    return rv;
}

int
tcam_entry_read_no_cache(int u, int app, int pkm, void *e, int index, int s_index)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u, app);
    rv = ALPM_DRV(u)->mem_entry_read(u, acb, acb->pvt_tbl[pkm], index, e, TRUE);

    return rv;
}

int
_tcam_entry_write(int u, int app, int pkm, _alpm_pvt_node_t *pvt_node, void *e,
                  int index, int s_index, int update_data)
{
    int         rv = BCM_E_UNAVAIL;
    _alpm_cb_t *acb;

    acb = ACB_TOP(u, app);
    rv = ALPM_DRV(u)->mem_entry_write(u, acb, pvt_node, acb->pvt_tbl[pkm], index, e, update_data);

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
tcam_entry_to_key(int u, int app, int pkm, void *e, int sub_idx, uint32 *key)
{
    int         rv = BCM_E_UNAVAIL;

    rv = ALPM_DRV(u)->tcam_entry_to_key(u, app, pkm, e, sub_idx, key);

    return rv;
}

/*
 *      Create a slot for the new entry rippling the entries if required
 */
static int
_tcam_entry_shift(int u, int app, int pkm, int from_ent, int to_ent)
{
    int         rv;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];

    ALPM_IER(tcam_entry_read(u, app, pkm, e, from_ent, from_ent));
    rv = _tcam_trie_update_by_ent(u, app, pkm, e, to_ent);
    if (BCM_SUCCESS(rv)) {
        rv = _tcam_entry_write(u, app, pkm, NULL, e, to_ent, from_ent, FALSE);
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
_tcam_shift_pfx_up(int u, int pfx, int zid, int app, int ipmc)
{
    int         pkm;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         from_ent;
    int         to_ent;
    uint32      v0, v1;
    int         rv = BCM_E_NONE;

    to_ent = _TCAM_STATE_END(u, zid, app, pfx) + 1;

    pkm = _tcam_zone_db_type_to_pkm(u, _TCAMZ_DB_TYPE(u, zid, app));
    if (pkm != ALPM_PKM_128) {
        pkm = _PFX_IS_PKM_64B(u, zid, app, pfx);
    }

    if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
        from_ent = _TCAM_STATE_END(u, zid, app, pfx);
        ALPM_IER(tcam_entry_read(u, app, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, app, pkm, e, 0);
        v1 = tcam_entry_valid(u, app, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            _tcam_trie_update_by_ent(u, app, pkm, e, to_ent);
            ALPM_IER(_tcam_entry_write(u, app, pkm, NULL, e, to_ent, from_ent, FALSE));
            to_ent--;
        }
    }

    from_ent = _TCAM_STATE_START(u, zid, app, pfx);
    if (from_ent != to_ent) {
        ALPM_IER(_tcam_entry_shift(u, app, pkm, from_ent, to_ent));
    }
    _TCAM_STATE_START(u, zid, app, pfx) += 1;
    _TCAM_STATE_END(u, zid, app, pfx) += 1;

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
_tcam_shift_pfx_down(int u, int pfx, int zid, int app, int ipmc)
{
    int         pkm;
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         from_ent;
    int         to_ent;
    int         prev_ent;
    uint32      v0, v1;
    int         rv = BCM_E_NONE;

    to_ent = _TCAM_STATE_START(u, zid, app, pfx) - 1;

    /* Don't move empty prefix . */
    if (_TCAM_STATE_VENT(u, zid, app, pfx) == 0) {
        _TCAM_STATE_START(u, zid, app, pfx) = to_ent;
        _TCAM_STATE_END(u, zid, app, pfx) = to_ent - 1;
        return BCM_E_NONE;
    }

    pkm = _tcam_zone_db_type_to_pkm(u, _TCAMZ_DB_TYPE(u, zid, app));
    if (pkm != ALPM_PKM_128) {
        pkm = _PFX_IS_PKM_64B(u, zid, app, pfx);
    }

    if (_tcam_pkm_uses_half_wide(u, pkm, ipmc) &&
        _TCAM_STATE_END(u, zid, app, pfx) != _TCAM_STATE_START(u, zid, app, pfx)) {

        from_ent = _TCAM_STATE_END(u, zid, app, pfx);

        ALPM_IER(tcam_entry_read(u, app, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, app, pkm, e, 0);
        v1 = tcam_entry_valid(u, app, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            /* Last entry is half full -> keep it last. */
            /* Shift entry before last to start - 1 position. */
            prev_ent = from_ent - 1;
            ALPM_IER(_tcam_entry_shift(u, app, pkm, prev_ent, to_ent));


            _tcam_trie_update_by_ent(u, app, pkm, e, prev_ent);
            ALPM_IER(_tcam_entry_write(u, app, pkm, NULL, e, prev_ent, from_ent, FALSE));
        } else {
            /* Last entry is full -> just shift it to start - 1  position. */
            _tcam_trie_update_by_ent(u, app, pkm, e, to_ent);
            ALPM_IER(_tcam_entry_write(u, app, pkm, NULL, e, to_ent, from_ent, FALSE));

        }

    } else  {
        from_ent = _TCAM_STATE_END(u, zid, app, pfx);
        ALPM_IER(_tcam_entry_shift(u, app, pkm, from_ent, to_ent));
    }
    _TCAM_STATE_START(u, zid, app, pfx) -= 1;
    _TCAM_STATE_END(u, zid, app, pfx) -= 1;

    return rv;
}

/*
 *      Create a slot for the new entry rippling the entries if required
 *      returned update=1 if half-free entry, update=0 if full-free entry.
 */
static int
_tcam_free_slot_create(int u, int pkm, int zid, int app, int pfx, int ipmc, int *free_slot, int *update)
{
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    int         prev_pfx;
    int         next_pfx;
    int         free_pfx;
    int         curr_pfx;
    int         from_ent;
    uint32      v0, v1;

    if (_TCAM_STATE_VENT(u, zid, app, pfx) == 0) {
        /*
         * Find the  prefix position. Only prefix with valid
         * entries are in the list.
         * next -> high to low prefix. low to high index
         * prev -> low to high prefix. high to low index
         * Unused prefix length _PFX_MAX_INDEX is the head of the
         * list and is node corresponding to this is always
         * present.
         */
        curr_pfx = _TCAMZ_PFX_MAX_IDX(u, zid, app);
        while (_TCAM_STATE_NEXT(u, zid, app, curr_pfx) > pfx) {
            curr_pfx = _TCAM_STATE_NEXT(u, zid, app, curr_pfx);
        }
        /* Insert the new prefix */
        next_pfx = _TCAM_STATE_NEXT(u, zid, app, curr_pfx);
        if (next_pfx != -1) {
            _TCAM_STATE_PREV(u, zid, app, next_pfx) = pfx;
        }
        _TCAM_STATE_NEXT(u, zid, app, pfx) = _TCAM_STATE_NEXT(u, zid, app, curr_pfx);
        _TCAM_STATE_PREV(u, zid, app, pfx) = curr_pfx;
        _TCAM_STATE_NEXT(u, zid, app, curr_pfx) = pfx;

        _TCAM_STATE_FENT(u, zid, app, pfx) =
            (_TCAM_STATE_FENT(u, zid, app, curr_pfx) + 1) / 2;
        _TCAM_STATE_FENT(u, zid, app, curr_pfx) -= _TCAM_STATE_FENT(u, zid, app, pfx);
        _TCAM_STATE_START(u, zid, app, pfx) = _TCAM_STATE_END(u, zid, app, curr_pfx) +
                                    _TCAM_STATE_FENT(u, zid, app, curr_pfx) + 1;
        _TCAM_STATE_END(u, zid, app, pfx) = _TCAM_STATE_START(u, zid, app, pfx) - 1;
        _TCAM_STATE_VENT(u, zid, app, pfx) = 0;
    } else if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
        /* For IPv4 Check if alternate entry is free */
        from_ent = _TCAM_STATE_START(u, zid, app, pfx);
        ALPM_IER(tcam_entry_read(u, app, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, app, pkm, e, 0);
        v1 = tcam_entry_valid(u, app, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
            *update = 1; /* half-free entry */
            return BCM_E_NONE;
        }

        from_ent = _TCAM_STATE_END(u, zid, app, pfx);
        ALPM_IER(tcam_entry_read(u, app, pkm, e, from_ent, from_ent));
        v0 = tcam_entry_valid(u, app, pkm, e, 0);
        v1 = tcam_entry_valid(u, app, pkm, e, 1);

        if ((v0 == 0) || (v1 == 0)) {
            *free_slot = (from_ent << 1) + ((v1 == 0) ? 1 : 0);
            *update = 1; /* half-free entry */
            return BCM_E_NONE;
        }
    }

    free_pfx = pfx;
    while (_TCAM_STATE_FENT(u, zid, app, free_pfx) == 0) {
        /* pkm <= ALPM_PKM_64B: starting from higher index.
         * else starting from lower index
         */
        free_pfx = _TCAM_STATE_NEXT(u, zid, app, free_pfx);
        if (free_pfx == -1) {
            /* No free entries on this side try the other side */
            free_pfx = pfx;
            break;
        }
    }

    while (_TCAM_STATE_FENT(u, zid, app, free_pfx) == 0) {
        free_pfx = _TCAM_STATE_PREV(u, zid, app, free_pfx);
        if (free_pfx == -1) {
            if (_TCAM_STATE_VENT(u, zid, app, pfx) == 0) {
                /* We failed to allocate entries for a newly allocated prefix.*/
                prev_pfx = _TCAM_STATE_PREV(u, zid, app, pfx);
                next_pfx = _TCAM_STATE_NEXT(u, zid, app, pfx);
                if (-1 != prev_pfx) {
                    _TCAM_STATE_NEXT(u, zid, app, prev_pfx) = next_pfx;
                }
                if (-1 != next_pfx) {
                    _TCAM_STATE_PREV(u, zid, app, next_pfx) = prev_pfx;
                }
            }
            return BCM_E_FULL;
        }
    }

    /*
     * Ripple entries to create free space
     */
    while (free_pfx > pfx) {
        next_pfx = _TCAM_STATE_NEXT(u, zid, app, free_pfx);
        ALPM_IER(_tcam_shift_pfx_down(u, next_pfx, zid, app, ipmc));
        _TCAM_STATE_FENT(u, zid, app, free_pfx) -= 1;
        _TCAM_STATE_FENT(u, zid, app, next_pfx) += 1;
        free_pfx = next_pfx;
    }

    while (free_pfx < pfx) {
        ALPM_IER(_tcam_shift_pfx_up(u, free_pfx, zid, app, ipmc));
        _TCAM_STATE_FENT(u, zid, app, free_pfx) -= 1;
        prev_pfx = _TCAM_STATE_PREV(u, zid, app, free_pfx);
        _TCAM_STATE_FENT(u, zid, app, prev_pfx) += 1;
        free_pfx = prev_pfx;
    }

    _TCAM_STATE_VENT(u, zid, app, pfx) += 1;
    _TCAM_STATE_FENT(u, zid, app, pfx) -= 1;
    _TCAM_STATE_END(u, zid, app, pfx) += 1;

    *free_slot = _TCAM_STATE_END(u, zid, app, pfx);
    if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
        *free_slot <<= 1;
    }
    *update = 0; /* full-free entry */

    return BCM_E_NONE;
}

/*
 * Delete a slot and adjust entry pointers if required.
 */
static int
_tcam_free_slot_delete(int u, int pkm, int zid, int app, int pfx, int slot, int ipmc)
{
    int         prev_pfx, next_pfx;
    int         fidx, tidx;
    uint32      ef[SOC_MAX_MEM_FIELD_WORDS];
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    void        *et;
    int         rv = BCM_E_NONE;

    fidx    = _TCAM_STATE_END(u, zid, app, pfx);
    tidx    = slot;

    if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
        tidx >>= 1;
        ALPM_IER(tcam_entry_read(u, app, pkm, ef, fidx, fidx));
        if (fidx != tidx) {
            ALPM_IER(tcam_entry_read(u, app, pkm, e, tidx, tidx));
            et = e;
        } else {
            et = ef;
        }

        if (tcam_entry_valid(u, app, pkm, ef, 1)) {
            rv = _tcam_entry_x_to_y(u, app, pkm, ef, et, TRUE, 1, slot & 1);
            (void)_tcam_entry_valid_set(u, app, pkm, ef, 1, 0);
        } else {
            rv = _tcam_entry_x_to_y(u, app, pkm, ef, et, TRUE, 0, slot & 1);
            (void)_tcam_entry_valid_set(u, app, pkm, ef, 0, 0);
            _TCAM_STATE_VENT(u, zid, app, pfx) -= 1;
            _TCAM_STATE_FENT(u, zid, app, pfx) += 1;
            _TCAM_STATE_END(u, zid, app, pfx) -= 1;
        }

        /* not true for OVERRIDE routes */
        if (tidx != fidx) {
            ALPM_IER(_tcam_trie_update_by_ent(u, app, pkm, et, tidx));
            ALPM_IER(_tcam_entry_write(u, app, pkm, NULL, et, tidx, tidx, FALSE));
        }
        ALPM_IER(_tcam_trie_update_by_ent(u, app, pkm, ef, fidx));
        ALPM_IER(_tcam_entry_write(u, app, pkm, NULL, ef, fidx, fidx, FALSE));
    } else { /* IPV6 */
        _TCAM_STATE_VENT(u, zid, app, pfx) -= 1;
        _TCAM_STATE_FENT(u, zid, app, pfx) += 1;
        _TCAM_STATE_END(u, zid, app, pfx)  -= 1;
        if (tidx != fidx) {
            ALPM_IER(tcam_entry_read(u, app, pkm, ef, fidx, fidx));
            ALPM_IER(_tcam_trie_update_by_ent(u, app, pkm, ef, tidx));
            ALPM_IER(_tcam_entry_write(u, app, pkm, NULL,ef, tidx, fidx, FALSE));
        }
        /* v6-64 indices are still raw in pivot tcam */
        sal_memset(ef, 0, sizeof(ef));
        ALPM_IER(_tcam_entry_write(u, app, pkm, NULL, ef, fidx, fidx, FALSE));

    }

    if (_TCAM_STATE_VENT(u, zid, app, pfx) == 0) {
        /* remove from the list */
        prev_pfx = _TCAM_STATE_PREV(u, zid, app, pfx); /* Always present */
        assert(prev_pfx != -1);
        next_pfx = _TCAM_STATE_NEXT(u, zid, app, pfx);
        _TCAM_STATE_NEXT(u, zid, app, prev_pfx) = next_pfx;
        _TCAM_STATE_FENT(u, zid, app, prev_pfx) += _TCAM_STATE_FENT(u, zid, app, pfx);
        _TCAM_STATE_FENT(u, zid, app, pfx) = 0;
        if (next_pfx != -1) {
            _TCAM_STATE_PREV(u, zid, app, next_pfx) = prev_pfx;
        }
        _TCAM_STATE_NEXT(u, zid, app, pfx) = -1;
        _TCAM_STATE_PREV(u, zid, app, pfx) = -1;
        _TCAM_STATE_START(u, zid, app, pfx) = -1;
        _TCAM_STATE_END(u, zid, app, pfx) = -1;
    }

    return rv;
}

/*
 * Extract vrf weighted prefix lenght from vrf_id, pkm, key_len.
 */
static void
tcam_pfx_len_get(int u, int ipv6, int zid, int app, int inst_id, int key_len, int *pfx_len)
{
    int ipv6_min;

    ipv6_min = _TCAMZ_PFX_MIN_V6(u, zid, app);

    if (ipv6) {
        key_len += ipv6_min;
    }

    /* In ALPM the arragnement of VRF is at he begining followed by VRF
       override and global */
    *pfx_len = key_len + inst_id * _TCAMZ_PFX_ENT_PER_INST(u, zid, app);
    return;
}

static void
_tcam_gpfx_len_get(
    int u, int defip_vrf, int ipv6, int zid,
    _bcm_defip_cfg_t *lpm_cfg, int *pfx_len)
{
    int inst_id;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    /* Calculate vrf weighted prefix lengh. */
    inst_id = _tcam_zone_inst_id_get(u, defip_vrf, lpm_cfg->defip_flags & BCM_L3_IPMC);
    tcam_pfx_len_get(u, ipv6, zid, app, inst_id, lpm_cfg->defip_sub_len, pfx_len);

    return;
}

static int
_tcam_cfg_write(int u, _bcm_defip_cfg_t *lpm_cfg, int index, int s_index,
                int update, uint32 write_flags, int update_data)
{
    int         rv = BCM_E_NONE;
    int         hw_idx;
    int         pkm = ALPM_LPM_PKM(u, lpm_cfg);
    uint32      e[SOC_MAX_MEM_FIELD_WORDS];
    _alpm_tcam_write_t *tcam_write = lpm_cfg->tcam_write;
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);
    int ipmc = (lpm_cfg->defip_flags & BCM_L3_IPMC);

    hw_idx = _tcam_pkm_uses_half_wide(u, pkm, ipmc) ? index >> 1 : index;

    /* Entry already present. Update the entry */
    if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
        if (update) {
            ALPM_IER(tcam_entry_read(u, app, pkm, e, hw_idx, s_index));
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
            _alpm_pvt_node_t *pvt_node = NULL;
            if (lpm_cfg && ACB_BKT_FIXED_FMT(ACB_TOP(u, app), 1)) {
                pvt_node = lpm_cfg->pvt_node;
                if (pvt_node) {
                    PVT_IDX(pvt_node) = index;
                }
            }
            rv = _tcam_entry_write(u, app, pkm, pvt_node, e, hw_idx, s_index, update_data);
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
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);
    int ipmc = (lpm_cfg->defip_flags & BCM_L3_IPMC);

    pkm = ALPM_LPM_PKM(u, lpm_cfg);
    _tcam_gpfx_len_get(u, lpm_cfg->defip_vrf, !!pkm, zid, lpm_cfg, &pfx);
    rv = _tcam_free_slot_create(u, pkm, zid, app, pfx, ipmc, &index, &update);
    if (BCM_SUCCESS(rv)) {
        lpm_cfg->defip_index = index;
        rv = _tcam_cfg_write(u, lpm_cfg, index, index, update, write_flags,
                             FALSE);
    }

    return rv;
}

int
bcm_esw_alpm_tcam_avail(int u, int app, int vrf_id, int ipt, int key_len, int mc)
{
    int pfx, zid, fent, cnt = 0;

    if (!TCAMC(u)) {
        return 0;
    }

    zid = _tcam_zone_zid_get(u, app, ALPM_VRF_ID_TO_VRF(u, vrf_id), ipt, ALPM_128B(u), mc);
    pfx = _TCAMZ_PFX_MAX_IDX(u, zid, app);
    while (pfx >= 0) {
        fent = _TCAM_STATE_FENT(u, zid, app, pfx);
        if (fent > 0) {
            cnt += fent;
        }
        pfx = _TCAM_STATE_NEXT(u, zid, app, pfx);
    }

    return cnt;
}

int
bcm_esw_alpm_tcam_state_free_get(int u, int app, int zn, int *free_cnt, int *used_cnt)
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

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u, app), i);
        if (!_TCAMZ_INITED(u, zid, app)) {
            continue;
        }
        _tcam_zone_status(u, zid, app, &tz_st);

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
bcm_esw_alpm_tcam_zone_state_dump(int u, int app)
{
    int i, zn, comb;
    int max_pfx_len;

    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int zid;

        comb = !!ALPM_TCAM_ZONED(u, app) << 1 | !!ALPM_128B(u);
        if (alpm_acl_app(app)) {
            comb = 4 + !!ALPM_128B(u); /* ACL base */
        }

        if (tz_conf[comb][zn] == tZoneInvalid) {
            continue;
        }

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u, app), zn);
        if (!_TCAMZ_INITED(u, zid, app)) {
            continue;
        }

        cli_out("Zone %d (%s) :\n", zn, _tz_db_str[_TCAMZ_DB_TYPE(u, zid, app)]);
        max_pfx_len = _TCAMZ_PFX_MAX_IDX(u, zid, app);
        for (i = max_pfx_len; i >= 0 ; i--) {
            if ((i != _TCAMZ_PFX_MAX_IDX(u, zid, app)) &&
                _TCAM_STATE_START(u, zid, app, i) == -1 &&
                _TCAM_STATE_FENT(u, zid, app, i) == 0) {
                continue;
            }

            cli_out("\tPFX = %3d P = %3d N = %3d START = %5d END = %5d "
                    "VENT = %5d FENT = %5d\n", i,
                    _TCAM_STATE_PREV(u, zid, app, i),
                    _TCAM_STATE_NEXT(u, zid, app, i),
                    _TCAM_STATE_START(u, zid, app, i),
                    _TCAM_STATE_END(u, zid, app, i),
                    _TCAM_STATE_VENT(u, zid, app, i),
                    _TCAM_STATE_FENT(u, zid, app, i));
        }
        cli_out("\n");
    }
}

void
bcm_esw_alpm_tcam_zone_dump(int u, int app, char *pfx_str)
{
    int zn, comb;
    int free_tcam = 0, used_tcam = 0, tot_tcam;

    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int zid;

        comb = !!ALPM_TCAM_ZONED(u, app) << 1 | !!ALPM_128B(u);
        if (alpm_acl_app(app)) {
            comb = 4 + !!ALPM_128B(u); /* ACL base */
        }

        if (tz_conf[comb][zn] == tZoneInvalid) {
            continue;
        }

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u, app), zn);
        if (!_TCAMZ_INITED(u, zid, app)) {
            continue;
        }

        (void)bcm_esw_alpm_tcam_state_free_get(u, app, zn, &free_tcam, &used_tcam);
        tot_tcam = used_tcam + free_tcam;
        if (tot_tcam == 0) {
            tot_tcam = 1; /* avoid divide by 0 */
        }
        cli_out("%sTCAM zone %d(%s): %4d (used), %4d (free), Usage: %d.%d%%\n",
                pfx_str, zn, _tz_db_str[_TCAMZ_DB_TYPE(u, zid, app)],
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
    int app, app_cnt = alpm_app_cnt(u);

    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return BCM_E_UNAVAIL;
    }

    for (app = 0; app < app_cnt; app++) {
        for (zid = 0; zid < _TCAM_ZID_CNT; zid++) {
            if (_TCAMZ_INITED(u, zid, app) &&
                _TCAM_STATE(u, zid, app) != NULL) {
                alpm_util_free(_TCAM_STATE(u, zid, app));
                _TCAM_STATE(u, zid, app) = NULL;
            }
        }

        for (i = 0; i < _TCAM_ZID_CNT; i++) {
            _TCAMZ(u, i, app) = NULL;
        }
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
    int app, app_cnt = alpm_app_cnt(u);

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
    for (app = 0; app < app_cnt; app++) {
        ALPM_IEG(_tcam_zone_init(u, app));
    }

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
    int app, app_cnt = alpm_app_cnt(u);

    if (!soc_feature(u, soc_feature_lpm_tcam)) {
        return BCM_E_UNAVAIL;
    }

    ALPM_REALLOC_EG(TCAMC(u), sizeof(_tcam_ctrl_t), "TCAMC");
    for (app = 0; app < app_cnt; app++) {
        ALPM_IEG(_tcam_zone_init(u, app));
    }

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
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    rv = _tcam_cfg_match(u, lpm_cfg, &index);
    if (rv == BCM_E_NOT_FOUND) {
        int zid;
        zid = _tcam_zone_zid_get(u, app, lpm_cfg->defip_vrf,
                                 ALPM_LPM_IPT(u, lpm_cfg), ALPM_128B(u),
                                 lpm_cfg->defip_flags & BCM_L3_IPMC);
        rv = _tcam_cfg_insert(u, zid, lpm_cfg, write_flags);
    } else if (rv == BCM_E_NONE) {
        /* Found */
        rv = _tcam_cfg_write(u, lpm_cfg, index, index, 1, write_flags, TRUE);
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
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);

    rv = _tcam_cfg_match(u, lpm_cfg, &index);
    if (rv == BCM_E_NONE) {
        int zid, pkm, pfx, defip_vrf;
        defip_vrf = lpm_cfg->defip_vrf;

        pkm = ALPM_LPM_PKM(u, lpm_cfg);
        ipt = ALPM_LPM_IPT(u, lpm_cfg);
        zid = _tcam_zone_zid_get(u, app, defip_vrf, ipt, ALPM_128B(u),
                                 lpm_cfg->defip_flags & BCM_L3_IPMC);
        lpm_cfg->defip_index = index;
        _tcam_gpfx_len_get(u, defip_vrf, ipt, zid, lpm_cfg, &pfx);
        rv = _tcam_free_slot_delete(u, pkm, zid, app, pfx, index, (lpm_cfg->defip_flags & BCM_L3_IPMC));
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
    int app = alpm_app_get(u, lpm_cfg->defip_inter_flags);
    int ipmc = (lpm_cfg->defip_flags & BCM_L3_IPMC);

    rv = _tcam_cfg_match(u, lpm_cfg, index_ptr);
    if (BCM_SUCCESS(rv)) {
        /*
         * If entry is ipv4 copy to the "zero" half of the,
         * buffer, "zero" half of lpm_entry if the  original entry
         * is in the "one" half.
         */
        hw_idx = *index_ptr;
        if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
            hw_idx >>= 1;
        }

        ALPM_IER(tcam_entry_read(u, app, pkm, e, hw_idx, hw_idx));

        if (_tcam_pkm_uses_half_wide(u, pkm, ipmc)) {
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
tcam_wb_reinit_done(int u, int app)
{
    int idx;
    int prev_idx;
    int defip_table_size;
    int zn;

    for (zn = 0; zn < _TCAM_ZONE_CNT; zn++) {
        int zid, pkm;

        zid = _TCAMZ_ZID_MAKE_FROM_ZN(0, ALPM_128B(u), ALPM_TCAM_ZONED(u, app), zn);

        if (!_TCAMZ_INITED(u, zid, app)) {
            continue;
        }

        prev_idx = _TCAMZ_PFX_MAX_IDX(u, zid, app);
        pkm = _tcam_zone_db_type_to_pkm(u, _TCAMZ_DB_TYPE(u, zid, app));
        defip_table_size = tcam_table_size(u, app, pkm);
        if (ALPM_ACL_EN(u) && alpm_acl_app(app)) {
            defip_table_size += _TCAMZ_PFX_BASE_IDX(u, zid, app);
        }

        if (ALPM_MODE_CHK(u, BCM_ALPM_MODE_COMBINED)) {
            /* Combined search mode */
            _TCAM_STATE_PREV(u, zid, app, _TCAMZ_PFX_MAX_IDX(u, zid, app)) = -1;

            for (idx = _TCAMZ_PFX_MAX_IDX(u, zid, app); idx > -1; idx--) {
                if (_TCAM_STATE_START(u, zid, app, idx) == -1) {
                    continue;
                }

                _TCAM_STATE_PREV(u, zid, app, idx) = prev_idx;
                _TCAM_STATE_NEXT(u, zid, app, prev_idx) = idx;

                _TCAM_STATE_FENT(u, zid, app, prev_idx) =                   \
                                 _TCAM_STATE_START(u, zid, app, idx) -      \
                                 _TCAM_STATE_END(u, zid, app, prev_idx) - 1;
                prev_idx = idx;

            }

            _TCAM_STATE_NEXT(u, zid, app, prev_idx) = -1;
            _TCAM_STATE_FENT(u, zid, app, prev_idx) =                       \
                             defip_table_size -                       \
                             _TCAM_STATE_END(u, zid, app, prev_idx) - 1;
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
            _TCAM_STATE_PREV(u, zid, app, _TCAMZ_PFX_MAX_IDX(u, zid, app)) = -1;
            for (idx = _TCAMZ_PFX_MAX_IDX(u, zid, app); idx > -1; idx--) {
                if (-1 == _TCAM_STATE_START(u, zid, app, idx)) {
                    continue;
                }

                _TCAM_STATE_PREV(u, zid, app, idx) = prev_idx;
                _TCAM_STATE_NEXT(u, zid, app, prev_idx) = idx;

                _TCAM_STATE_FENT(u, zid, app, prev_idx) =                    \
                                 _TCAM_STATE_START(u, zid, app, idx) -       \
                                 _TCAM_STATE_END(u, zid, app, prev_idx) - 1;

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

            _TCAM_STATE_NEXT(u, zid, app, prev_idx) = -1;
            _TCAM_STATE_FENT(u, zid, app, prev_idx) =
                tab_idx_max - _TCAM_STATE_END(u, zid, app, prev_idx) - 1;
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
tcam_wb_reinit(int u, int app, int vrf_id, int pkm, int idx, int key_len, int mc)
{
    int zid, inst_id;
    int pfx_len, ipt;

    ipt = ALPM_PKM2IPT(pkm);
    zid = _tcam_zone_zid_get(u, app, ALPM_VRF_ID_TO_VRF(u, vrf_id), ipt, ALPM_128B(u), mc);
    inst_id = _tcam_zone_inst_id_get(u, ALPM_VRF_ID_TO_VRF(u, vrf_id), mc);

    tcam_pfx_len_get(u, pkm, zid, app, inst_id, key_len, &pfx_len);

    if (_TCAM_STATE_VENT(u, zid, app, pfx_len) == 0) {
        _TCAM_STATE_START(u, zid, app, pfx_len) = idx;
        _TCAM_STATE_END(u, zid, app, pfx_len) = idx;
    } else {
        _TCAM_STATE_END(u, zid, app, pfx_len) = idx;
    }

    _TCAM_STATE_VENT(u, zid, app, pfx_len)++;

    return (BCM_E_NONE);
}

#else
typedef int bcm_esw_alpm2_alpm_tcam_not_empty; /* Make ISO compilers happy. */
#endif /* ALPM_ENABLE */
