/*! \file bcmlrd_field_data_get_by_def_type.c
 *
 * Get field data by field definition and data type.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <shr/shr_error.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmlrd/bcmlrd_types.h>
#include <bcmlrd/bcmlrd_internal.h>

/*! Convert a number of bits to a number of (8-bit) bytes. */
#define BITS2BYTES(x)         (((x) + 7) / 8)

/*! Size in bytes of a single field element value (64-bit). */
#define FIELD_SIZE_BYTES      BITS2BYTES(BCM_FIELD_SIZE_BITS)

/*!
 * \brief Get field value from bcmltd_field_data_t based on the dtag.
 *
 * This routine gets the value for a narrow field, scalar or array,
 * based on the field dtag.
 *
 * \param [in] dtag     Dtag.
 * \param [in] fd       Pointer to field data.
 *
 * \retval default/min/max value of the field.
 */
static uint64_t
field_value_by_dtag(bcmltd_field_data_tag_t dtag,
                    const bcmltd_field_data_t *fd)
{
    uint64_t value = 0;

    switch (dtag) {
    case BCMLT_FIELD_DATA_TAG_BOOL:
        value = fd->is_true;
        break;
    case BCMLT_FIELD_DATA_TAG_U8:
        value = fd->u8;
        break;
    case BCMLT_FIELD_DATA_TAG_U16:
        value = fd->u16;
        break;
    case BCMLT_FIELD_DATA_TAG_U32:
        value = fd->u32;
        break;
    case BCMLT_FIELD_DATA_TAG_U64:
        value = fd->u64;
        break;
    default:
        /* This function is for narrow field only */
        break;
    }

    return value;
}

int
bcmlrd_field_data_get_by_def_type(const bcmlrd_field_def_t *fdef,
                                  bcmlrd_field_data_type_t type,
                                  uint32_t num_alloc,
                                  uint64_t *field_data,
                                  uint32_t *num_actual)
{
    const uint32_t field_size_bytes = FIELD_SIZE_BYTES;
    uint64_t val;
    const uint8_t *fbb;
    uint32_t fidx;
    uint32_t max_fidx;
    uint32_t max_bytes;
    uint32_t alloc_bytes;
    uint32_t byte;
    uint32_t i;

    if ((fdef == NULL) || (field_data == NULL) || (num_actual == NULL)) {
        return SHR_E_PARAM;
    }

    *num_actual = 0;
    if (num_alloc == 0) {
        return SHR_E_NONE;
    }

    switch (type) {
    case BCMLRD_FIELD_DATA_TYPE_MIN:
        val = field_value_by_dtag(fdef->dtag, &fdef->min);
        fbb = fdef->min.binary.bytes;
        break;
    case BCMLRD_FIELD_DATA_TYPE_MAX:
        val = field_value_by_dtag(fdef->dtag, &fdef->max);
        fbb = fdef->max.binary.bytes;
        break;
    case BCMLRD_FIELD_DATA_TYPE_DEFAULT:
    default:
        val = field_value_by_dtag(fdef->dtag, &fdef->def);
        fbb = fdef->def.binary.bytes;
        break;
    }

    if (fdef->depth > 0) {
        /* Field array (allowed only for narrow fields) */
        max_fidx = (num_alloc < fdef->depth) ? num_alloc : fdef->depth;
        for (fidx = 0; fidx < max_fidx; fidx++) {
            field_data[fidx] = val;
            (*num_actual)++;
        }
    } else if (fdef->width <= 64) {
        /* Narrow field scalar */
        field_data[0] = val;
        *num_actual = 1;
    } else {
        /* Wide field */

        /* Determine number of bytes to copy */
        max_bytes = BITS2BYTES(fdef->width);
        alloc_bytes = num_alloc * field_size_bytes;
        if (alloc_bytes < max_bytes) {
            max_bytes = alloc_bytes;
        }

        /* Determine number of field indexes to copy */
        max_fidx = (max_bytes + (field_size_bytes - 1)) / field_size_bytes;

        byte = 0;
        for (fidx = 0; fidx < max_fidx; fidx++) {
            field_data[fidx] = 0;

            for (i = 0;
                 (i < field_size_bytes) && (byte < max_bytes);
                 i++, byte++) {
                field_data[fidx] |= ((uint64_t)(fbb[byte] & 0xff)) << (i * 8);
            }

            (*num_actual)++;
        }
    }

    return SHR_E_NONE;
}

