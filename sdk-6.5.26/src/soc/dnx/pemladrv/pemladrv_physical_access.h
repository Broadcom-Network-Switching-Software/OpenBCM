
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved. 
 */
/* *INDENT-OFF* */
#ifndef _PEMLADRV_PHYSICAL_ACCESS_H_
#define _PEMLADRV_PHYSICAL_ACCESS_H_


#ifdef BCM_DNX_SUPPORT
#include <soc/schanmsg.h>
#else 

#include "./types/physical_types.h"
#include "pemladrv_cfg_api_defines.h"

shr_error_e pemladrv_physical_read(int unit,
                            unsigned int block_identifier,
                            unsigned int is_reg,
                            unsigned int mem_address,
                            unsigned int mem_width_in_bits,
                            unsigned int reserve,
                            void* entry_data);

shr_error_e pemladrv_physical_write(int unit,
                            unsigned int block_identifier,
                            unsigned int is_reg,
                            unsigned int mem_address,
                            unsigned int mem_width_in_bits,
                            unsigned int reserve,
                            void* entry_data);

#endif 


#endif 



