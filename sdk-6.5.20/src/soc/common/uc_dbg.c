/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    ukernel_debug.c
 * Purpose:  
 */

#include <stddef.h>             /* for offsetof */

#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/dpc.h>
#include <shared/alloc.h>

#include <soc/drv.h>
#if 0
#include <soc/dcb.h>
#include <soc/dcbformats.h>     /* only for 5670 crc erratum warning */
#include <soc/pbsmh.h>
#include <soc/higig.h>
#include <soc/cm.h>
#include <soc/property.h>
#endif
#include <soc/debug.h>
#include <soc/error.h>
#include <shared/bsl.h>

#if defined(SOC_UKERNEL_DEBUG)
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)

#include <soc/cmicm.h>
#include <soc/shared/mos_intr_common.h>
#include <soc/uc_msg.h>
#include <soc/uc.h>

#include <soc/uc_dbg.h>

_soc_cmic_ucdbg_stats_t cmic_uc_dbg_stat[SOC_MAX_NUM_DEVICES];
int cmic_ucdbg_initialized[SOC_MAX_NUM_DEVICES] = {0};

mos_msg_cmic_ucdbg_t    ucdbg_stat;

#define UCDBG_HOSTLOG_HDR_VALID(pstat, indx)\
            (pstat)->hostlog[(indx)].hdr.h.valid

#define UCBDG_HOSTLOG_BUFF(pstat, indx)\
            (pstat)->hostlog[(indx)].log

#define UCBDG_CMICM_ADDR(base, field) \
    ((uint32) ((base) + offsetof(mos_msg_cmic_ucdbg_t,field)))

#define UCBDG_CMICM_HDR_WORD_ADDR(base, indx) \
    ((uint32) ((base) + offsetof(mos_msg_cmic_ucdbg_t,hdr.word[(indx)])))

STATIC
char *mos_msg_ucdbg_mod_state[] = {
    "_bcmUcDbgUcUnInitialized",
    "_bcmUcDbgUcSP408InitComplete",
    "_bcmUcDbgUcUARTInitComplete",
    "_bcmUcDbgUcCmicIRQInitComplete",
    "_bcmUcDbgUcTimesyncIRQInitComplete",
    "_bcmUcDbgUcMosDMAInitComplete",
    "_bcmUcDbgUcPKTDMAInitComplete",
    "_bcmUcDbgUcSBUSDMAInitComplete",
    "_bcmUcDbgUcMoSMsgInitStarted",
    "_bcmUcDbgUcBoardInitComplete",
    "_bcmUcDbgUcWaitingForHostSync",
    "_bcmUcDbgUcRunning",
    "_bcmUcDbgUcDMAFail",
    "_bcmUcDbgHostInitialized",
    "_bcmUcDbgHostRunning",
};


/* array of pointers to log buffer entry */
mos_msg_ucdbg_entry_t **puclog_buffarray = NULL; 

/* These are addresses in the uC SRAM and not directly addresable by SDK 
- they are addresses for PCI Operatins */
STATIC uint32 
_soc_cmic_ucdebug_shared_sram_base(int unit)
{
    uint32 ucdbg_msg_base = 0;

    ucdbg_msg_base = soc_cmic_timestamp_base(unit) + sizeof(mos_msg_timestamps_t);
    return (ucdbg_msg_base);
}

STATIC void
_soc_cmic_ucdebug_dump_sram_info(int unit)
{
    uint32                  ucdbg_msg_base = 0, tmp = 0, tmp1 = 0;
    uint32  debug[12];

    ucdbg_msg_base = _soc_cmic_ucdebug_shared_sram_base(unit);

    tmp = soc_uc_mem_read(unit, UCBDG_CMICM_ADDR(ucdbg_msg_base, puclog_buff));
    tmp1= soc_ntohl(tmp);
    bsl_printf("%s Got puclog_buff: %08x \n", __FUNCTION__, tmp1);

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (0 * 4)));
    debug[0]= soc_ntohl(tmp);
    bsl_printf("debug[0]: %08x \n", debug[0]);

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (1 * 4)));
    debug[1]= soc_ntohl(tmp);
    bsl_printf("debug[1]: %08x \n", debug[1]);

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (2 * 4)));
    debug[2]= soc_ntohl(tmp);
    bsl_printf("debug[2]: %08x \n", debug[2]);

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (3 * 4)));
    debug[3] = soc_ntohl(tmp);
    bsl_printf("debug[3]: %08x \n", debug[3]);
    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (4 * 4)));
    debug[4] = soc_ntohl(tmp);
    bsl_printf("debug[4]: %08x \n", debug[4]);
    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (5 * 4)));
    debug[5] = soc_ntohl(tmp);
    bsl_printf("debug[5]: %08x \n", debug[5]);
    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (6 * 4)));
    debug[6] = soc_ntohl(tmp);
    bsl_printf("debug[6]: %08x \n", debug[6]);
    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (7 * 4)));
    debug[7] = soc_ntohl(tmp);
    bsl_printf("debug[7]: %08x \n", debug[7]);
    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (8 * 4)));
    debug[8] = soc_ntohl(tmp);
    bsl_printf("debug[8]: %08x \n", debug[8]);
    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (9 * 4)));
    debug[9] = soc_ntohl(tmp);
    bsl_printf("debug[9]: %08x \n", debug[9]);

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (10 * 4)));
    debug[10] = soc_ntohl(tmp);
    bsl_printf("debug[10]: %08x \n", debug[10]);

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (11 * 4)));
    debug[11] = soc_ntohl(tmp);
    bsl_printf("debug[11]: %08x \n", debug[11]);
}

STATIC int 
_soc_cmic_ucdebug_allocate_shared_memory(int unit)
{
    int                         logbuffarray_size = 0;
    uint32                      ucdbg_msg_base = 0, tmp = 0;
    _soc_cmic_ucdbg_stats_t     *phostdbg_stat = &cmic_uc_dbg_stat[unit];
    mos_msg_cmic_ucdbg_t        *pucdbg_cache = NULL;
    
    pucdbg_cache = &phostdbg_stat->ucdbg_cache;

    /* ukernel log entries are given as a array of pointers to UC
     * mostly following the following contruct :
     * mos_msg_ucdbg_entry_t  *uclog[MOS_UCDBG_MAX_LOG_ENTRIES_UC]
     */

    logbuffarray_size = sizeof(mos_msg_ucdbg_entry_t*) *    
                        MOS_UCDBG_MAX_LOG_ENTRIES_UC;

    puclog_buffarray = soc_cm_salloc(unit, logbuffarray_size, 
                                     "uC log ent ptrs"); 
    if (puclog_buffarray == NULL) {
        bsl_printf("%s(): Error in allocating dmable buff for UC log ..\n",
                   FUNCTION_NAME());
        return (0);
    }

    for (tmp=0; tmp < MOS_UCDBG_MAX_LOG_ENTRIES_UC; tmp++) {

        /* keeping host side ptr cache of uc log buffers */
        phostdbg_stat->puclog[tmp] = soc_cm_salloc(unit, 
                                   sizeof(mos_msg_ucdbg_entry_t), "uc log ent");
        if (phostdbg_stat->puclog[tmp] == NULL) {
            bsl_printf("dmable memory allocation failure for uclog indx:%d \n",
                   tmp);
            goto exit;
        }

        memset((void*)phostdbg_stat->puclog[tmp], 0x0,
               sizeof(mos_msg_ucdbg_entry_t));
        soc_cm_sflush(unit, phostdbg_stat->puclog[tmp],
                      sizeof(mos_msg_ucdbg_entry_t));
        soc_cm_sinval(unit, phostdbg_stat->puclog[tmp],
                      sizeof(mos_msg_ucdbg_entry_t));
        puclog_buffarray[tmp] = (mos_msg_ucdbg_entry_t *)INT_TO_PTR(soc_htonl(soc_cm_l2p(unit, phostdbg_stat->puclog[tmp])));
   }

    soc_cm_sflush(unit, puclog_buffarray, logbuffarray_size);
    soc_cm_sinval(unit, puclog_buffarray, logbuffarray_size);

    pucdbg_cache->puclog_buff = puclog_buffarray;

    /* Putting the array of log buffers location (puclog_buffarray)
     *  to shared memory uC SRAM
     */
    ucdbg_msg_base = _soc_cmic_ucdebug_shared_sram_base(unit);
    soc_uc_mem_write(unit, UCBDG_CMICM_ADDR(ucdbg_msg_base, puclog_buff),
                     soc_htonl(soc_cm_l2p(unit, puclog_buffarray)));

exit:

    return (0);

}

STATIC void
_soc_cmic_ucdebug_thread(void *unit_vp)
{
    int unit = PTR_TO_INT(unit_vp);
    uint32                      ucdbg_msg_base = 0, tmp = 0;
    _soc_cmic_ucdbg_stats_t     *phostdbg_stat = &cmic_uc_dbg_stat[unit];
    mos_msg_cmic_ucdbg_t        *pucdbg_cache = NULL;
    
    pucdbg_cache = &phostdbg_stat->ucdbg_cache;
    ucdbg_msg_base = _soc_cmic_ucdebug_shared_sram_base(unit);

    while (1) {
        
        tmp = soc_uc_mem_read(unit, 
            UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, UCDBG_OFFSET_STATE_UC));
        pucdbg_cache->hdr.s.state_uc =  soc_ntohl(tmp);

        if(pucdbg_cache->hdr.s.state_uc == _bcmUcDbgUcWaitingForHostSync) {

            _soc_cmic_ucdebug_allocate_shared_memory(unit);

            pucdbg_cache->hdr.s.capture_mode = _bcmUcDbgLogCaptureFreerun;
            pucdbg_cache->hdr.s.state_host = _bcmUcDbgHostRunning;


             /* ucdbg_msg_base in uC SRAM - not directly addressable by SDK, 
              * needs to be accessed via PCI operations 
              */
            ucdbg_msg_base = _soc_cmic_ucdebug_shared_sram_base(unit);

            /* by defualt capture mode is _bcmUcDbgLogCaptureFreerun */
            soc_uc_mem_write(unit, 
             UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, UCDBG_OFFSET_CAPTURE_MODE),
             (uint32)soc_htonl(pucdbg_cache->hdr.s.capture_mode));

             /* setting the host state tp _bcmUcDbgHostRunning */
            soc_uc_mem_write(unit, 
             UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, UCBDG_OFFSET_STATE_HOST),
             (uint32)soc_htonl(pucdbg_cache->hdr.s.state_host));

            /* for now closing the thread since memory init is complete */
            break;
                    
        }
#if 0
        else {
           _soc_cmic_ucdebug_dump_sram_info(unit);
        }
#endif
        
        /* sleeping for sometime*/
        sal_sleep(10);
        
    }
    /* ukernel debug thread closing */
}

int
soc_cmic_ucdebug_init(int unit, int uC)
{
    uint32                      ucdbg_msg_base = 0;
    _soc_cmic_ucdbg_stats_t     *phostdbg_stat = &cmic_uc_dbg_stat[unit];
    mos_msg_cmic_ucdbg_t        *pucdbg_cache = NULL;

    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return (SOC_E_UNAVAIL);
    }

    if (uC != 1) {
        return (SOC_E_NONE);
    }


    pucdbg_cache = &phostdbg_stat->ucdbg_cache;

    /* host side memory initialization */

    phostdbg_stat->flags = 0;
    phostdbg_stat->sw_intr_count_uc0 = 0;
    phostdbg_stat->sw_intr_count_uc1 = 0;
    phostdbg_stat->hostlog_indx_wr = 0;
    phostdbg_stat->hostlog_indx_rd = 0;

    /* ukernel side data structure initialization */
    memset(pucdbg_cache, 0x0, sizeof(mos_msg_cmic_ucdbg_t));

    /* host side uC sRAM needs to be reinited after uC starts running */
    /* setting it to 0 instead of init since uc init sets SRAM to zero */
    pucdbg_cache->hdr.s.state_host = 0; 
    pucdbg_cache->hdr.s.rdindx = 0;
    pucdbg_cache->hdr.s.filter_mask = 0;
    pucdbg_cache->hdr.s.state_uc = _bcmUcDbgUcUnInitialized;
    pucdbg_cache->hdr.s.capture_mode = _bcmUcDbgLogCaptureFreerun; 


    /* ucdbg_msg_base in uC SRAM - not directly addressable by SDK, needs to be
    accessed via PCI operations */
    ucdbg_msg_base = _soc_cmic_ucdebug_shared_sram_base(unit);

    soc_uc_mem_write(unit, UCBDG_CMICM_ADDR(ucdbg_msg_base, puclog_buff), 0);

    soc_uc_mem_write(unit,
        UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, UCBDG_OFFSET_STATE_HOST),
        (uint32)soc_htonl(pucdbg_cache->hdr.s.state_host));
    soc_uc_mem_write(unit, 
        UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, UCDBG_OFFSET_STATE_UC),
        (uint32)soc_htonl(pucdbg_cache->hdr.s.state_uc));

    soc_uc_mem_write(unit, UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, 2), 0); 
    soc_uc_mem_write(unit, UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, 3), 0);
    soc_uc_mem_write(unit, UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, 4), 0);
    soc_uc_mem_write(unit, UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, 5), 0);
    soc_uc_mem_write(unit, UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, 6), 0);
    soc_uc_mem_write(unit, UCBDG_CMICM_HDR_WORD_ADDR(ucdbg_msg_base, 7), 0);

    if (cmic_ucdbg_initialized[unit]) {
        bsl_printf("UKENREL DBG module already initlaized\n");
        return (SOC_E_NONE);
    }

    /* create uc initialization monitoring thread */
    sal_thread_create("cmic uc dbg thrd", SAL_THREAD_STKSZ, 150, 
                    _soc_cmic_ucdebug_thread, INT_TO_PTR(unit));

    cmic_ucdbg_initialized[unit] = 1;

    return (SOC_E_NONE); 
}

void
soc_cmic_ucdebug_deinit(int unit)
{
    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return;
    }
}

int
soc_cmic_ucdebug_log_add(int unit, mos_msg_ucdbg_logtype_t logtype,
                      const char *format, ...)
{
    va_list vargs;
    char *buff = NULL;
    _soc_cmic_ucdbg_stats_t *phostdbg_stat = &cmic_uc_dbg_stat[unit];
    int bufflen = 0;

    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return (SOC_E_UNAVAIL);
    }
    if (!cmic_ucdbg_initialized[unit]) {
        return (SOC_E_INIT);
    }


    buff = UCBDG_HOSTLOG_BUFF(phostdbg_stat, phostdbg_stat->hostlog_indx_wr);

    memset(buff, 0x0, MOS_UCDBG_LOG_BUFF_SIZE);

    switch(logtype) {
        case _bcmCmicUcDbgLogHostMcsLoad:
            sal_sprintf(buff, "%s" , "HostMcsLoad: ");
            break;
        case _bcmCmicUcDbgLogHostUcMsg:
            sal_sprintf(buff, "%s", "HostUcMsg  : ");
            break;
        case _bcmCmicUcDbgLogHostPtpRx:
            sal_sprintf(buff, "%s", "HostPtpRx  : ");
            break;
        case _bcmCmicUcDbgLogHostPtp:
            sal_sprintf(buff, "%s", "HostPtp    : ");
            break;
        default:
            sal_sprintf(buff, "%s", "Host       : ");
    }

    bufflen = sal_strlen(buff);


    va_start(vargs, format);
    sal_vsnprintf(buff +  bufflen, (MOS_UCDBG_LOG_BUFF_SIZE - bufflen), format, vargs);
    va_end(vargs);

    phostdbg_stat->hostlog_indx_wr = 
        (phostdbg_stat->hostlog_indx_wr  + 1) % MOS_UCDBG_MAX_LOG_ENTRIES_HOST;

    return (SOC_E_NONE);

}

int
soc_cmic_ucdebug_sw_intr_count_inc(int unit, uint32 rupt_num)
{
     _soc_cmic_ucdbg_stats_t *phostdbg_stat = &cmic_uc_dbg_stat[unit];

    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return (SOC_E_UNAVAIL);
    }
    if (!cmic_ucdbg_initialized[unit]) {
        return (SOC_E_INIT);
    }


    switch(rupt_num) {
    case CMICM_SW_INTR_UC0:
        phostdbg_stat->sw_intr_count_uc0++;
        break;
    case CMICM_SW_INTR_UC1:
        phostdbg_stat->sw_intr_count_uc1++;
        break;
    default:
        break;
    }
    return (SOC_E_NONE);
}

int
soc_cmic_ucdebug_core_communication_status(int unit, unsigned int flag)
{
    if (!cmic_ucdbg_initialized[unit]) {
        return (SOC_E_INIT);
    }

    _soc_cmic_ucdbg_stats_t *phostdbg_stat = &cmic_uc_dbg_stat[unit];
    phostdbg_stat->flags |= flag;

    return (SOC_E_INIT);
}

STATIC void
_soc_cmic_ucddebug_output_show(void* owner, void* time_as_ptr, void *unit_as_ptr, 
                           void *unused_2, void* unused_3)
{
    sal_usecs_t callback_time = (sal_usecs_t)(size_t)time_as_ptr;
    int unit = (int)(size_t)unit_as_ptr;
    int tmp = 0;
    mos_msg_cmic_ucdbg_t    *pucdbg_cache =NULL;
    _soc_cmic_ucdbg_stats_t *phostdbg_stat = &cmic_uc_dbg_stat[unit];
    uint32 ucdbg_msg_base = 0;
    
    ucdbg_msg_base = _soc_cmic_ucdebug_shared_sram_base(unit);
    pucdbg_cache = &phostdbg_stat->ucdbg_cache;

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (UCDBG_OFFSET_STATE_UC * 4)));
    if (pucdbg_cache->hdr.s.state_uc !=  soc_ntohl(tmp)) {
        pucdbg_cache->hdr.s.state_uc =  soc_ntohl(tmp);
        bsl_printf("Ukernel State               : %s\n", 
            (pucdbg_cache->hdr.s.state_uc >= _bcmUcDbgMaxState) 
            ? "Unknown State" : mos_msg_ucdbg_mod_state[pucdbg_cache->hdr.s.state_uc]);
    }
 
    /* SDK side log output */
    for (tmp = phostdbg_stat->hostlog_indx_rd; tmp < phostdbg_stat->hostlog_indx_wr; tmp++) {
        bsl_printf("%s", phostdbg_stat->hostlog[tmp].log);
        phostdbg_stat->hostlog_indx_rd++;
    }

    /* UKERNEL side log output */
    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (UCDBG_OFFSET_WR_INDX * 4)));
    pucdbg_cache->hdr.s.wrindx = soc_ntohl(tmp);
    for (tmp = pucdbg_cache->hdr.s.rdindx; tmp < pucdbg_cache->hdr.s.wrindx; tmp++) {
        if (phostdbg_stat->puclog[tmp] != NULL) {
            soc_cm_sinval(unit, phostdbg_stat->puclog[tmp], sizeof(mos_msg_ucdbg_entry_t));
            bsl_printf("%s", phostdbg_stat->puclog[tmp]->log);
        }
        else {
            bsl_printf("phostdbg_stat->uclog[tmp] is NULL\n");
        }
        pucdbg_cache->hdr.s.rdindx++;
    }
    if (callback_time) {
        sal_dpc_time(callback_time, &_soc_cmic_ucddebug_output_show, 
                    INT_TO_PTR(&_soc_cmic_ucddebug_output_show), time_as_ptr, 
                    unit_as_ptr, 0, 0);
    }
 
}

int
soc_cmic_ucdebug_dump_start(int unit)
{
    sal_usecs_t callback_time = CMIC_UCDEBUG_OUTPUT_DISP_TIMEOUT;

    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return (SOC_E_UNAVAIL);
    }
    if (!cmic_ucdbg_initialized[unit]) {
        return (SOC_E_FAIL);
    }
    sal_dpc_cancel(INT_TO_PTR(&_soc_cmic_ucddebug_output_show));

    _soc_cmic_ucddebug_output_show(INT_TO_PTR(&_soc_cmic_ucddebug_output_show), 
                               INT_TO_PTR(callback_time), INT_TO_PTR(unit), 0, 0);
    return (SOC_E_NONE);

}

int
soc_cmic_ucdebug_dump_stop(int unit)
{
    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return (SOC_E_UNAVAIL);
    }
    if (!cmic_ucdbg_initialized[unit]) {
        return (SOC_E_FAIL);
    }
    sal_dpc_cancel(INT_TO_PTR(&_soc_cmic_ucddebug_output_show));
    return (SOC_E_NONE);
}

int
soc_cmic_ucdebug_status(int unit)
{
    _soc_cmic_ucdbg_stats_t *phostdbg_stat = &cmic_uc_dbg_stat[unit];
    mos_msg_cmic_ucdbg_t        *pucdbg_cache = NULL;
    uint32 tmp = 0;
    uint32 ucdbg_msg_base = 0;
    uint32 host_index = 0, area_in, area_out;
    
    pucdbg_cache = &phostdbg_stat->ucdbg_cache;
 
    if(!soc_feature(unit, soc_feature_ukernel_debug)) {
        return (SOC_E_UNAVAIL);
    }
    if (!cmic_ucdbg_initialized[unit]) {
        return (SOC_E_FAIL);
    }

    ucdbg_msg_base = _soc_cmic_ucdebug_shared_sram_base(unit);

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (UCBDG_OFFSET_STATE_HOST * 4)));
    pucdbg_cache->hdr.s.state_host = soc_ntohl(tmp);

    tmp = soc_uc_mem_read(unit, (ucdbg_msg_base+ (UCDBG_OFFSET_STATE_UC * 4)));
    pucdbg_cache->hdr.s.state_uc = soc_ntohl(tmp);

    bsl_printf("Host State                  : %s\n", 
        (pucdbg_cache->hdr.s.state_host >= _bcmUcDbgMaxState) 
        ? "Unknown State" : mos_msg_ucdbg_mod_state[pucdbg_cache->hdr.s.state_host]);
    bsl_printf("Ukernel State               : %s\n", 
        (pucdbg_cache->hdr.s.state_uc >= _bcmUcDbgMaxState) 
        ? "Unknown State" : mos_msg_ucdbg_mod_state[pucdbg_cache->hdr.s.state_uc]);
    bsl_printf("Software Interrupt Count uc0: %d\n", phostdbg_stat->sw_intr_count_uc0);
    bsl_printf("Software Interrupt Count uc1: %d\n", phostdbg_stat->sw_intr_count_uc1);

    for (host_index = 0; host_index < (NUM_UCS + 2); host_index++) {
        tmp = soc_uc_mem_read(unit, (uint32)(
                UCBDG_CMICM_ADDR(ucdbg_msg_base, msg_host) + 
                host_index * sizeof(mos_msg_ucbdg_msg_area_t)));
        area_in = soc_ntohl(tmp);
        tmp = soc_uc_mem_read(unit, (uint32)(
                UCBDG_CMICM_ADDR(ucdbg_msg_base, msg_host) + 
                (host_index * sizeof(mos_msg_ucbdg_msg_area_t))
                + sizeof(uint32)));
        area_out = soc_ntohl(tmp);
        bsl_printf("uC1 SRAM host index:%d  area_in : 0x%x area_out : 0x%x\n",
                     host_index, area_in, area_out);
    }

    tmp = soc_uc_mem_read(unit, (uint32)(
                    UCBDG_CMICM_ADDR(ucdbg_msg_base, msg_area_base)));
    bsl_printf("uc: msg area base                     : 0x%x\n", 
                 soc_ntohl(tmp));

    bsl_printf("Host: Ukernel debug module base sram  : 0x%x\n",
                 ucdbg_msg_base);
    tmp = soc_uc_mem_read(unit, (uint32)(
                    UCBDG_CMICM_ADDR(ucdbg_msg_base, ucdbg_msg_base)));
    bsl_printf("UC: uc debug module SRAM msg area base: 0x%x\n",
                 soc_ntohl(tmp));

    tmp = soc_uc_mem_read(unit, (uint32)(
                    UCBDG_CMICM_ADDR(ucdbg_msg_base, uc_processor_num)));
    bsl_printf("uC image seen processor number        : 0x%x\n",
                 soc_ntohl(tmp));

    return (SOC_E_NONE);

}
#else
typedef int uc_dbg_c_file_not_empty; /* Make ISO compilers happy. */
#endif /* #if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT) */
#endif

