/*! \file bcmbd_cmicx_schan.h
 *
 * S-Channel Message: data structure used by firmware to transport
 * S-Channel Data into SOC via CMIC.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_SCHAN_H
#define BCMBD_CMICX_SCHAN_H

#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd_cmicx2.h>

/*! \cond */

/* Use the larger value of CMICx v1 and v2 */
#define CMIC_SCHAN_WORDS_ALLOC  BCMBD_CMICX2_SCHAN_WORDS

typedef uint32_t schan_header_t;

#define SCMH_NACK_GET(d)        F32_GET(d,0,1)
#define SCMH_NACK_SET(d,v)      F32_SET(d,0,1,v)
#define SCMH_BANK_GET(d)        F32_GET(d,1,2)
#define SCMH_BANK_SET(d,v)      F32_SET(d,1,2,v)
#define SCMH_DMA_GET(d)         F32_GET(d,3,1)
#define SCMH_DMA_SET(d,v)       F32_SET(d,3,1,v)
#define SCMH_ECODE_GET(d)       F32_GET(d,4,2)
#define SCMH_ECODE_SET(d,v)     F32_SET(d,4,2,v)
#define SCMH_EBIT_GET(d)        F32_GET(d,6,1)
#define SCMH_EBIT_SET(d,v)      F32_SET(d,6,1,v)
#define SCMH_DATALEN_GET(d)     F32_GET(d,7,7)
#define SCMH_DATALEN_SET(d,v)   F32_SET(d,7,7,v)
#define SCMH_ACCTYPE_GET(d)     F32_GET(d,14,5)
#define SCMH_ACCTYPE_SET(d,v)   F32_SET(d,14,5,v)
#define SCMH_DSTBLK_GET(d)      F32_GET(d,19,7)
#define SCMH_DSTBLK_SET(d,v)    F32_SET(d,19,7,v)
#define SCMH_OPCODE_GET(d)      F32_GET(d,26,6)
#define SCMH_OPCODE_SET(d,v)    F32_SET(d,26,6,v)

/*
 * Individual S-Channel message formats.
 * Different ways of peeking and poking at an S-Channel message
 * packet.  Applicable message types are listed inside each structure.
 */

typedef struct schan_msg_plain_s {
    /* GBP Full Notification */
    /* GBP Available Notification */
    /* Write Memory Ack */
    /* Write Register Ack */
    /* ARL Insert Complete */
    /* ARL Delete Complete */
    /* Memory Failed Notification */
    /* Initialize CFAP (Cell FAP) */
    /* Initialize SFAP (Slot FAP) */
    /* Enter Debug Mode */
    /* Exit Debug Mode */
    schan_header_t header;
} schan_msg_plain_t;

typedef struct schan_msg_bitmap_s {
    /* Back Pressure Warning Status */
    /* Back Pressure Discard Status */
    /* Link Status Notification (except 5695) */
    /* COS Queue Status Notification */
    /* HOL Status Notification */
    schan_header_t header;
    uint32_t bitmap;
    uint32_t bitmap_word1;  /* 5665 only, so far */
} schan_msg_bitmap_t;

typedef struct schan_msg_readcmd_s {
    /* Read Memory Command */
    /* Read Register Command */
    schan_header_t header;
    uint32_t address;
} schan_msg_readcmd_t;

typedef struct schan_msg_readresp_s {
    /* Read Memory Ack */
    /* Read Register Ack */
    schan_header_t header;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_readresp_t;

typedef struct schan_msg_writecmd_s {
    /* Write Memory Command */
    /* Write Register Command */
    schan_header_t header;
    uint32_t address;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_writecmd_t;

typedef struct schan_msg_arlins_s {
    /* ARL Insert Command */
    /* (Also: ARL Message Buffer Format) */
    /* (Also: ARL DMA Message Format) */
    schan_header_t header;
    uint32_t data[3];
} schan_msg_arlins_t;

typedef struct schan_msg_arldel_s {
    /* ARL Delete Command */
    schan_header_t header;
    uint32_t data[2];
} schan_msg_arldel_t;

typedef struct schan_msg_arllkup_s {
    /* ARL Lookup Command */
    schan_header_t header;
    uint32_t address;
    uint32_t data[2];
} schan_msg_arllkup_t;

typedef struct schan_msg_l3ins_s {
    /* L3 Insert Command */
    schan_header_t header;
    uint32_t data[4];
} schan_msg_l3ins_t;

typedef struct schan_msg_l3del_s {
    /* L3 Delete Command */
    schan_header_t header;
    uint32_t data[4];
} schan_msg_l3del_t;

typedef struct schan_msg_l3lkup_s {
    /* L3 Lookup Command */
    schan_header_t header;
    uint32_t address;
    uint32_t data[4];
} schan_msg_l3lkup_t;

typedef struct schan_msg_l2x2_s {
    /* L2 Insert/Delete/Lookup Command 56120 */
    schan_header_t header;
    uint32_t data[3];
} schan_msg_l2x2_t;

typedef struct schan_msg_l3x2_s {
    /* L3 Insert/Delete/Lookup Command 56120 */
    schan_header_t header;
    uint32_t data[13];
} schan_msg_l3x2_t;

typedef struct schan_msg_gencmd_s {
    /* Generic table Insert/Delete/Lookup Command 5661x */
    schan_header_t header;
    uint32_t address;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_gencmd_t;

typedef struct schan_msg_genresp_s {
    /* Generic table Insert/Delete/Lookup Command 5661x */
    schan_header_t header;
    uint32_t response;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_genresp_t;

typedef struct schan_msg_popcmd_s {
    /* Pop Memory Command */
    schan_header_t header;
    uint32_t address;
} schan_msg_popcmd_t;

typedef struct schan_msg_popresp_s {
    /* Pop Memory Response */
    schan_header_t header;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_popresp_t;

typedef struct schan_msg_pushcmd_s {
    /* Push Memory Command */
    schan_header_t header;
    uint32_t address;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_pushcmd_t;

typedef struct schan_msg_pushresp_s {
    /* Push Memory Response */
    schan_header_t header;
    uint32_t data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_pushresp_t;

/*
 * Union of all S-Channel message types (use to declare all message buffers)
 *
 * When building messages, address the union according to packet type.
 * When writing to PCI, address data.dwords.
 * When writing to I2C, address data.bytes.
 */

#define SCHAN_MSG_CLEAR(m)      ((m)->header = 0)

typedef union schan_msg_u {
    schan_header_t header;
    schan_msg_plain_t plain;
    schan_msg_bitmap_t bitmap;
    schan_msg_readcmd_t readcmd;
    schan_msg_readresp_t readresp;
    schan_msg_writecmd_t writecmd;
    schan_msg_arlins_t arlins;
    schan_msg_arldel_t arldel;
    schan_msg_arllkup_t arllkup;
    schan_msg_l3ins_t l3ins;
    schan_msg_l3del_t l3del;
    schan_msg_l3lkup_t l3lkup;
    schan_msg_l2x2_t l2x2;
    schan_msg_l3x2_t l3x2;
    schan_msg_gencmd_t gencmd;
    schan_msg_genresp_t genresp;
    schan_msg_popcmd_t  popcmd;
    schan_msg_popresp_t popresp;
    schan_msg_pushcmd_t  pushcmd;
    schan_msg_pushresp_t pushresp;
    uint32_t dwords[CMIC_SCHAN_WORDS_ALLOC];
    uint8_t bytes[sizeof(uint32_t) * CMIC_SCHAN_WORDS_ALLOC];
} schan_msg_t;

/*
 * S-Channel Message Types
 */
#define BP_WARN_STATUS_MSG            0x01
#define BP_DISCARD_STATUS_MSG         0x02
#define COS_QSTAT_NOTIFY_MSG          0x03
#define IPIC_HOL_STAT_MSG             0x03
#define HOL_STAT_NOTIFY_MSG           0x04
#define GBP_FULL_NOTIFY_MSG           0x05
#define GBP_AVAIL_NOTIFY_MSG          0x06
#define READ_MEMORY_CMD_MSG           0x07
#define READ_MEMORY_ACK_MSG           0x08
#define WRITE_MEMORY_CMD_MSG          0x09
#define WRITE_MEMORY_ACK_MSG          0x0a
#define READ_REGISTER_CMD_MSG         0x0b
#define READ_REGISTER_ACK_MSG         0x0c
#define WRITE_REGISTER_CMD_MSG        0x0d
#define WRITE_REGISTER_ACK_MSG        0x0e
#define ARL_INSERT_CMD_MSG            0x0f
#define ARL_INSERT_DONE_MSG           0x10
#define ARL_DELETE_CMD_MSG            0x11
#define ARL_DELETE_DONE_MSG           0x12
#define LINKSTAT_NOTIFY_MSG           0x13
#define MEMORY_FAIL_NOTIFY            0x14
#define INIT_CFAP_MSG                 0x15
#define IPIC_GBP_FULL_MSG             0x15
#define INIT_SFAP_MSG                 0x16
#define IPIC_GBP_AVAIL_MSG            0x16
#define ENTER_DEBUG_MODE_MSG          0x17
#define EXIT_DEBUG_MODE_MSG           0x18
#define ARL_LOOKUP_CMD_MSG            0x19
#define L3_INSERT_CMD_MSG             0x1a
#define L3_INSERT_DONE_MSG            0x1b
#define L3_DELETE_CMD_MSG             0x1c
#define L3_DELETE_DONE_MSG            0x1d
#define L3_LOOKUP_CMD_MSG             0x1e
#define L2_LOOKUP_CMD_MSG             0x20
#define L2_LOOKUP_ACK_MSG             0x21
#define L3X2_LOOKUP_CMD_MSG           0x22
#define L3X2_LOOKUP_ACK_MSG           0x23
#define TABLE_INSERT_CMD_MSG          0x24
#define TABLE_INSERT_DONE_MSG         0x25
#define TABLE_DELETE_CMD_MSG          0x26
#define TABLE_DELETE_DONE_MSG         0x27
#define TABLE_LOOKUP_CMD_MSG          0x28
#define TABLE_LOOKUP_DONE_MSG         0x29
#define FIFO_POP_CMD_MSG              0x2a
#define FIFO_POP_DONE_MSG             0x2b
#define FIFO_PUSH_CMD_MSG             0x2c
#define FIFO_PUSH_DONE_MSG            0x2d

#define CMICX_SCHAN_NUM_MAX      3

/* Transact an S-Channel message to CMIC */
extern int bcmbd_cmicx_schan_op(int unit, schan_msg_t *msg,
                                int dwc_write, int dwc_read);

/*! \endcond */

#endif /* BCMBD_CMICX_SCHAN_H */
