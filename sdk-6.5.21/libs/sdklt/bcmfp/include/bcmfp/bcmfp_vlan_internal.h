/*! \file bcmfp_vlan_internal.h
 *
 * Init/Cleanup APIs for VLAN Field Processor.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_VLAN_INTERNAL_H
#define BCMFP_VLAN_INTERNAL_H

/*! Initialize the s/w state for Vlan field processor. */
extern int
bcmfp_vlan_init(int unit);

/*! Register call back function for Vlan field processor. */
extern int
bcmfp_vlan_imm_register(int unit);


/*! Cleanup the s/w state for Vlan field processor. */
extern int
bcmfp_vlan_cleanup(int unit);

/*!
 * \brief Handler for UFT bank change event for Vlan field processor.
 *
 * \param [in] unit Logical device id
 * \param [in] event Event description.
 * \param [in] ev_data Event data.
 *
 * \retval SHR_E_NONE Success
 */
extern void
bcmfp_vlan_grp_change_event_cb(int unit, const char *event, uint64_t ev_data);

#endif /* BCMFP_VLAN_INTERNAL_H */
