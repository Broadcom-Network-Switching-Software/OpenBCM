/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Dynamic print buffer support.
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#include <shared/shr_pb.h>

static int chunk_size = SHR_PB_CHUNK_SIZE_DEFAULT;

shr_pb_t *
shr_pb_create(void)
{
    shr_pb_t *pb;

    pb = sal_alloc(sizeof(shr_pb_t), "shrPbObj");
    if (pb) {
        sal_memset(pb, 0, sizeof(*pb));
        pb->bufsz = chunk_size;
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
            pb->bufsz += chunk_size;
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
        chunk_size = SHR_PB_CHUNK_SIZE_DEFAULT;
    } else if (csize >= SHR_PB_CHUNK_SIZE_MIN && csize <= SHR_PB_CHUNK_SIZE_MAX) {
        chunk_size = csize;
    }
    return chunk_size;
}

const char *
shr_pb_format_uint32(shr_pb_t *pb, const char *prefix,
                     const uint32 *val, int count, uint32 bitwidth)
{
    int idx, bit;
    int start = 0, minwidth = 0;
    uint32 datawidth = 0;

    if (pb == NULL || count <= 0) {
        return SHR_PB_ERR_STR;
    }

    if (prefix) {
        shr_pb_printf(pb, "%s", prefix);
    }

    if (bitwidth > 0) {
        /*
         * If the format bit width is specified, get the number of hex-digits
         * for the most significant word to support leading zeros format.
         */
        minwidth = ((bitwidth + 3) / 4) % 8;
        if (minwidth == 0) {
            minwidth = 8;
        }
        start = (bitwidth - 1) / 32;

        /* Check the data bit width of the given array. */
        for (idx = count - 1; idx >= 0; idx--) {
            if (val[idx] == 0) {
                continue;
            }
            if (datawidth == 0) {
                for (bit = 31; bit >= 0; bit--) {
                    if (val[idx] >> bit) {
                        datawidth = bit + 1;
                        break;
                    }
                }
            } else {
                datawidth += 32;
            }
        }

        /* Ignore leading zeros format if data exceeds the format width. */
        if (datawidth > bitwidth) {
            minwidth = 0;
        }
    }

    if (minwidth > 0) {
        uint32 val32;

        /*
         * Left-justify with the leading zeros to
         * match the specified bit width.
         */
        for (idx = start; idx >= 0; idx--) {
            if (idx >= count) {
                val32 = 0;
            } else {
                val32 = val[idx];
            }
            if (idx == start) {
                shr_pb_printf(pb, "0x%.*"SHR_PB_PRIx32, minwidth, val32);
            } else {
                shr_pb_printf(pb, "_%08"SHR_PB_PRIx32, val32);
            }
        }
    } else {
        /*
         * Format values without leading zeros.
         */
        for (start = -1, idx = count - 1; idx >= 0; idx--) {
            if (start < 0) {
                if (idx == 0 && val[idx] < 10) {
                    shr_pb_printf(pb, "%"SHR_PB_PRIu32, *val);
                    return shr_pb_str(pb);
                }
                /* Skip the leading zero elements. */
                if (val[idx] == 0) {
                    continue;
                }
                /* format the first non-zero element. */
                shr_pb_printf(pb, "0x%"SHR_PB_PRIx32, val[idx]);
                start = idx;
                continue;
            }
            shr_pb_printf(pb, "_%08"SHR_PB_PRIx32, val[idx]);
        }
    }

    return shr_pb_str(pb);
}

const char *
shr_pb_format_uint64(shr_pb_t *pb, const char *prefix,
                     const uint64 *val, int count, uint32 bitwidth)
{
    int idx, bit;
    int start = 0, minwidth = 0;
    uint32 datawidth = 0;
    uint64 val64;

    if (pb == NULL || count <= 0) {
        return SHR_PB_ERR_STR;
    }

    if (prefix) {
        shr_pb_printf(pb, "%s", prefix);
    }

    if (bitwidth > 0) {
        /*
         * If the format bit width is specified, get the number of hex-digits
         * for the most significant uint64 to support leading zeros format.
         */
        minwidth = ((bitwidth + 3) / 4) % 16;
        if (minwidth == 0) {
            minwidth = 16;
        }
        start = (bitwidth - 1) / 64;

        /* Check the bit width of the given data array. */
        for (idx = count - 1; idx >= 0; idx--) {
            if (COMPILER_64_IS_ZERO(val[idx])) {
                continue;
            }
            if (datawidth == 0) {
                for (bit = 63; bit >= 0; bit--) {
                    if (COMPILER_64_BITTEST(val[idx], bit)) {
                        datawidth = bit + 1;
                        break;
                    }
                }
            } else {
                datawidth += 64;
            }
        }

        /* Ignore leading zeros format if data exceeds the format width. */
        if (datawidth > bitwidth) {
            minwidth = 0;
        }
    }

    if (minwidth > 0) {

        /*
         * Left-justify with the leading zeros to
         * match the specified bit width.
         */
        for (idx = start; idx >= 0; idx--) {
            if (idx >= count) {
                COMPILER_64_ZERO(val64);
            } else {
                COMPILER_64_SET(val64, COMPILER_64_HI(val[idx]), COMPILER_64_LO(val[idx]));
            }
            if (idx == start) {
                if (minwidth > 8) {
#ifdef COMPILER_HAS_LONGLONG
                    shr_pb_printf(pb, "0x%.*"SHR_PB_PRIx64, minwidth - 8, (unsigned long long)val64 >> 32);
                    shr_pb_printf(pb, "_%08"SHR_PB_PRIx64, (unsigned long long)val64 & 0xffffffff);
#else
                    shr_pb_printf(pb, "0x%.*"SHR_PB_PRIx32, minwidth - 8, COMPILER_64_HI(val64));
                    shr_pb_printf(pb, "_%08"SHR_PB_PRIx32, COMPILER_64_LO(val64));
#endif
                } else {
#ifdef COMPILER_HAS_LONGLONG
                    shr_pb_printf(pb, "0x%.*"SHR_PB_PRIx64, minwidth, (unsigned long long)val64);
#else
                    shr_pb_printf(pb, "0x%.*"SHR_PB_PRIx32, minwidth, COMPILER_64_LO(val64));
#endif
                }
            } else {
#ifdef COMPILER_HAS_LONGLONG
                shr_pb_printf(pb, "_%08"SHR_PB_PRIx64, (unsigned long long)val64 >> 32);
                shr_pb_printf(pb, "_%08"SHR_PB_PRIx64, (unsigned long long)val64 & 0xffffffff);
#else
                shr_pb_printf(pb, "_%08"SHR_PB_PRIx32, COMPILER_64_HI(val64));
                shr_pb_printf(pb, "_%08"SHR_PB_PRIx32, COMPILER_64_LO(val64));
#endif
            }
        }
    } else {
        /*
         * Format values without leading zeros.
         */
        for (start = -1, idx = count - 1; idx >= 0; idx--) {
            if (start < 0) {
                COMPILER_64_SET(val64, 0, 10);
                if (idx == 0 && COMPILER_64_LT(val[idx], val64)) {
#ifdef COMPILER_HAS_LONGLONG
                    shr_pb_printf(pb, "%"SHR_PB_PRIu64, (unsigned long long)val[idx]);
#else
                    shr_pb_printf(pb, "%"SHR_PB_PRIx32, COMPILER_64_LO(val[idx]));
#endif
                    return shr_pb_str(pb);
                }
                /* Skip the leading zero elements. */
                if (COMPILER_64_IS_ZERO(val[idx])) {
                    continue;
                }
                /* format the first non-zero element. */
                COMPILER_64_SET(val64, 0, 0xffffffff);
                if (COMPILER_64_GT(val[idx], val64)) {
#ifdef COMPILER_HAS_LONGLONG
                    shr_pb_printf(pb, "0x%"SHR_PB_PRIx64, (unsigned long long)val[idx] >> 32);
                    shr_pb_printf(pb, "_%08"SHR_PB_PRIx64, (unsigned long long)val[idx] & 0xffffffff);
#else
                    shr_pb_printf(pb, "0x%"SHR_PB_PRIx32, COMPILER_64_HI(val[idx]));
                    shr_pb_printf(pb, "_%08"SHR_PB_PRIx32, COMPILER_64_LO(val[idx]));
#endif
                } else {
#ifdef COMPILER_HAS_LONGLONG
                    shr_pb_printf(pb, "0x%"SHR_PB_PRIx64, (unsigned long long)val[idx]);
#else
                    shr_pb_printf(pb, "0x%"SHR_PB_PRIx32, COMPILER_64_LO(val[idx]));
#endif
                }
                start = idx;
                continue;
            }
#ifdef COMPILER_HAS_LONGLONG
            shr_pb_printf(pb, "_%08"SHR_PB_PRIx64, (unsigned long long)val[idx] >> 32);
            shr_pb_printf(pb, "_%08"SHR_PB_PRIx64, (unsigned long long)val[idx] & 0xffffffff);
#else
            shr_pb_printf(pb, "_%08"SHR_PB_PRIx32, COMPILER_64_HI(val[idx]));
            shr_pb_printf(pb, "_%08"SHR_PB_PRIx32, COMPILER_64_LO(val[idx]));
#endif
        }
    }

    return shr_pb_str(pb);
}

const char *
shr_pb_format_bit_list(shr_pb_t *pb, const SHR_BITDCL *bits, int count)
{
    int idx, start, end;
    char *sep = "";

    /* Mark range as inactive */
    start = end = count;

    SHR_BIT_ITER(bits, count, idx) {
        if (idx != (end + 1)) {
            /* If the range is interrupted, terminate it */
            if (end > start) {
                shr_pb_printf(pb, "-%d", end);
            }
            /* Mark range as inactive */
            start = end = count;
        }
        if (start == count) {
            /* If no active range, start new */
            shr_pb_printf(pb, "%s%d", sep, idx);
            start = idx;
            sep = ",";
        }
        end = idx;
    }
    /* If active range, terminate it */
    if (end > start) {
        shr_pb_printf(pb, "-%d", end);
    }

    return shr_pb_str(pb);
}
