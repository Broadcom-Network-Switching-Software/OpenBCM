/*! \file libc_vsnprintf.c
 *
 * SDK implementation of vsnprintf.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>

#ifndef sal_vsnprintf

/*
 * Reasonably complete subset of ANSI-style printf routines.
 * Needs only strlen and stdarg.
 * Behavior was regressed against Solaris printf(3s) routines (below).
 *
 * Supported format controls:
 *
 *      %%      percent sign
 *      %c      character
 *      %d      integer
 *      %hd     short integer
 *      %ld     long integer
 *      %u      unsigned integer
 *      %o      unsigned octal integer
 *      %x      unsigned hexadecimal integer (lowercase)
 *      %X      unsigned hexadecimal integer (uppercase)
 *      %s      string
 *      %p      pointer
 *      %n      store number of characters output so far
 *
 * Flag modifiers supported:
 *      Field width, argument field width (*), left justify (-),
 *      zero-fill (0), alternate form (#), always include sign (+),
 *      space before positive numbers (space).
 *
 * Functions implemented:
 *
 * int vsnprintf(char *buf, size_t bufsz, const char *fmt, va_list ap);
 * int vsprintf(char *buf, const char *fmt, va_list ap);
 * int snprintf(char *buf, size_t bufsz, const char *fmt, ...);
 * int sprintf(char *buf, const char *fmt, ...);
 *
 * Note that some functions are implemented in separate source files.
 */
static void
_itoa(char *buf,        /* Large enough result buffer   */
      uint32_t num,     /* Number to convert            */
      int base,         /* Conversion base (2 to 16)    */
      int caps,         /* Capitalize letter digits     */
      int prec)         /* Precision (minimum digits)   */
{
    char tmp[36], *s, *digits;

    digits = (caps ? "0123456789ABCDEF" : "0123456789abcdef");

    s = &tmp[sizeof(tmp) - 1];

    for (*s = 0; num || s == &tmp[sizeof(tmp) - 1]; num /= base, prec--) {
        *--s = digits[num % base];
    }

    while (prec-- > 0) {
        *--s = '0';
    }

    sal_strcpy(buf, s);
}

#define X_STORE(c) {    \
    if (bp < be) {      \
        *bp = (c);      \
    }                   \
    bp++;               \
}

#define X_INF           SAL_VSNPRINTF_X_INF

int
sal_vsnprintf(char *buf, size_t bufsz, const char *fmt, va_list ap)
{
    char c, *bp, *be;
    char *p_null = NULL;
    char *b_inf = p_null - 1;

    bp = buf;
    be = (bufsz == X_INF) ? b_inf : &buf[bufsz - 1];

    while ((c = *fmt++) != 0) {
        int width = 0, ljust = 0, plus = 0, space = 0;
        int altform = 0, prec = 0, half = 0, base = 0;
        int tlong = 0, fillz = 0, plen, pad;
        long num = 0;
        char tmp[36], *p = tmp;

        if (c != '%') {
            X_STORE(c);
            continue;
        }

        for (c = *fmt++; ; c = *fmt++)
            switch (c) {
            case 'h':
                half = 1;
                break;
            case 'l':
                tlong = 1;
                break;
            case '-':
                ljust = 1;
                break;
            case '+':
                plus = 1;
                break;
            case ' ':
                space = 1;
                break;
            case '0':
                fillz = 1;
                break;
            case '#':
                altform = 1;
                break;
            case '*':
                /* Mark as need-to-fetch */
                width = -1;
                break;
            case '.':
                if ((c = *fmt++) == '*') {
                    /* Mark as need-to-fetch */
                    prec = -1;
                } else {
                    for (prec = 0; c >= '0' && c <= '9'; c = *fmt++) {
                        prec = prec * 10 + (c - '0');
                    }
                    fmt--;
                }
                break;
            default:
                if (c >= '1' && c <= '9') {
                    for (width = 0; c >= '0' && c <= '9'; c = *fmt++) {
                        width = width * 10 + (c - '0');
                    }
                    fmt--;
                } else {
                    goto break_for;
                }
                break;
            }

      break_for:
        if (width == -1) {
            width = va_arg(ap, int);
        }
        if (prec == -1) {
            prec = va_arg(ap, int);
        }
        if (c == 0) {
            break;
        }

        switch (c) {
        case 'd':
        case 'i':
            num = tlong ? va_arg(ap, long) : va_arg(ap, int);
            if (half) {
                num = (int)(short)num;
            }
            /* For zero-fill, the sign must be to the left of the zeroes */
            if (fillz && (num < 0 || plus || space)) {
                X_STORE(num < 0 ? '-' : space ? ' ' : '+');
                if (width > 0) {
                    width--;
                }
                if (num < 0) {
                    num = -num;
                }
            }
            if (!fillz) {
                if (num < 0) {
                    *p++ = '-';
                    num = -num;
                } else if (plus) {
                    *p++ = '+';
                } else if (space) {
                    *p++ = ' ';
                }
            }
            base = 10;
            break;
        case 'u':
            num = tlong ? va_arg(ap, long) : va_arg(ap, int);
            if (half) {
                num = (int)(short)num;
            }
            base = 10;
            break;
        case 'p':
            altform = 0;
            /* Fall through */
        case 'x':
        case 'X':
            num = tlong ? va_arg(ap, long) : va_arg(ap, int);
            if (half) {
                num = (int)(unsigned short)num;
            }
            if (altform) {
                prec += 2;
                *p++ = '0';
                *p++ = c;
            }
            base = 16;
            break;
        case 'o':
        case 'O':
            num = tlong ? va_arg(ap, long) : va_arg(ap, int);
            if (half) {
                num = (int)(unsigned short)num;
            }
            if (altform) {
                prec++;
                *p++ = '0';
            }
            base = 8;
            break;
        case 's':
            p = va_arg(ap, char *);
            if (prec == 0) {
                prec = X_INF;
            }
            break;
        case 'c':
            p[0] = va_arg(ap, int);
            p[1] = 0;
            prec = 1;
            break;
        case 'n':
            *va_arg(ap, int *) = bp - buf;
            p[0] = 0;
            break;
        case '%':
            p[0] = '%';
            p[1] = 0;
            prec = 1;
            break;
        default:
            X_STORE(c);
            continue;
        }

        if (base != 0) {
            _itoa(p, (unsigned int)num, base, (c == 'X'), prec);
            if (prec) {
                fillz = 0;
            }
            p = tmp;
            prec = X_INF;
        }

        if ((plen = sal_strlen(p)) > prec) {
            plen = prec;
        }

        if (width < plen) {
            width = plen;
        }

        pad = width - plen;

        while (!ljust && pad-- > 0) {
            X_STORE(fillz ? '0' : ' ');
        }
        for (; plen-- > 0 && width-- > 0; p++) {
            X_STORE(*p);
        }
        while (pad-- > 0) {
            X_STORE(' ');
        }
    }

    if ((be == b_inf) || (bp < be)) {
        *bp = 0;
    } else {
        /* coverity[var_deref_op] */
        *be = 0;
    }

    return (bp - buf);
}

#endif
