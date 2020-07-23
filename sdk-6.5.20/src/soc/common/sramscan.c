/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * SRAM Memory Error Scan
 *
 * This is an optional module that can be used to detect and correct
 * parity or ECC errors in certain static hardware memories (SRAM memories).
 * For SRAM memory, scan or scrub is not very critical. But performing SRAM 
 * scan has following benefit: ensuring that entries are proactively corrected 
 * prior to an actual hardware/packet lookup.
 * Additional CPU time is required to do the scanning.  Table DMA is
 * used to reduce this overhead.  The application may choose the overall
 * scan rate in entries/sec.
 *
 * similar to TCAM scan variables, the following configure variables will 
 * be added to support SRAM scan.
 * 
 * ?  sram_scan_chunk_size
 * To specify the number of table entries to be retrieved at a time during
 * SRAM memory scanning.

 * ?	sram_scan_enable
 * To control to automatically run background SRAM memory scan.
 * 
 * ?	sram_scan_interval 
 * To specify the SRAM scan repeat interval.
 * 
 * ?	sram_scan_rate
 * To specify the SRAM scan rate in terms of entries per pass.
 * 
 * ?	sram_scan_thread_pri
 * To specify the priority of the SRAM scanning and error correction thread.
 * 
 
 * File:      sramscan.c
 * Purpose:   scan sram memories to detect parity error in the chip.
 * Requires:  sal/soc/shared layer
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/cmic.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/l2x.h>

#ifdef BCM_SRAM_SCAN_SUPPORT

#if defined(BCM_TRIDENT2_SUPPORT)
#include <soc/trident2.h>
#endif /* BCM_TRIDENT2_SUPPORT */
#if defined(BCM_TOMAHAWK_SUPPORT)
#include <soc/tomahawk.h>
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_APACHE_SUPPORT)
#include <soc/apache.h>
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
#include <soc/monterey.h>
#endif /* BCM_MONTEREY_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(ALPM_ENABLE)
#include <soc/l3x.h>
#define _ALPM_LOCK(unit) \
    L3_LOCK(unit); \
    _alpm_lock = 1
#define _ALPM_UNLOCK(unit) \
    if (_alpm_lock) { \
        L3_UNLOCK(unit); \
    }
#else
#define _ALPM_LOCK(unit)
#define _ALPM_UNLOCK(unit)
#endif /* ALPM_ENABLE */

STATIC void _soc_sram_scan_thread(void *unit_vp);

/*
 * Function:
 * 	soc_sram_scan_running
 * Purpose:
 *   	Boolean to indicate if the sram scan thread is running
 * Parameters:
 *	unit - unit number.
 *	rate - (OUT) if non-NULL, number of entries scanned per interval
 *	interval - (OUT) if non-NULL, receives current wake-up interval.
 */

int
soc_sram_scan_running(int unit, int *rate, sal_usecs_t *interval)
{
    soc_control_t	*soc = SOC_CONTROL(unit);

    if (soc->sram_scan_pid != SAL_THREAD_ERROR) {
        if (rate != NULL) {
            *rate = soc->sram_scan_rate;
        }
        if (interval != NULL) {
            *interval = soc->sram_scan_interval;
        }
    }

    return (soc->sram_scan_pid != SAL_THREAD_ERROR);
}

/*
 * Function:
 * 	soc_sram_scan_start
 * Purpose:
 *   	Start sram scan thread
 * Parameters:
 *	unit - unit number.
 *	rate - maximum number of entries to scan each time thread runs
 *	interval - how often the thread should run (microseconds).
 * Returns:
 *	SOC_E_MEMORY if can't create thread.
 */

int
soc_sram_scan_start(int unit, int rate, sal_usecs_t interval)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int pri;

    if (rate <= 0) {
        return (SOC_E_PARAM);
    }

    if (soc->sram_scan_pid != SAL_THREAD_ERROR) {
        SOC_IF_ERROR_RETURN(soc_sram_scan_stop(unit));
    }
    sal_snprintf(soc->sram_scan_name, sizeof (soc->sram_scan_name),
                 "bcmSRAM_SCAN.%d", unit);
    soc->sram_scan_rate = rate;
    soc->sram_scan_interval = interval;
    if (interval == 0) {
        return SOC_E_NONE;
    }
    if (soc->sram_scan_pid == SAL_THREAD_ERROR) {
        pri = soc_property_get(unit, spn_SRAM_SCAN_THREAD_PRI, 50);
        soc->sram_scan_pid = sal_thread_create(soc->sram_scan_name,
                                               SAL_THREAD_STKSZ,
                                               pri,
                                               _soc_sram_scan_thread,
                                               INT_TO_PTR(unit));
        if (soc->sram_scan_pid == SAL_THREAD_ERROR) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "soc_sram_scan_start: Could not start sram_scan thread\n")));
            return SOC_E_MEMORY;
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 * 	soc_sram_scan_stop
 * Purpose:
 *   	Stop sram scan thread
 * Parameters:
 *	unit - unit number.
 * Returns:
 *	SOC_E_XXX
 */

int
soc_sram_scan_stop(int unit)
{
    soc_control_t	*soc = SOC_CONTROL(unit);
    int			rv = SOC_E_NONE;
    soc_timeout_t	to;
    int tosec;

    soc->sram_scan_interval = 0;		/* Request exit */
    if (soc->sram_scan_pid != SAL_THREAD_ERROR) {
        sal_sem_give(soc->sram_scan_notify);
        
        tosec = 5;
#ifdef PLISIM
        if (SAL_BOOT_PLISIM) {
            tosec = 15;
        }
#endif
        soc_timeout_init(&to, tosec * 1000000, 0);
        while (soc->sram_scan_pid != SAL_THREAD_ERROR) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "soc_sram_scan_stop: thread will not exit\n")));
                rv = SOC_E_INTERNAL;
                break;
            }
        }
    }
    return rv;
}

STATIC void
_soc_sram_scan_info_get (int unit, soc_mem_t mem, int blk, int *num_pipe, int *ser_flags)
{
    soc_acc_type_t mem_acc_type;
    
    if (NUM_PIPE(unit) == 2) {
        if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_IPIPE ||
            SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_EPIPE) {
            mem_acc_type = SOC_MEM_ACC_TYPE(unit, mem);
            switch (mem_acc_type) {
            case _SOC_ACC_TYPE_PIPE_Y:
                *num_pipe = 1;
                ser_flags[0] = _SOC_SER_FLAG_MULTI_PIPE |
                               _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y;
                if (!soc_mem_dmaable(unit, mem, blk)) {
                    ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                }
                break;
            case _SOC_ACC_TYPE_PIPE_X:
                *num_pipe = 1;
                ser_flags[0] = _SOC_SER_FLAG_MULTI_PIPE |
                               _SOC_MEM_ADDR_ACC_TYPE_PIPE_X;
                if (!soc_mem_dmaable(unit, mem, blk)) {
                    ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                }
                break;
            default:
                *num_pipe = 2;
                ser_flags[0] = _SOC_SER_FLAG_MULTI_PIPE |
                               _SOC_MEM_ADDR_ACC_TYPE_PIPE_X;
                ser_flags[1] = _SOC_SER_FLAG_MULTI_PIPE |
                               _SOC_MEM_ADDR_ACC_TYPE_PIPE_Y;
                if (!soc_mem_dmaable(unit, mem, blk)) {
                    ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
                    ser_flags[1] |= _SOC_SER_FLAG_NO_DMA;
                }
                break;
            }
        } else {
            *num_pipe = 1;
            ser_flags[0] = 0;
            if (!soc_mem_dmaable(unit, mem, blk)) {
                ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
            }
        }
#if defined(BCM_TRIDENT_SUPPORT)
        if ((SOC_IS_TRIDENT(unit) || SOC_IS_TITAN(unit)) &&
            (mem == EGR_VLANm)) {
            *num_pipe = 1;
            ser_flags[0] = _SOC_SER_FLAG_NO_DMA;
        }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
        if ((SOC_IS_TRIDENT2(unit) || SOC_IS_TITAN2(unit)) &&
            (mem == L3_DEFIP_ALPM_IPV4m ||
             mem == L3_DEFIP_ALPM_IPV4_1m ||
             mem == L3_DEFIP_ALPM_IPV6_64m ||
             mem == L3_DEFIP_ALPM_IPV6_64_1m ||
             mem == L3_DEFIP_ALPM_IPV6_128m ||
             mem == L3_DEFIP_ALPM_RAWm)) {
            ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
            ser_flags[1] |= _SOC_SER_FLAG_NO_DMA;
        }
#endif
    } else {
        *num_pipe = 1;
        if (SOC_IS_APACHE(unit) && (mem == EGR_VLANm)) {
            ser_flags[0] = _SOC_SER_FLAG_NO_DMA;
        } else {
            ser_flags[0] = 0;
        }
        if (!soc_mem_dmaable(unit, mem, blk)) {
            ser_flags[0] |= _SOC_SER_FLAG_NO_DMA;
        }
    }
}

STATIC int
_soc_mem_is_eligible_for_sramscan (int unit, soc_mem_t mem)
{
    if (!SOC_MEM_IS_VALID(unit, mem)) {
        return FALSE;
    }
    if (soc_mem_index_count(unit, mem) == 0) {
        return FALSE;
    }
    if (soc_feature(unit, soc_feature_ser_parity)) {
        if ((SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_ENGINE)) {
#if defined(BCM_ESW_SUPPORT)
            LOG_VERBOSE(BSL_LS_SOC_SER,
                        (BSL_META_U(unit,
                                    "sram_scrub: skipping mem %s (SOC_MEM_FLAG_SER_ENGINE is set)\n"),
                        SOC_MEM_NAME(unit, mem)));
#endif
            return FALSE;
        }
    }
#ifdef BCM_ESW_SUPPORT
    if(TRUE == soc_mem_is_mapped_mem(unit, mem)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "sram_scrub: skipping mem %s (soc_mem_is_mapped_mem)\n"),
                    SOC_MEM_NAME(unit, mem)));
        return FALSE;
    }
#endif
#ifdef SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT
    if(TRUE == soc_mem_is_cache_only(unit, mem)) {
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "sram_scrub: skipping mem %s (soc_mem_is_cache_only)\n"),
                    SOC_MEM_NAME(unit, mem)));
        return FALSE;
    }
#endif /*  SOC_L3_ECMP_PROTECTED_ACCESS_SUPPORT */

    if (SOC_MEM_SER_CORRECTION_TYPE(unit, mem) == 0) {
#if defined(BCM_ESW_SUPPORT)
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "sram_scrub: skipping mem %s (ERR_CORRECTION = 0)\n"),
                    SOC_MEM_NAME(unit, mem)));
#endif
        return FALSE;
    }
    if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_ENTRY_CLEAR) {
#if defined(BCM_ESW_SUPPORT)
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "sram_scrub: skipping mem %s (SER_RESPONSE = SER_ENTRY_CLEAR)\n"),
                    SOC_MEM_NAME(unit, mem)));
#endif
        return FALSE;
    }
    if (SOC_MEM_INFO(unit, mem).flags & SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP) {
#if defined(BCM_ESW_SUPPORT)
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "sram_scrub: skipping mem %s (SOC_MEM_FLAG_SER_PARITY_ENABLE_SKIP is set)\n"),
                    SOC_MEM_NAME(unit, mem)));
#endif
        return FALSE;
    }
#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit) && soc_th_check_scrub_skip(unit, mem, 0)) {
        return FALSE;
    }
#endif
#if defined(BCM_TOMAHAWK3_SUPPORT)
    /* In TH3, the flag SOC_MEM_FLAG_SER_ENGINE isn't set due to we use
           HW tcam scan engine. Hence using SOC_MEM_FLAG_CAM to check if
           the memory is tcam */
    if (SOC_IS_TOMAHAWK3(unit) && soc_mem_is_cam(unit, mem)) {
        return FALSE;
    }
#endif
#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3X(unit) && soc_td3_check_scrub_skip(unit, mem, 0)) {
        return FALSE;
    }
#endif
#if defined(BCM_TRIDENT2_SUPPORT)
    if (SOC_IS_TD2P_TT2P(unit) &&
    	  soc_trident2_ser_test_skip_check(unit, mem, _SOC_ACC_TYPE_PIPE_ANY)) {
        return FALSE;
    }
#endif
#if defined(BCM_MONTEREY_SUPPORT)
    if (SOC_IS_MONTEREY(unit) &&
        soc_monterey_ser_test_skip_check(unit, mem)) {
        return FALSE;
    }
#endif
#if defined(BCM_APACHE_SUPPORT)
    if (SOC_IS_APACHE(unit) &&
        soc_apache_ser_test_skip_check(unit, mem)) {
        return FALSE;
    }
#endif
    if (!soc_mem_cache_get(unit, mem, MEM_BLOCK_ALL)) {
    /* By default, we don't want to scan the memories which are not cacheable. 
     *  Exceptions to this rule are specified by chip-specific functions below. 
     */
#if defined(BCM_TOMAHAWK_SUPPORT)
        if (SOC_IS_TOMAHAWKX(unit)) {
            if (soc_th_mem_is_eligible_for_scan(unit, mem)) {
                return TRUE;
            }
        }
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT2PLUS_SUPPORT)
        if (SOC_IS_TD2P_TT2P(unit)) {
            if (soc_trident2p_mem_is_eligible_for_scan(unit, mem)) {
                return TRUE;
            }
        } 
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
#if defined(BCM_MONTEREY_SUPPORT)
        if (SOC_IS_MONTEREY(unit)) {
            if (soc_monterey_mem_is_eligible_for_scan(unit, mem)) {
                return TRUE;
            }
        }
#endif 
#if defined(BCM_APACHE_SUPPORT)
        if (SOC_IS_APACHE(unit)) {
            if (soc_apache_mem_is_eligible_for_scan(unit, mem)) {
                return TRUE;
            }
        }
#endif /* BCM_APACHE_SUPPORT */
#if defined(BCM_ESW_SUPPORT)
        LOG_VERBOSE(BSL_LS_SOC_SER,
                    (BSL_META_U(unit,
                                "sram_scrub: skipping mem %s (NOT_CACHEABLE)\n"),
                    SOC_MEM_NAME(unit, mem)));
#endif /* BCM_ESW_SUPPORT */
        return FALSE;
    }
    return TRUE;
}

/*
 * Function:
 * 	_soc_sram_scan_thread (internal)
 * Purpose:
 *   	Thread for scrubing SRAM parity error
 * Parameters:
 *	unit_vp - StrataSwitch unit # (as a void *).
 * Returns:
 *	Nothing
 * Notes:
 *	Exits when sram_scan_interval is set to zero and semaphore is given.
 */

STATIC void
_soc_sram_scan_thread(void *unit_vp)
{
    int     unit = PTR_TO_INT(unit_vp);
    soc_control_t	*soc = SOC_CONTROL(unit);
    int     rv;
    int     interval;
    int     chunk_size;
    int     entries_interval;
    uint32  *read_buf = NULL;
    soc_mem_t		mem;
    int     blk;
    int     idx, idx_count;
    char    thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t thread;
    int     pipe, num_pipe;
    int     ser_flags[SOC_MAX_NUM_PIPES];
#if defined(ALPM_ENABLE)
    uint8 _alpm_lock = 0;
#endif /* ALPM_ENABLE */

    chunk_size = soc_property_get(unit, spn_SRAM_SCAN_CHUNK_SIZE, 256);

    read_buf = soc_cm_salloc(unit, chunk_size * SOC_MAX_MEM_WORDS * 4,
                             "sram_scan_new");

    if (read_buf == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "soc_sram_scan_thread: not enough memory, exiting\n")));
        soc_event_generate(unit, SOC_SWITCH_EVENT_THREAD_ERROR, 
                           SOC_SWITCH_EVENT_THREAD_SRAMSCAN, __LINE__, 
                           SOC_E_MEMORY);
        goto cleanup_exit;
    }

    /*
     * Implement the sleep using a semaphore timeout so if the task is
     * requested to exit, it can do so immediately.
     */

    sal_memset(&ser_flags, 0, sizeof(ser_flags));
    entries_interval = 0;
    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));
    while ((interval = soc->sram_scan_interval) != 0) {
        for (mem = 0; mem < NUM_SOC_MEM; mem++) {
            if (soc->sram_scan_interval == 0) {
                break;
            }
            if (!_soc_mem_is_eligible_for_sramscan(unit, mem)) {
                continue;
            }
#ifdef BCM_ESW_SUPPORT
            LOG_VERBOSE(BSL_LS_SOC_SER, 
                        (BSL_META_U(unit,
                                    "sram_scrub: now scrubbing mem %s\n"),
                        SOC_MEM_NAME(unit, mem)));
#endif
            SOC_MEM_BLOCK_ITER(unit, mem, blk) {
                if (soc->sram_scan_interval == 0) {
                    break;
                }
                num_pipe = 0; /* re-init num_pipe for every mem */
#if defined(BCM_TOMAHAWK_SUPPORT)
                if (SOC_IS_TOMAHAWKX(unit)) {
                    soc_th_mem_scan_info_get(unit, mem, blk, &num_pipe, ser_flags);
                } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#if defined(BCM_TRIDENT3_SUPPORT)
                if (SOC_IS_TRIDENT3X(unit)) {
                    soc_td3_mem_scan_info_get(unit, mem, blk, &num_pipe, ser_flags);
                } else
#endif
                {
                    _soc_sram_scan_info_get(unit, mem, blk, &num_pipe, ser_flags);
                }
                for (idx = soc_mem_index_min(unit, mem);
                     idx <= soc_mem_index_max(unit, mem);
                     idx += idx_count) {
                    if (soc->sram_scan_interval == 0) {
                        break;
                    }
                    idx_count = soc_mem_index_count(unit, mem) - idx;
                    if (idx_count > chunk_size) {
                        idx_count = chunk_size;
                    }
                    if (entries_interval + idx_count > soc->sram_scan_rate) {
                        idx_count = soc->sram_scan_rate - entries_interval;
                    }

                    if (mem == L2_ENTRY_ONLYm ||
                        mem == L2_ENTRY_ONLY_ECCm) {
                        SOC_L2X_MEM_LOCK(unit);
                    } else if (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
                        mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
                        mem == L3_DEFIP_ALPM_IPV6_128m || mem == L3_DEFIP_ALPM_RAWm ||
                        mem == L3_DEFIP_ALPM_ECCm || mem == L3_DEFIP_AUX_TABLEm) {
                       _ALPM_LOCK(unit);
                    } else {
                        MEM_LOCK(unit, mem);
                    }

                    for (pipe = 0; pipe < num_pipe; pipe++) {
#if defined(BCM_ESW_SUPPORT)
                        LOG_VERBOSE(BSL_LS_SOC_SER,
                                    (BSL_META_U(unit,
                                                "sram_scan: will now scrub mem %s, pipe %d, range %0d - %0d, ser_flags 0x%x\n"),
                                                SOC_MEM_NAME(unit, mem), pipe,
                                                idx, idx + idx_count - 1,
                                                ser_flags[pipe]));

                        if (TRUE == soc_mem_is_shared_mem(unit, mem)) {
                            LOG_VERBOSE(BSL_LS_SOC_SER,
                                        (BSL_META_U(unit,
                                                    "sram_scrub: skipping mem %s (soc_mem_is_shared_mem)\n"),
                                        SOC_MEM_NAME(unit, mem)));
                            continue;
                        }
#endif
#ifdef BCM_TOMAHAWK3_SUPPORT
                        if (SOC_IS_TOMAHAWK3(unit) &&
                            soc_th3_ser_pipe_skip(unit, pipe)) {
                            continue;
                        }
#endif
                        rv = soc_mem_ser_read_range(unit, mem, blk, idx, idx + idx_count - 1,  
                                                    ser_flags[pipe], read_buf);
                        if (rv < 0) {
                            LOG_ERROR(BSL_LS_SOC_COMMON,
                                      (BSL_META_U(unit,
                                                  "AbnormalThreadExit:%s, read failed: %s\n"),
                                       thread_name,
                                       soc_errmsg(rv)));

                            soc_event_generate(unit,
                                               SOC_SWITCH_EVENT_THREAD_ERROR, 
                                               SOC_SWITCH_EVENT_THREAD_SRAMSCAN, 
                                               __LINE__, rv);
                            if (mem == L2_ENTRY_ONLYm ||
                                mem == L2_ENTRY_ONLY_ECCm) {
                                SOC_L2X_MEM_UNLOCK(unit);
                            } else if (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
                                mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
                                mem == L3_DEFIP_ALPM_IPV6_128m || mem == L3_DEFIP_ALPM_RAWm ||
                                mem == L3_DEFIP_ALPM_ECCm || mem == L3_DEFIP_AUX_TABLEm) {
                                _ALPM_UNLOCK(unit);
                            } else
                            {
                                MEM_UNLOCK(unit, mem);
                            }
                            goto cleanup_exit;
                        }
                    }
                    if (mem == L2_ENTRY_ONLYm ||
                        mem == L2_ENTRY_ONLY_ECCm) {
                        SOC_L2X_MEM_UNLOCK(unit);
                    } else if (mem == L3_DEFIP_ALPM_IPV4m || mem == L3_DEFIP_ALPM_IPV4_1m ||
                        mem == L3_DEFIP_ALPM_IPV6_64m || mem == L3_DEFIP_ALPM_IPV6_64_1m ||
                        mem == L3_DEFIP_ALPM_IPV6_128m || mem == L3_DEFIP_ALPM_RAWm ||
                        mem == L3_DEFIP_ALPM_ECCm || mem == L3_DEFIP_AUX_TABLEm) {
                        _ALPM_UNLOCK(unit);
                    } else
                    {
                        MEM_UNLOCK(unit, mem);
                    }

                    entries_interval += num_pipe * idx_count;
            
                    if (entries_interval >= soc->sram_scan_rate) {
                        sal_sem_take(soc->sram_scan_notify, interval);
                        entries_interval = 0;
                    }
                }
            }
        }

        if (soc->sram_scan_interval != 0) {
            /* Extra sleep in main loop, in case no caches are enabled. */
            sal_sem_take(soc->sram_scan_notify, interval);
            entries_interval = 0;
        }
    }
cleanup_exit:
    if (read_buf != NULL) {
        soc_cm_sfree(unit, read_buf);
    }
    soc->sram_scan_pid = SAL_THREAD_ERROR;
    sal_thread_exit(0);
}

#endif    /* BCM_SRAM_SCAN_SUPPORT */
