/*! \file bcmmgmt_comp.h
 *
 * Component name utility functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMMGMT_COMP_H
#define BCMMGMT_COMP_H

/*!
 * \brief Get component name from component ID.
 *
 * \param [in] name Component name.
 *
 * \return Component ID or BCMMGMT_MAX_COMP_ID on error.
 */
extern uint32_t
bcmmgmt_comp_id_from_name(const char *name);

/*!
 * \brief Get component ID from component name.
 *
 * \param [in] comp_id Component ID.
 *
 * \return Pointer to component name or NULL on error.
 */
extern const char *
bcmmgmt_comp_name_from_id(uint32_t comp_id);

#endif /* BCMMGMT_COMP_H */
