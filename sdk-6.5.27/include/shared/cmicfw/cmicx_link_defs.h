/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cmicx_link_defs.h
 * Purpose: Definition file for the CMICX_LINK.
 *
 */

#ifndef CMICX_LINK_DEFS_H
#define CMICX_LINK_DEFS_H

/*******************************************************************************
 *
 *        CMICX_LINK DEFINITIONS BEGIN HERE
 *
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_CTRL
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 ******************************************************************************/
#define FWLINKSCAN_CTRLr_OFFSET 0x00000400

#define FWLINKSCAN_CTRLr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_CTRL.
 *
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
 *
 */
#define FWLINKSCAN_CTRLr_PAUSEf_GET(r) (((r).fwlinkscan_ctrl[0]) & 0x1)
#define FWLINKSCAN_CTRLr_PAUSEf_SET(r,f) (r).fwlinkscan_ctrl[0]=(((r).fwlinkscan_ctrl[0] & ~((uint32)0x1)) | (((uint32)f) & 0x1))
#define FWLINKSCAN_CTRLr_HW_CFG_ASYNCf_GET(r) ((((r).fwlinkscan_ctrl[0]) >> 1) & 0x1)
#define FWLINKSCAN_CTRLr_HW_CFG_ASYNCf_SET(r,f) (r).fwlinkscan_ctrl[0]=(((r).fwlinkscan_ctrl[0] & ~((uint32)0x1 << 1)) | ((((uint32)f) & 0x1) << 1))
#define FWLINKSCAN_CTRLr_FLR_LINKUP_CB_ENf_GET(r) ((((r).fwlinkscan_ctrl[0]) >> 2) & 0x1)
#define FWLINKSCAN_CTRLr_FLR_LINKUP_CB_ENf_SET(r,f) (r).fwlinkscan_ctrl[0]=(((r).fwlinkscan_ctrl[0] & ~((uint32)0x1 << 2)) | ((((uint32)f) & 0x1) << 2))

/*
 * These macros can be used to access FWLINKSCAN_CTRL.
 *
 */
#define READ_FWLINKSCAN_CTRLr(u,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_CTRLr_OFFSET,r._fwlinkscan_ctrl)
#define WRITE_FWLINKSCAN_CTRLr(u,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_CTRLr_OFFSET,r._fwlinkscan_ctrl)

/*******************************************************************************
 * End of 'FWLINKSCAN_CTRLr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_LOOPBACK
 * BLOCKS:    FWLINKSCAN
 * SIZE:     384(bits)
 * Note:      This register is for the loopback configure of ports.
 *            it was required for fabric port to check another fsrd indication.
 *            But this indication was not avaliable on loopback port.
 *            This register memory(0x330-0x360) is only used on dnx devices
 ******************************************************************************/
#define FWLINKSCAN_LOOPBACKr_OFFSET 0x00000330
#define FWLINKSCAN_LOOPBACKr_SIZE   0x30

/*
 * This structure should be used to declare and program FWLINKSCAN_LOOPBACK.
 *
 */
typedef union FWLINKSCAN_LOOPBACKr_s {
    uint32 v[1];
    uint32 fwlinkscan_loopback[1];
    uint32 _fwlinkscan_loopback;
} FWLINKSCAN_LOOPBACKr_t;

#define FWLINKSCAN_LOOPBACKr_CLR(r)     (r).fwlinkscan_loopback[0] = 0
#define FWLINKSCAN_LOOPBACKr_SET(r,d)   (r).fwlinkscan_loopback[0] = d
#define FWLINKSCAN_LOOPBACKr_GET(r)     (r).fwlinkscan_loopback[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define FWLINKSCAN_LOOPBACKr_BITSf_GET(r,a) bcmdrd_field_get((r).fwlinkscan_loopback,0,384,a)
#define FWLINKSCAN_LOOPBACKr_BITSf_SET(r,a) bcmdrd_field_set((r).fwlinkscan_loopback,0,384,a)

/*
 * These macros can be used to access FWLINKSCAN_LOOPBACK.
 *
 */
#define READ_FWLINKSCAN_LOOPBACKr(u,i,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_LOOPBACKr_OFFSET+(4*(i)),r._fwlinkscan_loopback)
#define WRITE_FWLINKSCAN_LOOPBACKr(u,i,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_LOOPBACKr_OFFSET+(4*(i)),r._fwlinkscan_loopback)

/*******************************************************************************
 * End of 'FWLINKSCAN_LOOPBACKr_t'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_FLR_ENABLE
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 ******************************************************************************/
#define FWLINKSCAN_FLR_ENABLEr_OFFSET 0x00000300

#define FWLINKSCAN_FLR_ENABLEr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_FLR_ENABLE.
 *
 */
typedef union FWLINKSCAN_FLR_ENABLEr_s {
    uint32 v[1];
    uint32 fwlinkscan_flr_enable[1];
    uint32 _fwlinkscan_flr_enable;
} FWLINKSCAN_FLR_ENABLEr_t;

#define FWLINKSCAN_FLR_ENABLEr_CLR(r) (r).fwlinkscan_flr_enable[0] = 0
#define FWLINKSCAN_FLR_ENABLEr_SET(r,d) (r).fwlinkscan_flr_enable[0] = d
#define FWLINKSCAN_FLR_ENABLEr_GET(r) (r).fwlinkscan_flr_enable[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define FWLINKSCAN_FLR_ENABLEr_BITSf_GET(r,a) bcmdrd_field_get((r).fwlinkscan_flr_enable,0,384,a)
#define FWLINKSCAN_FLR_ENABLEr_BITSf_SET(r,a) bcmdrd_field_set((r).fwlinkscan_flr_enable,0,384,a)

/*
 * These macros can be used to access FWLINKSCAN_FLR_ENABLE.
 *
 */
#define READ_FWLINKSCAN_FLR_ENABLEr(u,i,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_FLR_ENABLEr_OFFSET+(4*(i)),r._fwlinkscan_flr_enable)
#define WRITE_FWLINKSCAN_FLR_ENABLEr(u,i,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_FLR_ENABLEr_OFFSET+(4*(i)),r._fwlinkscan_flr_enable)

/*******************************************************************************
 * End of 'FWLINKSCAN_FLR_ENABLEr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_FLR_LINKUP_CNT
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 ******************************************************************************/
#define FWLINKSCAN_FLR_LINKUP_CNTr_OFFSET 0x00000180

#define FWLINKSCAN_FLR_LINKUP_CNTr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_FLR_LINKUP_CNT.
 *
 */
typedef union FWLINKSCAN_FLR_LINKUP_CNTr_s {
    uint32 v[1];
    uint32 fwlinkscan_flr_linkup_cnt[1];
    uint32 _fwlinkscan_flr_linkup_cnt;
} FWLINKSCAN_FLR_LINKUP_CNTr_t;

#define FWLINKSCAN_FLR_LINKUP_CNTr_CLR(r) (r).fwlinkscan_flr_linkup_cnt[0] = 0
#define FWLINKSCAN_FLR_LINKUP_CNTr_SET(r,d) (r).fwlinkscan_flr_linkup_cnt[0] = d
#define FWLINKSCAN_FLR_LINKUP_CNTr_GET(r) (r).fwlinkscan_flr_linkup_cnt[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define FWLINKSCAN_FLR_LINKUP_CNTr_BYTE0f_GET(r) (((r).fwlinkscan_flr_linkup_cnt[0]) & 0xff)
#define FWLINKSCAN_FLR_LINKUP_CNTr_BYTE0f_SET(r,f) (r).fwlinkscan_flr_linkup_cnt[0]=(((r).fwlinkscan_flr_linkup_cnt[0] & ~((uint32)0xff)) | (((uint32)f) & 0xff))
#define FWLINKSCAN_FLR_LINKUP_CNTr_BYTE1f_GET(r) ((((r).fwlinkscan_flr_linkup_cnt[0]) >> 8) & 0xff)
#define FWLINKSCAN_FLR_LINKUP_CNTr_BYTE1f_SET(r,f) (r).fwlinkscan_flr_linkup_cnt[0]=(((r).fwlinkscan_flr_linkup_cnt[0] & ~((uint32)0xff << 8)) | ((((uint32)f) & 0xff) << 8))
#define FWLINKSCAN_FLR_LINKUP_CNTr_BYTE2f_GET(r) ((((r).fwlinkscan_flr_linkup_cnt[0]) >> 16) & 0xff)
#define FWLINKSCAN_FLR_LINKUP_CNTr_BYTE2f_SET(r,f) (r).fwlinkscan_flr_linkup_cnt[0]=(((r).fwlinkscan_flr_linkup_cnt[0] & ~((uint32)0xff << 16)) | ((((uint32)f) & 0xff) << 16))
#define FWLINKSCAN_FLR_LINKUP_CNTr_BYTE3f_GET(r) ((((r).fwlinkscan_flr_linkup_cnt[0]) >> 24) & 0xff)
#define FWLINKSCAN_FLR_LINKUP_CNTr_BYTE3f_SET(r,f) (r).fwlinkscan_flr_linkup_cnt[0]=(((r).fwlinkscan_flr_linkup_cnt[0] & ~((uint32)0xff << 24)) | ((((uint32)f) & 0xff) << 24))

/*
 * These macros can be used to access FWLINKSCAN_FLR_LINKUP_CNT.
 *
 */
#define READ_FWLINKSCAN_FLR_LINKUP_CNTr(u,i,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_FLR_LINKUP_CNTr_OFFSET+(4*(i)),r._fwlinkscan_flr_linkup_cnt)
#define WRITE_FWLINKSCAN_FLR_LINKUP_CNTr(u,i,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_FLR_LINKUP_CNTr_OFFSET+(4*(i)),r._fwlinkscan_flr_linkup_cnt)

/*******************************************************************************
 * End of 'FWLINKSCAN_FLR_LINKUP_CNTr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_FLR_TIMEOUT
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 ******************************************************************************/
#define FWLINKSCAN_FLR_TIMEOUTr_OFFSET 0x00000000

#define FWLINKSCAN_FLR_TIMEOUTr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_FLR_TIMEOUT.
 *
 */
typedef union FWLINKSCAN_FLR_TIMEOUTr_s {
    uint32 v[1];
    uint32 fwlinkscan_flr_timeout[1];
    uint32 _fwlinkscan_flr_timeout;
} FWLINKSCAN_FLR_TIMEOUTr_t;

#define FWLINKSCAN_FLR_TIMEOUTr_CLR(r) (r).fwlinkscan_flr_timeout[0] = 0
#define FWLINKSCAN_FLR_TIMEOUTr_SET(r,d) (r).fwlinkscan_flr_timeout[0] = d
#define FWLINKSCAN_FLR_TIMEOUTr_GET(r) (r).fwlinkscan_flr_timeout[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define FWLINKSCAN_FLR_TIMEOUTr_BYTE0f_GET(r) (((r).fwlinkscan_flr_timeout[0]) & 0xff)
#define FWLINKSCAN_FLR_TIMEOUTr_BYTE0f_SET(r,f) (r).fwlinkscan_flr_timeout[0]=(((r).fwlinkscan_flr_timeout[0] & ~((uint32)0xff)) | (((uint32)f) & 0xff))
#define FWLINKSCAN_FLR_TIMEOUTr_BYTE1f_GET(r) ((((r).fwlinkscan_flr_timeout[0]) >> 8) & 0xff)
#define FWLINKSCAN_FLR_TIMEOUTr_BYTE1f_SET(r,f) (r).fwlinkscan_flr_timeout[0]=(((r).fwlinkscan_flr_timeout[0] & ~((uint32)0xff << 8)) | ((((uint32)f) & 0xff) << 8))
#define FWLINKSCAN_FLR_TIMEOUTr_BYTE2f_GET(r) ((((r).fwlinkscan_flr_timeout[0]) >> 16) & 0xff)
#define FWLINKSCAN_FLR_TIMEOUTr_BYTE2f_SET(r,f) (r).fwlinkscan_flr_timeout[0]=(((r).fwlinkscan_flr_timeout[0] & ~((uint32)0xff << 16)) | ((((uint32)f) & 0xff) << 16))
#define FWLINKSCAN_FLR_TIMEOUTr_BYTE3f_GET(r) ((((r).fwlinkscan_flr_timeout[0]) >> 24) & 0xff)
#define FWLINKSCAN_FLR_TIMEOUTr_BYTE3f_SET(r,f) (r).fwlinkscan_flr_timeout[0]=(((r).fwlinkscan_flr_timeout[0] & ~((uint32)0xff << 24)) | ((((uint32)f) & 0xff) << 24))

/*
 * These macros can be used to access FWLINKSCAN_FLR_TIMEOUT.
 *
 */
#define READ_FWLINKSCAN_FLR_TIMEOUTr(u,i,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_FLR_TIMEOUTr_OFFSET+(4*(i)),r._fwlinkscan_flr_timeout)
#define WRITE_FWLINKSCAN_FLR_TIMEOUTr(u,i,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_FLR_TIMEOUTr_OFFSET+(4*(i)),r._fwlinkscan_flr_timeout)

/*******************************************************************************
 * End of 'FWLINKSCAN_FLR_TIMEOUTr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_SCHAN
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 ******************************************************************************/
#define FWLINKSCAN_SCHANr_OFFSET 0x00000408

#define FWLINKSCAN_SCHANr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_SCHAN.
 *
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
 *
 */
#define FWLINKSCAN_SCHANr_NUMf_GET(r) (((r).fwlinkscan_schan[0]) & 0xf)
#define FWLINKSCAN_SCHANr_NUMf_SET(r,f) (r).fwlinkscan_schan[0]=(((r).fwlinkscan_schan[0] & ~((uint32)0xf)) | (((uint32)f) & 0xf))
#define FWLINKSCAN_SCHANr_MAGICf_GET(r) ((((r).fwlinkscan_schan[0]) >> 31) & 0x1)
#define FWLINKSCAN_SCHANr_MAGICf_SET(r,f) (r).fwlinkscan_schan[0]=(((r).fwlinkscan_schan[0] & ~((uint32)0x1 << 31)) | ((((uint32)f) & 0x1) << 31))

/*
 * These macros can be used to access FWLINKSCAN_SCHAN.
 *
 */
#define READ_FWLINKSCAN_SCHANr(u,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_SCHANr_OFFSET,r._fwlinkscan_schan)
#define WRITE_FWLINKSCAN_SCHANr(u,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_SCHANr_OFFSET,r._fwlinkscan_schan)

/*******************************************************************************
 * End of 'FWLINKSCAN_SCHANr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  FWLINKSCAN_STATUS
 * BLOCKS:    FWLINKSCAN
 * SIZE:     32
 ******************************************************************************/
#define FWLINKSCAN_STATUSr_OFFSET 0x00000404

#define FWLINKSCAN_STATUSr_SIZE 4

/*
 * This structure should be used to declare and program FWLINKSCAN_STATUS.
 *
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
 *
 */
#define FWLINKSCAN_STATUSr_INITf_GET(r) (((r).fwlinkscan_status[0]) & 0x1)
#define FWLINKSCAN_STATUSr_INITf_SET(r,f) (r).fwlinkscan_status[0]=(((r).fwlinkscan_status[0] & ~((uint32)0x1)) | (((uint32)f) & 0x1))
#define FWLINKSCAN_STATUSr_RUNf_GET(r) ((((r).fwlinkscan_status[0]) >> 1) & 0x1)
#define FWLINKSCAN_STATUSr_RUNf_SET(r,f) (r).fwlinkscan_status[0]=(((r).fwlinkscan_status[0] & ~((uint32)0x1 << 1)) | ((((uint32)f) & 0x1) << 1))
#define FWLINKSCAN_STATUSr_LOOP_CNTf_GET(r) ((((r).fwlinkscan_status[0]) >> 2) & 0xf)
#define FWLINKSCAN_STATUSr_LOOP_CNTf_SET(r,f) (r).fwlinkscan_status[0]=(((r).fwlinkscan_status[0] & ~((uint32)0xf << 2)) | ((((uint32)f) & 0xf) << 2))
#define FWLINKSCAN_STATUSr_ERRORf_GET(r) ((((r).fwlinkscan_status[0]) >> 6) & 0x3)
#define FWLINKSCAN_STATUSr_ERRORf_SET(r,f) (r).fwlinkscan_status[0]=(((r).fwlinkscan_status[0] & ~((uint32)0x3 << 6)) | ((((uint32)f) & 0x3) << 6))

/*
 * These macros can be used to access FWLINKSCAN_STATUS.
 *
 */
#define READ_FWLINKSCAN_STATUSr(u,r) soc_iproc_m0ssq_shmem_read32(u,FWLINKSCAN_STATUSr_OFFSET,r._fwlinkscan_status)
#define WRITE_FWLINKSCAN_STATUSr(u,r) soc_iproc_m0ssq_shmem_write32(u,FWLINKSCAN_STATUSr_OFFSET,r._fwlinkscan_status)

/*******************************************************************************
 * End of 'FWLINKSCAN_STATUSr'
 ******************************************************************************/

#endif /* CMICX_LINK_DEFS_H */
