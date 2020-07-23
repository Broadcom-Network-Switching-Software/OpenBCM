/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * MMU/Cosq soc routines
 */

#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/trident2.h>
#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/td2_td2p.h>

#define _TD2_XPIPE 0
#define _TD2_YPIPE 1

#define _TD2_HSP_PORT_MAX_L0    5
#define _TD2_HSP_PORT_MAX_L1    10
#define _TD2_HSP_PORT_MAX_COS   8

#define _SOC_TD2_IS_HSP_PORT(u,p)   \
        ((IS_CE_PORT((u),(p))) ||   \
        ((IS_HG_PORT((u),(p))) && (SOC_INFO((u)).port_speed_max[(p)] >= 100000)))

typedef enum {
    _SOC_TD2_LLS_OP_TEAR = 0,
    _SOC_TD2_LLS_OP_SETUP,
    _SOC_TD2_LLS_OP_TRAVERSE
} _soc_td2p_lls_op_mode_t;

/* number of registers in the set to support Dynamic Scheduling */
#define _SOC_TD2PLUS_DYN_REGISTER_SET    4  

typedef struct soc_td2_fc_map_shadow_memory_s {
    soc_mem_t mem;
    uint32  *mem_shadow;
}soc_td2_fc_map_shadow_memory_t;

typedef struct soc_td2_fc_map_shadow_s {
    soc_td2_fc_map_shadow_memory_t* shadow_array;
    int mem_count;
}soc_td2_fc_map_shadow_t;


static soc_mem_t fc_map_mems[] = {
    MMU_INTFI_XPIPE_FC_MAP_TBL0m,
    MMU_INTFI_XPIPE_FC_MAP_TBL1m,
    MMU_INTFI_YPIPE_FC_MAP_TBL0m,
    MMU_INTFI_YPIPE_FC_MAP_TBL1m
    };

static soc_td2_fc_map_shadow_t soc_td2_fc_map_shadow[SOC_MAX_NUM_DEVICES];

static int _td2_invalid_ptr[SOC_MAX_NUM_DEVICES][SOC_TD2_NODE_LVL_MAX];
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
static int _td2p_prev_invalid_ptr[SOC_MAX_NUM_DEVICES][SOC_TD2_NODE_LVL_MAX];
#endif

/* Structure to maintain the ports that are undergoing dyn sched*/
typedef struct _soc_td2p_port_dyn_sched_s {
    sal_mutex_t     lock;
    int             port[_SOC_TD2PLUS_DYN_REGISTER_SET];
    int             _soc_td2plus_init_done[SOC_MAX_NUM_DEVICES];
} _soc_td2p_port_dyn_sched_t;

static _soc_td2p_port_dyn_sched_t _td2p_dyn_sched_unit_data[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *     _soc_td2plus_alloc_dyn_set
 * Purpose:
 *     This function allocates the register available from
 *     the set of SP_WERR_DYN_CHANGEnA/nB/nC (n=0,1,2,3) to the port and 
 *     updates the structure _td2p_dyn_sched_unit_data to keep track 
 *     of the used registers from the set.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 *     r_a        - (OUT) SP_WERR_DYN_CHANGEnA register
 *     r_b        - (OUT) SP_WERR_DYN_CHANGEnB register
 *     r_ctrl     - (OUT) SP_WERR_DYN_CHANGEnC register
 * Returns:
 *     SOC_E_XXX
 */
STATIC int _soc_td2plus_alloc_dyn_set(int unit, int port, 
            soc_reg_t *r_a, soc_reg_t *r_b, soc_reg_t *r_ctrl)
{
    _soc_td2p_port_dyn_sched_t *pcb;
    int i, empty_at = -1, rv = SOC_E_NONE, pipe;
    soc_info_t *si;

    soc_reg_t dyn_change_a_x[] = {
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_0Ar,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_1Ar,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_2Ar,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_3Ar
                                 };
    soc_reg_t dyn_change_b_x[] = {
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_0Br,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_1Br,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_2Br,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_3Br
                                 };
    
    soc_reg_t dyn_change_ctrl_x[] = {
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_0Cr,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_1Cr,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_2Cr,
                                       ES_PIPE0_LLS_SP_WERR_DYN_CHANGE_3Cr
                                    };
        
    soc_reg_t dyn_change_a_y[] = {
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_0Ar,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_1Ar,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_2Ar,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_3Ar
                                 };
    soc_reg_t dyn_change_b_y[] = {
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_0Br,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_1Br,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_2Br,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_3Br
                                 };
    
    soc_reg_t dyn_change_ctrl_y[] = {
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_0Cr,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_1Cr,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_2Cr,
                                       ES_PIPE1_LLS_SP_WERR_DYN_CHANGE_3Cr
                                    };

    si = &SOC_INFO(unit); 
    pipe = (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) ? 0 : 1;

    pcb = &_td2p_dyn_sched_unit_data[unit];
    
    sal_mutex_take(pcb->lock, sal_sem_FOREVER);
    for (i = 0; i < _SOC_TD2PLUS_DYN_REGISTER_SET; i++) {
        if (pcb->port[i] == -1) {
            empty_at = i;
            break;
        } else if (pcb->port[i] == port) {
            rv = SOC_E_BUSY;
            break;
        }
    }

    if (!rv && (empty_at >= 0)) {
        pcb->port[empty_at] = port;
    }
    
    sal_mutex_give(pcb->lock);

    if (rv) {
        return rv;
    }

    if (empty_at == -1) {
        return SOC_E_BUSY;
    }
    if (!pipe) {
        *r_a = dyn_change_a_x[empty_at];
        *r_b = dyn_change_b_x[empty_at];
        *r_ctrl = dyn_change_ctrl_x[empty_at];
    } else {
        *r_a = dyn_change_a_y[empty_at];
        *r_b = dyn_change_b_y[empty_at];
        *r_ctrl = dyn_change_ctrl_y[empty_at];
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *     _soc_td2plus_free_dyn_set
 * Purpose:
 *     This function is called after the switchover is done 
 *     to update the structure _td2p_dyn_sched_unit_data by  
 *     removing the port from it. This makes the register
 *     used for the request available for the next request.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 * Returns:
 *     SOC_E_XXX
 */
STATIC int _soc_td2plus_free_dyn_set(int unit, int port)
{
    _soc_td2p_port_dyn_sched_t *pcb;
    int i;

    pcb = &_td2p_dyn_sched_unit_data[unit];

    sal_mutex_take(pcb->lock, sal_sem_FOREVER);
    for (i = 0; i < _SOC_TD2PLUS_DYN_REGISTER_SET; i++) {
        if (pcb->port[i] == port) {
            pcb->port[i] = -1;
        }
    }
    sal_mutex_give(pcb->lock);
    
    return SOC_E_NONE;
}

STATIC int
soc_td2_init_invalid_pointers(int unit)
{
    uint32 mmu_bmp[4];
    int ii, port;
    int phy_port, mmu_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    sal_memset(mmu_bmp, 0, sizeof(uint32)*4);
    
    _td2_invalid_ptr[unit][SOC_TD2_NODE_LVL_ROOT] = -1;
    _td2_invalid_ptr[unit][SOC_TD2_NODE_LVL_L1] = 
                        soc_mem_index_max(unit, ES_PIPE0_LLS_L0_PARENTm);
    _td2_invalid_ptr[unit][SOC_TD2_NODE_LVL_L2] = 
                        soc_mem_index_max(unit, ES_PIPE0_LLS_L1_PARENTm);

    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        mmu_bmp[mmu_port/32] |= 1 << (mmu_port % 32);
    }

    /*PIPE0 & PIPE1 use the same base*/
    mmu_bmp[0] |= mmu_bmp[2];
    mmu_bmp[1] |= mmu_bmp[3];
    for (ii = 0; ii < (4 * 32); ii++) {
        if ((mmu_bmp[ii/32] & (1 << (ii % 32))) == 0) {
            _td2_invalid_ptr[unit][SOC_TD2_NODE_LVL_L0] = ii;
            break;
        }
    }

    if (!soc_feature(unit, soc_feature_lls_port_mema_config_match)) {
        /*Change invalid_ptr to 0 when it is over 52, avoid parity error*/
        if (_td2_invalid_ptr[unit][SOC_TD2_NODE_LVL_L0] >
                SOC_TD2_NUM_MMU_PORTS_PER_PIPE - 1) {
            _td2_invalid_ptr[unit][SOC_TD2_NODE_LVL_L0] = 0;
        }
    }
    return SOC_E_NONE;
}

int _soc_td2_invalid_parent_index(int unit, int level)
{
    return _td2_invalid_ptr[unit][level];
}

#define INVALID_PARENT(unit, level)   _soc_td2_invalid_parent_index((unit),(level))

#define _SOC_TD2_DEF_SCHED_NODE_BASE(u,p,cfg,t)   \
            (cfg)[(t)].base_factor * (p)

/*
 * Function:
 *      _soc_trident2_port_sched_type_get
 * Purpose:
 *      Gets the scheduling type of a port.
 * Parameters:
 *      unit           - (IN) Device Number
 *      port           - (IN) logical port
 * Returns:
 *      SOC_TD2_SCHED_HSP if HSP, SOC_TD2_SCHED_LLS if LLS,
 *      SOC_TD2_SCHED_UNKNOWN else. 
 */
soc_trident2_sched_type_t
_soc_trident2_port_sched_type_get(int unit, soc_port_t port)
{
    int  smode, speed;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    speed = si->port_speed_max[port];
#ifdef BCM_TRIDENT2PLUS_SUPPORT
    if (SOC_IS_TRIDENT2PLUS(unit) || SOC_IS_TITAN2PLUS(unit)) {
        SOC_IF_ERROR_RETURN(soc_td2p_port_speed_get(unit, port, &speed));
    }
#endif

    if (speed >= 100000 ||
        (speed >= 40000 && si->frequency < 760) ||
        SOC_PBMP_MEMBER(si->eq_pbm, port)) {
        return SOC_TD2_SCHED_HSP;
    } else if (speed < 40000) {
        return SOC_TD2_SCHED_LLS;
    } else {
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        /* Get the current state of port_sched_hsp. Flexport overrides this
           since the config parameter is based on logical port so when
           we flex, the config parameter is no longer valid. */
        if (SOC_IS_TRIDENT2PLUS(unit)) {
            SOC_IF_ERROR_RETURN(soc_td2p_port_sched_hsp_get(unit, port, &smode));
        } else 
#endif
        {
            smode = soc_property_port_get(unit, port, spn_PORT_SCHED_HSP, 0);
        }
        return (smode == 0) ? SOC_TD2_SCHED_LLS : SOC_TD2_SCHED_HSP;
    }
    return SOC_TD2_SCHED_UNKNOWN;
}

int soc_td2_port_common_attributes_get(int unit, int port, int *pipe,
                                       int *mmu_port, int *phy_port)
{
    int lphy_port, lmmu_port;
    soc_info_t *si;

    si = &SOC_INFO(unit);

    if (pipe) {
        *pipe = (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) ? 0 : 1;
    }

    lphy_port = si->port_l2p_mapping[port];
    /* lmmu_port = si->max_port_p2m_mapping[lphy_port]; */
    lmmu_port = si->port_p2m_mapping[lphy_port];

    if (phy_port) {
        *phy_port = lphy_port;
    }

    if (mmu_port) {
        *mmu_port = lmmu_port;
    }
    return 0;
}

int _soc_trident2_root_scheduler_index(int unit, int port)
{
    int mmu_port, in_pipe;

    SOC_IF_ERROR_RETURN(
        soc_td2_port_common_attributes_get(unit, port, &in_pipe, &mmu_port, NULL));
    return (in_pipe == 0) ? mmu_port : mmu_port - 64;
}
STATIC int
_soc_td2_flush_queue(int unit, int port, int queue_hw_index)
{
    
    return 0;
}

int
soc_td2_cosq_set_sched_parent(int unit, soc_port_t port,
                              int level, int hw_index,
                              int parent_hw_idx)
{
    soc_trident2_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], fval, fval1, rval;
    soc_mem_t mem;
    int l0off, l1off, idx;
    uint32 *bmap = NULL;
    int parent_index = 0;

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%d : %d parent:%d\n"),
               port, level - 1, hw_index, parent_hw_idx));

    sched_type = _soc_trident2_port_sched_type_get(unit, port);

    if (sched_type == SOC_TD2_SCHED_LLS) {
        mem = _SOC_TD2_NODE_PARENT_MEM(unit, port, level);
        if (mem == INVALIDm) {
            return SOC_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
        if (soc_mem_field32_get(unit, mem, entry, C_PARENTf) == parent_hw_idx) {
            return SOC_E_NONE;
        }

        /* disconnecting the queue , flush it */
        if ((parent_hw_idx == INVALID_PARENT(unit, level)) && 
                                            (level == SOC_TD2_NODE_LVL_L2)) {
            SOC_IF_ERROR_RETURN(_soc_td2_flush_queue(unit, port, hw_index));
        }
        
        soc_mem_field32_set(unit, mem, entry, C_PARENTf, parent_hw_idx);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
        if (level == SOC_TD2_NODE_LVL_L0) {
            bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
        } else if (level == SOC_TD2_NODE_LVL_L1) {
            bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
        } else if (level == SOC_TD2_NODE_LVL_L2) {
            bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
        }
        parent_index = _soc_td2_invalid_parent_index(unit, level);
        /* SDK has allocated memory space according to value of "bmap"
        * during process of "_soc_egress_metering_freeze".
        * If we set a new number during process of "_soc_egress_metering_freeze", the buffer
        * may overflow. So we add a lock at this.  We should add other actions
        *  between lock and unklock.
        */
        SOC_LLS_SCHEDULER_LOCK(unit);
        if (parent_hw_idx == parent_index) {
            SHR_BITCLR(bmap, hw_index);
        } else {
            SHR_BITSET(bmap, hw_index);
        }
        SOC_LLS_SCHEDULER_UNLOCK(unit);
    } else if (sched_type == SOC_TD2_SCHED_HSP) {
        if (level == SOC_TD2_NODE_LVL_L1) {
            l0off = parent_hw_idx % 5;
            l1off = hw_index % 10;

            /* disconnect from any existing node */
            for (idx = 0; idx < 5; idx++) {
                if ((l0off == idx) &&
                    (parent_hw_idx != INVALID_PARENT(unit, level))) {
                    continue;
                }

                /*
                 * For L0.1, L0.2, and L0.3 nodes: 
                 *     The bitmap is organized as {L1.7, L1.6, ..., L1.0}, 
                 * For L0.4 node: 
                 *     The bit 7 selects L1.9 and bit6 selects L1.8. 
                 *     The rest bits are not used and should be set to 0.
                 */
                if ((l1off >= 8) && (idx != 4)) {
                    continue;
                } else if ((l1off < 8) && (idx == 4)) {
                    continue;
                }

                SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                                        unit, port, idx, &rval));
                fval1 = fval = soc_reg_field_get(unit, 
                                      HSP_SCHED_L0_NODE_CONNECTION_CONFIGr, 
                                      rval, CHILDREN_CONNECTION_MAPf);
                /* L1.8 & L1.9 need to adjust hw offset */
                if (l1off >= 8) {
                    l1off -= 2;
                }
                fval &= ~(1 << l1off);
                if (fval1 == fval) {
                    continue;
                }
                soc_reg_field_set(unit, 
                                  HSP_SCHED_L0_NODE_CONNECTION_CONFIGr, 
                                  &rval, CHILDREN_CONNECTION_MAPf, fval);
                SOC_IF_ERROR_RETURN(
                    WRITE_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(unit, port,
                                                                idx, rval));
                break;
            }

            if (parent_hw_idx == INVALID_PARENT(unit, level)) {
                SOC_IF_ERROR_RETURN(_soc_td2_flush_queue(unit, port, hw_index));
            } else {
                SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                                unit, port, l0off, &rval));

                fval = soc_reg_field_get(unit, HSP_SCHED_L0_NODE_CONNECTION_CONFIGr, 
                                         rval, CHILDREN_CONNECTION_MAPf);

                /* L1.8 & L1.9 need to adjust hw offset */
                if (l1off >= 8) {
                    l1off -= 2;
                }
                fval |= 1 << l1off;
                soc_reg_field_set(unit, HSP_SCHED_L0_NODE_CONNECTION_CONFIGr, 
                                  &rval, CHILDREN_CONNECTION_MAPf, fval);

                SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                                        unit, port, l0off, rval));
            }
        }
    }

    return SOC_E_NONE;
}

int 
soc_td2_sched_weight_set(int unit, int port, int level,
                         int hw_index, int weight)
{
    soc_trident2_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "sched_weight_set L%d csch_index=%d wt=%d\n"),
              level, hw_index, weight));

    sched_type = _soc_trident2_port_sched_type_get(unit, port);

    if (weight > 0x7f) {
        return SOC_E_PARAM;
    }
    if (sched_type == SOC_TD2_SCHED_LLS) {
        mem = _SOC_TD2_NODE_WIEGHT_MEM(unit, port, level);
        if (mem == INVALIDm) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
        soc_mem_field32_set(unit, mem, &entry, C_WEIGHTf, weight);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
    } else if (sched_type == SOC_TD2_SCHED_HSP) {
        if ((level == SOC_TD2_NODE_LVL_L0) || (level == SOC_TD2_NODE_LVL_L1)) {
            if (level == SOC_TD2_NODE_LVL_L0) {
                hw_index = hw_index % 5;
                reg = HSP_SCHED_L0_NODE_WEIGHTr;
            } else if (level == SOC_TD2_NODE_LVL_L1) {
                hw_index = hw_index % 10;
                reg = HSP_SCHED_L1_NODE_WEIGHTr;
            }

            rval = 0;
            soc_reg_field_set(unit, reg, &rval, WEIGHTf, weight);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, hw_index, rval));
        } else if (level == SOC_TD2_NODE_LVL_L2) {
            if (hw_index < 1480)
            {
                reg = HSP_SCHED_L2_UC_QUEUE_WEIGHTr;
            } else {
                reg = HSP_SCHED_L2_MC_QUEUE_WEIGHTr;
            }

            rval = 0;
            hw_index = hw_index % 10;
            soc_reg_field_set(unit, reg, &rval, WEIGHTf, weight);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, hw_index, rval));
        }
    }
    return SOC_E_NONE;
}

int 
soc_td2_sched_weight_get(int unit, int port, int level,
                         int hw_index, int *weight)
{
    soc_trident2_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    soc_mem_t mem = INVALIDm;
    soc_reg_t reg = INVALIDr;

    sched_type = _soc_trident2_port_sched_type_get(unit, port);

    if (sched_type == SOC_TD2_SCHED_LLS) {
        mem = _SOC_TD2_NODE_WIEGHT_MEM(unit, port, level);
        if (mem == INVALIDm) {
            return SOC_E_INTERNAL;
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, hw_index, &entry));
        *weight = soc_mem_field32_get(unit, mem, &entry, C_WEIGHTf);
    } else if (sched_type == SOC_TD2_SCHED_HSP) {
        if ((level == SOC_TD2_NODE_LVL_L0) || (level == SOC_TD2_NODE_LVL_L1)) {
            if (level == SOC_TD2_NODE_LVL_L0) {
                hw_index = hw_index % 5;
                reg = HSP_SCHED_L0_NODE_WEIGHTr;
            } else if (level == SOC_TD2_NODE_LVL_L1) {
                hw_index = hw_index % 10;
                reg = HSP_SCHED_L1_NODE_WEIGHTr;
            }

            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, hw_index, &rval));
            *weight = soc_reg_field_get(unit, reg, rval, WEIGHTf);
        } else if (level == SOC_TD2_NODE_LVL_L2) {
            if (hw_index < 1480)
            {
                reg = HSP_SCHED_L2_UC_QUEUE_WEIGHTr;
            } else {
                reg = HSP_SCHED_L2_MC_QUEUE_WEIGHTr;
            }

            hw_index = hw_index % 10;
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, hw_index, &rval));
            *weight = soc_reg_field_get(unit, reg, rval, WEIGHTf);
        }
    }

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "sched_weight_get L%d csch_index=%d wt=%d\n"),
              level, hw_index, *weight));

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_td2_cosq_set_sched_child_config_dynamic
 * Purpose:
 *     This function enables the spri_vect_mode by setting the 
 *     spri_vect_mode_enable field to 1 in the LLS_CONFIG0 reg, which changes 
 *     p_num_spri[3:0] to p_vect_spri[7:0]. It allocates the register from set
 *     of SP_WERR_DYN_CHNG_nA/nB/nC registers to the port. It then sets the 
 *     SP_WERR_DYN_CHNG_nB/nC reg with the new value for the node id's parent.
 *     It sets the fields in SP_WERR_DYN_CHNG_nA with the info about the node
 *     to be switched like node_id, node_level and parent_id along with in_use
 *     field to 1, which triggers the request in H/W. The H/W clears in_use
 *     field to 0 once the switchover is complete, so we keep polling its
 *     to know about the completion of the request. 
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 *     level      - (IN) level of the node
 *     index      - (IN) index of the node
 *     num_spri   - (IN) number of SPRI child nodes
 *     first_child - (IN) first child node
 *     first_mc_child - (IN) first multicast child node
 *     ucmap        - (IN) unicast map
 *     spmap        - (IN) spri map
 *     child_index     - (IN) index number of the child
 * Returns:
 *     SOC_E_XXX
 */
int
soc_td2_cosq_set_sched_child_config_dynamic(int unit, soc_port_t port,
                                            int level, int index, 
                                            int num_spri, int first_child,
                                            int first_mc_child, uint32 ucmap,
                                            uint32 spmap, int child_index)
{
    uint32 entry[SOC_MAX_MEM_WORDS], rval, d32, timeout_val, old_spmap, f1, f2;
    soc_mem_t mem, mem2;
    int rv = SOC_E_NONE;
    soc_reg_t r_a = INVALIDr, r_b = INVALIDr, r_ctrl = INVALIDr;
    soc_timeout_t timeout;
    SOC_IF_ERROR_RETURN(READ_ES_PIPE0_LLS_CONFIG0r(unit, &rval));
     
    soc_reg_field_get(unit, ES_PIPE0_LLS_CONFIG0r, rval, SPRI_VECT_MODE_ENABLEf);
    if (!soc_reg_field_get(unit, ES_PIPE0_LLS_CONFIG0r, rval, SPRI_VECT_MODE_ENABLEf)) {
        soc_reg_field_set(unit, ES_PIPE0_LLS_CONFIG0r, &rval, SPRI_VECT_MODE_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(WRITE_ES_PIPE0_LLS_CONFIG0r(unit, rval));
        SOC_IF_ERROR_RETURN(WRITE_ES_PIPE1_LLS_CONFIG0r(unit, rval)); 
    }

    SOC_IF_ERROR_RETURN(_soc_td2plus_alloc_dyn_set(unit, port, 
                                               &r_a, &r_b, &r_ctrl));

    mem = _SOC_TD2_NODE_CONFIG_MEM(unit, port, level);
    mem2 = _SOC_TD2_NODE_CONFIG_MEM2(unit, port, level);
    if (mem == INVALIDm || mem2 == INVALIDm) {
        return SOC_E_INTERNAL;
    }

    /* Getting the old spmap */    
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    f1 = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
    f2 = soc_mem_field32_get(unit, mem, &entry, P_VECT_SPRI_7_4f);
    old_spmap = f1 | (f2 << 4);

    LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit,
            "Port:%d L%s%d config : index=%d FC=%d FMC=%d UMAP=0x%x NUMSP=%d\n"), 
             port, (level==0) ? "r" : "", level - 1, 
             index, first_child, first_mc_child, ucmap, num_spri));

    /* Setting the memory fields based on the new spmap */
    soc_mem_field32_set(unit, mem, &entry, P_NUM_SPRIf, spmap & 0xf);
    soc_mem_field32_set(unit, mem, &entry, P_VECT_SPRI_7_4f, (spmap >> 4) & 0xf);

    if ((mem == ES_PIPE0_LLS_L1_MEMA_CONFIGm) ||
        (mem == ES_PIPE1_LLS_L1_MEMA_CONFIGm)) {
        soc_mem_field32_set(unit, mem, &entry, P_START_UC_SPRIf, first_child);
        soc_mem_field32_set(unit, mem, &entry, P_START_MC_SPRIf, first_mc_child);
        soc_mem_field32_set(unit, mem, &entry, P_SPRI_SELECTf, 
                            (num_spri > 0) ? ucmap : 0);
    } else {
        soc_mem_field32_set(unit, mem, &entry, P_START_SPRIf, first_child);
    }

    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));
    SOC_IF_ERROR_RETURN(
        soc_mem_write(unit, mem2, MEM_BLOCK_ANY, index, &entry));
        

    rval = 0;
    soc_bits_get(entry, 0, 31, &d32);
    soc_reg_field_set(unit, r_ctrl, &rval, LLS_PARENT_CONFIGf, d32);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_ctrl, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_bits_get(entry, 32, 40, &d32);
    soc_reg_field_set(unit, r_b, &rval, LLS_PARENT_CONFIGf, d32);
    soc_reg_field_set(unit, r_b, &rval, LLS_PARENT_CONFIG_ORIGINAL_SP_VECTORf,
                                            old_spmap);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_b, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, r_a, &rval, NODE_LEVELf, level + 1);
    soc_reg_field_set(unit, r_a, &rval, NODE_IDf, child_index);
    soc_reg_field_set(unit, r_a, &rval, PARENT_IDf, index);
    soc_reg_field_set(unit, r_a, &rval, IN_USEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, r_a, REG_PORT_ANY, 0, rval));

    if (!SAL_BOOT_SIMULATION) {
        timeout_val = soc_property_get(unit, spn_MMU_QUEUE_FLUSH_TIMEOUT, 20000);
        soc_timeout_init(&timeout, timeout_val, 0);

        do {
            SOC_IF_ERROR_RETURN(
                soc_reg32_get(unit, r_a, REG_PORT_ANY, 0, &rval));
                
            if (soc_timeout_check(&timeout)) {
                rv = SOC_E_TIMEOUT;
                break;
            }
        } while (soc_reg_field_get(unit, r_a, rval, IN_USEf));
    }

    SOC_IF_ERROR_RETURN(_soc_td2plus_free_dyn_set(unit, port));

    return rv;
}

int
soc_td2_cosq_set_sched_child_config(int unit, soc_port_t port,
                              int level, int index,
                              int num_spri, int first_sp_child, 
                              int first_sp_mc_child, uint32 ucmap, uint32 spmap)
{
    soc_trident2_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], rval;
    soc_mem_t mem, mem2;

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d FC=%d FMC=%d UMAP=0x%x\n"),
              port, (level == 0) ? "r" : "", level - 1, 
              index, first_sp_child, first_sp_mc_child, ucmap));

    mem = INVALIDm;

    sched_type = _soc_trident2_port_sched_type_get(unit, port);
    if (sched_type == SOC_TD2_SCHED_HSP) {
        return SOC_E_PARAM;
    }

    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        SOC_IF_ERROR_RETURN(READ_ES_PIPE0_LLS_CONFIG0r(unit, &rval));
        if (!soc_reg_field_get(unit, ES_PIPE0_LLS_CONFIG0r, rval, SPRI_VECT_MODE_ENABLEf)) {
            soc_reg_field_set(unit, ES_PIPE0_LLS_CONFIG0r, &rval,
                              SPRI_VECT_MODE_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES_PIPE0_LLS_CONFIG0r(unit, rval));
            SOC_IF_ERROR_RETURN(WRITE_ES_PIPE1_LLS_CONFIG0r(unit, rval));
        }
    }

    mem = _SOC_TD2_NODE_CONFIG_MEM(unit, port, level);
    mem2 = _SOC_TD2_NODE_CONFIG_MEM2(unit, port, level);
    /* coverity[negative_returns : FALSE] */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        /* Setting the memory fields based on the new spmap */
        soc_mem_field32_set(unit, mem, &entry, P_NUM_SPRIf, spmap & 0xf);
        soc_mem_field32_set(unit, mem, &entry, P_VECT_SPRI_7_4f,
                            (spmap >> 4) & 0xf);
    } else {
        soc_mem_field32_set(unit, mem, &entry, P_NUM_SPRIf, num_spri);
    }

    if ((mem == ES_PIPE0_LLS_L1_MEMA_CONFIGm) || 
        (mem == ES_PIPE1_LLS_L1_MEMA_CONFIGm)) {
        soc_mem_field32_set(unit, mem, &entry, P_START_UC_SPRIf, first_sp_child);
        soc_mem_field32_set(unit, mem, &entry, P_START_MC_SPRIf, 
                                                       first_sp_mc_child);
        soc_mem_field32_set(unit, mem, &entry, P_SPRI_SELECTf, 
                            (num_spri > 0) ? ucmap : 0);
    } else {
        soc_mem_field32_set(unit, mem, &entry, P_START_SPRIf, first_sp_child);
    }

    SOC_IF_ERROR_RETURN(soc_mem_write
                        (unit, mem, MEM_BLOCK_ANY, index, &entry));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, mem2, MEM_BLOCK_ANY, index, &entry));

    return SOC_E_NONE;
}

int
soc_td2_cosq_set_sched_mode(int unit, soc_port_t port, int level, int index,
                            const soc_td2_sched_mode_e mode, int weight)
{
    soc_trident2_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], mfval = 0, rval, wrr_mask, sp_mask;
    soc_mem_t mem, mem2;
    soc_reg_t reg, reg2;
    int fval, idx, parent_idx = -1;

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d MODE=%d WT=%d\n"),
              port, (level == 0) ? "r" : "", level - 1, 
              index, mode, weight));

    reg = INVALIDr;
    reg2 = INVALIDr;
    mem = INVALIDm;
    sched_type = _soc_trident2_port_sched_type_get(unit, port);
    if (sched_type == SOC_TD2_SCHED_HSP) {
        /* get the port relative index / offset */
        if (level == SOC_TD2_NODE_LVL_L0) {
            index = index % 5;
            reg = HSP_SCHED_PORT_CONFIGr;
            reg2 = HSP_SCHED_L0_NODE_CONFIGr;
            parent_idx = 0;
        } else if (level == SOC_TD2_NODE_LVL_L1) {
            index = index % 10;
            reg = HSP_SCHED_L0_NODE_CONFIGr;
            reg2 = HSP_SCHED_L1_NODE_CONFIGr;
            /* Find the Parent index for the current child */
            for (idx = 1; idx < 5; idx++) {
                SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                                    unit, port, idx, &rval));
                fval = soc_reg_field_get(unit,
                            HSP_SCHED_L0_NODE_CONNECTION_CONFIGr,
                            rval, CHILDREN_CONNECTION_MAPf);
                if (idx == 4) {
                    if ((index >= 8) && (fval & (1 << (index - 2)))) {
                        parent_idx = idx;
                        break;
                    }
                } else {
                    if (fval & (1 << index)) {
                        parent_idx = idx;
                        break;
                    }
                }
            }
        } else if (level == SOC_TD2_NODE_LVL_L2) {
            uint32 rval = 0;
            uint32 mc_group_mode = 0;
            SOC_IF_ERROR_RETURN(
                READ_HSP_SCHED_PORT_CONFIGr(unit, port, &rval));
            mc_group_mode  = soc_reg_field_get(unit, 
                                               HSP_SCHED_PORT_CONFIGr,
                                               rval, MC_GROUP_MODEf);
            reg = HSP_SCHED_L1_NODE_CONFIGr;
            if (index >= 1480) {
                reg2 = HSP_SCHED_L2_MC_QUEUE_CONFIGr;
            } else {
                reg2 = HSP_SCHED_L2_UC_QUEUE_CONFIGr;
            }
            parent_idx = index % 10;
            
            if (mc_group_mode && (index >= 1480)) {
                if ((index % 10) < _TD2_HSP_PORT_MAX_COS) {
                    reg= HSP_SCHED_L0_NODE_CONFIGr;
                    parent_idx = 0;
                }
            }
        } else {
            return SOC_E_PARAM;
        }

        if (parent_idx == -1) {
            return SOC_E_INTERNAL;
        }

        if (mode == SOC_TD2_SCHED_MODE_STRICT) {
            weight = 0;
        } else if (mode == SOC_TD2_SCHED_MODE_WRR) {
            mfval = 1;
        } else if (mode == SOC_TD2_SCHED_MODE_WDRR) {
            mfval = 0;
        } else {
            return SOC_E_PARAM;
        }

        /* selection between SP and WxRR is based on weight property. */
        SOC_IF_ERROR_RETURN(soc_td2_sched_weight_set(unit, port, 
                                                level, index, weight));

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
        wrr_mask &= ~(1 << parent_idx);
        wrr_mask |= (mfval << parent_idx);
        soc_reg_field_set(unit, reg, &rval, ENABLE_WRRf, wrr_mask);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg2, port, 0, &rval));
        sp_mask = soc_reg_field_get(unit, reg2, rval, ENABLE_SP_IN_MINf);
        if (mode == SOC_TD2_SCHED_MODE_STRICT) {
            sp_mask |= (1 << (index % 10));
        } else {
            sp_mask &= ~(1 << (index % 10));
        }
        soc_reg_field_set(unit, reg2, &rval, ENABLE_SP_IN_MINf, sp_mask);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg2, port, 0, rval));
    } else {
        if (mode == SOC_TD2_SCHED_MODE_STRICT) {
            weight = 0;
        }
        
        SOC_IF_ERROR_RETURN(soc_td2_sched_weight_set(unit, port, 
                                                level, index, weight));

        if (mode != SOC_TD2_SCHED_MODE_STRICT) {
            mem = _SOC_TD2_NODE_CONFIG_MEM(unit, port, SOC_TD2_NODE_LVL_ROOT);
            mem2 = _SOC_TD2_NODE_CONFIG_MEM2(unit, port, SOC_TD2_NODE_LVL_ROOT); 
            index = _soc_trident2_root_scheduler_index(unit, port);
            SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
            soc_mem_field32_set(unit, mem, entry, 
                    PACKET_MODE_WRR_ACCOUNTING_ENABLEf, 
                    (mode == SOC_TD2_SCHED_MODE_WRR) ? 1 : 0);
            SOC_IF_ERROR_RETURN(
                    soc_mem_write(unit, mem, MEM_BLOCK_ANY, index, &entry));
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem2, MEM_BLOCK_ANY, index, &entry));
        }
    }
    return SOC_E_NONE;
}

int
soc_td2_cosq_set_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int num_spri, int first_child, 
                              int first_mc_child, uint32 ucmap, uint32 spmap,
                              const soc_td2_sched_mode_e mode, int weight)
{
    int child_level = -1, wt = 0;
    soc_td2_sched_mode_e cur_sched_mode = SOC_TD2_SCHED_MODE_UNKNOWN;
    int sp_vec = soc_feature(unit, soc_feature_vector_based_spri);

    if (level >= SOC_TD2_NODE_LVL_L2) {
        return SOC_E_PARAM;
    }

    if (child_index >= 0) {
        soc_td2_get_child_type(unit, port, level, &child_level);
        SOC_IF_ERROR_RETURN(soc_td2_cosq_get_sched_mode(unit, port, child_level,
                                child_index, &cur_sched_mode, &wt));
    }

    /* If dynamice switchover feature is enabled, we configure the child
       based on the switchover request and trigger the switchover in  H/W */ 
    if (sp_vec && (cur_sched_mode != mode)) {
        SOC_IF_ERROR_RETURN(
            soc_td2_cosq_set_sched_child_config_dynamic(unit, port,
                                   level, index, num_spri, first_child, 
                                   first_mc_child, ucmap, spmap, child_index));
    } else {
        SOC_IF_ERROR_RETURN(soc_td2_cosq_set_sched_child_config(unit, port,
            level, index, num_spri, first_child, first_mc_child, ucmap, spmap));

    }
   
    if (child_index >= 0) {
        SOC_IF_ERROR_RETURN(soc_td2_cosq_set_sched_mode(unit, port, child_level, 
                                child_index, mode, weight));
    }

    return SOC_E_NONE;
}

int
soc_td2_cosq_get_sched_child_config(int unit, soc_port_t port,
                              int level, int index,
                              int *pnum_spri, int *pfirst_sp_child, 
                              int *pfirst_sp_mc_child, uint32 *pucmap,
                              uint32 *pspmap)
{
    soc_trident2_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS];
   
    soc_mem_t mem;
    uint32 num_spri = 0, ucmap = 0, f1, f2;
    int first_sp_child = -1, first_sp_mc_child = -1, ii;
    int sp_vec = soc_feature(unit, soc_feature_vector_based_spri);

    *pspmap = 0;

    sched_type = _soc_trident2_port_sched_type_get(unit, port);

    mem = INVALIDm;

    if (sched_type == SOC_TD2_SCHED_HSP) {
        return SOC_E_PARAM;
    }

    mem = _SOC_TD2_NODE_CONFIG_MEM(unit, port, level);
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
    
    /* If dynamic switchover feature is enabled, we calculate the spmap based 
       on the 8 bit p_vect_spri[7:0] and also the number of SPRI nodes  */    
    if (sp_vec) {
        f1 = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
        f2 = soc_mem_field32_get(unit, mem, &entry, P_VECT_SPRI_7_4f);
        *pspmap = f1 | (f2 << 4);
        if ( (f1 >> 1) == 0 && f1 > 0) {
            num_spri = 1;
        } else {
    
            for (ii = 0; ii < 8; ii++) {
                if ((1 << ii) & *pspmap) {
                    num_spri++;
                }
            }
        }
    } else {
        num_spri = soc_mem_field32_get(unit, mem, &entry, P_NUM_SPRIf);
    }
 
    if ((mem == ES_PIPE0_LLS_L1_MEMA_CONFIGm) || 
        (mem == ES_PIPE1_LLS_L1_MEMA_CONFIGm)) {
        first_sp_child = soc_mem_field32_get(unit, mem, &entry, P_START_UC_SPRIf);
        first_sp_mc_child = soc_mem_field32_get(unit, mem, 
                                         &entry, P_START_MC_SPRIf);
        ucmap = soc_mem_field32_get(unit, mem, &entry, P_SPRI_SELECTf);
    } else {
        first_sp_child = soc_mem_field32_get(unit, mem, &entry, P_START_SPRIf);
        first_sp_mc_child = 0;
    }
    
    if (num_spri == 0) {
        ucmap = 0;
    }

    if (pnum_spri) {
        *pnum_spri = num_spri;
    }
       
    if (pucmap) {
        *pucmap = ucmap;
    }

    if (pfirst_sp_child) {
        *pfirst_sp_child = first_sp_child;
    }

    if (pfirst_sp_mc_child) {
        *pfirst_sp_mc_child = first_sp_mc_child;
    }

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d FC=%d FMC=%d UMAP=0x%x\n"),
              port, (level == 0) ? "r" : "", level - 1, 
              index, first_sp_child, first_sp_mc_child, ucmap));

    return SOC_E_NONE;
}

int
soc_td2_cosq_get_sched_mode(int unit, soc_port_t port, int level, 
                            int index, soc_td2_sched_mode_e *pmode, int *weight)
{
    soc_trident2_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS], rval, wrr_mask;
    soc_td2_sched_mode_e mode = SOC_TD2_SCHED_MODE_UNKNOWN;
    soc_mem_t mem;
    soc_reg_t reg;
    int parent_idx  = 0;
    int  idx, fval;    

    reg = INVALIDr;
    mem = INVALIDm;
    sched_type = _soc_trident2_port_sched_type_get(unit, port);

    if (sched_type == SOC_TD2_SCHED_HSP) {
        /* get the port relative index / offset */
        if (level == SOC_TD2_NODE_LVL_L0) {
            index = index % 5;
            reg = HSP_SCHED_PORT_CONFIGr;
            parent_idx = 0;
        } else if (level == SOC_TD2_NODE_LVL_L1) {
            index = index % 10;
            reg = HSP_SCHED_L0_NODE_CONFIGr;
            /* Find the Parent index for the current child */
            for (idx = 1; idx < 5; idx++) {
                SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                                    unit, port, idx, &rval));
                fval = soc_reg_field_get(unit,
                            HSP_SCHED_L0_NODE_CONNECTION_CONFIGr,
                            rval, CHILDREN_CONNECTION_MAPf);
                if (idx == 4) {
                    if ((index >= 8) && (fval & (1 << (index - 2)))) {
                        parent_idx = idx;
                        break;
                    }
                } else {
                    if (fval & (1 << index)) {
                        parent_idx = idx;
                        break;
                    }
                }
            }
        } else if (level == SOC_TD2_NODE_LVL_L2) {
            uint32 rval = 0;
            uint32 mc_group_mode = 0;
            SOC_IF_ERROR_RETURN(
                READ_HSP_SCHED_PORT_CONFIGr(unit, port, &rval));
            mc_group_mode  = soc_reg_field_get(unit, 
                                               HSP_SCHED_PORT_CONFIGr,
                                               rval, MC_GROUP_MODEf);
            reg = HSP_SCHED_L1_NODE_CONFIGr;
            parent_idx = index % 10;

            if (mc_group_mode && (index >= 1480)) {
                if ((index % 10) < _TD2_HSP_PORT_MAX_COS) {
                    reg= HSP_SCHED_L0_NODE_CONFIGr;
                    parent_idx = 0;
                }
            }
        } else {
            return SOC_E_PARAM;
        }

        /* selection between SP and WxRR is based on weight property. */
        SOC_IF_ERROR_RETURN(soc_td2_sched_weight_get(unit, port, 
                                                level, index, weight));
        if (*weight == 0) {
            mode = SOC_TD2_SCHED_MODE_STRICT;
        } else {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            wrr_mask = soc_reg_field_get(unit, reg, rval, ENABLE_WRRf);
            if (wrr_mask & (1 << parent_idx)) {
                mode = SOC_TD2_SCHED_MODE_WRR;
            } else {
                mode = SOC_TD2_SCHED_MODE_WDRR;
            }
        }
    } else {
        SOC_IF_ERROR_RETURN(soc_td2_sched_weight_get(unit, port, 
                                                level, index, weight));

        if (*weight == 0) {
            mode = SOC_TD2_SCHED_MODE_STRICT;
        } else {
            mem = _SOC_TD2_NODE_CONFIG_MEM(unit, port, SOC_TD2_NODE_LVL_ROOT);
            index = _soc_trident2_root_scheduler_index(unit, port);
            SOC_IF_ERROR_RETURN(
                    soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry));
            if (soc_mem_field32_get(unit, mem, 
                                entry, PACKET_MODE_WRR_ACCOUNTING_ENABLEf)) {
                mode = SOC_TD2_SCHED_MODE_WRR;
            } else {
                mode = SOC_TD2_SCHED_MODE_WDRR;
            }
        }
    }

    if (pmode) {
        *pmode = mode;
    }

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Port:%d L%s%d config : index=%d MODE=%d WT=%d\n"),
              port, (level == 0) ? "r" : "", level - 1, index, mode, *weight));

    return SOC_E_NONE;
}

int
soc_td2_cosq_get_sched_config(int unit, soc_port_t port,
                              int level, int index, int child_index,
                              int *pnum_spri, int *pfirst_child, 
                              int *pfirst_mc_child, uint32 *pucmap, uint32 *pspmap,
                              soc_td2_sched_mode_e *pmode, int *weight)
{
    int child_level;

    if (level >= SOC_TD2_NODE_LVL_L2) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(soc_td2_cosq_get_sched_child_config(unit, port, level,
                        index, pnum_spri, pfirst_child, pfirst_mc_child, pucmap,
                        pspmap));

    if (child_index >= 0) {
        soc_td2_get_child_type(unit, port, level, &child_level);
        
        SOC_IF_ERROR_RETURN(
                soc_td2_cosq_get_sched_mode(unit, port, child_level, child_index,
                                            pmode, weight));
    }
    
    return SOC_E_NONE;
}

int soc_td2_get_child_type(int unit, soc_port_t port, int level, 
                                    int *child_type)
{
    *child_type = -1;

    if (level == SOC_TD2_NODE_LVL_ROOT) {
        *child_type = SOC_TD2_NODE_LVL_L0;
    } else if (level == SOC_TD2_NODE_LVL_L0) {
        *child_type = SOC_TD2_NODE_LVL_L1;
    } else if (level == SOC_TD2_NODE_LVL_L1) {
        *child_type = SOC_TD2_NODE_LVL_L2;
    }
    return SOC_E_NONE;
}

typedef struct _soc_td2_lls_config_s {
    int    level;
    int    node_id;
    int    num_children;
    int    sched_mode;
    int    weights[48];
    uint32 uc_mc_map;
} _soc_td2_lls_config_t;

/* CPU lls tree is dynamically generated */
static _soc_td2_lls_config_t *_td2_cpu_port_lls_config[SOC_MAX_NUM_DEVICES];

static _soc_td2_lls_config_t _td2_port_lls_config[] = {
    /* SOC_TD2_NODE_LVL_ROOT */
    { SOC_TD2_NODE_LVL_ROOT, 0,
      1, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TD2_NODE_LVL_L0 L0.0 */
    { SOC_TD2_NODE_LVL_L0, 0,
      10, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TD2_NODE_LVL_L1 L1.0 */
    { SOC_TD2_NODE_LVL_L1, 0,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.1 */
    { SOC_TD2_NODE_LVL_L1, 1,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.2 */
    { SOC_TD2_NODE_LVL_L1, 2,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.3 */
    { SOC_TD2_NODE_LVL_L1, 3,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.4 */
    { SOC_TD2_NODE_LVL_L1, 4,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.5 */
    { SOC_TD2_NODE_LVL_L1, 5,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.6 */
    { SOC_TD2_NODE_LVL_L1, 6,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.7 */
    { SOC_TD2_NODE_LVL_L1, 7,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.8 */
    { SOC_TD2_NODE_LVL_L1, 8,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.7 */
    { SOC_TD2_NODE_LVL_L1, 9,
      2, SOC_TD2_SCHED_MODE_WRR, 
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0x1 /* 1 UC nodes, 1 MC node */},
    /* SOC_TD2_NODE_LVL_L2 */
    { -1, 0, 0, -1,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      1 }
};

static _soc_td2_lls_config_t _td2_lb_port_lls_config[] = {
    /* SOC_TD2_NODE_LVL_ROOT */
    { SOC_TD2_NODE_LVL_ROOT, 0,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TD2_NODE_LVL_L0 L0.0 */
    { SOC_TD2_NODE_LVL_L0, 0,
      9, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0},
    /* SOC_TD2_NODE_LVL_L1 L1.0 */
    { SOC_TD2_NODE_LVL_L1, 0,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.1 */
    { SOC_TD2_NODE_LVL_L1, 1,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.2 */
    { SOC_TD2_NODE_LVL_L1, 2,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.3 */
    { SOC_TD2_NODE_LVL_L1, 3,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.4 */
    { SOC_TD2_NODE_LVL_L1, 4,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.5 */
    { SOC_TD2_NODE_LVL_L1, 5,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.6 */
    { SOC_TD2_NODE_LVL_L1, 6,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.7 */
    { SOC_TD2_NODE_LVL_L1, 7,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 MC node */},
    /* SOC_TD2_NODE_LVL_L1 L1.8 */
    { SOC_TD2_NODE_LVL_L1, 8,
      1, SOC_TD2_SCHED_MODE_WRR,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 /* 1 QCN node */},
    /* SOC_TD2_NODE_LVL_L2 */
    { -1, 0, 0, -1,
      { 1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1 },
      0 }
};

STATIC _soc_td2_lls_config_t *
_soc_td2_get_config_for_level(_soc_td2_lls_config_t *cfg_tbl, int lvl, 
                              int offset)
{
    _soc_td2_lls_config_t *en = cfg_tbl;

    while (en->level != -1) {
        if ((en->level == lvl) && (en->node_id == offset)) {
            return en;
        }
        en++;
    }

    return NULL;
}

STATIC int
_soc_td2_get_sched_count(int unit, soc_port_t port, 
                         _soc_td2_lls_config_t *cfgtbl, int lvl)
{
    int cnt = 0;

    while (_soc_td2_get_config_for_level(cfgtbl, lvl, cnt)) {
        cnt++;
    }
    return cnt;
}

STATIC int
_soc_td2_alloc_sched(int unit, soc_port_t in_port, soc_td2_node_lvl_e lvl, 
                     int offset, int *hw_index)
{

    int port, in_pipe, port_in_pipe, tlm = -1, cpu_tlm;
    int mmu_port, empty, base, max = -1, jj;
    soc_trident2_sched_type_t stype;
    uint32  sched_bmap[32];
    int base_hsp = 0;

    COMPILER_REFERENCE(base_hsp);
    
    sal_memset(sched_bmap, 0, sizeof(uint32)*32);

    SOC_IF_ERROR_RETURN(
        soc_td2_port_common_attributes_get(unit, in_port, &in_pipe, &mmu_port, NULL));

    stype = _soc_trident2_port_sched_type_get(unit, in_port);

    if (lvl == SOC_TD2_NODE_LVL_ROOT) {
        tlm = 1;
        max = 106;
    } else if (lvl == SOC_TD2_NODE_LVL_L0) {
        tlm = 5; /* max 4 L0 under HSP */
        max = 265;
    } else if (lvl == SOC_TD2_NODE_LVL_L1) {
        tlm = 10;
        max = 1024;
    }

    if ((max < 0) || (tlm < 0)) {
        return SOC_E_PARAM;
    }

    base = 0;
    cpu_tlm = _soc_td2_get_sched_count(unit, in_port, _td2_cpu_port_lls_config[unit], lvl);
    if (lvl == SOC_TD2_NODE_LVL_L0) {
        if (cpu_tlm < SOC_TD2_COSQ_NUM_CPU_L0_NODES(unit)) {
            cpu_tlm = SOC_TD2_COSQ_NUM_CPU_L0_NODES(unit);
        }
    }

    if (stype == SOC_TD2_SCHED_HSP) {
        /* For HSP ports, the indexes of the scheduler and queues are fixed.*/
        if (offset >= tlm) {
            return SOC_E_PARAM;
        }
        mmu_port += (mmu_port >= 64) ? -64 : 0;
        *hw_index = (mmu_port * tlm) + offset;
        return SOC_E_NONE;
    } else {
        /* reserve all the scheduler and queus that are prefixed for hsp. */
        PBMP_ALL_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(soc_td2_port_common_attributes_get(unit, port, 
                                        &port_in_pipe, &mmu_port, NULL));
            if (in_pipe != port_in_pipe) {
                continue;
            }

            if (_soc_trident2_port_sched_type_get(unit, port) == SOC_TD2_SCHED_HSP) {
                mmu_port += (mmu_port >= 64) ? -64 : 0;
                base = mmu_port * tlm;
                for (jj = 0; jj < tlm; jj++) {
                    sched_bmap[(base + jj)/32] |= 1 << ((base + jj) % 32);
                }
                if (base + tlm > base_hsp) {
                    base_hsp = base + tlm;
                }
            }
        }
        
        if (lvl == SOC_TD2_NODE_LVL_L0) {
            base = SOC_TD2_COSQ_CPU_RESERVED_L0_BASE(unit);
            if (IS_CPU_PORT(unit, in_port)) {
                if (offset >= cpu_tlm) {
                    return SOC_E_PARAM;
                }
                base += offset;
                sched_bmap[base/32] |= 1 << (base % 32);
                *hw_index = base;
                return SOC_E_NONE;
            } else {
                /* reserve the CPU L0 nodes */
                for (jj = 0; jj < cpu_tlm; jj++) {
                    sched_bmap[(base + jj)/32] |= 1 << ((base + jj) % 32);
                }
            }
            base = INVALID_PARENT(unit, lvl);
            if (base == 0) {
                base = 53;
            }
            sched_bmap[base/32] |= (1 << (base % 32));
        } else {
            base = INVALID_PARENT(unit, lvl);
            sched_bmap[base/32] |= (1 << (base % 32));
        }

        cpu_tlm = (cpu_tlm + 3) & ~3;

        base = 0;
        PBMP_ALL_ITER(unit, port) {
            SOC_IF_ERROR_RETURN(
                soc_td2_port_common_attributes_get(unit, port, 
                                                 &port_in_pipe, &mmu_port, NULL));
            if ((in_pipe != port_in_pipe) || 
                (_soc_trident2_port_sched_type_get(unit, port) == SOC_TD2_SCHED_HSP)) {
                continue;
            }
            
            tlm = _soc_td2_get_sched_count(unit, port, 
                ((IS_CPU_PORT(unit, port)) ? _td2_cpu_port_lls_config[unit] :
                                             _td2_port_lls_config), lvl);
            tlm = (tlm + 3) & ~3;

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
            if (SOC_IS_TD2P_TT2P(unit)) {
                if (lvl == SOC_TD2_NODE_LVL_L1) {
                    if (IS_CPU_PORT(unit, in_port)) {
                        if (offset >= cpu_tlm) {
                            return SOC_E_PARAM;
                        }
                        base = base_hsp;
                        base += offset;
                        sched_bmap[base/32] |= 1 << (base % 32);
                        *hw_index = base;
                        return SOC_E_NONE;
                    }
                } 

                mmu_port += (mmu_port >= 64) ? -64 : 0;
                if (lvl == SOC_TD2_NODE_LVL_L1) {
                    base = cpu_tlm + (mmu_port * tlm);
                } else {
                    base = mmu_port * tlm;
                }
                if (base >= max) {
                    return SOC_E_RESOURCE;
                }
                if (port == in_port) {
                    if (offset >= tlm) {
                        return SOC_E_PARAM;
                    }
                    *hw_index = base + offset;
                    return SOC_E_NONE;
                } else {
                    for(jj = 0; jj < tlm; jj++) {
                        sched_bmap[base/32] |= 1 << (base % 32);
                        base += 1;
                    }
                }
            } else
#endif
            {
                /* step to empty resources start. */
                empty = 0;
                while (base < max) {
                    if ((sched_bmap[base/32] & (1 << (base % 32))) == 0) {
                        empty += 1;
                    } else {
                        empty = 0;
                    }

                    base += 1;
                    if (empty == tlm) {
                        base -= tlm;
                        break;
                    }
                }

                if (base == max) {
                    return SOC_E_RESOURCE;
                }

                if (port == in_port) {
                    if (offset >= tlm) {
                        return SOC_E_PARAM;
                    }
                    *hw_index = base + offset;
                    return SOC_E_NONE;
                } else {
                    for(jj = 0; jj < tlm; jj++) {
                        sched_bmap[base/32] |= 1 << (base % 32);
                        base += 1;
                    }
                }
            }
        }
    }

    return SOC_E_RESOURCE;
}

int soc_td2_l2_hw_index(int unit, int qnum, int uc)
{
    if (uc) {
        qnum -= (qnum >= 1480) ? 1480 : 0;
    } else {
        qnum -= (qnum >= 568) ? 568 : 0;
        qnum += 1480;
    }
    return qnum;
}

/*
 * Function:
 *     soc_td2_hw_index_logical_num
 * Purpose:
 *     Get the logical queue number according to hw index
 * Parameters:
 *     unit          - (IN) unit number
 *     port          - (IN) local port number
 *     index         -(IN) The index of Queue management related memory Tables
 *     uc            - (IN) unicast indicator
 * Returns:
 *     logical queue number: queue number in logical view
 *     In logical view,
 *     the range of unicast queue number is:   pipeX[0,1480),pipeY[1480,2960)
 *     the range of multicast queue number is: pipeX[0,568),pipeY[568,1136)
 */
int
soc_td2_hw_index_logical_num(int unit,int port,int index,int uc)
{
    int qnum;
    uint32 pipe =  SOC_TD2_PORT_PIPE(unit, port);
    if (uc) {
        qnum = index + ( pipe ? 1480 : 0);
    } else {
        qnum =  index - 1480;
        qnum += pipe ? 568 : 0;
    }

    return qnum;
}

STATIC int
soc_td2_port_lls_init(int unit, int port, _soc_td2_lls_config_t *cfg_tbl,
                      int setup, _soc_td2_lls_traverse_cb cb, void *cookie)
{
    _soc_td2_lls_config_t *pinfo, *cinfo;
    struct _td2_sched_pending_setup_s {
        int     parent;
        int     level;
        int     offset;
        int     hw_index;
        int     l2_uc;
    } pending_list[64], *ppending;
    int  list_size, lindex, child_lvl = 0, uc;
    int  l2_lvl_offsets[2], rc, numq;
    int spri, smcpri, num_spri, qnum, self_hw_index, c;
    uint32 ucmap, rval;
    int pipe, mmu_port;
    int num_children, lvl_offsets[4];
    int sp_vec = soc_feature(unit, soc_feature_vector_based_spri);


    /* If the dynamic switchover feature is enabled, we need to enable the
       spri_vect_mode by setting spri_vect_mode_enable in LLS_CONFIG0 reg */
    if (sp_vec) {
        
        SOC_IF_ERROR_RETURN(READ_ES_PIPE0_LLS_CONFIG0r(unit, &rval));
        if (!soc_reg_field_get(unit, ES_PIPE0_LLS_CONFIG0r, rval, 
                               SPRI_VECT_MODE_ENABLEf)) {
            soc_reg_field_set(unit, ES_PIPE0_LLS_CONFIG0r, &rval, 
                              SPRI_VECT_MODE_ENABLEf, 1);
            SOC_IF_ERROR_RETURN(WRITE_ES_PIPE0_LLS_CONFIG0r(unit, rval));
            SOC_IF_ERROR_RETURN(WRITE_ES_PIPE1_LLS_CONFIG0r(unit, rval)); 
        }
    }

    l2_lvl_offsets[0] = l2_lvl_offsets[1] = 0;
    lvl_offsets[0] = lvl_offsets[1] = lvl_offsets[2] = lvl_offsets[3] = 0;

    SOC_IF_ERROR_RETURN(
        soc_td2_port_common_attributes_get(unit, port, &pipe, &mmu_port, NULL));

    /* setup port scheduler */
    pending_list[0].parent = -1;
    pending_list[0].level = SOC_TD2_NODE_LVL_ROOT;
    pending_list[0].offset = 0;
    pending_list[0].hw_index = (pipe) ? mmu_port - 64 : mmu_port;

    list_size = 1;
    lindex = 0;
    do {
        ppending = &pending_list[lindex++];
        self_hw_index = ppending->hw_index;
        /* If we are just traversing the LLS heirarchy, just call the callback
           function, which is passed as an argument to this func */
        if (setup == _SOC_TD2_LLS_OP_TRAVERSE) {
            if (cb != NULL) {
                cb(unit, port, ppending->level, ppending->hw_index, cookie);
            }
        } else if (ppending->parent != -1) {            
            /* attach to parent */
            SOC_IF_ERROR_RETURN(soc_td2_cosq_set_sched_parent(unit, port, 
                                ppending->level, self_hw_index, 
            (setup) ? ppending->parent : INVALID_PARENT(unit, ppending->level)));
        }

        if (ppending->level == SOC_TD2_NODE_LVL_L2) {
            continue;
        }
        
        pinfo = _soc_td2_get_config_for_level(cfg_tbl, 
                                              ppending->level, 
                                              ppending->offset);
        if (!pinfo) {
            return SOC_E_INTERNAL;
        }
            
        soc_td2_get_child_type(unit, port, ppending->level, &child_lvl);
        ucmap = 0;
        spri = -1;
        smcpri = -1;
        num_spri = 0;

        num_children = pinfo->num_children;

        for (c = 0; c < num_children; c++) {
            pending_list[list_size].parent = self_hw_index;
            pending_list[list_size].level = child_lvl;
            pending_list[list_size].offset = lvl_offsets[child_lvl];
            lvl_offsets[child_lvl] += 1;
            if (child_lvl == SOC_TD2_NODE_LVL_L2) {
                uc = 0; /* default MC queue */
                if (!IS_CPU_PORT(unit, port) && !IS_LB_PORT(unit,port)) {
                    uc = (pinfo->uc_mc_map & (1 << c)) ? 1 : 0;
                }

                rc = soc_td2_get_def_qbase(unit, port, 
                                (uc ? _SOC_TD2_INDEX_STYLE_UCAST_QUEUE : 
                                _SOC_TD2_INDEX_STYLE_MCAST_QUEUE), &qnum, &numq);

                if (rc) {
                    return rc;
                }

                if ((numq == 0) || (qnum < 0) || l2_lvl_offsets[uc] >= numq) {
                    continue;
                }
                qnum = soc_td2_l2_hw_index(unit, qnum, uc);
                pending_list[list_size].hw_index = qnum + l2_lvl_offsets[uc];
                l2_lvl_offsets[uc]++;
                if (uc) {
                    if (spri == -1) {
                        spri = pending_list[list_size].hw_index;
                    }
                } else {
                    if (smcpri == -1) {
                        smcpri = pending_list[list_size].hw_index;
                    }
                }
                if ((pinfo->sched_mode == SOC_TD2_SCHED_MODE_STRICT) &&
                    (!IS_CPU_PORT(unit, port))) {
                    ucmap |= (uc) ? 0 : (1 << c);
                    num_spri++;
                }
                /* if we are traversing the LLS, there is no need to set
                   the scheduler parent and mode as it is to be done only 
                   during the init */
                if (setup == _SOC_TD2_LLS_OP_TRAVERSE) {
                    list_size++;
                } else {
                    SOC_IF_ERROR_RETURN(
                            soc_td2_cosq_set_sched_parent(unit, port, 
                                  SOC_TD2_NODE_LVL_L2, 
                                  pending_list[list_size].hw_index, 
                                  self_hw_index));

                    SOC_IF_ERROR_RETURN(
                        soc_td2_cosq_set_sched_mode(unit, port, SOC_TD2_NODE_LVL_L2,
                                pending_list[list_size].hw_index,
                                pinfo->sched_mode, pinfo->weights[c]));
                }

            } else {
                cinfo = _soc_td2_get_config_for_level(cfg_tbl, child_lvl, 
                                          pending_list[list_size].offset);
                if (!cinfo) {
                    return SOC_E_INTERNAL;
                }
                if (soc_td2_sched_hw_index_get(unit, port, child_lvl, 
                                            pending_list[list_size].offset, 
                                         &pending_list[list_size].hw_index)) {
                    return SOC_E_INTERNAL;
                }
                if (spri == -1) {
                    spri = pending_list[list_size].hw_index;
                }
                if (cinfo->sched_mode == SOC_TD2_SCHED_MODE_STRICT) {
                    num_spri++;
                }
                /* if we are traversing the LLS, there is no need to set
                   the scheduler parent and mode as it is to be done only 
                   during the init */
                if (setup != _SOC_TD2_LLS_OP_TRAVERSE) {
                    SOC_IF_ERROR_RETURN(
                            soc_td2_cosq_set_sched_parent(unit, port, child_lvl, 
                                pending_list[list_size].hw_index,
                                self_hw_index));

                    SOC_IF_ERROR_RETURN(
                        soc_td2_cosq_set_sched_mode(unit, port, child_lvl,
                                pending_list[list_size].hw_index,
                                cinfo->sched_mode, pinfo->weights[c]));
                }
                list_size++;
            }
        }

        if (spri == -1) {
            spri = 0;
        }
        if (smcpri == -1) {
            smcpri = 1480;
        }
        /* if we are NOT traversing the LLS, we need to set the scheduler
           configuration for the child  */
        if (setup != _SOC_TD2_LLS_OP_TRAVERSE) {
            SOC_IF_ERROR_RETURN(
                soc_td2_cosq_set_sched_child_config(unit, port, 
                                    ppending->level, self_hw_index, 
                                    num_spri, spri, smcpri, ucmap, 0));
        }
    } while (lindex < list_size);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_td2_hsp_sp_node_init
 * Purpose:
 *     This function initialises ENABLE_SP_IN_MIN setting in
 *     HSP_SCHED_Lx_NODE_CONFIG.
 *     Hardware default value for sched mode is SP. SP nodes
 *     with min bandwidth set may be scheduled in WRR manner.
 *     To schedule SP nodes with strict priority, ENABLE_SP_IN_MIN
 *     must be set.
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 * Returns:
 *     SOC_E_XXX
 */
int
soc_td2_hsp_sp_node_init(int unit, soc_port_t port)
{
    uint32 index, rval, fval;
    soc_reg_t regs[4] = {
            HSP_SCHED_L0_NODE_CONFIGr,
            HSP_SCHED_L1_NODE_CONFIGr,
            HSP_SCHED_L2_UC_QUEUE_CONFIGr,
            HSP_SCHED_L2_MC_QUEUE_CONFIGr
        };

    for (index = 0; index < COUNTOF(regs); index++) {
        if (regs[index] == HSP_SCHED_L0_NODE_CONFIGr) {
            fval = 0x1f;
        } else {
            fval = 0x3ff;
        }
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, regs[index], port, 0, &rval));
        soc_reg_field_set(unit, regs[index], &rval, ENABLE_SP_IN_MINf, fval);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, regs[index], port, 0, rval));
    }

    return SOC_E_NONE;
}

#define TD2_DFL_HSP_SCHED_MODE  SOC_TD2_SCHED_MODE_WDRR

STATIC int
soc_td2_setup_hsp_port(int unit, int port)
{
    uint32 rval, fval = 0, pbmf;
    int mmu_port, pipe, l0_index, l1_index, l0_1, l0_4, hw_index;
    int hsp_port_index;

    SOC_IF_ERROR_RETURN(
        soc_td2_port_common_attributes_get(unit, port, &pipe, &mmu_port, NULL));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_PORT_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L0_NODE_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L1_NODE_CONFIGr(unit, port, rval));
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_L2_UC_QUEUE_CONFIGr(unit, port, rval));

    SOC_IF_ERROR_RETURN(soc_td2_hsp_sp_node_init(unit, port));

    l0_1 = 0;
    l0_4 = 0;
    for (l0_index = 0; l0_index < 5; l0_index++) {
        SOC_IF_ERROR_RETURN(soc_td2_sched_hw_index_get(unit, port, 
                                    SOC_TD2_NODE_LVL_L0, l0_index, &hw_index));
        SOC_IF_ERROR_RETURN(soc_td2_cosq_set_sched_parent(unit, port,
                              SOC_TD2_NODE_LVL_L0, hw_index, mmu_port));

        if (l0_index == 1) {
            l0_1 = hw_index;
        } else if (l0_index == 4) {
            l0_4 = hw_index;
        }
    }

    for (l1_index = 0; l1_index < 10; l1_index++) {
        SOC_IF_ERROR_RETURN(soc_td2_sched_hw_index_get(unit, port, 
                                    SOC_TD2_NODE_LVL_L1, l1_index, &hw_index));

        SOC_IF_ERROR_RETURN(soc_td2_cosq_set_sched_parent(unit, port,
                   SOC_TD2_NODE_LVL_L1, hw_index, (l1_index < 8) ? l0_1 : l0_4));

        SOC_IF_ERROR_RETURN(soc_td2_cosq_set_sched_mode(unit, port, 
                 SOC_TD2_NODE_LVL_L1, l1_index, SOC_TD2_SCHED_MODE_WDRR, 1));
    }

    pbmf = pipe ? IS_HSP_PORT_IN_YPIPEf : IS_HSP_PORT_IN_XPIPEf;
    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_GLOBAL_CONFIGr(unit, &rval));
    fval = soc_reg_field_get(unit, HSP_SCHED_GLOBAL_CONFIGr, rval, pbmf);
    hsp_port_index = mmu_port - pipe * _TD2_PORTS_PER_PIPE;
    fval |= (1 << hsp_port_index);
    soc_reg_field_set(unit, HSP_SCHED_GLOBAL_CONFIGr, &rval, pbmf, fval);
    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_GLOBAL_CONFIGr(unit, rval));
    return SOC_E_NONE;
}

STATIC int
soc_td2_reset_hsp_port(int unit, int port)
{
    int l1_index, hw_index;

    for (l1_index = 0; l1_index < _TD2_HSP_PORT_MAX_L1; l1_index++) {
        SOC_IF_ERROR_RETURN(
            soc_td2_sched_hw_index_get(unit, port, SOC_TD2_NODE_LVL_L1, 
                                       l1_index, &hw_index));

        SOC_IF_ERROR_RETURN(
            soc_td2_cosq_set_sched_parent(
                unit, port, SOC_TD2_NODE_LVL_L1, hw_index, 
                INVALID_PARENT(unit, SOC_TD2_NODE_LVL_L1)));
    }

    return SOC_E_NONE;
}

STATIC int
soc_td2_cpu_port_lls_init(int unit, int port, int setup, int reserve,
                          _soc_td2_lls_traverse_cb cb, void *cookie)

{
    int num_l0, num_l1, l0_0, l0_1, l0_2, num_nodes, w, ii, jj;
    int l1_cnt[3], l2_cnt[3], nc, cmc;
    _soc_td2_lls_config_t *lnc, *pnode;
    int max_q = 48, l1off;

    if (_td2_cpu_port_lls_config[unit] != NULL) {
        sal_free(_td2_cpu_port_lls_config[unit]);
        _td2_cpu_port_lls_config[unit] = NULL;
    }

    max_q = (reserve==0) ? 48 : 48 - SOC_TD2_COSQ_CPU_RESERVED_L0_NUM(unit);

    /* alloc structure to hold lls node data, init it and kick off 
     * soc_td2_port_lls_init */
    
    l2_cnt[1] = l0_1 = NUM_CPU_ARM_COSQ(unit, SOC_ARM_CMC(unit, 0));
    l2_cnt[2] = l0_2 = NUM_CPU_ARM_COSQ(unit, SOC_ARM_CMC(unit, 1));
    l2_cnt[0] = l0_0 = max_q - (l0_1 + l0_2);
    
    if (l0_0 <= 0) {
        return SOC_E_PARAM;
    }

    num_l0 = 1 + (l0_1 > 0 ? 1 : 0) + (l0_2 > 0 ? 1 : 0);

    /* keep a max of 8 nodes under each l1. */
    l1_cnt[0] = l1_cnt[1] = l1_cnt[2] = 0;

    l1_cnt[0] = (l0_0 + 7)/8;
    l1_cnt[1] = (l0_1 + 7)/8;
    l1_cnt[2] = (l0_2 + 7)/8;

    num_l1  = l1_cnt[0] + l1_cnt[1] + l1_cnt[2];

    num_nodes = 1 /* root */ + num_l0 + num_l1 + 1 /* null last node */;

    for (cmc = 0, ii = 0; cmc < 3; cmc++) {
        SHR_BITCLR_RANGE(CPU_ARM_QUEUE_BITMAP(unit, cmc), 0, 48);
        SHR_BITCLR_RANGE(CPU_ARM_RSVD_QUEUE_BITMAP(unit, cmc), 0, 48);
        for (jj = 0; jj < l2_cnt[cmc]; jj++, ii++) {
            SHR_BITSET(CPU_ARM_QUEUE_BITMAP(unit, cmc), ii);
        }
        NUM_CPU_ARM_COSQ(unit, cmc) = l2_cnt[cmc];
    }

    if (SOC_IS_TD2P_TT2P(unit)) {
        /* TBD COSQ. The following should be chaneged after full solution for new queues */
        /* CPU  is L0.0 - bit 58 */
        /* ARM0 is L0.1 - bit 59 */
        /* ARM1 is L0.2 - bit 60 */
        SHR_BITSET(CPU_ARM_RSVD_QUEUE_BITMAP(unit, 0), 58);
        if (l2_cnt[1]) {
            SHR_BITSET(CPU_ARM_RSVD_QUEUE_BITMAP(unit, 1), 59);
        }
        if (l2_cnt[2]) {
            SHR_BITSET(CPU_ARM_RSVD_QUEUE_BITMAP(unit, 2), 60);
        }
    } else if (reserve) {
        for (cmc = 0; cmc < 3; cmc++) {
            if (l1_cnt[cmc] > 0) {
                SHR_BITSET(CPU_ARM_RSVD_QUEUE_BITMAP(unit, cmc), (max_q+cmc));
            }
        }
    }

    lnc = sal_alloc(sizeof(_soc_td2_lls_config_t)*num_nodes, "CPU LLS config");
    if (lnc == NULL) {
        /* No rollback needed */
        return SOC_E_MEMORY;
    }

    /* init root */
    pnode = lnc;
    pnode->level = SOC_TD2_NODE_LVL_ROOT;
    pnode->node_id = 0;
    pnode->sched_mode = SOC_TD2_SCHED_MODE_WRR;
    pnode->num_children = num_l0;
    pnode->uc_mc_map = 0;
    for (w = 0; w < 48; w++) {
        pnode->weights[w] = 1;
    }
    
    pnode++;
    l1off = 0;

    for (ii = 0; ii < num_l0; ii++) {
        pnode->level = SOC_TD2_NODE_LVL_L0;
        pnode->node_id = ii;
        pnode->num_children = l1_cnt[ii];
        pnode->sched_mode = SOC_TD2_SCHED_MODE_WRR;
        pnode->uc_mc_map = 0;
        for (w = 0; w < 48; w++) {
            pnode->weights[w] = 1;
        }
        pnode++;

        for (nc = 0, jj = 0; jj < l1_cnt[ii]; jj++) {
            pnode->level = SOC_TD2_NODE_LVL_L1;
            pnode->node_id = l1off++;

            if ((l2_cnt[ii] - nc) >= 8) {
                pnode->num_children = 8;
            } else {
                pnode->num_children = l2_cnt[ii] - nc;
            }

            nc += pnode->num_children;

            pnode->sched_mode = SOC_TD2_SCHED_MODE_WRR;
            pnode->uc_mc_map = 0;
            for (w = 0; w < 48; w++) {
                pnode->weights[w] = 1;
            }
            pnode++;
        }
    }

    /* null node */
    pnode->level = -1;
    pnode->node_id = -1;
    pnode->num_children = 0;
    pnode->uc_mc_map = 0;

    _td2_cpu_port_lls_config[unit] = lnc;

    SOC_IF_ERROR_RETURN(soc_td2_port_lls_init(unit, port, 
                                              _td2_cpu_port_lls_config[unit], setup,
                                              cb, cookie));
    return 0;
}

int soc_td2_lls_reset(int unit)
{
    int port, lvl, i;
    uint32 clrmap = 0, pipe;
    soc_info_t *si;
    int phy_port, mmu_port, index;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 *bmap = NULL;
    int alloc_size;

    si = &SOC_INFO(unit);

    PBMP_ALL_ITER(unit, port) {
        pipe = SOC_TD2_PORT_PIPE(unit, port);
        for (lvl = SOC_TD2_NODE_LVL_L0; lvl <= SOC_TD2_NODE_LVL_L2; lvl++) {
            mem = _SOC_TD2_NODE_PARENT_MEM(unit, port, lvl);
            /* coverity[negative_returns : FALSE] */
            alloc_size = SHR_BITALLOCSIZE(soc_mem_index_count(unit, mem));
            if (lvl == SOC_TD2_NODE_LVL_L0) {
                if (SOC_CONTROL(unit)->port_lls_l0_bmap[port] == NULL) {
                    SOC_CONTROL(unit)->port_lls_l0_bmap[port] = 
                                        sal_alloc(alloc_size, "LLS_L0_BMAP");
                }
                bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
            } else if (lvl == SOC_TD2_NODE_LVL_L1) {
                if (SOC_CONTROL(unit)->port_lls_l1_bmap[port] == NULL) {
                    SOC_CONTROL(unit)->port_lls_l1_bmap[port] = 
                                        sal_alloc(alloc_size, "LLS_L1_BMAP");
                }
                bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
            } else if (lvl == SOC_TD2_NODE_LVL_L2) {
                if (SOC_CONTROL(unit)->port_lls_l2_bmap[port] == NULL) {
                    SOC_CONTROL(unit)->port_lls_l2_bmap[port] = 
                                        sal_alloc(alloc_size, "LLS_L2_BMAP");
                }
                bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
            }
        
            sal_memset(bmap, 0, alloc_size);
            
            if ((clrmap & (1 << (pipe * SOC_TD2_NODE_LVL_MAX + lvl))) == 0) {
                sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
                soc_mem_field32_set(unit, mem, entry, C_PARENTf, INVALID_PARENT(unit, lvl));

                for (i = 0; i <= soc_mem_index_max(unit, mem); i++) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &entry));
                }
                clrmap |= (1 << (pipe * SOC_TD2_NODE_LVL_MAX + lvl));
            }
        }

        if (SOC_PBMP_MEMBER(si->eq_pbm, port)) { /* VBS (HSP) port */
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
            for (index = 0; index < 5; index++) {
                SHR_BITSET(SOC_CONTROL(unit)->port_lls_l0_bmap[port],
                           (mmu_port & 0x3f) * 5 + index);
            }
            for (index = 0; index < 10; index++) {
                SHR_BITSET(SOC_CONTROL(unit)->port_lls_l1_bmap[port],
                           (mmu_port & 0x3f) * 10 + index);
            }
            for (index = 0; index < 10; index++) {
                SHR_BITSET(SOC_CONTROL(unit)->port_lls_l2_bmap[port],
                           (mmu_port & 0x3f) * 10 + index);
            }
        }
    }

    return 0;
}

int soc_td2_lls_reset_port(int unit, int port)
{
    int lvl;
    soc_mem_t mem;
    uint32 *bmap = NULL;
    int alloc_size;

    for (lvl = SOC_TD2_NODE_LVL_L0; lvl <= SOC_TD2_NODE_LVL_L2; lvl++) {
        mem = _SOC_TD2_NODE_PARENT_MEM(unit, port, lvl);
        alloc_size = SHR_BITALLOCSIZE(soc_mem_index_count(unit, mem));
        if (lvl == SOC_TD2_NODE_LVL_L0) {
            if (SOC_CONTROL(unit)->port_lls_l0_bmap[port] == NULL) {
                SOC_CONTROL(unit)->port_lls_l0_bmap[port] = 
                                    sal_alloc(alloc_size, "LLS_L0_BMAP");
            }
            bmap = SOC_CONTROL(unit)->port_lls_l0_bmap[port];
        } else if (lvl == SOC_TD2_NODE_LVL_L1) {
            if (SOC_CONTROL(unit)->port_lls_l1_bmap[port] == NULL) {
                SOC_CONTROL(unit)->port_lls_l1_bmap[port] = 
                                    sal_alloc(alloc_size, "LLS_L1_BMAP");
            }
            bmap = SOC_CONTROL(unit)->port_lls_l1_bmap[port];
        } else if (lvl == SOC_TD2_NODE_LVL_L2) {
            if (SOC_CONTROL(unit)->port_lls_l2_bmap[port] == NULL) {
                SOC_CONTROL(unit)->port_lls_l2_bmap[port] = 
                                    sal_alloc(alloc_size, "LLS_L2_BMAP");
            }
            bmap = SOC_CONTROL(unit)->port_lls_l2_bmap[port];
        }
    
        sal_memset(bmap, 0, alloc_size);
    }

    return 0;
}

/*
 * Function:
 *     _soc_td2plus_lls_init
 * Purpose:
 *     This function initialises the structure _td2p_dyn_sched_unit_data, which is
 *     used later to allocate the register available from  the set of 
 *     SP_WERR_DYN_CHANGEnA/nB/nC (n=0,1,2,3) to the port and  to keep track 
 *     of the used registers from the set for the port 
 * Parameters:
 *     unit       - (IN) unit number
 * Returns:
 *     SOC_E_XXX
 */
STATIC int _soc_td2plus_lls_init(int unit)
{
    _soc_td2p_port_dyn_sched_t *pcb = NULL;
    int i;
    
    if (soc_feature(unit, soc_feature_vector_based_spri)) {
        pcb = &_td2p_dyn_sched_unit_data[unit];
        
        if (pcb->_soc_td2plus_init_done[unit] == 1) {
            return SOC_E_NONE;
        }

        pcb->lock = sal_mutex_create("td2plus_dyn_lock");
        for (i = 0; i < _SOC_TD2PLUS_DYN_REGISTER_SET; i++) {
            pcb->port[i] = -1;
        }
        pcb->_soc_td2plus_init_done[unit] = 1;
    }

    return SOC_E_NONE;
}

int soc_td2_lls_init(int unit)
{
    int port, rv = SOC_E_NONE, reserve_queues, pipe;
    soc_trident2_sched_type_t sched_type;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 rval;
    soc_reg_t reg;
    _soc_td2p_lls_op_mode_t op_mode;

    static const soc_reg_t llfc_cfgr[] = {
        ES_PIPE0_LLS_FC_CONFIGr, ES_PIPE1_LLS_FC_CONFIGr };

    reserve_queues = soc_feature(unit, soc_feature_cmic_reserved_queues);

    SOC_IF_ERROR_RETURN(_soc_td2plus_lls_init(unit));

    /* do dummy read from L0 parent mem per TD2-3313. */
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ES_PIPE0_LLS_L0_PARENTm, MEM_BLOCK_ALL, 0, &entry));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ES_PIPE1_LLS_L0_PARENTm, MEM_BLOCK_ALL, 0, &entry));

    SOC_IF_ERROR_RETURN(soc_td2_init_invalid_pointers(unit));

    SOC_IF_ERROR_RETURN(WRITE_HSP_SCHED_GLOBAL_CONFIGr(unit, 0));

    SOC_IF_ERROR_RETURN(soc_td2_lls_reset(unit));

    PBMP_ALL_ITER(unit, port) {
        sched_type = _soc_trident2_port_sched_type_get(unit, port);

        if (sched_type == SOC_TD2_SCHED_HSP) {
            rv = soc_td2_setup_hsp_port(unit, port);
            if (soc_td2_is_skip_default_lls_creation(unit)) {
                SOC_IF_ERROR_RETURN(
                    soc_td2_port_mmu_tx_enable_set(unit, port, 1));
            }
        } else if (IS_CPU_PORT(unit, port)) {
            op_mode = _SOC_TD2_LLS_OP_SETUP;
            rv = soc_td2_cpu_port_lls_init(unit, port, op_mode, reserve_queues, NULL, NULL);
        } else if (IS_LB_PORT(unit, port)) {
            if (!soc_td2_is_skip_default_lls_creation(unit)) {
                op_mode = _SOC_TD2_LLS_OP_SETUP;
                rv = soc_td2_port_lls_init(unit, port, _td2_lb_port_lls_config, op_mode, NULL, NULL);
            }
        } else {
            if (!soc_td2_is_skip_default_lls_creation(unit)) {
                op_mode = _SOC_TD2_LLS_OP_SETUP;
                rv = soc_td2_port_lls_init(unit, port, _td2_port_lls_config, op_mode, NULL, NULL);
            } else {
                SOC_IF_ERROR_RETURN(
                    soc_td2_port_mmu_tx_enable_set(unit, port, 0));
                SOC_IF_ERROR_RETURN(
                    soc_td2_mmu_rx_enable_set(unit, port, 0));
            }
        }
        if (rv) {
            return SOC_E_INTERNAL;
        }
    }

    for (pipe = _TD2_XPIPE; pipe <= _TD2_YPIPE; pipe++) {
        reg = llfc_cfgr[pipe];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, FC_CFG_DISABLE_XOFFf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }

    return SOC_E_NONE;
}

int soc_td2_lls_port_uninit(int unit, soc_port_t port)
{
    soc_trident2_sched_type_t sched_type;
    int rv = SOC_E_NONE;
    _soc_td2p_lls_op_mode_t op_mode;

    sched_type = _soc_trident2_port_sched_type_get(unit, port);

    if (sched_type == SOC_TD2_SCHED_LLS) {
        if (IS_CPU_PORT(unit, port)) {
            op_mode = _SOC_TD2_LLS_OP_TEAR;
            rv = soc_td2_cpu_port_lls_init(unit, port, op_mode, 0, NULL, NULL);
        } else if (IS_LB_PORT(unit, port)) {
            if (!soc_td2_is_skip_default_lls_creation(unit)) {
                op_mode = _SOC_TD2_LLS_OP_TEAR;
                rv = soc_td2_port_lls_init(unit, port, _td2_lb_port_lls_config, op_mode, NULL, NULL);
            }
        } else {
            if (!soc_td2_is_skip_default_lls_creation(unit)) {
                op_mode = _SOC_TD2_LLS_OP_TEAR;
                rv = soc_td2_port_lls_init(unit, port, _td2_port_lls_config, op_mode, NULL, NULL);
            }
        }
    } else {
        rv = soc_td2_reset_hsp_port(unit, port);
    }
    return rv;
}

/*
 * Function:
 *     soc_td2_port_lls_traverse
 * Purpose:
 *     This function initialises the structure _td2p_dyn_sched_unit_data, which is
 *     used later to allocate the register available from  the set of 
 *     SP_WERR_DYN_CHANGEnA/nB/nC (n=0,1,2,3) to the port and  to keep track 
 *     of the used registers from the set for the port 
 * Parameters:
 *     unit       - (IN) unit number
 *     port       - (IN) local port number
 *     cb         - (IN) callback function / function pointer
 *     cookie     - (OUT) previous state of LLS 
 * Returns:
 *     SOC_E_XXX
 */
int soc_td2_port_lls_traverse(int unit, soc_port_t port, 
                              _soc_td2_lls_traverse_cb cb, void *cookie)
{
    int rv = SOC_E_INTERNAL;
    _soc_td2p_lls_op_mode_t op_mode;
 
    if (_SOC_TD2_IS_HSP_PORT(unit, port)) {
        rv = SOC_E_UNAVAIL;
    } else if (IS_CPU_PORT(unit, port)) {
        if (soc_feature(unit, soc_feature_cmic_reserved_queues)) {
            op_mode = _SOC_TD2_LLS_OP_TRAVERSE;
            rv = soc_td2_cpu_port_lls_init(unit, port, op_mode,
                                           1, cb, cookie);
        } else {
            op_mode = _SOC_TD2_LLS_OP_TRAVERSE;
            rv = soc_td2_cpu_port_lls_init(unit, port, op_mode,
                                           0, cb, cookie);
        }
    } else {
        op_mode = _SOC_TD2_LLS_OP_TRAVERSE;
        rv = soc_td2_port_lls_init(unit, port, _td2_port_lls_config, 
                                    op_mode, cb, cookie);
    }
     return rv;
}

int
soc_td2_sched_hw_index_get(int unit, int port, int lvl, int offset, 
                            int *hw_index)
{
    int rv;

    rv = _soc_td2_alloc_sched(unit, port, lvl, offset, hw_index);

    LOG_INFO(BSL_LS_SOC_COSQ,
             (BSL_META_U(unit,
                         "Alloced : port=%d lvl=%d ofst=%d Index=%d\n"),
              port, lvl, offset, *hw_index));
    return rv;
}

int
soc_td2_qcn_counter_hw_index_get(int unit, soc_port_t port,
                                 int index, int *qindex)
{
    soc_mem_t mem;
    mmu_qcn_enable_entry_t entry;
    int resolved_index = -1;    
    
    if (qindex == NULL) {
        return SOC_E_PARAM;
    }

    mem = SOC_TD2_PMEM(unit, port, MMU_QCN_ENABLE_0m, MMU_QCN_ENABLE_1m);    
    
    resolved_index = soc_td2_l2_hw_index(unit, index, 1);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ANY, resolved_index, &entry));
    if (soc_mem_field32_get(unit, mem, &entry, CPQ_ENf)) {
        *qindex = soc_mem_field32_get(unit, mem, &entry, CPQ_IDf);
    } else {
        *qindex = -1;
    }

    return SOC_E_NONE;
}

#define MMU_LB_PORT     116
#define MMU_CMIC_PORT   52

int 
soc_td2_get_def_qbase(int unit, soc_port_t inport, int qtype, 
                       int *pbase, int *pnumq)
{
    soc_info_t *si;
    int base, numq;

    si = &SOC_INFO(unit);

    if (qtype == _SOC_TD2_INDEX_STYLE_UCAST_QUEUE) {
        base = si->port_uc_cosq_base[inport];
        numq = si->port_num_uc_cosq[inport];
    } else if (qtype == _SOC_TD2_INDEX_STYLE_MCAST_QUEUE) {
        base = si->port_cosq_base[inport];
        numq = si->port_num_cosq[inport];
    } else {
        return SOC_E_INTERNAL;
    }

    if (pbase) {
        *pbase = base;
    }
    if (pnumq) {
        *pnumq = numq;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_td2_dump_hsp_sched_at(int unit, int port, int level, int offset, int index)
{
    uint32     rval = 0;
    uint32     conn_map;
    uint32     l0_index, l1_index;
    uint32     mc_group_mode = 0;
    char       *lvl_name[] = { "Root", "L0", "L1", "UC", "MC" };
    char       *sched_modes[] = {"X", "SP", "WRR", "WDRR"};
    uint32     num_spri =0;
    uint32     first_child =0;
    uint32     first_mc_child =0;
    uint32     ucmap =0;
    uint32     sched_mode =0;
    soc_info_t *si;
    int        uc_cosq, mc_cosq;
    int        uc_cosq_base, mc_cosq_base;
    int        uc_hw_index, mc_hw_index;
    int        hw_index;
    int        wt =0;

    si = &SOC_INFO(unit);

    uc_cosq_base = si->port_uc_cosq_base[port];
    mc_cosq_base = si->port_cosq_base[port];
    uc_cosq = soc_td2_logical_qnum_hw_qnum(unit, port, uc_cosq_base, 1);
    mc_cosq = soc_td2_logical_qnum_hw_qnum(unit, port, mc_cosq_base, 0);

    if (level != SOC_TD2_NODE_LVL_ROOT) {
        return SOC_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(READ_HSP_SCHED_PORT_CONFIGr(unit, port, &rval));
    mc_group_mode = soc_reg_field_get(unit, HSP_SCHED_PORT_CONFIGr,
                                      rval, MC_GROUP_MODEf);
    /* Root Node */
    LOG_CLI((BSL_META_U(unit,
                        "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
             lvl_name[level], offset, index, num_spri, first_child,
             sched_modes[sched_mode], wt));

    /* L0 Node */
    for (l0_index = 0; l0_index < 5; l0_index++) {
        /* print the L0 Nodes*/
        level = SOC_TD2_NODE_LVL_L0;
        SOC_IF_ERROR_RETURN(soc_td2_sched_hw_index_get(unit,
                         port, level, l0_index, &hw_index));
        soc_td2_cosq_get_sched_mode(unit, port, SOC_TD2_NODE_LVL_L0,
                                     hw_index, &sched_mode, &wt);
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                 lvl_name[level], l0_index, hw_index, num_spri, first_child,
                 sched_modes[sched_mode], wt));

        /* read Connection map for the  L0.1, L0.2, L0.3, L0.4
         * by default connection map should be set to all zeros for L0.0
         */

        SOC_IF_ERROR_RETURN(READ_HSP_SCHED_L0_NODE_CONNECTION_CONFIGr(
                                 unit, port, l0_index, &rval));
        conn_map = soc_reg_field_get(unit,
                            HSP_SCHED_L0_NODE_CONNECTION_CONFIGr,
                            rval, CHILDREN_CONNECTION_MAPf);
        if ((l0_index >= 1) && (l0_index <= 4)) {
            for (l1_index = 0; l1_index < 8; l1_index++) {
                if (conn_map & (1U<<l1_index)) {
                    if (l0_index == 4) {
                        offset = l1_index + 2;
                    } else {
                        offset = l1_index;
                    }
                    /* print L1 node */
                    level = SOC_TD2_NODE_LVL_L1;
                    SOC_IF_ERROR_RETURN(soc_td2_sched_hw_index_get(unit,
                        port, level, offset, &hw_index));
                    soc_td2_cosq_get_sched_mode(unit, port, SOC_TD2_NODE_LVL_L1,
                                                hw_index, &sched_mode, &wt);
                    LOG_CLI((BSL_META_U(unit,
                                        "  %s.%d : INDEX=%d NUM_SP=%d FC=%d "
                                        "FMC=%d UCMAP=0x%08x MODE=%s WT=%d\n"),
                             lvl_name[level], offset, hw_index, num_spri,
                             first_child, first_mc_child, ucmap,
                             sched_modes[sched_mode], wt));

                    /* print L2 UC node attached to L1 */
                    uc_hw_index = soc_td2_l2_hw_index(
                                        unit, uc_cosq_base + offset, 1);
                    soc_td2_cosq_get_sched_mode(unit, port,
                                                    SOC_TD2_NODE_LVL_L2,
                                                    uc_hw_index, &sched_mode, &wt);
                    LOG_CLI((BSL_META_U(unit,
                                        "    L2.uc : INDEX=%d Mode=%s WEIGHT=%d\n"),
                             uc_cosq + offset, sched_modes[sched_mode], wt));

                    if (((mc_group_mode == 1) && (l0_index == 4)) ||
                         (mc_group_mode == 0)) {
                        /* print L2 MC node attached to L1 */
                        /* MC_GROUP_MODE = 1 then  8 MC Cos queues are
                         * grouped together and shared the parent (L0.0) bandwidth.
                         * MC_QM and MC_SC queues are still paired with
                         * UC_QM and UC_SC respectivly
                         */
                        mc_hw_index = soc_td2_l2_hw_index(
                                            unit, mc_cosq_base + offset, 0);
                        soc_td2_cosq_get_sched_mode(unit, port,
                                                    SOC_TD2_NODE_LVL_L2,
                                                    mc_hw_index, &sched_mode, &wt);
                        LOG_CLI((BSL_META_U(unit,
                                            "    L2.mc : INDEX=%d Mode=%s WEIGHT=%d\n"),
                                 mc_cosq + offset, sched_modes[sched_mode], wt));
                    }
                }
            }
        } else if (l0_index == 0) {
            if (mc_group_mode) {
                offset = 0;
                for (l1_index = 0; l1_index < 8; l1_index++) {
                    /* 8 MC Cos queues are grouped together and shared
                     * the parent (L0.0) bandwidth.
                     */
                    mc_hw_index = soc_td2_l2_hw_index(unit,
                                                      mc_cosq_base + offset, 0);
                    soc_td2_cosq_get_sched_mode(unit, port,
                                                    SOC_TD2_NODE_LVL_L2,
                                                    mc_hw_index, &sched_mode, &wt);
                    LOG_CLI((BSL_META_U(unit,
                                        "    L2.mc : INDEX=%d Mode=%s WEIGHT=%d\n"),
                             mc_cosq + offset, sched_modes[sched_mode], wt));
                    offset++;
                }

            }
        }
    }

   return SOC_E_NONE;
}

STATIC int
_soc_td2_child_num_get(int unit, int port, int level, int hw_index, int *count)
{
    soc_mem_t mem;
    int cindex, index_max, ii, child_level, num_child = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];

    soc_td2_get_child_type(unit, port, level, &child_level);
    mem = _SOC_TD2_NODE_PARENT_MEM(unit, port, child_level);
    
    if (mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }
    index_max = soc_mem_index_max(unit, mem);

    for (ii = 0; ii <= index_max; ii++) {
        SOC_IF_ERROR_RETURN( 
                soc_mem_read(unit, mem, MEM_BLOCK_ALL, ii, &entry));

        cindex = soc_mem_field32_get(unit, mem, entry, C_PARENTf);

        if (cindex == hw_index) {
            num_child += 1;
        }
    }

    if (count != NULL) {
        *count = num_child;
    }

    return SOC_E_NONE;
}

STATIC int 
_soc_td2_dump_sched_at(int unit, int port, int level, int offset, int hw_index)
{
    int num_spri, first_child, first_mc_child, rv, cindex;
    uint32 ucmap, spmap;
    soc_td2_sched_mode_e sched_mode;
    soc_mem_t mem;
    int index_max, ii, ci, child_level, wt = 0, num_child, num_l1_children = 0;
    uint32 entry[SOC_MAX_MEM_WORDS];
    char *lvl_name[] = { "Root", "L0", "L1", "L2" };
    char *sched_modes[] = {"X", "SP", "WRR", "WDRR"};

    if (level < SOC_TD2_NODE_LVL_L2) {
        soc_td2_get_child_type(unit, port, level, &child_level);
        if (hw_index == INVALID_PARENT(unit, child_level)) {
            if (soc_feature(unit, soc_feature_lls_port_mema_config_match)) {
                return SOC_E_NONE;
            }

            /* 
             * INVALID_PARENT(L0) may be zero. 
             * When this condition occurs, 
             * we need to do further check instead of returning.
             */
            if (!((child_level == SOC_TD2_NODE_LVL_L0) &&
                (hw_index == 0))) {
                return SOC_E_NONE;
            }
        }
    }

    /* get sched config */
    SOC_IF_ERROR_RETURN(
            soc_td2_cosq_get_sched_child_config(unit, port, level, hw_index, 
                             &num_spri, &first_child, &first_mc_child, &ucmap,
                             &spmap));
    sched_mode = 0;
    if (level != SOC_TD2_NODE_LVL_ROOT) {
        SOC_IF_ERROR_RETURN(
          soc_td2_cosq_get_sched_mode(unit, port, level, hw_index, &sched_mode, &wt));
    }
    
    if (level == SOC_TD2_NODE_LVL_L1) {
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SP=%d FC=%d "
                            "FMC=%d UCMAP=0x%08x MODE=%s WT=%d\n"),
                 lvl_name[level], offset, hw_index, num_spri, 
                 first_child, first_mc_child, ucmap, 
                 sched_modes[sched_mode], wt));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "  %s.%d : INDEX=%d NUM_SPRI=%d FC=%d MODE=%s Wt=%d\n"),
                 lvl_name[level], offset, hw_index, num_spri, first_child,
                 sched_modes[sched_mode], wt));
    }

    soc_td2_get_child_type(unit, port, level, &child_level);
    mem = _SOC_TD2_NODE_PARENT_MEM(unit, port, child_level);
    
    if(mem == INVALIDm) {
        return SOC_E_INTERNAL;
    }
    index_max = soc_mem_index_max(unit, mem);

    num_child = 0;
    for (ii = 0, ci = 0; ii <= index_max; ii++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, ii, &entry);
        if (rv) {
            LOG_CLI((BSL_META_U(unit,
                                "Failed to read memory at index: %d\n"), ii));
            break;
        }
        
        cindex = soc_mem_field32_get(unit, mem, entry, C_PARENTf);
        if (cindex == hw_index) {
            if (!soc_feature(unit, soc_feature_lls_port_mema_config_match)) {
                /*
                 * INVALID_PARENT(L0) may be zero.
                 * When this condition occurs,
                 * we should get the node`s child count to decide
                 * if it is a valid node or not.
                 */
                if ((cindex == 0) && (child_level == SOC_TD2_NODE_LVL_L0)) {
                    _soc_td2_child_num_get(unit, port, child_level,
                                                ii, &num_l1_children);
                    if (num_l1_children == 0) {
                        continue;
                    }
                }
            }
            if (child_level == SOC_TD2_NODE_LVL_L2) {
                SOC_IF_ERROR_RETURN(soc_td2_cosq_get_sched_mode(unit, port,
                                    SOC_TD2_NODE_LVL_L2, ii, &sched_mode, &wt));
                LOG_CLI((BSL_META_U(unit,
                                    "     L2.%s INDEX=%d Mode=%s WEIGHT=%d\n"), 
                         ((ii < 1480) ? "uc" : "mc"),
                         ii, sched_modes[sched_mode], wt));
            } else {
                _soc_td2_dump_sched_at(unit, port, child_level, ci, ii);
                ci += 1;
            }
            num_child += 1;
        }
    }
    if (num_child == 0) {
        LOG_CLI((BSL_META_U(unit,
                            "*** No children \n")));
    }
    return SOC_E_NONE;
}

int soc_td2_dump_port_lls(int unit, int port)
{
    int mmu_port, index;

    if (_soc_trident2_port_sched_type_get(unit, port) == SOC_TD2_SCHED_HSP) {
        return SOC_E_NONE;
    }


    soc_td2_port_common_attributes_get(unit, port, NULL, &mmu_port, NULL);

    LOG_CLI((BSL_META_U(unit,
                        "-------%s (LLS)------\n"), SOC_PORT_NAME(unit, (port))));
    index = _soc_trident2_root_scheduler_index(unit, port);
    LOG_INFO(BSL_LS_SOC_COSQ,
            (BSL_META_U(unit, "Port:%d  mmu_port %d index %d\n"), port, mmu_port, index));
    _soc_td2_dump_sched_at(unit, port, SOC_TD2_NODE_LVL_ROOT, 0, index);
    return SOC_E_NONE;
}
int soc_td2_dump_port_hsp(int unit, int port)
{
    int mmu_port, index;

    if (_soc_trident2_port_sched_type_get(unit, port) == SOC_TD2_SCHED_LLS) {
        return SOC_E_NONE;
    }

    soc_td2_port_common_attributes_get(unit, port, NULL, &mmu_port, NULL);

    LOG_CLI((BSL_META_U(unit,
                        "-------%s (HSP)------\n"), SOC_PORT_NAME(unit, (port))));
    index = _soc_trident2_root_scheduler_index(unit, port);
    _soc_td2_dump_hsp_sched_at(unit, port, SOC_TD2_NODE_LVL_ROOT, 0, index);
    return SOC_E_NONE;
}
int soc_td2_logical_qnum_hw_qnum(int unit, int port, int lqnum, int uc)
{
    if (uc) {
	if (lqnum >= 1480) {
	    return (lqnum - 1480) + 2048;
	} else {
	    return lqnum;
	}
    } else {
	if (lqnum >= 568) {
	    return (lqnum - 568) + 3528;
	} else {
	    return lqnum + 1480;
	}
    }
    return -1;
}

int soc_td2_mmu_get_shared_size(int unit, int *shared_size)
{
    uint32 entry0[SOC_MAX_MEM_WORDS];

    sal_memset(entry0, 0, sizeof(mmu_thdu_xpipe_config_queue_entry_t));
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, MMU_THDU_XPIPE_CONFIG_QUEUEm, MEM_BLOCK_ALL, 0, entry0));
    *shared_size = soc_mem_field32_get(unit, MMU_THDU_XPIPE_CONFIG_QUEUEm, entry0, Q_SHARED_LIMIT_CELLf);
    return 1;
}

int
soc_td2_mmu_config_res_limits_update(int unit,uint32 spid,
                                     uint32 shared_size, int flags)
{
    return soc_td2_mmu_config_shared_buf_recalc(unit, spid, shared_size, flags);
}

void
soc_trident2_fc_map_shadow_free(int unit)
{
    int mem_idx;
    soc_td2_fc_map_shadow_memory_t * shadow_info;
    soc_td2_fc_map_shadow_t* shadow_ctrl = &soc_td2_fc_map_shadow[unit];
    
    if (shadow_ctrl->shadow_array != NULL) {
        for (mem_idx = 0; mem_idx < shadow_ctrl->mem_count; mem_idx++){
            shadow_info = &(shadow_ctrl->shadow_array[mem_idx]);
            if (shadow_info->mem_shadow != NULL){
                sal_free(shadow_info->mem_shadow);
            }
        }
    sal_free(shadow_ctrl->shadow_array);
    shadow_ctrl->shadow_array = NULL;
    shadow_ctrl->mem_count = 0;
    }
}

int
soc_trident2_fc_map_shadow_create(int unit)
{
	  int mem_idx, alloc_size, num_entries, entry_size;
	  soc_td2_fc_map_shadow_memory_t* shadow_info = NULL;
	  uint32* shadow = NULL;
    
    if (soc_td2_fc_map_shadow[unit].shadow_array == NULL) {
        alloc_size = sizeof(soc_td2_fc_map_shadow_memory_t) * 
                     COUNTOF(fc_map_mems);
        shadow_info = sal_alloc(alloc_size, "fc map shadow control");
        if (shadow_info == NULL) {
            return SOC_E_MEMORY;
        }
        
        sal_memset(shadow_info, 0, alloc_size);
        soc_td2_fc_map_shadow[unit].shadow_array = shadow_info;
    
        for (mem_idx = 0; mem_idx < COUNTOF(fc_map_mems); mem_idx++) {
            num_entries = soc_mem_index_count(unit, fc_map_mems[mem_idx]);
            entry_size = soc_mem_entry_words(unit, fc_map_mems[mem_idx]);
            alloc_size = entry_size * num_entries * sizeof(uint32);
            shadow = sal_alloc(alloc_size, "fc map shadow tbl");
            if (shadow == NULL) {
                soc_trident2_fc_map_shadow_free(unit);
                return SOC_E_MEMORY;
            }
            sal_memset(shadow, 0, alloc_size);
            soc_td2_fc_map_shadow[unit].shadow_array[mem_idx].mem_shadow = shadow;
            soc_td2_fc_map_shadow[unit].shadow_array[mem_idx].mem = fc_map_mems[mem_idx];
            soc_td2_fc_map_shadow[unit].mem_count++;
        }
    }
    return SOC_E_NONE; 
}

int
soc_trident2_fc_map_shadow_entry_get (int unit, soc_mem_t mem, int index, 
                                      void* entry_data)
{
    int mem_idx, entry_words;
    uint32 * shadow = NULL;
    
    switch(mem){
        case MMU_INTFI_XPIPE_FC_MAP_TBL0m:
            mem_idx = 0;
            break;
        case MMU_INTFI_XPIPE_FC_MAP_TBL1m:
            mem_idx = 1;
            break;
        case MMU_INTFI_YPIPE_FC_MAP_TBL0m:
            mem_idx = 2;
            break;
        case MMU_INTFI_YPIPE_FC_MAP_TBL1m:
            mem_idx = 3;
            break;
        default:
            return SOC_E_NOT_FOUND;
        
    }
    
    shadow = soc_td2_fc_map_shadow[unit].shadow_array[mem_idx].mem_shadow;
    entry_words = soc_mem_entry_words(unit, mem);
    sal_memcpy(entry_data, shadow + index * entry_words, 
               entry_words * sizeof(uint32));
    return SOC_E_NONE;
}

int
soc_trident2_fc_map_shadow_entry_set (int unit, soc_mem_t mem, int index, 
                                      void* entry_data)
{
    int mem_idx, entry_words;
    uint32 * shadow = NULL;
    
    switch(mem){
        case MMU_INTFI_XPIPE_FC_MAP_TBL0m:
            mem_idx = 0;
            break;
        case MMU_INTFI_XPIPE_FC_MAP_TBL1m:
            mem_idx = 1;
            break;
        case MMU_INTFI_YPIPE_FC_MAP_TBL0m:
            mem_idx = 2;
            break;
        case MMU_INTFI_YPIPE_FC_MAP_TBL1m:
            mem_idx = 3;
            break;
        default:
            return SOC_E_NOT_FOUND;
        
    }
    shadow = soc_td2_fc_map_shadow[unit].shadow_array[mem_idx].mem_shadow;
    entry_words = soc_mem_entry_words(unit, mem);
    sal_memcpy(shadow + index * entry_words, entry_data, 
               entry_words * sizeof(uint32));
    return SOC_E_NONE;
}

int 
soc_trident2_fc_map_shadow_clear (int unit, soc_mem_t mem)
{
    uint32 * shadow = NULL;
    int entry_words, tbl_size, mem_idx, num_entries;
    switch(mem){
        case MMU_INTFI_XPIPE_FC_MAP_TBL0m:
            mem_idx = 0;
            break;
        case MMU_INTFI_XPIPE_FC_MAP_TBL1m:
            mem_idx = 1;
            break;
        case MMU_INTFI_YPIPE_FC_MAP_TBL0m:
            mem_idx = 2;
            break;
        case MMU_INTFI_YPIPE_FC_MAP_TBL1m:
            mem_idx = 3;
            break;
        default:
            return SOC_E_NOT_FOUND;
    }
    entry_words = soc_mem_entry_words(unit, mem);
    shadow = soc_td2_fc_map_shadow[unit].shadow_array[mem_idx].mem_shadow;
    num_entries = soc_mem_index_count(unit, fc_map_mems[mem_idx]);
    tbl_size = entry_words * num_entries;
    if (shadow != NULL) {
        sal_memset(shadow, 0, tbl_size * sizeof(uint32));
    }
    return SOC_E_NONE;
}
#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_trident2_fc_map_shadow_size_get (int unit, uint32 * size)
{
    int alloc_size = 0, mem_idx, num_entries, entry_size;
    for (mem_idx = 0; mem_idx < COUNTOF(fc_map_mems); mem_idx++) {
        num_entries = soc_mem_index_count(unit, fc_map_mems[mem_idx]);
        entry_size = soc_mem_entry_words(unit, fc_map_mems[mem_idx]);
        alloc_size += entry_size * num_entries * sizeof(uint32);
    }
    *size = alloc_size;
    return SOC_E_NONE;
}

int
soc_trident2_fc_map_shadow_sync (int unit, uint32 **sync_addr)
{
    int mem_idx, num_entries, tbl_size, entry_size;

    for (mem_idx = 0; mem_idx < COUNTOF(fc_map_mems); mem_idx++) {
        num_entries = soc_mem_index_count(unit, fc_map_mems[mem_idx]);
        entry_size = soc_mem_entry_words(unit, fc_map_mems[mem_idx]);
        tbl_size = entry_size * num_entries;
        sal_memcpy(*sync_addr, soc_td2_fc_map_shadow[unit].shadow_array[mem_idx].mem_shadow, 
                   tbl_size * sizeof(uint32));
        *sync_addr += tbl_size;
    }
    return SOC_E_NONE;
}

int
soc_trident2_fc_map_shadow_load (int unit, uint32 **load_addr)
{
    int blk, mem_idx, num_entries;
    int tbl_size, entry_size, vmap_size;
    uint32 *cache;
    uint32 *shadow;
    uint8 *vmap;
    int index_min;
    soc_mem_t mem;
    
    for (mem_idx = 0; mem_idx < COUNTOF(fc_map_mems); mem_idx++) {
        mem = fc_map_mems[mem_idx];
        num_entries = soc_mem_index_count(unit, mem);
        entry_size = soc_mem_entry_words(unit, mem);
        index_min = soc_mem_index_min(unit, mem);
        tbl_size = entry_size * num_entries;
        sal_memcpy(soc_td2_fc_map_shadow[unit].shadow_array[mem_idx].mem_shadow, *load_addr,
                   tbl_size * sizeof(uint32));
        *load_addr += tbl_size;
        /* restore memory cache */
        vmap_size = (num_entries + 7) / 8;
        SOC_MEM_BLOCK_ITER(unit, mem, blk) {
            if (SOC_MEM_STATE(unit, mem).cache[blk] == NULL) {
                continue;
            }
            cache = SOC_MEM_STATE(unit, mem).cache[blk];
            vmap = SOC_MEM_STATE(unit, mem).vmap[blk];
            shadow = soc_td2_fc_map_shadow[unit].shadow_array[mem_idx].mem_shadow;
            sal_memcpy(cache, shadow, tbl_size * sizeof(uint32));
        
            sal_memset(&vmap[index_min / 8], 0xff, 
                      (num_entries + 7 - index_min) / 8);
                
            /* Clear invalid bits at the left and right ends */
            vmap[index_min / 8] &= 0xff00 >> (8 - index_min % 8);
            vmap[vmap_size - 1] &= 0x00ff >> ((8 - num_entries % 8) % 8);
        }
    }
    return SOC_E_NONE;
}
#endif

/*
* Function:
* soc_trident2_port_sched_set
* Purpose:
* Update/Set Scheduling on the port
* Parameters:
* unit    - (IN) Unit number.
* port    - (IN) Logical SOC port number.
* Returns:
* SOC_E_XXX
 */
int
soc_trident2_port_sched_set(int unit, soc_port_t port)
{
    int rv = SOC_E_NONE;
    soc_trident2_sched_type_t sched_type;
    _soc_td2p_lls_op_mode_t op_mode;

     if (IS_CPU_PORT(unit, port) || (IS_LB_PORT(unit, port))) {
         return rv;
     }

    sched_type = _soc_trident2_port_sched_type_get(unit, port);

    if (sched_type == SOC_TD2_SCHED_HSP) {
        rv = soc_td2_setup_hsp_port(unit, port);
    } else {
        if (!soc_td2_is_skip_default_lls_creation(unit)) {
            op_mode = _SOC_TD2_LLS_OP_SETUP;
            rv = soc_td2_port_lls_init(unit, port,
                    _td2_port_lls_config, op_mode, NULL, NULL);
        }
    }
    if (rv) {
        return SOC_E_INTERNAL;
    }
    return SOC_E_NONE;
}


#if defined(BCM_TRIDENT2PLUS_SUPPORT)
/*
* Function:
*     soc_td2p_lls_reinit_invalid_pointers
* Purpose:
*     Makes a copy of _td2_invalid_ptr.
*     Reinitialize _td2_invalid_ptr
* Parameters:
*     unit          - (IN) unit number
* Returns:
* SOC_E_XXX
*/

int soc_td2p_lls_reinit_invalid_pointers(int unit)
{
    int lvl;

    for (lvl = SOC_TD2_NODE_LVL_ROOT; lvl < SOC_TD2_NODE_LVL_MAX; lvl++) {
        _td2p_prev_invalid_ptr[unit][lvl] = _td2_invalid_ptr[unit][lvl];
    }

    SOC_IF_ERROR_RETURN(soc_td2_init_invalid_pointers(unit));

    return SOC_E_NONE;
}

/*
* Function:
*     soc_td2p_lls_reset_flex
* Purpose:
*     updates LLS memories (_SOC_TD2_NODE_PARENT_MEM)
*     with INVALID PARENT
* Parameters:
*     unit          - (IN) unit number
* Returns:
* SOC_E_XXX
*/
int soc_td2p_lls_reset_flex(int unit)
{
    int lvl, i;
    uint32 clrmap = 0, pipe;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int rv, cindex;


    for (pipe = _TD2_XPIPE; pipe <= _TD2_YPIPE; pipe++) {
        for (lvl = SOC_TD2_NODE_LVL_L0; lvl <= SOC_TD2_NODE_LVL_L2; lvl++) {
            mem = _SOC_TD2_PIPED_NODE_PARENT_MEM(unit, pipe, lvl);

            if ((clrmap & (1 << (pipe * SOC_TD2_NODE_LVL_MAX + lvl))) == 0) {
                /* coverity[negative_returns : FALSE] */
                for (i = 0; i <= soc_mem_index_max(unit, mem); i++) {
                    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, i, &entry);
                    if (rv) {
                        LOG_ERROR(BSL_LS_SOC_COSQ,
                                (BSL_META_U(unit,
                                            "Failed to read memory at index: %d\n"), i));
                        break;
                    }
                    cindex = soc_mem_field32_get(unit, mem, entry, C_PARENTf);
                    if ((cindex == _td2p_prev_invalid_ptr[unit][lvl]) &&
                        (cindex != INVALID_PARENT(unit, lvl))) {
                        sal_memset(entry, 0, sizeof(uint32)*SOC_MAX_MEM_WORDS);
                        soc_mem_field32_set(unit, mem, entry, C_PARENTf, INVALID_PARENT(unit, lvl));
                        SOC_IF_ERROR_RETURN
                            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, i, &entry));
                    }
                }
                clrmap |= (1 << (pipe * SOC_TD2_NODE_LVL_MAX + lvl));
            }
        }
    }

    return SOC_E_NONE;
}
#endif

#endif /* BCM_TRIDENT2_SUPPORT */

