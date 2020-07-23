/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/bsl.h>

#include <sal/types.h>
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <soc/drv.h>
#include <soc/dcb.h>
#include <soc/dcbformats.h>     /* only for 5670 crc erratum warning */
#include <soc/pbsmh.h>
#include <soc/higig.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/error.h>
#include <soc/property.h>

#ifdef BCM_CMICX_SUPPORT
#include <soc/intr_cmicx.h>
#include <soc/mcm/memregs.h>
#endif

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/cmicm.h>
#include <soc/shared/mos_intr_common.h>
#include <soc/uc_msg.h>
#include <soc/uc.h>
#include <soc/uc_dbg.h>
#include <bcm_int/common/PTP_feature.h>
#include <bcm_int/common/Bs_feature.h>
#include <bcm_int/common/PTP_feature.h>


#define SOC_UC_MCS_ADDR_START(_u, _uc)                0x0
#define SOC_UC_MCS_ADDR_CFG_IMPLEMENTED(_u, _uc)      (SOC_UC_MCS_ADDR_START(_u, _uc) + 0x80)
#define SOC_UC_MCS_ADDR_CFG_VERSION(_u, _uc)          (SOC_UC_MCS_ADDR_START(_u, _uc) + 0x84)
#define SOC_UC_MCS_ADDR_CFG_APP_TYPE(_u, _uc)         (SOC_UC_MCS_ADDR_START(_u, _uc) + 0x88)

#if defined(INCLUDE_MPLS_LM_DM)
extern uint32 mpls_lm_dm_firmware_version;
#endif

#if defined(INCLUDE_FLOWTRACKER)
extern uint32 ft_msg_version;
#endif /* INCLUDE_FLOWTRACKER */

#if defined(INCLUDE_IFA)
extern uint32 ifa_firmware_version;
#endif /* INCLUDE_IFA*/

#if defined(INCLUDE_TELEMETRY)
extern uint32 st_firmware_version;
#endif /* INCLUDE_TELEMETRY*/

#if defined(INCLUDE_INT)
extern uint32 int_firmware_version;
#endif /* INCLUDE_INT*/

#if defined(INCLUDE_SUM)
extern uint32 sum_firmware_version;
#endif /* INCLUDE_SUM */

#if defined(INCLUDE_UC_ULINK)
#include <soc/shared/ulink_pack.h>

uint8 soc_cmic_ulink_uc_active[CMICM_NUM_UCS]; 

#define UC_ULINK_IS_ACTIVE(uc)          \
        (soc_cmic_ulink_uc_active[uc]) 

#define UC_ULINK_SET_ACTIVE(uc)         \
        if((uc) < CMICM_NUM_UCS) { soc_cmic_ulink_uc_active[uc]++; } 

#define UC_ULINK_SET_DEACTIVE(uc)                       \
        if((uc) < CMICM_NUM_UCS) {                      \
          if(soc_cmic_ulink_uc_active[uc] > 0)          \
            soc_cmic_ulink_uc_active[uc]--;             \
        };
#endif
/* Macro to do 32-bit address calculations for CMICm-based addresses.
   These are NOT local addresses and can only be accessed over the
   PCIe or via CMICm-based DMA operations */

#define CMICM_ADDR(base, field) \
    ((uint32) ((base) + SAL_OFFSETOF(mos_msg_area_t, field)))
#define CMICX_ADDR(base, field) \
        ((uint32) ((base) + SAL_OFFSETOF(mos_msg_area_t, field)))

#define CMICM_DATA_WORD0_BASE(base) \
    ((uint32) ((base) + SAL_OFFSETOF(mos_msg_area_t,data[0].words[0])))
#define CMICX_DATA_WORD0_BASE(base) \
    ((uint32) ((base) + SAL_OFFSETOF(mos_msg_area_t,data[0].words[0])))
#define CMICM_DATA_WORD1_BASE(base) \
    ((uint32) ((base) + SAL_OFFSETOF(mos_msg_area_t,data[0].words[1])))
#define CMICX_DATA_WORD1_BASE(base) \
    ((uint32) ((base) + SAL_OFFSETOF(mos_msg_area_t,data[0].words[1])))
#define MOS_MSG_DATA_SIZE  sizeof(mos_msg_data_t)
#define MOS_MSG64_DATA_SIZE  sizeof(mos_msg_data_t)

#define MOS_MSG_TEST_DMA_SIZE (32)

/* Macros for passing unit + uC numbers to the threads */
#define ENCODE_UNIT_UC(unit, uC)    INT_TO_PTR((unit) << 16 | (uC))
#define DECODE_UNIT(val)            ((int)(PTR_TO_INT(val) >> 16))
#define DECODE_UC(val)              ((int)(PTR_TO_INT(val) & 0xffff))

static int soc_cmic_uc_msg_receive_internal(int unit, int uC, uint8 mclass,
               mos_msg_data_t *msg, int timeout, int is_system);

/* Internal routine to determine the base address of the msg area */
/*
 * Function:
 *      _soc_cmic_uc_msg_base
 * Purpose:
 *      Determine the base address of the msg area
 * Parameters:
 *      unit - unit number
 * Returns:
 *      address of the msg area, or NULL
 */
static uint32 _soc_cmic_uc_msg_base(int unit)
{
#ifdef BCM_METROLITE_SUPPORT
    if (SOC_IS_METROLITE(unit)) {
        return 0x1b07f000;
    }
#endif

#ifdef BCM_SABER2_SUPPORT
    if (SOC_IS_SABER2(unit)) {
        return 0x0207f000;
    }
#endif

#if defined(BCM_HELIX4_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_HELIX4(unit) || SOC_IS_KATANA2(unit)) {
        return 0x1b07f008;
    }
#endif
    
#ifdef BCM_HURRICANE2_SUPPORT
    if (SOC_IS_HURRICANE2(unit)) {
        return 0x000f0000;
    }
#endif

#ifdef BCM_HURRICANE3_SUPPORT
    if (SOC_IS_HURRICANE3(unit)) {
        return 0x000f0000;
    }
#endif

#if defined(BCM_FIRELIGHT_SUPPORT)
    if (SOC_IS_FIRELIGHT(unit)) {
        return 0x012ff000;
    }
#endif

#ifdef BCM_GREYHOUND_SUPPORT
    if (SOC_IS_GREYHOUND(unit) || SOC_IS_GREYHOUND2(unit)) {
        return 0x0100f000;
    }
#endif

#if defined(BCM_APACHE_SUPPORT) || defined(BCM_TOMAHAWK2_SUPPORT) || \
    defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_APACHE(unit) || SOC_IS_TOMAHAWK2(unit) ||
        SOC_IS_MONTEREY(unit)) {
        return 0x0127f000;
    }
#endif

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK3(unit)) {
        return 0x012ff000;
    } else if (SOC_IS_TOMAHAWKX(unit)) {
        return 0x1b07f000;
    }
#endif

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3(unit)) {
        return 0x0127f000;
    }
#endif

#if defined(BCM_DNX_SUPPORT)
    if (SOC_IS_DNX(unit)) {
        return 0x012ff000;
    }
#endif

#if defined(BCM_MAVERICK2_SUPPORT)
    if (SOC_IS_MAVERICK2(unit)) {
        return 0x012ff000;
    }
#endif

#if defined(BCM_88375) || defined(BCM_88675) || defined(BCM_88470) || defined(BCM_88270)
    if (SOC_IS_JERICHO(unit)) {
        return 0x1b07f000;
    }
#endif

    return CMICM_MSG_AREAS;
}

/* Internal routine to read MCS memory */
static int
soc_cmic_mcs_read(int unit, uint32 addr)
{
#if defined(BCM_IPROC_SUPPORT) && defined(BCM_UC_MHOST_SUPPORT)
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        return soc_cm_iproc_read(unit, addr);
    }
#endif /* BCM_IPROC_SUPPORT && BCM_UC_MHOST_SUPPORT */

    return soc_pci_mcs_read(unit, addr);
}

/* Internal routine to write MCS memory */
static int
soc_cmic_mcs_write(int unit, uint32 addr, uint32 data)
{
#if defined(BCM_IPROC_SUPPORT) && defined(BCM_UC_MHOST_SUPPORT)
    if (soc_feature(unit, soc_feature_uc_mhost)) {
        soc_cm_iproc_write(unit, addr, data);
        return SOC_E_NONE;
    }
#endif /* BCM_IPROC_SUPPORT && BCM_UC_MHOST_SUPPORT */

    return soc_pci_mcs_write(unit, addr, data);
}

 /* Static routine to add received message to receive LL
 *Inserts an element at tail of LL */
static void ll_insert_tail(ll_ctrl_t *p_ll_ctrl,  ll_element_t *p_element)
{
    assert(p_element && p_ll_ctrl);

    p_element->p_prev = p_ll_ctrl->p_tail;
    p_element->p_next = NULL; 

    if(p_ll_ctrl->ll_count) {
        /* LL is not empty */
        p_ll_ctrl->p_tail->p_next = p_element;
    } else { 
        /* LL is empty */
        assert(!p_ll_ctrl->p_tail && !p_ll_ctrl->p_head);
        p_ll_ctrl->p_head = p_element;
    }

    p_ll_ctrl->ll_count++;
    p_ll_ctrl->p_tail = p_element;
}

/* Static routine to remove message from receive LL
 *Removes an element from head of LL */
static ll_element_t* ll_remove_head (ll_ctrl_t *p_ll_ctrl)
{
    ll_element_t* p_el;
    
    assert(p_ll_ctrl);

    if(p_ll_ctrl->ll_count) {
        /* list is not empty */
        assert(p_ll_ctrl->p_head && p_ll_ctrl->p_tail);

        /* keep the removed head */
        p_el = p_ll_ctrl->p_head;

       /*update ll_ctrl*/
        p_ll_ctrl->p_head = p_el->p_next;
        p_ll_ctrl->ll_count--;
        if(p_ll_ctrl->ll_count) {
            /* there are still elements in the list */
            assert(p_ll_ctrl->p_head);
            p_ll_ctrl->p_head->p_prev = NULL;
        } else {
            /* p_el was the only list element */
            assert(p_ll_ctrl->p_tail == p_el && !p_ll_ctrl->p_head);
            p_ll_ctrl->p_tail = NULL; 
        }

        /* disconnect the removed element from the list */
        p_el->p_next = NULL;
        p_el->p_prev = NULL;
		
        return p_el;
    }
    return NULL;

}

/* Internal routine to process a status word */
/*
 * Function:
 *      _soc_cmic_uc_msg_process_status
 * Purpose:
 *      Process the uC message status from a uC
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 * Returns:
 *      0 - OK
 *      1 - uC reset
 */

int _soc_cmic_uc_msg_process_status(int unit, int uC) {
    soc_control_t       *soc = SOC_CONTROL(unit);
    
    uint32 msg_base = _soc_cmic_uc_msg_base(unit);
    uint32 area_in = MSG_AREA_ARM_TO_HOST(msg_base, uC);
    uint32 area_out = MSG_AREA_HOST_TO_ARM(msg_base, uC);
    
    /* Snapshot the status from the uC memory */
    mos_msg_host_status_t cur_status_in;

    int         out_status = 0;        /* Set when new out status */
    int         i;
    mos_msg_ll_node_t *msg_node = NULL;

    cur_status_in = soc_cmic_mcs_read(unit, CMICM_ADDR(area_in, status));

    if (MOS_MSG_STATUS_STATE(cur_status_in) == MOS_MSG_RESET_STATE) {  /* Got a reset */
        return (1);                                  /* Restart init  */
    }

    sal_mutex_take(soc->uc_msg_control, sal_sem_FOREVER);

    /*  Process the ACKS */
    for (i = MOS_MSG_STATUS_ACK_INDEX(soc->uc_msg_prev_status_in[uC]);
         i != MOS_MSG_STATUS_ACK_INDEX(cur_status_in);
         i = MOS_MSG_INCR(i)) {
        
        /* See if any thread is waiting for an ack */
        if (soc->uc_msg_ack_sems[uC][i] != NULL) {
            if (soc->uc_msg_ack_data[uC][i] != NULL) {
                *soc->uc_msg_ack_data[uC][i] =
                    MOS_MSG_ACK_BIT(cur_status_in, i);
            }
            sal_sem_give(soc->uc_msg_ack_sems[uC][i]);
            soc->uc_msg_ack_sems[uC][i] = NULL;
            soc->uc_msg_ack_data[uC][i] = NULL;
        }

        /* Give back one for each ACK received */
        sal_sem_give(soc->uc_msg_send_queue_sems[uC]);
        
    }

    /* Process the messages */
    for (i = MOS_MSG_STATUS_SENT_INDEX(soc->uc_msg_prev_status_in[uC]);
         i != MOS_MSG_STATUS_SENT_INDEX(cur_status_in); i = MOS_MSG_INCR(i)) {

        /* Read the message data in in 2 32-bit chunks */
        mos_msg_data_t msg;
        msg.words[0] = soc_cmic_mcs_read(unit,CMICM_DATA_WORD0_BASE(area_in) +
                                         MOS_MSG_DATA_SIZE * i);
        msg.words[1] = soc_cmic_mcs_read(unit,CMICM_DATA_WORD1_BASE(area_in) +
                                         MOS_MSG_DATA_SIZE * i);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d: msg recv 0x%08x 0x%08x\n"),
                     uC, msg.words[0], msg.words[1]));

        /* swap to host endian */
        msg.words[0] = soc_ntohl(msg.words[0]);
        msg.words[1] = soc_ntohl(msg.words[1]);

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d: msg recv mclass 0x%02x sclass 0x%02x len 0x%04x data 0x%08x\n"),
                     uC, msg.s.mclass, msg.s.subclass,
                     msg.s.len, msg.s.data));
        if (msg.s.mclass > MAX_MOS_MSG_CLASS) {
            MOS_MSG_ACK_BIT_CLEAR(soc->uc_msg_prev_status_out[uC], i);
        } else if ((msg.s.mclass == MOS_MSG_CLASS_SYSTEM) &&
                   (msg.s.subclass == MOS_MSG_SUBCLASS_SYSTEM_PING)) {
            /* Special case - PING just gets an ACK */
            MOS_MSG_ACK_BIT_SET(soc->uc_msg_prev_status_out[uC], i);
        } else {
            if (((soc->uc_msg_rcvd_ll[uC][msg.s.mclass]).ll_count) < MOS_MSG_EVENT_QUEUE_SIZE) {
                /* Enqueue the message in respective LL */
                msg_node = (mos_msg_ll_node_t *)sal_alloc(sizeof(mos_msg_ll_node_t), "UC Msg Node");
                if (msg_node != NULL) {
                    MOS_MSG_ACK_BIT_SET(soc->uc_msg_prev_status_out[uC], i);
                    msg_node->msg_data.words[0] = msg.words[0];
                    msg_node->msg_data.words[1] = msg.words[1];
                    ll_insert_tail(&(soc->uc_msg_rcvd_ll[uC][msg.s.mclass]), (ll_element_t *)msg_node);

                    /* Kick the waiter */
                    sal_sem_give(soc->uc_msg_rcv_sems[uC][msg.s.mclass]);

                } else {
                    /* Malloc failed, no room for the message - NACK it */
                    MOS_MSG_ACK_BIT_CLEAR(soc->uc_msg_prev_status_out[uC], i);
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Could not malloc, Sending NACK, Msg Class - %d\n"), msg.s.mclass));
                }
            } else {
                MOS_MSG_ACK_BIT_CLEAR(soc->uc_msg_prev_status_out[uC], i);
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "More than 1000 nodes in LL for Msg Class - %d\n"), msg.s.mclass));
            }
        }

        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d: Ack slot %d 0x%08x 0x%08x\n"),
                     uC, i, msg.words[0], msg.words[1]));
        out_status = 1;
    } 

    /* Update the status out with the final index */
    MOS_MSG_STATUS_ACK_INDEX_W(soc->uc_msg_prev_status_out[uC], i);
        
    /* Remember previous status for next time */
    soc->uc_msg_prev_status_in[uC] = cur_status_in;

    if (out_status) {
        /*  We have determined the new status out - write it and notify
            the other uC by writing the word once here and
            interrupting the uC*/
        soc_cmic_mcs_write(unit, CMICM_ADDR(area_out, status),
                           soc->uc_msg_prev_status_out[uC]);
        /* coverity[result_independent_of_operands] */
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            CMICX_SW_INTR_SET(unit, CMICM_SW_INTR_UC(uC));
        } else
#endif
        {
            CMIC_CMC_SW_INTR_SET(unit, CMICM_SW_INTR_UC(uC));
        }
    }

    sal_mutex_give(soc->uc_msg_control);

    return (0);
}

/*
 * Function:
 *      soc_cmic_uc_msg_active_wait
 * Purpose:
 *      To wait for the message system to becone active
 * Parameters:
 *      unit - unit number
 *      uC - microcontroller number
 * Returns:
 *      SOC_E_NONE - Handshake complete, msg active
 *      SOC_E_UNAVAIL - system shut down
 */

int
soc_cmic_uc_msg_active_wait(int unit, uint32 uC) {
    soc_control_t       *soc = SOC_CONTROL(unit);

    if ((soc == NULL) || ((soc->swIntrActive & (1 << uC)) == 0) ||
        (soc->uc_msg_active[uC] == NULL)) {
        return (SOC_E_UNAVAIL);
    }

    /* Wait for the semaphore to be available */
    if (!sal_sem_take(soc->uc_msg_active[uC], 10000000)) {
        sal_sem_give(soc->uc_msg_active[uC]);
    }

    return (SOC_E_NONE);
}
    
/*
 * Function:
 *      _soc_cmic_uc_msg_system_thread
 * Purpose:
 *      Thread to system messages
 * Parameters:
 *      unit - unit number
 * Returns:
 *      Nothing
 */

void
_soc_cmic_uc_msg_system_thread(void *unit_vp) {
    int                 unit = DECODE_UNIT(unit_vp);
    int                 uC   = DECODE_UC(unit_vp);
    soc_control_t       *soc = SOC_CONTROL(unit);
    mos_msg_data_t      rcv, send;
    uint8 *endian_buf = NULL;
    volatile int rv;
    uint8 rsubclass;
    uint16 rlen;
    uint32 rdata;

    if (soc == NULL) {
        return;
    }

    while (1) {
        while (1) {
            rv = soc_cmic_uc_msg_receive_internal(unit, uC, MOS_MSG_CLASS_SYSTEM,
                    &rcv, sal_sem_FOREVER, 1);
            rsubclass = rcv.s.subclass;
            rlen = rcv.s.len;
            rdata = rcv.s.data;
            if (rv  == SOC_E_NONE) {
                break;
            }
            if (soc_cmic_uc_msg_active_wait(unit, uC) != SOC_E_NONE) {
                /* Exit Thread */
                if (endian_buf) {
                    /* Free the buffer if it was previously allocated */
                    soc_cm_sfree(unit, endian_buf);
                    endian_buf = NULL;
                }

                /*Let the receive thread kick in and exit*/
                sal_thread_yield();

                /* wait 0.5sec until all threads exit*/
                sal_udelay(500000);

                /*  Clear the swIntr, after exiting all threads*/ 
                sal_mutex_take(soc->uc_msg_control, sal_sem_FOREVER);
                if (soc->swIntr[CMICM_SW_INTR_UC(uC)] != NULL) {
                    sal_sem_destroy(soc->swIntr[CMICM_SW_INTR_UC(uC)]);
                    soc->swIntr[CMICM_SW_INTR_UC(uC)] = NULL;
                }
                sal_mutex_give(soc->uc_msg_control);

                /* Down and not coming back up - exit thread */
                LOG_CLI((BSL_META_U(unit,
                                    "System thread exiting\n")));
                SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg, \
                    "%s", "System thread exiting\n"));
                return;
            }
        }

        if (endian_buf) {
            /* Free the buffer if it was previously allocated */
            soc_cm_sfree(unit, endian_buf);
            endian_buf = NULL;
        }

        if (rsubclass == MOS_MSG_SUBCLASS_SYSTEM_LOG) {

        } else if (rsubclass == MOS_MSG_SUBCLASS_SYSTEM_INFO) {
            int send_rv = SOC_E_INTERNAL;
            if (rlen == 0) {
                /* For older firmware that does not process SYSTEM_DMA_ENDIAN,
                 * send endianness swapping based on tested combinations
                 * Not necessary for CMICx devices.
                 */
                int big_pio, big_packet, big_other;

                send.s.mclass = MOS_MSG_CLASS_SYSTEM;
                send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_INFO_REPLY;
                send.s.data = ~0;

                if (soc_feature(unit, soc_feature_iproc)) {
                    soc_cm_get_endian(unit, &big_pio, &big_packet, &big_other);
                    if (!big_other) {
                        send.s.data = 0;
                    }
                }

                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "SYSTEM INFO REPLY (%08x)\n"),
                             (unsigned)send.s.data));
                send_rv = soc_cmic_uc_msg_send(unit, uC, &send,
                                               soc->uc_msg_send_timeout);

            } else if (rlen == soc_htons(1)) {
                sal_paddr_t endian_buf_paddr;
                /* Tell Firmware to pull local data to determine endianness */
                endian_buf = soc_cm_salloc(unit, MOS_MSG_TEST_DMA_SIZE,
                                           "uKernel DMA");
                endian_buf[0] = 1;
                endian_buf[1] = 2;
                endian_buf[2] = 3;
                endian_buf[3] = 4;
                soc_cm_sflush(unit, endian_buf, MOS_MSG_TEST_DMA_SIZE);
                endian_buf_paddr = soc_cm_l2p(unit, endian_buf);

                send.s.mclass = MOS_MSG_CLASS_SYSTEM;
                send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_DMA_ENDIAN;
                send.s.len = 0;
                send.s.data = soc_htonl(PTR_TO_INT(endian_buf_paddr));
                send_rv = soc_cmic_uc_msg_send(unit, uC, &send,
                                               soc->uc_msg_send_timeout);
            }
            if (send_rv == SOC_E_NONE) {
                /* Indicate to waiters that we are up */
                sal_sem_give(soc->uc_msg_active[uC]);
            }

        } else if (rsubclass == MOS_MSG_SUBCLASS_SYSTEM_STATS) {
            /* TBD - drop for now */
                
        } else if (rsubclass == MOS_MSG_SUBCLASS_SYSTEM_CONSOLE_IN) {
            /* TBD - drop for now */
            
        } else if (rsubclass == MOS_MSG_SUBCLASS_SYSTEM_CONSOLE_OUT) {
            LOG_CLI((BSL_META_U(unit,
                                "%c"), rdata & 0x0ff));
#if defined (INCLUDE_UC_ULINK)
        } else if (rsubclass == MOS_MSG_SUBCLASS_SYSTEM_LINK_REQ) {
            LOG_DEBUG(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "SYSTEM ULINK REQ from UC (%d)\n"),
                     uC));
            UC_ULINK_SET_ACTIVE(uC);

            soc_cmic_uc_pkd_link_notify(unit);
#endif
        } else if (rsubclass == MOS_MSG_SUBCLASS_SYSTEM_EXCEPTION) {
            LOG_CLI((BSL_META_U(unit, "uC-%d Exception\n"), uC));
            /* Exception Type :
             * 0 - Reset Reentry
             * 1 - Undefined Instruction
             * 2 - SWI
             * 3 - Prefetch Abort
             * 4 - Data Abort
             */
            LOG_CLI((BSL_META_U(unit, "Type : 0x%08x\n"), soc_uc_mem_read(unit, soc_uc_addr_to_pcie(unit, uC, 0x64))));
            /* Data Fault Status Register */
            LOG_CLI((BSL_META_U(unit, "DFSR : 0x%08x\n"), soc_uc_mem_read(unit, soc_uc_addr_to_pcie(unit, uC, 0x68))));
            /* Data Fault Address Register */
            LOG_CLI((BSL_META_U(unit, "DFAR : 0x%08x\n"), soc_uc_mem_read(unit, soc_uc_addr_to_pcie(unit, uC, 0x6c))));
            /* Instruction Fault Status Register */
            LOG_CLI((BSL_META_U(unit, "IFSR : 0x%08x\n"), soc_uc_mem_read(unit, soc_uc_addr_to_pcie(unit, uC, 0x70))));
            /* Instruction Fault Address Register */
            LOG_CLI((BSL_META_U(unit, "IFAR : 0x%08x\n"), soc_uc_mem_read(unit, soc_uc_addr_to_pcie(unit, uC, 0x74))));
        }
    }
 }

/*
 * Function:
 *      _soc_cmic_uc_msg_thread
 * Purpose:
 *      Thread to handle comminucations with UCs (ARMs)
 * Parameters:
 *      unit - unit number
 * Returns:
 *      Nothing
 */

void
_soc_cmic_uc_msg_thread(void *unit_vp) {
    int                 unit = DECODE_UNIT(unit_vp);
    int                 uC   = DECODE_UC(unit_vp);
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 i;
    int                 uC_intr;

    uint32 msg_base;
    uint32 area_in;
    uint32 area_out;

    mos_msg_host_status_t cur_status_in;
    sal_sem_t tsem;
    mos_msg_ll_node_t *msg_node;

    /* Use the message control mutex to pick a UC */
    sal_mutex_take(soc->uc_msg_control, sal_sem_FOREVER);

    /* This uC is ready to start */
    uC_intr = CMICM_SW_INTR_UC(uC);
    soc->swIntr[uC_intr] = sal_sem_create("SW interrupt",
                                       sal_sem_BINARY, 0);
    if (soc->swIntr[CMICM_SW_INTR_UC(uC)] == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                       "soc_cmic_uc_msg_thread: failed (swIntr) %d\n"), uC));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "soc_cmic_uc_msg_thread: failed (swIntr) %d\n", uC));
        sal_mutex_give(soc->uc_msg_control);
        return;
    }

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        soc_cmicm_intr0_enable(unit, IRQ_CMCx_SW_INTR(uC_intr));
    }
#endif

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_cmic_intr_enable(unit, SW_PROG_INTR);
    }
#endif

    sal_mutex_give(soc->uc_msg_control);

    /* These are addresses in the uC SRAM and not directly addressible
       from the SDK - they are addesses for PCI operations */
    msg_base = _soc_cmic_uc_msg_base(unit);
    area_in = MSG_AREA_ARM_TO_HOST(msg_base, uC);
    area_out = MSG_AREA_HOST_TO_ARM(msg_base, uC);

    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg, 
                         "uC: %d msg_area_in: %p msg_area_out: %p\n",
                         uC, area_in, area_out));

    /* While not killed off */
    while (1) {
        /* We must write the IN status area to zero.  This is becuase
        the ECC for the SRAM where it lives might not be initialized
        and the resulting ECC error would cause a PCIe error which
        would cause the SDK to crash.  Zero is safe in this protocol
        since we will not consider that a reset or init state */
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            soc_cmic_mcs_write(unit, CMICX_ADDR(area_in, status), 0);
        } else
#endif
        {
            soc_cmic_mcs_write(unit, CMICM_ADDR(area_in, status), 0);
        }
        soc->uc_msg_prev_status_out[uC] = 0;

        /* Handshake protocol:

               Master (SDK host):
                   (restart or receive RESET state)
                   RESET STATE
                   Wait for INIT state
                   INIT state
                   Wait for READY state
                   READY state

               Slave (uC)
                   (retart or recieve RESET state)
                   RESET state
                   Wait for RESET state
                   INIT state
                   Wait for INIT state
                   READY sate

            */

            
        /* Phase 1 of init handshake (wait for INIT/RESET state) */
        while (1) {
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                cur_status_in = soc_cmic_mcs_read(unit, CMICX_ADDR(area_in, status));
            } else
#endif
            {
                cur_status_in = soc_cmic_mcs_read(unit, CMICM_ADDR(area_in, status));
            }
            if (MOS_MSG_STATUS_STATE(cur_status_in) == MOS_MSG_INIT_STATE) {
                /* Our partner is the subordinate so we go to
                   INIT state when we see him go to INIT state */
                break;
            }

            /* Set the status and write it out.  We do that inside the
               loop in the first phase to avoid a race where the uKernel
               is clearing memory to clear ECC errors and the first write
               is lost.  This is not needed after the first phase since
               we see the uKernel msg area change state. */
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "UC%d messaging system: reset\n"), uC));
            SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
                "UC%d messaging system: reset\n", uC));
            soc->uc_msg_prev_status_out[uC] = MOS_MSG_RESET_STATE;
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                soc_cmic_mcs_write(unit, CMICX_ADDR(area_out, status),
                                   soc->uc_msg_prev_status_out[uC]);
                CMICX_SW_INTR_SET(unit, uC_intr);
                if (sal_sem_take(soc->swIntr[uC_intr], sal_sem_FOREVER)) {
                    goto thread_stop;
                }
                soc_cmic_intr_enable(unit, SW_PROG_INTR);
            } else
#endif
            {
                soc_cmic_mcs_write(unit, CMICM_ADDR(area_out, status),
                                   soc->uc_msg_prev_status_out[uC]);
                CMIC_CMC_SW_INTR_SET(unit, uC_intr);          /* Wake up call */
                if (sal_sem_take(soc->swIntr[uC_intr], sal_sem_FOREVER)) {
                    goto thread_stop;
                } 
            }

            if ((soc->swIntrActive & (1 << uC)) == 0) { /* If exit signaled */
                goto thread_stop;
            } 
        }

        /* Proceed to INIT state */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d messaging system: init\n"), uC));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "UC%d messaging system: init\n", uC));
        MOS_MSG_STATUS_STATE_W(soc->uc_msg_prev_status_out[uC], MOS_MSG_INIT_STATE);
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            soc_cmic_mcs_write(unit, CMICX_ADDR(area_out, status),
                               soc->uc_msg_prev_status_out[uC]);
            CMICX_SW_INTR_SET(unit, uC_intr);
        } else
#endif
        {
            soc_cmic_mcs_write(unit, CMICM_ADDR(area_out, status),
                               soc->uc_msg_prev_status_out[uC]);
            CMIC_CMC_SW_INTR_SET(unit, uC_intr);          /* Wake up call */
        }

        /* Phase 2 of init handshake (wait for READY/INIT state */
        while (1) {
            /* Get the status once */
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                cur_status_in = soc_cmic_mcs_read(unit, CMICX_ADDR(area_in, status));
            } else
#endif
            {
                cur_status_in = soc_cmic_mcs_read(unit, CMICM_ADDR(area_in, status));
            }
            if (MOS_MSG_STATUS_STATE(cur_status_in) == MOS_MSG_READY_STATE) {
                /* Our partner is the subordinate so we go to
                   READY state when we see him go to READY state */
                break;
            } else if (MOS_MSG_STATUS_STATE(cur_status_in) == MOS_MSG_RESET_STATE) {
                /* The subordinate has requested another
                   reset.  process_msg_status will detect this
                   and we will go to the top of the loop */
                break;
            }
            
            if (sal_sem_take(soc->swIntr[uC_intr], sal_sem_FOREVER)) {
                goto thread_stop;
            } 
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                soc_cmic_intr_enable(unit, SW_PROG_INTR);
            }
#endif
            if ((soc->swIntrActive & (1 << uC)) == 0) { /* If exit signaled */
                goto thread_stop;
            } 
        }

        /* Go to the ready state */
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d messaging system: ready\n"), uC));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "UC%d messaging system: ready\n", uC));
        MOS_MSG_STATUS_STATE_W(soc->uc_msg_prev_status_out[uC], MOS_MSG_READY_STATE);
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            soc_cmic_mcs_write(unit, CMICX_ADDR(area_out, status),
                               soc->uc_msg_prev_status_out[uC]);
            soc->uc_msg_prev_status_in[uC] = cur_status_in;
            CMICX_SW_INTR_SET(unit, uC_intr);
        } else
#endif
        {
            soc_cmic_mcs_write(unit, CMICM_ADDR(area_out, status),
                               soc->uc_msg_prev_status_out[uC]);
            soc->uc_msg_prev_status_in[uC] = cur_status_in;
            CMIC_CMC_SW_INTR_SET(unit, uC_intr);          /* Wake up call */
        }

        LOG_CLI((BSL_META_U(unit,
                            "UC%d messaging system: up\n"), uC));

        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "UC%d messaging system: up\n", uC));
        
        /* Give one count for each slot (all free to start) */
        for (i = 0; i < NUM_MOS_MSG_SLOTS; i++) {
            sal_sem_give(soc->uc_msg_send_queue_sems[uC]);
        }


        /* Start normal processing */
        while (1) {
            if (_soc_cmic_uc_msg_process_status(unit, uC)) {
                break;              /* Received reset */
            }
            if (sal_sem_take(soc->swIntr[uC_intr], sal_sem_FOREVER)) {
                goto thread_stop;
            }
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                soc_cmic_intr_enable(unit, SW_PROG_INTR);
            }
#endif
            if ((soc->swIntrActive & (1 << uC)) == 0) {
                /* If exit signaled */
                goto thread_stop;
            }
        }

        LOG_CLI((BSL_META_U(unit,
                            "UC messaging back to reset\n")));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg, 
            "%s", "UC messaging back to reset\n"));

        /* Connection failed - go back to reset */

        if ((soc->swIntrActive & (1 << uC)) == 0) {
            /* If exit signaled */
            goto thread_stop;
        }

        sal_sem_take(soc->uc_msg_active[uC], 0);      /* Get or timeout */

        /* Take all of the slots */
        for (i = 0; i < NUM_MOS_MSG_SLOTS; i++) {
            /* These will timeout immedaitely */
            sal_sem_take(soc->uc_msg_send_queue_sems[uC], 0);
        }
            
        /*Clear receive LL and set Semaphore count to 0*/
        sal_mutex_take(soc->uc_msg_control, sal_sem_FOREVER);
        for (i = 0; i <= MAX_MOS_MSG_CLASS; i++) {
            while( soc->uc_msg_rcvd_ll[uC][i].ll_count ){
                msg_node = (mos_msg_ll_node_t *)ll_remove_head(&(soc->uc_msg_rcvd_ll[uC][i]));
                if (msg_node != NULL){
                    sal_free(msg_node);
                }
                /*Decrement the counting semaphore count to be in sync with LL*/
                sal_sem_take(soc->uc_msg_rcv_sems[uC][i], 10000000);
            };
            
            /* Kick the waiter */
            sal_sem_give(soc->uc_msg_rcv_sems[uC][i]);
        }
        sal_mutex_give(soc->uc_msg_control);
        /*Let the receive thread kick in and exit*/
        sal_thread_yield();

        for (i = 0; i < NUM_MOS_MSG_SLOTS; i++) {
            if (soc->uc_msg_ack_sems[uC][i] != NULL) {
                /* Kick the waiter */
                tsem = soc->uc_msg_ack_sems[uC][i];
                
                soc->uc_msg_ack_sems[uC][i] = NULL;
                sal_sem_give(tsem);
            }
        }

    } /* end while (1) */

 thread_stop:    

    LOG_CLI((BSL_META_U(unit,
                        "UC msg thread dies %x\n"), uC));
    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
        "UC msg thread dies %x\n", uC));

    sal_sem_take(soc->uc_msg_active[uC], 0);

    /* NAK all of the current messages */
    for (i = 0; i < NUM_MOS_MSG_SLOTS; i++) {
        if (soc->uc_msg_ack_sems[uC][i] != NULL) {
            tsem = soc->uc_msg_ack_sems[uC][i];
                
            soc->uc_msg_ack_sems[uC][i] = NULL;
            sal_sem_give(tsem);
        }
    } 


    /* Init the rest of our structures */
    if (soc->uc_msg_send_queue_sems[uC] != NULL) {
        sal_sem_destroy(soc->uc_msg_send_queue_sems[uC]);
        soc->uc_msg_send_queue_sems[uC] = NULL;
    }
    
    /*Clear receive LL and set Semaphore count to 0*/
    sal_mutex_take(soc->uc_msg_control, sal_sem_FOREVER);
    for (i = 0; i <= MAX_MOS_MSG_CLASS; i++) {
        while( soc->uc_msg_rcvd_ll[uC][i].ll_count ) {
            msg_node = (mos_msg_ll_node_t *)ll_remove_head(&(soc->uc_msg_rcvd_ll[uC][i]));
            if (msg_node != NULL) {
                sal_free(msg_node);
            }
            /*Decrement the counting semaphore count to be in sync with LL*/
            sal_sem_take(soc->uc_msg_rcv_sems[uC][i], 10000000);
        };
        /* Kick the waiter */
        sal_sem_give(soc->uc_msg_rcv_sems[uC][i]);
    }
    sal_mutex_give(soc->uc_msg_control);
    /* This print statement is required for this function
       to get some microseconds delay to exit cleanly */
    LOG_CLI((BSL_META_U(unit,"Exit Clean\n")));
}

/*
 * Function:
 *      soc_cmic_sw_intr
 * Purpose:
 *      Process a S/W generated interrupt from one of the ARMs
 * Parameters:
 *      s - pointer to socket structure.
 *      rupt_num - sw itnerrupt number
 * Returns:
 *      Nothing
 * Notes:
 *      INTERRUPT LEVEL ROUTINE
 */

void
soc_cmic_sw_intr(int unit, uint32 rupt_num)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 cmc = SOC_PCI_CMC(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "SW Intr %d\n"), rupt_num));
    soc_cmic_ucdebug_sw_intr_count_inc(unit, rupt_num);

    /* Clear the interrupt source */
    soc_pci_write(unit, CMIC_CMCx_SW_INTR_CONFIG_OFFSET(cmc), rupt_num);

    /* Re-enable interrupts (including this one) */
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        (void)soc_cmicm_intr0_enable(unit, 0);
    }
#endif

    if (soc->swIntr[rupt_num]) {
        sal_sem_give(soc->swIntr[rupt_num]);
    }
}


/*
 * Function:
 *      soc_cmic_uc_msg_init
 * Purpose:
 *      Do nothing - threads started elsewhere
 * Parameters:
 *      unit - unit number
 * Returns:
 *      Nothing
 */

void
soc_cmic_uc_msg_init(int unit) {
    COMPILER_REFERENCE(unit);
}

#ifdef BCM_CMICX_SUPPORT

/*
 * Function:
 *      soc_cmicx_sw_intr
 * Purpose:
 *      Process a S/W generated interrupt from one of the ARMs
 * Parameters:
 *      para - unused for time being
 * Returns:
 *      Nothing
 * Notes:
 *      INTERRUPT LEVEL ROUTINE
 */

void
soc_cmicx_sw_intr(int unit, void *data)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    uint32 mhost_0_intr_val = 0, mHost0_val = 0;
    uint32 mhost_1_intr_val = 0, mHost1_val = 0;
    uint32 m0_u0_sw_prg_intr = 0, u0_val = 0;
    uint32 m0_u1_sw_prg_intr = 0, u1_val = 0;
    uint32 m0_u2_sw_prg_intr = 0, u2_val = 0;
    uint32 m0_u3_sw_prg_intr = 0, u3_val = 0;


    /* By pass mHost interrupt if chip doesn't support mHost */
    if (SOC_REG_IS_VALID(unit, ICFG_MHOST0_SW_PROG_INTRr)) {
        READ_ICFG_MHOST0_SW_PROG_INTRr(unit, &mhost_0_intr_val);
        /* Bit-0 = PCIe Host */
        mHost0_val = mhost_0_intr_val & 1;
    }
    if (SOC_REG_IS_VALID(unit, ICFG_MHOST1_SW_PROG_INTRr)) {
        READ_ICFG_MHOST1_SW_PROG_INTRr(unit, &mhost_1_intr_val);
        /* Bit-0 = PCIe Host */
        mHost1_val = mhost_1_intr_val & 1;
    }

    /* Read Cortex M0 SW PROG INTR */
    READ_ICFG_CORTEXM0_U0_SW_PROG_INTRr(unit, &u0_val);
    READ_ICFG_CORTEXM0_U1_SW_PROG_INTRr(unit, &u1_val);
    READ_ICFG_CORTEXM0_U2_SW_PROG_INTRr(unit, &u2_val);
    READ_ICFG_CORTEXM0_U3_SW_PROG_INTRr(unit, &u3_val);
    /* Bit-0 = PCIe Host, Bit-1 = iHost. */
    m0_u0_sw_prg_intr = u0_val & 3;
    m0_u1_sw_prg_intr = u1_val & 3;
    m0_u2_sw_prg_intr = u2_val & 3;
    m0_u3_sw_prg_intr = u3_val & 3;

    /* Clear Interrupts */
    if (mHost0_val) 
    {
        mhost_0_intr_val &= ~(1 << 0);
        WRITE_ICFG_MHOST0_SW_PROG_INTRr(unit, mhost_0_intr_val);
    }
    if (mHost1_val) 
    {
        mhost_1_intr_val &= ~(1 << 0);
        WRITE_ICFG_MHOST1_SW_PROG_INTRr(unit, mhost_1_intr_val);
    }

    if (m0_u0_sw_prg_intr) {
        u0_val &= ~(3 << 0);
        WRITE_ICFG_CORTEXM0_U0_SW_PROG_INTRr(unit, u0_val);
    }
    if (m0_u1_sw_prg_intr) {
        u1_val &= ~(3 << 0);
        WRITE_ICFG_CORTEXM0_U1_SW_PROG_INTRr(unit, u1_val);
    }
    if (m0_u2_sw_prg_intr) {
        u2_val &= ~(3 << 0);
        WRITE_ICFG_CORTEXM0_U2_SW_PROG_INTRr(unit, u2_val);
    }
    if (m0_u3_sw_prg_intr) {
        u3_val &= ~(3 << 0);
        WRITE_ICFG_CORTEXM0_U3_SW_PROG_INTRr(unit, u3_val);
    }

    soc->stat.uc_sw_prg_intr++;

    if (mHost0_val) 
    {
        if (soc->swIntr[CMICM_SW_INTR_UC(0)]) {
            sal_sem_give(soc->swIntr[CMICM_SW_INTR_UC(0)]);
        }
    }
    if (mHost1_val) 
    {
        if (soc->swIntr[CMICM_SW_INTR_UC(1)]) {
            sal_sem_give(soc->swIntr[CMICM_SW_INTR_UC(1)]);
        }
    }
    if (m0_u0_sw_prg_intr) {
        soc->stat.m0_u0_sw_intr++;
        if (soc->iproc_m0ssq_ctrl[0].event_sema) {
            sal_sem_give(soc->iproc_m0ssq_ctrl[0].event_sema);
        }
    }
    if (m0_u1_sw_prg_intr) {
        soc->stat.m0_u1_sw_intr++;
        if (soc->iproc_m0ssq_ctrl[1].event_sema) {
            sal_sem_give(soc->iproc_m0ssq_ctrl[1].event_sema);
        }
    }
    if (m0_u2_sw_prg_intr) {
        soc->stat.m0_u2_sw_intr++;
        if (soc->iproc_m0ssq_ctrl[2].event_sema) {
            sal_sem_give(soc->iproc_m0ssq_ctrl[2].event_sema);
        }
    }
    if (m0_u3_sw_prg_intr) {
        soc->stat.m0_u3_sw_intr++;
        if (soc->iproc_m0ssq_ctrl[3].event_sema) {
            sal_sem_give(soc->iproc_m0ssq_ctrl[3].event_sema);
        }
    }
}

/*
 * Function:
 *      soc_cmicx_uc_msg_init
 * Purpose:
 *      Initialize uC messaging
 * Parameters:
 *      unit - unit number
 * Returns:
 *      standard SOC_E_XXX
 */

int
soc_cmicx_uc_msg_init(int unit) {
    soc_cmic_intr_handler_t *handle;
    int rv = SOC_E_NONE;

    /* Register interrupts */
    handle = sal_alloc(sizeof(soc_cmic_intr_handler_t), "sw_interrupt");
    if (handle == NULL) {
        return SOC_E_MEMORY;
    }

    handle->num = SW_PROG_INTR;
    handle->intr_fn = soc_cmicx_sw_intr;
    handle->intr_data = (void *)0;

    rv = soc_cmic_intr_register(unit, handle, 1);

    sal_free(handle);

    return rv;
}
#endif  /* BCM_CMICX_SUPPORT */

/*
 * Function:
 *      soc_cmic_uc_msg_start
 * Purpose:
 *      Start communication sessions with the UCs
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_xxx
 */

int
soc_cmic_uc_msg_start(int unit) {
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 i;

    if ((soc == NULL) || (soc->uc_msg_control != NULL)) {
        return (SOC_E_INIT);
    }

    soc->swIntrActive = 0;
    soc->uc_msg_control = NULL;
    
    /* Get a few properties for efficiency */
    soc->uc_msg_queue_timeout =
        soc_property_get(unit, spn_UC_MSG_QUEUE_TIMEOUT, 20000000);
    soc->uc_msg_ctl_timeout =
        soc_property_get(unit, spn_UC_MSG_CTL_TIMEOUT, 1000000);
    soc->uc_msg_send_timeout =
        soc_property_get(unit, spn_UC_MSG_SEND_TIMEOUT, 10000000);
    soc->uc_msg_send_retry_delay =
        soc_property_get(unit, spn_UC_MSG_SEND_RETRY_DELAY, 100);

    /* Init a few fields so that the threads can sort themselves out */
    soc->uc_msg_control = sal_mutex_create("Msgctrl");
    for (i = 0; i < COUNTOF(soc->swIntr); i++) {
        soc->swIntr[i] = NULL;
    }

    for (i = 0; i < COUNTOF(soc->uc_msg_active); i++) {
        /* Init the active sem and hold it */
        soc->uc_msg_active[i] = sal_sem_create("msgActive",
                                               sal_sem_BINARY, 0);
    }

    /* Init things for the system msg threads */
    soc->uc_msg_system_control = sal_mutex_create("SysMsgCtrl");
    soc->uc_msg_system_count = 0;

    return (SOC_E_NONE);
}


/*
 * Function:
 *      soc_cmic_uc_msg_stop
 * Purpose:
 *      Stop communication sessions with the UCs
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_xxx
 */

int
soc_cmic_uc_msg_stop(int unit) {
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 uC;

    if (soc == NULL) {
        return (SOC_E_INIT);      /* Already shut down */
    }
    
    /* Kill all of the msg threads */
    for (uC = 0; uC < COUNTOF(soc->uc_msg_active); uC++) {
        soc_cmic_uc_msg_uc_stop(unit, uC);

        /* Destroy the semaphore.  Threads using it must be quiecsed */
        if (soc->uc_msg_active[uC] != NULL) {
            sal_sem_destroy(soc->uc_msg_active[uC]);
            soc->uc_msg_active[uC] = NULL;
        }
    }

    /* Destroy control mutex */
    if (soc->uc_msg_control != NULL) {
        sal_mutex_destroy(soc->uc_msg_control);
        soc->uc_msg_control = NULL;
    }

    /* Destroy system control mutex */
    if (soc->uc_msg_system_control != NULL) {
        sal_mutex_destroy(soc->uc_msg_system_control);
        soc->uc_msg_system_control = NULL;
    }


    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_cmic_uc_msg_uc_start
 * Purpose:
 *      Start communicating with one of the uCs
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 * Returns:
 *      SOC_E_xxxx
 */

int
soc_cmic_uc_msg_uc_start(int unit, int uC) {
    char                prop_buff[20];
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 i;
    sal_thread_t thread_id_uc_msg;
    sal_thread_t thread_id_uc_system_msg;


    sal_sprintf(prop_buff, "uc_msg_ctrl_%i", uC);
    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg, "uc_msg_ctrl_%i\n", uC));
    if (soc_property_get(unit, prop_buff, 1) == 0) {
        return (SOC_E_UNAVAIL);
    }

    /* Check for the master control for this uC */
    if ((soc == NULL) || ((soc->swIntrActive & (1 << uC)) != 0)) {
        return (SOC_E_BUSY);
    }
    
    sal_mutex_take(soc->uc_msg_control, sal_sem_FOREVER);
    
    /* Init some data structures */
    soc->uc_msg_send_queue_sems[uC] =
        sal_sem_create("uC msg queue", sal_sem_COUNTING, 0);
    if (soc->uc_msg_send_queue_sems[uC] == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                       "soc_cmic_uc_msg_thread: failed (uC msg) %d\n"), uC));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "soc_cmic_uc_msg_thread: failed (uC msg) %d\n", uC));
        sal_mutex_give(soc->uc_msg_control);
        return (SOC_E_MEMORY);
    }

    /* Initialize receive LL, Create receive semaphores,*/
    for (i = 0; i <= MAX_MOS_MSG_CLASS; i++) {
        soc->uc_msg_rcvd_ll[uC][i].p_head = NULL;
        soc->uc_msg_rcvd_ll[uC][i].p_tail = NULL;
        soc->uc_msg_rcvd_ll[uC][i].ll_count = 0;
        soc->uc_msg_rcv_sems[uC][i] = sal_sem_create("us_msg_rcv", sal_sem_COUNTING, 0);
        soc->uc_msg_appl_cb[uC][i] = NULL;
        soc->uc_msg_appl_cb_data[uC][i] = NULL;
    }
    
    /* Clear Ack Semaphores and data*/    
    for (i = 0; i < NUM_MOS_MSG_SLOTS; i++) {
        soc->uc_msg_ack_sems[uC][i] = NULL;
        soc->uc_msg_ack_data[uC][i] = NULL;
    }

    soc->swIntrActive |= 1 << uC;   /* This uC is active */
    thread_id_uc_msg = sal_thread_create("uC msg", SAL_THREAD_STKSZ,
                      soc_property_get(unit, spn_UC_MSG_THREAD_PRI, 95),
                      _soc_cmic_uc_msg_thread, ENCODE_UNIT_UC(unit, uC));
    if (thread_id_uc_msg == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "uC msg thread create failed\n")));
        sal_mutex_give(soc->uc_msg_control);
        return SOC_E_INTERNAL;
    } 
    thread_id_uc_system_msg = sal_thread_create("uC system msg", SAL_THREAD_STKSZ,
                      soc_property_get(unit, spn_UC_MSG_THREAD_PRI, 100),
                      _soc_cmic_uc_msg_system_thread, ENCODE_UNIT_UC(unit, uC));
    if (thread_id_uc_system_msg == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "uC msg thread create failed\n")));
        sal_thread_destroy(thread_id_uc_msg);
        sal_mutex_give(soc->uc_msg_control);
        return SOC_E_INTERNAL;
    } 

    sal_mutex_give(soc->uc_msg_control);

    if (soc_cmic_uc_msg_active_wait(unit, uC) != SOC_E_NONE) {
        /* Down and not coming back up */
        return (SOC_E_UNAVAIL);
    }

    return (SOC_E_NONE);
}

/*
 * Function:
 *      soc_cmic_uc_msg_uc_stop
 * Purpose:
 *      Stop communicating with one of the uCs
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 * Returns:
 *      SOC_E_xxx
 */

int
soc_cmic_uc_msg_uc_stop(int unit, int uC) {
    soc_control_t       *soc = SOC_CONTROL(unit);
    int i;

    /* Check for the master control for this uC */
    if ((soc == NULL) || ((soc->swIntrActive & (1 << uC)) == 0)) {
        return (SOC_E_INIT);
    }

    sal_mutex_take(soc->uc_msg_control, sal_sem_FOREVER);
    soc->swIntrActive &= ~(1 << uC);   /* This uC is inactive */
    
    if (soc->swIntr[CMICM_SW_INTR_UC(uC)] != NULL) {
        sal_sem_give(soc->swIntr[CMICM_SW_INTR_UC(uC)]); /* Kick msg thread */

    }
    
    while (1) {
        if (soc->swIntr[CMICM_SW_INTR_UC(uC)] == NULL) {
            break;                      /* It is shut down */
        } 
        sal_mutex_give(soc->uc_msg_control);
        sal_thread_yield();
        sal_mutex_take(soc->uc_msg_control, sal_sem_FOREVER);
    }

    /*Destroy receive semaphores*/
    for (i = 0; i <= MAX_MOS_MSG_CLASS; i++) {
       if (soc->uc_msg_rcv_sems[uC][i] != NULL) {
           sal_sem_give(soc->uc_msg_rcv_sems[uC][i]);
           sal_thread_yield();
           sal_sem_destroy(soc->uc_msg_rcv_sems[uC][i]);
           soc->uc_msg_rcv_sems[uC][i] = NULL;
       } 
    }

    sal_mutex_give(soc->uc_msg_control);
    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
        "soc_cmic_uc_msg_uc_stop() called uC: %d\n", uC));

    return (SOC_E_NONE);

}

/*
 * Function:
 *      soc_cmic_uc_msg_apps_notify
 * Purpose:
 *      Notify applications about shutdown, etc
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_xxx
 */

int
soc_cmic_uc_msg_apps_notify(int unit, soc_cmic_uc_shutdown_stage_t notification) {
    soc_control_t *soc = SOC_CONTROL(unit);
    int uC, appl;

    if (soc == NULL) {
        return (SOC_E_INIT);
    }

    /* Notify registered applications */
    for (uC = 0; uC < COUNTOF(soc->uc_msg_active); uC++) {
        for (appl = 0; appl <= MAX_MOS_MSG_CLASS; ++appl) {
            if (soc->uc_msg_appl_cb[uC][appl]) {
                (soc->uc_msg_appl_cb[uC][appl])(unit, uC, notification,
                                              soc->uc_msg_appl_cb_data[uC][appl]);
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cmic_uc_msg_uc_apps_notify
 * Purpose:
 *      Notify applications about shutdown, etc
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_xxx
 */

int
soc_cmic_uc_msg_uc_apps_notify(int unit, soc_cmic_uc_shutdown_stage_t notification, int uC) {
    soc_control_t *soc = SOC_CONTROL(unit);
    int appl;

    if (soc == NULL) {
        return (SOC_E_INIT);
    }

    /* Notify registered applications */
    for (appl = 0; appl <= MAX_MOS_MSG_CLASS; ++appl) {
        if (soc->uc_msg_appl_cb[uC][appl]) {
            (soc->uc_msg_appl_cb[uC][appl])(unit, uC, notification,
                                          soc->uc_msg_appl_cb_data[uC][appl]);
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_cmic_uc_msg_uc_shutdown
 * Purpose:
 *      Shutdown messaging, notifying applications. Optionally halt uCs
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_xxx
 */

int
soc_cmic_uc_msg_shutdown_halt(int unit) {
    soc_control_t *soc = SOC_CONTROL(unit);
    int uC;
    int rv;

    if (soc == NULL) {
        return (SOC_E_INIT);
    }

    soc_cmic_uc_msg_apps_notify(unit, SOC_CMIC_UC_SHUTDOWN_STARTING);

    if (soc->swIntrActive != 0) { /* Shut down messaging if it has been setup */
        if ((rv = soc_cmic_uc_msg_stop(unit)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_cmic_uc_msg_stop: failed rv=%d\n"), rv));

        }
    }

    /* Halt each uC */
    for (uC = 0; uC < COUNTOF(soc->uc_msg_active); uC++) {
        soc_uc_reset(unit, uC);
    }

    /* notify registered applications that their core is halted */
    soc_cmic_uc_msg_apps_notify(unit, SOC_CMIC_UC_SHUTDOWN_HALTED);

    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_cmic_uc_msg_uc_shutdown_halt
 * Purpose:
 *      Shutdown messaging, notifying applications. Optionally halt uCs
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_xxx
 */

int
soc_cmic_uc_msg_uc_shutdown_halt(int unit, int uC) {
    soc_control_t *soc = SOC_CONTROL(unit);
    int rv;

    if (soc == NULL) {
        return (SOC_E_INIT);
    }

    soc_cmic_uc_msg_uc_apps_notify(unit, SOC_CMIC_UC_SHUTDOWN_STARTING, uC);

    if (soc->swIntrActive != 0) { /* Shut down messaging if it has been setup */
        if ((rv = soc_cmic_uc_msg_uc_stop(unit, uC)) != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_cmic_uc_msg_uc_stop: failed rv=%d\n"), rv));
        }
    }

    /* Halt uC */
    soc_uc_reset(unit, uC);

    /* notify registered applications that their core is halted */
    soc_cmic_uc_msg_uc_apps_notify(unit, SOC_CMIC_UC_SHUTDOWN_HALTED, uC);

    return SOC_E_NONE;
}

/* Function:
 *      soc_cmic_uc_msg_send
 * Purpose:
 *      Send a message to another processor and retry until an ack
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 *      msg - the 64-bit message to send
 *      timeout - time to wait in usecs or zero (no wait)
 * Returns:
 *      SOC_E_NONE = message ACK'd (agent received msg)
 *      SOC_E_xxx - message NAK'd (no agent or buffer space full or timeout)
 */
int soc_cmic_uc_msg_send(int unit, int uC, mos_msg_data_t *msg,
                                sal_usecs_t timeout) {
    soc_control_t       *soc = SOC_CONTROL(unit);
    int32               msg_base;
    int                 rc;
    uint8               ack;
    int index;
    sal_sem_t           ack_sem;
    uint32 area_out;
    mos_msg_data_t	omsg;
    /* Figure out when to give up */
    sal_usecs_t start_time = sal_time_usecs(); 
    sal_usecs_t end_time, time_elapsed;

    if ((soc == NULL) || ((soc->swIntrActive & (1 << uC)) == 0)) {
        return SOC_E_INIT;
    }

    SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
        "uC: %d msg->s.mclass: %d msg->s.subclass: %d msg->s.len :%d msg->s.addr: 0x%x\n",
        uC, msg->s.mclass, msg->s.subclass, msg->s.len, msg->s.data));

    ack_sem = sal_sem_create("uc_msg_send", 1, 0);

    msg_base = _soc_cmic_uc_msg_base(unit);

    while (1) {                 /* Retry loop */

        rc = SOC_E_NONE;

        /* Wait for a semaphore indicating a free slot */
        if (sal_sem_take(soc->uc_msg_send_queue_sems[uC], soc->uc_msg_queue_timeout) == -1) {
            rc = SOC_E_TIMEOUT;
            break;
        }

        /* Block others for a bit */
        if (sal_mutex_take(soc->uc_msg_control, soc->uc_msg_ctl_timeout) == 0) {
            if (MOS_MSG_STATUS_STATE(soc->uc_msg_prev_status_out[uC]) !=
                MOS_MSG_READY_STATE) {
                sal_mutex_give(soc->uc_msg_control);
                rc = SOC_E_INIT;
                break;
            }

            assert(MOS_MSG_INCR(MOS_MSG_STATUS_SENT_INDEX(soc->uc_msg_prev_status_out[uC])) !=
                   MOS_MSG_STATUS_ACK_INDEX(soc->uc_msg_prev_status_in[uC]));

            /* Room for the message - update the local status copy */
            index = MOS_MSG_STATUS_SENT_INDEX(soc->uc_msg_prev_status_out[uC]);
            MOS_MSG_STATUS_SENT_INDEX_W(soc->uc_msg_prev_status_out[uC],
                                        MOS_MSG_INCR(index));

            soc->uc_msg_ack_data[uC][index] = &ack;
            soc->uc_msg_ack_sems[uC][index] = ack_sem;

            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "UC%d: msg send mclass 0x%02x sclass 0x%02x len 0x%04x data 0x%08x\n"),
                         uC, msg->s.mclass, msg->s.subclass,
                         msg->s.len, msg->s.data));
            omsg.words[0] = soc_htonl(msg->words[0]);
            omsg.words[1] = soc_htonl(msg->words[1]);

            /* Update the outbound area that contains the new message */
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "UC%d: send slot %d 0x%08x 0x%08x\n"),
                         uC, index, omsg.words[0], omsg.words[1]));

            area_out = MSG_AREA_HOST_TO_ARM(msg_base, uC);
            soc_cmic_mcs_write(unit, CMICM_DATA_WORD0_BASE(area_out) +
                               MOS_MSG_DATA_SIZE * index, 
                               omsg.words[0]);
            soc_cmic_mcs_write(unit, CMICM_DATA_WORD1_BASE(area_out) +
                               MOS_MSG_DATA_SIZE *index,
                               omsg.words[1]);
            soc_cmic_mcs_write(unit, CMICM_ADDR(area_out,status),
                               soc->uc_msg_prev_status_out[uC]);
            /* coverity[result_independent_of_operands] */
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                CMICX_SW_INTR_SET(unit, CMICM_SW_INTR_UC(uC));
            } else
#endif
            {
                CMIC_CMC_SW_INTR_SET(unit, CMICM_SW_INTR_UC(uC));
            }

            /* Return the global message control mutex */
            sal_mutex_give(soc->uc_msg_control);

            /* Wait for the ACK semaphore to be posted indicating that
               a response has been received */
            if (sal_sem_take(ack_sem, soc->uc_msg_send_timeout)) {
                soc->uc_msg_ack_data[uC][index] = NULL;
                soc->uc_msg_ack_sems[uC][index] = NULL;
                rc = SOC_E_TIMEOUT;
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "uC:%d sal_sem_take failed\n"), uC));
                break;
            }

            if (ack == 1) {
                rc = SOC_E_NONE;
                break;                  /* Message received OK */
            }

            end_time = sal_time_usecs();
            if (end_time > start_time) {
                time_elapsed = end_time - start_time;
            } else {
                time_elapsed = SAL_USECS_TIMESTAMP_ROLLOVER - start_time + end_time;
            }

            if (time_elapsed >= timeout) {
                soc->uc_msg_ack_data[uC][index] = NULL;
                soc->uc_msg_ack_sems[uC][index] = NULL;
                rc = SOC_E_TIMEOUT;
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "uC:%d time out\n"), uC));
                break;                  /* Timeout */
            }

            sal_usleep(soc->uc_msg_send_retry_delay);
        }
        
    }

    sal_sem_destroy(ack_sem);

    return (rc);
}

/*
 * Function
 *      soc_cmic_uc_msg32/64_receive_internal
 * Purpose:
 *      Wait for a message of a given type from a uC, possibly before messaging is fully up
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 *      mclass - the message class
 *      msg - pointer to buffer for message (64 bits)
 *      is_system - true if this receive is done by system thread, so no wait on messaging being up
 * Returns:
 *      SOC_E_NONE = message ACK'd (agent received msg)
 *      SOC_E_xxx - message NAK'd (no agent or buffer space full or timeout)
 */
static int soc_cmic_uc_msg_receive_internal(int unit, int uC, uint8 mclass,
                            mos_msg_data_t *msg, int timeout, int is_system)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    int                 rc = SOC_E_NONE;
    mos_msg_ll_node_t   *msg_node;

    if ((soc == NULL) || ((soc->swIntrActive & (1 << uC)) == 0)) {
        return SOC_E_INIT;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "UC%d msg wait mclass %d\n"),
                 uC, mclass));

    /* Wait/Check if msg available to be received */
    if ( !soc->uc_msg_rcv_sems[uC][mclass] || sal_sem_take(soc->uc_msg_rcv_sems[uC][mclass], timeout) ) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d semtake  - uc_msg_rcv_sems failed\n"), uC));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "%s: msg wait mclass %d uC: %d: semtake  - uc_msg_rcv_sems failed\n",
            FUNCTION_NAME(), mclass, uC));
        return SOC_E_TIMEOUT;
    }

    /* System messages must be exchanged in order to bring up messaging */
    /* So only wait for messaging to be up if this is not a system message */
    if (!is_system && soc_cmic_uc_msg_active_wait(unit, uC) != SOC_E_NONE) {
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "%s: msg wait mclass %d uC: %d: uc message thread is not active\n",
            FUNCTION_NAME(), mclass, uC));
        return SOC_E_INIT;
    }

    /* Block others for a bit */
    if (sal_mutex_take(soc->uc_msg_control, soc->uc_msg_ctl_timeout)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d semtake  - uc_msg_control timed out\n"), uC));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "%s: msg wait mclass %d uC: %d: semtake  - uc_msg_control timed out\n",
            FUNCTION_NAME(), mclass, uC));
        return SOC_E_TIMEOUT;
    }
    
    /* Remove the message from Receive LL head*/
    msg_node = (mos_msg_ll_node_t *)ll_remove_head(&(soc->uc_msg_rcvd_ll[uC][mclass]));
    if (msg_node != NULL){
        msg->words[0] = msg_node->msg_data.words[0];
        msg->words[1] = msg_node->msg_data.words[1];
        sal_free(msg_node);
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d NULL node returned from LL\n"), uC));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "%s: msg wait mclass %d uC: %d: NULL node returned from LL\n",
            FUNCTION_NAME(), mclass, uC));

        /* Check the uc_msg_control mutex availability, to avoid runtime error */
        if (soc->uc_msg_control != NULL)  {
            sal_mutex_give(soc->uc_msg_control);
        }
        return SOC_E_INTERNAL;
    }
    
    /* Return the global message control mutex */
    sal_mutex_give(soc->uc_msg_control);

    if ((rc == SOC_E_NONE) && (msg->s.mclass != mclass)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d Reply from wrong mclass\n"), uC));
        SOC_CMIC_UCDBG_LOG_ADD((unit, _bcmCmicUcDbgLogHostUcMsg,
            "%s: msg wait mclass %d uC: %d: Reply from wrong mclass\n",
            FUNCTION_NAME(), mclass, uC));
        rc = SOC_E_INTERNAL;            /* Reply from wrong mclass */
    }

    return (rc);
}

/*
 * Function
 *      soc_cmic_uc_msg_receive
 * Purpose:
 *      Wait for a message of a given type from a uC
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 *      mclass - the message class
 *      msg - pointer to buffer for message (64 bits)
 * Returns:
 *      SOC_E_NONE = message ACK'd (agent received msg)
 *      SOC_E_xxx - message NAK'd (no agent or buffer space full or timeout)
 */
int soc_cmic_uc_msg_receive(int unit, int uC, uint8 mclass,
                            mos_msg_data_t *msg, int timeout)
{
        return soc_cmic_uc_msg_receive_internal(unit, uC, mclass, msg, timeout, 0);
}

/* Function:
 *      soc_cmic_uc_msg_receive_cancel
 * Purpose:
 *      Cancel wait to receive  an application message
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 *      msg_class - application message class
 * Returns:
 *      SOC_E_NONE = Waiters kicked or none waiting
 *      SOC_E_UNAVAIL = Feature not available
 */
int soc_cmic_uc_msg_receive_cancel(int unit, int uC, int msg_class)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    mos_msg_ll_node_t   *msg_node;

    if (!soc_feature(unit, soc_feature_uc)) {
        return (SOC_E_UNAVAIL);
    }

    if ((soc == NULL) || ((soc->swIntrActive & (1 << uC)) == 0) ||
        (soc->uc_msg_active[uC] == NULL)) {
        return (SOC_E_NONE);
    }

   /* Block others for a bit */
    if (sal_mutex_take(soc->uc_msg_control, soc->uc_msg_ctl_timeout)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "UC%d semtake  - uc_msg_control timed out\n"), uC));
        return SOC_E_INTERNAL;
    }
    
    /*Clear receive LL and set Semaphore count to 0*/
    while( soc->uc_msg_rcvd_ll[uC][msg_class].ll_count ) {
        msg_node = (mos_msg_ll_node_t *)ll_remove_head(&(soc->uc_msg_rcvd_ll[uC][msg_class]));
        if (msg_node != NULL) {
            sal_free(msg_node);
        }

        /*Decrement the counting semaphore count to be in sync with LL*/
        sal_sem_take(soc->uc_msg_rcv_sems[uC][msg_class], 10000000);
    };
    
    /* Kick the waiter */
    sal_sem_give( soc->uc_msg_rcv_sems[uC][msg_class] );

    sal_mutex_give(soc->uc_msg_control);
    return (SOC_E_NONE);
}

/* Function:
 *      soc_cmic_uc_msg_send_receive
 * Purpose:
 *      Send a message to another processor and wait for a reply
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 *      send - the 64-bit message to send
 *      reply - the 64-bit reply
 *      timeout - time to wait for reply
 * Returns:
 *      SOC_E_NONE = message ACK'd (agent received msg)
 *      SOC_E_xxx - message NAK'd (no agent or buffer space full or timeout)
 */
int soc_cmic_uc_msg_send_receive(int unit, int uC, mos_msg_data_t *send,
                                 mos_msg_data_t *reply, sal_usecs_t timeout) {
    int               rc;

    sal_usecs_t start = sal_time_usecs();
    sal_usecs_t now, time_elapsed;
    
    rc = soc_cmic_uc_msg_send(unit, uC, send, timeout);
    if (rc != SOC_E_NONE) {               /* send failed */
        return rc;
    }

    now = sal_time_usecs();
    /* Check for time rollover and adjust the timeout accordingly */
    if (now > start) {
        time_elapsed = now - start;
    } else {
        time_elapsed = SAL_USECS_TIMESTAMP_ROLLOVER - start + now;
    }

    if (timeout <= time_elapsed) {
        rc = SOC_E_TIMEOUT;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "UC%d time out\n"), uC));
    } else if (rc == SOC_E_NONE) {               /* If ack'd */
        timeout -= time_elapsed;
        rc = soc_cmic_uc_msg_receive_internal(unit, uC, send->s.mclass,
                                                reply, timeout, 0);
    }

    return (rc);
}

#if defined (INCLUDE_UC_ULINK)

uint8 *cmic_uc_link_dma_buffer;

int soc_cmic_uc_pkd_link_notify(int unit)
{
    mos_msg_data_t send; 
    int uc, rv = SOC_E_NONE;
    int word;
    int word_max;
    soc_ulink_pbm_msg_t ulink_msg;
    soc_info_t          *si;
    soc_persist_t       *sop;
    uint8 *buffer;
    int data_len;

    sop = SOC_PERSIST(unit);
    si  = &SOC_INFO(unit);

    if( cmic_uc_link_dma_buffer == NULL) {
        cmic_uc_link_dma_buffer = soc_cm_salloc(unit, UC_LINK_DMA_BUFFER_LEN,
                                                      "uC_LINK_BUFFER");
        if(cmic_uc_link_dma_buffer == NULL) {
            return (BCM_E_MEMORY);
        }
    }
    buffer = cmic_uc_link_dma_buffer;

    sal_memset(buffer, 0, UC_LINK_DMA_BUFFER_LEN);
    sal_memset(&ulink_msg, 0, sizeof(soc_ulink_pbm_msg_t));
    sal_memset(&send, 0, sizeof(send));

    word_max = (si->port_addr_max + _SHR_PBMP_WORD_WIDTH) / _SHR_PBMP_WORD_WIDTH;
    word_max = (word_max <= UL_PBMP_WORD_MAX) ? word_max : UL_PBMP_WORD_MAX; 

    for(word = 0; word < word_max; word++) {
         ulink_msg.pbits[word] = sop->lc_pbm_link.pbits[word];
    }
    ulink_msg.words = word_max;
    ulink_msg.flags = 0;
    buffer = shr_ulink_pbm_pack(buffer, &ulink_msg);

    data_len = buffer - cmic_uc_link_dma_buffer;
    buffer = cmic_uc_link_dma_buffer;
    
    LOG_INFO(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit, 
             "msg len %d si maxport %d, mywordmax %d,PBM maxword %d,sopbmp 0x%x\n"
             "active uc %d,%d,%d, max sizeof msg %d\n"),
             data_len, si->port_addr_max,
             word_max, _SHR_PBMP_WORD_MAX, sop->lc_pbm_link.pbits[0],
             UC_ULINK_IS_ACTIVE(0),UC_ULINK_IS_ACTIVE(1),UC_ULINK_IS_ACTIVE(CMICM_NUM_UCS-1),
             UC_LINK_DMA_BUFFER_LEN));
    
    send.s.mclass = MOS_MSG_CLASS_SYSTEM;
    send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_LINK;
    send.s.len = bcm_htons(data_len);
    send.s.data = bcm_htonl(soc_cm_l2p(unit, buffer));

    for(uc = 0; uc < SOC_INFO(unit).num_ucs; uc++) {
        if ((!soc_uc_in_reset(unit, uc)) && UC_ULINK_IS_ACTIVE(uc)) {

            soc_cm_sflush(unit, buffer, data_len);
            soc_cm_sinval(unit, buffer, UC_LINK_DMA_BUFFER_LEN);

            if (soc_cmic_uc_msg_active_wait(unit, uc) != SOC_E_NONE) {
                /* Down and not coming back up */
                return (SOC_E_UNAVAIL);
            } 
            /* Timeout in usec */
            rv = soc_cmic_uc_msg_send(unit, uc, &send, 1000000);
            if (rv < 0) {
                LOG_INFO(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "Uc %d, pbmp 0x%x word %d, rv %d\n"), 
                         uc,  sop->lc_pbm_link.pbits[word], word, rv));
                break;
            }
        }
    }
    return rv;
}

#endif


/* Function:
 *      soc_cmic_uc_appl_init
 * Purpose:
 *      Try to initialze an application on a UC
 * Parameters:
 *      unit - unit number
 *      uC - uC number
 *      msg_class - application message class
 *      timeout - time to wait for reply
 *      version_info -  version of the SDK-based application.  The
 *                      contents of this are application dependent.
 *                      Common uses include passing a pointer to an
 *                      area to be DMA'd to the Uc, or passing a
 *                      simple version number. 
 *      min_appl_version - min version for the remote app 
 * Returns:
 *      SOC_E_NONE = message ACK'd (agent received msg)
 *      SOC_E_UNAVAIL = Cannot init this app on this uC
 *      SOC_E_CONFIG = Appl started but app is < min version
 */
int soc_cmic_uc_appl_init(int unit, int uC, int msg_class,
                          sal_usecs_t timeout, uint32 version_info,
                          uint32 min_appl_version,
                          soc_cmic_uc_appl_cb_t *shutdown_cb, void *cb_data)
{
    soc_control_t       *soc = SOC_CONTROL(unit);
    mos_msg_data_t      send, rcv;
    int                 rc;
    uint16              rlen;
    uint32              rdata;

    if (!soc_feature(unit, soc_feature_uc)) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "feature unsupport\n")));
        return (SOC_E_UNAVAIL);
    } 

    if (soc_cmic_uc_msg_active_wait(unit, uC) != SOC_E_NONE) {
        /* Down and not coming back up */
        return (SOC_E_UNAVAIL);
    } 

#ifdef BCM_CMICM_SUPPORT
#if defined (BCM_TRIDENT2_SUPPORT)
    if (soc_feature(unit, soc_feature_mcs)) {
        uint32 cfg_data;
        uint32 mcs_app_type = MCS_APP_TYPE_INVALID;
        cfg_data = soc_uc_mem_read(unit, soc_uc_addr_to_pcie(unit, uC, SOC_UC_MCS_ADDR_CFG_IMPLEMENTED(unit, uC)));
        if (cfg_data == 0xBADC0DE1) {
            cfg_data = soc_uc_mem_read(unit, soc_uc_addr_to_pcie(unit, uC, SOC_UC_MCS_ADDR_CFG_VERSION(unit, uC)));
            if (cfg_data >= 0x2) {
                cfg_data = soc_uc_mem_read(unit, soc_uc_addr_to_pcie(unit, uC, SOC_UC_MCS_ADDR_CFG_APP_TYPE(unit, uC)));
                mcs_app_type = (cfg_data & 0xFFFF0000) >> 16;
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit, "UC%d CFG: app_type: %d\n"), (cfg_data >> 16), mcs_app_type));
            }
        }

        if (mcs_app_type == MCS_APP_TYPE_KNETSYNC) {
            /* SYSTEM_APPL_INIT message not supported in KNETSYNC. */
            return (SOC_E_UNAVAIL);
        }
    }
#endif
#endif

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        send.s.mclass = MOS_MSG_CLASS_SYSTEM;
        send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_APPL_INIT;
        send.s.len = soc_htons(msg_class);
        send.s.data = soc_htonl(version_info);
        rc = soc_cmic_uc_msg_send(unit, uC, &send, timeout);
        if (rc != SOC_E_NONE) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "UC%d soc_cmic_uc_msg_send failed\n"), uC));
            return(SOC_E_FAIL);
        }
        rc = soc_cmic_uc_msg_receive_internal(unit, uC, msg_class, &rcv, timeout, 0);
        rlen = rcv.s.len;
        rdata = rcv.s.data;
    } else
#endif
    {
        send.s.mclass = MOS_MSG_CLASS_SYSTEM;
        send.s.subclass = MOS_MSG_SUBCLASS_SYSTEM_APPL_INIT;
        send.s.len = soc_htons(msg_class);
        send.s.data = soc_htonl(version_info);
        rc = soc_cmic_uc_msg_send(unit, uC, &send, timeout);
        if (rc != SOC_E_NONE) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit, "UC%d soc_cmic_uc_msg_send failed\n"), uC));
            return(SOC_E_FAIL);
        }
        rc = soc_cmic_uc_msg_receive_internal(unit, uC, msg_class, &rcv, timeout, 0);
        rlen = rcv.s.len;
        rdata = rcv.s.data;
    }

    if (rc != SOC_E_NONE) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "UC%d soc_cmic_uc_msg_receive failed\n"), uC));
        return (SOC_E_UNAVAIL);
    }

    if (rlen != 0) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "UC%d soc_cmic_uc_msg_receive length != 0\n"), uC));
        return (SOC_E_UNAVAIL);
    }

    if (soc_ntohl(rdata) < min_appl_version) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit, "UC%d appl version\n"), uC));
        return (SOC_E_CONFIG);
    } 

    soc->uc_msg_appl_cb[uC][msg_class] = shutdown_cb;
    soc->uc_msg_appl_cb_data[uC][msg_class] = cb_data;
#if defined(INCLUDE_PTP)
    if(MOS_MSG_CLASS_1588 == msg_class) {
        ptp_firmware_version = soc_ntohl(rdata);
    }
#endif
    /* BS Msg Class */
    if(MOS_MSG_CLASS_BS == msg_class) {
        bs_firmware_version =  soc_ntohl(rdata);
    }
#if defined(INCLUDE_BFD)
    if(msg_class == MOS_MSG_CLASS_BFD) {
        bfd_firmware_version = soc_ntohl(rdata);
    }
#endif

#if defined(INCLUDE_BHH)
    if(msg_class == MOS_MSG_CLASS_BHH) {
        bhh_firmware_version = soc_ntohl(rdata);
    }
#endif

#if defined(INCLUDE_ETH_LM_DM)
    if(msg_class == MOS_MSG_CLASS_ETH_LM_DM) {
        eth_lm_dm_firmware_version = soc_ntohl(rdata);
    }
#endif
#if defined(INCLUDE_MPLS_LM_DM)
    if(msg_class == MOS_MSG_CLASS_MPLS_LM_DM) {
        mpls_lm_dm_firmware_version = soc_ntohl(rdata);
    }
#endif
#if defined(INCLUDE_FLOWTRACKER)
    if(msg_class == MOS_MSG_CLASS_FT) {
        ft_msg_version = soc_ntohl(rdata);
    }
#endif /* INCLUDE_FLOWTRACKER */
#if defined(INCLUDE_IFA)
    if(msg_class == MOS_MSG_CLASS_IFA) {
        ifa_firmware_version = soc_ntohl(rdata);
    }
#endif /* INCLUDE_IFA */
#if defined(INCLUDE_TELEMETRY)
    if(msg_class == MOS_MSG_CLASS_ST) {
        st_firmware_version = soc_ntohl(rdata);
    }
#endif /* INCLUDE_TELEMETRY */
#if defined(INCLUDE_INT)
    if(msg_class == MOS_MSG_CLASS_INT) {
        int_firmware_version = soc_ntohl(rdata);
    }
#endif /* INCLUDE_INT */
#if defined(INCLUDE_SUM)
    if(msg_class == MOS_MSG_CLASS_SUM) {
        sum_firmware_version = soc_ntohl(rdata);
    }
#endif /* INCLUDE_SUM */
    return (SOC_E_NONE);
}

/* Routine to determine the base address of the BroadSync Register cache on KT2 */
uint32 soc_cmic_bs_reg_cache(int unit, int bs_num)
{
    uint32 msg_end;
    msg_end = MSG_AREA_END(_soc_cmic_uc_msg_base(unit));
    return msg_end + bs_num * sizeof(mos_msg_bs_reg_cache_t);
}


/* Internal routine to determine the base address of the timestamp data area */
uint32 soc_cmic_timestamp_base(int unit)
{
    uint32 msg_end;
    msg_end = MSG_AREA_END(_soc_cmic_uc_msg_base(unit));
    if (SOC_IS_KATANA2(unit)) {
        /* KT2 has two BS register caches after the msg area, so skip over them */
        return msg_end + 2 * sizeof(mos_msg_bs_reg_cache_t);
    } else {
        return msg_end;
    }
}


static uint64 uc_mem_read_64(int unit, unsigned addr)
{
    uint32 hi = soc_uc_mem_read(unit, addr);
    uint32 lo = soc_uc_mem_read(unit, addr + 4);
    uint64 ret;

    COMPILER_64_SET(ret, hi,lo);
    return ret;
}

/*
 * Function:
 *      soc_cmic_uc_msg_timestamp_get
 * Purpose:
 *      Get most two most recent timestamps from a given source, and corresponding firmware time
 * Parameters:
 *      unit                  - (IN)  Unit number.
 *      event_id              - (IN)  Index of desired timestamp event
 *      ts_data               - (OUT) Timestamp data
 * Returns:
 *      SOC_E_PARAM    - event_id is out of bounds
 *      SOC_E_TIMEOUT  - unable to read consistent timestamp data before timeout
 */
int
soc_cmic_uc_msg_timestamp_get(
    int unit,
    int event_id,
    soc_cmic_uc_ts_data_t *ts_data)
{
    uint64 read_ts[2] = {COMPILER_64_INIT(0,0)};  /* event timestamps as read from shared memory */
    uint64 read_prev_ts[2] = {COMPILER_64_INIT(0,0)};  /* the preceeding ts of the same event */
    uint64 read_ts0[2] = {COMPILER_64_INIT(0,0)};  /* TS0 corresponding to PTP time */
    uint64 read_ts1[2] = {COMPILER_64_INIT(0,0)};  /* TS1 corresponding to PTP time */
    soc_cmic_uc_time_t read_full_time[2] = {{COMPILER_64_INIT(0,0)}};
    int32 event_ts_minus_ts0_nsec;
    int rv = SOC_E_NONE;
    uint32 timestamp_base = soc_cmic_timestamp_base(unit);
    const int max_iter = 100;
    int iter = 0;

    if (event_id >= MOS_MSG_MAX_TS_EVENTS) {
        return SOC_E_PARAM;
    }

    do {
        read_ts[1] = read_ts[0];
        read_prev_ts[1] = read_prev_ts[0];
        read_ts0[1] = read_ts0[0];
        read_ts1[1] = read_ts1[0];
        read_full_time[1] = read_full_time[0];

        /* Get from CMICM / IPROC shared memory */
        read_full_time[0].seconds = uc_mem_read_64(unit, timestamp_base + 4);
        read_full_time[0].nanoseconds = soc_uc_mem_read(unit, timestamp_base + 12);

        read_ts0[0] = uc_mem_read_64(unit, timestamp_base + 16);
        read_ts1[0] = uc_mem_read_64(unit, timestamp_base + 24);

        read_ts[0] = uc_mem_read_64(unit, timestamp_base + 32 + 8 * event_id);
        read_prev_ts[0] = uc_mem_read_64(unit, timestamp_base + 32 + 8 * MOS_MSG_MAX_TS_EVENTS + 8 * event_id);

        /* repeat until everything we care about comes out the same way twice */
        /* to ensure that we didn't race the firmware updating the same data */
    } while ((COMPILER_64_NE(read_full_time[0].seconds,  read_full_time[1].seconds) ||
              read_full_time[0].nanoseconds != read_full_time[1].nanoseconds ||
              COMPILER_64_NE(read_ts[0], read_ts[1]) ||
              COMPILER_64_NE(read_prev_ts[0], read_prev_ts[1]) ||
              COMPILER_64_NE(read_ts1[0], read_ts1[1]) ||
              COMPILER_64_NE(read_ts0[0], read_ts0[1])) &&
             ++iter < max_iter);

    if (iter == max_iter) {
        rv = SOC_E_TIMEOUT;
        /* don't exit- populate the fields with whatever we read, in case it is useful */
    }

    ts_data->hwts = read_ts[0];
    ts_data->prev_hwts = read_prev_ts[0];

    /* ts_minus_ts0_nsec = (read_ts - read_ts0) */
    event_ts_minus_ts0_nsec = COMPILER_64_LO(read_ts[0]) - COMPILER_64_LO(read_ts0[0]);

    /* ts0/ts1/full timestamps are all for the same time.  So, find offset from event timestamp
     * (made with timestamper0) from ts0, to compute event time in ts1 / full domains
     */

    /* Start with hwts_ts1 = read_ts1.  Adjusted below by (ts - ts0) */
    COMPILER_64_COPY(ts_data->hwts_ts1, read_ts1[0]);

    /* Start with the time corresponding to ts0.  Adjusted below by (ts - ts0) */
    COMPILER_64_COPY(ts_data->time.seconds, read_full_time[0].seconds);
    ts_data->time.nanoseconds = read_full_time[0].nanoseconds;

    if (event_ts_minus_ts0_nsec > 0) {
        /* Adjusting hwts_ts1 by (ts - ts0) */
        COMPILER_64_ADD_32(ts_data->hwts_ts1, event_ts_minus_ts0_nsec);

        /* Adjusting time by (ts - ts0) */
        ts_data->time.nanoseconds += event_ts_minus_ts0_nsec;
        if (ts_data->time.nanoseconds > 1000000000) {
            ts_data->time.nanoseconds -= 1000000000;
            COMPILER_64_ADD_32(ts_data->time.seconds, 1);
        }
    } else {
        /* Adjusting hwts_ts1 by (ts - ts0) */
        COMPILER_64_SUB_32(ts_data->hwts_ts1, -event_ts_minus_ts0_nsec);

        /* Adjusting time by (ts - ts0) */
        if (ts_data->time.nanoseconds < -event_ts_minus_ts0_nsec) {
            ts_data->time.nanoseconds += (1000000000 + event_ts_minus_ts0_nsec);
            COMPILER_64_SUB_32(ts_data->time.seconds, 1);
        } else {
            ts_data->time.nanoseconds += event_ts_minus_ts0_nsec;
        }
    }
    return rv;
}


/*
 * Function:
 *      soc_cmic_uc_msg_timestamp_enable
 * Purpose:
 *      Enable a specified timestamp event
 * Parameters:
 *      unit                  - (IN)  Unit number.
 *      event_id              - (IN)  Index of desired timestamp event
 * Returns:
 *      SOC_E_PARAM    - event_id is out of bounds
 * Note:  CPU-initiated capture is one-shot.  All other events are simply enabled
 */
int
soc_cmic_uc_msg_timestamp_enable(
    int unit,
    int event_id)
{
    uint32 timestamp_base = soc_cmic_timestamp_base(unit);
    uint32 capture_reg;

    capture_reg = soc_uc_mem_read(unit, timestamp_base);

    if (soc_feature(unit, soc_feature_iproc)) {
        /* IPROC */
        /* if (event_id > 18) { */
        if (event_id >= MOS_MSG_MAX_TS_EVENTS) {
            return SOC_E_PARAM;
        }
        capture_reg |= (1 << event_id);
    } else {
        /* CMICM */
        if (event_id > 12) {
            return SOC_E_PARAM;
        }
        capture_reg |= (1 << (event_id + 16));
    }
    soc_uc_mem_write(unit, timestamp_base, capture_reg);

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_cmic_uc_msg_timestamp_disable
 * Purpose:
 *      Disable a specified timestamp event, unless used by firmware
 * Parameters:
 *      unit                  - (IN)  Unit number.
 *      event_id              - (IN)  Index of desired timestamp event
 * Returns:
 *      SOC_E_PARAM    - event_id is out of bounds
 */
int
soc_cmic_uc_msg_timestamp_disable(
    int unit,
    int event_id)
{
    uint32 timestamp_base = soc_cmic_timestamp_base(unit);
    uint32 capture_reg;

    capture_reg = soc_uc_mem_read(unit, timestamp_base);

    if (soc_feature(unit, soc_feature_iproc)) {
        /* IPROC */
        /* if (event_id > 18) { */
        if (event_id >= MOS_MSG_MAX_TS_EVENTS) {
            return SOC_E_PARAM;
        }
        capture_reg &= ~(1 << event_id);
    } else {
        /* CMICM */
        if (event_id > 12) {
            return SOC_E_PARAM;
        }
        capture_reg &= ~(1 << (event_id + 16));
    }

    soc_uc_mem_write(unit, timestamp_base, capture_reg);

    return SOC_E_NONE;
}
#else
typedef int uc_msg_c_file_not_empty; /* Make ISO compilers happy. */
#endif /* CMICM or IPROC Support */
