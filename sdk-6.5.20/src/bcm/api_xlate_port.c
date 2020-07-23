/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * BCM internal port translation functions
 *
 * This module serves as database for BCM API port mappings as well as
 * system port to physical port mappings (when supported by hardware).
 *
 * Although the two mappings are independent, it does not make much
 * sense to have different mappings for the API and system ports. It
 * is however possible to turn each mapping on and off independently,
 * i.e. BCM API port mapping is not required for system port mapping
 * to be active.
 */

#include <soc/drv.h>
#include <soc/debug.h>
#include <shared/bsl.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/control.h>

#include <bcm/error.h>

/*******************************************************************************
 *
 * Local resources
 */

#define _BCM_API_XLATE_PORT_MAP_MAX     BCM_PBMP_PORT_MAX

#define _BCM_XLATE_PORT_F_IDENTITY      0x1     /* Use identity mapping */
#define _BCM_XLATE_PORT_F_API_PORT_EN   0x2     /* Enable API remapping */
#define _BCM_XLATE_PORT_F_SYSPORT_EN    0x4     /* Enable modport remapping */

/* Port translation structure */
typedef struct _bcm_api_xlate_port_info_s {
    int8        a2p_map[_BCM_API_XLATE_PORT_MAP_MAX];
    int         (*a2p_pbmp)(bcm_pbmp_t apbmp, bcm_pbmp_t *ppbmp);
    int8        p2a_map[_BCM_API_XLATE_PORT_MAP_MAX];
    int         (*p2a_pbmp)(bcm_pbmp_t ppbmp, bcm_pbmp_t *apbmp);
    uint32      flags;
    bcm_pbmp_t  api_pbmp_valid;
} _bcm_api_xlate_port_info_t;

static _bcm_api_xlate_port_info_t _bcm_api_xlate_port_info[BCM_MAX_NUM_UNITS];

#define BCM_API_XLATE_PORT_INFO(_u) &_bcm_api_xlate_port_info[_u]

#define XLATE_P2A       0
#define XLATE_A2P       1

#define MAP_ARRAY(_u,_m) _bcm_api_xlate_port_map_array(_u,_m,COUNTOF(_m))

STATIC void
_check_identity_mapping(int unit)
{
    _bcm_api_xlate_port_info_t *pt = BCM_API_XLATE_PORT_INFO(unit);
    int identity_mapping = TRUE;
    int idx;

    for (idx = 0; idx < _BCM_API_XLATE_PORT_MAP_MAX; idx++) {
        if (pt->a2p_map[idx] != -1) {
            identity_mapping = FALSE;
            break;
        }
    }
    if (identity_mapping) {
        pt->flags |= _BCM_XLATE_PORT_F_IDENTITY;
    } else {
        pt->flags &= ~_BCM_XLATE_PORT_F_IDENTITY;
    }
}

STATIC int
_bcm_api_xlate_port(int unit, bcm_port_t *port, int xlate, uint32 en_flags)
{
    _bcm_api_xlate_port_info_t *pt;
    int8 *map;
    int is_gport = 0;
    int dev = 0;

    if (port == NULL) {
        return BCM_E_PARAM;
    }

    /* Use identity mapping if remote unit or negative port number */
    if (BCM_IS_REMOTE(unit) || *port < 0) {
        return BCM_E_NONE;
    }

    pt = BCM_API_XLATE_PORT_INFO(unit);

    /* Use identity mapping also if not initialized */
    if ((pt->flags & _BCM_XLATE_PORT_F_IDENTITY) != 0 ||
        (pt->flags & en_flags) == 0) {
        return BCM_E_NONE;
    }

    /* Accept local ports only */
    if (BCM_GPORT_IS_LOCAL(*port)) {
        *port = SOC_GPORT_LOCAL_GET(*port);
        is_gport = _SHR_GPORT_TYPE_LOCAL;
    } else if (BCM_GPORT_IS_DEVPORT(*port)) {
        *port = SOC_GPORT_DEVPORT_PORT_GET(*port);
        dev = SOC_GPORT_DEVPORT_DEVID_GET(*port);
        is_gport = _SHR_GPORT_TYPE_DEVPORT;
    } else if (BCM_GPORT_IS_SET(*port)) {
        return BCM_E_NONE;
    }
    
    /* Validate port */
    if (*port >= _BCM_API_XLATE_PORT_MAP_MAX) {
        return BCM_E_PORT;
    }

    map = (xlate == XLATE_A2P) ? pt->a2p_map : pt->p2a_map;
    if (map[*port] != -1) {
        *port = map[*port];
    }
    
    /* Restore original format */
    if (is_gport == _SHR_GPORT_TYPE_LOCAL) {
        BCM_GPORT_LOCAL_SET(*port, *port);
    } else if (is_gport == _SHR_GPORT_TYPE_DEVPORT) {
        BCM_GPORT_DEVPORT_SET(*port, dev, *port);
    }

    return BCM_E_NONE;     
}

STATIC int
_bcm_api_xlate_port_pbmp(int unit, bcm_pbmp_t *pbmp, int xlate, uint32 en_flags)
{
    _bcm_api_xlate_port_info_t *pt;
    int8 *map;
    bcm_pbmp_t tmp_pbmp;
    int rv, port, tmp_port;
    /* Use identity mapping if remote unit */
    if (BCM_IS_REMOTE(unit)) {
        return BCM_E_NONE;
    }
    pt = BCM_API_XLATE_PORT_INFO(unit);

    /* Use identity mapping also if not initialized */
    if ((pt->flags & _BCM_XLATE_PORT_F_IDENTITY) != 0 ||
        (pt->flags & en_flags) == 0) {
        return BCM_E_NONE;
    }

    /* Use custom translation if available */
    if (pt->a2p_pbmp && xlate == XLATE_A2P) {
        if ((rv = pt->a2p_pbmp(*pbmp, &tmp_pbmp)) >= 0) {
            BCM_PBMP_ASSIGN(*pbmp, tmp_pbmp);
        }
        return rv;
    }
    if (pt->p2a_pbmp && xlate == XLATE_P2A) {
        if ((rv = pt->p2a_pbmp(*pbmp, &tmp_pbmp)) >= 0) {
            BCM_PBMP_ASSIGN(*pbmp, tmp_pbmp);
        }
        return rv;
    }

    if (xlate == XLATE_A2P) {
        BCM_PBMP_AND(*pbmp, pt->api_pbmp_valid);
    }

    /* Default bitmap translation */
    map = (xlate == XLATE_A2P) ? pt->a2p_map : pt->p2a_map;
    BCM_PBMP_CLEAR(tmp_pbmp);
    BCM_PBMP_ITER(*pbmp, port) {
        tmp_port = (map[port] != -1) ? map[port] : port;
        _SHR_PBMP_PORT_ADD(tmp_pbmp, tmp_port);
    }
    BCM_PBMP_ASSIGN(*pbmp, tmp_pbmp);

    return BCM_E_NONE;
}

STATIC int
_bcm_api_xlate_port_map_array(int unit, soc_port_t *ports, int size)
{
    bcm_port_t aport;

    for (aport = 0; aport < size; aport++) {
        if (ports[aport] >= 0) {
            _bcm_api_xlate_port_map(unit, aport, ports[aport]);
        }
    }
    return 0;
}

/*******************************************************************************
 *
 * Custom maps
 */

static soc_port_t bcm56524_to_bcm56504_map[] = {
    30, 31, 32, 33, 34, 35,     /* GE */
    36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53,
    26, 27, 28, 29,             /* HG/XE */
     0                          /* CPU */
};

static soc_port_t bcm56524_single_modid_map[] = {
     0,                         /* CPU */
    30, 31, 32, 33, 34, 35,     /* GE */
    36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53,
    26, 27, 28, 29              /* HG/XE */
};

static soc_port_t bcm56620_to_bcm56504_map[] = {
     2,  3,  4,  5,  6,  7,     /* GE */
    18, 19, 14, 15, 16, 17,
    26, 32, 33, 34, 35, 36,
    46, 47, 27, 43, 44, 45,
    28, 29, 30, 31,             /* HG/XE */
     0                          /* CPU */
};

static soc_port_t bcm56630_to_bcm56504_map[] = {
    30, 31, 32, 33, 34, 35,     /* GE */
    36, 37, 38, 39, 40, 41,
    42, 43, 44, 45, 46, 47,
    48, 49, 50, 51, 52, 53,
    26, 27, 28, 29,             /* HG/XE */
     0                          /* CPU */
};

/* Map ports in pin-out order (similar to dport order) */
static soc_port_t bcm56636_single_modid_map[] = {
     0,                         /* CPU */
    30, 31, 32, 33, 34, 35,     /* GE */
    36, 37,  2,  3,  4,  5,
     6,  7,  8,  9, 38, 39,
    40, 41, 10, 11, 12, 13,
     1,
    42, 50, 26, 27, 28, 29      /* HG/XE */
};

/* Preserve identity mapping for lower ports (0-13) */
static soc_port_t bcm56636_single_modid_alt_map[] = {
     0,                         /* CPU */
     1,  2,  3,  4,  5,  6,     /* GE */
     7,  8,  9, 10, 11, 12,
    13, 38, 39, 40, 41, 30,
    31, 32, 33, 34, 35, 36,  
    37,
    42, 50, 26, 27, 28, 29      /* HG/XE */
};

/*******************************************************************************
 *
 * Exported functions
 */

/*
 * Function:
 *      _bcm_api_xlate_port_init
 * Purpose:
 *      Allocate and initialize port translation structure.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_api_xlate_port_init(int unit)
{
    _bcm_api_xlate_port_info_t *pt;
    int idx, pport, aport;
    char *mstr;

    /* Use identity mapping if remote unit */
    if (BCM_IS_REMOTE(unit)) {
        return BCM_E_NONE;
    }

    pt = BCM_API_XLATE_PORT_INFO(unit);

    pt->flags &= ~_BCM_XLATE_PORT_F_API_PORT_EN;
    pt->flags &= ~_BCM_XLATE_PORT_F_SYSPORT_EN;

    if (soc_property_get(unit, spn_BCM_XLATE_PORT_ENABLE, TRUE) != FALSE &&
        soc_property_get(unit, spn_BCM_XLATE_API_PORT_ENABLE, TRUE) != FALSE) {
        pt->flags |= _BCM_XLATE_PORT_F_API_PORT_EN;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "API port translation on unit %d\n"), unit));
    }
    if (soc_property_get(unit, spn_BCM_XLATE_PORT_ENABLE, TRUE) != FALSE &&
        soc_property_get(unit, spn_BCM_XLATE_SYSPORT_ENABLE, FALSE) != FALSE) {
        pt->flags |= _BCM_XLATE_PORT_F_SYSPORT_EN;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "System port translation on unit %d\n"), unit));
    }

    for (idx = 0; idx < _BCM_API_XLATE_PORT_MAP_MAX; idx++) {
        pt->a2p_map[idx] = -1;
        pt->p2a_map[idx] = -1;
    }

    if ((mstr = soc_property_get_str(unit, spn_BCM_XLATE_PORT_MAP)) != NULL) {
        if (sal_strcmp(mstr, "bcm56524_to_bcm56504") == 0) {
            MAP_ARRAY(unit, bcm56524_to_bcm56504_map);
        } else if (sal_strcmp(mstr, "bcm56524_single_modid") == 0) {
            MAP_ARRAY(unit, bcm56524_single_modid_map);
        } else if (sal_strcmp(mstr, "bcm56620_to_bcm56504") == 0) {
            MAP_ARRAY(unit, bcm56620_to_bcm56504_map);
        } else if (sal_strcmp(mstr, "bcm56630_to_bcm56504") == 0) {
            MAP_ARRAY(unit, bcm56630_to_bcm56504_map);
        } else if (sal_strcmp(mstr, "bcm56636_single_modid") == 0) {
            MAP_ARRAY(unit, bcm56636_single_modid_map);
        } else if (sal_strcmp(mstr, "bcm56636_single_modid_alt") == 0) {
            MAP_ARRAY(unit, bcm56636_single_modid_alt_map);
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Unknown bcm_xlate_port_map: %s\n"), mstr));
        }
    }

    _check_identity_mapping(unit);

    PBMP_ALL_ITER(unit, pport) {
        aport = soc_property_port_get(unit, pport, spn_BCM_XLATE_PORT, -1);
        if (aport >= 0) {
            _bcm_api_xlate_port_map(unit, aport, pport);
        }
    }

    PBMP_ALL_ITER(unit, pport) {
        aport = pport;
        _bcm_api_xlate_port(unit, &aport, XLATE_P2A,
                            _BCM_XLATE_PORT_F_API_PORT_EN);
        BCM_PBMP_PORT_ADD(pt->api_pbmp_valid, aport);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_api_xlate_port_cleanup
 * Purpose:
 *      Cleanup resouces used for port translation.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int 
_bcm_api_xlate_port_cleanup(int unit)
{
    _bcm_api_xlate_port_info_t *pt;

    /* Use identity mapping if remote unit */
    if (BCM_IS_REMOTE(unit)) {
        return BCM_E_NONE;
    }

    pt = BCM_API_XLATE_PORT_INFO(unit);

    pt->flags &= ~_BCM_XLATE_PORT_F_API_PORT_EN;
    pt->flags &= ~_BCM_XLATE_PORT_F_SYSPORT_EN;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_api_xlate_port_map
 * Purpose:
 *      Create port mapping for a single port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      aport - (IN) API port number
 *      pport - (IN) Physical port number
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_api_xlate_port_map(int unit, bcm_port_t aport, bcm_port_t pport)
{
    _bcm_api_xlate_port_info_t *pt;

    pt = BCM_API_XLATE_PORT_INFO(unit);

    /* Boundary checks */
    if (aport < 0 || aport >= _BCM_API_XLATE_PORT_MAP_MAX) {
        return BCM_E_PORT;
    }
    if (pport < 0 || pport >= _BCM_API_XLATE_PORT_MAP_MAX) {
        return BCM_E_PORT;
    }

    /* Write to table */
    if (aport == pport) {
        pt->a2p_map[aport] = -1;
        pt->p2a_map[pport] = -1;
    } else {
        pt->a2p_map[aport] = pport;
        pt->p2a_map[pport] = aport;
    }

    _check_identity_mapping(unit);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_api_xlate_port_a2p
 * Purpose:
 *      Translate an API port number to a physical port number.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN/OUT) Port to translate
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_api_xlate_port_a2p(int unit, bcm_port_t *port)
{
    return _bcm_api_xlate_port(unit, port, XLATE_A2P,
                               _BCM_XLATE_PORT_F_API_PORT_EN);
}

/*
 * Function:
 *      _bcm_api_xlate_port_p2a
 * Purpose:
 *      Translate a physical port number to an API port number.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN/OUT) Port to translate
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_api_xlate_port_p2a(int unit, bcm_port_t *port)
{
    return _bcm_api_xlate_port(unit, port, XLATE_P2A,
                               _BCM_XLATE_PORT_F_API_PORT_EN);
}

/*
 * Function:
 *      _bcm_api_xlate_port_pbmp_a2p
 * Purpose:
 *      Translate an API port bitmap to a physical port bitmap.
 * Parameters:
 *      unit - (IN) Unit number.
 *      pbmp - (IN/OUT) Port bitmap
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_api_xlate_port_pbmp_a2p(int unit, bcm_pbmp_t *pbmp)
{
    return _bcm_api_xlate_port_pbmp(unit, pbmp, XLATE_A2P,
                               _BCM_XLATE_PORT_F_API_PORT_EN);
}

/*
 * Function:
 *      _bcm_api_xlate_port_pbmp_p2a
 * Purpose:
 *      Translate a physical port bitmap to an API port bitmap.
 * Parameters:
 *      unit - (IN) Unit number.
 *      pbmp - (IN/OUT) Port bitmap
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_api_xlate_port_pbmp_p2a(int unit, bcm_pbmp_t *pbmp)
{
    return _bcm_api_xlate_port_pbmp(unit, pbmp, XLATE_P2A,
                               _BCM_XLATE_PORT_F_API_PORT_EN);
}

/*
 * Function:
 *      _bcm_xlate_sysport_s2p
 * Purpose:
 *      Translate a system port number (modport) to a physical port number.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN/OUT) Port to translate
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_xlate_sysport_s2p(int unit, bcm_port_t *port)
{
    return _bcm_api_xlate_port(unit, port, XLATE_A2P,
                               _BCM_XLATE_PORT_F_SYSPORT_EN);
}

/*
 * Function:
 *      _bcm_xlate_sysport_p2s
 * Purpose:
 *      Translate a physical port number to a system port number (modport).
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN/OUT) Port to translate
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_xlate_sysport_p2s(int unit, bcm_port_t *port)
{
    return _bcm_api_xlate_port(unit, port, XLATE_P2A,
                               _BCM_XLATE_PORT_F_SYSPORT_EN);
}

