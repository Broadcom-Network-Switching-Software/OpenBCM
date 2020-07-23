/*! \file cosq_thd.c
 *
 * BCM level APIs for cosq_thd.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/cosq_thd.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/mbcm/cosq_thd.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/dev.h>

#include <shr/shr_debug.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <bcmlt/bcmlt.h>
#include <bcmltd/chip/bcmltd_str.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

/*
 * \brief COSQ_THD information data structure.
 */
typedef struct cosq_thd_info_s {

    /* Module is initialized. */
    bool inited;

    /* Module lock. */
    sal_mutex_t mutex;

} cosq_thd_info_t;

/*
 * Static global variable to hold COSQ_THD info.
 */
static cosq_thd_info_t cosq_thd_info[BCM_MAX_NUM_UNITS] = {{0}};

/* COSQ_THD info. */
#define COSQ_THD_INFO(unit) \
    (&cosq_thd_info[unit])

/* Check if COSQ_THD module is initialized. */
#define COSQ_THD_INIT_CHECK(unit) \
    do { \
        cosq_thd_info_t *cosq_thd = COSQ_THD_INFO(unit); \
        if (cosq_thd->inited == false) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/* Take COSQ_THD lock. */
#define COSQ_THD_LOCK(unit) \
    sal_mutex_take(COSQ_THD_INFO(unit)->mutex, SAL_MUTEX_FOREVER)

/* Give COSQ_THD lock. */
#define COSQ_THD_UNLOCK(unit) \
    sal_mutex_give(COSQ_THD_INFO(unit)->mutex)

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public functions
 */

int
bcmi_ltsw_cosq_thd_port_uc_spid_get(int unit, bcm_port_t port, int queue,
                                    int *spid, int *use_qgroup_min)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_uc_spid_get(unit, port, queue, spid, use_qgroup_min));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_thd_shared_limit_get(int unit, int itm, int spid,
                                    int *shared_limit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_shared_limit_get(unit, itm, spid, shared_limit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_thd_egress_pool_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_control_get(unit, gport, cosq, BCM_COSQ_BUFFER_ID_INVALID,
                                        type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_cosq_thd_ing_pg_pool_get(int unit, bcm_gport_t gport, bcm_cos_t prigroup,
                                   bcm_cosq_control_t type, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_control_get(unit, gport, prigroup, BCM_COSQ_BUFFER_ID_INVALID,
                                        type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the profile for Input priority to PG mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] profile_id Profile index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_port_inppri_profile_set(int unit, bcm_port_t port, int profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_inppri_profile_set(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the profile for Input priortty to PG mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] profile_id Profile index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_port_inppri_profile_get(int unit, bcm_port_t port, int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_inppri_profile_get(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Set the profile for PG to SPID/HPID/PFCPRI mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [in] profile_id Profile index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_port_pg_profile_set(int unit, bcm_port_t port, int profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_pg_profile_set(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the profile for PG to SPID/HPID/PFCPRI mapping for a port.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 * \param [out] profile_id Profile index.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_port_pg_profile_get(int unit, bcm_port_t port, int *profile_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_pg_profile_get(unit, port, profile_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Reset the guarantee of priority group, uc queue, mc queue to the default.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_port_guarantee_reset(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_guarantee_reset(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert BCM enum alpha value to LT enum value.
 *
 * \param [in] unit Unit Number.
 * \param [in] arg BCM Alpha value.
 * \param [out] alpha LT Alpha value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_alpha_bcm_to_lt_convert(
    int unit,
    int arg,
    const char **alpha)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_alpha_bcm_to_lt_convert(unit, arg, alpha));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert LT enum alpha value to BCM enum value.
 *
 * \param [in] unit Unit Number.
 * \param [in] alpha LT Alpha value.
 * \param [out] arg BCM Alpha value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_alpha_lt_to_bcm_convert(
    int unit,
    const char *alpha,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_alpha_lt_to_bcm_convert(unit, alpha, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert byte to cell.
 *
 * \param [in] unit Unit Number.
 * \param [in] bytes bytes.
 * \param [out] cells cells.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_byte_to_cell(
    int unit,
    uint32_t bytes,
    int *cells)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_byte_to_cell(unit, bytes, cells));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Convert cell to byte.
 *
 * \param [in] unit Unit Number.
 * \param [in] cells cells.
 * \param [out] bytes bytes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_cell_to_byte(
    int unit,
    uint32_t cells,
    int *bytes)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_cell_to_byte(unit, cells, bytes));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the cosq_thd module.
 *
 * Initial cosq_thd database data structure and
 * clear the cosq_thd related Logical Tables.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_thd_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (COSQ_THD_INFO(unit)->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_thd_detach(unit));
    }

    if (COSQ_THD_INFO(unit)->mutex == NULL) {
        COSQ_THD_INFO(unit)->mutex = sal_mutex_create("COSQ_THD mutex");
        SHR_NULL_CHECK(COSQ_THD_INFO(unit)->mutex, SHR_E_MEMORY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_init(unit));

    COSQ_THD_INFO(unit)->inited = true;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach the cosq_thd module.
 *
 * Detach cosq_thd database data structure.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE Success.
 */
int
bcmi_ltsw_cosq_thd_detach(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    /* If not initialized, return success. */
    if (COSQ_THD_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    COSQ_THD_INFO(unit)->inited = false;

    COSQ_THD_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_detach(unit));

    COSQ_THD_UNLOCK(unit);
    locked = false;

    if (COSQ_THD_INFO(unit)->mutex) {
        sal_mutex_destroy(COSQ_THD_INFO(unit)->mutex);
        COSQ_THD_INFO(unit)->mutex = NULL;
    }

exit:
    if (locked) {
        COSQ_THD_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get specified feature configuration
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   type                Feature.
 * \param [out]  arg                 Feature value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_control_get(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_control_get(unit, port, cosq, BCM_COSQ_BUFFER_ID_INVALID,
                                        type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set specified feature configuration
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   cosq                COS queue.
 * \param [in]   type                Feature.
 * \param [in]   arg                 Feature value.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_control_set(int unit, bcm_gport_t port, bcm_cos_queue_t cosq,
                          bcm_cosq_control_t type, int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_control_set(unit, port, cosq, BCM_COSQ_BUFFER_ID_INVALID,
                                        type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Check if a cosq_control_type property can be changed dynamically.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   type         type of operation.
 * \param [out]  dynamic      returns if type can be changed dynamically.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_control_dynamic_get(int unit, bcm_cosq_control_t type,
                                  bcm_cosq_dynamic_setting_type_t *dynamic)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_dynamic_get(unit, type, dynamic));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get various features at the gport, cosq, and/or other level.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   id           Compound index containing gport, cosq, buffer.
 * \param [out]  control      Operation type and corresponding argument.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_control_extended_get(int unit, bcm_cosq_object_id_t *id,
                                   bcm_cosq_control_data_t *control)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_control_get(unit, id->port, id->cosq, id->buffer,
                                        control->type, &(control->arg)));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set various features at the gport, cosq, and/or other level.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   id           Compound index containing gport, cosq, buffer.
 * \param [in]   control      Operation type and corresponding argument.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_control_extended_set(int unit, bcm_cosq_object_id_t *id,
                                   bcm_cosq_control_data_t *control)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_control_set(unit, id->port, id->cosq, id->buffer,
                                        control->type, control->arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get mutliple buffer ids associated with a specified port.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        gport id.
 * \param [in]   cosq         Reserved field.
 * \param [in]   direction    Specify ingress or egress direction.
 * \param [in]   array_max    number of entries to be retrieved.
 * \param [out]  buf_id_array Buffer id array.
 * \param [out]  array_count  Actural buffer id count in buf_id_array.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_buffer_id_multi_get(int unit, bcm_gport_t gport,
                                  bcm_cos_queue_t cosq,
                                  bcm_cosq_dir_t direction, int array_max,
                                  bcm_cosq_buffer_id_t *buf_id_array,
                                  int *array_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_buffer_id_multi_get(unit, gport, cosq, direction,
                                                array_max, buf_id_array, array_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To get enable or disable status of service pool for a given pool id.
 *
 * \param [in]   unit                   Unit number.
 * \param [in]   id                     Service pool id.
 * \param [out]  cosq_service_pool      Get enable or disable status.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_service_pool_get(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t *cosq_service_pool)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_service_pool_get(unit, id, cosq_service_pool));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief To Set enable or disable status of service pool for a given pool id.
 *
 * \param [in]   unit                   Unit number.
 * \param [in]   id                     Service pool id.
 * \param [in]   cosq_service_pool      Enable/Disable given type service pool.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_service_pool_set(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t cosq_service_pool)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_service_pool_set(unit, id, cosq_service_pool));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable or disable service pool override.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   type              Service pool override type.
 * \param [out]  service_pool      Service pool id.
 * \param [out]  enable            Enable or disable service pool override.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_service_pool_override_get(
    int unit,
    bcm_cosq_service_pool_override_type_t type,
    bcm_service_pool_id_t *service_pool,
    int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_service_pool_override_get(unit, type, service_pool, enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Enable or disable service pool override.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   type              Service pool override type.
 * \param [in]   service_pool      Service pool id.
 * \param [in]   enable            Enable or disable service pool override.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_service_pool_override_set(
    int unit,
    bcm_cosq_service_pool_override_type_t type,
    bcm_service_pool_id_t service_pool,
    int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_service_pool_override_set(unit, type, service_pool, enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a profile for the various mappings.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   profile_index     Profile index.
 * \param [in]   type              Mapping type.
 * \param [in]   array_max         Number of mappings allocated in array.
 * \param [out]  arg               Array of input to mapping type.
 * \param [out]  array_count       Number of mappings returned.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_priority_group_mapping_profile_get(
    int unit,
    int profile_index,
    bcm_cosq_priority_group_mapping_profile_type_t type,
    int array_max,
    int *arg,
    int *array_count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_priority_group_mapping_profile_get(unit, profile_index,
                                                               type, array_max,
                                                               arg, array_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set a profile for the following mappings:
 *        Input Pri to priority group UC
 *        Input Pri to priority group MC
 *        Priority Group to Service Pool
 *        Priority Group to Headroom Pool
 *
 * \param [in]   unit              Unit number.
 * \param [in]   profile_index     Profile index.
 * \param [in]   type              Mapping type.
 * \param [in]   array_count       Number of mappings in array.
 * \param [in]   arg               Value of input to mapping type.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_priority_group_mapping_profile_set(
    int unit,
    int profile_index,
    bcm_cosq_priority_group_mapping_profile_type_t type,
    int array_count,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_priority_group_mapping_profile_set(unit, profile_index,
                                                               type, array_count,
                                                               arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup per port per priority group properties.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   port              port number.
 * \param [in]   priority_group_id priority group.
 * \param [in]   type              Port/priority group property.
 * \param [out]  arg               set value of property.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_port_priority_group_property_get(
    int unit,
    bcm_port_t port,
    int priority_group_id,
    bcm_cosq_port_prigroup_control_t type,
    int *arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_priority_group_property_get(unit, port,
                                                             priority_group_id,
                                                             type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Setup per port per priority group properties.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   port              port number.
 * \param [in]   priority_group_id priority group.
 * \param [in]   type              Port/priority group property.
 * \param [in]   arg               set value of property.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_port_priority_group_property_set(
    int unit,
    bcm_port_t port,
    int priority_group_id,
    bcm_cosq_port_prigroup_control_t type,
    int arg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_priority_group_property_set(unit, port,
                                                             priority_group_id,
                                                             type, arg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Calculate and set Headroom Pool limit based on number of lossless classes and packet distribution.
 *
 * \param [in] unit Unit Number.
 * \param [in] hdrm_pool Headroom Pool number.
 * \param [in] num_lossless_class Max Number of lossless classes expected to use Headroom at a given time.
 * \param [in] arr_size Array size of packet distribution array.
 * \param [in] pkt_dist_array Packet distribution array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_cosq_hdrm_pool_limit_set(
    int unit,
    int hdrm_pool,
    int num_lossless_class,
    int arr_size,
    bcm_cosq_pkt_size_dist_t *pkt_dist_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_hdrm_pool_limit_set(unit, hdrm_pool,
                                                num_lossless_class,
                                                arr_size, pkt_dist_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
* \brief Setup Port PriorityGroup headroom value based on cable length and packet distribution.
*
* \param [in] unit Unit Number.
* \param [in] port Port number.
* \param [in] pri_bmp Internal Priority bitmap for which PG headroom needs to be modified.
* \param [in] cable_len Cable length (in meters).
* \param [in] arr_size Array size of packet distribution array.
* \param [in] pkt_dist_array Packet distribution array.
*
* \retval SHR_E_NONE No errors.
* \retval !SHR_E_NONE Failure.
*/
int
bcm_ltsw_cosq_port_prigrp_hdrm_set(
    int unit,
    bcm_port_t port,
    uint32 pri_bmp,
    int cable_len,
    int arr_size,
    bcm_cosq_pkt_size_dist_t *pkt_dist_array)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_port_prigrp_hdrm_set(unit, port, pri_bmp,cable_len,
                                                 arr_size, pkt_dist_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  To get enable/disable status on Rx of packets on the specified CPU cosq.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   cosq              CPU Cosq ID.
 * \param [out]  enable            Enable/Disable.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_cpu_cosq_enable_get(
    int unit,
    bcm_cos_queue_t cosq,
    int *enable)
{
    SHR_FUNC_ENTER(unit);
    COSQ_THD_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_cpu_cosq_enable_get(unit, cosq, enable));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  To enable/disable Rx of packets on the specified CPU cosq.
 *
 * \param [in]   unit              Unit number.
 * \param [in]   cosq              CPU Cosq ID.
 * \param [in]   enable            Enable/Disable.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_cpu_cosq_enable_set(
    int unit,
    bcm_cos_queue_t cosq,
    int enable)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    COSQ_THD_INIT_CHECK(unit);

    COSQ_THD_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_thd_cpu_cosq_enable_set(unit, cosq, enable));

    COSQ_THD_UNLOCK(unit);
    locked = false;

exit:
    if (locked) {
        COSQ_THD_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

