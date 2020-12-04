// $Id: leddasmcore.c,v 1.2 2005/01/17 19:53:19 csm Exp $
// This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
// 
// Copyright 2007-2020 Broadcom Inc. All rights reserved.
//
// This is a simple disassembler for the embedded processor used in the
// SM-Lite LED controller.  The particulars of the instruction set
// are documented elsewhere.
//
// The code is structured such that this portion can be called from
// different drivers, depending on need.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "ledasm.h"


// we return a string of the decoded field.
// if there is an error, we return an empty "" string, and *len is set to 0.
// otherwise, len contains the length of the instruction (1 or 2).
// field encodes one of A, B, #, (A), (B), (#), or CY
static char *
dasm_sssx(int field, int byte2, int *len)
{
    static char buf[16];

    switch (field) {
	case 0: *len = 1; return "a";
	case 1: *len = 1; return "b";
	case 2: *len = 2; sprintf(buf,"0x%02x",byte2); return buf;
	case 4: *len = 1; return "(a)";
	case 5: *len = 1; return "(b)";
	case 6: *len = 2; sprintf(buf,"(0x%02x)",byte2); return buf;
	case 7: *len = 1; return "cy";
	default: *len = 0; return "";
    }
}

// field encodes one of A, B, #, (A), (B), (#)
static char *
dasm_sss(int field, int byte2, int *len)
{
    if (field == 7) {
	*len = 0;
	return "";	// CY
    }
    return dasm_sssx(field, byte2, len);
}

// field encodes one of A, B, #.
static char *
dasm_ss(int field, int byte2, int *len)
{
    return dasm_sssx(field, byte2, len);
}


// field encodes one of A, B, (A), (B), (#)
// we can't simply call dasm_sss because a few instructions
// call both dasm_ddd and dasm_ss or dasm_sss.  since we
// use a static buf for output, if we called it twice in one
// statement we'd get hosed.
static char *
dasm_ddd(int field, int byte2, int *len)
{
    static char buf[16];

    switch (field) {
	case 0: *len = 1; return "a";
	case 1: *len = 1; return "b";
	case 4: *len = 1; return "(a)";
	case 5: *len = 1; return "(b)";
	case 6: *len = 2; sprintf(buf,"(0x%02x)",byte2); return buf;
	default: *len = 0; return "";
    }
}


// field encodes one of A or B.
static char *
dasm_d(int field)
{
    return (field) ? "b" : "a";
}


// all instructions are either one or two bytes long.  the caller
// passes in byte from (pc) and (pc+1), and we return an int
// indicating the length of the instruction.
// 0 is returned if it is an illegal op.
int
disassem(int b1, int b2, char *buf)
{
    int hit, op;
    int len, len2;


    // find a matching opcode
    hit = 0;
    for(op=0; optable[op].token != OP_SENTINAL; op++) {
	if (optable[op].opmask == 0x00)
	    continue;	// pseudo-ops
	if (optable[op].opcode == (b1 & optable[op].opmask)) {
	    hit = 1;
	    break;
	}
    }
    if (!hit) {
	sprintf(buf, "\tdb\t0x%02x", b1);
	return 0;	// unknown opcode
    }


    // handle the particulars for an opcode class
    switch (optable[op].token) {

    case OP_CLC:
    case OP_STC:
    case OP_CMC:
    case OP_RET:
    case OP_TAND:
    case OP_TOR:
    case OP_TXOR:
    case OP_TINV:
    case OP_PACK:
    case OP_POP:
	sprintf(buf, "\t%s", optable[op].label);
	len = 1;
	break;

    case OP_CALL:
    case OP_JMP:
    case OP_JZ:
    case OP_JC:
    case OP_JT:
    case OP_JNZ:
    case OP_JNC:
    case OP_JNT:
	sprintf(buf, "\t%s\t0x%02x", optable[op].label, b2);
	len = 2;
	break;

    case OP_PUSH:
	sprintf(buf, "\t%s\t%s", optable[op].label, dasm_sssx(b1&7,b2,&len));
	break;

    case OP_PORT:
    case OP_SEND:
	sprintf(buf, "\t%s\t%s", optable[op].label, dasm_sss(b1&7,b2,&len));
	break;

    case OP_PUSHST:
	sprintf(buf, "\t%s\t%s", optable[op].label, dasm_ss(b1&3,b2,&len));
	break;

    case OP_INC:
    case OP_DEC:
    case OP_ROL:
    case OP_ROR:
	sprintf(buf, "\t%s\t%s", optable[op].label, dasm_ddd(b1&7,b2,&len));
	break;

    case OP_ADD:
    case OP_SUB:
    case OP_CMP:
    case OP_AND:
    case OP_OR:
    case OP_XOR:
	sprintf(buf, "\t%s\t%s,%s", optable[op].label, dasm_d((b1>>3)&1),
						       dasm_sss(b1&7,b2,&len));
	break;

    case OP_TST:
    case OP_BIT:
	sprintf(buf, "\t%s\t%s,%s", optable[op].label, dasm_ddd((b1>>4)&7,b2,&len),
						       dasm_ss(b1&3,b2,&len2));
	if (len == 2 && len2 == 2) {
	    sprintf(buf, "\tdb\t0x%02x", b1);	// unknown opcode
	    len = 0;
	} else if (len2 == 2)
	    len = 2;	// longer one wins
	break;

    case OP_LD:
	sprintf(buf, "\t%s\t%s,%s", optable[op].label, dasm_ddd((b1>>4)&7,b2,&len),
						       dasm_sss(b1&7,b2,&len2));
	if (len == 2 && len2 == 2) {
	    sprintf(buf, "\tdb\t0x%02x", b1);	// unknown opcode
	    len = 0;
	} else if (len2 == 2)
	    len = 2;	// longer one wins
	break;

    default:
	printf("internal error: impossible case");
	exit(-1);
	break;
    }

    return len;
}

