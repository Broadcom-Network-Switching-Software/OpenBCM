/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:	l2xmsg.c
 * Purpose:	Provide a reliable stream of L2 insert/delete messages.
 *
 * This module monitors the L2X table for changes and performs callbacks
 * for each insert, delete, or port movement that is detected.
 *
 * There is a time lag from the actual table change to the callback
 * because the l2xmsg task scans the L2X table only periodically.
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>
#include <sal/core/time.h>
#include <shared/bsl.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/l2x.h>

#ifdef BCM_HURRICANE3_SUPPORT

#define L2_OVERFLOW_ENTRY_EQL(unit, e1, e2, sz, bits) \
    (!_soc_mem_cmp_l2x_sync(unit, (void *) e1, (void *) e2, sz, bits))

#define L2_OVERFLOW_IS_VALID_ENTRY(_u, _mem, _e, bitf)  \
           soc_mem_field32_get((_u), (_mem), (_e), bitf)


/* L2 overflow structure */
typedef struct l2x_overflow_data_s {
    soc_mem_t                   mem;
    int                         entry_words;
    int                         entry_bytes;
    int                         count;
    uint32                      *shadow_tab;
    uint32                      *buf;
} l2x_overflow_data_t;

STATIC l2x_overflow_data_t *l2x_ovf_data[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 * 	soc_hr3_l2_overflow_entry_get
 * Purpose:
 *   Get the values of L2 non-learned entry. 
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */

int
soc_hr3_l2_overflow_entry_get(int unit, l2x_entry_t *entry)
{
    sal_mac_addr_t  mac;
    uint64  mac_field;
    uint32  vlan_port, vlan, port;

    SOC_IF_ERROR_RETURN
        (READ_L2_NON_LEARNED_MESSAGE_MACSAr(unit, &mac_field));
    SOC_IF_ERROR_RETURN
        (READ_L2_NON_LEARNED_MESSAGE_VLAN_PORTr(unit, &vlan_port));

    SAL_MAC_ADDR_FROM_UINT64(mac, mac_field);
    soc_L2Xm_mac_addr_set(unit, entry, MAC_ADDRf, mac);
    vlan = soc_reg_field_get(unit, L2_NON_LEARNED_MESSAGE_VLAN_PORTr, 
                          vlan_port, VLANf);
    soc_L2Xm_field_set(unit, entry, VLAN_IDf, &vlan);
    port = soc_reg_field_get(unit, L2_NON_LEARNED_MESSAGE_VLAN_PORTr, 
                          vlan_port, PORTf);
    soc_L2Xm_field_set(unit, entry, PORT_NUMf, &port);
    
    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_hr3_l2_overflow_intr_disable
 * Purpose:
 *   Disable the interrupt of L2 non-learned event. 
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */

int
soc_hr3_l2_overflow_intr_disable(int unit)
{
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, IP1_INTR_ENABLEr, REG_PORT_ANY,
                                L2_ENTRY_OVF_NO_LEARN_INTRf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, IP1_INTR_ENABLEr, REG_PORT_ANY,
                                L2_ENTRY_NO_LEARN_INTRf, 0));
    /* Mark as inactive */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l2_overflow_active = FALSE;
    SOC_CONTROL_UNLOCK(unit);
    /* But don't disable interrupt as its shared by various parity events */
    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_hr3_l2_overflow_intr_enable
 * Purpose:
 *   Enable the interrupt of L2 non-learned event. 
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */

int
soc_hr3_l2_overflow_intr_enable(int unit)
{
    if (SOC_CONTROL(unit)->l2_overflow_bucket_enable) {
        /* clear status */
        SOC_IF_ERROR_RETURN(soc_reg32_set(
            unit, L2_ENTRY_OVF_NO_LEARN_STATUSr, REG_PORT_ANY, 0, 0));
        
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IP1_INTR_ENABLEr, REG_PORT_ANY,
                                L2_ENTRY_OVF_NO_LEARN_INTRf, 1));
    } else {
        /* clear status */
        SOC_IF_ERROR_RETURN(soc_reg32_set(
            unit, L2_ENTRY_NO_LEARN_STATUSr, REG_PORT_ANY, 0, 0));
        
        SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, IP1_INTR_ENABLEr, REG_PORT_ANY,
                                L2_ENTRY_NO_LEARN_INTRf, 1));
    }
    /* Mark as active */
    SOC_CONTROL_LOCK(unit);
    SOC_CONTROL(unit)->l2_overflow_active = TRUE;
    SOC_CONTROL_UNLOCK(unit);
    return SOC_E_NONE;
}


/*
 * Function:
 * 	soc_hr3_l2_overflow_fill
 * Purpose:
 *   Fill the values of L2 non-learned entry. 
 * Parameters:
 *	unit - unit number.
 *    zeros_or_ones - value to be filled
 * Returns:
 *	SOC_E_XXX.
 */

int
soc_hr3_l2_overflow_fill(int unit, uint8 zeros_or_ones)
{
    uint64  macsa;
    uint32  vlan_port;
    
    if (zeros_or_ones) {
        COMPILER_64_SET(macsa, 0xffffffff, 0xffffffff);
        vlan_port = 0xffffffff;
    } else {
        COMPILER_64_ZERO(macsa);
        vlan_port = 0;
    }
    SOC_IF_ERROR_RETURN
        (WRITE_L2_NON_LEARNED_MESSAGE_MACSAr(unit, macsa));
    SOC_IF_ERROR_RETURN
        (WRITE_L2_NON_LEARNED_MESSAGE_VLAN_PORTr(unit, vlan_port));
    
    return SOC_E_NONE;
}


/*
 * Function:
 * 	soc_hr3_l2_overflow_interrupt_handler
 * Purpose:
 *   The handler of l2 non-learned interrupts. 
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */

void
soc_hr3_l2_overflow_interrupt_handler(int unit)
{
    l2x_entry_t l2x_entry;

    if (!SOC_CONTROL(unit)->l2_overflow_active) {
        LOG_ERROR(BSL_LS_SOC_L2,
                  (BSL_META_U(unit,
                      "Received L2 overflow event with no app handler or"   \
                      " processing inactive !!\n")));
    }
    if (soc_hr3_l2_overflow_intr_disable(unit)) {
        return;
    }
    sal_memset(&l2x_entry, 0, sizeof(l2x_entry_t));
    if (soc_hr3_l2_overflow_entry_get(unit, &l2x_entry)) {
        return;
    }
    /* Callback */
    soc_l2x_callback(unit, SOC_L2X_ENTRY_OVERFLOW, NULL, &l2x_entry);  
}

/*
 * Function:
 * 	soc_hr3_l2_overflow_start
 * Purpose:
 *   Start reporting the L2 overflow events. 
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */

int
soc_hr3_l2_overflow_start(int unit)
{
    if (!SOC_CONTROL(unit)->l2_overflow_enable) {
        return SOC_E_NONE;
    }
    if (SOC_CONTROL(unit)->l2_overflow_active) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_hr3_l2_overflow_fill(unit, 0));
    SOC_IF_ERROR_RETURN(soc_hr3_l2_overflow_intr_enable(unit));
    
    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_hr3_l2_overflow_stop
 * Purpose:
 *   Stop reporting the L2 overflow events. 
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */

int
soc_hr3_l2_overflow_stop(int unit)
{
    if (!SOC_CONTROL(unit)->l2_overflow_enable) {
        return SOC_E_NONE;
    }

    SOC_IF_ERROR_RETURN(soc_hr3_l2_overflow_fill(unit, 0));
    SOC_IF_ERROR_RETURN(soc_hr3_l2_overflow_intr_disable(unit));
    
    return SOC_E_NONE;
}


/*
 * Function:
 * 	soc_hr3_l2_overflow_sync
 * Purpose:
 *   Synchronize the L2 overflow table and make any ARL callbacks that are necessary.
 *   It is called from _soc_l2x_thread.
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */

void
soc_hr3_l2_overflow_sync(int unit)
{
    uint32              new_valid, old_valid;
    uint32              *new_p, *old_p;
    int                 i, count, rv = SOC_E_NONE;

    if (l2x_ovf_data[unit] == NULL) {
        return;
    }
    
    new_p = l2x_ovf_data[unit]->buf;
    if ((rv = soc_mem_read_range(unit, l2x_ovf_data[unit]->mem,
                                 MEM_BLOCK_ANY, 0,
                                 l2x_ovf_data[unit]->count - 1,
                                 new_p)) < 0) {
                                 
        LOG_ERROR(BSL_LS_SOC_L2,
              (BSL_META_U(unit,"soc_hr3_l2_overflow_sync: DMA failed: %s\n"),
                     soc_errmsg(rv)));

        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_L2X, __LINE__, rv);

    } else {

        /*
         * Scan, compare, and sync the old and new chunks one bucket
         * at a time.
         */

        old_p = l2x_ovf_data[unit]->shadow_tab;
        count = l2x_ovf_data[unit]->count;
        for (i = 0; i < count; i++) {
            /* Check the new added entries of L2_ENTRY_OVERFLOW table */
            new_valid = L2_OVERFLOW_IS_VALID_ENTRY(unit, 
                            l2x_ovf_data[unit]->mem,
                            new_p, VALIDf);
            old_valid = L2_OVERFLOW_IS_VALID_ENTRY(unit, 
                            l2x_ovf_data[unit]->mem,
                            old_p, VALIDf);
           
            if (new_valid && !old_valid) {
                /* Add */
                soc_l2x_callback(unit, 0, 
                        NULL, (l2x_entry_t *) new_p);
            } else if (!new_valid && old_valid) {
                /* Delete */
                soc_l2x_callback(unit, 0, 
                        (l2x_entry_t *) old_p, NULL);
            } else if (new_valid && old_valid) {
                if (!L2_OVERFLOW_ENTRY_EQL(unit, old_p, new_p, 
                                WORDS2BYTES(l2x_ovf_data[unit]->entry_words), 0)) {
                    /* Change */
                    soc_l2x_callback(unit, 0, 
                        (l2x_entry_t *) old_p, (l2x_entry_t *) new_p);
                }
            }
            
            new_p += l2x_ovf_data[unit]->entry_words;
            old_p += l2x_ovf_data[unit]->entry_words;
        }

        sal_memcpy(l2x_ovf_data[unit]->shadow_tab, l2x_ovf_data[unit]->buf,
            count * l2x_ovf_data[unit]->entry_words * 4);
    }

    return;
}


/*
 * Function:
 * 	soc_hr3_l2_overflow_sync_cleanup
 * Purpose:
 *   	Cleanup shadow table of L2 overflow table synchronization.
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */

void
soc_hr3_l2_overflow_sync_cleanup(int unit) 
{
    if (l2x_ovf_data[unit]->shadow_tab) {
        sal_free(l2x_ovf_data[unit]->shadow_tab);
    }

    if (l2x_ovf_data[unit]->buf) {
        soc_cm_sfree(unit, l2x_ovf_data[unit]->buf);
    }

    if (l2x_ovf_data[unit]) {
        sal_free(l2x_ovf_data[unit]);
    }

    return;
}

/*
 * Function:
 * 	soc_hr3_l2_overflow_sync_init
 * Purpose:
 *   	Initialize shadow table and prepare for L2 overflow table synchronization.
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX.
 */
 
int
soc_hr3_l2_overflow_sync_init(int unit)
{
    int index_count;
    
    /* soc_mem_clear(unit, L2_ENTRY_OVERFLOWm, MEM_BLOCK_ALL, FALSE); */
    l2x_ovf_data[unit] = sal_alloc(sizeof(l2x_overflow_data_t), "l2x_ovf_data");
    if (l2x_ovf_data[unit] == NULL) {
        return SOC_E_MEMORY;
    }
    
    l2x_ovf_data[unit]->mem = L2_ENTRY_OVERFLOWm;
    l2x_ovf_data[unit]->entry_words = soc_mem_entry_words(unit, 
                                        l2x_ovf_data[unit]->mem);
    l2x_ovf_data[unit]->entry_bytes = soc_mem_entry_bytes(unit, 
                                        l2x_ovf_data[unit]->mem);

    index_count = soc_mem_index_count(unit, 
                                        l2x_ovf_data[unit]->mem);
    
    l2x_ovf_data[unit]->count = index_count;
    
    if (l2x_ovf_data[unit]->count == 0) {
        return SOC_E_EMPTY;
    }
    
    l2x_ovf_data[unit]->shadow_tab = 
                sal_alloc(index_count * l2x_ovf_data[unit]->entry_words * 4,
                           "l2x_ovf_old");

    l2x_ovf_data[unit]->buf = soc_cm_salloc(unit, 
                index_count * l2x_ovf_data[unit]->entry_words * 4,
                           "l2x_ovf_new");

    if ((l2x_ovf_data[unit]->shadow_tab == NULL) ||
        (l2x_ovf_data[unit]->buf == NULL)) {
        return SOC_E_MEMORY;
    }

    sal_memset(l2x_ovf_data[unit]->shadow_tab, 0, 
                index_count * l2x_ovf_data[unit]->entry_words * 4);
    
    return SOC_E_NONE;
}

#endif /* BCM_HURRICANE3_SUPPORT */
