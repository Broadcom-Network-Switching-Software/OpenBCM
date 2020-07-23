/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_msgq.h>

sal_msgq_t
sal_msgq_create(size_t element_size, size_t max_elements, const char *desc)
{
    return NULL;
}

void
sal_msgq_destroy(sal_msgq_t msgq_hdl)
{
}

int
sal_msgq_post(sal_msgq_t msgq_hdl,
              void *element,
              sal_msgq_priority_t pri,
              uint32_t usec)
{
    return -1;
}

int
sal_msgq_recv(sal_msgq_t msgq_hdl, void *element, uint32_t usec)
{
    return -1;
}

size_t
sal_msgq_count_get(sal_msgq_t msgq_hdl)
{
    return 0;
}
