/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        celldata.h
 * Purpose:     
 */

#ifndef   _DIAG_CELLDATA_H_
#define   _DIAG_CELLDATA_H_

#define DIAG_CELLDATA_CHUNKS        17
#define DIAG_CELLDATA_CHUNK_WORDS    8
#define DIAG_CELLDATA_SLICES        61
#define DIAG_CELLDATA_SLICE_WORDS    3
#define DIAG_CELLDATA_SLICE_COLUMNS  4

#define DIAG_CELLDATA_SLICE4S        16
#define DIAG_CELLDATA_SLICE4_WORDS    9

#define DIAG_CELLDATA_CBMP_MASK     0xf

#define DIAG_CELLDATA_DIFF_S60_C2_MASK 0x3fff0

typedef uint32 diag_cd_chunk_t[DIAG_CELLDATA_CHUNKS][SOC_MAX_MEM_WORDS];
typedef uint32 diag_cd_slice_t[DIAG_CELLDATA_SLICES][DIAG_CELLDATA_SLICE_WORDS];
typedef uint32 diag_cd_columns_t[DIAG_CELLDATA_SLICES][DIAG_CELLDATA_SLICE_COLUMNS];
typedef uint32 diag_cd_slice4_t[DIAG_CELLDATA_SLICE4S][SOC_MAX_MEM_WORDS];

extern int diag_celldata_chunk_read(int unit, soc_mem_t mem,
                                    int index, diag_cd_chunk_t chunk_data);
extern void diag_celldata_chunk_to_slice(diag_cd_chunk_t chunk_data,
                                         diag_cd_slice_t slice_data);
extern void diag_celldata_slice_to_columns(diag_cd_slice_t slice_data,
                                           diag_cd_columns_t columns_data);

extern void diag_celldata_columns_to_slice(diag_cd_columns_t columns_data,
                                           diag_cd_slice_t slice_data);
extern void diag_celldata_slice_to_chunk(diag_cd_slice_t slice_data,
                                         diag_cd_chunk_t chunk_data);
extern int diag_celldata_chunk_write(int unit, soc_mem_t mem,
                                     int index, diag_cd_chunk_t chunk_data);

extern int diag_celldata_columns_test(int unit, soc_mem_t mem, int index,
                                      int do_write, int do_read,
                                      diag_cd_columns_t columns_data,
                                      diag_cd_columns_t columns_result,
                                      diag_cd_columns_t columns_diff);

#endif /* _DIAG_CELLDATA_H_ */
