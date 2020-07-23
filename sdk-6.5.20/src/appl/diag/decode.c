/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Routines to Decode ethernet/ip packets and print various amounts of 
 * information.
 */

#ifndef GHS
#ifndef NO_SAL_APPL
#ifndef __KERNEL__ 

#ifdef linux
#undef  _POSIX_SOURCE
#undef  _POSIX_C_SOURCE
#undef _GNU_SOURCE
#undef _SVID_SOURCE
#define _BSD_SOURCE
#endif

#include <sal/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <appl/diag/system.h>
#include <appl/diag/decode.h>

#if defined(BCM_ESW_SUPPORT)
#include <soc/dcbformats.h>
#include <bcm_int/esw/oam.h>
#endif

/*
 * Macro: 	HDR_DECL
 * Purpose:	Used for declaration of a local header pointer. 
 * Parameters:	t - type of header
 *		h - pointer variable to declare
 *		p - pointer to header in packet.
 *
 * 
 * This macro is used as follows:
 *
 * HDR_DECL(struct foo, local_variable, parameter_pointer)
 * 
 * Where a variable "struct foo *local_variable" is declared locally, 
 * and if p is not 4-byte aligned, a local variable of type "struct foo"
 * is also declared, and "local_variable" points to it, otherwise, 
 * local_variable will point  directly into the packet. If a local
 * variable is used to hold the header, the passed in header is 
 * copied into it.
 */
#define	HDR_DECL(_t, _h, _p) 					\
    _t _h##_aligned_structure;					\
    _t *_h = (PTR_TO_INT(_p) & 0x3) ? 				\
        (_t *)memcpy(&_h##_aligned_structure, _p, sizeof(_t))	\
        : (_t *)(_p)

#define	DECODE_LIST(_l)	    (_l) , COUNTOF(_l)

#define	DECODE_LIST_NULL	NULL, 0

typedef struct decode_list_s {
    uint32 		dl_low;		/* Low compare value*/
    uint32		dl_high;	/* Hi compare value */
    struct decode_s	*dl_decode;	/* Next layer decode */
    char		*dl_string;	/* Desciption */
} decode_list_t;

typedef	struct decode_s {
    char		*d_name;	/* Decode "name" ie, IP/Ethernet */
    char  		*(*d_df)(const char *, char *, void *, 
				 int, decode_list_t *, void *); 
    int			(*d_compare)(decode_list_t *, void *);
    int			(*d_length)(decode_list_t *, void *);
    decode_list_t	*d_dl;		/* Decode list this type */
    int			d_dl_cnt;	/* # entires in decode list */
} decode_t;

/************************************************************************
 ************************************************************************
 ************************************************************************
 *									*
 *	    	      Forward declare decode tables			*
 */

static decode_t d_ether_table;		/* Ethernet Decode */
static decode_t d_arp_table;		/* ARP Decode */
static decode_t	d_ip_table;		/* IP Decode */
static decode_t	d_tcp_table;		/* TCP Decode */
static decode_t d_icmp_table;		/* ICMP Decode */
static decode_t d_oam_table;		/* OAM Decode */

#if defined(BCM_ESW_SUPPORT)
static decode_t d_oam_lmm_table;		/* OAM LM Decode */
static decode_t d_oam_lmr_table;		/* OAM LM Decode */
static decode_t d_oam_dmm_table;		/* OAM DM Decode */
static decode_t d_oam_dmr_table;		/* OAM DM Decode */
#endif

static	char	*
d_skip(char *d)
/*
 * Function: 	d_skip
 * Purpose:	Skip passed all chars in current string.
 * Parameters:	d - pointer to string to skip 
 * Returns:	Pointer to \0 in d.
 */
{
    while (*d++)
	;
    return(d - 1);
}

static	char	*
d_strcat(char *d, char *s)
/*
 * Function:	d_strcat
 * Purpose:	Our local strcat that returns a pointer to the end of
 *		the destination string.
 * Parameters:	d - destination
 *		s - source
 * Returns:	Pointer to the end of the destination string (ie. a 
 *		pointer to the '\0'
 */
{
    d = d_skip(d);
    while ('\0' != (*d++ = *s++))
	;
    return(d - 1);
}

static	char	*
d_strnl(char *s, const char *pfx)
/*
 * Function: 	d_strnl
 * Purpose:	Add a newline to the end of a buffer.
 * Parameters:	s - pointer to string formatting into.
 *		pfx - prefix for output.
 * Returns:	Pointer to end of string.
 */
{
    s = d_skip(s);
    s = d_strcat(s, "\n");
    s = d_strcat(s, (char *)pfx);
    return(s);
}

static	char	*
d_vformat_bytes(char *s, unsigned char *d, int l, int brk, char sep)
/*
 * Function:	p_vformat_bytes
 * Pupose:	Format raw hex output.
 * Parameters:	s - pointer to string to format into.
 *		d - pointer to bytes to format
 *		l - length (# bytes to format)
 *		brk - # bytes to format before "sep" character inserted.
 *		sep - character used to break up stream of bytes.
 * Returns:	Pointer to end of string ('\0')
 */
{
    int	i;

    s = d_skip(s);
    for (i = 0; i < l; i++, d++) {
	*s++ = (char)i2xdigit(*d >> 4);
	*s++ = (char)i2xdigit(*d & 0xf);
	if (((i % brk) == (brk - 1)) && (i != (l-1))) {
	    *s++ = sep;
	}
    }
    *s = '\0';
    return(s);
}

static	char	*
d_strcat_dec_cvt(char *s, unsigned int n)
/*
 * Function:	d_strcat_dec_cvt
 * Purpose:	Concat a decimal number into a string, but does not
 *		null terminate.
 * Parameters:	s - pointer to string to concat into.
 *		n - number to format.
 * Returns:	Pointer to char after end of string.
 */
{
    if (10 > n) {			/* Bottom out condition */
	*s++ = (char)(n + '0');
    } else {
	s = d_strcat_dec_cvt(s, n / 10);
	*s++ = (char)((n % 10) + '0');
    }
    return(s);
}

static	char	*
d_strcat_dec(char *s, char *b, unsigned int n, char *a)
/*
 * Function:	d_strcat_dec
 * Purpose:	Concat a decimal number ito a string.
 * Parameters:	s - pointer to string to concat into.
 *		b - string to prepend to number (Before)
 *		n - number to format.
 *		a - string to concat after number (After)
 * Returns:	Pointer to end of string ('\0')
 */
{
    s = d_skip(s);
    if (b) {
	s = d_strcat(s, b);
    }
    if (0 == n) {
	*s++ = '0';
    } else {
	s = d_strcat_dec_cvt(s, n);
    }
    *s = '\0';
    if (a) {
	s = d_strcat(s, a);
    }
    return(s);
}

static	char	*
d_vformat_dec(char *s, unsigned char *d, int l, int brk, char sep)
/*
 * Function:	p_vformat_dev
 * Pupose:	Format raw hex output.
 * Parameters:	s - pointer to string to format into.
 *		d - pointer to bytes to format
 *		l - length (# bytes to format)
 *		brk - # bytes to format before "sep" character inserted.
 *		sep - character used to break up stream of bytes.
 * Returns:	Pointer to end of string ('\0')
 */
{
    int	i;

    for (i = 0; i < l; i++, d++) {
	s = d_strcat_dec(s, NULL, (uint32)*d, NULL);
	if (((i % brk) == (brk -1)) && (i != (l-1))) {
	    *s++ = sep;
	    *s   = '\0';		/* Needed for strcat */
	}
    }
    *s = '\0';
    return(s);
}

static	char	*
d_vformat_hex(char *s, unsigned int x, int digits, int pfx)
/*
 * Function: 	d_vformat_hex
 * Purpose:	Format a number of hex digits into a string.
 * Parameters:	s - pointer to string to format into.
 *		x - number to format
 *		digits - number of hex digits, low order nibbles 
 *			used to form number.
 * Returns:
 */
{
    if (pfx) {
        *s++ = '0';
        *s++ = 'x';
    }
    while (digits--) {
	*s++ = i2xdigit(x >> (digits * 4));
    }
    *s = '\0';
    return(s);
}

static decode_list_t *
d_find_decode(decode_t *decode, void *hdr, int l)
/*
 * Function:	d_find_decode
 * Purpose:	Locate an entry in a decode list corresponding to the 
 *		current header.
 * Parameters:	decode - pointer to decode list.
 *		hdr - pointer to current header.
 * 		l   - length of current header.
 * Returns:	Pointer to decode list entry or NULL if not found.
 */
{
    int	lo, hi, cur;			/* Binary search values */
    decode_list_t *rv = NULL;

    COMPILER_REFERENCE(l);

    /* Perform simple binary search on packet */

    if (decode->d_dl) {			/* If no list, give up now */
	lo   = 0;
	hi = decode->d_dl_cnt - 1;

	do {
	    cur = lo + (hi - lo) / 2;
	    switch(decode->d_compare(&decode->d_dl[cur], hdr)) {
	    case -1:			/* low */
		hi = cur - 1;
		break;
	    case 0:			/* found */
		rv = &decode->d_dl[cur];
		break;
	    case 1:			/* high */
		lo = cur + 1;
		break;
	    }
	} while (!rv && ((hi - lo) > 0));

	/* Check if we have just landed on it */ 
	if (!rv && (0 == decode->d_compare(&decode->d_dl[lo], hdr))) {
	    rv = &decode->d_dl[lo];
	}	    
    }
    return(rv);
}

static char *
d_decode(const char *pfx, char *s, decode_t *decode, void *hdr, int l, void *pd)
/*
 * Function: 	d_decode
 * Purpose:	Decode multiple levels of a packet.
 * Parameters:	pfx - prefix string attached to each line.
 *		s - pointer to string to format into.
 *		decode - pointer to decode entry to start from
 *		hdr - pointer to shere to start i packet.
 *		l - # bytes i packet valid starting at hdr.
 * Returns:	Pointer to end of string.
 */
{
    decode_list_t	*dl;
    int			hl;		/* header length */

    if (decode->d_name) {
	s = d_strnl(s, pfx);
	s = d_strcat(s, "  ");
	s = d_strcat(s, decode->d_name);
	s = d_strcat(s, ": ");
    }
    dl = d_find_decode(decode, hdr, l); /* Find this level decode */
    if (decode->d_df) {
	/*
	 * May or may not be further decode function at a given 
	 * level.
	 */
	s = decode->d_df(pfx, s, hdr, l, dl, pd);
    }
    if (dl) {
	hl = decode->d_length(dl, hdr);
	hdr = (void *)((char *)hdr + hl);
	l -= hl;			/* Remove our header */
	if (dl->dl_decode) {		/* Next level if there */
	    s = d_decode(pfx, s, dl->dl_decode, hdr, l, pd);
	}
    } else {				/* end of the line */
	if (!decode->d_df) {
	s = d_strcat(s, ": ");
	s = d_vformat_bytes(s, hdr, 20, 1, ' ');
    }
    }
    return(s);
}

/************************************************************************
 ************************************************************************
 ************************************************************************
 *									*
 *			       	Ethernet 				*
 */

#include <soc/enet.h>

static	int
d_ether_compare(decode_list_t *dl, void *hdr)
/*
 * Function:	d_ether_compare
 * Purpose:    	Compare routine for ethernet packet type.
 * Parameters:	dl - pointer to entry to compare.
 *		hdr - pointer to ethernet header to compare.
 * Returns:	-1 - hdr LESS than hdr
 *		0  - hdr EQUAL hdr
 *		1  - hdr GREATER than hdr
 * Notes:	If LEN < 0x600, it is an 802.3 packet, otherwise
 *		type field indicates packet type. The 802.3 "entry" is 
 *		the first entry so we make sure we zoom in on it.
 */
{
    enet_hdr_t *eh = (enet_hdr_t *)hdr;
    uint16	t;

    if (ENET_TAGGED(eh)) {
	t = bcm_ntohs(eh->en_tag_len);
    } else {
	t = bcm_ntohs(eh->en_untagged_len);
    }
	
    if (dl->dl_low > t) {
	return(-1);
    } else if (dl->dl_high < t) {
	return(1);
    } else {
	return(0);
    }
}

char *
d_ether_decode(const char *pfx, char *s, void *ehp, int el, decode_list_t *dl, void *pd)
/*
 * Function:	p_ether_decode
 * Purpose: 	Format ethernet header into string.
 * Parameters:	pfx - prefix for output.
 *		s - pointer to string to format into
 *		ehp - ethernet header
 *		el - length of packet starting at eh
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
    enet_hdr_t *eh = (enet_hdr_t *)ehp;

    COMPILER_REFERENCE(pfx);
    COMPILER_REFERENCE(pd);

    /*
     * Make a best guess at if the header is a valid length, if it
     * is less that min length (UNTAGGED) or if it is less that
     * a calculated length it is in error. This is not perfect, but it is
     * close enough for now.
     */
    if ((el < ENET_UNTAGGED_HDR_LEN) ||
	(el < ENET_HDR_LEN(eh))) {
	s = d_strcat_dec(s, "ETHER (***INVALID LENGTH ", el, "***):");
	s = d_vformat_bytes(s, (unsigned char *)eh, el, 1, ' ');
	return(s);
    }

    s = d_strcat(s, "dst<");
    s = d_vformat_bytes(s,(unsigned char *)&eh->en_dhost, 
			sizeof(eh->en_dhost), 1, ':');
    s = d_strcat(s, "> src<");
    s = d_vformat_bytes(s,(unsigned char *)&eh->en_shost, 
			sizeof(eh->en_shost), 1, ':');
    s = d_strcat(s, "> ");

    if (ENET_SNAP(eh)) {
	s = d_strcat(s, "SNAP Packet ");
    } else if (ENET_TAGGED(eh)) {
	s = d_strcat(s, "Tagged Packet ");
    } else {
	s = d_strcat(s, "Untagged Packet ");
    }

    if (ENET_SNAP(eh) || (ENET_TAGGED(eh))) {
	s = d_strcat(s, "ProtID<");
	s = d_vformat_hex(s, bcm_ntohs(eh->en_tag_tpid), 4, 1);
	s = d_strcat(s, "> Ctrl<");
	s = d_vformat_hex(s, bcm_ntohs(eh->en_tag_ctrl), 4, 1);
	s = d_strcat(s, "> ");
    } else if (bcm_ntohs(eh->en_untagged_len) < 0x600) {
	s = d_strcat_dec(s, "Length<",bcm_ntohs(eh->en_untagged_len),"-bytes> ");
    }
   
    if (dl) {
	s = d_strcat(s, dl->dl_string);
    } else {
	s = d_strcat(s, "* Unknown/Experimental format * ");
	s = d_vformat_bytes(s, (unsigned char *)eh, 64 > el ? el : 64, 1, 
			    ' ');
    }

    return(s);
}

/*ARGSUSED*/
static	int
d_ether_length(decode_list_t *dl, void *ehp)
/*
 * Function:	d_ether_length
 * Purpose:	Determine the length of the ethernet header.
 * Parameters:	dl - pointer to decode list entry.
 *		ehp - pointer to ethernet header.
 * Returns:	Integer length.
 * Notes:	For tagged packets, the ethernet header is larger.
 */
{
    enet_hdr_t	*eh = (enet_hdr_t *)ehp;

    COMPILER_REFERENCE(dl);
    return(ENET_HDR_LEN(eh));
}

static decode_list_t	d_ether_type[] = {
    {0x0000,	0x0600,	&d_ip_table,"802.3 Packet"},
    {0x0800,	0x0800,	&d_ip_table,"Internet Protocol (IP)"},
    {0x0801,	0x0801,	NULL,	"X.75 Internet"},
    {0x0802,	0x0802,	NULL,	"NBS Internet"},
    {0x0803,	0x0803,	NULL,	"ECMA Internet"},
    {0x0804,	0x0804,	NULL,	"CHAOSnet"},
    {0x0805,	0x0805,	NULL,	"X.25 Level 3"},
    {0x0806,	0x0806,	&d_arp_table,	"ARP (IP and CHAOS)"},
    {0x0807,	0x0807,	NULL,	"XNS Compatibility"},
    {0x081C,	0x081C,	NULL,	"Symbolics Private"},
    {0x0888,	0x088a,	NULL,	"Xyplex"},
    {0x0900,	0x0900,	NULL,	"Ungermann-Bass network debugger"},
    {0x0A00,	0x0A00,	NULL,	"Xerox IEEE802.3 PUP"},
    {0x0A01,	0x0A01,	NULL,	"Xerox IEEE802.3 PUP Address Translation"},
    {0x0BAD,	0x0BAD,	NULL,	"Banyan Systems"},
    {0x0BAF,	0x0BAF,	NULL,	"Banyon VINES Echo"},
    {0x1000,	0x100f,	NULL,	"Berkeley Trailer negotiation"},
    {0x1234,	0x1234,	NULL,	"DCA - Multicast"},
    {0x1600,	0x1600,	NULL,	"VALID system protocol"},
    {0x1989,	0x1989,	NULL,	"Artificial Horizons (\"Aviator\" dogfight "
	                        "simulator [on Sun])"},
    {0x1995,	0x1995,	NULL,	"Datapoint Corporation (RCL lan protocol)"},
    {0x3C00,	0x3C00,	NULL,	"3Com NBP virtual circuit datagram (like "
	                        "XNS SPP) not registered"},
    {0x3C01,	0x3C01,	NULL,	"3Com NBP System control datagram not "
	                        "registered"},
    {0x3C02,	0x3C02,	NULL,	"3Com NBP Connect request (virtual cct) "
	                        "not registered"},
    {0x3C03,	0x3C03,	NULL,	"3Com NBP Connect response not registered"},
    {0x3C04,	0x3C04,	NULL,	"3Com NBP Connect complete not registered"},
    {0x3C05,	0x3C05,	NULL,	"3Com NBP Close request (virtual cct) "
	                        "not registered"},
    {0x3C06,	0x3C06,	NULL,	"3Com NBP Close response not registered"},
    {0x3C07,	0x3C07,	NULL,	"3Com NBP Datagram (like XNS IDP) not "
	                        "registered"},
    {0x3C08,	0x3C08,	NULL,	"3Com NBP Datagram broadcast not "
	                        "registered"},
    {0x3C09,	0x3C09,	NULL,	"3Com NBP Claim NetBIOS name not "
	                        "registered"},
    {0x3C0A,	0x3C0A,	NULL,	"3Com NBP Delete Netbios name not "
	                        "registered"},
    {0x3C0B,	0x3C0B,	NULL,	"3Com NBP Remote adaptor status request "
	                        "not registered"},
    {0x3C0C,	0x3C0C,	NULL,	"3Com NBP Remote adaptor response not "
	                        "registered"},
    {0x3C0D,	0x3C0D,	NULL,	"3Com NBP Reset not registered"},
    {0x4242,	0x4242,	NULL,	"PCS Basic Block Protocol"},
    {0x424C,	0x424C,	NULL,	"Information Modes Little Big LAN "
	                        "diagnostic"},
    {0x4321,	0x4321,	NULL,	"THD - Diddle"},
    {0x4C42,	0x4C42,	NULL,	"Information Modes Little Big LAN"},
    {0x5208,	0x5208,	NULL,	"BBN Simnet Private"},
    {0x6000,	0x6000,	NULL,	"DEC unassigned, experimental"},
    {0x6001,	0x6001,	NULL,	"DEC Maintenance Operation Protocol (MOP) "
	                        "Dump/Load Assistance"},
    {0x6002,	0x6002,	NULL,	"DEC Maintenance Operation Protocol (MOP) "
	                        "Remote Console"},
    {0x6003,	0x6003,	NULL,	"DECNET Phase IV, DNA Routing"},
    {0x6004,	0x6004,	NULL,	"DEC Local Area Transport (LAT)"},
    {0x6005,	0x6005,	NULL,	"DEC diagnostic protocol (at interface "
	                        "initialization)"},
    {0x6006,	0x6006,	NULL,	"DEC customer protocol"},
    {0x6007,	0x6007,	NULL,	"DEC Local Area VAX Cluster,System "
	                        "Communication Architecture"},
    {0x6008,	0x6008,	NULL,	"DEC AMBER"},
    {0x6009,	0x6009,	NULL,	"DEC MUMPS"},
    {0x6010,	0x6014,	NULL,	"3Com Corporation"},
    {0x7000,	0x7000,	NULL,	"Ungermann-Bass download"},
    {0x7001,	0x7001,	NULL,	"Ungermann-Bass NIUs"},
    {0x7002,	0x7002,	NULL,	"Ungermann-Bass diagnostic/loopback"},
    {0x7003,	0x7003,	NULL,	"Ungermann-Bass (NMC to/from UB Bridge)"},
    {0x7005,	0x7005,	NULL,	"Ungermann-Bass Bridge Spanning Tree"},
    {0x7007,	0x7007,	NULL,	"OS/9 Microware"},
    {0x7009,	0x7009,	NULL,	"OS/9 Net"},
    {0x7020,	0x7029,	NULL,	"LRT (England) (now Sintrom)"},
    {0x7030,	0x7030,	NULL,	"Racal-Interlan"},
    {0x7031,	0x7031,	NULL,	"Prime NTS (Network Terminal Service)"},
    {0x7034,	0x7034,	NULL,	"Cabletron"},
    {0x8003,	0x8003,	NULL,	"Cronus VLN"},
    {0x8004,	0x8004,	NULL,	"Cronus Direct"},
    {0x8005,	0x8005,	NULL,	"HP Probe protocol"},
    {0x8006,	0x8006,	NULL,	"Nestar"},
    {0x8008,	0x8008,	NULL,	"AT&T/Stanford University"},
    {0x8010,	0x8010,	NULL,	"Excelan"},
    {0x8013,	0x8013,	NULL,	"Silicon Graphics diagnostic"},
    {0x8014,	0x8014,	NULL,	"Silicon Graphics network games"},
    {0x8015,	0x8015,	NULL,	"Silicon Graphics reserved"},
    {0x8016,	0x8016,	NULL,	"Silicon Graphics XNS NameServer, bounce " 
	                        "server"},
    {0x8019,	0x8019,	NULL,	"Apollo DOMAIN"},
    {0x802E,	0x802E,	NULL,	"Tymshare"},
    {0x802F,	0x802F,	NULL,	"Tigan, Inc."},
    {0x8035,	0x8035,	NULL,	"Reverse Address Resolution Protocol "
	                        "(RARP)"},
    {0x8036,	0x8036,	NULL,	"Aeonic Systems"},
    {0x8037,	0x8037,	NULL,	"IPX (Novell Netware)"},
    {0x8038,	0x8038,	NULL,	"DEC LanBridge Management"},
    {0x8039,	0x8039,	NULL,	"DEC DSM/DDP"},
    {0x803A,	0x803A,	NULL,	"DEC Argonaut Console"},
    {0x803B,	0x803B,	NULL,	"DEC VAXELN"},
    {0x803C,	0x803C,	NULL,	"DEC DNS Naming Service"},
    {0x803D,	0x803D,	NULL,	"DEC Ethernet CSMA/CD Encryption Protocol"},
    {0x803E,	0x803E,	NULL,	"DEC Distributed Time Service"},
    {0x803F,	0x803F,	NULL,	"DEC LAN Traffic Monitor Protocol"},
    {0x8040,	0x8040,	NULL,	"DEC PATHWORKS DECnet NETBIOS Emulation"},
    {0x8041,	0x8041,	NULL,	"DEC Local Area System Transport"},
    {0x8042,	0x8042,	NULL,	"DEC unassigned"},
    {0x8044,	0x8044,	NULL,	"Planning Research Corp."},
    {0x8046,	0x8046,	NULL,	"AT&T"},
    {0x8047,	0x8047,	NULL,	"AT&T"},
    {0x8048,	0x8048,	NULL,	"DEC Availability Manager for Distributed "
	                        "Systems DECamds"},
    {0x8049,	0x8049,	NULL,	"ExperData"},
    {0x805B,	0x805B,	NULL,	"VMTP (Versatile Message Transaction "
	                        "Protocol,RFC-1045) (Stanford)"},
    {0x805C,	0x805C,	NULL,	"Stanford V Kernel, version 6.0"},
    {0x805D,	0x805D,	NULL,	"Evans & Sutherland"},
    {0x8060,	0x8060,	NULL,	"Little Machines"},
    {0x8062,	0x8062,	NULL,	"Counterpoint Computers"},
    {0x8065,	0x8065,	NULL,	"University of Mass. at Amherst"},
    {0x8066,	0x8066,	NULL,	"University of Mass. at Amherst"},
    {0x8067,	0x8067,	NULL,	"Veeco Integrated Automation"},
    {0x8068,	0x8068,	NULL,	"General Dynamics"},
    {0x8069,	0x8069,	NULL,	"AT&T"},
    {0x806A,	0x806A,	NULL,	"Autophon"},
    {0x806C,	0x806C,	NULL,	"ComDesign"},
    {0x806D,	0x806D,	NULL,	"Compugraphic Corporation"},
    {0x806E,	0x8077,	NULL,	"Landmark Graphics Corporation"},
    {0x807A,	0x807A,	NULL,	"Matra"},
    {0x807B,	0x807B,	NULL,	"Dansk Data Elektronik"},
    {0x807C,	0x807C,	NULL,	"Merit Internodal (or Univ of Michigan)"},
    {0x807D,	0x807f,	NULL,	"Vitalink Communications"},
    {0x8080,	0x8080,	NULL,	"Vitalink TransLAN III Management"},
    {0x8081,	0x8083,	NULL,	"Counterpoint Computers"},
    {0x8088,	0x808a,	NULL,	"Xyplex"},
    {0x809B,	0x809B,	NULL,	"EtherTalk (AppleTalk over Ethernet)"},
    {0x809C,	0x809C,	NULL,	"Datability"},
    {0x809D,	0x809D,	NULL,	"Datability"},
    {0x809E,	0x809E,	NULL,	"Datability"},
    {0x809F,	0x809F,	NULL,	"Spider Systems Ltd."},
    {0x80A3,	0x80A3,	NULL,	"Nixdorf Computers"},
    {0x80A4,	0x80b3,	NULL,	"Siemens Gammasonics Inc."},
    {0x80C0,	0x80c3,	NULL,	"DCA (Digital Comm. Assoc.) Data Exchange "
	                        "Cluster"},
    {0x80C6,	0x80C6,	NULL,	"Pacer Software"},
    {0x80C7,	0x80C7,	NULL,	"Applitek Corporation"},
    {0x80C8,	0x80cc,	NULL,	"Intergraph Corporation"},
    {0x80CD,	0x80CD,	NULL,	"Harris Corporation"},
    {0x80CE,	0x80CE,	NULL,	"Harris Corporation"},
    {0x80CF,	0x80d2,	NULL,	"Taylor Instrument"},
    {0x80D3,	0x80D4,	NULL,	"Rosemount Corporation"},
    {0x80D5,	0x80D5,	NULL,	"IBM SNA Services over Ethernet"},
    {0x80DD,	0x80DD,	NULL,	"Varian Associates"},
    {0x80DE,	0x80DE,	NULL,	"TRFS (Integrated Solutions Transparent "
	                        "Remote File System)"},
    {0x80DF,	0x80DF,	NULL,	"TRFS (Integrated Solutions Transparent "
	                        "Remote File System)"},
    {0x80E0,	0x80E3,	NULL,	"Allen-Bradley"},
    {0x80E4,	0x80ed,	NULL,	"Datability"},
    {0x80EF,	0x80f0,	NULL,	"Datability"},
    {0x80F2,	0x80F2,	NULL,	"Retix"},
    {0x80F3,	0x80F3,	NULL,	"AppleTalk Address Resolution Protocol "
	                        "(AARP)"},
    {0x80F4,	0x80F4,	NULL,	"Kinetics"},
    {0x80F5,	0x80F5,	NULL,	"Kinetics"},
    {0x80F7,	0x80F7,	NULL,	"Apollo Computer"},
    {0x80FF,	0x8101,	NULL,	"Wellfleet Communications"},
    {0x8102,	0x8102,	NULL,	"Wellfleet; BOFL (Breath OF Life) pkts "
	                        "[every 5-10 secs.]"},
    {0x8103,	0x8103,	NULL,	"Wellfleet Communications"},
    {0x8107,	0x8109,	NULL,	"Symbolics Private"},
    {0x812B,	0x812B,	NULL,	"Talaris"},
    {0x8130,	0x8130,	NULL,	"Waterloo Microsystems Inc."},
    {0x8131,	0x8131,	NULL,	"VG Laboratory Systems"},
    {0x8137,	0x8137,	NULL,	"Novell (old) NetWare IPX (ECONFIG E "
	                        "option)"},
    {0x8138,	0x8138,	NULL,	"Novell, Inc."},
    {0x8139,	0x8139,	NULL,	"KTI"},
    {0x813a,	0x813a,	NULL,	"KTI"},
    {0x813b,	0x813b,	NULL,	"KTI"},
    {0x813c,	0x813c,	NULL,	"KTI"},
    {0x813d,	0x813d,	NULL,	"KTI"},
    {0x813F,	0x813F,	NULL,	"M/MUMPS data sharing"},
    {0x8145,	0x8147,	NULL,	"Vrije Universiteit (NL"},
    {0x814C,	0x814C,	NULL,	"SNMP over Ethernet (see RFC1089)"},
    {0x814F,	0x814F,	NULL,	"Technically Elite Concept"},
    {0x8191,	0x8191,	NULL,	"PowerLA"},
    {0x817D,	0x817D,	NULL,	"XTP"},
    {0x81D6,	0x81D6,	NULL,	"Artisoft Lantastic"},
    {0x81D7,	0x81D7,	NULL,	"Artisoft Lantastic"},
    {0x8203,	0x8205,	NULL,	"QNX Software Systems Ltd."},
    {0x8390,	0x8390,	NULL,	"Accton Technologies (unregistered)"},
    {0x852B,	0x852B,	NULL,	"Talaris multicast"},
    {0x8582,	0x8582,	NULL,	"Kalpana"},
    {0x86DD,	0x86DD,	NULL,	"IP version 6"},
    {0x8739,	0x8739,	NULL,	"Control Technology Inc"},
    {0x873A,	0x873A,	NULL,	"Control Technology Inc"},
    {0x873B,	0x873B,	NULL,	"Control Technology Inc"},
    {0x873C,	0x873C,	NULL,	"Control Technology Inc"},
    {0x8820,	0x8820,	NULL,	"Hitachi Cable (Optoelectronic Systems "
	                        "Laboratory)"},
    {0x8856,	0x8856,	NULL,	"Axis Communications AB"},
    {0x8888,	0x8888,	NULL,	"HP LanProbe test"},
    {0x8902,	0x8902,	&d_oam_table,"OAM"},
    {0x9000,	0x9000,	NULL,	"Loopback (Configuration Test Protocol)"},
    {0x9001,	0x9001,	NULL,	"3Com (Formerly Bridge Communications), "
	                        "XNS Systems Management"},
    {0x9002,	0x9002,	NULL,	"3Com (Formerly Bridge Communications), "
	                        "TCP/IP Systems Management"},
    {0x9003,	0x9003,	NULL,	"3Com (Formerly Bridge Communications), "
	                        "loopback detection"},
    {0xAAAA,	0xAAAA,	NULL,	"DECNET"},
    {0xFAF5,	0xFAF5,	NULL,	"Sonix Arpeggio"},
    {0xFF00,	0xFF00,	NULL,	"BBN VITAL-LanBridge cache wakeups"},
};
/*
 * Ethernet decode table.
 */
static decode_t d_ether_table = {
    "Ethernet",
    d_ether_decode, 
    d_ether_compare,
    d_ether_length, 
    DECODE_LIST(d_ether_type)
};

/************************************************************************
 ************************************************************************
 ************************************************************************
 *									*
 *			       	ARP	 				*
 */
/* From <net/if_arp.h> which does not exist on all systems */
struct arphdr {
    uint16 ar_hrd;
    uint16 ar_pro;
    uint8 ar_hln;
    uint8 ar_pln;
    uint8 ar_op;
};

static	int
d_arp_compare(decode_list_t *dl, void *ahp)
/*
 * Function:	d_arp_compare
 * Purpose:    	Compare routine for arp packet type.
 * Parameters:	dl - pointer to entry to compare.
 *		hdr - pointer to arp header to compare.
 * Returns:	-1 - hdr LESS than dl
 *		0  - hdr EQUAL dl
 *		1  - hdr GREATER than dl
 */
{
    HDR_DECL(struct arphdr, ah, ahp);

    if (dl->dl_low > bcm_ntohs(ah->ar_op)) {
	return(-1);
    } else if (dl->dl_high < bcm_ntohs(ah->ar_op)) {
	return(1);
    } else {
	return(0);
    }
}

char *
d_arp_decode(const char *pfx, char *s, void *ahp, int el, decode_list_t *dl, void *pd)
/*
 * Function:	p_arp_decode
 * Purpose: 	Format arp header into string.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		ahp - arp header
 *		el - length of packet starting at ahp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
    HDR_DECL(struct arphdr, ah, ahp);
    unsigned char *aptr = (unsigned char *)ahp + sizeof(struct arphdr);

    COMPILER_REFERENCE(pfx);
    COMPILER_REFERENCE(pd);

    if (el < (int)sizeof(struct arphdr)) {
	s = d_strcat(s, "ARP (***INVALID***):");
	s = d_vformat_bytes(s, (unsigned char *)ah, el, 1, ' ');
	return(s);
    }

    s = d_strcat(s, dl->dl_string);
    s = d_strcat_dec(s, " fmt<", bcm_ntohs(ah->ar_hrd), "> src-ha<");
    s = d_vformat_bytes(s, aptr, ah->ar_hln, 1, ':');
    aptr += ah->ar_hln;
    s = d_strcat(s, "> src_pa<");
    s = d_vformat_dec(s, aptr, ah->ar_pln, 1, ':');
    aptr += ah->ar_pln;
    s = d_strcat(s, "> tar_ha<");
    s = d_vformat_bytes(s, aptr, ah->ar_hln, 1, ':');
    aptr += ah->ar_hln;
    s = d_strcat(s, "> tar_pa<");
    s = d_vformat_dec(s, aptr, ah->ar_pln, 1, ':');
    aptr += ah->ar_pln;
    s = d_strcat(s, ">");
    
    return(s);
}

static	int
d_arp_length(decode_list_t *dl, void *ahp)
/*
 * Function:	d_arp_length
 * Purpose:	Determine the length of the arp header.
 * Parameters:	dl - pointer to decode list entry.
 *		ehp - pointer to arp header.
 * Returns:	Integer length.
 */
{
    HDR_DECL(struct arphdr, ah, ahp);

    COMPILER_REFERENCE(dl);

    return(sizeof(struct arphdr) + ah->ar_hln * 2 + ah->ar_pln * 2);
}

static decode_list_t	d_arp_type[] = {
     {0, 0, 	NULL, 	"Invalid (0)"},
     {1, 1, 	NULL, 	"Request [RFC826]"},
     {2, 2,  	NULL,	"Reply [RFC826]"},
     {3, 3,  	NULL,	"Request Reverse (RARP) [RFC903]"},
     {4, 4,  	NULL,	"Reply Reverse (RARP) [RFC903]"},
     {5, 5,  	NULL,	"DRARP-Request [David Brownell]"},
     {6, 6,  	NULL,	"DRARP-Reply [David Brownell]"},
     {7, 7,  	NULL,	"DRARP-Error [David Brownell]"},
     {8, 8,  	NULL,	"InARP-Request [RFC1293]"},
     {9, 9,  	NULL,	"InARP-Reply [RFC1293]"},
    {10, 10,  	NULL,	"ARP-NAK [RFC1577]"},
    {11, 11,  	NULL,	"MARS-Request [Armitage]"},
    {12, 12,  	NULL,	"MARS-Multi [Armitage]"},
    {13, 13,  	NULL,	"MARS-MServ [Armitage]"},
    {14, 14,  	NULL,	"MARS-Join [Armitage]"},
    {15, 15,  	NULL,	"MARS-Leave [Armitage]"},
    {16, 16,  	NULL,	"MARS-NAK [Armitage]"},
    {17, 17,  	NULL,	"MARS-Unserv [Armitage]"},
    {18, 18,  	NULL,	"MARS-SJoin [Armitage]"},
    {19, 19,  	NULL,	"MARS-SLeave [Armitage]"},
    {20, 20,  	NULL,	"MARS-Grouplist-Request [Armitage]"},
    {21, 21,  	NULL,	"MARS-Grouplist-Reply [Armitage]"},
    {22, 22,  	NULL,	"MARS-Redirect-Map [Armitage]"},
    {23, 23,  	NULL,	"MAPOS-UNARP [Maruyama]"},
    {24, 0xffff,NULL, 	"Undefined"}
};
    
static decode_t d_arp_table = {
    "ARP",
    d_arp_decode, 
    d_arp_compare,
    d_arp_length, 
    DECODE_LIST(d_arp_type)
};

#if defined(BCM_ESW_SUPPORT)
static	int
d_oam_compare(decode_list_t *dl, void *ahp)
/*
 * Function:	d_arp_compare
 * Purpose:    	Compare routine for arp packet type.
 * Parameters:	dl - pointer to entry to compare.
 *		hdr - pointer to arp header to compare.
 * Returns:	-1 - hdr LESS than dl
 *		0  - hdr EQUAL dl
 *		1  - hdr GREATER than dl
 */
{
    HDR_DECL(oam_hdr_t, ah, ahp);

    if (dl->dl_low > ah->opcode) {
	return(-1);
    } else if (dl->dl_high < ah->opcode) {
	return(1);
    } else {
	return(0);
    }
}

char *
d_oam_decode(const char *pfx, char *s, void *ahp, int el, decode_list_t *dl, void *pd)
/*
 * Function:	p_arp_decode
 * Purpose: 	Format arp header into string.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		ahp - arp header
 *		el - length of packet starting at ahp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
    HDR_DECL(oam_hdr_t, ah, ahp);

    COMPILER_REFERENCE(pfx);
    COMPILER_REFERENCE(pd);

    if (el < (int)sizeof(oam_hdr_t)) {
	s = d_strcat(s, "OAM (***INVALID***):");
	s = d_vformat_bytes(s, (unsigned char *)ah, el, 1, ' ');
	return(s);
    }

    s = d_strcat(s, dl->dl_string);
    s = d_strcat_dec(s, " mdl<", (ah->mdl_ver >> 5), ">");
    s = d_strcat_dec(s, " opcode<", ah->opcode, ">");
    s = d_strcat_dec(s, " flag<", ah->flags, ">");
    s = d_strcat_dec(s, " first_tlvoffset<", ah->first_tlvoffset, ">");
    
    return(s);
}

static	int
d_oam_length(decode_list_t *dl, void *ahp)
/*
 * Function:	d_arp_length
 * Purpose:	Determine the length of the arp header.
 * Parameters:	dl - pointer to decode list entry.
 *		ehp - pointer to arp header.
 * Returns:	Integer length.
 */
{
    COMPILER_REFERENCE(ahp);
    COMPILER_REFERENCE(dl);

    return(sizeof(oam_hdr_t));
}

char *
d_oam_lmr_decode(const char *pfx, char *s, void *ahp, int el, decode_list_t *dl, void *pd)
/*
 * Function:	p_arp_decode
 * Purpose: 	Format arp header into string.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		ahp - oam header
 *		el - length of packet starting at ahp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
#ifdef BCM_ENDURO_SUPPORT
    dcb20_t *pdcb;
#endif
    HDR_DECL(oam_lm_pkt_t, ah, ahp);

    COMPILER_REFERENCE(pfx);

    if (el < (int)sizeof(oam_lm_pkt_t)) {
        s = d_strcat(s, "LMR (***INVALID***):");
        s = d_vformat_bytes(s, (unsigned char *)ah, el, 1, ' ');
        return(s);
    }
    
    s = d_strcat(s, "TxFCf:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->txfcf), 8, 1);
    s = d_strcat(s, "> RxFCf:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->rxfcf), 8, 1);
    s = d_strcat(s, "> TxFCb:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->txfcb), 8, 1);
    s = d_strcat(s, ">");
#ifdef BCM_ENDURO_SUPPORT    
    if (pd) {
        pdcb = (dcb20_t *)pd;
        s = d_strcat(s, " RxFCb:<");
        s = d_vformat_hex(s, pdcb->timestamp, 8, 1);
        s = d_strcat(s, ">");
    }
#endif    
    return(s);
}

char *
d_oam_lmm_decode(const char *pfx, char *s, void *ahp, int el, decode_list_t *dl, void *pd)
/*
 * Function:	p_arp_decode
 * Purpose: 	Format arp header into string.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		ahp - oam header
 *		el - length of packet starting at ahp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
#ifdef BCM_ENDURO_SUPPORT
    dcb20_t *pdcb;
#endif
    HDR_DECL(oam_lm_pkt_t, ah, ahp);

    COMPILER_REFERENCE(pfx);

    if (el < (int)sizeof(oam_lm_pkt_t)) {
        s = d_strcat(s, "LMM (***INVALID***):");
        s = d_vformat_bytes(s, (unsigned char *)ah, el, 1, ' ');
        return(s);
    }
    
    s = d_strcat(s, "TxFCf:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->txfcf), 8, 1);
    s = d_strcat(s, "> RxFCf:<");
#ifdef BCM_ENDURO_SUPPORT
    if (pd) {
        pdcb = (dcb20_t *)pd;
        s = d_vformat_hex(s, pdcb->timestamp, 8, 1);
    } else 
#endif
    {
        s = d_vformat_hex(s, bcm_ntohl(ah->rxfcf), 8, 1);
    }
    s = d_strcat(s, "> TxFCb:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->txfcb), 8, 1);
    s = d_strcat(s, ">");
   
    return(s);
}

char *
d_oam_dmr_decode(const char *pfx, char *s, void *ahp, int el, decode_list_t *dl, void *pd)
/*
 * Function:	p_arp_decode
 * Purpose: 	Format arp header into string.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		ahp - oam header
 *		el - length of packet starting at ahp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
#ifdef BCM_ENDURO_SUPPORT
    dcb20_t *pdcb;
#endif
    HDR_DECL(oam_dm_pkt_t, ah, ahp);

    COMPILER_REFERENCE(pfx);

    if (el < (int)sizeof(oam_dm_pkt_t)) {
        s = d_strcat(s, "DMR (***INVALID***):");
        s = d_vformat_bytes(s, (unsigned char *)ah, el, 1, ' ');
        return(s);
    }
    s = d_strcat(s, "TxTSf:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->txtsf_upper), 8, 1);
    s = d_vformat_hex(s, bcm_ntohl(ah->txtsf), 8, 0);
    s = d_strcat(s, "> RxTSf:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->rxtsf_upper), 8, 1);
    s = d_vformat_hex(s, bcm_ntohl(ah->rxtsf), 8, 0);
    s = d_strcat(s, "> TxTSb:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->txtsb_upper), 8, 1);
    s = d_vformat_hex(s, bcm_ntohl(ah->txtsb), 8, 0);
    s = d_strcat(s, "> RxTSb:<");
#ifdef BCM_ENDURO_SUPPORT
    if (pd) {
        pdcb = (dcb20_t *)pd;
        s = d_vformat_hex(s, pdcb->timestamp_upper, 8, 1);
        s = d_vformat_hex(s, pdcb->timestamp, 8, 0);
    } else 
#endif
    {
        s = d_vformat_hex(s, bcm_ntohl(ah->rxtsb_upper), 8, 1);
        s = d_vformat_hex(s, bcm_ntohl(ah->rxtsb), 8, 0);
    }
    s = d_strcat(s, ">");
    
    ahp = (void *)((char *)ahp + sizeof(oam_dm_pkt_t));
    return(s);
}

char *
d_oam_dmm_decode(const char *pfx, char *s, void *ahp, int el, decode_list_t *dl, void *pd)
/*
 * Function:	p_arp_decode
 * Purpose: 	Format arp header into string.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		ahp - oam header
 *		el - length of packet starting at ahp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
#ifdef BCM_ENDURO_SUPPORT
    dcb20_t *pdcb;
#endif
    HDR_DECL(oam_dm_pkt_t, ah, ahp);

    COMPILER_REFERENCE(pfx);

    if (el < (int)sizeof(oam_dm_pkt_t)) {
        s = d_strcat(s, "DMM (***INVALID***):");
        s = d_vformat_bytes(s, (unsigned char *)ah, el, 1, ' ');
        return(s);
    }
    s = d_strcat(s, "TxTSf:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->txtsf_upper), 8, 1);
    s = d_vformat_hex(s, bcm_ntohl(ah->txtsf), 8, 0);
    s = d_strcat(s, "> RxTSf:<");
#ifdef BCM_ENDURO_SUPPORT
    if (pd) {
        pdcb = (dcb20_t *)pd;
        s = d_vformat_hex(s, pdcb->timestamp_upper, 8, 1);
        s = d_vformat_hex(s, pdcb->timestamp, 8, 0);
    } else 
#endif
    {
        s = d_vformat_hex(s, bcm_ntohl(ah->rxtsf_upper), 8, 1);
        s = d_vformat_hex(s, bcm_ntohl(ah->rxtsf), 8, 0);
    }
    s = d_strcat(s, "> TxTSb:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->txtsb_upper), 8, 1);
    s = d_vformat_hex(s, bcm_ntohl(ah->txtsb), 8, 0);
    s = d_strcat(s, "> RxTSb:<");
    s = d_vformat_hex(s, bcm_ntohl(ah->rxtsb_upper), 8, 1);
    s = d_vformat_hex(s, bcm_ntohl(ah->rxtsb), 8, 0);
    s = d_strcat(s, ">");
    
    ahp = (void *)((char *)ahp + sizeof(oam_dm_pkt_t));
    return(s);
}

static decode_list_t	d_oam_type[] = {
    {0,  41, 	NULL, 	"Undefined"},
    {42, 42, &d_oam_lmr_table, "LMR"},
    {43, 43, &d_oam_lmm_table, "LMM"},
    {46, 46, &d_oam_dmr_table, "DMR"},
    {47, 47, &d_oam_dmm_table, "DMM"},
    {48, 0xff,  NULL,   "Undefined"}
};

static decode_t d_oam_table = {
    "OAM",
    d_oam_decode, 
    d_oam_compare,
    d_oam_length, 
    DECODE_LIST(d_oam_type)
};

static decode_t d_oam_lmr_table = {	/* OAM LMR*/
    "LMR", 
    d_oam_lmr_decode,
    NULL,
    NULL, 
    DECODE_LIST_NULL
};

static decode_t d_oam_lmm_table = {	/* OAM LMM */
    "LMM", 
    d_oam_lmm_decode,
    NULL,
    NULL, 
    DECODE_LIST_NULL
};

static decode_t d_oam_dmr_table = { /* OAM DMR */
    "DMR", 
    d_oam_dmr_decode, 
    NULL,
    NULL, 
    DECODE_LIST_NULL
};

static decode_t d_oam_dmm_table = { /* OAM DMM */
    "DMM", 
    d_oam_dmm_decode, 
    NULL,
    NULL, 
    DECODE_LIST_NULL
};
#endif

/************************************************************************
 ************************************************************************
 ************************************************************************
 *									*
 *			       	IP	 				*
 */
struct ip {
#if defined(LE_HOST)
    unsigned int ip_hl:4;
    unsigned int ip_v:4;
#else /* !LE_HOST */
    unsigned int ip_v:4;
    unsigned int ip_hl:4;
#endif
    uint8    ip_tos;
    uint16   ip_len;
    uint16   ip_id;
    uint16   ip_off;
    uint8    ip_ttl;
    uint8    ip_p;
    uint16   ip_sum;
    struct in_addr ip_src;
    struct in_addr ip_dst;
};

static	int
d_ip_compare(decode_list_t *dl, void *ipp)
/*
 * Function:	d_arp_compare
 * Purpose:    	Compare routine for arp packet type.
 * Parameters:	dl - pointer to entry to compare.
 *		ipp - pointer to ip header to compare.
 * Returns:	-1 - hdr LESS than dl
 *		0  - hdr EQUAL dl
 *		1  - hdr GREATER than dl
 * Notes:	Comparing byte field so no alignment required.
 */
{
    struct ip *iph = (struct ip *)ipp;

    if (dl->dl_low > iph->ip_p) {
	return(-1);
    } else if (dl->dl_high < iph->ip_p) {
	return(1);
    } else {
	return(0);
    }
}

char *
d_ip_decode(const char *pfx, char *s, void *ihp, int il, decode_list_t *dl, void *pd)
/*
 * Function:	p_ip_decode
 * Purpose: 	Format ip header into string.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		ihp - ip header
 *		il - length of packet starting at ihp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
    HDR_DECL(struct ip, ih, ihp);

    COMPILER_REFERENCE(pfx);
    COMPILER_REFERENCE(dl);
    COMPILER_REFERENCE(pd);

    if (il < (int)sizeof(struct ip)) {
	s = d_strcat(s, "(***INVALID***):");
	s = d_vformat_bytes(s, (unsigned char *)ih, il, 1, ' ');
	return(s);
    }

    s = d_strcat_dec(s, "V(", ih->ip_v, ") src<");
    s = d_vformat_dec(s, (unsigned char *)&ih->ip_src, sizeof(ih->ip_src), 
		      1, '.');
    s = d_strcat(s, "> dst<");
    s = d_vformat_dec(s, (unsigned char *)&ih->ip_dst, sizeof(ih->ip_dst), 
		      1, '.');
    s = d_strcat_dec(s, "> hl<", ih->ip_hl, "> ");
    s = d_strcat_dec(s, "service-type<", ih->ip_tos, "> ");
    s = d_strcat_dec(s, "tl<", bcm_ntohs(ih->ip_len), "> ");
    s = d_strcat_dec(s, "id<", bcm_ntohs(ih->ip_id), "> ");
    s = d_strcat_dec(s, "frg-off<", bcm_ntohs(ih->ip_off), "> ");
    s = d_strcat_dec(s, "ttl<", ih->ip_ttl, "> ");
    s = d_strcat(s, "> chk-sum<");
    s = d_vformat_hex(s, bcm_ntohs(ih->ip_sum), 4, 1);
    s = d_strcat(s, ">");

    return(s);
}

/*ARGSUSED*/
static	int
d_ip_length(decode_list_t *dl, void *ihp)
/*
 * Function:	d_ip_length
 * Purpose:	Determine the length of the ip header.
 * Parameters:	dl - pointer to decode list entry.
 *		ihp - pointer to ip header.
 * Returns:	Integer length.
 */
{
    HDR_DECL(struct ip, ih, ihp);

    COMPILER_REFERENCE(dl);

    if (ih->ip_hl < sizeof(struct ip)) {
	return(sizeof(struct ip));
    }
    return(ih->ip_hl);
}

static decode_list_t	d_ip_type[] = {
     {0, 0,	NULL,		"IPv6 Hop-by-Hop Option [RFC1883]"},
     {1, 1,	&d_icmp_table,	"ICMP Internet Control Message [RFC792]"},
     {2, 2,	NULL,		"Internet Group Management [RFC1112]"},
     {3, 3,	NULL,		"Gateway-to-Gateway [RFC823]"},
     {4, 4,	NULL,		"IP in IP (encapsulation) [RFC2003]"},
     {5, 5,	NULL,		"Stream [RFC119,IEN119]"},
     {6, 6,	&d_tcp_table,	"TCP (Transmission Control) [RFC793]"},
     {7, 7,	NULL,		"CBT [Ballardie]"},
     {8, 8,	NULL,		"Exterior Gateway Protocol [RFC88,DLM1]"},
     {9, 9,	NULL,		"any private interior gateway [IANA]"},
    {10, 10,	NULL,		"BBN-RCC-MON BBN RCC Monitoring [SGC]"},
    {11, 11,	NULL,		"NVP-II Network Voice Protocol [RFC74,SC3]"},
    {12, 12,	NULL,		"PUP [PU,XEROX]"},
    {13, 13,	NULL,		"ARGUS [RWS4]"},
    {14, 14,	NULL,		"EMCON [BN7]"},
    {15, 15,	NULL,		"Cross Net Debugger [IEN158,JFH2]"},
    {16, 16,	NULL,		"Chaos [NC3]"},
    {17, 17,	NULL,		"User Datagram [RFC768,JBP]"},
    {18, 18,	NULL,		"Multiplexing [IEN90,JBP]"},
    {19, 19,	NULL,		"DCN Measurement Subsystems [DLM1]"},
    {20, 20,	NULL,		"Host Monitoring [RFC869,RH6]"},
    {21, 21,	NULL,		"Packet Radio Measurement [ZSU]"},
    {22, 22,	NULL,		"XEROX NS IDP [ETHERNET,XEROX]"},
    {23, 23,	NULL,		"Trunk-1 [BWB6]"},
    {24, 24,	NULL,		"Trunk-2 [BWB6]"},
    {25, 25,	NULL,		"Leaf-1 [BWB6]"},
    {26, 26,	NULL,		"Leaf-2 [BWB6]"},
    {27, 27,	NULL,		"Reliable Data Protocol [RFC908,RH6]"},
    {28, 28,	NULL,		"Internet Reliable Transaction "
	                        "[RFC938,TXM]"},
    {29, 29,	NULL,	   	"ISO Transport Protocol Class 4 "
	                        "[RFC969,DDC1]"},
    {31, 31,	NULL,		"MFE Network Services Protocol"},
    {32, 32,	NULL,		"MERIT Internodal Protocol [HWB]"},
    {33, 33,	NULL,		"Sequential Exchange Protocol [JC120]"},
    {34, 34,	NULL,		"Third Party Connect Protocol [SAF3]"},
    {35, 35,	NULL,		"Inter-Domain Policy Routing Protocol "
	                        "[GXC]"},
    {37, 37,	NULL,		"Datagram Delivery Protocol [WXC]"},
    {38, 38,	NULL,		"IDPR Control Message Transport Proto "
	                        "[DXF]"},
    {40, 40,	NULL,		"IL Transport Protocol [Presotto]"},
    {41, 41,	NULL,		"Ipv6 [Deering]    "},
    {42, 42,	NULL,		"Source Demand Routing Protocol [DXE1]"},
    {43, 43,	NULL,		"Route  Routing Header for IPv6 [Deering]"},
    {44, 44,	NULL,		"Frag   Fragment Header for IPv6 [Deering]"},
    {45, 45,	NULL,		"Inter-Domain Routing Protocol [Sue Hares]"},
    {46, 46,	NULL,		"Reservation Protocol [Bob Braden]"},
    {47, 47,	NULL,		"General Routing Encapsulation [Tony Li]"},
    {48, 48,	NULL,		"Mobile Host Routing Protocol"},
    {50, 50,	NULL,		"Encap Security Payload for IPv6 [RFC1827]"},
    {51, 51,	NULL,		"Authentication Header for IPv6 [RFC1826]"},
    {52, 52,	NULL,		"Integrated Net Layer Security  TUBA [JI6]"},
    {54, 54,	&d_arp_table,	"ARP Address Resolution Protocol [RFC1735]"},
    {55, 55,	NULL,		"IP Mobility [Perkins]"},
    {56, 56,	NULL,		"Transport Layer Security Protocol [Oberg]"},
    {57, 57,	NULL,		"SKIP [Markson]"},
    {58, 58,	NULL,		"ICMP for IPv6 [RFC1883]"},
    {59, 59,	NULL,		"No Next Header for IPv6 [RFC1883]"},
    {60, 60,	NULL,		"Destination Options for IPv6 [RFC1883]"},
    {61, 61,	NULL,		"host internal protocol [IANA]"},
    {62, 62,	NULL,		"CFTP [CFTP,HCF2]"},
    {63, 63,	NULL,		"local network [IANA]"},
    {64, 64,	NULL,		"SATNET and Backroom EXPAK [SHB]"},
    {65, 65,	NULL,		"Kryptolan [PXL1]"},
    {66, 66,	NULL,		"MIT Remote Virtual Disk Protocol [MBG]"},
    {67, 67,	NULL,		"Internet Pluribus Packet Core [SHB]"},
    {68, 68,	NULL,		"distributed file system [IANA]"},
    {69, 69,	NULL,		"SATNET Monitoring [SHB]"},
    {70, 70,	NULL,		"VISA Protocol [GXT1]"},
    {71, 71,	NULL,		"Internet Packet Core Utility [SHB]"},
    {72, 72,	NULL,		"Computer Protocol Network Executive "
	                        "[DXM2]"},
    {73, 73,	NULL,		"Computer Protocol Heart Beat [DXM2]"},
    {74, 74,	NULL,		"Wang Span Network [VXD]"},
    {75, 75,	NULL,		"Packet Video Protocol [SC3]"},
    {76, 76,	NULL,		"SAT-MON  Backroom SATNET Monitoring [SHB]"},
    {77, 77,	NULL,		"SUN ND PROTOCOL-Temporary [WM3]"},
    {78, 78,	NULL,		"WIDEBAND Monitoring [SHB]"},
    {79, 79,	NULL,		"WIDEBAND EXPAK [SHB]"},
    {80, 80,	NULL,		"ISO Internet Protocol [MTR]"},
    {81, 81,	NULL,		"VMTP [DRC3]"},
    {82, 82,	NULL,		"VMTP SECURE-VMTP [DRC3]"},
    {83, 83,	NULL,		"VINES [BXH]"},
    {84, 84,	NULL,		"TTP [JXS]"},
    {85, 85,	NULL,		"IGP  NSFNET-IGP [HWB]"},
    {86, 86,	NULL,		"Dissimilar Gateway Protocol [DGP,ML109]"},
    {87, 87,	NULL,		"TCF [GAL5]"},
    {88, 88,	NULL,		"EIGRP [CUSTOM,GXS]"},
    {89, 89,	NULL,		"OSPFIGP [RFC1583,JTM4]"},
    {90, 90,	NULL,		"RPC  Sprite RPC Protocol [SPRITE,BXW] "},
    {91, 91,	NULL,		"Locus Address Resolution Protocol [BXH]"},
    {92, 92,	NULL,		"Multicast Transport Protocol [SXA]"},
    {93, 93,	NULL,		"AX.25 Frames [BK29]"},
    {94, 94,	NULL,		"IP-within-IP Encapsulation Protocol [JI6]"},
    {95, 95,	NULL,		"Mobile Internetworking Control Pro [JI6]"},
    {96, 96,	NULL,		"Semaphore Communications Sec. Pro [HXH]"},
    {97, 97,	NULL,		"Ethernet-within-IP Encapsulation [RXH1]"},
    {98, 98,	NULL,		"Encapsulation Header [RFC1241,RXB3]"},
    {99, 99,	NULL,		"private encryption scheme [IANA]"},
   {100, 100,	NULL,		"GMTP [RXB5]"},
   {101, 101,	NULL,		"Ipsilon Flow Management Protocol [Hinden]"},
   {102, 102,	NULL,		"PNNI over IP [Callon]"},
   {103, 103,	NULL,		"Protocol Independent Multicast "
	                        "[Farinacci]"},
   {104, 104,	NULL,		"ARIS [Feldman]"},
   {105, 105,	NULL,		"SCPS [Durst]"},
   {106, 106,	NULL,		"QNX [Hunter]"},
   {107, 107,	NULL,		"Active Networks [Braden]"},
   {108, 108,	NULL,		"IP Payload Compression Protocol [RFC2393]"},
   {109, 109,	NULL,		"Sitara Networks Protocol"},
   {110, 110,	NULL,		"Peer Compaq Peer Protocol"},
   {111, 111,	NULL,		"in-IP   IPX in IP"},
   {112, 112,	NULL,		"Virtual Router Redundancy Protocol"},
   {114, 114,	NULL,		"0-hop protocol [IANA]"},
   {115, 115,	NULL,		"Layer Two Tunneling Protocol [Aboba]"},
   {116, 116,	NULL,		"D-II Data Exchange (DDX [Worley] "},
   {117, 117,	NULL,		"Interactive Agent Transfer Protocol"},
   {118, 118,	NULL,		"Schedule Transfer [JMP]"},
   {119, 119,	NULL,		"SpectraLink Radio Protocol [Hamilton]"},
   {120, 120,	NULL,		"UTI [Lothberg]"},
   {121, 121,	NULL,		"Simple Message Protocol [Ekblad]"},
   {122, 254,	NULL,		"Unassigned"},
   {255, 255,	NULL,		"Reserved"}
};

static decode_t d_ip_table = {
    "IP",
    d_ip_decode, 
    d_ip_compare,
    d_ip_length, 
    DECODE_LIST(d_ip_type)
};

/************************************************************************
 ************************************************************************
 ************************************************************************
 *									*
 *			       	ICMP	 				*
 */
#define ICMP_MINLEN 8
struct icmp_ra_addr {
    uint32 ira_addr;
    uint32 ira_preference;
};

struct icmp {
  uint8  icmp_type;
  uint8  icmp_code;
  uint16 icmp_cksum;
  union
  {
    uint8 ih_pptr;
    struct in_addr ih_gwaddr;
    struct ih_idseq
    {
      uint16 icd_id;
      uint16 icd_seq;
    } ih_idseq;
    uint32 ih_void;
    struct ih_pmtu
    {
      uint16 ipm_void;
      uint16 ipm_nextmtu;
    } ih_pmtu;
    struct ih_rtradv
    {
      uint8 irt_num_addrs;
      uint8 irt_wpa;
      uint16 irt_lifetime;
    } ih_rtradv;
  } icmp_hun;
  union
  {
    struct
    {
      uint32 its_otime;
      uint32 its_rtime;
      uint32 its_ttime;
    } id_ts;
    struct
    {
      struct ip idi_ip;
      /* options and then 64 bits of data */
    } id_ip;
    struct icmp_ra_addr id_radv;
    uint32   id_mask;
    uint8    id_data[1];
  } icmp_dun;
};

static	int
d_icmp_compare(decode_list_t *dl, void *icp)
/*
 * Function: 	d_icmp_compare
 * Purpose:	Compare routine for ICMP packets.
 * Parameters:	dl - pointer to entry to compare
 *		icp - pointer to header to compare.
 * Returns:	-1 - hdr LESS than dl
 *		0  - hdr EQUAL dl
 *		1  - hdr GREATER than dl
 */
{
    struct icmp *ic = (struct icmp *)icp;

    if (dl->dl_low > ic->icmp_type) {
	return(-1);
    } else if (dl->dl_high < ic->icmp_type) {
	return(1);
    } else {
	return(0);
    }
}

char *
d_icmp_decode(const char *pfx, char *s, void *icp, int il, decode_list_t *dl, void *pd)
/*
 * Function:	p_icmp_decode
 * Purpose: 	Format ICMP header into string.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		icp - icmp header pointer
 *		il - length of packet starting at icp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
    HDR_DECL(struct icmp, ic, icp);

    COMPILER_REFERENCE(pfx);
    COMPILER_REFERENCE(il);
    COMPILER_REFERENCE(pd);

    s = d_strcat_dec(s, "Type-", ic->icmp_type, " (");
    if (dl) {
	s = d_strcat(s, dl->dl_string);
	s = d_strcat(s, ") ");
	s = d_strcat_dec(s, "Code-", ic->icmp_code, " ");
    } else {
	s = d_strcat(s, "INVALID");
	s = d_strcat(s, ") ");
	return(s);
    }

    return(s);
}

static	int
d_icmp_compare_code(decode_list_t *dl, void *icp)
/*
 * Function: 	d_icmp_compare_code
 * Purpose:	Compare routine for ICMP packets on their "code".
 * Parameters:	dl - pointer to entry to compare
 *		icp - pointer to header to compare.
 * Returns:	-1 - hdr LESS than dl
 *		0  - hdr EQUAL dl
 *		1  - hdr GREATER than dl
 */
{
    struct icmp *ic = (struct icmp *)icp;

    if (dl->dl_low > ic->icmp_code) {
	return(-1);
    } else if (dl->dl_high < ic->icmp_code) {
	return(1);
    } else {
	return(0);
    }
}

char *
d_icmp_decode_code(const char *pfx, char *s, void *icp, int il, 
		   decode_list_t *dl)
/*
 * Function:	p_icmp_decode_code
 * Purpose: 	Format ICMP output for a type/code match.
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		icp - icmp header pointer
 *		il - length of packet starting at icp
 *		dl - pointer to data list entry matching
 * Returns:	Pointer to where string ends ('\0')
 */
{
    HDR_DECL(struct icmp, ic, icp);

    COMPILER_REFERENCE(pfx);

    s = d_strcat_dec(s, "Code-", ic->icmp_code, " (");
    if (dl) {
	s = d_strcat(s, dl->dl_string);
	s = d_strcat(s, ") ");
    } else {
	s = d_strcat(s, "*Invalid*");
	s = d_strcat(s, ") ");
	s = d_vformat_bytes(s, (unsigned char *)ic, il, 1, ' ');
	return(s);
    }

    return(s);
}

/*ARGSUSED*/
static	int
d_icmp_length(decode_list_t *dl, void *icp)
/*
 * Function:	d_icmp_length
 * Purpose:	Determine the length of the icmp header.
 * Parameters:	dl - pointer to decode list entry.
 *		icp - pointer to ICMP header.
 * Returns:	Integer length.
 */
{
    COMPILER_REFERENCE(dl);
    COMPILER_REFERENCE(icp);

    return(ICMP_MINLEN);		/* Next layer decode handles it */
}

static	char *
d_icmp_decode_default(const char *pfx, char *s, void *icp, int il, 
		      decode_list_t *dl, void *pd)
/*
 * Function:	p_icmp_decode_default
 * Purpose: 	Format ICMP output for an arbitrary ICMP packet, starting
 *		with the "code". 
 * Parameters:	pfx - output prefix
 *		s - pointer to string to format into
 *		icp - icmp header pointer
 *		il - length of packet starting at icp
 *		dl - pointer to data list entry matching, if NULL, 
 *		     assumes code is not used.
 * Returns:	Pointer to where string ends ('\0')
 */
{
    HDR_DECL(struct icmp, ic, icp);

    COMPILER_REFERENCE(pfx);
    COMPILER_REFERENCE(il);

    if (dl) {
	s = d_strcat(s, " (");
	s = d_strcat(s, dl->dl_string);
	s = d_strcat(s, ")");
    }
    s = d_strcat_dec(s, "Code <", bcm_ntohs(ic->icmp_code), "> Checksum <");
    s = d_vformat_hex(s, bcm_ntohs(ic->icmp_cksum), 4, 1);
    s = d_strcat(s, "> hun <");
    /* For now, simply display remainder in bytes */

    s = d_vformat_bytes(s, (unsigned char *)&ic->icmp_hun, 4, 1, ' ');
    s = d_strcat(s, "> ");

    return(s);
}

/* Forward declare local tables */

static decode_t d_icmp_type3_table;
static decode_t d_icmp_type5_table;
static decode_t d_icmp_type11_table;
static decode_t d_icmp_type12_table;
static decode_t d_icmp_default_table;

static decode_list_t	d_icmp_type[] = {
    {0, 0,	&d_icmp_default_table,	"Echo Reply [RFC792]"},
    {1, 1,	&d_icmp_default_table,	"Unassigned [JBP]"},
    {2, 2,	&d_icmp_default_table,	"Unassigned [JBP]"},
    {3, 3,	&d_icmp_type3_table,	"Destination Unreachable [RFC792]"},
    {4, 4,	&d_icmp_default_table,	"Source Quench [RFC792]"},
    {5, 5,	&d_icmp_type5_table,	"Redirect [RFC792]"},
    {6, 6,	&d_icmp_default_table,	"Alternate Host Address [JBP]"},
    {7, 7,	&d_icmp_default_table,	"Unassigned [JBP]"},
    {8, 8,	&d_icmp_default_table,	"Echo [RFC792]"},
    {9, 9,	&d_icmp_default_table,	"Router Advertisement [RFC1256]"},
    {10, 10,	&d_icmp_default_table,	"Router Selection [RFC1256]"},
    {11, 11,	&d_icmp_type11_table,	"Time Exceeded [RFC792]"},
    {12, 12,	&d_icmp_type12_table,	"Parameter Problem [RFC792]"},
    {13, 13,	&d_icmp_default_table,	"Timestamp [RFC792]"},
    {14, 14,	&d_icmp_default_table,	"Timestamp Reply [RFC792]"},
    {15, 15,	&d_icmp_default_table,	"Information Request [RFC792]"},
    {16, 16,	&d_icmp_default_table,	"Information Reply [RFC792]"},
    {17, 17,	&d_icmp_default_table,	"Address Mask Request [RFC950]"},
    {18, 18,	&d_icmp_default_table,	"Address Mask Reply [RFC950]"},
    {19, 19,	&d_icmp_default_table,	"Reserved (for Security [Solo]"},
    {20, 20,	&d_icmp_default_table,	"Reserved (for Robustness "
	                                "Experiment [ZSu]"},
    {21, 29, 	&d_icmp_default_table,  "Unknown"},	
    {30, 30,	&d_icmp_default_table,	"Traceroute [RFC1393]"},
    {31, 31,	&d_icmp_default_table,	"Datagram Conversion Error "
	                                "[RFC1475]"},
    {32, 32,	&d_icmp_default_table,	"Mobile Host Redirect "
	                                "[David Johnson]"},
    {33, 33,	&d_icmp_default_table,	"IPv6 Where-Are-You"},
    {34, 34,	&d_icmp_default_table,	"IPv6 I-Am-Here"},
    {35, 35,	&d_icmp_default_table,	"Mobile Registration Request"},
    {36, 36,	&d_icmp_default_table,	"Mobile Registration Reply"},
    {37, 37,	&d_icmp_default_table,	"Domain Name Request"},
    {38, 38,	&d_icmp_default_table,	"Domain Name Reply"},
    {39, 39,	&d_icmp_default_table,	"SKIP [Markson]"},
    {40, 40,	&d_icmp_default_table,	"Photuris [Simpson]"},
    {41, 255,	&d_icmp_default_table,	"Reserved"}
};
    
static decode_list_t	d_icmp_type3[] = {
    {0, 0,	NULL,		"Net Unreachable"},
    {1, 1,	NULL,		"Host Unreachable"},
    {2, 2,	NULL,		"Protocol Unreachable"},
    {3, 3,	NULL,		"Port Unreachable"},
    {4, 4,	NULL,		"Fragmentation Needed and Don't "
	                        "Fragment was Set"},
    {5, 5,	NULL,		"Source Route Failed"},
    {6, 6,	NULL,		"Destination Network Unknown"},
    {7, 7,	NULL,		"Destination Host Unknown"},
    {8, 8,	NULL,		"Source Host Isolated"},
    {9, 9,	NULL,		"Communication with Destination Network is "
	                        "Administratively Prohibited"},
    {10, 10,	NULL,		"Communication with Destination Host is "
	                        "Administratively Prohibited"},
    {11, 11,	NULL,		"Destination Network Unreachable for Type "
	                        "of Service"},
    {12, 12,	NULL,		"Destination Host Unreachable for Type "
	                        "of Service"},
    {13, 13,	NULL,		"Communication Administratively Prohibited "
	                        "[RFC1812]"},
    {14, 14,	NULL,		"Host Precedence Violation [RFC1812]"},
    {15, 15,	NULL,		"Precedence cutoff in effect [RFC1812]"},
    {16, 255,	NULL, 		"*Invalid*"}
};

static decode_list_t	d_icmp_type5[] = {
    {0, 0,	NULL,		"Redirect Datagram for the Network "
	                        "(or subnet)"},
    {1, 1,	NULL,		"Redirect Datagram for the Host"},
    {2, 2,	NULL,		"Redirect Datagram for the Type of "
	                        "Service and Network"},
    {3, 3,	NULL,		"Redirect Datagram for the Type of "
	                        "Service and Host"},
    {4, 255,	NULL, 		"*Invalid*"}
};

static decode_list_t	d_icmp_type11[] = {
    {0, 0,	NULL,		"Time to Live exceeded in Transit"},
    {1, 1,	NULL,		"Fragment Reassembly Time Exceeded"},
    {2, 255,	NULL, 		"*Invalid*"}
};

static decode_list_t	d_icmp_type12[] = {
    {0, 0,	NULL,		"Pointer indicates the error"},
    {1, 1,	NULL,		"Missing a Required Option [RFC1108]"},
    {2, 2,	NULL,		"Bad Length"},
    {3, 255,	NULL, 		"*Invalid*"}
    
};

static decode_t d_icmp_table = {
    "ICMP",
    d_icmp_decode, 
    d_icmp_compare,
    d_icmp_length, 
    DECODE_LIST(d_icmp_type)
};

static decode_t d_icmp_type3_table = {	/* Type 3 CODE table */
    "Type 3", 
    d_icmp_decode_default, 
    d_icmp_compare_code,
    d_icmp_length, 
    DECODE_LIST(d_icmp_type3)
};

static decode_t d_icmp_type5_table = {	/* Type 5 CODE table */
    "Type 5", 
    d_icmp_decode_default,
    d_icmp_compare_code,
    d_icmp_length, 
    DECODE_LIST(d_icmp_type5)
};
static decode_t d_icmp_type11_table = {	/* Type 11 CODE table */
    "Type 11", 
    d_icmp_decode_default,
    d_icmp_compare_code,
    d_icmp_length, 
    DECODE_LIST(d_icmp_type11)
};

static decode_t d_icmp_type12_table = {	/* Type 12 CODE table */
    "Type 12", 
    d_icmp_decode_default,
    d_icmp_compare_code,
    d_icmp_length, 
    DECODE_LIST(d_icmp_type12)
};

static decode_t d_icmp_default_table = { /* Generic code table */
    "Generic", 
    d_icmp_decode_default, 
    d_icmp_compare_code,
    d_icmp_length, 
    DECODE_LIST_NULL
};

/************************************************************************
 ************************************************************************
 ************************************************************************
 *									*
 *			       	TCP	 				*
 */

#include <netinet/tcp.h>

static	int
d_tcp_length(decode_list_t *tl, void *tcpp)
/*
 * Function: 
 * Purpose:
 * Parameters:
 * Returns:
 */
{
    COMPILER_REFERENCE(tl);
    COMPILER_REFERENCE(tcpp);
    
    return(sizeof(struct tcphdr));
}

static	char	*
d_tcp_decode(const char *pfx, char *s, void *tcpp, int tl, decode_list_t *dl, void *pd)
/*
 * Function: 	d_tcp_decode
 * Purpose:	Format TCP header into string.
 * Parameters:	pfx - prefix for output.
 *		s - pointer to string to format into.
 *		tcpp - pointer to tcp header.
 *		tl - length remaining in packet.
 *		tl - pointer to matching data list entry.
 * Returns: 	Pointer to where string ends ('\0')
 */
{
    static const char *tcp_flags[] = {
	"FIN", "SYN", "RST", "PUSH", "ACK", "URG"
    };
    HDR_DECL(struct tcphdr, tcp, tcpp);
    int	first, i;

    COMPILER_REFERENCE(pfx);
    COMPILER_REFERENCE(pd);

    if (tl < d_tcp_length(dl, tcpp)) {	/* Invalid length */
	s = d_strcat_dec(s, "*Invalid Length for TCP packet* ", tl, "-bytes");
    }

    s = d_strcat_dec(s, "sPort<", bcm_ntohs(tcp->th_sport), "> ");
    s = d_strcat_dec(s, "dPort<", bcm_ntohs(tcp->th_dport), "> ");
    s = d_strcat_dec(s, "Seq<", ntohl(tcp->th_seq), "> ");
    s = d_strcat_dec(s, "Ack<", ntohl(tcp->th_ack), "> ");
    s = d_strcat_dec(s, "Off<", tcp->th_off, "> Flags<");
    s = d_vformat_hex(s, tcp->th_flags, 2, 1);
    s = d_strcat(s, "=");
    first = TRUE;
    for (i = 0; i < COUNTOF(tcp_flags); i++) {
	if (tcp->th_flags & (1 << i)) {
	    if (first) {
		first = FALSE;
	    } else {
		s = d_strcat(s, ",");
	    }
	    s = d_strcat(s, (char *)tcp_flags[i]);
	}
    }
    s = d_strcat(s, "> Csum<");
    s = d_vformat_hex(s, bcm_ntohs(tcp->th_sum), 4, 1);
    s = d_strcat(s, "> Urp<");
    s = d_vformat_hex(s, bcm_ntohs(tcp->th_urp), 4, 1);
    s = d_strcat(s, ">");
    return(s);
}

static decode_t d_tcp_table = {
    "TCP",
    d_tcp_decode, 
    NULL,
    d_tcp_length, 
    DECODE_LIST_NULL			/* Nothing more to decode */
};

/************************************************************************
 ************************************************************************
 ************************************************************************
 *									*
 *			       	Main Decode 				*
 */

int
d_packet_vformat(char *pfx, char *s, int type, void *p, int pl, void *d)
/*
 * Function:	d_packet_vformat
 * Purpose:	Decode a packet into a buffer.
 * Parameters:	pfx - output prefix
 *		type - type of packet (DECODE_ETHER etc)
 *		p - pointer to packet
 *		pl - length of packet
 * Returns:	0 - success, -1 failed.
 * Notes:	Calls NO routines that require malloc/free etc, and can 
 *		thus be called from an interrupt handler.
 */
{
    switch(type) {
    case DECODE_ETHER:
	(void)d_decode(pfx, s, &d_ether_table, p, pl, d);
	break;
    case DECODE_IP:
	(void)d_decode(pfx, s, &d_ip_table, p, pl, d);
	break;
    case DECODE_TCP:
	(void)d_decode(pfx, s, &d_tcp_table, p, pl, d);
	break;
    case DECODE_ARP:
	(void)d_decode(pfx, s, &d_arp_table, p, pl, d);
	break;
    case DECODE_GB_ETHER:
	(void)d_decode(pfx, s, &d_ether_table, p, pl, d);
	break;
    default:
	(void)d_strcat(s, "****** Invalid type ******");
	return(-1);
    }
    return(0);
}

int
d_packet_format(char *pfx, int type, void *p, int pl, void *d)
/*
 * Function:	d_packet_format
 * Purpose:	Decode a packet and display results. 
 * Parameters:	pfx - pointer to prefix string inserted at start of 
 *			every line.
 *		type - type of packet (DECODE_ETHER etc)
 *		p - pointer to packet
 *		pl - length of packet
 * Returns:	0 - success
 *		-1 - failed
 * Notes:	Calls malloc and can not be used in interrupt handler.
 */
{
    int		rv;
    char 	*s, *ts;

    if (NULL == (s = sal_alloc(4096,"decode"))) {
	rv = -1;
    } else {
	*s = '\0';
	ts = d_strcat(s, pfx);
	rv = d_packet_vformat(pfx, ts, type, p, pl, d);
	if (0 == rv) {
	    sal_printf("%s\n", s);
	}
	sal_free(s);
    }
  
    return(rv);
}

#endif /* __KERNEL__ */

#endif /* NO_SAL_APPL */
#endif /* GHS */
