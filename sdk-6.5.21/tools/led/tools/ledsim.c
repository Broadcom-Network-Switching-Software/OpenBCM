// $Id: ledsim.c,v 1.2 2005/01/17 19:53:19 csm Exp $
// This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
// 
// Copyright 2007-2020 Broadcom Inc. All rights reserved.
//
// This is a simulator for the SML LED processor.
// See the cmd_help() subroutine for the commands.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include "ledasm.h"

// prototype for external routine
int disassem(int b1, int b2, char *buf);


// processor state
struct {
    uint8 program[256];		// the program space
    uint8 data[256];		// the data space
    uint8 pc;			// program counter
    uint8 ret[2];		// return stack
    uint8 reg_a;		// register a
    uint8 reg_b;		// register b
    int   cflag;		// carry flag
    int   zflag;		// zero flag
    int   tstack;		// 4-bit boolean T stack
    // hidden state
    int   bitptr;		// PACK bit pointer
    int   port;			// PORT state

    // fictitious state
    int   trace;		// trace simulation
    int   halted;

} up;

int g_int;		// got keyboard interrupt
int g_valid_program;	// program space has been initialized
int g_valid_data;	// data space has been initialized


#define TSTACK(n) ((up.tstack >> (n))&1)

int
tstack_pop(void)
{
    int tos = TSTACK(0);
    up.tstack = (up.tstack >> 1) & 0x7;
    return tos;
}

void
tstack_push(int b)
{
    up.tstack = ((up.tstack << 1) & 0xE)
	      | (b & 0x1);
}


long
my_atol(char *p)
{
    // accepts octal, decimal, or hex input constants
    return strtol(p, NULL, 0);
}

// catch CTRL-C
void
catch_int(int foo)
{
    g_int = 1;
    printf("\n");
}

void
install_sig_int(void)
{
    if (signal(SIGINT, catch_int) == SIG_IGN)
	printf("Hey, someone already said to ignore this one");
    g_int = 0;
}

void
remove_sig_int(void)
{
    signal(SIGINT, SIG_DFL);	// default handler
}


// display bytes, from addr1 to addr2, inclusive
void
cmd_db(int addr1, int addr2)
{
    int aflag=0;
    int i;

    if (addr1 < 0x00 || addr2 > 0xFF) {
	printf("Error: db <a1> <a2> -- address out of range\n");
	return;
    }

    if (addr2-addr1+1 <= 16) {
	// it will all fit on one line
	printf("%02X: ", addr1);
	for(i=addr1; i<=addr2; i++) {
	    printf(" %02X", up.data[i]);
	}
	printf("\n");
	return;
    }

    // it will take multiple lines.  line them up.
    for(i=addr1&~15; i<=addr2; i++) {
	if (!aflag) {
	    printf("%02X: ", i);
	    aflag = 1;
	}
	if (i < addr1)
	    printf("   ");
	else
	    printf(" %02X", up.data[i]);
	if (i%16 == 15) {
	    aflag = 0;
	    printf("\n");
	}
    }
    if (aflag)
	printf("\n");
}


// disassemble address range, inclusive
void
cmd_dasm(int addr1, int addr2)
{
    char line[256];
    int pc, instr_len;

    if (addr1 < 0x00 || addr2 > 0xFF) {
	printf("Error: dis <a1> <a2> -- address out of range\n");
	return;
    }

    for(pc=addr1; pc<=addr2; pc+=instr_len) {
	uint8 b1 = up.program[pc];
	uint8 b2 = up.program[pc+1];
	instr_len = disassem(b1,b2,line);
	if (instr_len == 0)
	    instr_len = 1;	// illegal op -- make forward progress anyway
	if (instr_len == 2)
	    printf("%02X: %02X %02X  %s\n", pc, up.program[pc],
						up.program[pc+1], line);
	else
	    printf("%02X: %02X     %s\n", pc, up.program[pc], line);
    }
}


// this fills the program ram from a file in a simple format:
// there are no addresses, just 16 lines of 16 bytes per line,
// printed in upper case hex.
// return 1 on success, 0 on failure.
int
cmd_program(char *basename)
{
    FILE *fp = 0;
    int i;

    // open the file exactly as specified, and failing that, try
    // appending ".asm" and opening again.
    fp = fopen(basename, "r");
    if (!fp) {
	char name[256];
	strcpy(name,basename);
	strcat(name,".hex");
	fp = fopen(name, "r");
    }
    if (!fp) {
	printf("Error opening source file '%s'\n", basename);
	return 0;
    }

    // read input file into up.program[] array
    for(i=0; i<256; i++) {
	int byte;
	if (fscanf(fp,"%02X", &byte) == 0) {
	    printf("Error reading hex file: failure on the %d-th byte\n", i);
	    return 0;
	}
	up.program[i] = byte;
    }

    fclose(fp);
    return 1;
}


// this fills the data ram from a file in a simple format:
// there are no addresses, just n lines of 16 bytes per line,
// printed in upper case hex.
// return 1 on success, 0 on failure.
int
cmd_data(char *basename)
{
    FILE *fp = 0;
    int i;

    // open the file exactly as specified, and failing that, try
    // appending ".dat" and opening again.
    fp = fopen(basename, "r");
    if (!fp) {
	char name[256];
	strcpy(name,basename);
	strcat(name,".dat");
	fp = fopen(name, "r");
    }
    if (!fp) {
	printf("Error opening source file '%s'\n", basename);
	return 0;
    }

    // read input file into up.data[] array
    for(i=0; i<256; i++) {
	int byte;
	if (fscanf(fp,"%02X", &byte) == 0) {
	    printf("Error reading hex file: failure on the %d-th byte\n", i);
	    return 0;
	}
	up.data[i] = byte;
    }

    fclose(fp);
    return 1;
}


// reset simulated processor state
void
sim_reset_regs(void)
{
    up.pc     = 0x00;
    up.ret[0] = 0xFE;
    up.ret[1] = 0xFE;
    up.reg_a  = 0x00;
    up.reg_b  = 0x00;
    up.cflag  = 0;
    up.zflag  = 0;
    up.tstack = 0x0;
    up.bitptr = 0x40 << 3;	// bit 0 of byte 64
    up.port   = 0x00;

    up.halted = 0;		// running after reset
}

void
sim_reset(void)
{
    int i;
    sim_reset_regs();
    for(i=0; i<256; i++) {
	up.program[i] = 0x00;
	up.data[i]    = 0x00;
    }
    g_valid_program = 0;
    g_valid_data    = 0;

    up.trace = 0;		// trace off by default
}


// field encodes one of A, B, #, (A), (B), (#), or CY
int
src_sssx(int field, int byte2, int *len)
{
    switch (field) {
	case 0: *len = 1; return up.reg_a;
	case 1: *len = 1; return up.reg_b;
	case 2: *len = 2; return byte2;
	case 4: *len = 1; return up.data[up.reg_a];
	case 5: *len = 1; return up.data[up.reg_b];
	case 6: *len = 2; return up.data[byte2];
	case 7: *len = 1; return up.cflag;
	default: *len = 0; return 0x00;
    }
}

// field encodes one of A, B, #, (A), (B), (#)
int
src_sss(int field, int byte2, int *len)
{
    if (field == 7) {
	*len = 0;
	return 0x00;	// CY
    }
    return src_sssx(field, byte2, len);
}

// field encodes one of A, B, #.
int
src_ss(int field, int byte2, int *len)
{
    return src_sssx(field, byte2, len);
}


// field encodes one of A, B, (A), (B), (#)
// illegal field returns with len=0.
void
dst_ddd(int field, int byte2, int val, int *len)
{
    switch (field) {
	case 0: *len = 1; up.reg_a          = val; break;
	case 1: *len = 1; up.reg_b          = val; break;
	case 4: *len = 1; up.data[up.reg_a] = val; break;
	case 5: *len = 1; up.data[up.reg_b] = val; break;
	case 6: *len = 2; up.data[byte2]    = val; break;
	default: *len = 0;                         break;
    }
}


// field encodes one of A, B, (A), (B), (#)
int
src_ddd(int field, int byte2, int *len)
{
    if ((field == 2) || (field == 7)) {
	*len = 0;
	return 0x00;
    }
    return src_sssx(field, byte2, len);
}

// field encodes one of A or B.
int
src_d(int field)
{
    if (field)
	return up.reg_b;
    else
	return up.reg_a;
}

// field encodes one of A or B.
void
dst_d(int field, int val)
{
    if (field)
	up.reg_b = val;
    else
	up.reg_a = val;
}


// disassemble current instruction, print current state
void
do_trace(void)
{
    char line[128];
    uint8 b1 = up.program[up.pc];
    uint8 b2 = up.program[up.pc+1];
    int instr_len = disassem(b1,b2,line);

    if (instr_len == 0)
	instr_len = 1;	// illegal op -- make forward progress anyway

    if (instr_len == 2)
	printf("%02X: %02X %02X%s", up.pc, b1, b2, line);
    else
	printf("%02X: %02X     %s", up.pc, b1, line);

    // because of the use of tabs, we need to explicitly count columns
    // in order to get the register dump to line up.  yuk.
    {
	int col = 0;
	char *p;

	for(p = line; *p; p++) {
	    if (*p == '\t')
		col = (col+8) & ~7;
	    else
		col++;
	}
	while (col < 32) {
	    printf("\t");
	    col = (col+8) & ~7;
	}

	printf("ra=%02X, rb=%02X, c=%d, z=%d, t=%d%d%d%d, ret=%02X\n",
		up.reg_a, up.reg_b, up.cflag, up.zflag,
		TSTACK(3), TSTACK(2), TSTACK(1), TSTACK(0), up.ret[0]);
    }
}


// simulate execution of one instruction
void
sim_instruction(void)
{
    int hit, op;
    int len, len2;
    int b1, b2;
    int opc, imm;
    int byte, bit;
    int tmp, tmp2;

    if (up.halted) {
	printf("no instruction: processor is halted\n");
	return;
    }

    if (g_valid_program && up.trace)
	do_trace();

    opc = up.program[up.pc];
    imm = up.program[up.pc + 1];

    // find a matching opcode
    hit = 0;
    for(op=0; optable[op].token != OP_SENTINAL; op++) {
	if (optable[op].opmask == 0x00)
	    continue;	// pseudo-ops
	if (optable[op].opcode == (opc & optable[op].opmask)) {
	    hit = 1;
	    break;
	}
    }
    if (!hit) {
	printf("hit bad opcode: pc=0x%02X, instr=0x%02X\n", up.pc, opc);
	up.halted = 1;
	return;
    }


    // handle the particulars for an opcode class
    switch (optable[op].token) {

    case OP_CLC:
	up.cflag = 0;
	len = 1;
	break;

    case OP_STC:
	up.cflag = 1;
	len = 1;
	break;

    case OP_CMC:
	up.cflag = !up.cflag;
	len = 1;
	break;

    case OP_RET:
	up.pc     = up.ret[0];
	up.ret[0] = up.ret[1];
	up.ret[1] = 0xFE;
	return;

    case OP_TAND:
	b1 = tstack_pop();
	b2 = tstack_pop();
	tstack_push(b1 & b2);
	len = 1;
	break;

    case OP_TOR:
	b1 = tstack_pop();
	b2 = tstack_pop();
	tstack_push(b1 | b2);
	len = 1;
	break;

    case OP_TXOR:
	b1 = tstack_pop();
	b2 = tstack_pop();
	tstack_push(b1 ^ b2);
	len = 1;
	break;

    case OP_TINV:
	b1 = tstack_pop();
	tstack_push(!b1);
	len = 1;
	break;

    case OP_POP:
	up.cflag = tstack_pop();
	len = 1;
	break;

    case OP_PACK:
	byte = (up.bitptr >> 3) & 0xFF;
	bit  = (up.bitptr & 7);
	up.data[byte] = (up.data[byte] & ~(1<<bit))	// clear bit
		      | (tstack_pop()<<bit);		// copy bit
	up.bitptr++;
	if (up.bitptr >= (0x60 << 3))
	    printf("Warning: more than 255 PACK operations");
	len = 1;
	break;

    case OP_CALL:
	up.ret[1] = up.ret[0];
	up.ret[0] = (up.pc + 2) & 0xFF;
	up.pc     = imm;
	return;

    case OP_JMP:
	up.pc = imm;
	return;

    case OP_JZ:  up.pc = ( up.zflag)  ? imm : (up.pc+2); return;
    case OP_JC:  up.pc = ( up.cflag)  ? imm : (up.pc+2); return;
    case OP_JT:  up.pc = ( TSTACK(0)) ? imm : (up.pc+2); return;
    case OP_JNZ: up.pc = (!up.zflag)  ? imm : (up.pc+2); return;
    case OP_JNC: up.pc = (!up.cflag)  ? imm : (up.pc+2); return;
    case OP_JNT: up.pc = (!TSTACK(0)) ? imm : (up.pc+2); return;

    case OP_PUSH:
	tmp = src_sssx(opc&7,imm,&len);
	if (len != 0)
	    tstack_push(tmp&1);
	break;

    case OP_PORT:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0)
	    up.port = tmp;
	break;

    case OP_SEND:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    up.reg_a  = tmp;	// left with # of led clocks to send
	    up.halted = 1;
	}
	break;

    case OP_PUSHST:
	tmp = src_ss(opc&3,imm,&len);
	if (len != 0) {
	    byte = (tmp >> 3) & 1;
	    bit  = (tmp >> 0) & 7;
	    b1 = up.data[2*up.port + byte];
	    tstack_push((b1 >> bit) & 1);
	}
	break;

    case OP_INC:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    up.cflag = (tmp == 0xFF);
	    up.zflag = (tmp == 0xFF);
	    tmp = (tmp + 1) & 0xFF;
	    dst_ddd(opc&7,imm,tmp,&len);
	}
	break;

    case OP_DEC:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    up.cflag = (tmp == 0x00);
	    up.zflag = (tmp == 0x01);
	    tmp = (tmp - 1) & 0xFF;
	    dst_ddd(opc&7,imm,tmp,&len);
	}
	break;

    case OP_ROL:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    up.cflag = (tmp >> 7) & 1;
	    up.zflag = (tmp == 0x00);
	    tmp = ((tmp << 1) | (tmp >> 7)) & 0xFF;
	    dst_ddd(opc&7,imm,tmp,&len);
	}
	break;


    case OP_ROR:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    up.cflag = (tmp >> 0) & 1;
	    up.zflag = (tmp == 0x00);
	    tmp = ((tmp >> 1) | (tmp << 7)) & 0xFF;
	    dst_ddd(opc&7,imm,tmp,&len);
	}
	break;

    case OP_ADD:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    tmp2 = src_d((opc>>3)&1);
	    tmp2 = tmp2 + tmp;
	    up.cflag = (tmp2 > 0xFF);
	    tmp2 &= 0xFF;
	    up.zflag = (tmp2 == 0x00);
	    dst_d((opc>>3)&1,tmp2);
	}
	break;

    case OP_SUB:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    tmp2 = src_d((opc>>3)&1);
	    tmp2 = tmp2 - tmp;
	    up.cflag = (tmp2 < 0x00);
	    tmp2 &= 0xFF;
	    up.zflag = (tmp2 == 0x00);
	    dst_d((opc>>3)&1,tmp2);
	}
	break;

    case OP_CMP:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    tmp2 = src_d((opc>>3)&1);
	    tmp2 = tmp2 - tmp;
	    up.cflag = (tmp2 < 0x00);
	    tmp2 &= 0xFF;
	    up.zflag = (tmp2 == 0x00);
	}
	break;

    case OP_AND:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    tmp2 = src_d((opc>>3)&1);
	    tmp2 = tmp2 & tmp;
	    up.zflag = (tmp2 == 0x00);
	    dst_d((opc>>3)&1,tmp2);
	}
	break;

    case OP_OR:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    tmp2 = src_d((opc>>3)&1);
	    tmp2 = tmp2 | tmp;
	    up.zflag = (tmp2 == 0x00);
	    dst_d((opc>>3)&1,tmp2);
	}
	break;

    case OP_XOR:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    tmp2 = src_d((opc>>3)&1);
	    tmp2 = tmp2 ^ tmp;
	    up.zflag = (tmp2 == 0x00);
	    dst_d((opc>>3)&1,tmp2);
	}
	break;

    case OP_TST:
	tmp  = src_ss(opc&3,imm,&len);
	tmp2 = src_ddd((opc>>4)&7,imm,&len2);
	if (len != 0 && len2 != 0) {
	    tmp &= 7;
	    up.cflag = (tmp2 >> tmp) & 1;
	    if (len2 > len)
		len = len2;
	}
	break;

    case OP_BIT:
	tmp  = src_ss(opc&3,imm,&len);
	tmp2 = src_ddd((opc>>4)&7,imm,&len2);
	if (len != 0 && len2 != 0) {
	    tmp &= 7;
	    tmp2 = (tmp2 & ~(1<<tmp))	// clear bit ss
		 | (up.cflag  <<tmp);	// conditionally set bit ss
	    dst_ddd((opc>>4)&7,imm,tmp2,&len2);
	    if (len2 > len)
		len = len2;
	} else
	    len = 0;
	break;

    case OP_LD:
	tmp = src_sss(opc&7,imm,&len);
	if (len != 0) {
	    dst_ddd((opc>>4)&7,imm,tmp,&len2);
	    if (len2 > len)
		len = len2;
	} else
	    len = 0;
	break;

    default:
	len = 0;
	break;
    }

    // all typical instructions (those that don't explicitly modify
    // the PC), fall through to here
    if (len == 0) {
	printf("hit bad opcode: pc=0x%02X, instr=0x%02X\n", up.pc, opc);
	up.halted = 1;
    } else {
	up.pc += len;
    }
}


void
cmd_help(void)
{
    printf("Commands:\n");
    printf("    program <filename>      -- initializes program memory from file\n");
    printf("    data <filename>         -- initializes 0x00 to 0x3F port status from file\n");
    printf("    reset                   -- reset the processor state\n");
    printf("    run                     -- runs program from instruction 0x00 until halt\n");
    printf("    s(tep) [<n>]            -- steps one (or <n>) instruction(s)\n");
    printf("    c(ont)                  -- continue running until halt\n");
    printf("    db <addr1> [<addr2>]    -- display byte or range of bytes from data space\n");
    printf("    dis [<addr1> [<addr2>]] -- disassemble program address range\n");
    printf("    trace [on|off]          -- enable/disable detailed tracing\n");
    printf("    help                    -- this message\n");
    printf("    quit (or exit)          -- end simulation\n");
}


void
usage(void)
{
    printf("Usage:\n");
    printf("    ledsim\n");
    printf("\tenter interactive mode.\n");
    exit(-1);
}


// get input
void
cmd_interpreter(void)
{
    char ibuf[1024];
    char ibuf2[1024];
    char cmd[1024];
    char arg1[1024];
    char arg2[1024];
    int  args;
    long iarg1, iarg2;
    int  old_ok = 0;

    sim_reset();

    while (1) {

	// disassemble current instruction
	if (g_valid_program)
	    do_trace();

	// prompt user
	printf("cmd>"); fflush(stdout);

	// get user input
	if (fgets(ibuf, sizeof(ibuf), stdin) == NULL) {
	    // no more input
	    return;
	}

	if (ibuf[0] == '\n') {
	    if (!old_ok)
		continue;
	    strcpy(ibuf, ibuf2);	// recycle last command
	} else {
	    old_ok = 1;
	    strcpy(ibuf2, ibuf);	// save for next time
	}

	// parse user input
	cmd[0] = arg1[0] = arg2[0] = '\0';
	args = sscanf(ibuf, "%s %s %s",cmd,arg1,arg2);
	//printf("args=%d, arg[0]='%s', arg[1]='%s', arg2='%s'\n", args, cmd, arg1, arg2);

	// if there is no input, just reuse previous command
	if (args == -1) {
	    printf("bad command\n");
		continue;
	}

	if (!strcmp(cmd, "help")) {
	    cmd_help();
	    continue;
	}

	if (!strcmp(cmd, "exit") || !strcmp(cmd, "quit"))
	    return;

	if (!strcmp(cmd, "reset")) {
	    sim_reset_regs();
	    continue;
	}

	if (!strcmp(cmd, "run")) {
	    if (!g_valid_program) {
		printf("program space is uninitialized -- nothing to run\n");
		continue;
	    }
	    sim_reset_regs();
	    install_sig_int();
	    do {
		sim_instruction();
	    } while (!up.halted && !g_int);
	    remove_sig_int();
	    continue;
	}

	if (!strcmp(cmd, "c") || !strcmp(cmd, "cont")) {
	    if (!g_valid_program) {
		printf("program space is uninitialized -- nothing to run\n");
		continue;
	    }
	    install_sig_int();
	    do {
		sim_instruction();
	    } while (!up.halted && !g_int);
	    remove_sig_int();
	    continue;
	}

	if (!strcmp(cmd, "trace")) {
	    if (args == 1) {
		printf("trace is now on\n");
		up.trace = 1;
		continue;
	    }
	    if (!strcmp(arg1, "on")) {
		printf("trace is now on\n");
		up.trace = 1;
		continue;
	    }
	    if (!strcmp(arg1, "off")) {
		printf("trace is now off\n");
		up.trace = 0;
		continue;
	    }
	    printf("Error: badly formed trace command\n");
	    continue;
	}
	    
	if (!strcmp(cmd, "program") || !strcmp(cmd, "prog")) {
	    if (args != 2) {
		printf("Error: program must have exactly one arg\n");
	    } else {
		g_valid_program = cmd_program(arg1);
	    }
	    continue;
	}

	if (!strcmp(cmd, "data")) {
	    if (args != 2) {
		printf("Error: data must have exactly one arg\n");
	    } else {
		g_valid_data = cmd_data(arg1);
	    }
	    continue;
	}

	// all other commands accept a number as arg1 and arg2
	// no error checking
	if (args > 1) {
	    iarg1 = my_atol(arg1);
	    iarg2 = (args > 2) ? my_atol(arg2) : iarg1;
	}

	if (!strcmp(cmd, "dis")) {
	    if (args == 1)
		iarg1 = iarg2 = up.pc;
	    cmd_dasm(iarg1,iarg2);
	    continue;
	}

	if (!strcmp(cmd, "db")) {
	    if (args < 2) {
		printf("Error: must have at least one arg\n");
	    } else {
		cmd_db(iarg1,iarg2);
	    }
	    continue;
	}

	if (!strcmp(cmd, "s") || !strcmp(cmd, "step")) {
	    if (!g_valid_program) {
		printf("program space is uninitialized -- nothing to run\n");
		continue;
	    }
	    if (args == 1)
		iarg1 = 1;
	    while ((iarg1-- > 0) && !up.halted) {
		sim_instruction();
	    }
	    continue;
	}

	printf("Error: bad command\n");

    } // while (1)
}


// command line accepts no args.
int
main(int argc, char *argv[])
{
    if (argc != 1) {
	usage();
    }

    cmd_interpreter();

    return 0;
}
