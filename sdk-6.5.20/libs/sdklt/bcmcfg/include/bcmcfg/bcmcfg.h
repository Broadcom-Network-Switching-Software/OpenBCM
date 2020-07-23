/*! \file bcmcfg.h
 *
 * BCMCFG public API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_H
#define BCMCFG_H

/*!
 * \brief Parse a BCMCFG format YAML file.
 *
 * The results of parsing initialize configuration state. If called
 * multiple times, subsequent calls update configuration state. Calls
 * may be freely intermixed with bcmcfg_string_parse().
 *
 * \param [in]  file            File to parse.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmcfg_file_parse(const char *file);

/*!
 * \brief Parse a BCMCFG format YAML string.
 *
 * The results of parsing initialize configuration state. If called
 * multiple times, subsequent calls update configuration state. Calls
 * may be freely intermixed with bcmcfg_file_parse().
 *
 * \param [in]  str             String to parse.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int
bcmcfg_string_parse(const char *str);

#endif /* BCMCFG_H */
