/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */




#ifndef __ARAD_NIF_RECOVER_H__
#define __ARAD_NIF_RECOVER_H__

#define ARAD_NIF_RECOVER_FLAGS_XLP_SET(flags)     (flags |= 0x10000000)
#define ARAD_NIF_RECOVER_FLAGS_XLP_GET(flags)     (flags & 0x10000000)
#define ARAD_NIF_RECOVER_FLAGS_XLP_CLEAR(flags)   (flags &= ~0x10000000)

#define ARAD_NIF_RECOVER_FLAGS_CLP_SET(flags)     (flags |= 0x20000000)
#define ARAD_NIF_RECOVER_FLAGS_CLP_GET(flags)     (flags & 0x20000000)
#define ARAD_NIF_RECOVER_FLAGS_CLP_CLEAR(flags)   (flags &= ~0x20000000)

#define ARAD_NIF_RECOVER_F_DONT_RUN_RX_TRAFFIC 0X1

int arad_nif_recover_run_recovery_test(int unit, soc_pbmp_t* pbmp, uint32 flags, int iterations, int is_init_sequence);


#endif

