
/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
 *         
 *     
 * DO NOT EDIT THIS FILE!
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX family only!"
#endif /* BCM_DNX_SUPPORT */

#ifndef _IMB_MGU_H__H_
#define _IMB_MGU_H__H_

#include <soc/portmod/portmod.h>
#include <bcm/port.h>
#include <bcm/cosq.h>
#include <bcm_int/dnx/port/imb/imb_diag.h>
#include <bcm_int/dnx/cosq/ingress/cosq_ingress_port.h>
#include <soc/dnxc/dnxc_port.h>
/*initialize an IMB.*/
int imb_mgu_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info);

/*De-initialize an existing IMB.*/
int imb_mgu_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info);

/*Add new IMB (Interface Management Block) port*/
int imb_mgu_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags);

/*Remove an existing IMB (Interface Management Block) port*/
int imb_mgu_port_detach(
    int unit,
    bcm_port_t port);

#endif /*_IMB_MGU_H_*/
