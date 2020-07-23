/*! \file cosq.c
 *
 * BCM level APIs for cosq.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/cosq.h>
#include <bcm_int/ltsw/cosq_int.h>
#include <bcm_int/ltsw/cosq_thd.h>
#include <bcm_int/ltsw/cosq_stat.h>
#include <bcm_int/ltsw/bst.h>
#include <bcm_int/ltsw/ebst.h>
#include <bcm_int/ltsw/obm.h>
#include <bcm_int/ltsw/pfc.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/mbcm/cosq.h>
#include <bcm_int/ltsw/init.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/ecn.h>
#include <bcm_int/ltsw/oob.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/mod.h>

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_mutex.h>
#include <sal/sal_sem.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_COSQ

/*!
 * \brief COSQ information data structure.
 */
typedef struct cosq_info_s {

    /*! Module is initialized. */
    bool inited;

    /*! Module lock. */
    sal_mutex_t mutex;

} cosq_info_t;

/*
 * Static global variable to hold COSQ info.
 */
static cosq_info_t cosq_info[BCM_MAX_NUM_UNITS] = {{0}};

/* COSQ info. */
#define COSQ_INFO(unit) \
    (&cosq_info[unit])

/* Check if COSQ module is initialized. */
#define COSQ_INIT_CHECK(unit) \
    do { \
        if (COSQ_INFO(unit)->inited == false) { \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT); \
        } \
    } while(0)

/* Take COSQ lock. */
#define COSQ_LOCK(unit) \
    sal_mutex_take(COSQ_INFO(unit)->mutex, SAL_MUTEX_FOREVER)

/* Give COSQ lock. */
#define COSQ_UNLOCK(unit) \
    sal_mutex_give(COSQ_INFO(unit)->mutex)

/******************************************************************************
 * Private functions
 */

/*
 * \brief Check port type.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          PORT ID.
 *
 * \retval TRUE               Port is cpu port.
 * \retval FALSE              Port is non cpu port.
 */
static int
is_cpu_port(int unit, bcm_port_t lport)
{
    return bcmi_ltsw_port_is_type(unit, lport,  BCMI_LTSW_PORT_TYPE_CPU);
}

/*
 * \brief Check port type.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          PORT ID.
 *
 * \retval TRUE               Port is lb port.
 * \retval FALSE              Port is non lb port.
 */
static int
is_lb_port(int unit, bcm_port_t lport)
{
    return bcmi_ltsw_port_is_type(unit, lport,  BCMI_LTSW_PORT_TYPE_LB);
}

/*
 * \brief Get classifier type information.
 *
 * \param [in]  unit           Unit number.
 * \param [in]  classifier_id  Classifier ID.
 * \param [out] classifier     Classifier info.
 *
 * \retval SHR_E_NONE          No errors.
 * \retval !SHR_E_NONE         Failure.
 */
static int
cosq_field_classifier_get(int unit, int classifier_id,
                          bcm_cosq_classifier_t *classifier)
{
    SHR_FUNC_ENTER(unit);

    if (classifier == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    sal_memset(classifier, 0, sizeof(bcm_cosq_classifier_t));

    if (BCMINT_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        classifier->flags |= BCM_COSQ_CLASSIFIER_FIELD;
    } else if (BCMINT_COSQ_CLASSIFIER_IS_FIELD_HI(classifier_id)) {
        classifier->flags |= BCM_COSQ_CLASSIFIER_FIELD |
                             BCM_COSQ_CLASSIFIER_NO_INT_PRI;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public functions
 */

/*
 * \brief Retrieve number UC&MC queue according to multcast queue mode.
 *
 * \param [in]  unit          Unit number.
 * \param [out] num_ucq       unicast queue number.
 * \param [out] num_mcq       multcast queue number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_num_queue_get(int unit, int *num_ucq, int *num_mcq)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_num_queue_get(unit, num_ucq, num_mcq));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get port map information.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  lport         Logic port number.
 * \param [out] info          ltsw_cosq_port_map_info_t.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_port_map_info_get(int unit,
                                 bcm_port_t lport,
                                 bcmi_ltsw_cosq_port_map_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_map_info_get(unit, lport, info));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get device specific TM information.
 *
 * \param [in]   unit         Unit number.
 * \param [out]  info         bcmi_ltsw_cosq_device_info_t.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_device_info_get(int unit, bcmi_ltsw_cosq_device_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_device_info_get(unit, info));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Resolve GPORT if it is a local port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT identifier.
 * \param [out] local_port    local port number.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_localport_resolve(int unit, bcm_gport_t gport,
                                 bcm_port_t *local_port)
{
    bcm_module_t module;
    bcm_port_t port;
    bcm_trunk_t trunk;
    int id, result, rv;

    SHR_FUNC_ENTER(unit);

    if (!BCM_GPORT_IS_SET(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, gport, &port));

        *local_port = port;
        SHR_EXIT();
    }

    rv = bcmi_ltsw_port_gport_resolve(unit, gport, &module, &port, &trunk, &id);
    if (rv == SHR_E_PORT) {
        /* returning E_PARAM when invalid gport is given as param */
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_modid_is_local(unit, module, &result));
    if (result == FALSE) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *local_port = port;

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get port number and queue or scheduler identifier for the specified GPORT .
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT identifier
 * \param [out] port          Port number
 * \param [out] id            Queue or scheduler identifier.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_gport_port_resolve(int unit, bcm_gport_t gport,
                                  bcm_port_t *port, int *id)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_port_resolve(unit, gport, port, id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate input parameters of ingress threshold.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          port ID.
 * \param [in/out]  itm           itm.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_port_itm_validate(int unit, int port, int *itm)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_itm_validate(unit, port, itm));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Displays COS Queue information maintained by software.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
void
bcmi_ltsw_cosq_sw_dump(int unit)
{
    (void)mbcm_ltsw_cosq_sw_dump(unit);
}

/*
 * \brief COS queue bandwidth control bucket setting.
 *        If port is any form of local port, cosq is the hardware queue index.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  gport           Port number or GPORT identifier.
 * \param [in]  cosq            COS queue number.
 * \param [in]  min_quantum     kbps or packet/second.
 * \param [in]  max_quantum     kbps or packet/second.
 * \param [in]  burst_size_auto Enables automatic calculation of burst size.
 * \param [in]  kbits_burst_min Configured size of minimum burst traffic.
 * \param [in]  kbits_burst_max Configured size of maximum burst traffic.
 * \param [in]  flags           flags.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_bucket_set(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                          uint32_t min_quantum, uint32_t max_quantum,
                          bool burst_size_auto,
                          uint32_t kbits_burst_min, uint32_t kbits_burst_max,
                          uint32_t flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bucket_set(unit, gport, cosq, min_quantum,
                                   max_quantum, burst_size_auto,
                                   kbits_burst_min, kbits_burst_max, flags));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get COS queue bandwidth control bucket setting.
 *        If port is any form of local port, cosq is the hardware queue index.
 *
 * \param [in]   unit            Unit number.
 * \param [in]   gport           Port number or GPORT identifier.
 * \param [in]   cosq            COS queue number.
 * \param [out]  min_quantum     kbps or packet/second.
 * \param [out]  max_quantum     kbps or packet/second.
 * \param [out]  burst_size_auto Enables automatic calculation of burst size.
 * \param [out]  kbits_burst_min Configured size of minimum burst traffic.
 * \param [out]  kbits_burst_max Configured size of maximum burst traffic.
 * \param [out]  flags           flags.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_bucket_get(int unit, bcm_port_t gport, bcm_cos_queue_t cosq,
                          uint32_t *min_quantum, uint32_t *max_quantum,
                          bool burst_size_auto,
                          uint32_t *kbits_burst_min, uint32_t *kbits_burst_max,
                          uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_bucket_get(unit, gport, cosq, min_quantum,
                                   max_quantum, burst_size_auto,
                                   kbits_burst_min, kbits_burst_max, flags));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get device-level shaper configuration properties.
 *
 * \param [in]  unit          Unit Number.
 * \param [out] refresh_time  Refresh time. NULL to ignore.
 * \param [out] itu_mode      Itu mode status. NULL to ignore
 * \param [out] shaper        Shaper status. NULL to ignore
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_shaper_config_get(
    int unit,
    bcmi_ltsw_sharper_refresh_time_t *refresh_time,
    int *itu_mode,
    int *shaper)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_shaper_config_get(unit, refresh_time, itu_mode, shaper));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Clear cosq configuration of port.
 *
 * \param [in]  unit        Unit Number.
 * \param [in]  port        Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_port_detach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_detach(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Initialize Cosq for newly added port.
 *
 * \param [in]  unit        Unit Number.
 * \param [in]  port        Logical port ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_port_attach(int unit, bcm_port_t port)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_attach(unit, port));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Enable to use multicast COS from mirroring.
 *
 * \param [in] unit Unit Number.
 * \param [in] enable Enable
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_mc_cos_mirror_set(
    int unit,
    int enable)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_mc_cos_mirror_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Enable to use multicast COS from mirroring.
 *
 * \param [in] unit Unit Number.
 * \param [out] enable Enable status
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_cosq_mc_cos_mirror_get(
    int unit,
    int *enable)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_mc_cos_mirror_get(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set cut-through mode for a specific port.
 *
 * \param [in]  unit        Unit Number.
 * \param [in]  port        Logical port ID.
 * \param [in]  enable      Enable to use cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_ct_mode_set(int unit, int port, int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_ct_mode_set(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get cut-through mode for a specific port.
 *
 * \param [in]  unit        Unit Number.
 * \param [in]  port        Logical port ID.
 * \param [out] enable      Enable to use cut-through mode.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_port_ct_mode_get(int unit, int port, int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(enable, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_ct_mode_get(unit, port, enable));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Dump per-port asf configurations to diag Shell.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  Logical port number.
 * \param [in]  port_speed            Port speed in MB/s.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_port_asf_config_dump(
    int unit,
    bcm_port_t port,
    int port_speed)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_asf_config_dump(unit, port, port_speed));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Print per-port traffic forwarding mode in diag Shell.
 *
 * \param [in]  unit                  Unit number.
 * \param [in]  port                  Logical port number.
 * \param [in]  port_speed            Port speed in MB/s.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_port_asf_config_show(
    int unit,
    bcm_port_t port,
    int port_speed)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_asf_config_show(unit, port, port_speed));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the associated rx channel with a given cosq
 *
 * \param [in] unit Unit Number.
 * \param [in] queue_id input queue id
 * \param [in] chan_id channel index
 * \param [out] enable enable state
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_rx_queue_channel_get(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id,
    bool *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_rx_queue_channel_get(unit, queue_id, chan_id, enable));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Assign a RX channel to a cosq
 *
 * \param [in] unit Unit Number.
 * \param [in] queue_id input queue id
 * \param [in] chan_id channel index
 * \param [in] enable enable state
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_rx_queue_channel_set(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id,
    bool enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_rx_queue_channel_set(unit, queue_id, chan_id, enable));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Tests whether the corresponding COS_BMP bit is set or not
 *
 * \param [in] unit Unit Number.
 * \param [in] queue_id input queue id
 * \param [in] chan_id channel index
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmi_ltsw_rx_queue_channel_set_test(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_rx_queue_channel_set_test(unit, queue_id, chan_id));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get number of COSQs configured for this port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          Local port.
 *
 * \retval Per port cos num.
 */
int
bcmi_ltsw_cosq_port_num_cos(int unit, int port)
{
    bcmi_ltsw_cosq_device_info_t device_info = {0};

    (void)bcmi_ltsw_cosq_device_info_get(unit, &device_info);

    return is_cpu_port(unit, port) ? device_info.num_cpu_queue : \
                                     device_info.num_queue;
}

/*
 * \brief Get Function to return the number of child nodes and child node gport
 *        array for scheduler gport.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  sched_gport   scheduler gport.
 * \param [out] num_uc_child  number of uc child.
 * \param [out] uc_queue      uc queue array.
 * \param [out] num_mc_child  number of mc child.
 * \param [out] mc_queue      mc queue array.
 *
 * \retval Per port cos num.
 */
int
bcmi_ltsw_cosq_sched_node_child_get(int unit, bcm_gport_t sched_gport,
                                    int *num_uc_child, int *uc_queue,
                                    int *num_mc_child, int *mc_queue)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_sched_node_child_get(unit, sched_gport, num_uc_child,
                                             uc_queue, num_mc_child, mc_queue));

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Get the ifp cos map profile id from classifier id.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   classifier_id       Classifier ID.
 * \param [out]  flag                BCM_COSQ_CLASSIFIER_NO_INT_PRI.
 * \param [out]  index               ifp cos map profile id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_classifier_field_get(
    int unit,
    int classifier_id,
    int *flag,
    int *index)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(index, SHR_E_PARAM);

    if (BCMINT_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        *index = BCMINT_COSQ_CLASSIFIER_FIELD_GET(classifier_id);
        if (flag) {
            *flag = 0;
        }
    } else if (BCMINT_COSQ_CLASSIFIER_IS_FIELD_HI(classifier_id)) {
        *index = BCMINT_COSQ_CLASSIFIER_FIELD_HI_GET(classifier_id);
        if (0 == *index % 16) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        if (flag) {
            *flag = BCM_COSQ_CLASSIFIER_NO_INT_PRI;
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*
 * \brief Set the classifier id for give ifp cos map profile id.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   index               ifp cos map profile id.
 * \param [in]   flag                BCM_COSQ_CLASSIFIER_NO_INT_PRI or 0.
 * \param [out]  classifier_id       Classifier ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcmi_ltsw_cosq_classifier_field_set(
    int unit,
    int index,
    int flag,
    int *classifier_id)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(classifier_id, SHR_E_PARAM);

    if (BCM_COSQ_CLASSIFIER_NO_INT_PRI == flag) {
        if (0 == index % 16) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        BCMINT_COSQ_CLASSIFIER_FIELD_HI_SET(*classifier_id, index);
    } else {
        BCMINT_COSQ_CLASSIFIER_FIELD_SET(*classifier_id, index);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize cosq module.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (COSQ_INFO(unit)->inited) {
        SHR_IF_ERR_EXIT
            (bcm_ltsw_cosq_detach(unit));
    }

    if (COSQ_INFO(unit)->mutex == NULL) {
        COSQ_INFO(unit)->mutex = sal_mutex_create("COSQ mutex");
        SHR_NULL_CHECK(COSQ_INFO(unit)->mutex, SHR_E_MEMORY);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_cosq_init(unit));

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_cosq_thd_init(unit),
         SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_cosq_stat_init(unit),
         SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_cosq_bst_init(unit),
         SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_pfc_init(unit),
         SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_obm_init(unit),
         SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_ecn_init(unit),
         SHR_E_UNAVAIL);

    if (ltsw_feature(unit, LTSW_FT_OOB_FC)) {
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmi_ltsw_oob_init(unit),
             SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_cosq_mod_init(unit),
         SHR_E_UNAVAIL);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmi_ltsw_cosq_ebst_init(unit),
         SHR_E_UNAVAIL);

    COSQ_INFO(unit)->inited = true;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach cosq module.
 *
 * \param [in]  unit          Unit number.
 *
 * \retval SHE_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_detach(int unit)
{
    bool locked = false;
    SHR_FUNC_ENTER(unit);

    /* If not initialized, return success. */
    if (COSQ_INFO(unit)->inited == false) {
        SHR_EXIT();
    }

    COSQ_INFO(unit)->inited = false;

    COSQ_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_thd_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_stat_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_bst_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_pfc_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_obm_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_mod_detach(unit));

    if (ltsw_feature(unit, LTSW_FT_OOB_FC)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_oob_detach(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_ecn_deinit(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_ebst_detach(unit));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_detach(unit));

    COSQ_UNLOCK(unit);
    locked = false;

    if (COSQ_INFO(unit)->mutex) {
        sal_mutex_destroy(COSQ_INFO(unit)->mutex);
        COSQ_INFO(unit)->mutex = NULL;
    }

exit:
    if (locked) {
        COSQ_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a cosq gport structure.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          GPORT ID.
 * \param [in]  numq          number of COS queues.
 * \param [in]  flags         flags (BCM_COSQ_GPORT_XXX).
 * \param [out] gport         GPORT identifier.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_add(int unit, bcm_gport_t port, int numq, uint32_t flags,
                        bcm_gport_t *gport)
{
    if (port == BCM_GPORT_INVALID) {
        return SHR_E_PORT;
    }

    /* Return SHR_E_PARAM because of fixed hierarchy gport tree
       is created during init */
    return SHR_E_PARAM;
}

/*!
 * \brief Destroy a cosq gport structure.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT identifier.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_delete(int unit, bcm_gport_t gport)
{
    if (gport == BCM_GPORT_INVALID) {
        return SHR_E_PORT;
    }

    /* Return SHR_E_PARAM because of fixed hierarchy gport tree
       is created during init */
    return SHR_E_PARAM;
}

/*!
 * \brief Get a cosq gport structure.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT identifier.
 * \param [out] port          port number.
 * \param [out] numq          number of COS queues.
 * \param [out] flags         flags (BCM_COSQ_GPORT_XXX).
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_get(int unit, bcm_gport_t gport, bcm_gport_t *port,
                        int *numq, uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_get(unit, gport, port, numq, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the child node GPORT atatched to N-th index (cosq). Only CPU PORT
 *        of the scheduler GPORT.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  in_gport      Scheduler GPORT ID.
 * \param [in]  cosq          COS queue attached to.
 * \param [out] out_gport     child GPORT ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_child_get(int unit, bcm_gport_t in_gport,
                              bcm_cos_queue_t cosq,
                              bcm_gport_t *out_gport)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_child_get(unit, in_gport, cosq, out_gport));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach sched_port to the specified index (cosq) of input_port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  input_gport   GPORT id, that will attach to parent gport.
 * \param [in]  parent_gport  Parent Gport to which current gport will be attached.
 * \param [in]  cosq          COS queue to attach to (-1 for first available cosq.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_attach(int unit, bcm_gport_t input_gport,
                           bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_attach(unit, input_gport, parent_gport, cosq));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach sched_port from the specified index (cosq) of input_port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  input_gport   scheduler GPORT identifier.
 * \param [in]  parent_gport  GPORT to detach from.
 * \param [in]  cosq          COS queue to detach from.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_detach(int unit, bcm_gport_t input_gport,
                           bcm_gport_t parent_gport, bcm_cos_queue_t cosq)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_detach(unit, input_gport, parent_gport, cosq));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get attached status of a scheduler port.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  sched_port    scheduler GPORT identifier.
 * \param [out] input_port    GPORT to attach to.
 * \param [out] cosq          COS queue to attached to.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_attach_get(int unit, bcm_gport_t sched_gport,
                               bcm_gport_t *input_gport, bcm_cos_queue_t *cosq)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_attach_get(unit, sched_gport, input_gport, cosq));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Walks through the valid COSQ GPORTs and calls
 *        the user supplied callback function for each entry.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  trav_fn       Callback function.
 * \param [out] user_data     User data to be passed to callback function.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_traverse(int unit, bcm_cosq_gport_traverse_cb cb,
                             void *user_data)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_traverse(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the COS queue WRED parameters
 *
 * \param [in]  unit          Unit number.
 * \param [in]  flags         BCM_COSQ_DISCARD_*
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_discard_set(int unit, uint32_t flags)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_discard_set(unit, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the COS queue WRED parameters
 *
 * \param [in]  unit          Unit number.
 * \param [out] flags         BCM_COSQ_DISCARD_*
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_discard_get(int unit, uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_discard_get(unit, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the COS queue WRED parameters
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          GPORT ID.
 * \param [in]  cosq          COS queue to configure.
 * \param [in]  discard       Discard settings
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_discard_t *discard)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_discard_set(unit, gport, cosq, discard));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the COS queue WRED parameters
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT ID.
 * \param [in]  cosq          COS queue to configure.
 * \param [out] discard       Discard settings
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                                bcm_cos_queue_t cosq,
                                bcm_cosq_gport_discard_t *discard)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_discard_get(unit, gport, cosq, discard));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an empty rule profile
 *
 * \param [in]  unit          Unit number.
 * \param [out] profile_id    profile index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_discard_profile_create(int unit, int *profile_id)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    COSQ_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_discard_profile_create(unit, profile_id));

    COSQ_UNLOCK(unit);
    locked = false;

exit:
    if (locked) {
        COSQ_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a rule profile
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    profile index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_discard_profile_destroy(int unit, int profile_id)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    COSQ_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_discard_profile_destroy(unit, profile_id));

    COSQ_UNLOCK(unit);
    locked = false;

exit:
    if (locked) {
        COSQ_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update one rule in a profile.
 *        Number of entries per congestion profile.
 *         | SP | Q_MIN | Q_AVG
 *          3  2|   1   |   0
 *        SP : 00 Low congestion
 *             01 Medium congestion
 *             10 High congestion
 *             11: Not valid
 *        Q_MIN and Q_AVG: 0 (no congestion) and 1 (congestion).
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    profile index.
 * \param [in]  rule          discard rule.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_discard_profile_update(int unit, int profile_id,
                                     bcm_cosq_discard_rule_t *rule)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_discard_profile_update(unit, profile_id, rule));


exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief  Get all rules of a profile
 *
 * \param [in]  unit          Unit number.
 * \param [in]  profile_id    profile index.
 * \param [in]  max           rule array size.
 * \param [out] rule_array    rule array.
 * \param [out] rule_count    actual rules count.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_discard_profile_get(int unit,
                                  int profile_id,
                                  int max,
                                  bcm_cosq_discard_rule_t *rule_array,
                                  int *rule_count)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_discard_profile_get(unit, profile_id, max,
                                            rule_array, rule_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set which cosq a given priority should fall into
 *
 * \param [in]  unit          Unit number.
 * \param [in]  priority      Priority value to map.
 * \param [in]  cosq          COS queue to map to.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_mapping_set(int unit, bcm_cos_t priority, bcm_cos_queue_t cosq)
{
    bcm_pbmp_t pbmp;
    bcm_port_t local_port;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    BCM_PBMP_CLEAR(pbmp);

    /*
     * port value of -1 will cause the cosq mapping set for
     * all types of local ports except Loopback ports.
     * So we do the sanity check of cos value with Default Number
     * of COS
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT, &pbmp));

    BCM_PBMP_ITER(pbmp, local_port) {
        if (is_lb_port(unit, local_port)) {
            continue;
        }

        /* If no ETS/gport, map the int prio symmetrically for ucast and
         * mcast
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_cos_mapping_set(unit, local_port,
                                  priority,
                                  BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |
                                  BCM_COSQ_GPORT_MCAST_QUEUE_GROUP,
                                  -1, cosq));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine which COS queue a given priority currently maps to.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  priority      Priority value to map.
 * \param [out] cosq          COS queue number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_mapping_get(int unit, bcm_cos_t priority, bcm_cos_queue_t *cosq)
{
    bcm_port_t local_port;
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    BCM_PBMP_CLEAR(pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_PORT, &pbmp));

    BCM_PBMP_ITER(pbmp, local_port) {
        if (is_lb_port(unit, local_port)) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_cos_mapping_get(unit, local_port, priority,
                                  BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, NULL, cosq));
        break;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set COS queue for the specified priority of an ingress port
 *
 * \param [in]  unit          Unit number.
 * \param [in]  ing_port      ingress port.
 * \param [in]  int_pri      Priority value to map.
 * \param [in]  flags         BCM_COSQ_GPORT_XXX_QUEUE_GROUP.
 * \param [in]  gport         queue group GPORT identifier.
 * \param [in]  cosq          COS queue number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_mapping_set(int unit, bcm_port_t ing_port,
                                bcm_cos_t int_pri, uint32_t flags,
                                bcm_gport_t gport, bcm_cos_queue_t cosq)
{
    bcm_port_t local_port;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, ing_port, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_cos_mapping_set(unit, local_port, int_pri,
                                        flags, gport, cosq));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Determine which COS queue a given priority currently maps to.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  ing_port      ingress port.
 * \param [in]  int_pri       Priority value to map.
 * \param [in]  flags         BCM_COSQ_GPORT_XXX_QUEUE_GROUP.
 * \param [out] gport         queue group GPORT identifier.
 * \param [out] cosq          COS queue number.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_mapping_get(int unit, bcm_port_t ing_port,
                                bcm_cos_t int_pri, uint32_t flags,
                                bcm_gport_t *gport, bcm_cos_queue_t *cosq)
{
    bcm_port_t local_port;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (gport == NULL || cosq == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_localport_resolve(unit, ing_port, &local_port));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_cos_mapping_get(unit, local_port, int_pri,
                                        flags, gport, cosq));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set bandwidth values for given COS policy.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          GPORT ID.
 * \param [in]  cosq          COS queue to configure, -1 for all COS queues.
 * \param [in]  kbits_sec_min minimum bandwidth, kbits/sec.
 * \param [in]  kbits_sec_max maximum bandwidth, kbits/sec.
 * \param [in]  flags         BCM_COSQ_BW_*_PREF.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_bandwidth_set(int unit, bcm_gport_t gport,
                                  bcm_cos_queue_t cosq,
                                  uint32_t kbits_sec_min,
                                  uint32_t kbits_sec_max, uint32_t flags)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (gport == BCM_GPORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }

    if ((kbits_sec_max != 0) && (kbits_sec_max < kbits_sec_min )) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_bucket_set(unit, gport, cosq, kbits_sec_min,
                                   kbits_sec_max,
                                   TRUE, 0, 0, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve bandwidth values for given COS policy.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  port          GPORT ID.
 * \param [in]  cosq          COS queue to configure, -1 for all COS queues.
 * \param [out] kbits_sec_min minimum bandwidth, kbits/sec.
 * \param [out] kbits_sec_max maximum bandwidth, kbits/sec.
 * \param [out] flags         BCM_COSQ_BW_*_PREF.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_bandwidth_get(int unit, bcm_gport_t gport,
                                  bcm_cos_queue_t cosq, uint32_t *kbits_sec_min,
                                  uint32_t *kbits_sec_max, uint32_t *flags)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (gport == BCM_GPORT_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_cosq_bucket_get(unit, gport, cosq, kbits_sec_min,
                                   kbits_sec_max,
                                   TRUE, NULL, NULL, flags));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set up class-of-service policy and corresponding weights and delay.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT ID.
 * \param [in]  cosq          COS queue to configure.
 * \param [in]  mode          Scheduling mode, one of BCM_COSQ_xxx.
 * \param [in]  weight        Weight for the specified COS queue(s)
 *                            Unused if mode is BCM_COSQ_STRICT.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_sched_set(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq, int mode, int weight)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (BCM_GPORT_IS_SET(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_sched_set(unit, gport, cosq, mode, weight));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Retrieve class-of-service policy and corresponding weights and delay.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  gport         GPORT ID.
 * \param [in]  cosq          COS queue to configure.
 * \param [out] mode          Scheduling mode, one of BCM_COSQ_xxx.
 * \param [out] weight        Weight for the specified COS queue(s)
 *                            Unused if mode is BCM_COSQ_STRICT.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_sched_get(int unit, bcm_gport_t gport,
                              bcm_cos_queue_t cosq, int *mode, int *weight)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if ((mode == NULL) ||
        (weight == NULL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (BCM_GPORT_IS_SET(gport)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_sched_get(unit, gport, cosq, mode, weight));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve maximum weights for given COS policy.
 *
 * \param [in]  unit          Unit number.
 * \param [in]  mode          Scheduling mode, one of BCM_COSQ_xxx.
 * \param [out] weight_max    Maximum weight for COS queue.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_sched_weight_max_get(int unit, int mode, int *weight_max)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_cosq_sched_weight_max_get(unit, mode, weight_max));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief cosq schedq mapping set.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  profile_index       Profile index.
 * \param [in]  size                Array size.
 * \param [out] cosq_mapping_arr    Cosq mapping.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_schedq_mapping_set(int unit, int profile_index,
    int size, bcm_cosq_mapping_t *cosq_mapping_arr)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    COSQ_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_cosq_schedq_mapping_set(unit, profile_index, size,
                                          cosq_mapping_arr));
    COSQ_UNLOCK(unit);
    locked = false;

exit:
    if (locked) {
        COSQ_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief cosq schedq mapping get.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  profile_index       Profile index.
 * \param [out] size                Array size.
 * \param [out] cosq_mapping_arr    Cosq mapping.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_schedq_mapping_get(int unit, int profile_index,
    int array_max, bcm_cosq_mapping_t *cosq_mapping_arr, int *size)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_cosq_schedq_mapping_get(unit, profile_index, array_max,
                                          cosq_mapping_arr, size));

exit:
    SHR_FUNC_EXIT();

}

/*!
 * \brief Set Port to Profile Mapping for Cosq profiles.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  port                logical port.
 * \param [in]  profile_type        Profile type.
 * \param [in]  profile_id          profile id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_port_profile_set(int unit, bcm_port_t port,
    bcm_cosq_profile_type_t profile_type, int profile_id)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            if (is_cpu_port(unit, port)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
            }

            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_cosq_port_scheduler_profile_set(unit, port, profile_id));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_pfc_config_profile_id_set(unit, port, profile_id));
            break;
        case bcmCosqProfilePGProperties:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_pg_profile_set(unit, port, profile_id));

            break;
        case bcmCosqProfileIntPriToPGMap:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_inppri_profile_set(unit, port, profile_id));

            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_oob_egr_profile_id_set(unit, port, profile_id));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get Port to Profile Mapping for Cosq profiles.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  port                logical port.
 * \param [in]  profile_type        Profile type.
 * \param [out] profile_id          profile id.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_port_profile_get(int unit, bcm_port_t port,
    bcm_cosq_profile_type_t profile_type, int *profile_id)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &port));

    if (profile_id == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }
    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            if (is_cpu_port(unit, port)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PORT);
            }

            SHR_IF_ERR_VERBOSE_EXIT
               (mbcm_ltsw_cosq_port_scheduler_profile_get(unit, port,
                                                          profile_id));

            break;
        case bcmCosqProfilePGProperties:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_pg_profile_get(unit, port,
                                                        profile_id));

            break;
        case bcmCosqProfileIntPriToPGMap:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_cosq_thd_port_inppri_profile_get(unit, port,
                                                            profile_id));

            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_oob_egr_profile_id_get(unit, port, profile_id));
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Query if a profile can be modified dynamically.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  profile_type        Profile type.
 * \param [out] dynamic             Dynamic changeable type.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_profile_property_dynamic_get(
    int unit,
    bcm_cosq_profile_type_t profile_type,
    bcm_cosq_dynamic_setting_type_t *dynamic)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (dynamic == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            *dynamic = bcmCosqDynamicTypeConditional;
            break;
        case bcmCosqProfilePGProperties:
            *dynamic = bcmCosqDynamicTypeFixed;
            break;
        case bcmCosqProfileIntPriToPGMap:
            *dynamic = bcmCosqDynamicTypeFlexible;
            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            *dynamic = bcmCosqDynamicTypeConditional;
            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Query if a profile id of a profile is in use.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  profile_type        Profile type.
 * \param [in]  profile_id          Profile id.
 * \param [out] in_use              If a profile is in use.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_profile_info_inuse_get(
    int unit,
    bcm_cosq_profile_type_t profile_type,
    int profile_id,
    int *in_use)
{
    bcm_pbmp_t pbmp;
    int port, ret_profile = -1, oob_prf_num, max_num;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (in_use == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_cosq_profile_max_num_get(unit, profile_type, &max_num));

    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            if ((profile_id < 0) ||
                (profile_id >= max_num)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            break;
        case bcmCosqProfilePGProperties:
        case bcmCosqProfileIntPriToPGMap:
            if ((profile_id < 0) ||
                (profile_id >= max_num)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            /* Get profile num. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_oob_egr_profile_num_get(unit, &oob_prf_num));

            if ((profile_id < 0) || (profile_id >= oob_prf_num)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    BCM_PBMP_CLEAR(pbmp);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    BCM_PBMP_ITER(pbmp, port) {
        if (is_cpu_port(unit, port) &&
            profile_type != bcmCosqProfileOobfcEgressQueueMap) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_cosq_port_profile_get(unit, port, profile_type,
                                            &ret_profile));
        if (ret_profile == profile_id) {
            *in_use = 1;
            SHR_EXIT();
        }
    }
    *in_use = 0;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Query the list of ports using a specific profile.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  profile_type        Profile type.
 * \param [in]  profile_id          Profile id.
 * \param [out] port_count          Number of ports using this profile.
 * \param [out] max_port_count      Max Number of ports that can use this profile.
 * \param [out] port_list           List of ports using this profile.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_profile_info_portlist_get(
    int unit,
    bcm_cosq_profile_type_t profile_type,
    int profile_id,
    int max_port_count,
    bcm_port_t *port_list,
    int *port_count)
{
    bcm_pbmp_t pbmp;
    int port, ret_profile = -1, oob_prf_num, max_num;
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (port_count == NULL || port_list == NULL) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (max_port_count <= 0 || max_port_count > bcmi_ltsw_dev_logic_port_num(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_cosq_profile_max_num_get(unit, profile_type, &max_num));

    switch (profile_type) {
        case bcmCosqProfilePFCAndQueueHierarchy:
            if ((profile_id < 0) ||
                (profile_id >= max_num)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            break;
        case bcmCosqProfilePGProperties:
        case bcmCosqProfileIntPriToPGMap:
            if ((profile_id < 0) ||
                (profile_id >= max_num)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            break;
        case bcmCosqProfileOobfcEgressQueueMap:
            /* Get profile num. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_oob_egr_profile_num_get(unit, &oob_prf_num));

            if ((profile_id < 0) || (profile_id >= oob_prf_num)) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }

            break;
        default:
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    BCM_PBMP_CLEAR(pbmp);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    *port_count = 0;
    BCM_PBMP_ITER(pbmp, port) {
        if (is_cpu_port(unit, port) &&
            profile_type != bcmCosqProfileOobfcEgressQueueMap) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_cosq_port_profile_get(unit, port, profile_type,
                                            &ret_profile));
        if (ret_profile == profile_id) {
            port_list[(*port_count)++] = port;
            if((*port_count) == max_port_count) {
                break;
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set up class-of-service policy and corresponding weights and mode.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  mode                Scheduling mode.
 * \param [in]  weight_arr_size     Size of the weights array.
 * \param [in]  weights             Weights for each COS queue.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_sched_config_set(int unit, int mode, int weight_arr_size,
                               int *weights)
{
    bcm_pbmp_t pbmp;

    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_cosq_port_sched_config_set(unit, pbmp, mode, weight_arr_size,
                                             weights));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve class-of-service policy and corresponding weights and mode.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   weight_arr_size     Size of the weights array.
 * \param [out]  mode                Scheduling mode.
 * \param [out]  weights             Weights for each COS queue.
 * \param [out]  weight_arr_count    Actual size of weights arrays.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_sched_config_get(int unit, int weight_arr_size, int *weights,
                               int *weight_arr_count, int *mode)
{
    bcm_pbmp_t pbmp;
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_ALL, &pbmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm_ltsw_cosq_port_sched_config_get(unit, pbmp, weight_arr_size,
                                             weights, weight_arr_count, mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set up class-of-service policy and corresponding weights and mode.
 *
 * \param [in]  unit                Unit number.
 * \param [in]  pbm                 port bitmap.
 * \param [in]  mode                Scheduling mode.
 * \param [in]  weight_arr_size     Size of the weights array.
 * \param [in]  weights             Weights for each COS queue.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_port_sched_config_set(int unit, bcm_pbmp_t pbm, int mode,
                                    int weight_arr_size, int *weights)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_sched_config_set(unit, pbm, mode, weight_arr_size,
                                              weights));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve class-of-service policy and corresponding weights and mode.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   pbm                 port bitmap.
 * \param [in]   weight_arr_size     Size of the weights array.
 * \param [out]  weights             Weights for each COS queue.
 * \param [out]  weight_arr_count    Actual size of weights array.
 * \param [out]  mode                Scheduling mode.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_port_sched_config_get(int unit, bcm_pbmp_t pbm,
                                    int weight_arr_size,
                                    int *weights,
                                    int *weight_arr_count,
                                    int *mode)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_port_sched_config_get(unit, pbm, weight_arr_size,
                                              weights, weight_arr_count, mode));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create a cosq classifier.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   classifier          Classifier attributes.
 * \param [out]  classifier_id       Classifier ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_classifier_create(int unit, bcm_cosq_classifier_t *classifier,
                                int *classifier_id)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (NULL == classifier) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((classifier->flags & BCM_COSQ_CLASSIFIER_FIELD) &&
        (!(classifier->flags & BCM_COSQ_CLASSIFIER_WITH_ID))) {
        if (classifier->flags & BCM_COSQ_CLASSIFIER_NO_INT_PRI) {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_cosq_field_hi_classifier_id_create(unit, classifier_id));
        } else {
            SHR_IF_ERR_VERBOSE_EXIT
                (mbcm_ltsw_cosq_field_classifier_id_create(unit, classifier_id));
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a cosq classifier.
 *
 * \param [in]   unit                Unit number.
 * \param [in]  classifier_id        Classifier ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_classifier_destroy(int unit, int classifier_id)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (BCMINT_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_field_classifier_id_destroy(unit, classifier_id));
    } else if (BCMINT_COSQ_CLASSIFIER_IS_FIELD_HI(classifier_id)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_field_hi_classifier_id_destroy(unit, classifier_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get info about a cosq classifier.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   classifier_id       Classifier ID.
 * \param [out]  classifier          Classifier attributes.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_classifier_get(int unit, int classifier_id,
                             bcm_cosq_classifier_t *classifier)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (BCMINT_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_field_classifier_get(unit, classifier_id, classifier));
    } else if (BCMINT_COSQ_CLASSIFIER_IS_FIELD_HI(classifier_id)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (cosq_field_classifier_get(unit, classifier_id, classifier));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the mapping from port, classifier, and internal priorities
 *        to COS queues in a queue group.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   classifier_id       Classifier ID.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_classifier_mapping_clear(int unit, bcm_gport_t port,
                                       int classifier_id)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (BCMINT_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
         SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_field_classifier_map_clear(unit, classifier_id));
    } else if (BCMINT_COSQ_CLASSIFIER_IS_FIELD_HI(classifier_id)) {
         SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_field_hi_classifier_map_clear(unit, classifier_id));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the mapping from port, classifier, and multiple internal priorities
 *        to multiple COS queues in a queue group.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   classifier_id       Classifier ID.
 * \param [out]  queue_group         Queue group.
 * \param [in]   array_max           Size of priority_array and cosq_array.
 * \param [in]   priority_array      Array of internal priorities.
 * \param [out]   cosq_array         Array of COS queues.
 * \param [out]  array_count         Size of cosq_array.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_classifier_mapping_multi_get(int unit,
                                           bcm_gport_t port,
                                           int classifier_id,
                                           bcm_gport_t *queue_group,
                                           int array_max,
                                           bcm_cos_t *priority_array,
                                           bcm_cos_queue_t *cosq_array,
                                           int *array_count)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if (BCMINT_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
         SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_field_classifier_map_get(unit,
                                                     classifier_id,
                                                     array_max,
                                                     priority_array,
                                                     cosq_array,
                                                     array_count));
    } else if (BCMINT_COSQ_CLASSIFIER_IS_FIELD_HI(classifier_id)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_field_hi_classifier_map_get(unit,
                                                        classifier_id,
                                                        array_max,
                                                        priority_array,
                                                        cosq_array,
                                                        array_count));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the mapping from port, classifier, and multiple internal priorities
 *        to multiple COS queues in a queue group.
 *
 * \param [in]   unit                Unit number.
 * \param [in]   port                GPORT ID.
 * \param [in]   classifier_id       Classifier ID.
 * \param [in]   queue_group         Queue group.
 * \param [in]   array_count         Number of elements in priority_array and cosq_array.
 * \param [in]   priority_array      Array of internal priorities.
 * \param [in]   cosq_array          Array of COS queues.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_classifier_mapping_multi_set(int unit,
                                           bcm_gport_t port,
                                           int classifier_id,
                                           bcm_gport_t queue_group,
                                           int array_count,
                                           bcm_cos_t *priority_array,
                                           bcm_cos_queue_t *cosq_array)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    if ((array_count <= 0) || (NULL == cosq_array)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (BCMINT_COSQ_CLASSIFIER_IS_FIELD(classifier_id)) {
        if (NULL == priority_array) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_field_classifier_map_set(unit,
                                                     classifier_id,
                                                     array_count,
                                                     priority_array,
                                                     cosq_array));
    } else if (BCMINT_COSQ_CLASSIFIER_IS_FIELD_HI(classifier_id)) {
        if (NULL != priority_array) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_cosq_field_hi_classifier_map_set(unit,
                                                        classifier_id,
                                                        array_count,
                                                        priority_array,
                                                        cosq_array));
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Walks through the valid COSQ GPORTs belong to a port and calls
 *        the user supplied callback function for each entry.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   port         port number or GPORT identifier.
 * \param [in]   trav_fn      Callback function.
 * \param [in]   user_data    User data to be passed to callback function.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
int
bcm_ltsw_cosq_gport_traverse_by_port(int unit, bcm_gport_t port,
                                     bcm_cosq_gport_traverse_cb cb,
                                     void *user_data)
{
    SHR_FUNC_ENTER(unit);
    COSQ_INIT_CHECK(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_cosq_gport_traverse_by_port(unit, port, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}
