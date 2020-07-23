/*! \file config.h
 *
 * Configuration management.
 * This file contains the interfaces to configuration management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SOC_LTSW_CONFIG_H
#define SOC_LTSW_CONFIG_H

/*!
 * \brief Initialize config module.
 *
 * \param None.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
soc_ltsw_config_init(void);

/*!
 * \brief Retrieve a global configuration string.
 *
 * \param [in] unit     Device unit number.
 * \param [in] name     Base name of property to look up.
 *
 * \retval String of configuration property string.
 */
extern char *
soc_ltsw_config_str_get(int unit, const char *name);

/*!
 * \brief Retrieve a per-port configuration string.
 *
 * \param [in] unit     Device unit number.
 * \param [in] port     Zero-based port number.
 * \param [in] name     Base name of property to look up.
 *
 * \retval String of configuration property string.
 */
extern char *
soc_ltsw_config_port_str_get(int unit, int port, const char *name);

/*!
 * \brief Set a global configuration string.
 *
 * \param [in] unit     Device unit number.
 * \param [in] name     Name of property to set.
 * \param [in] value    Value of property to set.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
soc_ltsw_config_str_set(int unit, const char *name, const char *value);

/*!
 * \brief Set a per-port configuration string.
 *
 * \param [in] unit     Device unit number.
 * \param [in] port     Zero-based port number.
 * \param [in] name     Name of property to set.
 * \param [in] value    Value of property to set.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
soc_ltsw_config_port_str_set(int unit, int port,
                             const char *name, const char *value);

/*!
 * \brief Display configuration software information.
 *
 * \param [in] unit     Device Unit number.
 *
 * \retval None.
 */
extern void
soc_ltsw_config_sw_dump(int unit);

/*!
 * \brief Get configuration HA data size.
 *
 * \param [in] unit     Device unit number.
 * \param [out] size    HA data size.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
soc_ltsw_config_ha_size_get(int unit, int *size);

/*!
 * \brief Save configuration HA data.
 *
 * \param [in] unit     Device unit number.
 * \param [out] Buffer  HA data buffer.
 * \param [in] size HA  data size.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
soc_ltsw_config_ha_save(int unit, void *buffer, int size);

/*!
 * \brief Restore configuration HA data.
 *
 * \param [in] unit     Device unit number.
 * \param [in] Buffer   Buffer HA data buffer.
 * \param [in] size     HA data size.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
soc_ltsw_config_ha_restore(int unit, void *buffer, int size);

/*!
 * \brief Display emulator configuration in yaml format.
 *
 * \param [in] unit     Device Unit number.
 *
 * \retval None.
 */
extern void
soc_ltsw_show_config(int unit);

#endif  /* SOC_LTSW_CONFIG_H */
