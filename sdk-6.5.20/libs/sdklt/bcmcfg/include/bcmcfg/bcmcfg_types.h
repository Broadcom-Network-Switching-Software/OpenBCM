/*! \file bcmcfg_types.h
 *
 * BCMCFG types primarily for YAML readers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCFG_TYPES_H
#define BCMCFG_TYPES_H

#include <shr/shr_types.h>

/*!
 * \brief Indicates that there no index present.
 */
#define BCMCFG_NO_IDX ((uint32_t)-1)

/*!
 * \brief Component schema node.
 *
 * Component schema node to guide the component YAML reader.
 *
 * The component schema is an array of component nodes, with the map
 * nodes forming a linked list - linked by an index within the
 * array. This describes both how to interpret an element in the YAML
 * input, and where to store the data in the backing storage.
 */
typedef struct bcmcfg_comp_node_s {
    /*! Component node type. */
    enum {
        /*! Scalar node. */
        BCMCFG_COMP_SCALAR,

        /*! Map node. */
        BCMCFG_COMP_MAP,
    } node;

    /*! If greater than 0, then this node is an array of that length. */
    uint32_t array;

    /*! Map or scalar key. */
    const char *key;

    /*! Index of next node or BCMCFG_NO_IDX. */
    uint32_t next;

    /*! Storage offset in bytes. */
    size_t offset;

    /*! Data size in bytes. */
    size_t size;
} bcmcfg_comp_node_t;

/*!
 * \brief Component scanner.
 *
 * A component scanner is an array of component schema nodes, and
 * a pointer to a pointer to a region that contains parsed data.
 */
typedef struct bcmcfg_comp_scanner_s {
    /*! Number schema nodes.*/
    uint32_t schema_count;

    /*! Variable length array of schema nodes. */
    const bcmcfg_comp_node_t *schema;

    /*! Size of backing store. */
    size_t data_size;

    /* Note the double indirection. This allows the scanner data
     structure to be entirely const, but the actual storage of the
     parsed configuration data to be writable. */

    /*! Pointer to backing store. */
    uint32_t **data;

    /*! Pointer to default data. */
    uint32_t *default_data;
} bcmcfg_comp_scanner_t;

/*!
 * \brief Component scanner configuration.
 *
 * A collection of component scanners. Each scanner describes a
 * software component schema.
 */
typedef struct bcmcfg_comp_scanner_conf_s {
    /*! Number of component scanners. */
    uint32_t count;

    /*! Variable length array of component scanners in this configuration. */
    const bcmcfg_comp_scanner_t * const *scanner;
} bcmcfg_comp_scanner_conf_t;

/*!
 * \brief YAML reader state.
 *
 * Reader state is tracked by the various data readers, and represents
 * the current YAML event context.
 */
typedef enum bcmcfg_read_state_e {
    /*! Undefined YAML reader state. */
    BCMCFG_READ_STATE_NONE,

    /*! Reading a YAML stream. */
    BCMCFG_READ_STATE_STREAM,

    /*! Reading a YAML document. */
    BCMCFG_READ_STATE_DOC,

    /*! Reading a YAML sequence. */
    BCMCFG_READ_STATE_SEQ,

    /*! Reading a YAML map key. */
    BCMCFG_READ_STATE_MAP,

    /*! Reading a YAML map value. */
    BCMCFG_READ_STATE_MAP_V,
} bcmcfg_read_state_t;

/* This could eventually be derived from analysis */
/*!
 * \brief Maximum YAML state depth.
 *
 * To ease implementation, there is a fixed upper limit on the depth
 * of the YAML reader. This roughly corresponds to the "indent level"
 * of the YAML file. The value given is expected to be much larger
 * then what would be considered practical.
 */
#define BCMCFG_MAX_LEVEL 20

/*!
 * \brief YAML per level reader information.
 *
 * This contains generic information that may be needed by all readers
 * this is derivable from LibYAML event processing.
 */
typedef struct bcmcfg_read_level_info_s {
    /*! Reader state. */
    bcmcfg_read_state_t                 state;

    /*! Level of the sequence container if present. */
    int                                 seq_container;

    /*! Current sequence index if in a container. */
    uint32_t                            seq_idx;

    /*! Reader locus (filename or string). */
    const char *locus;

    /*! Reader line number. */
    int line;

    /*! Reader column number. */
    int column;

    /*! Read handler for this level. */
    const struct bcmcfg_read_handler_s *handler;

    /*! Private data for the read handler. */
    void                               *user_data;
} bcmcfg_read_level_info_t;

/*!
 * \brief YAML read context.
 *
 * Current read nesting level, and info for each level all
 * all those below.
 */
typedef struct bcmcfg_read_context_s {
    /*! Current reader level. */
    int level;

    /*! Info for each level. */
    bcmcfg_read_level_info_t info[BCMCFG_MAX_LEVEL];
} bcmcfg_read_context_t;

/*!
 * \brief Initialization stage.
 */
typedef enum bcmcfg_init_stage_e {
    /*! Early initialization stage. */
    INIT_STAGE_EARLY,

    /*! Initialization stage. */
    INIT_STAGE_NORMAL
} bcmcfg_init_stage_t;

/*!
 * \brief YAML parse mode.
 */
typedef enum bcmcfg_parse_mode_e {
    /*! Parse mode: Generic. */
    PARSE_MODE_GEN,

    /*! Parse mode: Per-unit early init. */
    PARSE_MODE_UNIT_EARLY,

    /*! Parse mode: Per-unit init. */
    PARSE_MODE_UNIT
} bcmcfg_parse_mode_t;

/*!
 * \brief Parse info.
 */
typedef struct bcmcfg_parse_info_s {
    /*! Parse unit. */
    bcmcfg_parse_mode_t mode;

    /*! Target unit. */
    int target_unit;
} bcmcfg_parse_info_t;

/*!
 * \brief Reader setup function.
 *
 * Prototype for a YAML reader setup function.
 *
 * \param [in]  start           Start (true) or stop (false).
 * \param [in]  info            Parse info.
 * \param [in]  user_data       Read handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmcfg_read_s_f)(bool start,
                               bcmcfg_parse_info_t *info,
                               void *user_data);

/*!
 * \brief Reader element function.
 *
 * Prototype for a YAML reader non-scalar element function.
 *
 * \param [in]  start           Start (true) or stop (false).
 * \param [in]  context         Reader context.
 * \param [in]  user_data       Read handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmcfg_read_e_f)(bool start,
                               bcmcfg_read_context_t *context,
                               void *user_data);

/*!
 * \brief Reader scalar function.
 *
 * Prototype for a YAML reader scalar element function.
 *
 * \param [in]  value           Scalar value.
 * \param [in]  context         Reader context.
 * \param [in]  user_data       Read handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmcfg_read_v_f)(const char *value,
                               bcmcfg_read_context_t *context,
                               void *user_data);

/*!
 * \brief Abort function.
 *
 * Prototype for a YAML reader abort function.
 *
 * \param [in]  context         Reader context.
 * \param [in]  user_data       Read handler user data.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
typedef int (*bcmcfg_read_n_f)(bcmcfg_read_context_t *context,
                               void *user_data);

/*!
 * \brief Reader handler.
 *
 * YAML reader handler structure that contains the handler
 * functions and the user data. Functions not required may
 * be set to NULL.
 */
typedef struct bcmcfg_read_handler_s {
    /*! Key string for this handler. */
    const char *key;

    /*! Setup/Cleanup function. */
    const bcmcfg_read_s_f setup;

    /*! Document start/stop function. */
    const bcmcfg_read_e_f doc;

    /*! Scalar read function. */
    const bcmcfg_read_v_f scalar;

    /*! Sequence start/stop function. */
    const bcmcfg_read_e_f seq;

    /*! Map start/stop function. */
    const bcmcfg_read_e_f map;

    /*! Abort notification function. */
    const bcmcfg_read_n_f abort;

    /*! Handler user data. */
    void *user_data;
} bcmcfg_read_handler_t;

#endif /* BCMCFG_TYPES_H */
