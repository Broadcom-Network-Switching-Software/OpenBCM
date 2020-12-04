/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bfcmap.h>
#include <bfcmap_int.h>

#define BFCMAP_COUNTER_TBL(mpc)      \
        ((BFCMAP_PORT_DEVICE_CONTROL(mpc))->counter_info)

#define BFCMAP_COUNTER_CB_GET_CHAN_IDX(mpc, chanId)  \
             ((mpc)->parent->counter_cb->get_chan_idx((mpc), (chanId)))

#define BFCMAP_COUNTER_CB_GET_ASSOC_IDX(mpc, chanId)  \
             ((mpc)->parent->counter_cb->get_assoc_idx((mpc), (assocId)))

#define BFCMAP_COUNTER_NUM_CHAN(mpc) (mpc)->parent->counter_cb->num_chan

#define BFCMAP_COUNTER_NUM_ASSOC(mpc) (mpc)->parent->counter_cb->num_assoc

#define BFCMAP_COUNTER_HW_CLEAR_SUPPORTED(mpc) \
                        (mpc)->parent->counter_cb->hw_clear_port_counter

#define BFCMAP_COUNTER_HW_CLEAR(mpc)           \
            (mpc)->parent->counter_cb->hw_clear_port_counter((mpc))

#define BFCMAP_COUNTER_SW_IMPLEMENTED(mpc) \
                        (mpc)->parent->counter_cb->sw_clear_port_counter

#define BFCMAP_COUNTER_SW_CLEAR(mpc)           \
            (mpc)->parent->counter_cb->sw_clear_port_counter((mpc))

#define BFCMAP_PORT_SHIFT   12

STATIC int 
_bfcmap_get_index(bfcmap_port_ctrl_t *mpc, bfcmap_counter_info_t *info,
                   int chanId, int assocId, int *index);

STATIC int 
_bfcmap_stat_op_idx(bfcmap_port_ctrl_t *mpc, 
                     bfcmap_counter_info_t *info,
                     int do_write,
                     int idx,
                     buint32_t *val32,
                     buint64_t *val64)
{
    bfcmap_counter_info_t *cInfo = info;
    buint32_t  caddr;
    buint32_t  u64array[2];

    caddr = cInfo->address + (BFCMAP_PORT(mpc) << BFCMAP_PORT_SHIFT);

    caddr += (idx * cInfo->delta);

    if (do_write) {
        if (val32) {
            BFCMAP_REG32_WRITE(mpc, caddr, val32);
        } else {
            u64array[0] = BFCMAP_COMPILER_64_LO(*val64);
            u64array[1] = BFCMAP_COMPILER_64_HI(*val64);
            if (info->flags & BFCMAP_COUNTER_F_SIZE64) {
                BFCMAP_REG64_WRITE(mpc, caddr, u64array);
            } else {
                BFCMAP_REG32_WRITE(mpc, caddr, u64array);
            }
        }
    } else {
        if (val32) {
            BFCMAP_REG32_READ(mpc, caddr, val32);
        } else {
            if (info->flags & BFCMAP_COUNTER_F_SIZE64) {
                BFCMAP_REG64_READ(mpc, caddr, u64array);
                BFCMAP_COMPILER_64_SET(*val64, u64array[1], u64array[0]);
            } else {
                BFCMAP_REG32_READ(mpc, caddr, u64array);
                BFCMAP_COMPILER_64_SET(*val64, 0, u64array[0]);
            }
        }
    }

    return BFCMAP_E_NONE;
}

STATIC int 
_bfcmap_stat_op(bfcmap_port_ctrl_t *mpc, 
                 bfcmap_counter_info_t *info,
                 int do_write,
                 int chanId, int assocId,
                 buint32_t *val32,
                 buint64_t *val64)
{
    int        idx;

    if (info->flags & BFCMAP_COUNTER_F_SW) {
        if (do_write) {
            return mpc->parent->counter_cb->sw_set_counter(mpc, info, 
                                        chanId, assocId, val32, val64);
        } else {
            return mpc->parent->counter_cb->sw_get_counter(mpc, info, 
                                        chanId, assocId, val32, val64);
        }
    }

    if (_bfcmap_get_index(mpc, info, chanId, assocId, &idx) < 0) {
        return BFCMAP_E_NOT_FOUND;
    }

    return _bfcmap_stat_op_idx(mpc, info, do_write, idx, val32, val64);
}

STATIC bfcmap_counter_info_t* 
_bfcmap_get_counter_info(bfcmap_port_ctrl_t *mpc, bfcmap_stat_t stat)
{
    bfcmap_counter_info_t *info;

    if (((info = BFCMAP_COUNTER_TBL(mpc)) == NULL) || 
        (stat >= bfcmap_stat__count)) {
        return NULL;
    }

    info += stat;
    return ((info->flags == 0) || 
            (info->flags & BFCMAP_COUNTER_F_NOT_IMPLEMENTED)) ? 
            NULL : info;
}

STATIC int 
_bfcmap_get_index(bfcmap_port_ctrl_t *mpc, 
                   bfcmap_counter_info_t *info,
                   int chanId, int assocId, int *index)
{
#if 0
    if (info->flags & BFCMAP_COUNTER_F_SECURE_CHAN) {
        if (!BFCMAP_CHAN_ID_VALID(chanId)) {
            return BFCMAP_E_PARAM;
        }
        *index = BFCMAP_COUNTER_CB_GET_CHAN_IDX(mpc, chanId);
        return (*index < 0) ? BFCMAP_E_NOT_FOUND : BFCMAP_E_NONE;
    }

    if (info->flags & BFCMAP_COUNTER_F_SECURE_ASSOC) {
        if (!BFCMAP_ASSOC_ID_VALID(assocId)) {
            return BFCMAP_E_PARAM;
        }
        *index = BFCMAP_COUNTER_CB_GET_ASSOC_IDX(mpc, assocId);
        return (*index < 0) ? BFCMAP_E_NOT_FOUND : BFCMAP_E_NONE;
    }
#endif

    *index = 0;
    return BFCMAP_E_NONE;
}

int bfcmap_int_stat_clear(bfcmap_port_ctrl_t *mpc)
{
    bfcmap_counter_info_t *cinfo, *iter;
    buint32_t  val32 = 0;
    buint64_t  val64;
    int     chanIdx, assocIdx;

    if ((cinfo = BFCMAP_COUNTER_TBL(mpc)) == NULL) {
        return BFCMAP_E_UNAVAIL;
    }

    /* If driver supports software based counters, clear them */
    if (BFCMAP_COUNTER_SW_IMPLEMENTED(mpc)) {
        BFCMAP_COUNTER_SW_CLEAR(mpc);
    }

    if (BFCMAP_COUNTER_HW_CLEAR_SUPPORTED(mpc)) {
        return BFCMAP_COUNTER_HW_CLEAR(mpc);
    }

    BFCMAP_COMPILER_64_ZERO(val64);

    /* clear port registers */
    iter = cinfo;
    while(iter->flags) {
        if (iter->flags & BFCMAP_COUNTER_F_PORT) {
            _bfcmap_stat_op_idx(mpc, iter, 1, 0, NULL, &val64);
        } else if (iter->flags & BFCMAP_COUNTER_F_SECURE_CHAN) {
            for (chanIdx = 0; chanIdx < BFCMAP_COUNTER_NUM_CHAN(mpc); 
                                                            chanIdx++) {
                _bfcmap_stat_op_idx(mpc, iter, 1, chanIdx, &val32, &val64);
            }
        } else if (iter->flags & BFCMAP_COUNTER_F_SECURE_ASSOC) {
            for (assocIdx = 0; assocIdx < BFCMAP_COUNTER_NUM_ASSOC(mpc); 
                                                            assocIdx++) {
                _bfcmap_stat_op_idx(mpc, iter, 1, assocIdx, &val32, &val64);
            }
        }
        iter++;
    }
    return BFCMAP_E_NONE;
}

int 
bfcmap_int_stat_get(bfcmap_port_ctrl_t *mpc, bfcmap_stat_t stat,
                     int chanId, int assocId, buint64_t *val)
{
    bfcmap_counter_info_t *info;

    if((stat ==	fc_class2_rxgoodframes       ) ||   
       (stat == fc_class2_rxinvalidcrc       ) ||   
       (stat == fc_class2_rxframetoolong     ) ||
       (stat == fc_class3_rxgoodframes       ) ||
       (stat == fc_class3_rxinvalidcrc       ) || 
       (stat == fc_class3_rxframetoolong     ) ||
       (stat == fc_classf_rxgoodframes       ) ||
       (stat == fc_classf_rxinvalidcrc       ) ||
       (stat == fc_classf_rxframetoolong     ) ||
       (stat == fc_rxbbc0drop                ) ||
       (stat == fc_rxsyncfail                ) ||
       (stat == fc_rxbadxword                ) ||
       (stat == fc_class2_rxruntframes       ) ||
       (stat == fc_class3_rxruntframes       ) ||
       (stat == fc_classf_rxruntframes       ) ||
       (stat == fc_class2_rxbyt              ) ||
       (stat == fc_class3_rxbyt              ) ||
       (stat == fc_classf_rxbyt              ) ||
       (stat == fc_class2_txframes           ) ||
       (stat == fc_class3_txframes           ) ||
       (stat == fc_classf_txframes           ) ||
       (stat == fc_txframes                  ) ||
       (stat == fc_class2_txoversized_frames ) ||
       (stat == fc_class3_txoversized_frames ) ||
       (stat == fc_classf_txoversized_frames ) ||
       (stat == fc_txoversized_frames        ) ||
       (stat == fc_class2_txbyt              ) ||
       (stat == fc_class3_txbyt              ) ||
       (stat == fc_classf_txbyt              )) {
        BFCMAP_SAL_DBG_PRINTF("Unsupported Stat Counter Requested\n");
        return BFCMAP_E_PARAM;
    }

    if ((info = _bfcmap_get_counter_info(mpc, stat)) == NULL) {
        return BFCMAP_E_UNAVAIL;
    }

    return _bfcmap_stat_op(mpc, info, 0, chanId, assocId, NULL, val);
}

int 
bfcmap_int_stat_get32(bfcmap_port_ctrl_t *mpc, bfcmap_stat_t stat,
                       int chanId, int assocId, buint32_t *val)
{
    bfcmap_counter_info_t *info;

    if ((info = _bfcmap_get_counter_info(mpc, stat)) == NULL) {
        return BFCMAP_E_UNAVAIL;
    }

    return _bfcmap_stat_op(mpc, info, 0, chanId, assocId, val, NULL);
}

int 
bfcmap_int_stat_set(bfcmap_port_ctrl_t *mpc, bfcmap_stat_t stat, 
                     int chanId, int assocId, buint64_t val)
{
    bfcmap_counter_info_t *info;

    if ((info = _bfcmap_get_counter_info(mpc, stat)) == NULL) {
        return BFCMAP_E_UNAVAIL;
    }

    return _bfcmap_stat_op(mpc, info, 1, chanId, assocId, NULL, &val);
}

int 
bfcmap_int_stat_set32(bfcmap_port_ctrl_t *mpc, bfcmap_stat_t stat, 
                       int chanId, int assocId, buint32_t val)
{
    bfcmap_counter_info_t *info;

    if ((info = _bfcmap_get_counter_info(mpc, stat)) == NULL) {
        return BFCMAP_E_UNAVAIL;
    }

    return _bfcmap_stat_op(mpc, info, 1, chanId, assocId, &val, NULL);
}

bfcmap_counter_info_t *
bfcmap_int_stat_tbl_create(struct bfcmap_dev_ctrl_s *mdc, 
                            bfcmap_counter_info_t *info)
{
    bfcmap_counter_info_t *stats_entry, *stats_tbl;
    bfcmap_stat_t         stat;

    BFCMAP_COMPILER_SATISFY(mdc);

    if ((stats_tbl = BFCMAP_SAL_MALLOC(
        ((bfcmap_stat__count + 1) * sizeof(bfcmap_counter_info_t)),
                                     "BFCMAP_stats_tbl")) == NULL) {
        return NULL;
    }

    for (stat = 0, stats_entry = stats_tbl;
                        stat < bfcmap_stat__count; stat++) {
        stats_entry[stat].flags = BFCMAP_COUNTER_F_NOT_IMPLEMENTED;
    }

    stats_entry = &stats_tbl[stat];
    stats_entry->flags = 0; /* Flag = 0 marks end of stats table */

    while (info->flags) {
        if (info->flags &  BFCMAP_COUNTER_F_NOT_IMPLEMENTED) {
            info++;
            continue;
        }

        stat = info->stat;
        stats_entry = &stats_tbl[stat];

        stats_entry->stat  = info->stat;
        stats_entry->flags = info->flags;
        stats_entry->delta = info->delta;
        stats_entry->address = info->address;

        info++;
    }

    return stats_tbl;
}

