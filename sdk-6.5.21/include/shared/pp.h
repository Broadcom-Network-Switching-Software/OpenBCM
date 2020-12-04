/*
 * $Id: pp.h,v 1.1 2019/02/20 Michael Frank
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _SHR_PP_H
#define _SHR_PP_H

#define SHR_PP_SIGNAL_NAME_MAX_SIZE 128
#define SHR_PP_STAGE_NAME_MAX_SIZE 16
#define SHR_PP_BLOCK_NAME_MAX_SIZE 8

#define SHR_PP_SIGNAL_VALUE_MAX_SIZE_UINT32 96

#define SHR_PP_SIGNAL_PRINT_VALUE_MAX_SIZE_STR ((SHR_PP_SIGNAL_VALUE_MAX_SIZE_UINT32 * 4 * 2) + 2)

#endif /* _SHR_PP_H */
