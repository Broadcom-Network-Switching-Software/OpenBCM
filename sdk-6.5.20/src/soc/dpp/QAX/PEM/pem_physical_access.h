/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _PEM_PHYSICAL_ACCESS_H_
#define _PEM_PHYSICAL_ACCESS_H_

#include <stdlib.h>

#define PEM_BLOCK_ID  38
#ifdef DUNE_BCM
    #include <soc/schanmsg.h>
    #include <sal/core/libc.h>
    #include <soc/sand/sand_signals.h>
int phy_pem_mem_read(int unit, phy_mem_t* mem, void *entry_data);
int phy_pem_mem_write(int unit, phy_mem_t *mem, void *entry_data);
#else 

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef struct {
  unsigned int   reserve;        
  unsigned int   block_identifier;      
  unsigned int   mem_address;  
  unsigned int   row_index;      
  unsigned int   mem_width_in_bits;
} phy_mem_t;


int phy_pem_mem_read(int          unit,
                 phy_mem_t*    mem,
                 unsigned int *entry_data);

int phy_pem_mem_write(int          unit,
                  phy_mem_t*    mem,
                  unsigned int *entry_data);



#define DSIG_MAX_SIZE_UINT32 32

int dpp_dsig_read(int unit, int core,  char *block, char *from, char *to, char *name, uint32 *value, int size);

#endif 

#endif 

