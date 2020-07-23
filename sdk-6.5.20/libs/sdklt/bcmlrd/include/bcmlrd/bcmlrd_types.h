/*! \file bcmlrd_types.h
 *
 * \brief Logical Table Types
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLRD_TYPES_H
#define BCMLRD_TYPES_H

#include <sal/sal_types.h>
#include <shr/shr_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/chip/bcmlrd_limits.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_tile.h>

/*!
 * \brief Table identifier.
 *
 * Table identifier similar to those used by the DRD.
 */
typedef bcmltd_sid_t bcmlrd_sid_t;   /* Generic table ID local to symbol. */

/*!
 * \brief Field identifier.
 *
 * Field identifiers similar to those used by the DRD.
 */
typedef bcmltd_fid_t bcmlrd_fid_t;   /* DRD compatible field ID local to logical. */

/*!
 * \brief Special index indicator
 *
 * Physical tables do not have a field to specifically indicate an
 * index into a table. BCMLRD_FIELD_INDEX is an indicator that the
 * destination is to be treated as an index. It is expected that this
 * index value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_INDEX ((uint32_t)-1)

/*!
 * \brief Special port indicator
 *
 * Physical tables do not have a field to specifically indicate an
 * port index into a table. BCMLRD_FIELD_PORT is an indicator that
 * the destination is to be treated as an index. It is expected that
 * this index value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_PORT  ((uint32_t)-2)

/*!
 * \brief Special TCAM Entry priority indicator
 *
 * BCMLRD_FIELD_ENTRY_PRIORITY is an indicator that the destination is
 * to be treated as a TCAM entry priority.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_ENTRY_PRIORITY ((uint32_t)-3)

/*!
 * \brief Special table instance identifier
 *
 * BCMLRD_TABLE_INSTANCE is an indicator that the destination is to be
 * treated as a table instance identifier.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_INSTANCE ((uint32_t)-4)

/*!
 * \brief Special table selector identifier
 *
 * BCMLRD_FIELD_TABLE_SEL is an indicator that the destination is to be
 * treated as a table selector identifier.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_TABLE_SEL ((uint32_t)-5)

/*!
 * \brief Special track index indicator
 *
 * BCMLRD_FIELD_TRACK_INDEX is an indicator that the destination is to be
 * treated as a track index identifier.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_TRACK_INDEX ((uint32_t)-6)

/*!
 * \brief Special PT suppress indicator
 *
 * BCMLRD_FIELD_PT_SUPPRESS is an indicator that the destination is to be
 * treated as a PTM operation suppress identifier.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_PT_SUPPRESS ((uint32_t)-7)

/*!
 * \brief Special field selector identifier
 *
 * BCMLRD_TABLE_SELECTOR is an indicator that the destination is to be
 * treated as a virtual field selector identifier.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_SELECTOR ((uint32_t)-8)

/*!
 * \brief Special logical data type indicator
 *
 * BCMLRD_FIELD_LDTYPE is an indicator that the destination is to be
 * treated as a PDD logical data type.  It is expected that this
 * field value is beyond any actual table field IDs.
 */

#define BCMLRD_FIELD_LDTYPE ((uint32_t)-9)

/*!
 *  \brief No operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_NOP        0x00000000

/*!
 *  \brief Insert operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_INSERT     0x00000001

/*!
 *  \brief Update operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_UPDATE     0x00000002

/*!
 *  \brief Delete operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_DELETE     0x00000004

/*!
 *  \brief Delete operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_LOOKUP     0x00000008

/*!
 *  \brief Delete operation is supported.
 */
#define BCMLRD_MAP_TABLE_ENTRY_OPERATION_TRAVERSE   0x00000010


/*!
 *  \brief Standard custom table handler interface.
 */
#define BCMLRD_MAP_TABLE_ATTRIBUTE_HANDLER_INTF_STANDARD      0

/*!
 *  \brief Expedited custom table handler interface.
 */
#define BCMLRD_MAP_TABLE_ATTRIBUTE_HANDLER_INTF_EXPEDITED     1


/*!
 * \brief Invalid TILE Identifier.
 */
#define BCMLRD_INVALID_TILE BCMDRD_INVALID_TILE


/*!
 * \brief Generic mapping function pointer type
 */

typedef void (*bcmlrd_generic_f)(void);

/*!
 * \brief Generic mapping function pointer argument.
 */

typedef struct bcmlrd_generic_arg_s {
    /*! Number of Generic function arguments. */
    uint32_t values;

    /*! Array of Generic function argument values. */
    const uint32_t *value;
} bcmlrd_generic_arg_t;

/*!
 * \brief Table Map entry type.
 */
typedef enum bcmlrd_map_entry_type_e {
    /*! Map entry copies u.mapped.src to desc. */
    BCMLRD_MAP_ENTRY_MAPPED_KEY,

    /*! Map entry copies u.fixed.value to desc. */
    BCMLRD_MAP_ENTRY_FIXED_KEY,

    /*! Map entry copies u.mapped.src to desc. */
    BCMLRD_MAP_ENTRY_MAPPED_VALUE,

    /*! Map entry copies u.fixed.value to desc. */
    BCMLRD_MAP_ENTRY_FIXED_VALUE,

    /*! Forward Key Field Transform Handler. */
    BCMLRD_MAP_ENTRY_FWD_KEY_FIELD_XFRM_HANDLER,

    /*! Forward Value Field Transform Handler. */
    BCMLRD_MAP_ENTRY_FWD_VALUE_FIELD_XFRM_HANDLER,

    /*! Reverse Key Field Transform Handler. */
    BCMLRD_MAP_ENTRY_REV_KEY_FIELD_XFRM_HANDLER,

    /*! Reverse Value Field Transform Handler. */
    BCMLRD_MAP_ENTRY_REV_VALUE_FIELD_XFRM_HANDLER,

    /*! Always Reverse Value Field Transform Handler. */
    BCMLRD_MAP_ENTRY_ALWAYS_REV_VALUE_FIELD_XFRM_HANDLER,

    /*! Key Field Validation */
    BCMLRD_MAP_ENTRY_KEY_FIELD_VALIDATION,

    /*! Value Field Validation */
    BCMLRD_MAP_ENTRY_VALUE_FIELD_VALIDATION,

    /*! Table mapping is by data in u.table_handler. */
    BCMLRD_MAP_ENTRY_TABLE_HANDLER,

    /*! Field attribute of desc in  u.attribute. */
    BCMLRD_MAP_ENTRY_FIELD_ATTRIBUTE,

    /*! Map entry must be ignored. */
    BCMLRD_MAP_ENTRY_IGNORE,

    /*! Table Commit Handler. */
    BCMLRD_MAP_ENTRY_TABLE_COMMIT_HANDLER,

    /*! Table Entry Limit Handler. */
    BCMLRD_MAP_ENTRY_TABLE_ENTRY_LIMIT_HANDLER,
} bcmlrd_map_entry_type_t;

/*!
 * \brief Table Map field descriptor
 */
typedef bcmltd_field_desc_t bcmlrd_field_desc_t;

/*!
 * \brief Kind of table in descriptor.
 */
typedef enum bcmlrd_table_kind_e {
    /*! Logical table. */
    BCMLRD_MAP_LOGICAL,

    /*! Physical table. */
    BCMLRD_MAP_PHYSICAL,

    /*! Field Validation. */
    BCMLRD_MAP_VALIDATION,

    /*! Custom Table. */
    BCMLRD_MAP_CUSTOM,

    /*! LTM Managed Table. */
    BCMLRD_MAP_LTM,

    /*! Table Commit Table. */
    BCMLRD_MAP_TABLE_COMMIT,

    /*! Table Entry Limit. */
    BCMLRD_MAP_TABLE_ENTRY_LIMIT,
} bcmlrd_table_kind_t;

/*!
 * \brief Table descriptor.
 */
typedef struct bcmlrd_table_desc_s {
    /*! Table kind. */
    bcmlrd_table_kind_t kind;
    /*! Table ID. */
    uint32_t            id;
} bcmlrd_table_desc_t;

/* Temporary: tcam ordering */
enum {
    LPM,
    ALPM,
};

/*!
 * \brief Transform field descriptor.
 */
typedef struct bcmlrd_field_xfrm_desc_s {
    /*! LTA Handler ID. */
    uint32_t handler_id;

    /*! Number of source transform fields. */
    uint32_t src_fields;

    /*! List of source field descriptors. */
    const bcmltd_field_desc_t *src;

    /*! Number of source transform fields. */
    uint32_t dst_fields;

    /*! List of source field descriptors. */
    const bcmltd_field_desc_t *dst;

    /*! Number of tables in table transform. */
    uint32_t tables;

    /*! List of source field descriptors. */
    const uint32_t *tbl;
} bcmlrd_field_xfrm_desc_t;

/*!
 * \brief Table Map entry - per destination field.
 */
typedef struct bcmlrd_map_entry_s {
    /*! Map entry type. */
    bcmlrd_map_entry_type_t entry_type;

    /*! Mapping field descriptor. */
    bcmlrd_field_desc_t desc;

    union {
        /*! Mapped Source field. */
        struct {
            /*! Source field descriptor. */
            bcmlrd_field_desc_t src;
        } mapped;

        /*! Fixed mapping data. */
        struct {
            /*! Destination value. */
            uint32_t            value;
            /*! Default value. */
            uint32_t            default_value;
        } fixed;

        /*! Special handling. */
        struct {
            /*! Field handler. (deprecated) */
            bcmlrd_generic_f handler;

            /*! Handler argument list. */
            const bcmlrd_generic_arg_t *arg;
        } special;

        /*! Transform descriptor. */
        struct {
            const bcmlrd_field_xfrm_desc_t *desc;
        } xfrm;

        /*! LTA Handler ID. */
        uint32_t handler_id;

    } u; /*!< Set of mapping attribute structures. */

} bcmlrd_map_entry_t;

/*!
 * \brief Table Map group - per destination table
 */
typedef struct bcmlrd_map_group_s {
    /*! Map destination type. */
    bcmlrd_table_desc_t dest;

    /*! Number of map entries in this configuration. */
    uint32_t entries;

    /*! Variable length array of maps in this group. */
    const bcmlrd_map_entry_t *entry;
} bcmlrd_map_group_t;

/*!
 * \brief Table Map attributes
 */
typedef enum bcmlrd_map_table_attr_name_s {
    BCMLRD_MAP_TABLE_ATTRIBUTE_INTERACTIVE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_MIN_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_MAX_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_SIZE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TABLE_TYPE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MIN_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_MAX_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_PORT_SIZE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_MIN_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_MAX_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_INSTANCE_SIZE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TCAM_ORDERING,
    BCMLRD_MAP_TABLE_ATTRIBUTE_SHARED_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TRACK_INDEX_MIN_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TRACK_INDEX_MAX_INDEX,
    BCMLRD_MAP_TABLE_ATTRIBUTE_TRACK_INDEX_SIZE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_ENTRY_LIMIT,
    BCMLRD_MAP_TABLE_ATTRIBUTE_PREPOPULATE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_RESOURCE_INFO,
    BCMLRD_MAP_TABLE_ATTRIBUTE_HARDWARE,
    BCMLRD_MAP_TABLE_ATTRIBUTE_HANDLER_INTF,
} bcmlrd_map_table_attr_name_t;

/*!
 * \brief Table attribute
 */
typedef struct bcmlrd_map_table_attr_s {
    /*! Attribute key. */
    bcmlrd_map_table_attr_name_t key;

    /*! Attribute value. */
    uint32_t value;
} bcmlrd_map_table_attr_t;

/*!
 * \brief Table attributes - per logical table
 */
typedef struct bcmlrd_map_attr_s {
    /*! Number of attributes. */
    uint32_t attributes;

    /*! Array of table attribute. */
    const bcmlrd_map_table_attr_t *attr;
} bcmlrd_map_attr_t;

/*!
 * \brief Table Map field data - per logical table
 */
typedef struct bcmlrd_field_data_s {
    /*! Field flags from mapping. */
    uint32_t flags;

    /*! Minimum field value. */
    const bcmltd_field_data_t *min;

    /*! Default field value. */
    const bcmltd_field_data_t *def;

    /*! Maximum field value. */
    const bcmltd_field_data_t *max;

    /*! Width of field in bits. */
    uint32_t width;

    /*! Depth of field in elements. */
    uint32_t depth;

    /*! Enumeration data if needed. */
    const bcmltd_enum_type_t *edata;
} bcmlrd_field_data_t;

/*!
 * \brief Table Map field data - per logical table
 */
typedef struct bcmlrd_map_field_data_s {
    /*! Number field data entries. */
    uint32_t fields;

    /*! Variable length array of field data. */
    const bcmlrd_field_data_t *field;
} bcmlrd_map_field_data_t;

/*!
 * \brief Mapping condition operators.
 */
typedef enum bcmlrd_cond_op_e {
    /*! Logical AND condition. */
    BCMLRD_CONDITION_AND,

    /*! Logical OR condition. */
    BCMLRD_CONDITION_OR,
} bcmlrd_cond_op_t;

/*!
 * \brief Mapping conditions.
 */
typedef struct bcmlrd_condition_s {
    /*! Condition operator. */
    bcmlrd_cond_op_t op;

    /*! Condition table. */
    bcmlrd_sid_t  table;

    /*! Condition field. */
    bcmlrd_fid_t  field;

    /*! Condition value. */
    uint64_t value;

} bcmlrd_condition_t;

/*!
 * \brief Mapping condition expressions.
 */
typedef struct bcmlrd_cond_expr_s {
    /*! Number of conditions in this expression. */
    size_t num_cond;

    /*! First condition in this expression. */
    const bcmlrd_condition_t *cond;
} bcmlrd_cond_expr_t;


/*!
 * \brief Field condition expressions.
 */
typedef struct bcmlrd_cond_field_s {
    /*! Conditioned field. */
    bcmlrd_fid_t field;

    /*! Condition expression. */
    const bcmlrd_cond_expr_t *expr;

    /*! Groups/Entries associated with field. */
    const uint32_t *edit;

} bcmlrd_cond_field_t;

/*!
 * \brief Field condition expressions.
 */
typedef struct bcmlrd_cond_data_s {
    /*! Table condition expression. */
    const bcmlrd_cond_expr_t *table_expr;

    /*! Number of field conditions. */
    size_t num_field_cond;

    /*! Field condition expressions. */
    const bcmlrd_cond_field_t *field_cond;

} bcmlrd_cond_data_t;

/*!
 * \brief Field selector types.
 */
typedef enum bcmlrd_field_selector_type_e {
    /*! Field. */
    BCMLRD_FIELD_SELECTOR_TYPE_FIELD,

    /*! Map entry. */
    BCMLRD_FIELD_SELECTOR_TYPE_MAP_ENTRY,

    /*! Root. */
    BCMLRD_FIELD_SELECTOR_TYPE_ROOT,
} bcmlrd_field_selector_type_t;

/*!
 * \brief Invalid Selector Index.
 */
#define BCMLRD_INVALID_SELECTOR_INDEX ((uint32_t)-1)

/*!
 * \brief Field selector type.
 */
typedef struct bcmlrd_field_selector_s {

    /*! Selector type. */
    bcmlrd_field_selector_type_t selector_type;

    /*! Field ID of selected field when selector type is not 'root'. */
    bcmlrd_fid_t        field_id;

    /*! Group index of selected mapping when selector type is 'map_entry'. */
    uint32_t            group_index;

    /*! Entry index of selected mapping when selector type is 'map_entry'. */
    uint32_t            entry_index;

    /*! Selection parent index or BCMLRD_INVALID_SELECTOR_INDEX. */
    uint32_t            selection_parent;

    /*!
     * Selection group.
     *
     * The selection group is used to identify those fields
     * which are valid or absent based on the field selector value.
     *
     * If the selected field is specified in all selector values
     * of a field selector, then the field is always present.
     * The field is logically not being subject to any selection.
     * In this case, the invalid group BCMLRD_INVALID_SELECTOR_INDEX
     * is assigned.
     */
    uint32_t            group;

    /*! Selector field ID. */
    bcmlrd_fid_t        selector_id;

    /*! Selector value. */
    uint64_t            selector_value;

} bcmlrd_field_selector_t;

/*!
 * \brief Field selector information.
 */
typedef struct bcmlrd_field_selector_data_s {
    /*! Number of field conditions. */
    size_t num_field_selector;

    /*! Field condition expressions. */
    const bcmlrd_field_selector_t *field_selector;

} bcmlrd_field_selector_data_t;

/*!
 * \brief Table Map - per logical table.
 */
typedef struct bcmlrd_map_s {
    /*! Logical Table source ID. */
    uint32_t src_id;

    /*! Field data. */
    const bcmlrd_map_field_data_t *field_data;

    /*! Number of map groups. */
    uint32_t groups;

    /*! Variable length array of map groups. */
    const bcmlrd_map_group_t *group;

    /*! Table attributes. */
    const bcmlrd_map_attr_t *table_attr;

    /*! Table conditions. */
    const bcmlrd_cond_data_t *cond;

    /*! Table entry opcodes. */
    uint32_t entry_ops;

    /*! Field selectors. */
    const bcmlrd_field_selector_data_t *sel;

} bcmlrd_map_t;

/*!
 * \brief Information on match ID fields.
 *
 * This structure is used to store information for each
 * match id field.
 */
typedef struct bcmlrd_match_id_db_s {
    /*! Match ID name. */
    const char *name;

    /*! Match. */
    uint32_t match;

    /*! Mask for match. */
    uint32_t match_mask;

    /*! Maxbit of the match id field in the physical container. */
    uint8_t match_maxbit;

    /*! Minbit of the match id field in the physical container. */
    uint8_t match_minbit;

    /*! Maxbit of the match id field. */
    uint8_t maxbit;

    /*! Minbit of the match id field. */
    uint8_t minbit;

    /*! Default value for the match id field. */
    uint32_t value;

    /*! Mask for the default value for the match id field. */
    uint32_t mask;

    /*! Maxbit of the field within match_id container. */
    uint8_t pmaxbit;

    /*! Minbit of the field within match_id container. */
    uint8_t pminbit;
} bcmlrd_match_id_db_t;

/*!
 * \brief Information on match ID fields.
 *
 * This structure is used to store information match id data.
 */
typedef struct bcmlrd_match_id_db_info_s {
    /*! Number of entries in the match ID DB. */
    uint32_t num_entries;

    /*! Pointer to match ID DB. */
    const bcmlrd_match_id_db_t *db;
} bcmlrd_match_id_db_info_t;

/*!
 * \brief Table Map configuration storage representation
 */
typedef struct bcmlrd_map_conf_rep_s {
    /*! Map configuration name. */
    const char *name;

    /*! Probe function for configuration. */
    int (*probe)(int unit);

    /*! Fixed length array of maps in this configuration index by lsid. */
    const bcmlrd_map_t *map[BCMLRD_TABLE_COUNT];

} bcmlrd_map_conf_rep_t;

/*!
 * \brief Information on physical containers.
 *
 * This structure is used to store information for each
 * physical container that a logical field is mapped to.
 */
typedef struct bcmlrd_cont_info_s {
    /*! Section in which the container is available. */
    uint8_t section_id;

    /*! Offset of the container within the section. */
    uint16_t cont_id;

    /*! Width of the container in the section. */
    uint8_t width;

    /*! Bit offset of the container within the section. */
    uint16_t bit_offset;
} bcmlrd_cont_info_t;

/*!
 * \brief PDD information for physical containers.
 *
 * This structure is used to store PDD information for each
 * physical container that a logical field is mapped to.
 */
typedef struct bcmlrd_pdd_info_s {
    /*!
     * Physical container id. This is the bit id of
     * the physical container in the PDD bitmap.
     */
    uint16_t phy_cont_id;

    /*!
     * SBR Physical container id. This is the bit id of
     * the physical container in the SBR bitmap.
     */
    uint16_t sbr_phy_cont_id;

    /*! Physical container size. */
    uint16_t phy_cont_size;

    /*! Offset of action in the physical container. */
    uint8_t offset;

    /*! Width of action in the physical container from the offset */
    uint8_t width;

    /*! If set, then PDD is aligned from LSB. */
    bool is_lsb;
} bcmlrd_pdd_info_t;

/*!
 * \brief Container information per logical field.
 *
 * This structure is used to maintain the container information
 * per logical field.
 *
 * Each logical field can be mapped to multiple containers.
 * In which case, the information would be available as
 * an array of this structure.
 * Count specifies the array length.
 */
typedef struct bcmlrd_field_cont_info_s {
    /*! Number of instances that physical container is mapped in the TILE. */
    uint8_t instances;

    /*! Number of containers that logical field is mapped to. */
    uint8_t count;

    /*! Physical container information. */
    const bcmlrd_cont_info_t *info;
} bcmlrd_field_cont_info_t;

/*!
 * \brief SBR type.
 */
typedef enum bcmlrd_field_sbr_type_e {
    /*! Non SBR eligible action. */
    BCMLRD_SBR_NONE,

    /*! Non SBR eligible action, mapped to SBR container. */
    BCMLRD_SBR_INTERNAL,

    /*! SBR eligible action, mapped to SBR container. */
    BCMLRD_SBR_EXTERNAL,
} bcmlrd_field_sbr_type_t;

/*!
 * \brief PDD information on containers per logical field.
 *
 * This structure is used to maintain the PDD information for containers
 * per logical field.
 *
 * Each logical field can be mapped to multiple containers.
 * In which case, the information would be available as
 * an array of this structure.
 * Count specifies the array length.
 */
typedef struct bcmlrd_field_pdd_info_s {
    /*!  SBR type of the field. */
    bcmlrd_field_sbr_type_t sbr_type;

    /*! Number of containers that logical field is mapped to. */
    uint8_t count;

    /*! PDD information for each physical container. */
    const bcmlrd_pdd_info_t *info;
} bcmlrd_field_pdd_info_t;

/*!
 * \brief Container map information for logical field.
 *
 * This structure provides container and PDD information for
 * each physical container that the logical field is mapped to.
 */
typedef struct bcmlrd_field_info_s {
    /*! Name of the physical field. */
    const char *name;

   /*! Field ID. */
   bcmltd_fid_t id;

   /*! Container information for the logical field. */
   const bcmlrd_field_cont_info_t *cont_info;

   /*! PDD container information for the logical field. */
   const bcmlrd_field_pdd_info_t *pdd_info;

} bcmlrd_field_info_t;

/*!
 * \brief Table information for the special tables.
 *
 * This structure provides physical container information for each
 * logical field.
 */
typedef struct bcmlrd_table_pcm_info_s {
    /*! Logical Table source ID. */
    uint32_t src_id;

    /*! Number of fields in the table. */
    uint16_t field_count;

    /*! Field information for each field. */
    const bcmlrd_field_info_t *field_info;

    /*! Mux information for this logical table. */
    uint32_t tile_id;
} bcmlrd_table_pcm_info_t;

/*!
 * \brief Table PCM configuration storage representation.
 */
typedef struct bcmlrd_pcm_conf_rep_s {
    /*! PCM configuration name. */
    char name[80];

    /*! Fixed length array of PCM configurations index by LTID. */
    const bcmlrd_table_pcm_info_t *pcm[BCMLRD_TABLE_COUNT];

} bcmlrd_pcm_conf_rep_t;

/*!
 * \brief Table PCM configuration storage representation.
 */
typedef struct bcmlrd_pcm_conf_s {
    /*! Fixed length array of PCM configurations index by LTID. */
    const bcmlrd_table_pcm_info_t *pcm[BCMLRD_TABLE_COUNT];
} bcmlrd_pcm_conf_t;

/*!
 * \brief Table PCM configuration storage compact representation.
 */
typedef struct bcmlrd_pcm_conf_compact_rep_s {
    /*! PCM configuration name. */
    const char *name;

    /*! Number of tables that support PCM in the device. */
    uint32_t num_pcm;

    /*! Pointer to the array of PCM configurations indexed by LTID. */
    const bcmlrd_table_pcm_info_t **pcm;

} bcmlrd_pcm_conf_compact_rep_t;

/*!
 * \brief Function pointer to retrieve the match id information.
 */
typedef int (*bcmlrd_match_id_db_get_t)(int unit, const bcmlrd_sid_t sid,
                                 const bcmlrd_fid_t fid,
                                 const bcmlrd_match_id_db_t **info);

/*!
 * \brief Index table flag.
 */
#define BCMLRD_TABLE_F_TYPE_INDEX BCMLTD_TABLE_F_TYPE_INDEX

/*!
 * \brief TCAM table flag.
 */
#define BCMLRD_TABLE_F_TYPE_TCAM BCMLTD_TABLE_F_TYPE_TCAM

/*!
 * \brief HASH table flag.
 */
#define BCMLRD_TABLE_F_TYPE_HASH BCMLTD_TABLE_F_TYPE_HASH

/*!
 * \brief Table type mask.
 */
#define BCMLRD_TABLE_F_TYPE_MASK BCMLTD_TABLE_F_TYPE_MASK

/*!
 * \brief Table type mapped.
 */
#define BCMLRD_TABLE_F_TYPE_MAPPED BCMLTD_TABLE_F_TYPE_MAPPED

/*!
 * \brief Table read-only flag.
 */
#define BCMLRD_TABLE_F_READ_ONLY BCMLTD_TABLE_F_READ_ONLY

/*!
 * \brief Table type mask.
 */
#define BCMLRD_TABLE_F_TYPE_INDEX_ALLOC BCMLTD_TABLE_F_TYPE_INDEX_ALLOC

/*!
 * \brief Field untyped flag.
 */
#define BCMLRD_FIELD_F_UNTYPED BCMLTD_FIELD_F_UNTYPED

/*!
 * \brief Field special type mask.
 */
#define BCMLRD_FIELD_F_TYPE_MASK BCMLTD_FIELD_F_TYPE_MASK

/*!
 * \brief Field read-only flag.
 */
#define BCMLRD_FIELD_F_READ_ONLY BCMLTD_FIELD_F_READ_ONLY

/*!
 * \brief Field "is key" flag.
 */
#define BCMLRD_FIELD_F_KEY BCMLTD_FIELD_F_KEY

/*!
 * \brief Field "is unmapped" flag.
 */
#define BCMLRD_FIELD_F_UNMAPPED BCMLTD_FIELD_F_UNMAPPED

/*!
 * \brief Field enumeration flag.
 */
#define BCMLRD_FIELD_F_ENUM BCMLTD_FIELD_F_ENUM

/*!
 * \brief Field non-cacheable flag.
 */
#define BCMLRD_FIELD_F_NON_CACHEABLE BCMLTD_FIELD_F_NON_CACHEABLE



/*!
 * \brief Field storage representation for components using LRD types
 */
typedef bcmltd_field_rep_t bcmlrd_field_rep_t;

/*!
 * \brief Table storage representation for components using LRD types
 */
typedef bcmltd_table_rep_t bcmlrd_table_rep_t;

/*!
 * \brief Field default type
 */
typedef bcmltd_field_def_t bcmlrd_field_def_t;

/*!
 * \brief Device configuration structure.
 *
 * Per-device map configuration.
 */
typedef struct bcmlrd_dev_conf_s {
    /*! Map configuration. */
    const bcmlrd_map_conf_rep_t *conf;
} bcmlrd_dev_conf_t;

/*!
 * \brief Device PCM configuration structure.
 *
 * Per-device PCM configuration.
 */
typedef struct bcmlrd_dev_pcm_conf_s {
    /*! PCM configuration. */
    const bcmlrd_pcm_conf_compact_rep_t *(*pcm_conf_get)(void);
} bcmlrd_dev_pcm_conf_t;

/*!
 * \brief LRD device configuration structure.
 *
 * Per-device LRD configuration.
 */
typedef struct bcmlrd_device_conf_s {
    /*! Device map configuration. */
    bcmlrd_dev_conf_t   *lrd_dev_conf;

    /*! Table configuration. */
    const bcmltd_table_conf_t *ltd_table_conf;
} bcmlrd_device_conf_t;

/*!
 * \brief LRD structure for each handler in component.
 *
 * This structure provides information on handlers defined per component.
 */
typedef struct bcmlrd_comp_hdl_s {
    /*! Handler name. */
    const char *hdl_name;

    /*! Number of tables supported by this handler. */
    uint32_t num_tbl;

    /*! Tables supported by this handler. */
    const bcmlrd_sid_t *tbl;
} bcmlrd_comp_hdl_t;

/*!
 * \brief LRD structure for component data.
 *
 * This structure provides component list per device.
 */
typedef struct bcmlrd_comp_desc_s {
    /*! Component name. */
    const char *comp_name;

    /*! Number of handlers in this component. */
    uint32_t num_hdl;

    /*! Handlers in this component. */
    const bcmlrd_comp_hdl_t *hdl;
} bcmlrd_comp_desc_t;

/*!
 * \brief LRD structure for device level component data.
 *
 * This structure provides information on list of components
 * supported in the device.
 */
typedef struct bcmlrd_dev_comp_s {
    /*! Number of components. */
    uint32_t num_comp;

    /*! Component data. */
    const bcmlrd_comp_desc_t *desc;
} bcmlrd_dev_comp_t;

/*!
 * \brief Device component configuration structure.
 *
 * Per-device component configuration.
 */
typedef struct bcmlrd_dev_comp_conf_s {
    /*! Component configuration. */
    const bcmlrd_dev_comp_t *conf;
} bcmlrd_dev_comp_conf_t;

#endif /* BCMLRD_TYPES_H */
