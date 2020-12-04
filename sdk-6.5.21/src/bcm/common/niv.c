/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * NIV initializers
 */
#if defined(INCLUDE_L3)

#include <sal/core/libc.h>
 
#include <bcm/niv.h>

/* 
 * Function:
 *      bcm_niv_port_t_init
 * Purpose:
 *      Initialize the NIV port struct
 * Parameters: 
 *      niv_port - Pointer to the struct to be init'ed
 */
void
bcm_niv_port_t_init(bcm_niv_port_t *niv_port)
{   
    if (niv_port != NULL) {
        sal_memset(niv_port, 0, sizeof(*niv_port));
    }
    return;
}

/* 
 * Function:
 *      bcm_niv_forward_t_init
 * Purpose:
 *      Initialize the NIV forward entry struct
 * Parameters: 
 *      iv_fwd_entry - Pointer to the struct to be init'ed
 */
void
bcm_niv_forward_t_init(bcm_niv_forward_t *iv_fwd_entry)
{   
    if (iv_fwd_entry != NULL) {
        sal_memset(iv_fwd_entry, 0, sizeof(*iv_fwd_entry));
    }
    return;
}

/* 
 * Function:
 *      bcm_niv_egress_t_init
 * Purpose:
 *      Initialize the NIV egress object struct
 * Parameters: 
 *      niv_egress - Pointer to the struct to be init'ed
 */
void
bcm_niv_egress_t_init(bcm_niv_egress_t *niv_egress)
{   
    if (niv_egress != NULL) {
        sal_memset(niv_egress, 0, sizeof(*niv_egress));
    }
    return;
}

#else
typedef int _bcm_niv_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */

