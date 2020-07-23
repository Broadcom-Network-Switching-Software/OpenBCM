/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: soc_ser_log.h
 * Purpose: SER logging using a circular buffer
 */

#ifndef _SOC_SER_LOG_H_
#define _SOC_SER_LOG_H_

#include <soc/mem.h>

typedef enum {
    SOC_PARITY_TYPE_NONE,
    SOC_PARITY_TYPE_GENERIC,
    SOC_PARITY_TYPE_PARITY,
    SOC_PARITY_TYPE_ECC,
    SOC_PARITY_TYPE_HASH,
    SOC_PARITY_TYPE_EDATABUF,
    SOC_PARITY_TYPE_COUNTER,
    SOC_PARITY_TYPE_MXQPORT,
    SOC_PARITY_TYPE_SER
} soc_ser_parity_type_t;

typedef enum {
    SOC_SER_UNKNOWN = -1,
    SOC_SER_UNCORRECTED = 0,
    SOC_SER_CORRECTED = 1
} soc_ser_correct_t;

/* Used to indicate the format of the data/value portion of a TLV */
typedef enum {
    SOC_SER_LOG_TLV_NONE = 0,
    SOC_SER_LOG_TLV_MEMORY = 1,
    SOC_SER_LOG_TLV_REGISTER = 2,
    SOC_SER_LOG_TLV_CONTENTS = 3,
    SOC_SER_LOG_TLV_SER_FIFO = 4,
    SOC_SER_LOG_TLV_CACHE = 5,
    SOC_SER_LOG_TLV_GENERIC = 6
} soc_ser_log_tlv_type_t;

typedef enum {
    SOC_SER_LOG_ACC_ANY = -1,
    SOC_SER_LOG_ACC_GROUP = 0,
    SOC_SER_LOG_ACC_X = 1,
    SOC_SER_LOG_ACC_Y = 2,
    SOC_SER_LOG_ACC_SBS = 6
} soc_ser_log_acc_type_t;

/* Type length value header for elements of the log */
typedef struct soc_ser_log_tlv_hdr_s {
    int type;
    int length;
} soc_ser_log_tlv_hdr_t;

/* Data/value portion of memory TLVs */
typedef struct soc_ser_log_tlv_memory_s {
    soc_mem_t memory;
    int index;
    uint32 hwbase;
} soc_ser_log_tlv_memory_t;

/* Data/value portion of register TLVs */
typedef struct soc_ser_log_tlv_register_s {
    soc_reg_t reg;
    int index;
    int port;
} soc_ser_log_tlv_register_t;

typedef enum {
    SOC_SER_LOG_FLAG_ERR_SRC = 1,
    SOC_SER_LOG_FLAG_MULTIBIT = 1 << 1,
    SOC_SER_LOG_FLAG_DOUBLEBIT = 1 << 2
} soc_ser_log_flag_t;

/* Data/value portion of generic TLVs */
typedef struct soc_ser_log_tlv_generic_s {
    soc_ser_log_flag_t      flags;
    sal_usecs_t             time;
    uint8                   boot_count;
    uint32                  address;
    soc_ser_log_acc_type_t  acc_type;
    soc_block_t             block_type;
    soc_ser_parity_type_t   parity_type;
    int                     ser_response_flag;
    int                     corrected;
    int                     pipe_num;
    int                     inst;
    uint8                   hw_cache; /* Recover from other pipe */
} soc_ser_log_tlv_generic_t;

#define _SOC_SER_LOG_BUFFER_SIZE 5000

extern int soc_ser_log_init(int unit, void *location, int size);
extern int soc_ser_log_invalidate(int unit);
extern int soc_ser_log_get_entry_size(int unit, int id, int *size);
extern int soc_ser_log_get_entry(int unit, int id, int size, void *entry);
extern int soc_ser_log_create_entry(int unit, int size);
extern int soc_ser_bus_buffer_log_find(int unit, uint32 hwbase);
extern int soc_ser_log_find_recent(int unit, soc_mem_t mem, int index, sal_usecs_t time);
extern int soc_ser_log_print_entry(int unit, void *buffer);
extern int soc_ser_log_add_tlv( int unit, 
                                int id,
                                soc_ser_log_tlv_type_t type, 
                                int size, 
                                void * buffer);

extern int soc_ser_log_get_tlv( int unit, 
                                int id,
                                soc_ser_log_tlv_type_t type, 
                                int size, 
                                void * buffer);

extern int soc_ser_log_mod_tlv( int unit, 
                                int id,
                                soc_ser_log_tlv_type_t type, 
                                int size, 
                                void * buffer);
extern int soc_ser_log_find_tlv(void *buffer, soc_ser_log_tlv_type_t type);
extern int soc_ser_log_print_tlv(int unit, void *buffer);
extern int soc_ser_log_get_boot_count(int unit);
extern int soc_ser_log_print_all(int unit);
extern int soc_ser_log_print_one(int unit, int id);
#endif
