/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * S-Channel Message: data structure used by firmware to transport
 * S-Channel Data into SOC via CMIC.
 */

#ifndef _SOC_SCHAN_H
#define _SOC_SCHAN_H

#include <soc/types.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
#define CMIC_SCHAN_WORDS(unit)  \
    ((soc_feature(unit, soc_feature_schmsg_alias) || soc_feature(unit, soc_feature_cmicm) || soc_feature(unit, soc_feature_cmicx)) ? \
    ((soc_feature(unit, soc_feature_sbus_width_120B)) ? 32 : 22) : 20)
extern int fschan_wait_idle(int unit);
#else
#define CMIC_SCHAN_WORDS(unit)  \
    (soc_feature(unit, soc_feature_schmsg_alias) ? 22 : 20)
#endif

/* number of words to use when allocating space */
/** Iproc17 sbus data increased to 120 bytes */
#define CMIC_SCHAN_WORDS_ALLOC 32

/*
 * The endianness of the host is taken into account by the routines
 * which copy S-Channel messages into and out of PCI space as 32-bit
 * words.  Unfortunately, the compiler also switches bit field packing
 * order according to host endianness.  We must undo this "feature" by
 * giving the fields in both orders.
 */

typedef union schan_header_s {
    struct v2_s {
#if defined(LE_HOST)
        uint32 nack:1;
        uint32 bank_ignore_mask:2;
        uint32 dma:1;
        uint32 ecode: 2;
        uint32 err:1;
        uint32 data_byte_len:7;
        uint32 src_blk:6;
        uint32 dst_blk:6;
        uint32 opcode:6;
#else
        uint32 opcode:6;
        uint32 dst_blk:6;
        uint32 src_blk:6;
        uint32 data_byte_len:7;
        uint32 err:1;
        uint32 ecode: 2;
        uint32 dma:1;
        uint32 bank_ignore_mask:2;
        uint32 nack:1;
#endif
    } v2;
    struct v3_s {
#if defined(LE_HOST)
        uint32 nack:1;
        uint32 bank_ignore_mask:2;
        uint32 dma:1;
        uint32 ecode: 2;
        uint32 err:1;
        uint32 data_byte_len:7;
        uint32 acc_type:3;
        uint32 :3;
        uint32 dst_blk:6;
        uint32 opcode:6;
#else
        uint32 opcode:6;
        uint32 dst_blk:6;
        uint32 :3;
        uint32 acc_type:3;
        uint32 data_byte_len:7;
        uint32 err:1;
        uint32 ecode: 2;
        uint32 dma:1;
        uint32 bank_ignore_mask:2;
        uint32 nack:1;
#endif
    } v3;
    struct v4_s {
#if defined(LE_HOST)
        uint32 nack:1;
        uint32 bank_ignore_mask:2;
        uint32 dma:1;
        uint32 ecode: 2;
        uint32 err:1;
        uint32 data_byte_len:7;
        uint32 acc_type:5;
        uint32 dst_blk:7;
        uint32 opcode:6;
#else
        uint32 opcode:6;
        uint32 dst_blk:7;
        uint32 acc_type:5;
        uint32 data_byte_len:7;
        uint32 err:1;
        uint32 ecode: 2;
        uint32 dma:1;
        uint32 bank_ignore_mask:2;
        uint32 nack:1;
#endif
    } v4;
    uint32 word;
} schan_header_t;

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
    uint32 bitmap;
    uint32 bitmap_word1;  /* 5665 only, so far */
} schan_msg_bitmap_t;

typedef struct schan_msg_readcmd_s {
    /* Read Memory Command */
    /* Read Register Command */
    schan_header_t header;
    uint32 address;
} schan_msg_readcmd_t;

typedef struct schan_msg_readresp_s {
    /* Read Memory Ack */
    /* Read Register Ack */
    schan_header_t header;
    uint32 data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_readresp_t;

typedef struct schan_msg_writecmd_s {
    /* Write Memory Command */
    /* Write Register Command */
    schan_header_t header;
    uint32 address;
    uint32 data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_writecmd_t;

typedef struct schan_msg_arlins_s {
    /* ARL Insert Command */
    /* (Also: ARL Message Buffer Format) */
    /* (Also: ARL DMA Message Format) */
    schan_header_t header;
    uint32 data[3];
} schan_msg_arlins_t;

typedef struct schan_msg_arldel_s {
    /* ARL Delete Command */
    schan_header_t header;
    uint32 data[2];
} schan_msg_arldel_t;

typedef struct schan_msg_arllkup_s {
    /* ARL Lookup Command */
    schan_header_t header;
    uint32 address;
    uint32 data[2];
} schan_msg_arllkup_t;

typedef struct schan_msg_l3ins_s {
    /* L3 Insert Command */
    schan_header_t header;
    uint32 data[4];
} schan_msg_l3ins_t;

typedef struct schan_msg_l3del_s {
    /* L3 Delete Command */
    schan_header_t header;
    uint32 data[4];
} schan_msg_l3del_t;

typedef struct schan_msg_l3lkup_s {
    /* L3 Lookup Command */
    schan_header_t header;
    uint32 address;
    uint32 data[4];
} schan_msg_l3lkup_t;

typedef struct schan_msg_l2x2_s {
    /* L2 Insert/Delete/Lookup Command 56504 / 5630x / 5610x */
    schan_header_t header;
    uint32 data[3];
} schan_msg_l2x2_t;

typedef struct schan_msg_l3x2_s {
    /* L3 Insert/Delete/Lookup Command 56504 / 5630x / 5610x */
    schan_header_t header;
    uint32 data[13];
} schan_msg_l3x2_t;

#define SCHAN_GEN_RESP_TYPE_FOUND           0
#define SCHAN_GEN_RESP_TYPE_NOT_FOUND       1
#define SCHAN_GEN_RESP_TYPE_FULL            2
#define SCHAN_GEN_RESP_TYPE_INSERTED        3
#define SCHAN_GEN_RESP_TYPE_REPLACED        4
#define SCHAN_GEN_RESP_TYPE_DELETED         5
#define SCHAN_GEN_RESP_TYPE_ENTRY_IS_OLD    6
#define SCHAN_GEN_RESP_TYPE_CLEARED_VALID   7
#define SCHAN_GEN_RESP_TYPE_L2_FIFO_FULL    8
#define SCHAN_GEN_RESP_TYPE_MAC_LIMIT_THRE  9
#define SCHAN_GEN_RESP_TYPE_MAC_LIMIT_DEL   10
#define SCHAN_GEN_RESP_TYPE_L2_STATIC       11

#define SCHAN_GEN_RESP_L2_MOD_FIFO_FULL     6
#ifdef BCM_ISM_SUPPORT
#define SCHAN_GEN_RESP_MAC_LIMIT_THRESHOLD  7
#define SCHAN_GEN_RESP_MAC_LIMIT_DELETE     8
#endif
#define SCHAN_GEN_RESP_TYPE_ERROR           15

#define SCHAN_GEN_RESP_ERR_INFO_ERROR_IN_HASH_TABLE 0
#define SCHAN_GEN_RESP_ERR_INFO_ERROR_IN_LP_TABLE 1


#define SCHAN_GEN_RESP_ERROR_BUSY           -1
#define SCHAN_GEN_RESP_ERROR_PARITY         -1

typedef struct schan_genresp_s {
#if defined(LE_HOST) 
    uint32 index:20,
           r0:1,            /* Reserved */
           err_info:4,      /* SCHAN_GEN_RESP_ERROR_* */
           r1:1,            /* Reserved */
           type:4,          /* SCHAN_GEN_RESP_TYPE_* */
           src:2;
#else
    uint32 src:2,
           type:4,          /* SCHAN_GEN_RESP_TYPE_* */
           r0:1,              /* Reserved */
           err_info:4,      /* SCHAN_GEN_RESP_ERROR_* */
           r1:1,              /* Reserved */
           index:20; 
#endif 
} schan_genresp_t;


typedef struct schan_genresp_v2_s {
#if defined(LE_HOST) 
    uint32 index:20,
           r0:4,            /* Reserved */
           err_info:4,      /* SCHAN_GEN_RESP_ERROR_* */
           type:4;          /* SCHAN_GEN_RESP_TYPE_* */
#else
    uint32 type:4,          /* SCHAN_GEN_RESP_TYPE_* */
           err_info:4,      /* SCHAN_GEN_RESP_ERROR_* */
           r0:4,            /* Reserved */
           index:20; 
#endif 
} schan_genresp_v2_t;

typedef struct schan_msg_gencmd_s {
    /* Generic table Insert/Delete/Lookup Command 5662x */
    schan_header_t header;
    uint32 address;
    uint32 data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_gencmd_t;

#define SOC_HASH_BANK_MASK_ISM    0xfffff
#define SOC_HASH_BANK_MASK_SHARED 0xfff

typedef struct schan_msg_genresp_s {
    /* Generic table Insert/Delete/Lookup Command 5662x */
    schan_header_t header;
    schan_genresp_t response;
    uint32 data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_genresp_t;

typedef struct schan_msg_genresp_v2_s {
    /* Generic table Insert/Delete/Lookup Command 5664x */
    schan_header_t header;
    schan_genresp_v2_t response;
    uint32 data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_genresp_v2_t;

typedef struct schan_msg_popcmd_s {
    /* Pop Memory Command */
    schan_header_t header;
    uint32 address;
} schan_msg_popcmd_t;

typedef struct schan_msg_popresp_s {
    /* Pop Memory Response */
    schan_header_t header;
    uint32 data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_popresp_t;

typedef struct schan_msg_pushcmd_s {
    /* Push Memory Command */
    schan_header_t header;
    uint32 address;
    uint32 data[CMIC_SCHAN_WORDS_ALLOC - 2];
} schan_msg_pushcmd_t;

typedef struct schan_msg_pushresp_s {
    /* Push Memory Response */
    schan_header_t header;
    uint32 data[CMIC_SCHAN_WORDS_ALLOC - 1];
} schan_msg_pushresp_t;

/*
 * Union of all S-Channel message types (use to declare all message buffers)
 *
 * When building messages, address the union according to packet type.
 * When writing to PCI, address data.dwords.
 * When writing to I2C, address data.bytes.
 */

#define schan_msg_clear(m)      ((m)->header_dword = 0)

typedef union schan_msg_u {
    schan_header_t header;
    uint32 header_dword;
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
    schan_msg_l2x2_t    l2x2;
    schan_msg_l3x2_t    l3x2;
    schan_msg_gencmd_t  gencmd;
    schan_msg_genresp_t genresp;
    schan_msg_genresp_v2_t genresp_v2;
    schan_msg_popcmd_t  popcmd;
    schan_msg_popresp_t popresp;
    schan_msg_pushcmd_t  pushcmd;
    schan_msg_pushresp_t pushresp;
    uint32 dwords[CMIC_SCHAN_WORDS_ALLOC];
    uint8 bytes[sizeof(uint32) * CMIC_SCHAN_WORDS_ALLOC];
} schan_msg_t;

/*
 * S-Channel Message Types
 */

#define BP_WARN_STATUS_MSG            0x01
#define BP_DISCARD_STATUS_MSG         0x02
#define COS_QSTAT_NOTIFY_MSG          0x03      /* Not on XGS */
#define IPIC_HOL_STAT_MSG             0x03      /* 5665 (alias) */
#define HOL_STAT_NOTIFY_MSG           0x04
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
#define LINKSTAT_NOTIFY_MSG           0x13      /* Strata I/II only */
#define MEMORY_FAIL_NOTIFY            0x14
#define INIT_CFAP_MSG                 0x15      /* 5690 only */
#define IPIC_GBP_FULL_MSG             0x15      /* 5665 (alias) */
#define IPIC_GBP_AVAIL_MSG            0x16      /* 5665 (alias) */
#define ENTER_DEBUG_MODE_MSG          0x17
#define EXIT_DEBUG_MODE_MSG           0x18
#define ARL_LOOKUP_CMD_MSG            0x19
#define L3_INSERT_CMD_MSG             0x1a
#define L3_INSERT_DONE_MSG            0x1b
#define L3_DELETE_CMD_MSG             0x1c
#define L3_DELETE_DONE_MSG            0x1d
#define L3_LOOKUP_CMD_MSG             0x1e      /* 5695 */
#define L2_LOOKUP_CMD_MSG             0x20      /* 56504 / 5630x / 5610x */
#define L2_LOOKUP_ACK_MSG             0x21      /* 56504 / 5630x / 5610x */
#define L3X2_LOOKUP_CMD_MSG           0x22      /* 56504 / 5630x / 5610x */
#define L3X2_LOOKUP_ACK_MSG           0x23      /* 56504 / 5630x / 5610x */
/* New for 5662x (see soc_feature_generic_table_ops) */
#define TABLE_INSERT_CMD_MSG          0x24      /* 5662x */
#define TABLE_INSERT_DONE_MSG         0x25      /* 5662x */
#define TABLE_DELETE_CMD_MSG          0x26      /* 5662x */
#define TABLE_DELETE_DONE_MSG         0x27      /* 5662x */
#define TABLE_LOOKUP_CMD_MSG          0x28      /* 5662x */
#define TABLE_LOOKUP_DONE_MSG         0x29      /* 5662x */
#define FIFO_POP_CMD_MSG              0x2a      /* 5662x */
#define FIFO_POP_DONE_MSG             0x2b      /* 5662x */
#define FIFO_PUSH_CMD_MSG             0x2c      /* 5662x */
#define FIFO_PUSH_DONE_MSG            0x2d      /* 5662x */


/*
 * Schan error types
 */
typedef enum soc_schan_err_e {
    SOC_SCERR_CFAP_OVER_UNDER,
    SOC_SCERR_SDRAM_CHKSUM,
    SOC_SCERR_UNEXP_FIRST_CELL,
    SOC_SCERR_MMU_SOFT_RST,
    SOC_SCERR_CBP_CELL_CRC,
    SOC_SCERR_CBP_HEADER_PARITY,
    SOC_SCERR_MMU_NPKT_CELLS,
    SOC_SCERR_MEMORY_PARITY,
    SOC_SCERR_PLL_DLL_LOCK_LOSS,
    SOC_SCERR_CELL_PTR_CRC,
    SOC_SCERR_CELL_DATA_CRC,
    SOC_SCERR_FRAME_DATA_CRC,
    SOC_SCERR_CELL_PTR_BLOCK_CRC,
    SOC_SCERR_MULTIPLE_ERR,
    SOC_SCERR_INVALID
} soc_schan_err_t;

/* SCHAN flags for interrupts and not allowing timeout prints */
#define SCHAN_OP_FLAG_DEFAULT                    0
#define SCHAN_OP_FLAG_ALLOW_INTERRUPTS           1
#define SCHAN_OP_FLAG_DO_NOT_LOG_TIMEOUTS        2
#define SCHAN_OP_FLAG_ALLOW_INDIRECT_ACCESS_READ 4

/*******************************************
* @function soc_schan_header_cmd_set
* purpose API to set the command header
*
* @param handle [in] unit
* @param header [out] Populated Command header pointer
* @param opcode [in] Opcode
* @param dst_blk [in] Block ID
* @param acc_type [in] Acc Type
* @param data_byte_len [in] Dlen
* @param dma, bank_ignore_mask [in] Misc Ctrl/ Status
* @returns None
*
* @end
 */
extern void soc_schan_header_cmd_set(int unit, schan_header_t *header,
                                     int opcode, int dst_blk, int src_blk,
                                     int acc_type, int data_byte_len, int dma,
                                     uint32 bank_ignore_mask);

/*******************************************
* @function soc_schan_header_cmd_get
* purpose API to get the command header
*
* @param handle [in] unit
* @param header [in] Populated Command header pointer
* @param opcode [out] Opcode
* @param dst_blk [out] Block ID
* @param acc_type [out] Acc Type
* @param data_byte_len [out] Dlen
* @param dma, bank_ignore_mask [out] Misc Ctrl/ Status
* @returns None
*
* @end
 */
extern void soc_schan_header_cmd_get(int unit, schan_header_t *header,
                                     int *opcode, int *dst_blk, int *src_blk,
                                     int *acc_type, int *data_byte_len,
                                     int *dma, uint32 *bank_ignore_mask);

/*******************************************
* @function soc_schan_header_status_get
* purpose API to get the header status
*
* @param handle [in] unit
* @param header [in] Populated Command header pointer
* @param opcode [out] Opcode
* @param dst_blk [out] Block ID
* @param acc_type [out] Acc Type
* @param data_byte_len [out] Dlen
* @param err, ecode, nack [out] Misc Ctrl/ Status
* @returns None
*
* @end
 */
extern void soc_schan_header_status_get(int unit, schan_header_t *header,
                                        int *opcode, int *src_blk,
                                        int *data_byte_len, int *err,
                                        int *ecode, int *nack);
/*******************************************
* @function soc_schan_op
* purpose SDK-6 API used to initiate the schan operation
*
* @param handle [in] unit
* @param msg [in] SCHAN message pointer
* @param dwc_write [in] Number of messages to write
* @param dwc_read [in] Number of messages to read
* @param intr [in] Interrupt
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX

* @end
 */
extern int soc_schan_op(
         int unit,
         schan_msg_t *msg,
         int dwc_write,
         int dwc_read,
         uint32 flags);

/*******************************************
* @function soc_schan_dump
* @Purpose Dump an S-Channel message for debugging
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern void
soc_schan_dump(int unit, schan_msg_t *msg, int dwc);


/*******************************************
* @function soc_schan_init
* purpose API to Initialize and config SCHAN
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int soc_schan_init(int unit);

/*******************************************
* @function soc_schan_deinit
* purpose SDK-7 API to de-Initialize SCHAN
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int soc_schan_deinit(int unit);

/*******************************************
* @function soc_schan_override_enable
* @Purpose Enable schan override
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_schan_override_enable(int unit);

/*******************************************
* @function soc_schan_override_disable
* @Purpose Disable schan override
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
extern int
soc_schan_override_disable(int unit);

/*
 * Structures and prototypes related to PEM block access.
 * {
 */
typedef struct phy_mem_s {
  unsigned int   reserve ;
  unsigned int   block_identifier ;
  unsigned int   mem_address ;
  unsigned int   row_index ;
  unsigned int   mem_width_in_bits ;
  unsigned int   is_reg;
  unsigned int   is_industrial_tcam;
} phy_mem_t ;

int phy_mem_read(int unit, phy_mem_t *mem, void *entry_data) ;
int phy_mem_write(int unit, phy_mem_t *mem, void *entry_data) ;

/*
 * }
 */
#endif  /* !_SOC_SCHAN_H */
