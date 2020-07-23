/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *         
 *     
 * Phymod external definitions
 */

#ifndef _PHYMOD_DEFINITIONS_H_
#define _PHYMOD_DEFINITIONS_H_

#include <phymod/phymod_types.h>

#define _PHYMOD_MSG(string) "%s[%d]%s: " string "\n", __FILE__, __LINE__, FUNCTION_NAME()


#ifdef PHYMOD_DIAG 
typedef struct enum_mapping_s {
    char        *key;           /* String to match */
    uint32_t    enum_value;     /* Value associated with string */
} enum_mapping_t;
#endif


#define PHYMOD_E_OK PHYMOD_E_NONE

#define PHYMOD_LOCK_TAKE(t) do{\
        if((t->access.bus->mutex_give != NULL) && (t->access.bus->mutex_take != NULL)){\
            PHYMOD_IF_ERR_RETURN(t->access.bus->mutex_take(t->access.user_acc));\
        }\
    }while(0)
 
#define PHYMOD_LOCK_GIVE(t) do{\
        if((t->access.bus->mutex_give != NULL) && (t->access.bus->mutex_take != NULL)){\
            PHYMOD_IF_ERR_RETURN(t->access.bus->mutex_give(t->access.user_acc));\
        }\
    }while(0)

/* Dispatch */
#define PHYMOD_DRIVER_TYPE_GET(p,t) ((*t) = p->type)

/* Functions structure */
#define PHYMOD_IF_ERR_RETURN(A) \
    do {   \
        int loc_err ; \
        if ((loc_err = (A)) != PHYMOD_E_NONE) \
        {   if (A > PHYMOD_E_NONE) {\
                return PHYMOD_E_FAIL;\
            } else {\
                return loc_err;\
            }\
        }\
    } while (0)

#define PHYMOD_RETURN_WITH_ERR(A, B) \
    do {   \
        PHYMOD_DEBUG_ERROR(B);     \
        return A; \
    } while (0)

#define PHYMOD_NULL_CHECK(A) \
    do {   \
        if ((A) == NULL) \
        { PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("null parameter"))); } \
    } while (0)

#define PHYMOD_ERR_VALUE_CHECK(A, VAL) \
    do {   \
        if ((A) == VAL) \
        { PHYMOD_RETURN_WITH_ERR(PHYMOD_E_PARAM, (_PHYMOD_MSG("invalid value"))); } \
    } while (0)

#define PHYMOD_PMD_IF_ERR_RETURN(A)    PHYMOD_IF_ERR_RETURN(A)

/*
 * Map global configuration flags to the corresponding private ones.
*/
#if PHYMOD_CONFIG_INCLUDE_FLOATING_POINT
#define SERDES_API_FLOATING_POINT
#endif


#endif /*_PHYMOD_DEFINITIONS_H_*/
