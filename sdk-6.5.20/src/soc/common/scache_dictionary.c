/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: Level 2 Warm Boot cache dictionary management    
 */

#include <soc/error.h>

#ifdef BCM_WARM_BOOT_SUPPORT

#include <sal/core/libc.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/scache_dictionary.h>

#define SOC_WB_VERSION_1_0                  SOC_SCACHE_VERSION(1,0)
#define SOC_WB_DEFAULT_VERSION              SOC_WB_VERSION_1_0

#define SOC_SCACHE_DICTIONARY_ENTRY_MAX_1_0 64
#define SOC_SCACHE_DICTIONARY_ENTRY_MAX     SOC_SCACHE_DICTIONARY_ENTRY_MAX_1_0

static uint16 soc_scache_dictionary_version[SOC_MAX_NUM_DEVICES];
static soc_scache_dictionary_entry_t *soc_scache_dictionary[SOC_MAX_NUM_DEVICES];

#define SOC_SCACHE_DICTIONARY_ENTRY_EMPTY(_u, d_ptr, idx)                 \
    ((d_ptr[idx].owner == 0) && (d_ptr[idx].key == 0) &&                 \
     (sal_strlen(d_ptr[idx].name) == 0))

#define SOC_SCACHE_DICTIONARY_ENTRY_MATCH(_u, d_ptr, idx, _o, _k, _n)     \
    ((d_ptr[idx].owner == _o) && (d_ptr[idx].key == _k) &&               \
     (sal_strncmp(d_ptr[idx].name, _n, SOC_SCACHE_DICTIONARY_ENTRY_NAME_MAX) == 0))

#define SOC_SCACHE_DICTIONARY_ENTRY_GET(_u, d_ptr, idx, _v)               \
    do {                                                                  \
        if (_v) *_v = d_ptr[idx].value;                                   \
    } while (0)

#define SOC_SCACHE_DICTIONARY_ENTRY_SET(_u, d_ptr, idx, _o, _k, _n, _v)   \
    do {                                                                  \
        int _l = sal_strlen(_n) + 1;                                      \
        if (_l <= SOC_SCACHE_DICTIONARY_ENTRY_NAME_MAX) {                 \
            d_ptr[idx].owner = _o;                                        \
            d_ptr[idx].key = _k;                                          \
            sal_strncpy(d_ptr[idx].name, _n, _l);                         \
            d_ptr[idx].value = _v;                                        \
        }                                                                 \
    } while (0)

#define SOC_SCACHE_DICTIONARY_ENTRY_SET_VALUE(_u, d_ptr, idx, _v)         \
    do {                                                                  \
        d_ptr[idx].value = _v;                                            \
    } while (0)


STATIC void
soc_scache_dictionary_init(int unit, uint8 *scache_ptr, uint16 version)
{
    soc_scache_dictionary[unit] = (soc_scache_dictionary_entry_t *)scache_ptr;
    soc_scache_dictionary_version[unit] = version;
}

STATIC int
soc_scache_dictionary_size(int unit, uint16 version)
{
    int dictionary_size = 0;
    if (version >= SOC_WB_VERSION_1_0) {
        dictionary_size = SOC_SCACHE_DICTIONARY_ENTRY_MAX_1_0;
    } else {
        dictionary_size = SOC_SCACHE_DICTIONARY_ENTRY_MAX;
    }
    return dictionary_size;
}

STATIC int
soc_scache_dictionary_alloc_size(int unit, uint16 version)
{
    int dictionary_size = 0;
    dictionary_size = soc_scache_dictionary_size(unit, version);
    return dictionary_size * sizeof(soc_scache_dictionary_entry_t);
}

int
soc_scache_dictionary_alloc(int unit)
{
    int rv = SOC_E_NONE;
    int stable_size = 0;
    uint8 *scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    uint16 default_ver = SOC_WB_DEFAULT_VERSION;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if (!stable_size) {
        /* stable not allocated */
        return SOC_E_NONE;
    }

    alloc_size = soc_scache_dictionary_alloc_size(unit, default_ver);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_DICTIONARY_HANDLE, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      TRUE, &alloc_size, &scache_ptr,
                                      default_ver, NULL);
    /* Proceed with Level 1 Warm Boot */
    if (SOC_FAILURE(rv) && (rv != SOC_E_NOT_FOUND)) {
        return rv;
    }

    if (SOC_SUCCESS(rv) && scache_ptr) {
        sal_memset(scache_ptr, 0, alloc_size);
        soc_scache_dictionary_init(unit, scache_ptr, default_ver);
    }

    return SOC_E_NONE;
}

int
soc_scache_dictionary_recover(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    uint16 default_ver = SOC_WB_DEFAULT_VERSION;
    uint16 recovered_ver = 0;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_DICTIONARY_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &alloc_size, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (SOC_SUCCESS(rv)) {
        soc_scache_dictionary_init(unit, scache_ptr, recovered_ver);
    }

    return SOC_E_NONE;
}

int
soc_scache_dictionary_sync(int unit)
{
    int rv = SOC_E_NONE;
    int stable_size = 0;
    uint8 *scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    uint16 default_ver = SOC_WB_DEFAULT_VERSION;
    soc_scache_dictionary_entry_t *dictionary_ptr;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));
    if (!stable_size) {
        /* stable not allocated */
        return SOC_E_NONE;
    }

    alloc_size = soc_scache_dictionary_alloc_size(unit, default_ver);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_DICTIONARY_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      TRUE, &alloc_size, &scache_ptr,
                                      default_ver, NULL);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    if (SOC_SUCCESS(rv) && scache_ptr) {
        soc_scache_dictionary_init(unit, scache_ptr, default_ver);
        dictionary_ptr = (soc_scache_dictionary_entry_t *)scache_ptr;

        SOC_SCACHE_DICTIONARY_ENTRY_SET(unit, dictionary_ptr, 0, 
                        0, 0, ssden_SOC_MAX_NUM_PORTS, SOC_MAX_NUM_PORTS);
        SOC_SCACHE_DICTIONARY_ENTRY_SET(unit, dictionary_ptr, 1, 
                        0, 0, ssden_SOC_MAX_NUM_PP_PORTS, SOC_MAX_NUM_PP_PORTS);
        SOC_SCACHE_DICTIONARY_ENTRY_SET(unit, dictionary_ptr, 2, 
                        0, 0, ssden_SOC_MAX_NUM_BLKS, SOC_MAX_NUM_BLKS);
        SOC_SCACHE_DICTIONARY_ENTRY_SET(unit, dictionary_ptr, 3, 
                        0, 0, ssden_SOC_MAX_NUM_PIPES, SOC_MAX_NUM_PIPES);
        SOC_SCACHE_DICTIONARY_ENTRY_SET(unit, dictionary_ptr, 4, 
                        0, 0, ssden_SOC_MAX_MEM_BYTES, SOC_MAX_MEM_BYTES);
        SOC_SCACHE_DICTIONARY_ENTRY_SET(unit, dictionary_ptr, 5, 
                        0, 0, ssden_SOC_PBMP_PORT_MAX, SOC_PBMP_PORT_MAX);
    }

    return SOC_E_NONE;
}

STATIC int
soc_scache_dictionary_entry_get_real(int unit, const char *name, int *value)
{
    int rv = SOC_E_NONE;
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    uint16 version = 0;
    uint16 default_ver = SOC_WB_DEFAULT_VERSION;
    uint16 recovered_ver = 0;
    int dictionary_size, i;
    soc_scache_dictionary_entry_t *dictionary_ptr;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_DICTIONARY_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &alloc_size, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    if (SOC_SUCCESS(rv) && scache_ptr) {
        version = SOC_WARM_BOOT(unit) ? recovered_ver : default_ver;
        dictionary_size = soc_scache_dictionary_size(unit, version);
        dictionary_ptr = (soc_scache_dictionary_entry_t *)scache_ptr;
        for (i = 0; i < dictionary_size; i++) {
            if (SOC_SCACHE_DICTIONARY_ENTRY_MATCH(unit, dictionary_ptr, i, 
                                                  0, 0, name)) {
                SOC_SCACHE_DICTIONARY_ENTRY_GET(unit, dictionary_ptr, i, value);
                return SOC_E_NONE;
            }
        }
    }

    return SOC_E_NOT_FOUND;
}

int
soc_scache_dictionary_entry_get(int unit, const char *name, int defl)
{
    int rv, value=0;
    if ((rv = soc_scache_dictionary_entry_get_real(unit, name, &value)) < 0) {
        return defl;
    }
    return value;
}

int
soc_scache_dictionary_entry_set(int unit, char *name, int value)
{
    int rv = SOC_E_NONE;
    uint8 *scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_size = 0;
    uint16 version = 0;
    uint16 default_ver = SOC_WB_DEFAULT_VERSION;
    uint16 recovered_ver = 0;
    int dictionary_size, i, first_empty = -1;
    soc_scache_dictionary_entry_t *dictionary_ptr;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit,
                          SOC_SCACHE_DICTIONARY_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                      FALSE, &alloc_size, &scache_ptr,
                                      default_ver, &recovered_ver);
    if (SOC_FAILURE(rv)) {
        return rv;
    }

    if (SOC_SUCCESS(rv) && scache_ptr) {
        version = SOC_WARM_BOOT(unit) ? recovered_ver : default_ver;
        dictionary_size = soc_scache_dictionary_size(unit, version);
        dictionary_ptr = (soc_scache_dictionary_entry_t *)scache_ptr;
        for (i = 0; i < dictionary_size; i++) {
            if (SOC_SCACHE_DICTIONARY_ENTRY_MATCH(unit, dictionary_ptr, i, 
                                                  0, 0, name)) {
                SOC_SCACHE_DICTIONARY_ENTRY_SET_VALUE(unit, dictionary_ptr, i, 
                                                      value);
                return SOC_E_NONE;
            }
            if ((first_empty < 0) && 
                SOC_SCACHE_DICTIONARY_ENTRY_EMPTY(unit, dictionary_ptr, i)) {
                first_empty = i;
            }
        }
        if (first_empty < 0) {
            return SOC_E_FULL;
        }

        SOC_SCACHE_DICTIONARY_ENTRY_SET(unit, dictionary_ptr, first_empty, 
                        0, 0, name, value);
    }

    return rv;
}
#else /* !BCM_WARM_BOOT_SUPPORT */
typedef int soc_common_scache_dictionary_not_empty; /* Make ISO compilers happy. */
#endif /* !BCM_WARM_BOOT_SUPPORT */

