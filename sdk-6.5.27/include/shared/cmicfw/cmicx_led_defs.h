/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File:        cmicx_link_defs.h
 * Purpose:     Define layout of m0 led shared memory.
 */

#ifndef CMICX_LED_DEFS_H
#define CMICX_LED_DEFS_H
/*******************************************************************************
 * REGISTER:  CMICX_LED_ACCU_ADDR
 * BLOCKS:
 * SIZE:     32
 ******************************************************************************/
#define CMICX_LED_ACCU_ADDRr_OFFSET 0x00000038

#define CMICX_LED_ACCU_ADDRr_ENUM 0
#define CMICX_LED_ACCU_ADDRr_SIZE 4

/*
 * This structure should be used to declare and program CMICX_LED_ACCU_ADDR.
 *
 */
typedef union CMICX_LED_ACCU_ADDRr_s {
    uint32 v[1];
    uint32 cmicx_led_accu_addr[1];
    uint32 _cmicx_led_accu_addr;
} CMICX_LED_ACCU_ADDRr_t;

#define CMICX_LED_ACCU_ADDRr_CLR(r) (r).cmicx_led_accu_addr[0] = 0
#define CMICX_LED_ACCU_ADDRr_SET(r,d) (r).cmicx_led_accu_addr[0] = d
#define CMICX_LED_ACCU_ADDRr_GET(r) (r).cmicx_led_accu_addr[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define CMICX_LED_ACCU_ADDRr_VALUEf_GET(r) ((r).cmicx_led_accu_addr[0])
#define CMICX_LED_ACCU_ADDRr_VALUEf_SET(r,f) (r).cmicx_led_accu_addr[0]=((uint32)f)

/*
 * These macros can be used to access CMICX_LED_ACCU_ADDR.
 *
 */
#define READ_CMICX_LED_ACCU_ADDRr(u,r) soc_iproc_m0ssq_shmem_read32(u,CMICX_LED_ACCU_ADDRr_OFFSET,r._cmicx_led_accu_addr)
#define WRITE_CMICX_LED_ACCU_ADDRr(u,r) soc_iproc_m0ssq_shmem_write32(u,CMICX_LED_ACCU_ADDRr_OFFSET,r._cmicx_led_accu_addr)

/*******************************************************************************
 * End of 'CMICX_LED_ACCU_ADDRr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  CMICX_LED_CONTROL_DATA
 * BLOCKS:
 * SIZE:     32
 ******************************************************************************/
#define CMICX_LED_CONTROL_DATAr_OFFSET 0x00000040

#define CMICX_LED_CONTROL_DATAr_ENUM 1
#define CMICX_LED_CONTROL_DATAr_SIZE 4

/*
 * This structure should be used to declare and program CMICX_LED_CONTROL_DATA.
 *
 */
typedef union CMICX_LED_CONTROL_DATAr_s {
    uint32 v[1];
    uint32 cmicx_led_control_data[1];
    uint32 _cmicx_led_control_data;
} CMICX_LED_CONTROL_DATAr_t;

#define CMICX_LED_CONTROL_DATAr_CLR(r) (r).cmicx_led_control_data[0] = 0
#define CMICX_LED_CONTROL_DATAr_SET(r,d) (r).cmicx_led_control_data[0] = d
#define CMICX_LED_CONTROL_DATAr_GET(r) (r).cmicx_led_control_data[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define CMICX_LED_CONTROL_DATAr_BYTE_0f_GET(r) (((r).cmicx_led_control_data[0]) & 0xff)
#define CMICX_LED_CONTROL_DATAr_BYTE_0f_SET(r,f) (r).cmicx_led_control_data[0]=(((r).cmicx_led_control_data[0] & ~((uint32)0xff)) | (((uint32)f) & 0xff))
#define CMICX_LED_CONTROL_DATAr_BYTE_1f_GET(r) ((((r).cmicx_led_control_data[0]) >> 8) & 0xff)
#define CMICX_LED_CONTROL_DATAr_BYTE_1f_SET(r,f) (r).cmicx_led_control_data[0]=(((r).cmicx_led_control_data[0] & ~((uint32)0xff << 8)) | ((((uint32)f) & 0xff) << 8))
#define CMICX_LED_CONTROL_DATAr_BYTE_2f_GET(r) ((((r).cmicx_led_control_data[0]) >> 16) & 0xff)
#define CMICX_LED_CONTROL_DATAr_BYTE_2f_SET(r,f) (r).cmicx_led_control_data[0]=(((r).cmicx_led_control_data[0] & ~((uint32)0xff << 16)) | ((((uint32)f) & 0xff) << 16))
#define CMICX_LED_CONTROL_DATAr_BYTE_3f_GET(r) ((((r).cmicx_led_control_data[0]) >> 24) & 0xff)
#define CMICX_LED_CONTROL_DATAr_BYTE_3f_SET(r,f) (r).cmicx_led_control_data[0]=(((r).cmicx_led_control_data[0] & ~((uint32)0xff << 24)) | ((((uint32)f) & 0xff) << 24))

/*
 * These macros can be used to access CMICX_LED_CONTROL_DATA.
 *
 */
#define READ_CMICX_LED_CONTROL_DATAr(u,i,r) soc_iproc_m0ssq_shmem_read32(u,CMICX_LED_CONTROL_DATAr_OFFSET+(4*(i)),r._cmicx_led_control_data)
#define WRITE_CMICX_LED_CONTROL_DATAr(u,i,r) soc_iproc_m0ssq_shmem_write32(u,CMICX_LED_CONTROL_DATAr_OFFSET+(4*(i)),r._cmicx_led_control_data)

/*******************************************************************************
 * End of 'CMICX_LED_CONTROL_DATAr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  CMICX_LED_CTRL
 * BLOCKS:
 * SIZE:     32
 ******************************************************************************/
#define CMICX_LED_CTRLr_OFFSET 0x00000008

#define CMICX_LED_CTRLr_ENUM 2
#define CMICX_LED_CTRLr_SIZE 4

/*
 * This structure should be used to declare and program CMICX_LED_CTRL.
 *
 */
typedef union CMICX_LED_CTRLr_s {
    uint32 v[1];
    uint32 cmicx_led_ctrl[1];
    uint32 _cmicx_led_ctrl;
} CMICX_LED_CTRLr_t;

#define CMICX_LED_CTRLr_CLR(r) (r).cmicx_led_ctrl[0] = 0
#define CMICX_LED_CTRLr_SET(r,d) (r).cmicx_led_ctrl[0] = d
#define CMICX_LED_CTRLr_GET(r) (r).cmicx_led_ctrl[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define CMICX_LED_CTRLr_STARTf_GET(r) (((r).cmicx_led_ctrl[0]) & 0x3)
#define CMICX_LED_CTRLr_STARTf_SET(r,f) (r).cmicx_led_ctrl[0]=(((r).cmicx_led_ctrl[0] & ~((uint32)0x3)) | (((uint32)f) & 0x3))

/*
 * These macros can be used to access CMICX_LED_CTRL.
 *
 */
#define READ_CMICX_LED_CTRLr(u,r) soc_iproc_m0ssq_shmem_read32(u,CMICX_LED_CTRLr_OFFSET,r._cmicx_led_ctrl)
#define WRITE_CMICX_LED_CTRLr(u,r) soc_iproc_m0ssq_shmem_write32(u,CMICX_LED_CTRLr_OFFSET,r._cmicx_led_ctrl)

/*******************************************************************************
 * End of 'CMICX_LED_CTRLr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  CMICX_LED_SIGNATURE
 * BLOCKS:
 * SIZE:     32
 ******************************************************************************/
#define CMICX_LED_SIGNATUREr_OFFSET 0x00000000

#define CMICX_LED_SIGNATUREr_SIZE 4

/*
 * This structure should be used to declare and program CMICX_LED_SIGNATURE.
 *
 */
typedef union CMICX_LED_SIGNATUREr_s {
    uint32 v[1];
    uint32 cmicx_led_signature[1];
    uint32 _cmicx_led_signature;
} CMICX_LED_SIGNATUREr_t;

#define CMICX_LED_SIGNATUREr_CLR(r) (r).cmicx_led_signature[0] = 0
#define CMICX_LED_SIGNATUREr_SET(r,d) (r).cmicx_led_signature[0] = d
#define CMICX_LED_SIGNATUREr_GET(r) (r).cmicx_led_signature[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define CMICX_LED_SIGNATUREr_DATAf_GET(r) ((r).cmicx_led_signature[0])
#define CMICX_LED_SIGNATUREr_DATAf_SET(r,f) (r).cmicx_led_signature[0]=((uint32)f)

/*
 * These macros can be used to access CMICX_LED_SIGNATURE.
 *
 */
#define READ_CMICX_LED_SIGNATUREr(u,r) soc_iproc_m0ssq_shmem_read32(u,CMICX_LED_SIGNATUREr_OFFSET,r._cmicx_led_signature)
#define WRITE_CMICX_LED_SIGNATUREr(u,r) soc_iproc_m0ssq_shmem_write32(u,CMICX_LED_SIGNATUREr_OFFSET,r._cmicx_led_signature)

/*******************************************************************************
 * End of 'CMICX_LED_SIGNATUREr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  CMICX_LED_STATUS
 * BLOCKS:
 * SIZE:     32
 ******************************************************************************/
#define CMICX_LED_STATUSr_OFFSET 0x0000000c

#define CMICX_LED_STATUSr_ENUM 4
#define CMICX_LED_STATUSr_SIZE 4

/*
 * This structure should be used to declare and program CMICX_LED_STATUS.
 *
 */
typedef union CMICX_LED_STATUSr_s {
    uint32 v[1];
    uint32 cmicx_led_status[1];
    uint32 _cmicx_led_status;
} CMICX_LED_STATUSr_t;

#define CMICX_LED_STATUSr_CLR(r) (r).cmicx_led_status[0] = 0
#define CMICX_LED_STATUSr_SET(r,d) (r).cmicx_led_status[0] = d
#define CMICX_LED_STATUSr_GET(r) (r).cmicx_led_status[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define CMICX_LED_STATUSr_STARTf_GET(r) (((r).cmicx_led_status[0]) & 0x3)
#define CMICX_LED_STATUSr_STARTf_SET(r,f) (r).cmicx_led_status[0]=(((r).cmicx_led_status[0] & ~((uint32)0x3)) | (((uint32)f) & 0x3))

/*
 * These macros can be used to access CMICX_LED_STATUS.
 *
 */
#define READ_CMICX_LED_STATUSr(u,r) soc_iproc_m0ssq_shmem_read32(u,CMICX_LED_STATUSr_OFFSET,r._cmicx_led_status)
#define WRITE_CMICX_LED_STATUSr(u,r) soc_iproc_m0ssq_shmem_write32(u,CMICX_LED_STATUSr_OFFSET,r._cmicx_led_status)

/*******************************************************************************
 * End of 'CMICX_LED_STATUSr'
 ******************************************************************************/

/*******************************************************************************
 * REGISTER:  CMICX_LED_WAR_DATA
 * BLOCKS:
 * SIZE:     32
 ******************************************************************************/
#define CMICX_LED_WAR_DATAr_OFFSET 0x0000003c

#define CMICX_LED_WAR_DATAr_ENUM 6
#define CMICX_LED_WAR_DATAr_SIZE 4

/*
 * This structure should be used to declare and program CMICX_LED_WAR_DATA.
 *
 */
typedef union CMICX_LED_WAR_DATAr_s {
    uint32 v[1];
    uint32 cmicx_led_war_data[1];
    uint32 _cmicx_led_war_data;
} CMICX_LED_WAR_DATAr_t;

#define CMICX_LED_WAR_DATAr_CLR(r) (r).cmicx_led_war_data[0] = 0
#define CMICX_LED_WAR_DATAr_SET(r,d) (r).cmicx_led_war_data[0] = d
#define CMICX_LED_WAR_DATAr_GET(r) (r).cmicx_led_war_data[0]

/*
 * These macros can be used to access individual fields.
 *
 */
#define CMICX_LED_WAR_DATAr_BYTE_0f_GET(r) (((r).cmicx_led_war_data[0]) & 0xff)
#define CMICX_LED_WAR_DATAr_BYTE_0f_SET(r,f) (r).cmicx_led_war_data[0]=(((r).cmicx_led_war_data[0] & ~((uint32)0xff)) | (((uint32)f) & 0xff))
#define CMICX_LED_WAR_DATAr_BYTE_1f_GET(r) ((((r).cmicx_led_war_data[0]) >> 8) & 0xff)
#define CMICX_LED_WAR_DATAr_BYTE_1f_SET(r,f) (r).cmicx_led_war_data[0]=(((r).cmicx_led_war_data[0] & ~((uint32)0xff << 8)) | ((((uint32)f) & 0xff) << 8))
#define CMICX_LED_WAR_DATAr_BYTE_2f_GET(r) ((((r).cmicx_led_war_data[0]) >> 16) & 0xff)
#define CMICX_LED_WAR_DATAr_BYTE_2f_SET(r,f) (r).cmicx_led_war_data[0]=(((r).cmicx_led_war_data[0] & ~((uint32)0xff << 16)) | ((((uint32)f) & 0xff) << 16))
#define CMICX_LED_WAR_DATAr_BYTE_3f_GET(r) ((((r).cmicx_led_war_data[0]) >> 24) & 0xff)
#define CMICX_LED_WAR_DATAr_BYTE_3f_SET(r,f) (r).cmicx_led_war_data[0]=(((r).cmicx_led_war_data[0] & ~((uint32)0xff << 24)) | ((((uint32)f) & 0xff) << 24))

/*
 * These macros can be used to access CMICX_LED_WAR_DATA.
 *
 */
#define READ_CMICX_LED_WAR_DATAr(u,r) soc_iproc_m0ssq_shmem_read32(u,CMICX_LED_WAR_DATAr_OFFSET,r._cmicx_led_war_data)
#define WRITE_CMICX_LED_WAR_DATAr(u,r) soc_iproc_m0ssq_shmem_write32(u,CMICX_LED_WAR_DATAr_OFFSET,r._cmicx_led_war_data)

/*******************************************************************************
 * End of 'CMICX_LED_WAR_DATAr'
 ******************************************************************************/

#endif /* CMICX_LED_DEFS_H */
