/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BFCMAP_UTIL_H
#define BFCMAP_UTIL_H

#include <bbase_util.h>

#define BFCMAP_PRINTF              BBASE_PRINTF

#define BFCMAP_DBG_PRINTF          BBASE_DBG_PRINTF


#ifndef BFCMAP_SAL_ASSERT
#define BFCMAP_SAL_ASSERT(c)       BMF_SAL_ASSERT(c) 
#endif

#ifndef BFCMAP_SAL_PRINTF
#define BFCMAP_SAL_PRINTF          BMF_SAL_PRINTF
#endif

#ifndef BFCMAP_SAL_DBG_PRINTF
#define BFCMAP_SAL_DBG_PRINTF      BMF_SAL_DBG_PRINTF
#endif

#ifndef BFCMAP_SAL_MEMCPY
#define BFCMAP_SAL_MEMCPY          BMF_SAL_MEMCPY
#endif

#ifndef BFCMAP_SAL_MEMCMP
#define BFCMAP_SAL_MEMCMP          BMF_SAL_MEMCMP
#endif

#ifndef BFCMAP_SAL_MEMSET
#define BFCMAP_SAL_MEMSET          BMF_SAL_MEMSET
#endif


#ifndef BFCMAP_SAL_STRLEN
#define BFCMAP_SAL_STRLEN          BMF_SAL_STRLEN
#endif

#ifndef BFCMAP_SAL_STRCPY
#define BFCMAP_SAL_STRCPY          BMF_SAL_STRCPY
#endif

#ifndef BFCMAP_SAL_STRNCPY
#define BFCMAP_SAL_STRNCPY         BMF_SAL_STRNCPY
#endif

#ifndef BFCMAP_SAL_STRCMP
#define BFCMAP_SAL_STRCMP          BMF_SAL_STRCMP
#endif

#ifndef BFCMAP_SAL_STRCMPI
#define BFCMAP_SAL_STRCMPI         BMF_SAL_STRCMPI
#endif

#ifndef BFCMAP_SAL_STRNCMP
#define BFCMAP_SAL_STRNCMP         BMF_SAL_STRNCMP
#endif

#ifndef BFCMAP_SAL_STRNCASECMP
#define BFCMAP_SAL_STRNCASECMP     BMF_SAL_STRNCASECMP
#endif

#ifndef BFCMAP_SAL_STRCHR
#define BFCMAP_SAL_STRCHR          BMF_SAL_STRCHR
#endif

#ifndef BFCMAP_SAL_STRSTR
#define BFCMAP_SAL_STRSTR          BMF_SAL_STRSTR
#endif

#ifndef BFCMAP_SAL_STRCAT
#define BFCMAP_SAL_STRCAT          BMF_SAL_STRCAT
#endif

#ifndef BFCMAP_SAL_STRTOK
#define BFCMAP_SAL_STRTOK          BMF_SAL_STRTOK
#endif

#ifndef BFCMAP_SAL_ATOI
#define BFCMAP_SAL_ATOI            BMF_SAL_ATOI
#endif

#ifndef BFCMAP_SAL_STRDUP 
#define BFCMAP_SAL_STRDUP          BMF_SAL_STRDUP
#endif

#ifndef BFCMAP_SAL_XSPRINTF
#define BFCMAP_SAL_XSPRINTF        BMF_SAL_XSPRINTF
#endif /* BFCMAP_SAL_XSPRINTF */

#endif /* BFCMAP_UTIL_H */

