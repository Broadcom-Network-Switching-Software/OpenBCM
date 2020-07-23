/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Routines for managing HW profile tables.
 */

#ifndef _SOC_PROFILE_MEM_H
#define _SOC_PROFILE_MEM_H

#include <soc/defs.h>
#include <soc/mem.h>

#define SOC_PROFILE_MEM_F_NO_SHARE      0x1
#define SOC_PROFILE_MEM_DMA_THRESHHOLD  8
#define SOC_PROFILE_MEM_WRITE_DMA_MODE  1
#define SOC_PROFILE_MEM_WRITE_PIO_MODE  2

typedef struct soc_profile_mem_entry_s {
    uint32     ref_count;       /* Reference counters */
    int        entries_per_set; /* Number of entries per set */
} soc_profile_mem_entry_t;

typedef struct soc_profile_mem_table_s {
    soc_mem_t  mem;             /* Memory id */
    int        index_min;
    int        index_max;
    int        entry_words;     /* Entry word size */
    uint32     *data_mask;      /* optional entry comparison mask */
    soc_profile_mem_entry_t *entries; /* Entry info array */
    uint32     *cache_p;        /* Pointer to concatenated cached entries */   
} soc_profile_mem_table_t;

typedef struct soc_profile_mem_s {
    soc_profile_mem_table_t *tables;  /* Table info array */
    int        table_count;           /* Number of entries in tables array */
    uint32     flags;                 /* SOC_PROFILE_MEM_F_xxx */
} soc_profile_mem_t;

typedef struct soc_profile_reg_entry_s {
    uint32     ref_count;       /* Reference counters */
    int        entries_per_set; /* Number of entries per set */
    uint64     *cache_p;        /* Pointer to cached entries */
} soc_profile_reg_entry_t;

#define SOC_PROFILE_FLAG_XPE_SINGLE_ACC 0x1
typedef struct soc_profile_reg_s {
    soc_reg_t  *regs;                 /* Pointer to array of register id */
    int        regs_count;            /* Number of entries in regs */
    soc_profile_reg_entry_t *entries; /* Pointer to array of entries */
    int        profile_flag;          /* Profile flag */
} soc_profile_reg_t;

extern void soc_profile_mem_t_init(soc_profile_mem_t *profile);
extern int soc_profile_mem_index_create(int unit,
                                        soc_mem_t *mem_array,
                                        int *entry_words_array,
                                        int *index_min_array,
                                        int *index_max_array,
                                        void **data_mask_array,
                                        int table_count,
                                        soc_profile_mem_t *profile);
extern int soc_profile_mem_create(int unit,
                                  soc_mem_t *mem_array,
                                  int *entry_words_array,
                                  int table_count,
                                  soc_profile_mem_t *profile);
extern int soc_profile_mem_destroy(int unit,
                                   soc_profile_mem_t *profile);

extern int soc_profile_mem_add_unique(int unit,
                    soc_profile_mem_t *profile,
                    void **entries_array,
                    int entries_per_set0,
                    int pipe,
                    uint32 *index0);

extern int soc_profile_mem_add(int unit,
                               soc_profile_mem_t *profile,
                               void **entries,
                               int entries_per_set0,
                               uint32 *index0);

extern int soc_profile_mem_search(int unit,
                               soc_profile_mem_t *profile,
                               void **entries,
                               int entries_per_set0,
                               uint32 *index0);

extern int soc_profile_mem_single_table_add(int unit,
                                            soc_profile_mem_t *profile,
                                            void *entries,
                                            int entries_per_set,
                                            int *index);
extern int soc_profile_mem_delete_unique(int unit,
                                   soc_profile_mem_t *profile,
                                   uint32 index0, int pipe);
extern int soc_profile_mem_delete(int unit,
                                  soc_profile_mem_t *profile,
                                  uint32 index);
extern int soc_profile_mem_set(int unit,
                               soc_profile_mem_t *profile,
                               void **entries_array,
                               uint32 index0);
extern int soc_profile_mem_get(int unit,
                               soc_profile_mem_t *profile,
                               int index0,
                               int count,
                               void **entries_array);
extern int soc_profile_mem_index_get(int unit,
                                     soc_profile_mem_t *profile,
                                     soc_mem_t mem,
                                     int *index);
extern int soc_profile_mem_single_table_get(int unit,
                                            soc_profile_mem_t *profile,
                                            int index,
                                            int count,
                                            void *entries);
extern int soc_profile_mem_reference_unique(int unit,
                          soc_profile_mem_t *profile,
                          int index0,
                          int entries_per_set0,
                          int pipe);
extern int soc_profile_mem_reference(int unit,
                                     soc_profile_mem_t *profile,
                                     int index0,
                                     int entries_per_set0);
extern int soc_profile_mem_ref_count_get(int unit,
                                         soc_profile_mem_t *profile,
                                         int index, int *ref_count);
extern int soc_profile_mem_fields32_modify_unique(int unit,
                                soc_profile_mem_t *profile,
                                int table_id,
                                int field_count,
                                soc_field_t *fields,
                                uint32 *values);
extern int soc_profile_mem_fields32_modify(int unit,
                                           soc_profile_mem_t *profile,
                                           int table_id,
                                           int field_count,
                                           soc_field_t *fields,
                                           uint32 *values);
extern int soc_profile_mem_field32_modify(int unit,
                                          soc_profile_mem_t *profile,
                                          int table_id,
                                          soc_field_t field,
                                          uint32 value);

extern void soc_profile_reg_t_init(soc_profile_reg_t *profile_reg);
extern int soc_profile_reg_create(int unit,
                                  soc_reg_t *regs,
                                  int regs_count,
                                  soc_profile_reg_t *profile_reg);
extern int soc_profile_reg_destroy(int unit,
                                   soc_profile_reg_t *profile_reg);
extern int soc_profile_reg_add(int unit,
                               soc_profile_reg_t *profile_reg,
                               uint64 **entries,
                               int entries_per_set,
                               uint32 *index);
extern int soc_profile_reg_delete(int unit,
                                  soc_profile_reg_t *profile_reg,
                                  uint32 index);
extern int soc_profile_reg_get(int unit,
                               soc_profile_reg_t *profile_reg,
                               uint32 index,
                               int count,
                               uint64 **entries);
extern int soc_profile_reg_reference(int unit,
                                     soc_profile_reg_t *profile_reg,
                                     uint32 index,
                                     int entries_per_set_override);
extern int soc_profile_reg_ref_count_get(int unit,
                                         soc_profile_reg_t *profile_reg,
                                         uint32 index, int *ref_count);
extern int soc_profile_mem_sw_state_set(int unit, soc_profile_mem_t *profile,
                                        void **entries_array, int ent_per_set0,
                                        uint32 index0);

extern int soc_profile_mem_write_mode_get(int unit,
                               soc_profile_mem_t *profile,
                               uint32 index0, int *mode);

/* Macro to get a pointer to the entry at the specified index */
#define SOC_PROFILE_MEM_ENTRY(_unit, _profile_mem, _cast, _index) \
    ((_cast)&_profile_mem->tables[0].cache_p[(_index) * _profile_mem->tables[0].entry_words])

/* Macro to adjust the ref count at the specifed index.
 * Positive and negative "_count" values are accepted.
 */
#define SOC_PROFILE_MEM_REFERENCE(_unit, _profile_mem, _index, _count) \
    (_profile_mem->tables[0].entries[_index].ref_count += _count)

/* Macro to set the number of entries per set */
#define SOC_PROFILE_MEM_ENTRIES_PER_SET(_unit, _profile_mem, _index, _num) \
    (_profile_mem->tables[0].entries[_index].entries_per_set = _num)

#endif /* !_SOC_PROFILE_MEM_H */
