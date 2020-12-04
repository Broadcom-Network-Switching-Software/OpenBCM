/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef _DNX_SW_STATE_ATTRIBUTRE_PACKED__
#define _DNX_SW_STATE_ATTRIBUTRE_PACKED__

#ifdef _MSC_VER

#ifndef __ATTRIBUTE_PACKED__

#define __ATTRIBUTE_PACKED__

#endif

#pragma warning(disable  : 4103)
#pragma warning(disable  : 4127)
#pragma pack(push)
#pragma pack(1)

#elif defined(__GNUC__)

#ifndef __ATTRIBUTE_PACKED__

#define __ATTRIBUTE_PACKED__ __attribute__ ((packed))

#endif

#elif defined(GHS)

#ifndef __ATTRIBUTE_PACKED__

#define __ATTRIBUTE_PACKED__ __attribute__ ((__packed__))

#endif

#else

#define __ATTRIBUTE_PACKED__

#endif

#endif
