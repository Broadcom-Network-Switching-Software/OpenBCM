/*! \file bfd.c
 *
 * BCM level APIs for BFD.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#if defined(INCLUDE_BFD)

#include <bcm/bfd.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/bfd_int.h>

#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>

#define BSL_LOG_MODULE BSL_LS_BCM_BFD

/*
 * Function:
 *      bcm_ltsw_bfd_detach
 * Purpose:
 *      Shut down the BFD subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_detach(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (BFD_LTSW_INFO(unit)->init == 0) {
        SHR_EXIT();
    }

    BFD_LTSW_LOCK(unit);

    /* Call common detach routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_detach_common(unit));

exit:
    if (BFD_LTSW_INFO(unit)->init) {
        BFD_LTSW_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_init
 * Purpose:
 *      Initialize the BFD subsystem
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SHR_E_NONE Success.
 *      !SHR_E_NONE Failure.
 */
int
bcm_ltsw_bfd_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_detach(unit));

    if (BFD_LTSW_INFO(unit)->lock == NULL) {
        BFD_LTSW_INFO(unit)->lock = sal_mutex_create("bcmLtswBfdMutex");
        SHR_NULL_CHECK(BFD_LTSW_INFO(unit)->lock, SHR_E_MEMORY);
    }

    /* Call common init routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_init_common(unit));

exit:
    if (SHR_FUNC_ERR()) {
        if (BFD_LTSW_INFO(unit)->lock != NULL) {
            sal_mutex_destroy(BFD_LTSW_INFO(unit)->lock);
            BFD_LTSW_INFO(unit)->lock = NULL;
        }
    }
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_endpoint_create
 * Purpose:
 *      Create or update an BFD endpoint object
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint_info - (IN/OUT) Pointer to an BFD endpoint structure.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_endpoint_create(int unit,
                              bcm_bfd_endpoint_info_t *endpoint_info)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    /* Call common routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_create_common(unit, endpoint_info));

exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_endpoint_destroy
 * Purpose:
 *      Destroy an BFD endpoint object.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to destroy.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_endpoint_destroy(int unit,
                              bcm_bfd_endpoint_t endpoint)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    /* Call common routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_destroy_common(unit, endpoint));


exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_endpoint_destroy_all
 * Purpose:
 *      Destroy all BFD endpoint objects.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_endpoint_destroy_all(int unit)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    /* Call common routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_destroy_all_common(unit));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_endpoint_get
 * Purpose:
 *      Get an BFD endpoint object
 * Parameters:
 *      unit          - (IN) Unit number.
 *      endpoint      - (IN) The ID of the endpoint object to get.
 *      endpoint_info - (OUT) Pointer to an BFD endpoint structure
 *                            to receive the data.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_endpoint_get(int unit,
                          bcm_bfd_endpoint_t endpoint,
                          bcm_bfd_endpoint_info_t *endpoint_info)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    /* Call common routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_get_common(unit, endpoint,
                                          endpoint_info));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_endpoint_poll
 * Purpose:
 *      Poll an BFD endpoint object.  Valid only for endpoint in Demand
 *      Mode and in state bcmBFDStateUp.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the BFD endpoint object to poll.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_endpoint_poll(int unit, bcm_bfd_endpoint_t endpoint)
{

    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    /* Call common routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_poll_common(unit, endpoint));

exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_endpoint_stat_get
 * Purpose:
 *      Get BFD endpoint statistics.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get stats for.
 *      ctr_info - (IN/OUT) Pointer to endpoint count structure to receive
 *                 the data.
 *      clear    - (IN) If set, clear stats after read.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_endpoint_stat_get(int unit,
                                bcm_bfd_endpoint_t endpoint,
                                bcm_bfd_endpoint_stat_t *ctr_info, uint32 options)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    /* Call common routine */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_endpoint_stat_get_common(unit, endpoint,
            ctr_info, options));

exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_event_register
 * Purpose:
 *      Register a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback will be invoked.
 *      cb          - (IN) A pointer to the callback function.
 *      user_data   - (IN) Pointer to user data to pass to the callback.

 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_event_register(int unit,
                             bcm_bfd_event_types_t event_types,
                             bcm_bfd_event_cb cb, void *user_data)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_event_register_common(unit,
            event_types, cb, user_data));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_event_unregister
 * Purpose:
 *      Unregister a callback for handling BFD events.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      event_types - (IN) The set of BFD events for which the specified
 *                    callback should not be invoked.
 *      cb          - (IN) A pointer to the callback function.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_event_unregister(int unit,
                               bcm_bfd_event_types_t event_types,
                               bcm_bfd_event_cb cb)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_event_unregister_common(unit,
            event_types, cb));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_tx_start
 * Purpose:
 *      Start all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_tx_start(int unit)
{

    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_tx_set_common(unit, true));

exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_tx_stop
 * Purpose:
 *      Stop all BFD endpoint TX BFD PDUs.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_tx_stop(int unit)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_tx_set_common(unit, false));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_auth_sha1_get
 * Purpose:
 *      Get SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to retrieve.
 *      sha1  - (IN/OUT) Pointer to SHA1 info structure to receive the data.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_auth_sha1_get(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_auth_sha1_get_common(unit, index, sha1));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_auth_sha1_set
 * Purpose:
 *      Set SHA1 authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the SHA1 entry to configure.
 *      sha1  - (IN) Pointer to SHA1 info structure.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_auth_sha1_set(int unit,
                            int index, bcm_bfd_auth_sha1_t *sha1)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_auth_sha1_set_common(unit, index, sha1));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_auth_simple_password_set
 * Purpose:
 *      Set Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to configure.
 *      sp    - (IN) Pointer to Simple Password info structure.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_auth_simple_password_set(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_auth_simple_password_set_common(unit, index, sp));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_auth_simple_password_get
 * Purpose:
 *      Get Simple Password authentication entry.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      index - (IN) Index of the Simple Password entry to retrieve.
 *      sp    - (IN/OUT) Pointer to Simple Password info structure to
 *              receive the data.
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_auth_simple_password_get(int unit,
                                       int index,
                                       bcm_bfd_auth_simple_password_t *sp)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_auth_simple_password_get_common(unit, index, sp));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_discard_stat_get
 * Purpose:
 *      Get the BFD discard statistics
 * Parameters:
 *      unit  - (IN) Unit number.
 *      discarded_info - (OUT) Pointer to discard info
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_discard_stat_get(int unit,
                               bcm_bfd_discard_stat_t *discarded_info)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    BFD_LTSW_PARAM_NULL_CHECK(discarded_info);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_discard_stat_entry_get(unit, discarded_info));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*
 * Function:
 *      bcm_ltsw_bfd_discard_stat_set
 * Purpose:
 *      Reset the BFD discard statistics
 * Parameters:
 *      unit  - (IN) Unit number.
 *      discarded_info - (OUT) Pointer to discard info
 * Returns:
 *      SHR_E_XXX
 */
int
bcm_ltsw_bfd_discard_stat_set(int unit,
                               bcm_bfd_discard_stat_t *discarded_info)
{
    SHR_FUNC_ENTER(unit);

    BFD_LTSW_LOCK(unit);
    BFD_LTSW_INIT_CHECK(unit);

    BFD_LTSW_PARAM_NULL_CHECK(discarded_info);

    SHR_IF_ERR_EXIT
        (bcm_ltsw_bfd_discard_stat_entry_set(unit, discarded_info));
exit:
    BFD_LTSW_UNLOCK(unit);
    SHR_FUNC_EXIT();
}
#endif
