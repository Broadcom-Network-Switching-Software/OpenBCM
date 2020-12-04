/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        iproc_fwconfig.h
 * Purpose:     iproc M0SSQ resource configuration to run application on M0SSQ
 */
#ifndef __IPROC_FWCONFIG_H__
#define __IPROC_FWCONFIG_H__

/*Per ucore register offsets*/
/* Store ucnum for each ucore at last
 * 4 bytes of Code segment
 */
#define CPU_ID_OFFSET           0x3FFC
#define DEVID_ID_OFFSET         0x37F0

/*Registers defined in M0SSQ shmem*/
#define U0_MBOX_BASE_OFFSET                0x0
#define U0_MBOX_SIZE_OFFSET                0x4
#define U1_MBOX_BASE_OFFSET                0x8
#define U1_MBOX_SIZE_OFFSET                0xc
#define U2_MBOX_BASE_OFFSET                0x10
#define U2_MBOX_SIZE_OFFSET                0x14
#define U3_MBOX_BASE_OFFSET                0x18
#define U3_MBOX_SIZE_OFFSET                0x1c
#define MBOX_INTERRUPT_MODE_OFFSET         0x20
#define MAX_IPROC_MBOX_PER_CORE_OFFSET     0x24

/* Include U0 debug mechanism. */
#define U0_DEBUG


/* 0x80 ~ 0x90 is temporiray resevered for U0 debug purpose. */
#define U0_DBG                      0x80
#define U0_DBG_SIZE                 0x10

/* Enable U0 debug mechanism such as log and debug count. */
#define FW_TAG                      0x80
#define FW_SYSTICK_CNT              0x84
#define FW_BGLOOP_CNT               0x88
#define FW_DBG_EN                   0x8c
#define FW_DBG_EN_MAGIC             0xA9999999
#define FW_TAG_START                0xABCDDCBA
#define FW_TAG_STOP                 0xDEADBEEF

/*
   0x90 ~ 0xCF is temporiray resevered for U0 debug counter
   total 16 * u32. 12 counters and 4 flags for debug purpose
*/
#define FW_DBG_COUNTER_OFFSET       0x90
#define FW_DBG_COUNTER_SIZE         0x40
typedef struct fw_dbg_counter_s
{
    uint32 counters[12];
    uint32 flags[4];
} fw_dbg_counter_t;



/* Temporiary debug buffer. */
#define U0_LOG_BUFFER             (24 * 1024)
#define U0_LOG_BUFFER_SIZE        (8 * 1024)
#define FW_LOG_MAGIC              (0x464C4F47)
typedef struct {
    uint32 magic;
    uint32 ctrl;
    uint32 base;
    uint32 size;
    uint32 rp;
    uint32 wp;
} fwlog_t;

#define MAX_IPROC_MBOX_TYPES         2
#define M0SSQ_SHMEM_SIZE             (128 * 1024)
#define M0SSQ_SRAM_FWREG_SIZE        256
#define M0SSQ_SHMEM_MBOX_BASE        (IPROC_M0SSQ_SRAM_BASE + M0SSQ_SRAM_FWREG_SIZE)

/******************************************************************************/
/* Following are the configurable per unit parameters based on application requirement */
#define M0SSQ_SHMEM_MBOX_SIZE            (4096*4)
#define MAX_UCORES_ENABLED               1
#define MAX_IPROC_MBOX_SHIFT             2
#define NUM_MBOX_PER_UCORE               (1 << MAX_IPROC_MBOX_SHIFT)
#define MAX_IPROC_MBOX_MASK              ((1 << MAX_IPROC_MBOX_SHIFT) - 1)
#define U0_MAX_MBOXS                     2
#define U1_MAX_MBOXS                     0
#define U2_MAX_MBOXS                     0
#define U3_MAX_MBOXS                     0
#define MBOX_INTERRUPT_ENABLE            0

#if (U0_MAX_MBOXS > NUM_MBOX_PER_UCORE)
#error "IPROC Mbox Configuration Error : U0_MAX_MBOXS > NUM_MBOX_PER_UCORE"
#endif /* (U0_MAX_MBOXS > NUM_MBOX_PER_UCORE) */

#if (U1_MAX_MBOXS > NUM_MBOX_PER_UCORE)
#error "IPROC Mbox Configuration Error : U0_MAX_MBOXS > NUM_MBOX_PER_UCORE"
#endif /* (U1_MAX_MBOXS > NUM_MBOX_PER_UCORE) */

#if ((M0SSQ_SHMEM_MBOX_SIZE *  MAX_UCORES_ENABLED * MAX_IPROC_MBOX_TYPES) > (M0SSQ_SHMEM_SIZE - M0SSQ_SRAM_FWREG_SIZE))
#error "IPROC Mbox Configuration Error : Mailbox size exceeding the total SRAM size"
#endif /* M0SSQ_SHMEM_MBOX_SIZE */
/******************************************************************************/

#ifndef CMICX_FW_BUILD
#define MAX_IPROC_MBOXS          (NUM_MBOX_PER_UCORE * MAX_UCORES)
#define HOST_TO_LE32(data)       htol32(data)
#define LE32_TO_HOST(data)       ltoh32(data)

#define LOCK_FOREVER                    sal_mutex_FOREVER
#define RESOURCE_LOCK(lock, timeout)    sal_mutex_take(lock, timeout)
#define RESOURCE_UNLOCK(lock)           sal_mutex_give(lock)
#define soc_iproc_alloc(size)           sal_alloc(size, "Iproc buffer")
#define soc_iproc_free(ptr)             sal_free(ptr)
#define soc_iproc_memcpy                memcpy

#else
#ifdef SOC_MAX_NUM_DEVICES
#undef SOC_MAX_NUM_DEVICES
#define SOC_MAX_NUM_DEVICES      1
#endif

#define MAX_IPROC_MBOXS          (NUM_MBOX_PER_UCORE * MAX_UCORES)
#define HOST_TO_LE32(data)       (data)
#define LE32_TO_HOST(data)       (data)

#define LOCK_FOREVER                    1
#define RESOURCE_LOCK(lock, timeout)    { }
#define RESOURCE_UNLOCK(lock)           { }

#define WRITE_UCORE_NUM(ucnum)            soc_m0ssq_tcam_write32(CPU_ID_OFFSET, ucnum)
#define WRITE_MBOX_BASE(ucnum, val)       soc_m0ssq_sram_write32((U0_MBOX_BASE_OFFSET + (ucnum * 8)), val)
#define WRITE_MBOX_SIZE(ucnum, val)       soc_m0ssq_sram_write32((U0_MBOX_SIZE_OFFSET + (ucnum * 8)), val)
#define GET_UCORE_NUM()                   soc_m0ssq_tcam_read32(CPU_ID_OFFSET)
#define GET_MBOX_BASE(ucnum)              soc_m0ssq_sram_read32(U0_MBOX_BASE_OFFSET + (ucnum * 8))
#define GET_MBOX_SIZE(ucnum)              soc_m0ssq_sram_read32(U0_MBOX_SIZE_OFFSET + (ucnum * 8))
#endif /* !CMICX_FW_BUILD */

#define UC_APPID_SHIFT           8
#define UC_APPID_MASK            MAX_IPROC_MBOX_MASK
#define UC_APPID(ucnum, app_id)  ((ucnum << UC_APPLICATION_ID_SHIFT) | (app_id & UC_APPLICATION_ID_MASK))
#define UC_APPID_TO_UCNUM(uc_app_id)  (uc_app_id >> UC_APPID_SHIFT)
#define UC_APPID_TO_MBOX(uc_app_id)   (uc_app_id & UC_APPID_MASK)

/* Application IDs */
typedef enum soc_iproc_u0m0_apps_e {
   U0_LED_APP = 0,
   U0_LINKSCAN_APP = 1
} soc_iproc_u0m0_apps_t;

typedef enum soc_iproc_u1m0_apps_e {
   U1_LINKSCAN_APP = 0
} soc_iproc_u1m0_apps_t;

extern int soc_iproc_dbg_enable(void);

#endif /* __IPROC_FWCONFIG_H__ */
