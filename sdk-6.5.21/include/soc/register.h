/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        register.h
 * Purpose:     Base definitions for register types
 * Requires:    
 */

#ifndef _SOC_REGISTER_H
#define _SOC_REGISTER_H

#include <soc/defs.h>
#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)
#include <soc/mcm/allenum.h>
#endif
#include <sal/core/time.h>
#include <soc/field.h>
#include <soc/error.h>

/*
 * Options for soc_reg_addr_get()
 *
 * WRITE          - Indicates this is a call for a write operation, which
 *                  may result in a different block number.
 * PRESERVE_PORT  - Preserve the value of the 'port' argument and
 *                  do not make any translation to the the index
 *                  (e.g. use 'port' value for physical port).
 */
#define SOC_REG_ADDR_OPTION_NONE             (0x0)
#define SOC_REG_ADDR_OPTION_WRITE            (1 << 0)
#define SOC_REG_ADDR_OPTION_PRESERVE_PORT    (1 << 1)


/* argument to soc_reg_addr() */
#define    REG_PORT_ANY        (-10)

/* use bit 31 to indicate the port argument is an instance id */
#define SOC_REG_ADDR_INSTANCE_BP         (31)
#define SOC_REG_ADDR_INSTANCE_MASK       (1<<SOC_REG_ADDR_INSTANCE_BP)
#define SOC_REG_ADDR_SCHAN_ID_MASK       (1<<(SOC_REG_ADDR_INSTANCE_BP-1))
#define SOC_REG_ADDR_BLOCK_ID_MASK       (1<<(SOC_REG_ADDR_INSTANCE_BP-2))
#define SOC_REG_ADDR_PHY_ACC_MASK        (1<<(SOC_REG_ADDR_INSTANCE_BP-3))

#define _SOC_MAX_REGLIST        (2 * SOC_MAX_NUM_PORTS * SOC_MAX_NUM_COS)


#define SOC_REG_FIRST_BLK_TYPE(regblklist) regblklist[0]



#define FE_GET(unit, port, reg, gth_reg, fld, var)  \
    soc_reg_field_get(unit, reg, var, fld)

#define FE_SET(unit, port, reg, gth_reg, fld, var, val)  \
    soc_reg_field_set(unit, reg, &var, fld, val)

#define FE_READ(reg, gth_reg, unit, port, val_p) \
    soc_reg32_get(unit, reg, port, 0, val_p)

#define FE_WRITE(reg, gth_reg, unit, port, val) \
    soc_reg32_set(unit, reg, port, 0, val)

/*************ABOVE 64 REGS******************/

#define SOC_REG_ABOVE_64_MAX_SIZE_U32 20 

typedef uint32 soc_reg_above_64_val_t[SOC_REG_ABOVE_64_MAX_SIZE_U32];

#define SOC_REG_ABOVE_64_SET_PATTERN(reg_above_64_val, patern) \
    sal_memset(reg_above_64_val, patern, SOC_REG_ABOVE_64_MAX_SIZE_U32*4);
    
#define SOC_REG_ABOVE_64_CLEAR(reg_above_64_val) \
    SOC_REG_ABOVE_64_SET_PATTERN(reg_above_64_val, 0);

#define SOC_REG_ABOVE_64_ALLONES(reg_above_64_val) \
    SOC_REG_ABOVE_64_SET_PATTERN(reg_above_64_val, 0xFFFFFFFF);
     
#define SOC_REG_ABOVE_64_CREATE_MASK(reg_above_64_val, len, start) \
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val) \
    SHR_BITSET_RANGE(reg_above_64_val, start, len);
    
#define SOC_REG_ABOVE_64_NOT(reg_above_64_val) \
    SHR_BITNEGATE_RANGE(reg_above_64_val, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32*32, reg_above_64_val);
 
#define SOC_REG_ABOVE_64_AND(reg_above_64_val1, reg_above_64_val2) \
    SHR_BITAND_RANGE(reg_above_64_val1, reg_above_64_val2, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32*32, reg_above_64_val1);   

#define SOC_REG_ABOVE_64_OR(reg_above_64_val1, reg_above_64_val2) \
    SHR_BITOR_RANGE(reg_above_64_val1, reg_above_64_val2, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32*32, reg_above_64_val1);   

#define SOC_REG_ABOVE_64_XOR(reg_above_64_val1, reg_above_64_val2) \
    SHR_BITXOR_RANGE(reg_above_64_val1, reg_above_64_val2, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32*32, reg_above_64_val1);  
    
#define SOC_REG_ABOVE_64_COPY(reg_above_64_val1, reg_above_64_val2) \
    SHR_BITCOPY_RANGE(reg_above_64_val1, 0, reg_above_64_val2, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32*32);
       
#define SOC_REG_ABOVE_64_IS_ZERO(reg_above_64_val) \
    SHR_BITNULL_RANGE(reg_above_64_val, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32*32) 
   
#define SOC_REG_ABOVE_64_IS_EQUAL(reg_above_64_val1, reg_above_64_val2) \
    SHR_BITEQ_RANGE(reg_above_64_val1, reg_above_64_val2, 0, SOC_REG_ABOVE_64_MAX_SIZE_U32*32)   
    
/* 
 *  Copy width bits to reg_above_64_val offset reg_above_64_start_bit from bit array uint32_arr offset uint32_arr_start_bit
 *                                                                                                                                       *
 *  reg_above_64_val[reg_above_64_start_bit:reg_above_64_start_bit + width] = uint32_arr[uint32_arr_start_bit:uint32_arr_start_bit + width]* 
 */
#define SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, reg_above_64_start_bit, uint32_arr,  uint32_arr_start_bit, width) \
    SHR_BITCOPY_RANGE(reg_above_64_val, reg_above_64_start_bit, uint32_arr,  uint32_arr_start_bit, width)

#define SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, uint32_val, uint32_index)               \
    do                                                                                      \
    {                                                                                       \
        uint32 _val = uint32_val;                                                           \
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, uint32_index*32, &_val, 0, 32);       \
    } while(0)

#define SOC_REG_ABOVE_64_SET_WORD_PATTERN(reg_above_64_val, uint32_pattern)                 \
    do                                                                                      \
    {                                                                                       \
        int _i;                                                                             \
        for(_i = 0; _i < SOC_REG_ABOVE_64_MAX_SIZE_U32; ++_i) {                             \
            SOC_REG_ABOVE_64_WORD_SET(reg_above_64_val, uint32_pattern, _i);                \
        }                                                                                   \
    } while(0)

#define SOC_REG_ABOVE_64_WORD_GET(reg_above_64_val, uint32_valp, uint32_index)              \
    do                                                                                      \
    {                                                                                       \
        SHR_BITCOPY_RANGE(uint32_valp, 0, reg_above_64_val, uint32_index*32, 32);           \
    } while(0)

/* The max number of block instances to write to by software, and not by ASIC broadcast writes */
#define SOC_REG_ACCESS_INFO_MAX_NOF_BLOCKS_PER_REG 64
/* register information structure, needed for accessing it using s-channel */
typedef struct soc_reg_access_info_s {
    uint32 offset; /* register offset/address in its block */
    unsigned num_blks; /* The number of s-channel blocks */
    int blk_list[SOC_REG_ACCESS_INFO_MAX_NOF_BLOCKS_PER_REG]; /* The s-channel block IDs of the register, usually just one */
    uint8 acc_type; /* access type */
} soc_reg_access_info_t;

extern int soc_reg_above_64_get(int unit, soc_reg_t reg, int port, int index, 
                soc_reg_above_64_val_t data);
extern int soc_reg_above_64_set(int unit, soc_reg_t reg, int port, int index, 
                soc_reg_above_64_val_t data);
extern void soc_reg_above_64_field_get(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval, 
                soc_field_t field, soc_reg_above_64_val_t field_val);
extern int soc_reg_above_64_field_read(int unit, soc_reg_t reg, soc_port_t port, int index, soc_field_t field, soc_reg_above_64_val_t out_field_val);
extern void soc_reg_above_64_field_set(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval,
                soc_field_t field, CONST soc_reg_above_64_val_t value);
extern uint32 soc_reg_above_64_field32_get(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval, soc_field_t field);
extern int soc_reg_above_64_field32_read(int unit, soc_reg_t reg, soc_port_t port, int index, soc_field_t field, uint32* out_field_val);
extern void soc_reg_above_64_field32_set(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval, soc_field_t field, uint32 value);
extern uint64 soc_reg_above_64_field64_get(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval, soc_field_t field);
extern int soc_reg_above_64_field64_read(int unit, soc_reg_t reg, soc_port_t port, int index, soc_field_t field, uint64* out_field_val);
extern int SOC_REG_IS_DYNAMIC(int unit, soc_reg_t reg);

/******************************************/

/* SOC Register Routines */

extern int soc_reg_field_length(int unit, soc_reg_t reg, soc_field_t field);
extern uint32 soc_reg_field_get(int unit, soc_reg_t reg, uint32 regval,
             soc_field_t field);
extern int soc_ftmh_cfg_get(int unit, int * p_cfg_ftmh_lb_key_ext_en,
                         int * p_cfg_ftmh_stacking_ext_enable);
extern uint64 soc_reg64_field_get(int unit, soc_reg_t reg, uint64 regval,
               soc_field_t field);
extern void soc_reg_set_field_if_exists(int unit, soc_reg_t reg, uint32 *regval,
                  soc_field_t field, uint32 value);
extern void soc_reg_field_set(int unit, soc_reg_t reg, uint32 *regval,
                  soc_field_t field, uint32 value);
extern void soc_reg64_field_set(int unit, soc_reg_t reg, uint64 *regval,
                soc_field_t field, uint64 value);
extern uint32 soc_reg64_field32_get(int unit, soc_reg_t reg, uint64 regval,
               soc_field_t field);
extern void soc_reg64_field32_set(int unit, soc_reg_t reg, uint64 *regval,
                  soc_field_t field, uint32 value);
extern void soc_reg_above_64_field64_set(int unit, soc_reg_t reg, soc_reg_above_64_val_t regval,
                  soc_field_t field, uint64 value);
extern int soc_reg_field_validate(int unit, soc_reg_t reg, soc_field_t field, uint32 value);
extern int soc_reg64_field_validate(int unit, soc_reg_t reg, soc_field_t field, uint64 value);
extern int soc_reg_signed_field_mask(int unit, soc_reg_t reg, soc_field_t field, 
                                     int32 value_in, uint32 *value_out);
extern int soc_reg_unsigned_field_mask(int unit, soc_reg_t reg, soc_field_t field,
                                     int32 value_in, uint32 *value_out);
extern uint32 soc_reg_addr(int unit, soc_reg_t reg, int port, int index);
extern uint32 soc_reg_addr_get(int unit, soc_reg_t reg, int port, int index,
                               uint32 options,
                               int *blk, uint8 *acc_type);
extern int soc_reg_xaddr_get(int unit, soc_reg_t reg, int port, int index,
                         uint32 options, soc_reg_access_info_t *access_info);
extern uint32 soc_reg_datamask(int unit, soc_reg_t reg, int flags);
extern uint64 soc_reg64_datamask(int unit, soc_reg_t reg, int flags);
extern void   soc_reg_above_64_datamask(int unit, soc_reg_t reg, int flags, soc_reg_above_64_val_t datamask);
extern int soc_reg_fields32_modify(int unit, soc_reg_t reg, soc_port_t port,
                                   int field_count, soc_field_t *fields,
                                   uint32 *values);
extern int soc_reg_field32_modify(int unit, soc_reg_t reg, soc_port_t port, 
                                  soc_field_t field, uint32 value);
extern int soc_reg_above_64_field32_modify(int unit, soc_reg_t reg, soc_port_t port, int index, 
                                           soc_field_t field, uint32 value);
extern int soc_reg_above_64_field64_modify(int unit, soc_reg_t reg, soc_port_t port, int index, 
                                           soc_field_t field, uint64 value);
extern int soc_reg_field_valid(int unit, soc_reg_t reg, soc_field_t field);

extern void soc_reg_field_acc_mode_get(int unit, soc_reg_t reg, soc_field_t field, uint32 *is_read_only,
                                       uint32 *is_write_only);
extern int soc_is_reg_flexe_core(int unit, soc_reg_t reg);

/* Get register length in bytes */
extern int soc_reg_bytes(int unit, soc_reg_t reg);
/* Get register length in bits */
extern int soc_reg_bits(int unit, soc_reg_t reg);

#define SOC_REG_FIELD_VALID(_u_,_reg_,_fld_) \
    soc_reg_field_valid(_u_, _reg_, _fld_)

extern int soc_reg_port_idx_valid(int unit, soc_reg_t reg, soc_port_t port, int idx);

#define SOC_REG_PORT_VALID(_u_,_reg_,_port_) \
    soc_reg_port_idx_valid(_u_, _reg_, _port_, 0)
#define SOC_REG_PORT_IDX_VALID(_u_,_reg_,_port_,_idx_) \
        soc_reg_port_idx_valid(_u_, _reg_, _port_, _idx_)

void soc_reg_init(void);

typedef struct soc_large_reg_info_t {
    uint32 size; /*Size in uint32*/
    uint32* reset;
    uint32* mask;
} soc_reg_above_64_info_t;

/* Flags for memory snooping callback register */
#define SOC_REG_SNOOP_WRITE     0x00000001 /* snooping write operations */
#define SOC_REG_SNOOP_READ      0x00000002 /* snooping read operations */


/* additional information for registers which are arrays */
/* To be used if in soc_reg_info_t, the SOC_REG_FLAG_REG_ARRAY bit is set */
typedef struct soc_reg_array_info_t { /* additional information for register which are arrays */
    uint32 element_skip; /* how many bytes to skip between array elements */
} soc_reg_array_info_t;

typedef struct soc_regaddrinfo_t {
    uint32          addr;
    int             valid;
    soc_reg_t       reg;        /* INVALIDr if not used */
    int             idx;        /* If array, this is an index */
    soc_block_t     block;      /* SOC_BLK_NONE if not used */
    soc_port_t      port;       /* -1 if not used */
    soc_cos_t       cos;        /* -1 if not used */
    int             is_custom;  /* custom regs use soc_custom_reg_access_t API's instead of standart soc_reg_set/get */
    uint8           acc_type;
    soc_field_t     field;      /* INVALIDf for entire reg */
} soc_regaddrinfo_t;
/* Routines to register/unregister UserCallBack routine for register snooping */
typedef void (*soc_reg_snoop_cb_t) (int unit, soc_reg_t reg,
                                    soc_regaddrinfo_t *ainfo, uint32 flags,
                                    uint32 data_hi, uint32 data_lo, 
                                    void *user_data);
void soc_reg_snoop_register(int unit, soc_reg_t reg, uint32 flags, 
                           soc_reg_snoop_cb_t snoop_cb, void *user_data);

void soc_reg_snoop_unregister(int unit, soc_reg_t reg); 

#define SOC_REG_FLAG_64_BITS (1<<0)     /* Register is 64 bits wide */
#define SOC_REG_FLAG_COUNTER (1<<1)     /* Register is a counter */
#define SOC_REG_FLAG_ARRAY   (1<<2)     /* Register is an array */
#define SOC_REG_FLAG_NO_DGNL (1<<3)     /* Array does not have diagonal els */
#define SOC_REG_FLAG_RO      (1<<4)     /* Register is write only */
#define SOC_REG_FLAG_WO      (1<<5)     /* Register is read only */
#define SOC_REG_FLAG_ED_CNTR (1<<6)     /* Counter of discard/error */
#define SOC_REG_FLAG_SPECIAL (1<<7)     /* Counter requires special
                                           processing */
#define    SOC_REG_FLAG_EMULATION    (1<<8)    /* Available only in emulation */
#define    SOC_REG_FLAG_VARIANT1    (1<<9)    /* Not available in chip variants  */
#define    SOC_REG_FLAG_VARIANT2    (1<<10)    /* -- '' -- */
#define    SOC_REG_FLAG_VARIANT3    (1<<11)    /* -- '' -- */
#define    SOC_REG_FLAG_VARIANT4    (1<<12)    /* -- '' -- */
#define SOC_REG_FLAG_32_BITS    (1<<13) /* Register is 32 bits wide */
#define SOC_REG_FLAG_16_BITS    (1<<14) /* Register is 16 bits wide */
#define SOC_REG_FLAG_8_BITS     (1<<15) /* Register is 8 bits wide */
#define SOC_REG_FLAG_ARRAY2     (1<<16) /* Register is an array with stride 2*/
#define SOC_REG_FLAG_ARRAY4     (1<<17) /* Register is an array with stride 4*/
#define SOC_REG_FLAG_ABOVE_64_BITS (1<<20)     /* Register is above 64 bits wide */
#define SOC_REG_FLAG_REG_ARRAY  (1<<21)     /* This is a register array with indexed access */
#define SOC_REG_FLAG_INTERRUPT  (1<<22)     /* This is register is an interrupt */
#define SOC_REG_FLAG_GENERAL_COUNTER (1<<23) /* This is a general counter */
#define SOC_REG_FLAG_SIGNAL (1<<24) /* this is signal register */
#define    SOC_REG_FLAG_IGNORE_DEFAULT (1<<25) /* this is register, default value of it should be ignored */
#define SOC_REG_FLAG_ACC_TYPE_MASK   0x1f  /* Access type */
#define SOC_REG_FLAG_ACC_TYPE_SHIFT  26
#define SOC_REG_ACC_TYPE(unit, reg)  ((SOC_REG_INFO(unit, reg).flags >> \
                                       SOC_REG_FLAG_ACC_TYPE_SHIFT) &   \
                                       SOC_REG_FLAG_ACC_TYPE_MASK)
#define SOC_REG_BASE_TYPE(unit, reg)  ((SOC_REG_INFO(unit, reg).offset >> \
                                        23) & 0x7)

/* Bit Definitios for flags1 */
#define SOC_REG_FLAG_CCH (1<<0)     /* Register is CCH */
#define SOC_REG_FLAG_COUNTER_FEILDS (1<<1)     /* Register counter fields */
#define SOC_REG_FLAG_CLEAR_BITS_ON_WRITE (1<<2) /* this is a register that is clearing bits on write */
#define SOC_REG_FLAG_INIT_ON_READ (1<<3) /* this is a register that is initilized on read */
#define SOC_REG_FLAG_ERROR_FEILDS (1<<4) /* Register error counter fields */

typedef struct soc_reg_info_t {
    soc_block_types_t  block;
    soc_regtype_t      regtype;        /* Also indicates invalid */
    int                numels;          /* If array, num els in array. */
                                        /* Otherwise -1. */
    uint32             first_array_index; /* first index of arrays */
    uint32             offset;        /* Includes 2-bit form field */

    uint32             flags;
    uint32             flags1;
    int                nFields;
    soc_field_info_t   *fields;
#if !defined(SOC_NO_RESET_VALS)
    uint32             rst_val_lo;
    uint32             rst_val_hi;      /* 64-bit regs only */
    uint32             rst_mask_lo;     /* 1 where resetVal is valid */
    uint32             rst_mask_hi;     /* 64-bit regs only */
#endif
    int                ctr_idx;         /* Counters only; sw idx */
    int                numelportlist_idx;  /* Per-Port Registers Only */
    uint32             access_latency_mode;  /* accessibility in latency mode */
    int                pipe_stage;  /*  pipe_stage_num of the register */
    soc_reg_snoop_cb_t snoop_cb;         /* UserCallBack for snooping register*/
    void               *snoop_user_data; /* User data for callback function   */
    uint32             snoop_flags;      /* Snooping flags SOC_REG_SNOOP_* */
} soc_reg_info_t;


typedef struct soc_regaddrlist_t {
    int            count;
    soc_regaddrinfo_t    *ainfo;
} soc_regaddrlist_t;

typedef struct soc_numelport_set_t {
    int f_idx;
    int l_idx;
    int pl_idx;
} soc_numelport_set_t;

#ifdef BCM_CMICM_SUPPORT
typedef struct soc_cmicm_reg_t {
    uint32 addr;
    char *name;
    soc_reg_t reg;
} soc_cmicm_reg_t;

extern soc_cmicm_reg_t *soc_cmicm_reg_get(uint32 idx);
extern soc_reg_t soc_cmicm_addr_reg(uint32 addr);
extern char *soc_cmicm_addr_name(uint32 addr);
#endif

#ifdef BCM_CMICX_SUPPORT
typedef struct soc_cmicx_reg_t {
    uint32 addr;
    char *name;
    soc_reg_t reg;
} soc_cmicx_reg_t;

extern soc_cmicx_reg_t *soc_cmicx_reg_get(uint32 idx);
extern soc_reg_t soc_cmicx_addr_reg(int unit, uint32 addr);
extern char *soc_cmicx_addr_name(int unit, uint32 addr);
extern int soc_parse_cmicx_regname(int unit, char *name, uint32 *offset_ptr);
#endif

extern int soc_regaddrlist_alloc(soc_regaddrlist_t *addrlist);
extern int soc_regaddrlist_free(soc_regaddrlist_t *addrlist);

/* Function for register iteration. */
typedef int (*soc_reg_iter_f)(int unit, soc_regaddrinfo_t *ainfo, void *data);

extern void soc_regaddrinfo_get(int unit, soc_regaddrinfo_t *ainfo,
             uint32 addr);
extern void soc_regaddrinfo_extended_get(int unit, soc_regaddrinfo_t *ainfo,
                                         soc_block_t block, int acc_type,
                                         uint32 addr);
extern void soc_cpuregaddrinfo_get(int unit, soc_regaddrinfo_t *ainfo,
                            uint32 addr);

extern int soc_reg_iterate(int unit, soc_reg_iter_f do_it, void *data);

extern void soc_reg_sprint_addr(int unit, char *bp,
                                soc_regaddrinfo_t *ainfo);

extern void soc_reg_sprint_data(int unit, char *bp, char *infix,
                                soc_reg_t reg, uint32 value);


/* In soc_common.c */
extern char *soc_regtypenames[];
extern int soc_regtype_gran[];

/*
 * The following callbacks are used to allow registration of user defined callbacks for
 * registers and memories operations.
 */
typedef int (*soc_reg32_get_f)      (int unit, soc_reg_t reg, int port, int index, uint32* data);
typedef int (*soc_reg64_get_f)      (int unit, soc_reg_t reg, int port, int index, uint64* data);
typedef int (*soc_reg_above64_get_f)(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data);

typedef int (*soc_reg32_set_f)      (int unit, soc_reg_t reg, int port, int index, uint32 data);
typedef int (*soc_reg64_set_f)      (int unit, soc_reg_t reg, int port, int index, uint64 data);
typedef int (*soc_reg_above64_set_f)(int unit, soc_reg_t reg, int port, int index, soc_reg_above_64_val_t data);

typedef int (*soc_mem_array_read_f) (int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);
typedef int (*soc_mem_array_write_f)(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);

typedef int (*soc_mem_array_fill_range_f)(int unit, soc_mem_t mem, unsigned min_ar_index, unsigned max_ar_index, int copyno, int index_min, int index_max, const void *buffer);

typedef struct soc_reg_access_s {
    soc_reg32_get_f         reg32_get;
    soc_reg64_get_f         reg64_get;
    soc_reg_above64_get_f   reg_above64_get;

    soc_reg32_set_f         reg32_set;
    soc_reg64_set_f         reg64_set;
    soc_reg_above64_set_f   reg_above64_set;

    soc_mem_array_read_f    mem_read;
    soc_mem_array_write_f   mem_write;

} soc_reg_access_t;

typedef struct soc_custom_reg_access_s {
    soc_reg32_get_f         custom_reg32_get;
    soc_reg64_get_f         custom_reg64_get;
    soc_reg_above64_get_f   custom_reg_above64_get;

    soc_reg32_set_f         custom_reg32_set;
    soc_reg64_set_f         custom_reg64_set;
    soc_reg_above64_set_f   custom_reg_above64_set;

    soc_mem_array_read_f    custom_mem_array_read;
    soc_mem_array_write_f   custom_mem_array_write;

    soc_mem_array_fill_range_f custom_mem_array_fill_range;

    soc_pbmp_t              custom_port_pbmp;

} soc_custom_reg_access_t;

typedef int (*soc_polling_f)        (int   unit, sal_usecs_t time_out, int32 min_polls, soc_reg_t reg, int32 port, int32 index, soc_field_t field, uint32 expected_value);
typedef int (*soc_direct_reg_set_f) (int unit, int cmic_block, uint32 addr, uint32 dwc_write, uint32 *data);
typedef int (*soc_fast_reg_set_f)   (int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);
typedef int (*soc_fast_reg_get_f)   (int unit, soc_reg_t reg, int acc_type, int addr, int block, soc_reg_above_64_val_t data);

typedef int (*soc_reg_set_f)        (int unit, soc_reg_t reg, int port, int index, uint64 data);
typedef int (*soc_reg_get_f)        (int unit, soc_reg_t reg, int port, int index, uint64 *data);
typedef int (*soc_reg_set_nocache_f)  (int unit, soc_reg_t reg, int port, int index, uint64 data);

int soc_reg32_set_all_instances(int unit, soc_reg_t reg, int array_index, uint32 data);
int soc_reg64_set_all_instances(int unit, soc_reg_t reg, int array_index, uint64 data);
int soc_reg_above_64_set_all_instances(int unit, soc_reg_t reg, int array_index, soc_reg_above_64_val_t data);

#ifdef CRASH_RECOVERY_SUPPORT
/* callbacks specific to CRASH RECOVERY */
typedef int (*soc_mem_sbusdma_write_f) (int unit, int flags, soc_mem_t mem, unsigned array_index_start, unsigned array_index_end, int copyno, int index_begin, int index_end, void *buffer, uint8 mem_clear, int clear_buf_ent, int vchan); 
typedef int (*soc_mem_sbusdma_read_f)  (int unit, soc_mem_t mem, unsigned array_index,
                            int copyno, int index_min, int index_max,
                            uint32 ser_flags, void *buffer, int vchan);

typedef int (*soc_mem_descdma_read_f) (int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);
typedef int (*soc_mem_descdma_write_f)(int unit, soc_mem_t mem, unsigned array_index, int copyno, int index, void *entry_data);


typedef struct soc_hw_log_access_s {
    soc_reg32_get_f         reg32_get;
    soc_reg64_get_f         reg64_get;
    soc_reg_above64_get_f   reg_above64_get;

    soc_reg32_set_f         reg32_set;
    soc_reg64_set_f         reg64_set;
    soc_reg_above64_set_f   reg_above64_set;

    soc_mem_array_read_f    mem_read;
    soc_mem_array_write_f   mem_write;

    soc_polling_f           polling;
    soc_direct_reg_set_f    direct_reg_set;
    soc_fast_reg_set_f      fast_reg_set;
    soc_fast_reg_get_f      fast_reg_get;

    soc_reg_set_f           soc_reg_set;
    soc_reg_get_f           soc_reg_get;

    soc_reg_set_nocache_f   soc_reg_set_nocache;

    soc_mem_sbusdma_write_f soc_sbusdma_write;
    soc_mem_sbusdma_read_f  soc_sbusdma_read;

    soc_mem_descdma_write_f soc_descdma_write;
} soc_hw_log_access_t;
#endif /* CRASH_RECOVERY_SUPPORT */


int soc_reg_access_func_register(int unit, soc_reg_access_t* reg_access) ;

typedef struct soc_reg_brdc_block_info_s
{
    soc_block_t blk_type;
    int blk_ids[SOC_MAX_NUM_BLKS]; /*ids for schan commands of the blocks that controlled by the brdc block
                                     last element must be set to -1*/
} soc_reg_brdc_block_info_t;

#if defined(SER_TR_TEST_SUPPORT)
#define SOC_MAX_REG_NAME_LEN 200

typedef struct ser_reg_test_data_s {
    soc_reg_t      reg; /* reg for error inject */
    soc_reg_t      parity_enable_reg;
    soc_field_t    parity_enable_field;
    soc_port_t     port;
    int            index;
    soc_reg_info_t *reg_info;
    soc_field_t    test_field;
    uint64         *reg_buf;
    uint32         *field_buf;
    char           reg_name[SOC_MAX_REG_NAME_LEN];
    char           field_name[SOC_MAX_REG_NAME_LEN];
    uint32         badData;
} ser_reg_test_data_t;

extern void soc_ser_create_reg_test_data(int unit, uint32 *reg_data, uint32 *field_data,
                                         soc_reg_t parity_enable_reg,
                                         soc_field_t parity_enable_field,
                                         soc_reg_t reg, soc_field_t test_field,
                                         soc_port_t port, int index,
                                         ser_reg_test_data_t *reg_test_data);

extern soc_error_t ser_test_reg(int unit, ser_reg_test_data_t *test_reg,
                                  int *error_count);

#endif

#endif    /* !_SOC_REGISTER_H */
