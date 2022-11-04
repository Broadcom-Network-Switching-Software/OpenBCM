

#include <stdint.h>
#include <stdio.h>

#include "kaps_jr1_errors.h"

const char *
kaps_jr1_get_status_string(
    kaps_jr1_status status)
{
    static const char *status_strings[] = {
#define KAPS_JR1_INC_SEL(name,string) string,
#include "kaps_jr1_error_tbl.def"
#undef KAPS_JR1_INC_SEL
        "should never be seen"
    };

    if (((uint32_t) status) >= KAPS_JR1_STATUS_LAST_UNUSED)
        return NULL;

    return status_strings[status];
}
