/* 
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tx.c
 * Purpose: Implementation of bcm_petra_tx* API for dune devices
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_TX

#include <shared/bsl.h>

#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm_int/common/tx.h>
#include <bcm/pkt.h>
#include <bcm_int/common/tx.h>

#include <bcm_int/control.h>
#include <bcm_int/dpp/utils.h>

#include <soc/dpp/headers.h>
#include <bcm_int/petra_dispatch.h>

#ifdef BCM_WARM_BOOT_API_TEST
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif


#if defined(BCM_SBUSDMA_SUPPORT) 
sbusdma_desc_handle_t _soc_port_tx_handles[FRAGMENT_SIZE][3];
#endif

volatile uint32 _soc_tx_pending[BCM_MAX_NUM_UNITS];

/* Internal structures */
typedef struct _bcm_petra_tx_async_state_s {
    bcm_pkt_t                           *first_pkt; 
    bcm_pkt_t                           *pkt; 
    void                                *cookie;
    bcm_pkt_cb_f                        cb_f;
    struct _bcm_petra_tx_async_state_s  *next;
    sal_time_t                          start_time;
} _bcm_petra_tx_async_state_t;

typedef struct _bcm_petra_tx_state_s {
    uint8                       init_done;
    sal_mutex_t                 tx_lock;
    sal_sem_t                   tx_list_sem;
    _bcm_petra_tx_async_state_t *tx_list_head;
    _bcm_petra_tx_async_state_t *tx_list_tail;
    int                         tx_list_count;
    sal_thread_t                tid;
} _bcm_petra_tx_state_t;

/* Static variables */
static _bcm_petra_tx_state_t _bcm_petra_tx_state[BCM_MAX_NUM_UNITS];

/* Local macros */
#define DPP_TX_LOCK(unit) \
        sal_mutex_take(_bcm_petra_tx_state[unit].tx_lock, sal_mutex_FOREVER)

#define DPP_TX_UNLOCK(unit) \
        sal_mutex_give(_bcm_petra_tx_state[unit].tx_lock)

#define DPP_TX_LIST_WAIT(unit) \
        sal_sem_take(_bcm_petra_tx_state[unit].tx_list_sem, sal_sem_FOREVER)

#define DPP_TX_LIST_SIGNAL(unit) \
        sal_sem_give(_bcm_petra_tx_state[unit].tx_list_sem)

#define DPP_TX_INIT_CHECK(unit) \
        BCM_DPP_UNIT_CHECK(unit);\
        if(_bcm_petra_tx_state[unit].init_done == 0) { return BCM_E_INIT; }

#define DPP_DEFAULT_TX_THREAD_PRI   (50)
#define DPP_TX_MAX_RETRY            (50)
#define DPP_TX_SYNC_WAIT            (50000)




void _soc_sbusdma_port_tx_cb(int unit, int status, sbusdma_desc_handle_t handle, void *data);
int bcm_arad_tx_cpu_tunnel(bcm_pkt_t *pkt, int dest_unit,  int remote_port,  uint32 flags, bcm_cpu_tunnel_mode_t mode);


int bcm_petra_tx_cpu_tunnel(bcm_pkt_t *pkt,    int dest_unit, int remote_port, uint32 flags, bcm_cpu_tunnel_mode_t mode);

#ifdef BCM_PETRA_SUPPORT
/* Function:    _bcm_petra_tx_list_dequeue
 * Purpose:     Dequeue from a unit specific TX list
 * Parameters:  unit (IN)   - unit number
 *              pp_st (OUT) - async state info
 * Returns:     BCM_E_XXX
 * Note:        Dequeues from head
 */
int _bcm_petra_tx_list_dequeue(int unit, _bcm_petra_tx_async_state_t **pp_st)
{
    _bcm_petra_tx_state_t *tx_state = &_bcm_petra_tx_state[unit];

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(pp_st);
    if ((tx_state == NULL) || (tx_state->tx_list_count < 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid TX state.")));
    }
    if (tx_state->tx_list_count == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_EMPTY, (_BSL_BCM_MSG("TX list count is empty, unable to Dequeue.")));
    } else if ((tx_state->tx_list_head == NULL) || 
               (tx_state->tx_list_tail == NULL)) {
        /* just an additional check...should not come here */
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "TX thread error. Invalid TX list state\n")));
        BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL); 
    }

    *pp_st = tx_state->tx_list_head;
    tx_state->tx_list_head = tx_state->tx_list_head->next;
    if (tx_state->tx_list_head) {
        if (tx_state->tx_list_head->next == NULL) {
            tx_state->tx_list_tail = tx_state->tx_list_head;
        } else {
            /* tail remains the same */
        }
    } else {
        tx_state->tx_list_tail = NULL;
    }
    tx_state->tx_list_count--;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_list_enqueue
 * Purpose:     Enqueue into unit specific TX list to be TX'ed asynchronously
 * Parameters:  unit (IN)   - unit number
 *              state (IN)  - async state info
 * Returns:     BCM_E_XXX
 * Notes:       Enqueues at tail
 */
int _bcm_petra_tx_list_enqueue(int unit, _bcm_petra_tx_async_state_t *state)
{
    _bcm_petra_tx_state_t       *tx_state = &_bcm_petra_tx_state[unit];
    _bcm_petra_tx_async_state_t *old_head;
    _bcm_petra_tx_async_state_t *old_tail;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(state);
    if ((tx_state == NULL) || (tx_state->tx_list_count < 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Invalid TX state.")));
    }

    old_head = tx_state->tx_list_head;
    old_tail = tx_state->tx_list_tail;

    if (tx_state->tx_list_count == 0) {
        tx_state->tx_list_head = state;
    } else {
        if (tx_state->tx_list_tail == NULL) {
            /* just an additional check...should not come here */
            LOG_ERROR(BSL_LS_BCM_TX,
                      (BSL_META_U(unit,
                                  "TX thread error. Invalid TX list state\n")));
            BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL);
        }
        tx_state->tx_list_tail->next = state;
    }
    state->next = NULL;
    tx_state->tx_list_tail = state; /* insert at tail */
    tx_state->tx_list_count++;

    if (DPP_TX_LIST_SIGNAL(unit) < 0) {
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "TX thread signal failed. \n")));
        tx_state->tx_list_head = old_head;
        tx_state->tx_list_tail = old_tail;
        tx_state->tx_list_count--;
        BCM_RETURN_VAL_EXIT(BCM_E_INTERNAL);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_async_tx_ready_or_done
 * Purpose:     Checks if async TX operation is pending
 * Parameters:  unit (IN)   - Device Number
 * Returns:     0 - no pending async tx op
 *              1 - if pending
 */
int
_bcm_petra_async_tx_pending(int unit)
{

    int     pending = 0;
    
    BCMDNX_INIT_FUNC_DEFS;

    
    LOG_VERBOSE(BSL_LS_BCM_TX,
                (BSL_META_U(unit,
                            "Arad _bcm_petra_async_tx_pending routine\n")));

    BCMDNX_IF_ERR_EXIT(pending);
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_async_tx_op
 * Purpose:     Send an individual packet on a local unit.
 * Parameters:  unit (IN)   - Device Number
 *              pkt  (IN)   - tx packet structure
 * Returns:     BCM_E_XXX
 * Notes:       This function does a synchronous TX operation using the async
 *              tx interface. Meaning, this function blocks till HW indicates
 *              TX operation completed or timesout.
 */
#define REG_IF_PKT_DATA_OFFSET_END      (0x2516)
int
_bcm_petra_async_tx_op(int unit, bcm_pkt_t *pkt)
{
    int hdr_len,  total_len, num_fragments,fragment;
    int     ret = SOC_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_VERBOSE(BSL_LS_BCM_TX,
                (BSL_META_U(unit,
                            "_bcm_petra_async_tx_op routine START\n")));

    if ((pkt == NULL) || (pkt->_pkt_data.len <= 0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid Packet")));
    }
    if (_bcm_petra_async_tx_pending(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("Pending TX.")));
    }
    
    hdr_len = soc_dpp_hdr_len_get(unit, pkt->_dpp_hdr_type);
    total_len = hdr_len + pkt->_pkt_data.len;
    num_fragments = (total_len + FRAGMENT_SIZE - 1)/FRAGMENT_SIZE;
    LOG_VERBOSE(BSL_LS_BCM_TX,
                (BSL_META_U(unit,
                            "Arad: Unit:%d sending a packet with"
                            " %d fragments \n"), unit, num_fragments));
    

     for (fragment=0; fragment < num_fragments; fragment++) {
            do {
#ifdef BCM_SBUSDMA_SUPPORT
                ret = soc_sbusdma_desc_run(unit, _soc_port_tx_handles[0][0]);
#endif
                if ((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) {
                    sal_usleep(SAL_BOOT_QUICKTURN ? 10000 : 1000);
                } else {
#ifdef BCM_SBUSDMA_SUPPORT
                    SOC_SBUSDMA_DM_LOCK(unit);
#endif
                    _soc_tx_pending[unit]++;
#ifdef BCM_SBUSDMA_SUPPORT
                    SOC_SBUSDMA_DM_UNLOCK(unit);
#endif
                }

            } while(((ret == SOC_E_BUSY) || (ret == SOC_E_INIT)) );

     }                                

    BCM_EXIT;
exit:
BCMDNX_FUNC_RETURN;
}

void _soc_sbusdma_port_tx_cb(int unit, int status, sbusdma_desc_handle_t handle, 
                              void *data)
{

}

/* Function:    bcm_petra_tx_init
 * Purpose:     Initialize internal data structures asynchronous transmit.
 * Parameters:  unit (IN)   - Device Number
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_init(int unit)
{
    return (bcm_common_tx_init(unit));
}

/* Function:    _bcm_petra_tx_list_age
 * Purpose:     Age out entries in TX list
 * Parameters:  uint (IN) - unit number
 * Returns:     none
 */
int
_bcm_petra_tx_list_age(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("_bcm_petra_tx_list_age is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_tunnel
 * Purpose:     Tunnel a packet to be transmited to remote unit
 * Parameters:  unit (IN)   - Device Number
 *              pkt  (IN)   - tx packet structure
 *              cookie (IN) - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_tx_tunnel(int unit, bcm_pkt_t *pkt, void *cookie)
{
    bcm_cpu_tunnel_mode_t mode = BCM_CPU_TUNNEL_PACKET; BCMDNX_INIT_FUNC_DEFS;
                                                        /* default */
    if (pkt->flags & BCM_TX_BEST_EFFORT) {
        mode = BCM_CPU_TUNNEL_PACKET_BEST_EFFORT;
    } else if (pkt->flags & BCM_TX_RELIABLE) {
        mode = BCM_CPU_TUNNEL_PACKET_RELIABLE;
    }

    BCMDNX_IF_ERR_EXIT(bcm_arad_tx_cpu_tunnel(pkt, unit, 0, BCM_CPU_TUNNEL_F_PBMP, mode));
exit:
    BCMDNX_FUNC_RETURN;
}




int
bcm_arad_tx_cpu_tunnel(bcm_pkt_t *pkt,
                  int dest_unit,
                  int remote_port,
                  uint32 flags,
                  bcm_cpu_tunnel_mode_t mode)
{
#ifdef BCM_SBUSDMA_SUPPORT       
            soc_sbusdma_desc_ctrl_t ctrl = {0};
            soc_sbusdma_desc_cfg_t cfg;
            int rv = BCM_E_NONE;
            BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
            ctrl.flags = 0;
            ctrl.cfg_count = 1;
            ctrl.buff = pkt->pkt_data; /*    soc->counter_buf64+(port*soc->counter_perport); */
            ctrl.cb = _soc_sbusdma_port_tx_cb;
            cfg.blk = pkt->blk_count ;
            cfg.addr = (uint32)remote_port; /*ing_addr + port*/;
            cfg.width = 1;
            cfg.count = pkt->blk_count;
            cfg.addr_shift = 8;
            rv = (soc_sbusdma_desc_create(dest_unit, &ctrl, &cfg, TRUE, &_soc_port_tx_handles[0][0]));
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_TX,
                          (BSL_META("Error(%s) in soc_sbusdma_desc_create\n"), bcm_errmsg(rv)));
                BCMDNX_IF_ERR_EXIT(rv);
            }
            LOG_VERBOSE(BSL_LS_BCM_TX,
                        (BSL_META("handle ip: %d\n"), _soc_port_tx_handles[0][0]));
            _bcm_petra_async_tx_op(dest_unit, pkt);
            LOG_VERBOSE(BSL_LS_BCM_TX,
                        (BSL_META("_bcm_petra_async_tx_op exit\n")));
#else
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
#endif
    BCM_EXIT; /*INIT;*/
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_local
 * Purpose:     Send packets on a local unit.
 * Parameters:  unit (IN)       - Device Number
 *              pkt  (IN)       - ptr to tx packet structure
 *              count (IN)      - number of packets to send
 *              all_done_cb (IN)- callback function when tx is done
 *              cookie (IN)     - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_tx_local_array(int unit, bcm_pkt_t **pkt, int count, 
                          bcm_pkt_cb_f all_done_cb, void *cookie)
{
    BCMDNX_INIT_FUNC_DEFS;
    if(count < 0) {
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "Invalid number of packets to send: %d\n"), count));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }
    BCMDNX_IF_ERR_EXIT(bcm_common_tx_array(unit, pkt, count, all_done_cb, cookie));
    
exit:
    BCMDNX_FUNC_RETURN;
}
/* Function:    bcm_petra_tx
 * Purpose:     Transmit a packet
 * Parameters:  unit (IN)   - Device Number
 *              pkt  (IN)   - tx packet structure
 *              cookie (IN) - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int bcm_arad_tx(int unit, bcm_pkt_t *pkt, void *cookie);

int
bcm_petra_tx(int unit, bcm_pkt_t *pkt, void *cookie)
{
    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_API_TEST
#ifdef CRASH_RECOVERY_SUPPORT
    soc_dcmn_cr_suppress(unit, dcmn_cr_no_support_tx);
#endif
#endif

    if (pkt == NULL) {
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "Invalid packet structure (NULL)\n")));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }

    BCMDNX_IF_ERR_EXIT(bcm_common_tx(unit, pkt, cookie));
exit:
    BCMDNX_FUNC_RETURN;
}




/* Function:    bcm_petra_tx_array
 * Purpose:     Transmit an array of packets
 * Parameters:  unit (IN)       - Device Number
 *              pkt  (IN)       - array of tx packet structures
 *              count (IN)      - number of packets to be sent
 *              all_done_cb (IN)- callback when all packets are sent
 *              cookie (IN)     - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_array(int unit, bcm_pkt_t **pkt, int count, 
                   bcm_pkt_cb_f all_done_cb, void *cookie)
{
    BCMDNX_INIT_FUNC_DEFS;


    BCMDNX_NULL_CHECK(pkt);
    if(count <= 0){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("count must be positive")));
    }
    if(!BCM_IS_LOCAL(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unit must be local")));
    }

    BCMDNX_IF_ERR_EXIT( bcm_common_tx_array(unit, pkt, count, all_done_cb, cookie));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_tx_list
 * Purpose:     Transmit a list of packets
 * Parameters:  unit (IN)       - Device Number
 *              pkt  (IN)       - list of tx packet structures
 *              all_done_cb (IN)- callback when all packets are sent
 *              cookie (IN)     - cookie used in callback when tx is done
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_list(int unit, bcm_pkt_t *pkt, bcm_pkt_cb_f all_done_cb, 
                  void *cookie)
{
    bcm_pkt_t *pkt_ptr[1];
    BCMDNX_INIT_FUNC_DEFS;
    
    *pkt_ptr = pkt;

    BCMDNX_NULL_CHECK(pkt);
    if(!BCM_IS_LOCAL(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unit must be local")));
    }

    BCMDNX_IF_ERR_EXIT( bcm_common_tx_list(unit, pkt, all_done_cb, cookie));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    _bcm_petra_tx_pkt_header_setup
 * Purpose:     Configure header for the packet depending on CPU port config
 * Parameters:  unit (IN)   - Device Number
 *              pkt (IN)    - TX packet structure
 * Returns:     BCM_E_XXX
 */
int
_bcm_petra_tx_pkt_header_setup(int unit, bcm_pkt_t *pkt)
{
    soc_dpp_itmh_t  itmh;
    uint32          prio_val, dp, fwd_type;

    BCMDNX_INIT_FUNC_DEFS;
    sal_memset(&itmh, 0, sizeof(soc_dpp_itmh_t));

    /* The following fields are not configurable using bcm_pkt_t fields. 
     * If they need to be configured, a header needs to be built directly
     * using soc_dpp_hdr_* functions and _dpp_hdr and _dpp_hdr_type should
     * be set appropriately
     * 
     *  version
     *  pph_present
     *  out_mirr_dis
     *  in_mirr_dis
     *  snoop_cmd
     *  exclude_src 
     */
    prio_val = (pkt->flags & BCM_TX_PRIO_INT) ? pkt->prio_int : pkt->cos;
    soc_dpp_itmh_field_set(unit, &itmh, ITMH_traffic_class, prio_val);

    switch (pkt->color) {
    case bcmColorGreen:
        dp = 0;
        break;
    case bcmColorYellow:
        dp = 1;
        break;
    case bcmColorRed:
        dp = 2;
        break;
    case bcmColorBlack:
        dp =3;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_TX,
                  (BSL_META_U(unit,
                              "Unit:%d invalid color (%d) specified. Could not map to a "
                              "valid drop-precedence\n"), unit, pkt->color));
        BCM_RETURN_VAL_EXIT(BCM_E_PARAM);
    }
    soc_dpp_itmh_field_set(unit, &itmh, ITMH_dp, dp);

    if (pkt->flow_id > 0) {
        fwd_type = DPP_HDR_ITMH_FWD_TYPE_UNICAST_FLOW;
        soc_dpp_itmh_field_set(unit, &itmh, ITMH_flow_id, pkt->flow_id);
    } else if (pkt->multicast_group > 0) {
        fwd_type = DPP_HDR_ITMH_FWD_TYPE_SYSTEM_MULTICAST;
        soc_dpp_itmh_field_set(unit, &itmh, ITMH_multicast_id, 
                               pkt->multicast_group);
    } else {
        /* default to UC direct */
        fwd_type = DPP_HDR_ITMH_FWD_TYPE_UNICAST_DIRECT;
        soc_dpp_itmh_field_set(unit, &itmh, ITMH_dest_sys_port, 
                               pkt->src_gport);
    }
    soc_dpp_itmh_field_set(unit, &itmh, ITMH_fwd_type, fwd_type);
    soc_dpp_itmh_field_set(unit, &itmh, ITMH_pph_present, 1);
    sal_memcpy(&pkt->_dpp_hdr, &itmh.base.raw.bytes[0], 
               DPP_HDR_ITMH_BASE_LEN);

    {
        
#if 0

        uint8 pph[15] =
            {0x00,0x00,0x80,0x00,0x10,
             0x40,0x00,0x00,0x00,0x00,
             0x00,0x00,0x00,0x00,0x63};

        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "PPH: 0x%02x-%02x-%02x-%02x-%02x \n"),
                     pph[0], pph[1], pph[2], pph[3], pph[4]));
        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "       %02x-%02x-%02x-%02x-%02x \n"),
                     pph[5], pph[6], pph[7], pph[8], pph[9]));
        LOG_VERBOSE(BSL_LS_BCM_TX,
                    (BSL_META_U(unit,
                                "       %02x-%02x-%02x-%02x-%02x \n"),
                     pph[10], pph[11], pph[12], pph[13], pph[14]));
        sal_memcpy(&pkt->_dpp_hdr[DPP_HDR_ITMH_BASE_LEN], &pph[0], 15);

#endif

    }

    pkt->_dpp_hdr_type = DPP_HDR_itmh_base;
    
    BCM_EXIT;
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Function:    bcm_petra_tx_pkt_setup
 * Purpose:     Configure the tx packet structure for TX
 * Parameters:  unit (IN)   - Device Number
 *              pkt (IN)    - TX packet structure
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_pkt_setup(int unit, bcm_pkt_t *pkt)
{
    BCMDNX_INIT_FUNC_DEFS;
  

    BCMDNX_NULL_CHECK(pkt);
    if (!BCM_IS_LOCAL(unit)) {
        /* nothing to do for non-local units */
        BCM_EXIT;
    }
    BCMDNX_IF_ERR_EXIT( bcm_common_tx_pkt_setup(unit,pkt));
exit:
    BCMDNX_FUNC_RETURN;
}

#if defined(BROADCOM_DEBUG)
/* Function:    bcm_petra_tx_show
 * Purpose:     Display info about tx state
 * Parameters:  unit (IN)   - Device Number
 * Returns:     BCM_E_XXX
 */
int
bcm_petra_tx_show(int unit)
{

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT( bcm_common_tx_show(unit));
exit:
    BCMDNX_FUNC_RETURN;
}
#endif /* BROADCOM_DEBUG */

#endif /* BCM_PETRA_SUPPORT */

