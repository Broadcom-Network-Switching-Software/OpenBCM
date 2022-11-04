



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kaps_jr1_sw_model.h"
#include "kaps_jr1_device.h"
#include "kaps_jr1_device_alg.h"
#include "kaps_jr1_model_xpt.h"



#define KAPS_JR1_SW_MODEL_VERBOSE    (10)


struct xpt_common_fields {
    uint32_t device_type;   
};




kaps_jr1_status kaps_jr1_sw_model_init(
    enum kaps_jr1_device_pair_type pair_type,
    uint32_t flags,
    uint32_t dev_id,
    uint32_t kaps_jr1_sub_type,
    uint32_t profile,
    void **xpt)
{
    struct kaps_jr1_model_xpt *model_xpt;

    if (xpt == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    *xpt = NULL;

    KAPS_JR1_STRY(kaps_jr1_c_model_xpt_init(pair_type, dev_id, kaps_jr1_sub_type,
                    profile, &model_xpt));

    model_xpt->xpt_common.device_type = KAPS_JR1_DEVICE;
    *xpt = model_xpt;
    return KAPS_JR1_OK;
}


kaps_jr1_status kaps_jr1_sw_model_destroy(void *xpt)
{
    struct xpt_common_fields *common_fields = xpt;
    enum kaps_jr1_device_type type;

    if (!xpt)
        return KAPS_JR1_INVALID_ARGUMENT;

    type = (enum kaps_jr1_device_type) common_fields->device_type;

    if (type == KAPS_JR1_DEVICE) {
        KAPS_JR1_STRY(kaps_jr1_c_model_xpt_destroy(xpt));
    }
    return KAPS_JR1_OK;
}



kaps_jr1_status kaps_jr1_sw_model_set_property(void *xpt, enum kaps_jr1_sw_model_property property, ...)
{
    va_list vl;
    kaps_jr1_status status = KAPS_JR1_OK;
    struct xpt_common_fields *common_fields;
    enum kaps_jr1_device_type type;

    if (xpt == NULL)
        return KAPS_JR1_INVALID_ARGUMENT;

    common_fields = xpt;
    type = (enum kaps_jr1_device_type) common_fields->device_type;

    va_start(vl, property);
    switch(property) {
        case KAPS_JR1_SW_MODEL_DUMP: {
            
            FILE *fp = va_arg(vl, FILE *);

            if (type == KAPS_JR1_DEVICE) {
                status = kaps_jr1_c_model_xpt_set_log(xpt, fp);
                break;
            } else {
                status = KAPS_JR1_INVALID_ARGUMENT;
                break;
            }
            break;
        }

        default:
            break;

    }

    va_end(vl);
    return status;
}

