/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        raptor.h
 * Purpose:     Function declarations for Raptor bcm functions
 */

#ifndef _BCM_INT_RAPTOR_H_
#define _BCM_INT_RAPTOR_H_
#ifndef _BCM_INT_RAVEN_H_
#define _BCM_INT_RAVEN_H_

#ifdef BCM_FIELD_SUPPORT 
#define _BCM_RP_FIELD_IPBM_INDEX(_unit_, _port_, _slice_) \
           (((_port_) << (SOC_IS_RAVEN((_unit_)) ? 4 : (SOC_IS_HAWKEYE((_unit_)) ? 2 : 3))) | (_slice_))
extern int _bcm_field_raptor_init(int, _field_control_t *);
extern int _bcm_field_raven_qualify_ttl(int unit, bcm_field_entry_t entry,
                                      uint8 data, uint8 mask);
extern int _bcm_field_raven_qualify_ttl_get(int unit, bcm_field_entry_t entry,
                                            uint8 *data, uint8 *mask);
extern int _bcm_field_raptor_qualify_ip_type_get(int unit,
                                                 bcm_field_entry_t entry,
                                                 bcm_field_IpType_t *type,
                                                 bcm_field_qualify_t qual);
#endif /* BCM_FIELD_SUPPORT */

#endif  /* !_BCM_INT_RAVEN_H_ */
#endif  /* !_BCM_INT_RAPTOR_H_ */
