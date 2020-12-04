/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        iproc_m0ssq.h
 * Purpose:     iproc M0SSQ read and write tcam and sram from M0 and host.
 */
#ifndef __IPROC_M0SSQ_H__
#define __IPROC_M0SSQ_H__
#ifndef CMICX_FW_BUILD
#include <soc/iproc.h>
#include <sal/core/thread.h>
#endif
#if defined(BCM_CMICX_SUPPORT) || defined(CMICX_FW_BUILD)

#define IPROC_MDIO_SYS_BASE_HX5              0x10019000
#define MIIM_PHY_LINK_STATUS0_OFFSET_HX5     0x174
#define IPROC_MDIO_SYS_BASE                  0x1319000
#define MIIM_PHY_LINK_STATUS0_OFFSET         0x174
#define M0SSQ_RESET_ENABLE                   1
#ifdef CMICX_FW_BUILD
#define IPROC_M0SSQ_SRAM_BASE            0x1340000
/* 64KB is reserved by IFA CC Eapp */
#define IPROC_M0SSQ_SRAM_SIZE            (64 * 1024)
#define IPROC_M0SSQ_SRAM_BASE_HX5        0x10040000
#define IPROC_M0SSQ_SRAM_SIZE_HX5        (128 * 1024)
#define IPROC_M0SSQ_TCAM_BASE_HX5        0x10000000
#define IPROC_M0SSQ_TCAM_SIZE_HX5        (32 * 1024)
#define IPROC_M0SSQ_TCAM_BASE            0x0000
#define IPROC_M0SSQ_TCAM_SIZE            (32 * 1024)
#define IPROC_M0SSQ_CORE_OFFSET          0x10000
#define IPROC_M0SSQ_PERCORE_REGBASE      0x8000
#define IPROC_M0SSQ_PERCORE_PRIPORT_BASE 0x9000
#else
#define IPROC_M0SSQ_SRAM_BASE_HX5        0x10040000
#define IPROC_M0SSQ_SRAM_SIZE_HX5        (128 * 1024)
#define IPROC_M0SSQ_SRAM_BASE            0x1340000
/* 64KB is reserved by IFA CC Eapp */
#define IPROC_M0SSQ_SRAM_SIZE            (64 * 1024)
#define IPROC_M0SSQ_TCAM_BASE_HX5        0x10000000
#define IPROC_M0SSQ_TCAM_SIZE_HX5        (32 * 1024)
#define IPROC_M0SSQ_TCAM_BASE            0x1300000
#define IPROC_M0SSQ_TCAM_SIZE            (32 * 1024)
#define IPROC_M0SSQ_PERCORE_OFFSET       0x10000
#define IPROC_M0SSQ_PERCORE_REGBASE      0x1380000
#define IPROC_M0SSQ_PERCORE_PRIPORT_BASE 0x1390000
#endif /* CMICX_FW_BUILD */

#define MAX_UCORES                       4

typedef struct {

  /* Device unit number. */
  int unit;

  /* Base address of shared memory. */
  uint32 base;

  /* Size of shared memory. */
  uint32 size;

} soc_iproc_m0ssq_shmem_t;


#ifndef CMICX_FW_BUILD
extern uint32 soc_iproc_percore_membase_get(int unit, int ucnum);
extern uint32 soc_iproc_sram_membase_get(int unit);

typedef struct {
    VOL sal_thread_t thread_pid;         /* Cortex-M0 pid */
    char thread_name[16];
    VOL  sal_usecs_t thread_interval;
    uint32 flags;
    sal_sem_t event_sema;
} soc_iproc_m0ssq_control_t;

#define IPROC_M0SSQ_THREAD_INTERVAL   150
#define IPROC_M0SSQ_THREAD_PRI  50

/* Macros for passing unit + ucore numbers to the threads */
#define M0SSQ_ENCODE_UNIT_UCORE(unit, ucore)    INT_TO_PTR((unit) << 16 | (ucore))
#define M0SSQ_DECODE_UNIT(val)                  ((int)(PTR_TO_INT(val) >> 16))
#define M0SSQ_DECODE_UCORE(val)                 ((int)(PTR_TO_INT(val) & 0xffff))


static __inline__ void soc_m0ssq_tcam_write32(int unit, int ucnum, int offset, uint32 val)
{
   uint32 base;

   base = soc_iproc_percore_membase_get(unit, ucnum);
   soc_iproc_setreg(unit, (base + offset), val);
}

static __inline__ uint32 soc_m0ssq_tcam_read32(int unit, int ucnum, int offset)
{
   uint32 base;
   uint32 val;

   base = soc_iproc_percore_membase_get(unit, ucnum);
   soc_iproc_getreg(unit, (base + offset), &val);
   return val;
}

static __inline__ void soc_m0ssq_sram_write32(int unit, int offset, uint32 val)
{
   uint32 base;

   base = soc_iproc_sram_membase_get(unit);
   soc_iproc_setreg(unit, (base + offset), val);
}

static __inline__ uint32 soc_m0ssq_sram_read32(int unit, int offset)
{
   uint32 base;
   uint32 val;

   base = soc_iproc_sram_membase_get(unit);
   soc_iproc_getreg(unit, (base + offset), &val);
   return val;
}
/* APIs */
extern int soc_iproc_m0ssq_init(int unit);
extern int soc_iproc_m0ssq_exit(int unit);
extern int soc_iproc_m0ssq_reset_ucore(int unit, int ucore, int reset);
extern int soc_iproc_m0ssq_reset_ucore_get(int unit, int ucore, int *enable);
extern void soc_iproc_m0ssq_reset(int unit);
extern int soc_iproc_m0_init(int unit);
extern int soc_iproc_m0_exit(int unit);
extern int soc_iproc_m0ssq_uc_fw_load(int unit, int uc, int offset, const uint8 *data, int len);
extern int soc_iproc_m0ssq_uc_fw_dump(int unit, int uc, int offset, int len);
extern int soc_iproc_m0ssq_uc_fw_get(int unit, int uc, int* len, uint8* data);
extern int soc_iproc_m0ssq_uc_fw_crc(int unit, int uc, int* len, uint32* load_crc32, uint32* hw_crc32);
extern void soc_iproc_m0ssq_shmem_free(soc_iproc_m0ssq_shmem_t *shmem);
extern int soc_iproc_m0ssq_shmem_get(int unit, char *name, soc_iproc_m0ssq_shmem_t **shmem);
extern int soc_iproc_m0ssq_shmem_write32(soc_iproc_m0ssq_shmem_t *shmem,
                                         uint32 offset, uint32 value);
extern int soc_iproc_m0ssq_shmem_read32(soc_iproc_m0ssq_shmem_t *shmem,
                                        uint32 offset, uint32 *value);
extern int soc_iproc_m0ssq_shmem_clear(soc_iproc_m0ssq_shmem_t *shmem);
extern int soc_iproc_m0ssq_log_dump(int unit);
#else
extern int soc_iproc_m0ssq_shmem_get(char *name, soc_iproc_m0ssq_shmem_t **shmem);
extern int soc_iproc_m0ssq_shmem_write32(soc_iproc_m0ssq_shmem_t *shmem,
                                         uint32 offset, uint32 value);
extern int soc_iproc_m0ssq_shmem_read32(soc_iproc_m0ssq_shmem_t *shmem,
                                        uint32 offset, uint32 *value);
extern int soc_iproc_m0ssq_shmem_clear(soc_iproc_m0ssq_shmem_t *shmem);
extern int soc_iproc_m0ssq_log_dump(int unit);
#endif /* !CMICX_FW_BUILD */
#endif /*defined(BCM_CMICX_SUPPORT) | defined(CMICX_FW_BUILD)*/
#endif /*__IPROC_M0SSQ_H__ */
