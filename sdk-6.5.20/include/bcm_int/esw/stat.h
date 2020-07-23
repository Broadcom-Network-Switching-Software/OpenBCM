/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file contains STAT definitions internal to the BCM library.
 */

#ifndef _BCM_INT_STAT_H
#define _BCM_INT_STAT_H

#include <bcm/stat.h>

/*
 * Utility routines for statistics accumulation
 */

/* The REG_* macros require the following declaration in any function which
 * uses them:
 */

#define REG_MATH_DECL \
        uint64 reg_val

#define _BCM_SYNC_MODE_NO_SYNC  0 /* No hardware sync. */
#define _BCM_SYNC_MODE_SYNC     1 /* sync hardware and update cache. */
#define _BCM_SYNC_MODE_NO_CACHE 2 /* sync hardware and no cache update. */

#define REG_ADD(unit, port, sync_mode, reg, val)                           \
    if (SOC_REG_IS_COUNTER_TABLE(unit, reg) ||                             \
        (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg))) {  \
        if (sync_mode == _BCM_SYNC_MODE_NO_CACHE) {                        \
            SOC_IF_ERROR_RETURN(soc_counter_direct_get(unit, port, reg,    \
                                                       0, &reg_val));      \
        } else if (sync_mode) {                                            \
            SOC_IF_ERROR_RETURN(soc_counter_sync_get(unit, port, reg,      \
                                                0, &reg_val));             \
        } else {                                                           \
            SOC_IF_ERROR_RETURN(soc_counter_get(unit, port, reg,           \
                                                0, &reg_val));             \
        }                                                                  \
        COMPILER_64_ADD_64(val, reg_val);                                  \
    }
#define REG_ADD_IDX(unit, port, sync_mode, reg, val, ar_idx)               \
    if (SOC_REG_IS_COUNTER_TABLE(unit, reg) ||                             \
        (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg))) {  \
        if (sync_mode) {                                                   \
            SOC_IF_ERROR_RETURN(soc_counter_sync_get(unit, port, reg,      \
                                                ar_idx, &reg_val));        \
        } else {                                                           \
            SOC_IF_ERROR_RETURN(soc_counter_get(unit, port, reg,           \
                                                ar_idx, &reg_val));        \
        }                                                                  \
        COMPILER_64_ADD_64(val, reg_val);                                  \
    }
#define REG_SUB(unit, port, sync_mode, reg, val)                           \
    if (SOC_REG_IS_COUNTER_TABLE(unit, reg) ||                             \
        (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg))) {  \
        if (sync_mode) {                                                   \
            SOC_IF_ERROR_RETURN(soc_counter_sync_get(unit, port, reg,      \
                                                0, &reg_val));             \
        } else {                                                           \
            SOC_IF_ERROR_RETURN(soc_counter_get(unit, port, reg,           \
                                                0, &reg_val));             \
        }                                                                  \
        if (COMPILER_64_GT(val, reg_val)) {                                \
            COMPILER_64_SUB_64(val, reg_val);                              \
        } else {                                                           \
            COMPILER_64_ZERO(val);                                         \
        }                                                                  \
    }
#define REG_SUB_IDX(unit, port, sync_mode, reg, val, ar_idx)               \
    if (SOC_REG_IS_COUNTER_TABLE(unit, reg) ||                             \
        (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg))) {  \
        if (sync_mode) {                                                   \
            SOC_IF_ERROR_RETURN(soc_counter_sync_get(unit, port, reg,      \
                                                ar_idx, &reg_val));        \
        } else {                                                           \
            SOC_IF_ERROR_RETURN(soc_counter_get(unit, port, reg,           \
                                                ar_idx, &reg_val));        \
        }                                                                  \
        COMPILER_64_SUB_64(val, reg_val);                                  \
    }
#define REG_CLEAR(unit, port, reg, val)                                    \
    if (SOC_REG_IS_COUNTER_TABLE(unit, reg) ||                             \
        (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg))) {  \
            SOC_IF_ERROR_RETURN(soc_counter_clear_by_port_reg(unit,        \
                                                              port, reg,   \
                                                              0, val));    \
    }
#define REG_CLEAR_IDX(unit, port, sync_mode, reg, val, ar_idx)             \
    if (SOC_REG_IS_COUNTER_TABLE(unit, reg) ||                             \
        (SOC_REG_IS_VALID(unit, reg) && SOC_REG_IS_COUNTER(unit, reg))) {  \
            SOC_IF_ERROR_RETURN(soc_counter_clear_by_port_reg(unit,        \
                                                              port, reg,   \
                                                              ar_idx,      \
                                                              val));       \
    }

#define BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ctr_reg, val)      \
    BCM_IF_ERROR_RETURN (_bcm_stat_reg_op(unit, port, sync_mode, reg_op, ctr_reg, val))

#define BCM_STAT_GENERIC_REG_OPER(unit, port, sync_mode, reg_op, ctr_reg, val)      \
    if (soc_feature(unit, soc_feature_cxl_mib) ||                                   \
        soc_feature(unit, soc_feature_cd_mib)) {                                    \
        soc_reg_t ctr_reg_adj = ctr_reg##r;                                         \
                                                                                    \
       if (IS_CL_PORT(unit, port)) {                                               \
            if (SOC_REG_IS_VALID(unit, CLMIB_##ctr_reg##r)) {                          \
                ctr_reg_adj = CLMIB_##ctr_reg##r;                                      \
            }                                                                       \
        } else if (IS_XL_PORT(unit, port)) {                                        \
            if (SOC_REG_IS_VALID(unit, XLMIB_##ctr_reg##r)) {                          \
                ctr_reg_adj = XLMIB_##ctr_reg##r;                                      \
            }                                                                       \
        } else if (IS_CD_PORT(unit, port)) {                                        \
            if (SOC_REG_IS_COUNTER_TABLE(unit, CDMIB_##ctr_reg##f +                 \
                                         SOC_COUNTER_TABLE_FIELD_START)) {          \
                ctr_reg_adj = CDMIB_##ctr_reg##f + SOC_COUNTER_TABLE_FIELD_START;   \
            }                                                                       \
        }                                                                           \
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ctr_reg_adj, val);          \
    } else {                                                                         \
        BCM_STAT_REG_OPER(unit, port, sync_mode, reg_op, ctr_reg##r, val);          \
    }

#define BCM_STAT_GENERIC_REG_VALIDATE(unit, port, ctr, valid)                  \
    if (soc_feature(unit, soc_feature_cxl_mib) ||                              \
        soc_feature(unit, soc_feature_cd_mib)) {                               \
       valid = FALSE;                                                          \
       if (IS_CL_PORT(unit, port)) {                                           \
            if (SOC_REG_IS_VALID(unit, CLMIB_##ctr##r)) {                      \
                valid = TRUE;                                                  \
            }                                                                  \
        } else if (IS_XL_PORT(unit, port)) {                                   \
            if (SOC_REG_IS_VALID(unit, XLMIB_##ctr##r)) {                      \
                valid = TRUE;                                                  \
            }                                                                  \
        } else if (IS_CD_PORT(unit, port)) {                                   \
            if (SOC_REG_IS_COUNTER_TABLE(unit, CDMIB_##ctr##f +                \
                                         SOC_COUNTER_TABLE_FIELD_START)) {     \
                valid = TRUE;                                                  \
            }                                                                  \
        }                                                                      \
    } else {                                                                   \
        valid = FALSE;                                                         \
        if (SOC_REG_IS_VALID(unit, ctr##r)) {                                  \
            valid = TRUE;                                                      \
        }                                                                      \
    }

/*
 * For collecting addition non-DMA counters, unlike the non-DMA counter
 * in the soc_counter_non_dma_t, the counters collected here are not shown
 * in "show counter" command.
 */
enum {
    _BCM_STAT_EXTRA_COUNTER_EGRDROPPKTCOUNT = 0,
    _BCM_STAT_EXTRA_COUNTER_COUNT
};

typedef enum _bcm_stat_oper_e {
    _BCM_STAT_GET,
    _BCM_STAT_CLEAR
}_bcm_stat_oper_t;

typedef enum _bcm_stat_reg_oper_e {
    _BCM_STAT_REG_ADD,
    _BCM_STAT_REG_SUB,
    _BCM_STAT_REG_CLEAR
}_bcm_stat_reg_oper_t;

typedef struct _bcm_stat_extra_counter_s {
    soc_reg_t reg;
    uint32 *ctr_prev;
    uint64 *count64;
} _bcm_stat_extra_counter_t;

/* Oversize packet error control */
extern SHR_BITDCL *_bcm_stat_ovr_control;

#define COUNT_OVR_ERRORS(unit)                                    \
    (soc_feature(unit, soc_feature_stat_jumbo_adj) &&             \
     ((_bcm_stat_ovr_control != NULL) &&                          \
      (SHR_BITGET(&_bcm_stat_ovr_control[(unit)],(port)))))

/* Oversize packet size threshold accessor functions */
extern int _bcm_esw_stat_ovr_threshold_set(int unit, bcm_port_t port, int value);
extern int _bcm_esw_stat_ovr_threshold_get(int unit, bcm_port_t port, int *value);
extern int _bcm_esw_stat_ovr_error_control_set(int unit, bcm_port_t port, int value);
extern int _bcm_esw_stat_ovr_error_control_get(int unit, bcm_port_t port, int *value);

extern int
_bcm_stat_reg_op(int unit, bcm_port_t port,
                 int sync_mode, int reg_op,
                 soc_reg_t ctr_reg, uint64 *val);
extern int
_bcm_stat_reg_idx_op(int unit, bcm_port_t port,
                     int sync_mode, int reg_op,
                     soc_reg_t ctr_reg, int ar_idx,
                     uint64 *val);


/* Library-private functions exported from stat_fe.c */

extern int _bcm_stat_fe_get_set(int unit, bcm_port_t port, 
                                int sync_mode, int stat_op, 
                                bcm_stat_val_t type, uint64 *val);

/* Library-private functions exported from stat_ge.c */

extern int _bcm_stat_ge_get_set(int unit, bcm_port_t port, int sync_mode, 
                                int stat_op, bcm_stat_val_t type, 
                                uint64 *val, int incl_non_ge_stat);

/* Library-private functions exported from stat_xe.c */

extern int _bcm_stat_xe_get_set(int unit, bcm_port_t port, 
                                int sync_mode, int stat_op, 
                                bcm_stat_val_t type, uint64 *val);

/* Library-private functions exported from stat_hg.c */

extern int _bcm_stat_hg_get_set(int unit, bcm_port_t port, 
                                int sync_mode, int stat_op, 
                                bcm_stat_val_t type, uint64 *val);

/* Library-private functions exported from stat_generic.c */

extern int _bcm_stat_generic_get_set(int unit, bcm_port_t port,
                                     int sync_mode, int stat_op, 
                                     bcm_stat_val_t type, uint64 *val);

/* Library-private functions exported from stat_mxq.c */
extern int _bcm_stat_mxq_get_set(int unit, bcm_port_t port, int sync_mode,
                                 int stat_op,
                      bcm_stat_val_t type, uint64 *val);

/* Library-private functions exported from stat_clmib.c */
extern int bcmi_stat_clmib_get_set(int unit, bcm_port_t port,
                                   int sync_mode, int stat_op,
                                   bcm_stat_val_t type, uint64 *val);

/* Library-private functions exported from stat_preempt.c */
extern int bcmi_stat_preempt_ge_get_set(int unit, bcm_port_t port,
                                        int sync_mode, int stat_op,
                                        bcm_stat_val_t type, uint64 *val, int incl_non_ge_stat);

extern int bcmi_stat_preempt_xl_get_set(int unit, bcm_port_t port,
                                        int sync_mode, int stat_op,
                                        bcm_stat_val_t type, uint64 *val);

extern int bcmi_stat_preempt_get_set(int unit, bcm_port_t port,
                                        int sync_mode, int stat_op,
                                        bcm_stat_val_t type, uint64 *val);

extern int _bcm_stat_counter_extra_get(int unit, soc_reg_t reg,
                                       soc_port_t port, uint64 *val);
extern int _bcm_stat_counter_non_dma_extra_get(int unit,
                                  soc_counter_non_dma_id_t non_dma_id,
                                                    soc_port_t port,
                                                    uint64 *val);
extern int _bcm_esw_stat_detach(int unit);
extern int _bcm_esw_stat_sync(int unit);

/* Stat chunks for warm-boot functionality */
#ifdef BCM_WARM_BOOT_SUPPORT
#define _BCM_STAT_WARM_BOOT_CHUNK_PORTS    0
#define _BCM_STAT_WARM_BOOT_CHUNK_FLEX     1
extern int _bcm_esw_stat_sync_version_above_equal(int unit,uint16 version);
extern int _bcm_esw_stat_sync_version_below_equal(int unit,uint16 version);
#endif

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
extern void _bcm_stat_sw_dump(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

extern void _bcm_stat_flexctr_pool_info_sw_dump(int unit);

#endif /* !_BCM_INT_STAT_H */
