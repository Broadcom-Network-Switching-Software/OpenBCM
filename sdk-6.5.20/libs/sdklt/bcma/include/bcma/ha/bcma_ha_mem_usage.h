/*! \file bcma_ha_mem_usage.h
 *
 * Debug API for HA memory usage.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_HA_MEM_USAGE_H
#define BCMA_HA_MEM_USAGE_H

#include <sal/sal_types.h>

#include <shr/shr_pb.h>

/*!
 * \brief Get the HA memory usage for the specified component name.
 *
 * The HA component memory usage includes per-unit memory usage and
 * generic memory usage. The argument \c gen_mem_inc can be used to decide
 * whether to include generic memory usage to the HA memory usage result.
 *
 * This function will also format the component memory usage
 * (including per sub-id of the specified component) to print buffer
 * if \c pb is not NULL. If \c fmt_hdr is TRUE, a header information will
 * be first added to print buffer \c pb.
 *
 * \param [in] unit Unit number.
 * \param [in] comp_name Component name.
 * \param [in] gen_mem_inc Include HA generic memory usage if TURE.
 * \param [in] pb Print buffer.
 * \param [in] fmt_hdr Whether to format header to print buffer.
 *
 * \return The HA memory usage in bytes for component \c comp_name.
 */
extern uint32_t
bcma_ha_comp_mem_usage(int unit, const char *comp_name, bool gen_mem_inc,
                       shr_pb_t *pb, bool fmt_hdr);

/*!
 * \brief Get the HA memory usage for the current device.
 *
 * The HA memory usage includes per-unit memory usage and generic memory usage.
 * The argument \c gen_mem_inc can be used to decide whether to include
 * generic memory usage to the HA memory usage result.
 *
 * This function will also format the HA memory usage (including per-component
 * usage) to print buffer if \c pb is not NULL. If \c fmt_hdr is TRUE,
 * a header information will be first added to print buffer \c pb.
 *
 * \param [in] unit Unit number.
 * \param [in] gen_mem_inc Include HA generic memory usage if TURE.
 * \param [in] pb Print buffer.
 * \param [in] fmt_hdr Whether to format header to print buffer.
 * \param [in] long_fmt Display detailed HA information if TRUE.
 *
 * \return The HA memory usage in bytes for the current device.
 */
extern uint32_t
bcma_ha_mem_usage(int unit, bool gen_mem_inc, shr_pb_t *pb, bool fmt_hdr,
                  bool long_fmt);

#endif /* BCMA_HA_MEM_USAGE_H */
