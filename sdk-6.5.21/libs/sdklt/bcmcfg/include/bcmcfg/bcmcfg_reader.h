/*! \file bcmcfg_reader.h
 *
 * BCMCFG outer reader.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_READER_H
#define BCMCFG_READER_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmcfg/bcmcfg_types.h>

/*!
 * \brief Read component handler.
 *
 * Data structure for the software component configuration reader.
 */
extern const bcmcfg_read_handler_t bcmcfg_read_component;

/*!
 * \brief Read device handler.
 *
 * Data structure for the device configuration reader.
 */
extern const bcmcfg_read_handler_t bcmcfg_read_device;

/*!
 * \brief YAML read format.
 */
typedef enum bcmcfg_read_format_e {
    /*! Input string is a file path. */
    FILE_INPUT,

    /*! Input string is YAML formatted data. */
    STRING_INPUT
} bcmcfg_read_format_t;

/*!
 * \brief Read data staging.
 */
typedef struct bcmcfg_read_data_s {
    /*! Data format. */
    bcmcfg_read_format_t format;

    /*! Data pointer. */
    const char *data;

    /*! File Path. */
    const char *fpath;

    /*! Locus. */
    const char *locus;

    /*! Next pointer. */
    struct bcmcfg_read_data_s *next;
} bcmcfg_read_data_t;

/*!
 * \brief BCMCFG read initializer.
 *
 * Initialize the YAML reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_gen_init(void);

/*!
 * \brief BCMCFG read cleanup.
 *
 * Cleanup the YAML reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_gen_cleanup(void);

/*!
 * \brief BCMCFG read unit initializer.
 *
 * Initialize the YAML unit reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_unit_init(int unit,
                                 bcmcfg_init_stage_t stage);

/*!
 * \brief BCMCFG read unit initializer.
 *
 * Initialize the YAML unit reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_unit_cleanup(int unit);

/*!
 * \brief BCMCFG read data get.
 *
 * Get the read data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_data_get(const bcmcfg_read_data_t **read);

/*!
 * \brief BCMCFG read data cleanup.
 *
 * Cleanup the read data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_data_cleanup(void);

/*!
 * \brief BCMCFG outer read initializer.
 *
 * The outer reader is the first reader in the handler stack, so it
 * must be initialized before parsing. This function will be
 * responsible for initializing any other readers.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_outer_gen_init(void);

/*!
 * \brief BCMCFG outer read initializer for unit.
 *
 * Initialize out reader for ounit.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_outer_unit_init(int unit,
                                       bcmcfg_init_stage_t stage);

/*!
 * \brief BCMCFG read context initializer.
 *
 * Initialize the YAML reader context stack.
 *
 * \param [in,out]  context     Reader context.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_outer_init_context(bcmcfg_read_context_t *context);

/*!
 * \brief BCMCFG outer read cleanup.
 *
 * Cleanup the YAML outer reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_outer_gen_cleanup(void);

/*!
 * \brief BCMCFG outer read cleanup.
 *
 * Cleanup the YAML outer reader.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_outer_unit_cleanup(int unit);

/*!
 * \brief BCMCFG read handlers unregister.
 *
 * Unregister all read handlers.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
extern int bcmcfg_read_handler_unregister_all(void);

#endif /* BCMCFG_READER_H */
