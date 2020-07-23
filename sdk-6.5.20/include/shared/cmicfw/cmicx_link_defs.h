/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cmicx_link_defs.h
 * Purpose:     Define layout of m0 linkscan shared memory.
 */

#ifndef CMICX_LINK_DEFS_H
#define CMICX_LINK_DEFS_H
/*******************************************************************************
 *
 *        CMICX FW DEFINITIONS BEGIN HERE
 */

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_CTRL
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 */
#define FWLINKSCAN_CTRLr_OFFSET 0x00000000

#define FWLINKSCAN_CTRLr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_CTRL.
 */
typedef union FWLINKSCAN_CTRLr_s {
    uint32 v[1];
    uint32 fwlinkscan_ctrl[1];
    uint32 _fwlinkscan_ctrl;
} FWLINKSCAN_CTRLr_t;

#define FWLINKSCAN_CTRLr_CLR(r) (r).fwlinkscan_ctrl[0] = 0
#define FWLINKSCAN_CTRLr_SET(r,d) (r).fwlinkscan_ctrl[0] = d
#define FWLINKSCAN_CTRLr_GET(r) (r).fwlinkscan_ctrl[0]

/*
 * These macros can be used to access individual fields.
 */
#define FWLINKSCAN_CTRLr_PAUSEf_GET(r) (((r).fwlinkscan_ctrl[0]) & 0x1)
#define FWLINKSCAN_CTRLr_PAUSEf_SET(r,f) (r).fwlinkscan_ctrl[0]=(((r).fwlinkscan_ctrl[0] & ~((uint32)0x1)) | (((uint32)f) & 0x1))

/*
 * These macros can be used to access FWLINKSCAN_CTRL.
 */
#define READ_FWLINKSCAN_CTRLr(u,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_CTRLr_OFFSET,r._fwlinkscan_ctrl)
#define WRITE_FWLINKSCAN_CTRLr(u,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_CTRLr_OFFSET,r._fwlinkscan_ctrl)

/*******************************************************************************
 * End of 'FWLINKSCAN_CTRLr'
 */


/*******************************************************************************
 * REGISTER:  FWLINKSCAN_STATUS
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 */
#define FWLINKSCAN_STATUSr_OFFSET 0x00000004

#define FWLINKSCAN_STATUSr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_STATUS.
 */
typedef union FWLINKSCAN_STATUSr_s {
    uint32 v[1];
    uint32 fwlinkscan_status[1];
    uint32 _fwlinkscan_status;
} FWLINKSCAN_STATUSr_t;

#define FWLINKSCAN_STATUSr_CLR(r) (r).fwlinkscan_status[0] = 0
#define FWLINKSCAN_STATUSr_SET(r,d) (r).fwlinkscan_status[0] = d
#define FWLINKSCAN_STATUSr_GET(r) (r).fwlinkscan_status[0]

/*
 * These macros can be used to access individual fields.
 */
#define FWLINKSCAN_STATUSr_INITf_GET(r) (((r).fwlinkscan_status[0]) & 0x1)
#define FWLINKSCAN_STATUSr_INITf_SET(r,f) (r).fwlinkscan_status[0]=(((r).fwlinkscan_status[0] & ~((uint32)0x1)) | (((uint32)f) & 0x1))
#define FWLINKSCAN_STATUSr_RUNf_GET(r) ((((r).fwlinkscan_status[0]) >> 1) & 0x1)
#define FWLINKSCAN_STATUSr_RUNf_SET(r,f) (r).fwlinkscan_status[0]=(((r).fwlinkscan_status[0] & ~((uint32)0x1 << 1)) | ((((uint32)f) & 0x1) << 1))
#define FWLINKSCAN_STATUSr_ERRORf_GET(r) ((((r).fwlinkscan_status[0]) >> 2) & 0x3f)
#define FWLINKSCAN_STATUSr_ERRORf_SET(r,f) (r).fwlinkscan_status[0]=(((r).fwlinkscan_status[0] & ~((uint32)0x3f << 2)) | ((((uint32)f) & 0x3f) << 2))

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_STATUS
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 */
/*
 * These macros can be used to access FWLINKSCAN_STATUS.
 */
#define READ_FWLINKSCAN_STATUSr(u,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_STATUSr_OFFSET,r._fwlinkscan_status)
#define WRITE_FWLINKSCAN_STATUSr(u,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_STATUSr_OFFSET,r._fwlinkscan_status)

/*******************************************************************************
 * End of 'FWLINKSCAN_STATUSr'
 */


/*******************************************************************************
 * REGISTER:  FWLINKSCAN_SCHAN_NUM
 * BLOCKS:   FWLINKSCAN
 * SIZE:     32
 */
#define FWLINKSCAN_SCHANr_OFFSET 0x00000008
#define FWLINKSCAN_SCHANr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_SCHAN_NUM.
 */
typedef union FWLINKSCAN_SCHANr_s {
    uint32 v[1];
    uint32 fwlinkscan_schan[1];
    uint32 _fwlinkscan_schan;
} FWLINKSCAN_SCHANr_t;

#define FWLINKSCAN_SCHANr_CLR(r) (r).fwlinkscan_schan[0] = 0
#define FWLINKSCAN_SCHANr_SET(r,d) (r).fwlinkscan_schan[0] = d
#define FWLINKSCAN_SCHANr_GET(r) (r).fwlinkscan_schan[0]

/*
 * These macros can be used to access individual fields.
 */
#define FWLINKSCAN_SCHANr_NUMf_GET(r) (((r).fwlinkscan_schan[0]) & 0xf)
#define FWLINKSCAN_SCHANr_NUMf_SET(r,f) (r).fwlinkscan_schan[0]=(((r).fwlinkscan_schan[0] & ~((uint32)0xf)) | (((uint32)f) & 0xf))
#define FWLINKSCAN_SCHANr_MAGICf_GET(r) ((((r).fwlinkscan_schan[0]) >> 8) & 0xffffff)
#define FWLINKSCAN_SCHANr_MAGICf_SET(r,f) (r).fwlinkscan_schan[0]=(((r).fwlinkscan_schan[0] & 0xFF) | ((((uint32)f) & 0xffffff) << 8))

/*
 * These macros can be used to access FWLINKSCAN_SCHAN_NUM.
 */
#define READ_FWLINKSCAN_SCHANr(u,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_SCHANr_OFFSET,r._fwlinkscan_schan)
#define WRITE_FWLINKSCAN_SCHANr(u,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_SCHANr_OFFSET,r._fwlinkscan_schan)

/*******************************************************************************
 * End of 'FWLINKSCAN_SCHAN_NUMr'
 */

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_COUNTER
 * BLOCKS:   FWLINKSCAN
 * SIZE:     32
 */
#define FWLINKSCAN_COUNTERr_OFFSET 0x0000003C

#define FWLINKSCAN_COUNTERr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_COUNTER.
 */
typedef union FWLINKSCAN_COUNTERr_s {
    uint32 v[1];
    uint32 fwlinkscan_counter[1];
    uint32 _fwlinkscan_counter;
} FWLINKSCAN_COUNTERr_t;

#define FWLINKSCAN_COUNTERr_CLR(r) (r).fwlinkscan_counter[0] = 0
#define FWLINKSCAN_COUNTERr_SET(r,d) (r).fwlinkscan_counter[0] = d
#define FWLINKSCAN_COUNTERr_GET(r) (r).fwlinkscan_counter[0]

/*
 * These macros can be used to access individual fields.
 */
#define FWLINKSCAN_COUNTERr_LOOPf_GET(r) ((r).fwlinkscan_counter[0])
#define FWLINKSCAN_COUNTERr_LOOPf_SET(r,f) (r).fwlinkscan_counter[0]=((uint32)f)

/*
 * These macros can be used to access FWLINKSCAN_COUNTER.
 */
#define READ_FWLINKSCAN_COUNTERr(u,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_COUNTERr_OFFSET,r._fwlinkscan_counter)
#define WRITE_FWLINKSCAN_COUNTERr(u,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_COUNTERr_OFFSET,r._fwlinkscan_counter)

/*******************************************************************************
 * End of 'FWLINKSCAN_COUNTERr'
 */

#endif /* CMICX_LINK_DEFS_H */
