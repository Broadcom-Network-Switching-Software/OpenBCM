/*! \file shr_pb.c
 *
 * Dynamic print buffer support.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_alloc.h>

#include <shr/shr_pb.h>

/* Mainly for debug and test */
static int chunk_size = 0;

shr_pb_t *
shr_pb_create(void)
{
    shr_pb_t *pb;

    pb = sal_alloc(sizeof(shr_pb_t), "shrPbObj");
    if (pb) {
        sal_memset(pb, 0, sizeof(*pb));
        if (chunk_size) {
            pb->chnksz = chunk_size;
            pb->bufsz = pb->chnksz;
        } else {
            pb->bufsz = SHR_PB_CHUNK_SIZE_DEFAULT;
        }
        pb->buf = sal_alloc(pb->bufsz, "shrPbBuf");
        if (pb->buf) {
            /* Start with an empty string */
            *pb->buf = 0;
        } else {
            shr_pb_destroy(pb);
            pb = NULL;
        }
    }
    return pb;
}

void
shr_pb_destroy(shr_pb_t *pb)
{
    if (pb) {
        if (pb->buf) {
            sal_free(pb->buf);
        }
        sal_free(pb);
    }
}

int
shr_pb_printf(shr_pb_t *pb, const char *fmt, ...)
{
    va_list ap;
    char *buf;
    int len;
    int rv;

    if (!pb) {
        return -1;
    }
    if (!pb->buf) {
        return -1;
    }
    if (pb->len >= pb->bufsz) {
        return -1;
    }

    /* Try printf with existing buffer */
    va_start(ap, fmt);
    rv = sal_vsnprintf(&pb->buf[pb->len], pb->bufsz - pb->len, fmt, ap);
    va_end(ap);
    if (rv < 0) {
        return -1;
    }

    /* Check if the entire string fits */
    len = rv + pb->len;
    if (len >= pb->bufsz) {
        /* If not, then allocate a larger buffer */
        do {
            if (chunk_size == 0) {
                /* Exponentially increase chunk size */
                if (pb->chnksz < SHR_PB_CHUNK_SIZE_DEFAULT) {
                    pb->chnksz = SHR_PB_CHUNK_SIZE_DEFAULT;
                }
                if (pb->chnksz < SHR_PB_CHUNK_SIZE_MAX) {
                    pb->chnksz <<= 1;
                }
            }
            pb->bufsz += pb->chnksz;
        } while (len >= pb->bufsz);
        buf = sal_alloc(pb->bufsz, "shrPbBuf");
        if (!buf) {
            return -1;
        }
        /* Move current string to new buffer */
        sal_memcpy(buf, pb->buf, pb->len);
        sal_free(pb->buf);
        pb->buf = buf;
        /* Do printf again with the larger buffer */
        va_start(ap, fmt);
        rv = sal_vsnprintf(&pb->buf[pb->len], pb->bufsz - pb->len, fmt, ap);
        va_end(ap);
        if (rv < 0) {
            return -1;
        }
    }
    pb->len += rv;

    return rv;
}

int
shr_pb_mark(shr_pb_t *pb, int offs)
{
    if (pb) {
        if (offs < 0 || offs > pb->len) {
            offs = pb->len;
        }
        pb->mark = offs;
    }
    return offs;
}

void
shr_pb_reset(shr_pb_t *pb)
{
    if (pb) {
        if (pb->mark <= pb->len) {
            pb->len = pb->mark;
        }
        if (pb->buf) {
            pb->buf[pb->len] = 0;
        }
    }
}

int
shr_pb_replace_last(shr_pb_t *pb, char ch)
{
    if (!pb) {
        return 0;
    }
    if (pb->buf && pb->len) {
        if ((pb->buf[pb->len - 1] = ch) == 0) {
            pb->len--;
        }
    }
    return pb->len;
}

const char *
shr_pb_str(shr_pb_t *pb)
{
    if (!pb || !pb->buf) {
        return SHR_PB_ERR_STR;
    }
    return pb->buf;
}

int
shr_pb_chunk_size(int csize)
{
    if (csize < 0) {
        chunk_size = 0;
    } else if (csize >= SHR_PB_CHUNK_SIZE_MIN && csize <= SHR_PB_CHUNK_SIZE_MAX) {
        chunk_size = csize;
    }
    return chunk_size;
}
