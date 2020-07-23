/* 
   $Id: $
   This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
   
   Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     IDENT = 258,
     KEY = 259,
     CONSTANT = 260,
     AGGREGATE = 261,
     PROMPT = 262,
     EMPTY = 263,
     KEY_VALUE = 264,
     RANGE = 265,
     ITEM = 266,
     ASSIGN = 267,
     VALUE = 268,
     PRINT = 269,
     VAR = 270,
     CREATE = 271
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif



#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif



#ifndef YYPUSH_DECLS
#  define YYPUSH_DECLS
struct api_mode_pstate;
typedef struct api_mode_pstate api_mode_pstate;
enum { YYPUSH_MORE = 4 };
#if defined __STDC__ || defined __cplusplus
int api_mode_parse (yyscan_t yyscanner, void *parser);
#else
int api_mode_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
int api_mode_push_parse (api_mode_pstate *yyps, int yypushed_char, YYSTYPE const *yypushed_val, YYLTYPE const *yypushed_loc, yyscan_t yyscanner, void *parser);
#else
int api_mode_push_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
int api_mode_pull_parse (api_mode_pstate *yyps, yyscan_t yyscanner, void *parser);
#else
int api_mode_pull_parse ();
#endif
#if defined __STDC__ || defined __cplusplus
api_mode_pstate * api_mode_pstate_new (void);
#else
api_mode_pstate * api_mode_pstate_new ();
#endif
#if defined __STDC__ || defined __cplusplus
void api_mode_pstate_delete (api_mode_pstate *yyps);
#else
void api_mode_pstate_delete ();
#endif
#endif

