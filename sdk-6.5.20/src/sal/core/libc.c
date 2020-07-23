/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Some routines to avoid dependence on libc in the driver.  Define
 * RTOS_STRINGS if your RTOS already has these routines, because the RTOS
 * implementations may be more efficient and save room.
 *
 * Note: ONLY libc routines needed by library code (drv or bcm) should
 * be here.  This file is not a general dumping-ground for all libc
 * routines needed by diagnostics, etc.
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#ifndef RTOS_STRINGS
/* { */
int 
sal_strlen(
    const char *s
    )
{
    const char *s_orig = s;

    while (*s != 0)
    {
	s++;
    }

    return (s - s_orig);
}

char *
sal_strcpy(
    char *dst,
    const char *src
    )
{
    char *dst_orig = dst;

    while ((*dst++ = *src++) != 0);

    return dst_orig;
}

char *
sal_strncpy(
    char *dst,
    const char *src,
    size_t length
    )
{
  int i = 0;
  char *dst_orig = dst;

  while (i < length && (*dst++ = *src++) != 0)
  {
    ++i;
  }

  return dst_orig;
}


int 
sal_strcmp(
    const char *s1,
    const char *s2
    )
{
    do
    {
	if (*s1 < *s2)
        {
	    return -1;
	}
        else if (*s1 > *s2)
        {
	    return 1;
	}
	s1++;
    } while (*s2++);

    return 0;
}

int
sal_strncmp(
    const char *s1,
    const char *s2,
    size_t length
    )
{
    int i = 0;

    while (i < length && *s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
        i++;
    }
    if ((i == length) || (*s1 == *s2))
    {
        return 0;
    }
    else if (*s1 < *s2)
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

void *
sal_memcpy(
    void *dst_void,
    const void *src_void,
    size_t len
    )
{
    unsigned char *dst = dst_void;
    const unsigned char *src = src_void;

    while (len--)
    {
	*dst++ = *src++;
    }

    return dst_void;
}

void *
sal_memset(
    void *dst_void,
    int val, size_t len
    )
{
    unsigned char *dst = dst_void;

    while (len--)
    {
	*dst++ = (unsigned char) val;
    }

    return dst_void;
}
/*
 * strstr:
 *    Locate the first instance of a substring in a string.
 * string -
 *   String to search.
 * substring -
 *   Substring to try to find in string.
 */
char *
sal_strstr(
    const char *string,             /* String to search. */
    const char *substring           /* Substring to try to find in string. */
    )
{
    const char *aa;
    const char *bb;
    char *ret;

    ret = NULL;
    /*
     * First scan quickly through the two strings looking for a
     * single-character match.  When it's found, then compare the
     * rest of the substring.
     */
    bb = substring;
    if (*bb == 0)
    {
        ret = (char *)string;
        goto exit;
    }
    for ( ; *string != 0; string += 1)
    {
        if (*string != *bb)
        {
            continue;
        }
        aa = string;
        while (1)
        {
            if (*bb == 0)
            {
                ret = (char *)string;
                goto exit;
            }
            if (*aa++ != *bb++)
            {
                break;
            }
        }
        bb = substring;
    }
exit:
    return (ret);
}
/*
 * Concatenate 'src' to the end of 'dst'.  At most strlen(dst)+nn+1 bytes
 * are written at dst (at most nn+1 bytes being appended).  Return 'dst'.
 */
char *
sal_strncat(
    char *dst,
    const char *src,
    size_t nn
    )
{
    if (nn != 0)
    {
        char *dd;
        const char *ss;

        dd = dst;
        ss = src;
        while (*dd != 0)
        {
            dd++;
        }
        do
        {
            if ((*dd = *ss++) == 0)
            {
                break;
            }
            dd++;
        } while (--nn != 0);
        *dd = 0;
    }
    return (dst);
}
/*
 * sal_strchr:
 *   Returns a pointer to the first occurrence of
 *   the character ch in the string pp
 */
char *
sal_strchr(
    const char *pp,
    int ch
    )
{
    char cc;
    char *ret;

    ret = NULL;
    cc = ch;
    for (;; ++pp)
    {
        if (*pp == cc)
        {
            ret = ((char *)pp);
            goto exit;
        }
        if (*pp == '\0')
        {
            goto exit;
        }
    }
exit:
    return (ret);
}
/*
 * sal_strrchr
 *   Find the last occurrence of cc in ss.
 */
char *
sal_strrchr (
    const char *ss,
    int cc
    )
{
    const char *found, *pp;
    char *ret;

    cc = (unsigned char) cc;
    /*
     * Since sal_strchr is fast, we use it rather than the obvious loop.
     */
    if (cc == '\0')
    {
        ret = sal_strchr (ss, '\0');
        goto exit;
    }
    found = NULL;
    while ((pp = sal_strchr(ss, cc)) != NULL)
    {
        found = pp;
        ss = pp + 1;
    }
    ret = ((char *)found);
exit:
    return (ret);
}
/*
 * Return length of initial segment of pp that consists entirely of
 * characters from ss
 */
size_t
sal_strspn(
    const char *pp,
    const char *ss
    )
{
    int ii, jj;

    for (ii = 0; pp[ii]; ii++)
    {
        for (jj = 0; ss[jj]; jj++)
        {
            if (ss[jj] == pp[ii])
            {
                break;
            }
        }
        if (!ss[jj])
        {
            break;
        }
    }
    return (ii);
}
/*
 * Append SRC on the end of DEST.
 */
char *
sal_strcat (
    char *dest,
    const char *src
    )
{
    sal_strcpy (dest + sal_strlen (dest), src);
    return dest;
}
int
sal_islower(
    int c
    )
{
    int ret;

    ret = ((c >= (int)'a') && (c <= (int)'z'));
    return (ret);
}
int
sal_isspace(
    int c
    )
{
    return (c == '\t' || c == '\n' ||
        c == '\v' || c == '\f' || c == '\r' || c == ' ' ? 1 : 0);
}
/* } */
#else
/* { */
#if defined(memcpy)
/* { */
void *sal_memcpy_wrapper(void *dst, const void *src, size_t len)
{
        return memcpy(dst, src, len);
}
/* } */
#endif
/* } */
#endif /* !RTOS_STRINGS */

/*
 * See header of prototype in libc.h
 */
int
sal_strnlen(const char *s, size_t max_len)
{
    size_t ii = 0;

    while (ii < max_len && s[ii]) {
        ii++;
    }

    return ii;
}

/*
 * See header of prototype in libc.h
 */
char *
sal_strncpy_s(char *dst, const char *src, size_t length)
{
    char *ret;
    ret = sal_strncpy(dst,src,length);
    /*
     * Unlike strncpy(), the destination here is ended with NULL regardless
     * of the contents of the source.
     */
    dst[length - 1] = 0;

    return (ret);
}

/**
 * See header of prototype in libc.h
 */
char *
sal_strncat_s(char *dst, const char *src, size_t dest_max_size)
{
    char *ret;
    size_t size;
    ret = NULL;
    /*
     * Always set the last character of 'dst' to NULL
     * (to be used within the following 'strnlen').
     */
    dst[dest_max_size - 1] = 0;
    /*
     * Get number of characters on 'dst' including potential ending
     * NULL (even if it is not there).
     */
    size = sal_strnlen(dst,dest_max_size) + 1;
    if (size >= dest_max_size)
    {
        /*
         * If there is no place to append any character to 'dst' then
         * return with 'NULL'. Note that appending 1 character requires
         * 2 places since we also need to add ending NULL.
         */
        goto exit;
    }
    /*
     * Point to the first caracter to be appended.
     */
    dst = &dst[size - 1];
    /*
     * Copy as many characters as there is place for and set ending NULL.
     */
    sal_strncpy_s(dst,src,dest_max_size - size + 1);
    ret = dst;
exit:
    return (ret);
}

size_t
sal_strlcpy(char *dest, const char *src, size_t cnt)
{
    char *ptr = dest;
    size_t copied = 0;

    while (*src && (cnt > 1)) {
        *ptr++ = *src++;
        cnt--;
        copied++;
    }
    *ptr = '\0';

    return copied;
}

int 
sal_memcmp(const void *m1_void, const void *m2_void, size_t len)
{
    const unsigned char *m1 = m1_void, *m2 = m2_void;

    while (len-- != 0) {
    if (*m1 < *m2) {
        return -1;
    } else if (*m1 > *m2) {
        return 1;
    }
    m1++;
    m2++;
    } 

    return 0;
}

char *
sal_strdup(const char *s)
{
    int len = sal_strlen(s);
    char *rc = sal_alloc(len + 1, "sal_strdup");
    if (rc != NULL) {
        /* coverity[secure_coding] */
    sal_strcpy(rc, s);
    }
    return rc;
}

/*
 * Duplicate a portion of a string given the number of characters in len.
 */
char *
sal_strndup(const char *src, size_t len)
{
  register size_t n   = strlen(src);
  register char  *dst = NULL;

  if (len < n) n = len;

  dst = sal_alloc(n + 1, "sal_strndup");

  if (dst) {
    sal_memcpy(dst, src, n);
    dst[n] = '\0';
  }
  return dst;
}

char *
sal_strtok_r(char *s1, const char *delim, char **s2)
{
    char *ret_str;
    char *p;
    int len = 0;

    if (s1 == NULL) {
        s1 = *s2;
        if (s1 == NULL) {
            return NULL;
        }
    }

    p = s1;
    /* skip initial delimiters */
    while ((*p) && (sal_strchr(delim, *p++) != NULL)) {
       len++;    
    } 

    s1 += len;

    if (*s1 == '\0') {
        return NULL;
    }

    ret_str = s1;

    while (*s1) {
       if (sal_strchr(delim, *s1) != NULL) {
           break;
       }
       s1++;
    }

    if (*s1) {
        *s1++ = '\0';
    }

    if (s2 != NULL) {
        *s2 = s1;
    }

    return ret_str;
}

/*
 * sal_ctoi
 *
 *   Converts a C-style constant to integer.
 *   Also supports '0b' prefix for binary.
 */

int 
sal_ctoi(const char *s, char **end)
{
    unsigned int    n, neg;
    int    base = 10;

    if (s == 0) {
    if (end != 0) {
        end = 0;
    }
    return 0;
    }

    s += (neg = (*s == '-'));

    if (*s == '0') {
    s++;
    if (*s == 'x' || *s == 'X') {
        base = 16;
        s++;
    } else if (*s == 'b' || *s == 'B') {
        base = 2;
        s++;
    } else {
        base = 8;
    }
    }

    for (n = 0; ((*s >= 'a' && *s < 'a' + base - 10) ||
         (*s >= 'A' && *s < 'A' + base - 10) ||
         (*s >= '0' && *s <= '9')); s++) {
    n = n * base + ((*s <= '9' ? *s : *s + 9) & 15);
    }

    if (end != 0) {
    *end = (char *) s;
    }

    return (int) (neg ? -n : n);
}

/*
 * Curt's Printf
 *
 * Reasonably complete subset of ANSI-style printf routines.
 * Needs only sal_strlen and stdarg.
 * Behavior was regressed against Solaris printf(3s) routines (below).
 *
 * Supported format controls:
 *
 *    %%    percent sign
 *    %c    character
 *    %d    integer
 *    %hd    short integer
 *    %ld    long integer
 *    %u    unsigned integer
 *    %o    unsigned octal integer
 *    %x    unsigned hexadecimal integer (lowercase)
 *    %X    unsigned hexadecimal integer (uppercase)
 *    %s    string
 *    %p    pointer
 *    %n    store number of characters output so far
 *    %f    float
 *    %lf    double        (if COMPILER_HAS_DOUBLE is defined)
 *
 * Flag modifiers supported:
 *    Field width, argument field width (*), left justify (-),
 *    zero-fill (0), alternate form (#), always include sign (+),
 *    space before positive numbers (space).
 *
 * Not supported: long long
 *
 * Functions implemented:
 *
 * int sal_vsnprintf(char *buf, size_t bufsize, const char *fmt, va_list ap);
 * int sal_vsprintf(char *buf, const char *fmt, va_list ap);
 * int sal_snprintf(char *buf, size_t bufsize, const char *fmt, ...);
 * int sal_sprintf(char *buf, const char *fmt, ...);
 */

void
sal_ltoa(char *buf,             /* Large enough result buffer   */
     unsigned long num,         /* Number to convert            */
     int base,                  /* Conversion base (2 to 16)    */
     int caps,                  /* Capitalize letter digits     */
     int prec)                  /* Precision (minimum digits)   */
{
    char        tmp[68], *s, *digits;

    digits = (caps ? "0123456789ABCDEF" : "0123456789abcdef");

    s = &tmp[sizeof (tmp) - 1];

    for (*s = 0; num || s == &tmp[sizeof (tmp) - 1]; num /= base, prec--)
        *--s = digits[num % base];

    while (prec-- > 0)
        *--s = '0';
    /* coverity[secure_coding] */
    sal_strcpy(buf, s);
}

void
sal_itoa(char *buf,             /* Large enough result buffer   */
     uint32 num,                /* Number to convert            */
     int base,                  /* Conversion base (2 to 16)    */
     int caps,                  /* Capitalize letter digits     */
     int prec)                  /* Precision (minimum digits)   */
{
    sal_ltoa(buf, num, base, caps, prec);
}

#ifdef COMPILER_HAS_DOUBLE

void
sal_ftoa(char *buf, double f, int decimals)
{
    int            exp = 0;
    unsigned int    int_part;
    double        round;
    int            i;

    if (f < 0.0) {
    *buf++ = '-';
    f = -f;
    }

    for (round = 0.5, i = 0; i < decimals; i++)
    round /= 10.0;

    f += round;

    if (f >= 4294967296.0)
    while (f >= 10.0) {
        f /= 10.0;
        exp++;
    }

    int_part = (unsigned int) f;
    f -= int_part;

    sal_itoa(buf, int_part, 10, 0, 0);
    while (*buf)
    buf++;

    *buf++ = '.';

    for (i = 0; i < decimals; i++) {
    f *= 10.0;
    int_part = (unsigned int) f;
    f -= int_part;
    *buf++ = '0' + int_part;
    }

    if (exp) {
    *buf++ = 'e';
    sal_itoa(buf, exp, 10, 0, 0);
    } else
    *buf = 0;
}

#endif /* COMPILER_HAS_DOUBLE */

#define X_STORE(c) {     \
        if (PTR_TO_INT(bp) < PTR_TO_INT(be))    \
            *bp = (c);                             \
        bp++;                                 \
}

#define X_INF        0x7ffffff0

int sal_vsnprintf(char *buf, size_t bufsize, const char *fmt, va_list ap)
{
    char        c, *bp, *be;
    char                *p_null = NULL;
    char        *b_inf = p_null - 1;

    bp = buf;
    be = (bufsize == X_INF) ? b_inf : &buf[bufsize - 1];

    while ((c = *fmt++) != 0) {
    int         width = 0, ljust = 0, plus = 0, space = 0;
    int        altform = 0, prec = 0, half = 0, base = 0;
    int        tlong = 0, fillz = 0, plen, pad;
    long        num = 0;
    char        tmp[36], *p = tmp;
#ifdef COMPILER_HAS_DOUBLE
    int prec_given = 0;
#endif
#ifdef COMPILER_HAS_LONGLONG
    int tlonglong = 0;
#endif

    if (c != '%') {
        X_STORE(c);
        continue;
    }

	for (c = *fmt++; ; c = *fmt++)
	    switch (c) {
	    case 'h': half = 1;	 	break;
	    case 'l':
#ifdef COMPILER_HAS_LONGLONG
                  tlonglong = tlong && *(fmt - 1) == 'l' ? 1 : 0;
#endif
                  tlong = 1; 	break;
	    case '-': ljust = 1; 	break;
	    case '+': plus = 1; 	break;
	    case ' ': space = 1; 	break;
	    case '0': fillz = 1; 	break;
	    case '#': altform = 1; 	break;
	    case '*': width = -1;	break;	/* Mark as need-to-fetch */
	    case '.':
		if ((c = *fmt++) == '*')
		    prec = -1;			/* Mark as need-to-fetch */
		else {
		    for (prec = 0; c >= '0' && c <= '9'; c = *fmt++)
			prec = prec * 10 + (c - '0');
		    fmt--;
		}
#ifdef COMPILER_HAS_DOUBLE
        prec_given = 1;
#endif
        break;
        default:
        if (c >= '1' && c <= '9') {
            for (width = 0; c >= '0' && c <= '9'; c = *fmt++)
            width = width * 10 + (c - '0');
            fmt--;
        } else
            goto break_for;
        break;
        }
    break_for:

    if (width == -1)
        width = va_arg(ap,int);
    if (prec == -1)
        prec = va_arg(ap,int);

    if (c == 0)
        break;

    switch (c) {
    case 'd':
    case 'i':
        num =
#ifdef COMPILER_HAS_LONGLONG
            tlonglong ? va_arg(ap, long long) :
#endif
            tlong ? va_arg(ap, long) : va_arg(ap, int);
        if (half)
            num = (int) (short) num;
        else if (!tlong)
            num = (int) num;
        /* For zero-fill, the sign must be to the left of the zeroes */
        if (fillz && (num < 0 || plus || space)) {
            X_STORE(num < 0 ? '-' : space ? ' ' : '+');
            if (width > 0)
                width--;
            if (num < 0)
                num = -num;
        }
        if (! fillz) {
            if (num < 0) {
                *p++ = '-';
                num = -num;
            } else if (plus)
                *p++ = '+';
            else if (space)
                *p++ = ' ';
        }
        base = 10;
        break;
    case 'u':
        num =
#ifdef COMPILER_HAS_LONGLONG
            tlonglong ? va_arg(ap, long long) :
#endif
            tlong ? va_arg(ap, long) : va_arg(ap, int);
        if (half)
            num = (int) (unsigned short) num;
        else if (!tlong)
            num = (long) (unsigned int) num;
        base = 10;
        break;
    case 'p':
            *p++ = '0';
            *p++ = 'x';
            tlong = 1;
            altform = 0;
        /* Fall through */
    case 'x':
    case 'X':
        num =
#ifdef COMPILER_HAS_LONGLONG
            tlonglong ? va_arg(ap, long long) :
#endif
            tlong ? va_arg(ap, long) : va_arg(ap, int);
        if (half)
            num = (int) (unsigned short) num;
        else if (!tlong)
            num = (long) (unsigned int) num;
        if (altform) {
            prec += 2;
            *p++ = '0';
            *p++ = c;
        }
        base = 16;
        break;
    case 'o':
    case 'O':
        num =
#ifdef COMPILER_HAS_LONGLONG
            tlonglong ? va_arg(ap, long long) :
#endif
            tlong ? va_arg(ap, long) : va_arg(ap, int);
        if (half)
            num = (int) (unsigned short) num;
        else if (!tlong)
            num = (long) (unsigned int) num;
        if (altform) {
        prec++;
        *p++ = '0';
        }
        base = 8;
        break;
#ifdef COMPILER_HAS_DOUBLE
    case 'f':
        {
        double        f;

        f = va_arg(ap, double);
        if (! prec_given)
            prec = 6;
        sal_ftoa(p, f, prec);
        fillz = 0;
        p = tmp;
        prec = X_INF;
        }
        break;
#endif /* COMPILER_HAS_DOUBLE */
    case 's':
        p = va_arg(ap,char *);
        if (prec == 0)
        prec = X_INF;
        break;
    case 'c':
        p[0] = va_arg(ap,int);
        p[1] = 0;
        prec = 1;
        break;
    case 'n':
        *va_arg(ap,int *) = bp - buf;
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
        sal_ltoa(p, num, base, (c == 'X'), prec);
        if (prec)
        fillz = 0;
        p = tmp;
        prec = X_INF;
    }

	plen = 0;
	if (p && ((plen = sal_strlen(p)) > prec))
	    plen = prec;

    if (width < plen)
        width = plen;

    pad = width - plen;

    while (! ljust && pad-- > 0)
        X_STORE(fillz ? '0' : ' ');
    for (; plen-- > 0 && width-- > 0; p++)
        X_STORE(*p);
    while (pad-- > 0)
        X_STORE(' ');
    }

    if ((be == b_inf) || (bp < be))
    *bp = 0;
    else
    /*    coverity[var_deref_op : FALSE]    */
    *be = 0;

    return (bp - buf);
}

int sal_vsprintf(char *buf, const char *fmt, va_list ap)
{
    return sal_vsnprintf(buf, (size_t) X_INF, fmt, ap);
}

int sal_snprintf(char *buf, size_t bufsize, const char *fmt, ...)
{
    va_list        ap;
    int            r;

    va_start(ap,fmt);
    r = sal_vsnprintf(buf, bufsize, fmt, ap);
    va_end(ap);

    return r;
}

int sal_sprintf(char *buf, const char *fmt, ...)
{
    va_list        ap;
    int            r;

    va_start(ap,fmt);
    r = sal_vsnprintf(buf, (size_t) X_INF, fmt, ap);
    va_end(ap);

    return r;
}

void sal_free_safe(void* ptr)
{
    if(ptr) {
        sal_free(ptr); 
    }   
}
uint32 sal_ceil_func(uint32 numerators , uint32 denominator)
{
    uint32  result;
    if (denominator == 0) {
        return 0xFFFFFFFF;
    }
    result = numerators / denominator;
    if (numerators % denominator != 0) {
        result++;
    }
    return result;
}
uint32 sal_floor_func(uint32 numerators , uint32 denominator)
{
    uint32  result;
    if (denominator == 0) {
        return 0xFFFFFFFF;
    }
    result = numerators / denominator;
    return result;
}
      

static unsigned long next = 1;

int sal_rand(void) {
    next = next * 1103515245 + 12345;

    /* SAL_RAND_MAX assumed to be 32767 */
    return ((unsigned)(next / 65536) % (SAL_RAND_MAX + 1));
}

void sal_srand(unsigned seed) {
    next = seed;
}
