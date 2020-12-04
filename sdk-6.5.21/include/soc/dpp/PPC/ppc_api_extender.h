/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __SOC_PPC_API_EXTENDER_INCLUDED__

#define __SOC_PPC_API_EXTENDER_INCLUDED__


















typedef enum
{
    SOC_PPC_EXTENDER_PORT_ING_ECID_NOP,
    SOC_PPC_EXTENDER_PORT_ING_ECID_KEEP
} SOC_PPC_EXTENDER_PORT_ING_ECID_MODE;

typedef struct
{
  
  SOC_PPC_EXTENDER_PORT_ING_ECID_MODE ing_ecid_mode;
} SOC_PPC_EXTENDER_PORT_INFO;














#endif

