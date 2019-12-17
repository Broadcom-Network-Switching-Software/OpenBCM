/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 */
#include <sal/core/boot.h>
#include <sal/core/libc.h>
#include <sal/core/sync.h>
#include <shared/alloc.h>
#include <shared/bsl.h>

#if defined(BCM_ESW_SUPPORT) || defined(BCM_DNXF_SUPPORT) || defined(BCM_DNX_SUPPORT)
#include <soc/mcm/cmicx.h>
#include <soc/mcm/intr_iproc.h>
#include <soc/mcm/memregs.h>
#include <soc/intr.h>
#include <soc/cm.h>
#include <soc/cmic.h>
#include <soc/drv.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager_utils.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <soc/iproc.h>
#include <soc/cmicx.h>
#include <shared/cmicfw/iproc_mbox.h>
#include <shared/cmicfw/cmicx_led.h>
#include <shared/cmicfw/cmicx_link.h>

int iproc_m0ssq_init_done[SOC_MAX_NUM_DEVICES] = {0};

uint32 soc_iproc_percore_membase_get(int unit, int ucnum)
{
  if (SOC_IS_HELIX5(unit)) {
      return (IPROC_M0SSQ_TCAM_BASE_HX5 + (ucnum * IPROC_M0SSQ_TCAM_SIZE_HX5));
  } else {
      return (IPROC_M0SSQ_TCAM_BASE + (ucnum * IPROC_M0SSQ_TCAM_SIZE));
  }
}

uint32 soc_iproc_percore_memsize_get(int unit, int ucnum)
{
  if (SOC_IS_HELIX5(unit)) {
      return IPROC_M0SSQ_TCAM_SIZE_HX5;
  } else {
      return IPROC_M0SSQ_TCAM_SIZE;
  }
}

uint32 soc_iproc_sram_membase_get(int unit)
{
  if (SOC_IS_HELIX5(unit)) {
      return IPROC_M0SSQ_SRAM_BASE_HX5;
  } else {
      return IPROC_M0SSQ_SRAM_BASE;
  }
}


/*
 * Function:
 *      soc_iproc_m0ssq_reset_ucore
 * Purpose:
 *      Reset Cortex-M0 in Iproc subsystem quad
 * Parameters:
 *      unit number
 *      ucore number
 * Returns:
 *      SOC_E_xxx
 */
int soc_iproc_m0ssq_reset_ucore(int unit, int ucore, int enable)
{
   uint32 val = 0;
   int rv = SOC_E_NONE;

   LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit,"ucore 0x%x enable 0x%x\n"), ucore, enable));

   switch(ucore) {
       case 0:
           soc_iproc_getreg(unit, soc_reg_addr(unit, U0_M0SS_CONTROLr, REG_PORT_ANY, 0), &val);
           soc_reg_field_set(unit, U0_M0SS_CONTROLr, &val, SOFT_RESETf, enable);
           soc_iproc_setreg(unit, soc_reg_addr(unit, U0_M0SS_CONTROLr, REG_PORT_ANY, 0), val);
           break;
       case 1:
           soc_iproc_getreg(unit, soc_reg_addr(unit, U1_M0SS_CONTROLr, REG_PORT_ANY, 0), &val);
           soc_reg_field_set(unit, U1_M0SS_CONTROLr, &val, SOFT_RESETf, enable);
           soc_iproc_setreg(unit, soc_reg_addr(unit, U1_M0SS_CONTROLr, REG_PORT_ANY, 0), val);
           break;
       case 2:
           soc_iproc_getreg(unit, soc_reg_addr(unit, U2_M0SS_CONTROLr, REG_PORT_ANY, 0), &val);
           soc_reg_field_set(unit, U2_M0SS_CONTROLr, &val, SOFT_RESETf, enable);
           soc_iproc_setreg(unit, soc_reg_addr(unit, U2_M0SS_CONTROLr, REG_PORT_ANY, 0), val);
           break;
       case 3:
           soc_iproc_getreg(unit, soc_reg_addr(unit, U3_M0SS_CONTROLr, REG_PORT_ANY, 0), &val);
           soc_reg_field_set(unit, U3_M0SS_CONTROLr, &val, SOFT_RESETf, enable);
           soc_iproc_setreg(unit, soc_reg_addr(unit, U3_M0SS_CONTROLr, REG_PORT_ANY, 0), val);
           break;
       default:
           rv = SOC_E_PARAM;
    }

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0ssq_reset
 * Purpose:
 *      Reset Iproc Cortex-M0 subsystem quad
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
void soc_iproc_m0ssq_reset(int unit, int enable)
{
   int ucnum;

   for(ucnum = 0; ucnum < MAX_UCORES; ucnum++)
       (void)soc_iproc_m0ssq_reset_ucore(unit, ucnum, enable);
}

/*
 * Function:
 *      soc_iproc_quad_event_thread
 * Purpose:
 *      Thread to handle iproc Cortex-M0 subsystem events
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
void soc_iproc_quad_event_thread(void *unit_vp)
{
    int unit = M0SSQ_DECODE_UNIT(unit_vp);
    int ucnum = M0SSQ_DECODE_UCORE(unit_vp);
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_iproc_m0ssq_control_t *iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_ADD(unit);
    }
#endif

    while (iproc_m0ssq->thread_interval != 0) {
        LOG_DEBUG(BSL_LS_SOC_M0, (BSL_META_U(unit,"soc_iproc_quad_event_thread: sleep %d\n"),
                                                iproc_m0ssq->thread_interval));

        (void) sal_sem_take(iproc_m0ssq->event_sema, sal_sem_FOREVER);
        soc_cmic_intr_enable(unit, SW_PROG_INTR);

        /* Interrupt Handler */
        soc_iproc_msgintr_handler(unit, NULL);
    }

    sal_sem_destroy(iproc_m0ssq->event_sema);
    iproc_m0ssq->thread_interval = 0;
    iproc_m0ssq->thread_pid = NULL;

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREAD_REMOVE(unit);
    }
#endif

    sal_thread_exit(0);
}

/*
 * Function:
 *      soc_iproc_m0ssq_init
 * Purpose:
 *      Initialize iproc Cortex-M0 subsystem
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0ssq_init(int unit)
{
   int rv = SOC_E_NONE;
   soc_control_t       *soc = SOC_CONTROL(unit);
   soc_iproc_m0ssq_control_t *iproc_m0ssq = NULL;
   uint32 interval = IPROC_M0SSQ_THREAD_INTERVAL;
   uint32 max_ucores, ucnum;

   if (iproc_m0ssq_init_done[unit]) {
       return rv;
   }

   /* Init max number of ucores enabled */
   _soc_iproc_fw_config(unit);

   /* Init linkscan firmware config. */

   /* Get max ucores enabled */
   max_ucores = _soc_iproc_num_ucore_get(unit);

   for (ucnum = 0; ucnum < max_ucores; ucnum++) {
       iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

       iproc_m0ssq->event_sema = sal_sem_create("m0ssq_intr", sal_sem_BINARY, 0);
       if (iproc_m0ssq->event_sema == NULL) {
           LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ event sem create failed\n")));
           return SOC_E_MEMORY;
       }

       sal_snprintf(iproc_m0ssq->thread_name, sizeof(iproc_m0ssq->thread_name),
                "IPROC_M0SSQ_EVENT.%d", unit);
       iproc_m0ssq->thread_interval = interval;
       iproc_m0ssq->thread_pid = sal_thread_create(iproc_m0ssq->thread_name,
                                                  SAL_THREAD_STKSZ,
                                                  soc_property_get(unit,
                                                                   spn_LINKSCAN_THREAD_PRI,
                                                                   IPROC_M0SSQ_THREAD_PRI),
                                                  (void (*)(void*))soc_iproc_quad_event_thread,
                                                  M0SSQ_ENCODE_UNIT_UCORE(unit, ucnum));

       if (iproc_m0ssq->thread_pid == SAL_THREAD_ERROR) {
           iproc_m0ssq->thread_interval = 0;
           sal_sem_destroy(iproc_m0ssq->event_sema);
           LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ thread creation failed \n")));
           return SOC_E_MEMORY;
       }
   }

   soc_cmic_intr_enable(unit, SW_PROG_INTR);

   iproc_m0ssq_init_done[unit] = 1;

   return rv;
}

/*
 * Function:
 *      soc_iproc_m0ssq_exit
 * Purpose:
 *      Exit iproc Cortex-M0 subsystem
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0ssq_exit(int unit)
{
    int rv = SOC_E_NONE;
    soc_control_t       *soc = SOC_CONTROL(unit);
    soc_iproc_m0ssq_control_t *iproc_m0ssq;
    uint32 ucnum;

    if (!iproc_m0ssq_init_done[unit]) {
        return rv;
    }

    for (ucnum = 0; ucnum < MAX_UCORES; ucnum++) {
        iproc_m0ssq = &soc->iproc_m0ssq_ctrl[ucnum];

        iproc_m0ssq->thread_interval = 0;

        if (iproc_m0ssq->event_sema) {
            sal_sem_give(iproc_m0ssq->event_sema);
        }
    }

    soc_cmic_intr_disable(unit, SW_PROG_INTR);

    iproc_m0ssq_init_done[unit] = 0;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0_init
 * Purpose:
 *      Initialize iproc Cortex-M0 subsystem and mbox
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0_init(int unit)
{
    int rv = SOC_E_NONE;

    if (SOC_CONTROL(unit)->iproc_m0_init_done) {
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "IPROC M0 init\n")));

    /* Initialize Iproc ARM Cortex-M0 subsystem quad */
    rv = soc_iproc_m0ssq_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ init failed\n")));
        return rv;
    }

    /* Initialize Iproc ARM Cortex-M0 subsystem quad(M0SSQ) mailbox */
    rv = soc_iproc_mbox_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc mbox init failed\n")));
        return rv;
    }

    /* Initialize LED mailbox */
    rv = soc_cmicx_led_mbox_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "LED mbox init failed\n")));
        return rv;
    }

    rv = soc_cmicx_link_fw_config_init(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "LED mbox init failed\n")));
        return rv;
    }

    /* bring M0 core out of reset */
    /* uC0 for Linkscan/LED */
    (void)soc_iproc_m0ssq_reset_ucore(unit, 0, 0);

    SOC_CONTROL(unit)->iproc_m0_init_done = 1;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0_exit
 * Purpose:
 *      Exit iproc Cortex-M0 subsystem and mbox
 * Parameters:
 *      unit number
 * Returns:
 *      None
 */
int soc_iproc_m0_exit(int unit)
{
    int rv = SOC_E_NONE;

    LOG_VERBOSE(BSL_LS_SOC_M0, (BSL_META_U(unit, "IPROC M0 exit\n")));

    if (!SOC_CONTROL(unit)->iproc_m0_init_done) {
        return rv;
    }

    /* Call LED deinit */
    rv = soc_cmicx_led_deinit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "cmicx led deinit failed\n")));
        return rv;
    }

    /* Call Linkscan deinit */
    rv = soc_cmicx_linkscan_hw_deinit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "cmicx linkscan deinit failed\n")));
        return rv;
    }

    /* Cleanup Iproc ARM Cortex-M0 subsystem quad(M0SSQ) mailbox */
    rv = soc_iproc_mbox_exit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc mbox exit failed\n")));
        return rv;
    }

    /* Cleanup Iproc ARM Cortex-M0 subsystem quad */
    rv = soc_iproc_m0ssq_exit(unit);
    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_M0, (BSL_META_U(unit, "Iproc M0SSQ exit failed\n")));
        return rv;
    }

    (void)soc_iproc_m0ssq_reset(unit, M0SSQ_RESET_ENABLE);

    SOC_CONTROL(unit)->iproc_m0_init_done = 0;

    return rv;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_get
 * Purpose:
 *      Get shared memory object by name.
 * Parameters:
 *      unit Device unit number.
 *      name Name of memory object.
 *      shmem Shared memory object.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_get(int unit, char *name, soc_iproc_m0ssq_shmem_t **pshmem)
{
    if (pshmem == NULL) {
        return SOC_E_PARAM;
    }

    if (sal_strncmp(name, "linkscan", 8) == 0) {

        /* Static allocation of linkscan shmem. */
        *pshmem = sal_alloc(sizeof(soc_iproc_m0ssq_shmem_t), "SocIprocM0ssqShmem");
        if (*pshmem == NULL) {
            return SOC_E_MEMORY;
        }

        (*pshmem)->size = CMICX_LS_SHMEM_SIZE;
        (*pshmem)->base = soc_iproc_percore_membase_get(unit, 0) +
                          soc_iproc_percore_memsize_get(unit, 0) -
                          CMICX_LS_SHMEM_OFFSET_DEFAULT;
        (*pshmem)->unit = unit;
        return SOC_E_NONE;
    }

    return SOC_E_FAIL;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_write32
 * Purpose:
 *      Write 32bits data into shared memory.
 * Parameters:
 *      shmem Shared memory object.
 *      offset Offset within memory object.
 *      value 32bits data.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_write32(soc_iproc_m0ssq_shmem_t *shmem,
                              uint32 offset, uint32 value)
{
    uint32 addr;

    if (shmem == NULL) {
        return SOC_E_UNAVAIL;
    }

    if (offset >= shmem->size) {
        return SOC_E_PARAM;
    }

    addr = shmem->base + offset;
    soc_iproc_setreg(shmem->unit, addr, value);

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_iproc_m0ssq_shmem_read32
 * Purpose:
 *      Read 32bits data from shared memory.
 * Parameters:
 *      shmem Shared memory object.
 *      offset Offset within memory object.
 *      value 32bits data.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_read32(soc_iproc_m0ssq_shmem_t *shmem,
                             uint32 offset, uint32 *value)
{
    uint32 addr;

    if (shmem == NULL ||
        value == NULL)
    {
        return SOC_E_UNAVAIL;
    }

    if (offset >= shmem->size) {
        return SOC_E_PARAM;
    }

    addr = shmem->base + offset;

    soc_iproc_getreg(shmem->unit, addr, value);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_iproc_m0ssq_shmem_clear
 * Purpose:
 *      Clear shared memory object as zero.
 * Parameters:
 *      shmem Shared memory object.
 * Returns:
 *      SOC_E_NONE if success, otherwise return SOC_E_XXX.
 */
int
soc_iproc_m0ssq_shmem_clear(soc_iproc_m0ssq_shmem_t *pshmem)
{
    int rv;
    uint32 addr;
    for (addr = 0; addr < pshmem->size; addr += 4) {
        rv = soc_iproc_m0ssq_shmem_write32(pshmem, addr, 0);
        if (SOC_FAILURE(rv)) {
            return rv;
        }
    }
    return SOC_E_NONE;
}

#endif /* BCM_CMICX_SUPPORT */
#endif /* BCM_ESW_SUPPORT */
