/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file is used by the SM-Lite LED processor assembler and disassembler.
 */

typedef unsigned int uint8;

typedef enum {
  PO_EQU,	/* pseudo op: define a label */
  PO_SET,	/* pseudo op: (re)define a label */
    OP_CLC,
    OP_STC,
    OP_CMC,
    OP_RET,
    OP_CALL,
    OP_JMP,
    OP_TAND,
    OP_TOR,
    OP_TXOR,
    OP_TINV,
    OP_PACK,
    OP_POP,
    OP_PUSH,
    OP_PORT,
    OP_PUSHST,
    OP_SEND,
    OP_JZ,
    OP_JC,
    OP_JT,
    OP_JNZ,
    OP_JNC,
    OP_JNT,
    OP_INC,
    OP_ROL,
    OP_DEC,
    OP_ROR,
    OP_XOR,
    OP_OR,
    OP_AND,
    OP_CMP,
    OP_SUB,
    OP_ADD,
    OP_TST,
    OP_BIT,
    OP_LD,
    OP_SENTINAL
} opval;


static struct {
    char  *label;
    opval  token; 
    uint8  opcode;
    uint8  opmask;
} optable[] = {
    { "equ",	PO_EQU,		0x00,	0x00 },
    { "set",	PO_SET,		0x00,	0x00 },
    { "clc",	OP_CLC,		0x07,	0xFF },
    { "stc",	OP_STC,		0x17,	0xFF },
    { "cmc",	OP_CMC,		0x37,	0xFF },
    { "ret",	OP_RET,		0x57,	0xFF },
    { "call",	OP_CALL,	0x67,	0xFF },
    { "jmp",	OP_JMP,		0x77,	0xFF },
    { "pack",	OP_PACK,	0x87,	0xFF },
    { "pop",	OP_POP,		0x97,	0xFF },
    { "txor",	OP_TXOR,	0xA7,	0xFF },
    { "tor",	OP_TOR,		0xB7,	0xFF },
    { "tand",	OP_TAND,	0xC7,	0xFF },
    { "tinv",	OP_TINV,	0xD7,	0xFF },
    { "push",	OP_PUSH,	0x20,	0xF8 },
    { "port",	OP_PORT,	0x28,	0xF8 },
    { "pushst",	OP_PUSHST,	0x30,	0xFC },
    { "send",	OP_SEND,	0x38,	0xF8 },
    { "jz",	OP_JZ,		0x70,	0xFF },
    { "jc",	OP_JC,		0x71,	0xFF },
    { "jt",	OP_JT,		0x72,	0xFF },
    { "jnz",	OP_JNZ,		0x74,	0xFF },
    { "jnc",	OP_JNC,		0x75,	0xFF },
    { "jnt",	OP_JNT,		0x76,	0xFF },
    { "inc",	OP_INC,		0x80,	0xF8 },
    { "dec",	OP_DEC,		0x90,	0xF8 },
    { "rol",	OP_ROL,		0x88,	0xF8 },
    { "ror",	OP_ROR,		0x98,	0xF8 },
    { "xor",	OP_XOR,		0xA0,	0xF0 },
    { "or",	OP_OR,		0xB0,	0xF0 },
    { "and",	OP_AND,		0xC0,	0xF0 },
    { "cmp",	OP_CMP,		0xD0,	0xF0 },
    { "sub",	OP_SUB,		0xE0,	0xF0 },
    { "add",	OP_ADD,		0xF0,	0xF0 },
    { "tst",	OP_TST,		0x08,	0x8C },
    { "bit",	OP_BIT,		0x0C,	0x8C },
    { "ld",	OP_LD,		0x00,	0x88 },
    { "",	OP_SENTINAL,	0x00,	0x00 }
};

