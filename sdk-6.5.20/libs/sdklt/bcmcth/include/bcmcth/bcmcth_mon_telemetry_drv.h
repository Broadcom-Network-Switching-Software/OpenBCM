/*! \file bcmcth_mon_telemetry_drv.h
 *
 * BCMCTH Monitor Telemetry driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_MON_TELEMETRY_DRV_H
#define BCMCTH_MON_TELEMETRY_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmcth/bcmcth_mon_util.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmbd/mcs_shared/mcs_mon_telemetry.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/generated/mon_telemetry_ha.h>

/*! Maximum field ID of MON logical tables. */
#define BCMMON_FIELD_ID_MAX (32)

/*! Maximum number of BTE cores available in the system. */
#define BCMMON_BTE_CORE_MAX (4)

/*! Maximum number of telemetry stats in the object. */
#define BCMMON_TELEMETRY_STATS_MAX \
    (BCMLTD_COMMON_TELEMETRY_STAT_T_T_INTF_METADATA + 1)

/*! Maximum length of port name of port specified in the object. */
#define BCMMON_TELEMETRY_MAX_PORT_NAME (10)

/*! Maximum number of telemetry objects supported in the system. */
#define BCMMON_TELEMETRY_OBJECTS_MAX (160)

/*! Maximum number of telemetry instances supported in the system. */
#define BCMMON_TELEMETRY_INSTANCES_MAX (1)

/*! Value used to initialize invalid telemetry index. */
#define BCMMON_TELEMETRY_INVALID_INDEX (-1)

/*! Number of words required for objects Bitmap*/
#define BCMMON_TELEMETRY_OBJ_BMP_WORDS (BCMMON_TELEMETRY_OBJECTS_MAX / 32) + 1

/*! Invalid/default BTE core number */
#define BCMMON_TELEMETRY_INVALID_CORE 0xFF

/*! Telemetry global information SW data structure */
typedef struct bcmcth_mon_telemetry_info_s {
    /*! Persistent HA memory */
    bcmcth_mon_telemetry_ha_t *ha_mem;
    /*! DMA buffer size */
    int dma_buffer_len;
    /*! DMA buffer physical address */
    uint64_t dma_buffer;
    /*! DMA buffer logical address */
    uint8_t* dma_logical_buffer;
} bcmcth_mon_telemetry_info_t;

/*! Global telemetry SW data structure */
extern bcmcth_mon_telemetry_info_t *g_telemetry_info[BCMDRD_CONFIG_MAX_UNITS];

/*! Macro to access the global structure */
#define MON_TELEMETRY_INFO(u_)                     (g_telemetry_info[(u_)])

/*! Opcodes for writing MON_TELEMETRY_CONTROLt hardware. */
typedef enum bcmcth_mon_telemetry_control_op_e {
    /*! Update telemetry control streaming telemetry application. */
    BCMCTH_MON_TELEMETRY_CONTROL_ST_OP_SET,

    /*! Delete telemetry control streaming telemetry application. */
    BCMCTH_MON_TELEMETRY_CONTROL_ST_OP_DEL,

    /*! Must be the last. */
    BCMCTH_MON_TELEMETRY_CONTROL_OP_COUNT

} bcmcth_mon_telemetry_control_op_t;

/*! The data structure for MON_TELEMETRY_CONTROLt entry. */
typedef struct bcmcth_mon_telemetry_control_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_TELEMETRY_CONTROLt_FIELD_COUNT);

    /*! Enable to initialize the streaming telemetry application
     *  on the specified BTE core.
     */
    bool streaming_telemetry;

    /*! Maximum export packet length. */
    uint32_t max_export_len;

    /*! Maximum number of ports that can be configured anytime. */
    uint32_t max_num_ports;

    /*! Oper state of maximum export packet length. */
    uint32_t max_export_len_oper;

    /*! Oper state of maximum number of ports. */
    uint32_t max_num_ports_oper;

    /*! Entry operational state. */
    uint32_t oper_state;

} bcmcth_mon_telemetry_control_t;

/*! Opcodes for writing MON_TELEMETRY_OBJECTt hardware. */
typedef enum bcmcth_mon_telemetry_object_op_e {
    /*! Update telemetry object stats information. */
    BCMCTH_MON_TELEMETRY_OBJECT_STAT_OP_SET,

    /*! Delete telemetry object stats information. */
    BCMCTH_MON_TELEMETRY_CONTROL_STAT_OP_DEL,

    /*! Update telemetry object port ID information. */
    BCMCTH_MON_TELEMETRY_OBJECT_PORT_OP_SET,

    /*! Delete telemetry object port ID information. */
    BCMCTH_MON_TELEMETRY_CONTROL_PORT_OP_DEL,

    /*! Update telemetry object port name information. */
    BCMCTH_MON_TELEMETRY_OBJECT_PORT_NAME_OP_SET,

    /*! Delete telemetry object port name information. */
    BCMCTH_MON_TELEMETRY_OBJECT_PORT_NAME_OP_DEL,

    /*! Must be the last. */
    BCMCTH_MON_TELEMETRY_OBJECT_OP_COUNT

} bcmcth_mon_telemetry_object_op_t;

/*! The data structure for MON_TELEMETRY_OBJECTt entry. */
typedef struct bcmcth_mon_telemetry_object_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_TELEMETRY_OBJECTt_FIELD_COUNT);

    /*! Bitmap of TELEMETRY_STAT[] to be operated. */
    SHR_BITDCLNAME(fbmp_tel_stats, BCMMON_TELEMETRY_STATS_MAX);

    /*! Bitmap of PORT_NAME[] to be operated. */
    SHR_BITDCLNAME(fbmp_port_name, BCMMON_TELEMETRY_MAX_PORT_NAME);

    /*! Index into the MON_TELEMETRY_OBJECT table. */
    uint32_t object_id;

    /*! Type of statistics that need to be collected. */
    bcmltd_common_telemetry_stat_t_t telemetry_stat[BCMMON_TELEMETRY_STATS_MAX];

    /*! Logical port ID of port being monitored. */
    uint16_t port_id;

    /*! Actual length of the port name. */
    uint8_t port_name_len;

    /*!
     * Name of the port being monitored.
     *  Should be filled with an interface name string.
     */
    uint8_t port_name[BCMMON_TELEMETRY_MAX_PORT_NAME];

} bcmcth_mon_telemetry_object_t;

/*! Opcodes for writing MON_TELEMETRY_INSTANCEt hardware. */
typedef enum bcmcth_mon_telemetry_instance_op_e {
    /*! Update telemetry instance objects information. */
    BCMCTH_MON_TELEMETRY_INSTANCE_OBJ_OP_SET,

    /*! Delete telemetry instance objects information. */
    BCMCTH_MON_TELEMETRY_INSTANCE_OBJ_OP_DEL,

    /*! Update telemetry instance collector information. */
    BCMCTH_MON_TELEMETRY_INSTANCE_COLLECTOR_OP_SET,

    /*! Delete telemetry instance collector information. */
    BCMCTH_MON_TELEMETRY_INSTANCE_COLLECTOR_OP_DEL,

    /*! Update telemetry instance export profile information. */
    BCMCTH_MON_TELEMETRY_INSTANCE_EXPORT_PROFILE_OP_SET,

    /*! Delete telemetry instance export profile information. */
    BCMCTH_MON_TELEMETRY_INSTANCE_EXPORT_PROFILE_OP_DEL,

    /*! Update instance statistics information to BTE. */
    BCMCTH_MON_TELEMETRY_INSTANCE_STATS_OP_SET,

    /*! Update instance statistics information from BTE. */
    BCMCTH_MON_TELEMETRY_INSTANCE_STATS_OP_LOOKUP,

    /*! Must be the last. */
    BCMCTH_MON_TELEMETRY_INSTANCE_OP_COUNT

} bcmcth_mon_telemetry_instance_op_t;

/*! The data structure for MON_TELEMETRY_INSTANCEt entry. */
typedef struct bcmcth_mon_telemetry_instance_s {
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(fbmp, MON_TELEMETRY_INSTANCEt_FIELD_COUNT);

    /*! Bitmap of telemetry objects be operated. */
    SHR_BITDCLNAME(fbmp_tel_objs, BCMMON_TELEMETRY_OBJECTS_MAX);

    /*! Index into the MON_TELEMETRY_INSTANCE table. */
    uint32_t instance_id;

    /*! Core on which this instance needs to be run. */
    uint32_t core_instance;

    /*! Number of valid object IDs. */
    uint32_t num_objects;

    /*! List of telemetry object IDs that need to be monitored
     *  by this instance.
     */
    uint32_t object_id[BCMMON_TELEMETRY_OBJECTS_MAX];

    /*! COLLECTOR table index. Collector to which this instance
     *  is associated.
     */
    uint32_t  collector_id;

    /*!  EXPORT_PROFILE table index. Export profile to which this
     *   instance is associated.
     */
    uint32_t  export_profile_id;

    /*! Number of pkts exported. */
    uint64_t tx_pkts;

    /*! Number of pkts that failed to export. */
    uint64_t tx_pkt_fails;

    /*! Instance operational state. */
    uint32_t oper_state;
} bcmcth_mon_telemetry_instance_t;

/*! The data structure for MON_INBAND_TELEMETRY_METADATA_FIELD_INFO entry. */
typedef struct bcmcth_mon_inband_telemetry_metadata_field_s {
    /*! Enum value of the field. */
    uint32_t fld_enum;

    /*! Size of the field in bits. */
    uint32_t fld_size;

    /*! Chunk index of field in wide chunk. */
    uint32_t wide_index;

    /*! Starting bit position of the field within wide chunk. */
    uint32_t wide_start;

    /*! Chunk index of field in narrow chunk. */
    uint32_t narrow_index;

    /*! Starting bit position of the field within narrow chunk. */
    uint32_t narrow_start;
} bcmcth_mon_inband_telemetry_metadata_field_t;

/*! The data structure for generic attributes for inband telemetry. */
typedef struct bcmcth_mon_inband_telemetry_metadata_attrs_s {
    /*! Size of the wide chunk in bits. */
    uint32_t wide_size;

    /*! Size of the narrow chunk in bits. */
    uint32_t narrow_size;

    /*! Number of supported field in metadata bus. */
    uint32_t field_num;
} bcmcth_mon_inband_telemetry_metadata_attrs_t;

/*!
 * \brief Initializes telemetry applications.
 *
 * Initializes the steaming telemetry application.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Telemetry control information
 * \param [out] output_fields out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_init(
    int unit,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *output_fields);

/*!
 * \brief De-initializes telemetry applications.
 *
 * De-initializes the steaming telemetry application.
 *
 * \param [in] unit Unit number.
 * \param [out] output_fields out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_detach(
    int unit,
    bcmltd_fields_t *output_fields);

/*!
 * \brief Update MON_TELEMETRY_CONTROLt embedded application.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_TELEMETRY_CONTROLt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation
 *             (see \ref bcmcth_mon_telemetry_control_op_t).
 * \param [in] entry Telemetry control entry to update HW.
 * \param [out] output_fields out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_mon_telemetry_control_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_telemetry_control_op_t op,
    bcmcth_mon_telemetry_control_t *entry,
    bcmltd_fields_t *output_fields);

/*!
 * \brief Update MON_TELEMETRY_INSTANCEt embedded application.
 *
 * This device-specific function is called by the IMM callback handler
 * in order to update the actual hardware registers associated with
 * the MON_TELEMETRY_INSTANCEt table.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] op Type of operation
 *             (see \ref bcmcth_mon_telemetry_instance_op_t).
 * \param [in] entry Telemetry instance entry to update HW.
 * \param [out] output_fields out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO I/O access error.
 */
extern int
bcmcth_mon_telemetry_instance_op(
    int unit,
    uint32_t trans_id,
    bcmltd_sid_t lt_id,
    bcmcth_mon_telemetry_instance_op_t op,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields);

/*!
 * \brief Allocate resources needed for telemetry applications.
 *
 * Allocates global data and DMA buffers needed for telemetry applications.
 *
 * \param [in] unit Unit number.
 * \param [in] warm warmboot or not
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_telemetry_sw_resources_alloc(
    int unit,
    bool warm);

/*!
 * \brief Free up resources used by telemetry applications.
 *
 * Deallocates global data and DMA buffers used by telemetry applications.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_telemetry_sw_resources_free(
    int unit);

/*!
 * \brief Handle the telemetry instance object updates.
 *
 * Provide updates to ST application on BTE for object updates.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to instance entry data.
 * \param [out] output_fields out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_instance_object_update(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields);

/*!
 * \brief Handle the telemetry instance object delete.
 *
 * Provide updates to ST application on BTE for object delete.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to instance entry data.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_instance_object_delete(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry);

/*!
 * \brief Handle the telemetry object LT updates for instance LT.
 *
 * Provide updates to Instance LT on object LT changes.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to instance entry data.
 * \param [in] obj_entry Pointer to object entry data.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_instance_handle_object_update(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmcth_mon_telemetry_object_t *obj_entry);

/*!
 * \brief Handle the telemetry object LT entry delete for instance LT.
 *
 * Provide updates to Instance LT on object LT entry delete.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to instance entry data.
 * \param [in] obj_entry Pointer to object entry data.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_instance_handle_object_delete(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmcth_mon_telemetry_object_t *obj_entry);

/*!
 * \brief Handle the telemetry instance collector create.
 *
 * Provide updates to ST application on BTE for collector create.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to instance entry data.
 * \param [in] max_export_len Maximum export packet length.
 * \param [out] output_fields out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_instance_collector_create(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    uint32_t max_export_len,
    bcmltd_fields_t *output_fields);

/*!
 * \brief Handle the telemetry instance collector delete.
 *
 * Provide updates to ST application on BTE for collector delete.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to instance entry data.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_instance_collector_delete(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry);

/*!
 * \brief Handle the telemetry instance stats set operation.
 *
 * Provide updates to ST application on BTE for instance stats set.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to instance entry data.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_instance_stats_set(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry);

/*!
 * \brief Handle the telemetry instance stats fetch operation.
 *
 * Provide updates to ST application on BTE for instance stats get.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Pointer to instance entry data.
 * \param [out] output_fields Out fields to be updated if any.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmcth_mon_st_instance_stats_get(
    int unit,
    bcmcth_mon_telemetry_instance_t *entry,
    bcmltd_fields_t *output_fields);

#endif /* BCMCTH_MON_TELEMETRY_DRV_H */
