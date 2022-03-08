
/* *INDENT-OFF* */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_DATA_MAX_FIFODMA_H_

#define _DNX_DATA_MAX_FIFODMA_H_




#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif



#define DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS (1)
#ifdef BCM_DNX2_SUPPORT

#undef DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS

#define DNX_DATA_MAX_FIFODMA_GENERAL_NOF_FIFODMA_CHANNELS (12)

#endif 


#define DNX_DATA_MAX_FIFODMA_GENERAL_MIN_NOF_HOST_ENTRIES (1)
#ifdef BCM_DNX2_SUPPORT

#undef DNX_DATA_MAX_FIFODMA_GENERAL_MIN_NOF_HOST_ENTRIES

#define DNX_DATA_MAX_FIFODMA_GENERAL_MIN_NOF_HOST_ENTRIES (64)

#endif 


#define DNX_DATA_MAX_FIFODMA_GENERAL_MAX_NOF_HOST_ENTRIES (1)
#ifdef BCM_DNX2_SUPPORT

#undef DNX_DATA_MAX_FIFODMA_GENERAL_MAX_NOF_HOST_ENTRIES

#define DNX_DATA_MAX_FIFODMA_GENERAL_MAX_NOF_HOST_ENTRIES (16384)

#endif 




#endif 

