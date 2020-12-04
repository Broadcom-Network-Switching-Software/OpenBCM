/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    hla.h
 * Purpose: Provides structures and macros to handle 
 *          Hardware License Authenticator (HLA)
 */
#ifndef __HLA_H__
#define __HLA_H__

#ifdef BCM_HLA_SUPPORT
/* SDK */
#include <bcm/types.h>
#define HLA_AXI_BASE (0xe8000000)
#else
/* FW */
#include <types.h>
#define HLA_AXI_BASE (0)
#endif

#define HLA_LIC_OFFSET (HLA_AXI_BASE + 0x00040000)
#define HLA_AUTH_DATA_SIZE_MAX (16*1024)
#define HLA_PUBLIC_REG_BASE (HLA_AXI_BASE + 0x000F0000)

#define HLA_PUBLIC_SCRATCH_REG_BASE (HLA_PUBLIC_REG_BASE + 0x400)
#define HLA_PUBLIC_SCRATCH_REG_WIDTH 32
#define HLA_PUBLIC_SCRATCH_REG_NUM_MAX 16

#define HLA_USR_CTRL_STATr  HLA_PUBLIC_SCRATCH_REG_BASE
#define HLA_USR_CTRL_STAT_START_SHIFT 0
#define HLA_USR_CTRL_STAT_START_WIDTH 1
#define HLA_USR_CTRL_STAT_BUSY_SHIFT 8
#define HLA_USR_CTRL_STAT_BUSY_WIDTH 1
#define HLA_USR_CTRL_STAT_DONE_SHIFT 9
#define HLA_USR_CTRL_STAT_DONE_WIDTH 1
#define HLA_USR_CTRL_STAT_ERROR_SHIFT 16
#define HLA_USR_CTRL_STAT_ERROR_WIDTH 16

#define HLA_STATE_CURr (HLA_PUBLIC_SCRATCH_REG_BASE + 4)
#define HLA_STATE_CUR_STATE_SHIFT 0
#define HLA_STATE_CUR_STATE_WIDTH 16
#define HLA_STATE_CUR_ITERATION_SHIFT 16
#define HLA_STATE_CUR_ITERATION_WIDTH 16

#define HLA_STATE_PREVr (HLA_PUBLIC_SCRATCH_REG_BASE + 0x8)
#define HLA_SEC_ERRr (HLA_PUBLIC_SCRATCH_REG_BASE + 0xc)
#define HLA_DBG_CHIP_ID_L_OTPr (HLA_PUBLIC_SCRATCH_REG_BASE + 0x10)
#define HLA_DBG_CHIP_ID_H_OTPr (HLA_PUBLIC_SCRATCH_REG_BASE + 0x14)
#define HLA_DBG_CHIP_ID_L_CDBr (HLA_PUBLIC_SCRATCH_REG_BASE + 0x18)
#define HLA_DBG_CHIP_ID_H_CDBr (HLA_PUBLIC_SCRATCH_REG_BASE + 0x1c)
#define HLA_DBG_CDBr       (HLA_PUBLIC_SCRATCH_REG_BASE + 0x20)
#define HLA_DBG_CDB_CDBNUM_SHIFT 0
#define HLA_DBG_CDB_CDBNUM_WIDTH 8
#define HLA_FW_VERSIONr        (HLA_PUBLIC_SCRATCH_REG_BASE + 0x3c)
#define HLA_FW_VERSION_MINOR_SHIFT 0
#define HLA_FW_VERSION_MINOR_WIDTH 16
#define HLA_FW_VERSION_MAJOR_SHIFT 16
#define HLA_FW_VERSION_MAJOR_WIDTH 8
#define HLA_FW_VERSION_TYPE_SHIFT 24
#define HLA_FW_VERSION_TYPE_WIDTH 8

/* HLA bootrom execution status codes */
#define HLA_STATE_CODE_RELOC         (0x8001)
#define HLA_STATE_CCF_WAIT           (0x8002)
#define HLA_STATE_SDK_HDR_PROC       (0x8003)
#define HLA_STATE_CCF_AUTH           (0x8004)
#define HLA_STATE_CCF_PROC           (0x8005)
#define HLA_STATE_CDB_AUTH           (0x8006)
#define HLA_STATE_CDB_PROC           (0x8007)
#define HLA_STATE_LC_AUTH            (0x8008)
#define HLA_STATE_LC_PROC            (0x8009)
#define HLA_STATE_APP_LOAD           (0x800a)
#define HLA_STATE_SEC_ERR            (0x800b)
#define HLA_STATE_DONE               (0x800f)


/* Error Codes */
#define HLA_ERR_NONE                 (0x0)
#define HLA_ERR_INTERNAL             (0x0001)
#define HLA_ERR_NO_CCF               (0x0002)
#define HLA_ERR_BOUNDARY_CHK_FAIL    (0x0003)
#define HLA_ERR_CCF_BOUNDAY_CHK_FAIL (0x0004)
#define HLA_ERR_CCF_AUTH_FAIL        (0x0005)
#define HLA_ERR_CDB_NONE             (0x0006)
#define HLA_ERR_CDB_BOUNDAY_CHK_FAIL (0x0007)
#define HLA_ERR_CHIPID_MATCH_FAIL    (0x0008)
#define HLA_ERR_HASH_MATCH_FAIL      (0x0009)

#define BSWAP(val) (val << 24 | (val & 0xff00) << 8 | (val & 0xff0000) >> 8 | val >>24)

/* (CSG) Switch Content for Hardware Authentication (SCHA) */
#pragma pack(1)
typedef struct scha_hdr_s {
    uint32 id;         /* SCHA identifier - must be 0x53434841  */
    uint16 enctype;    /* encryption type, details TBD */
    uint16 flags;      /* Content type flags */
    uint32 len;        /* size of payload in bytes, i.e CCFC Header + CCF blob length */
    uint32 offset;     /* Loader code offset, valid only if flags.O is set */
    uint32 opaque[8];  /* optional 256bit opaque field, exists only if flags.O is set */
} scha_hdr_t;
#define SCHA_IDENTIFIER (0x53434841)
#pragma pack()

typedef struct hla_status_s {
    uint32 fw_ver;
    uint32 reset;
    uint32 busy;
    uint32 error;
    uint32 error2;
    uint32 cur_run;
    uint32 cur_state;
    uint32 prev_state;
    uint32 chipid_l;
    uint32 chipid_h;
} hla_status_t;

extern int soc_hla_ccf_load(int unit, const uint8 *data, int len);
extern int soc_hla_status(int unit, hla_status_t *hla_status);

#endif /* __HLA_H__ */
