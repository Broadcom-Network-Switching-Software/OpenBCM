/*! \file xflow_macsec_int.h
 *
 * Xflow_macsec internal management header file.
 * This file contains the management for XFLOW_MACSEC module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_XFLOW_MACSEC_INT_H
#define BCMINT_LTSW_XFLOW_MACSEC_INT_H

#include <sal/sal_mutex.h>


typedef struct ltsw_xflow_macsec_sc_tcam_param_s {
    /* Secure channel index. */
    int sc_index;

    /* Is IPsec entry or Macsec. */
    bool is_ipsec;

    /* SCI TCAM parameter. */
    uint64 sci;

    /* SCI TCAM mask parameter. */
    uint64 sci_mask;

    /* Policy ID TCAM parameter. */
    xflow_macsec_policy_id_t policy_id;
} ltsw_xflow_macsec_sc_tcam_param_t;

typedef struct ltsw_xflow_macsec_cfg_s {

    /* xflow_macsec module initialized. */
    int inited;

    /* Mutex lock. */
    sal_mutex_t mutex;

    /*! xflow_macsec sc encrypt bitmap. */
    uint32_t *sc_encrypt_bmp;

    /*! xflow_macsec sc encrypt bitmap. */
    uint32_t *sc_decrypt_bmp;

    /*! xflow_macsec policy bitmap. */
    uint32_t *policy_bmp;

    /*! Number of blocks. */
    int num_blocks;

    /*! Number of secure channel per block for encryption. */
    int num_sc_encrypt;

    /*! Number of secure channel per block for decryption. */
    int num_sc_decrypt;

    /*! Number of policies per block. */
    int num_policies;

    /*! SC TCAM parameters. */
    ltsw_xflow_macsec_sc_tcam_param_t *sc_tcam_param;

    /* Reserved policy index for no flow match. */
    int reserved_policy_index;
} ltsw_xflow_macsec_cfg_t;

extern ltsw_xflow_macsec_cfg_t ltsw_xflow_macsec_info[BCM_MAX_NUM_UNITS];

/* Xflow_macsec info. */
#define XFLOW_MACSEC_INFO(unit) \
    (&ltsw_xflow_macsec_info[unit])

/* Xflow_macsec SC encrypt bitmap info. */
#define XFLOW_MACSEC_SC_ENCRYPT_BMP_INFO(unit) \
    (XFLOW_MACSEC_INFO(unit)->sc_encrypt_bmp)

/* Xflow_macsec policy bitmap info. */
#define XFLOW_MACSEC_POLICY_BMP_INFO(unit) \
    (XFLOW_MACSEC_INFO(unit)->policy_bmp)

/* Check if xflow_macsec module is initialized */
#define XFLOW_MACSEC_INIT_CHECK(u)                                \
    do {                                                \
        ltsw_xflow_macsec_cfg_t *xflow_macsec_cfg = XFLOW_MACSEC_INFO(u);               \
        if (xflow_macsec_cfg->inited == false) {                     \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);        \
        }                                               \
    } while(0)

/* xflow_macsec Lock */
#define XFLOW_MACSEC_LOCK(u) \
    sal_mutex_take(XFLOW_MACSEC_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define XFLOW_MACSEC_UNLOCK(u) \
    sal_mutex_give(XFLOW_MACSEC_INFO(u)->mutex);

/*!
 *  * \brief HA subcomponent id for XFLOW_MACSEC module.
 */

/*! XFLOW_MACSEC resource sc encrypt bitmap structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_ENCRYPT_BITMAP   0

/*! XFLOW_MACSEC resource sc encrypt bitmap structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_DECRYPT_BITMAP   1

/*! XFLOW_MACSEC resource policy bitmap structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_POLICY_BITMAP       2

/*! XFLOW_MACSEC SC TCAM map structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_TCAM_MAP         3

#endif /* BCMINT_LTSW_XFLOW_MACSEC_INT_H */
