/* Instruction printing code for the ARC.
   Copyright 1994, 1995, 1997, 1998, 2000, 2001, 2002, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by Doug Evans (dje@cygnus.com).

   Copyright 2008-2012 Synopsys Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include <ctype.h>
#include <stdarg.h>
#include <ansidecl.h>
#include <string.h>
#include <assert.h>

#include "dis-asm.h"
#include "opcode/arc.h"
#include "arc-ext.h"
#include "arc-dis-old.h"
#include "arcompact-dis.h"
#include "elf-bfd.h"
#include "elf/arc.h"

  /*
    warning: implicit declaration of function `printf_unfiltered'
    if dbg is 1 then this definition is required
  */
  void printf_unfiltered (const char *,...);
static bfd_vma bfd_getm32 (unsigned int);
static bfd_vma bfd_getm32_ac (unsigned int) ATTRIBUTE_UNUSED;


#ifndef dbg
#define dbg	(0)
#endif

  /*
    Ravi:
    : undefined reference to `printf_unfiltered'
    if dbg is 1 then this definition is required
  */
#if dbg
  void printf_unfiltered (const char *,...)
 {
   va_list args;
   va_start (args, format);
   vfprintf_unfiltered (gdb_stdout, format, args);
   va_end (args);
 }
#endif

#undef _NELEM
#define _NELEM(ary)	(sizeof(ary) / sizeof(ary[0]))

#define BIT(word,n)       ((word) & (1 << n))
/* START ARC LOCAL */
#define BITS(word,s,e)    (((word) << (sizeof(word)*8-1 - e)) >> (s+(sizeof(word)*8-1 - e)))
/* END ARC LOCAL */
#define OPCODE(word)      (BITS ((word), 27, 31))
#define FIELDA(word)      (BITS ((word), 0, 5))
#define FIELDb(word)      (BITS ((word), 24, 26))
#define FIELDB(word)      (BITS ((word), 12, 14))
#define FIELDC(word)      (BITS ((word), 6, 11))
#define OPCODE_AC(word)   (BITS ((word), 11, 15))

#define FIELDA_AC(word)   (BITS ((word), 0, 2))
#define FIELDB_AC(word)   (BITS ((word), 8, 10))
#define FIELDC_AC(word)   (BITS ((word), 5, 7))
#define FIELDU_AC(word)   (BITS ((word), 0, 4))

#ifdef ARC_NPS_CMDS
#define FIELDB_AC16(word)   (BITS ((word), 24, 26))
#define FIELDC_AC16(word)   (BITS ((word), 21, 23))
#endif // #ifdef ARC_NPS_CMDS
/*
 * FIELDS_AC is the 11-bit signed immediate value used for
 * GP-relative instructions.
 */
#define FIELDS_AC(word)   (BITS (((signed int) word), 0, 8))

/*
 * FIELDD is signed in all of its uses, so we make sure argument is
 * treated as signed for bit shifting purposes.
 */
#define FIELDD(word)      (BITS (((signed int) word), 16, 23))

/*
 * FIELDD9 is the 9-bit signed immediate value used for
 * load/store instructions.
 */
#define FIELDD9(word)     ((BITS(((signed int)word),15,15) << 8) | (BITS((word),16,23)))

/*
 * FIELDS is the 12-bit signed immediate value
 */
#define FIELDS(word)      ((BITS(((signed int)word),0,5) << 6) | (BITS((word),6,11)))					\

/*
 * FIELD S9 is the 9-bit signed immediate value used for
 * bbit0/bbit instruction
 */
#define FIELDS9(word)     (((BITS(((signed int)word),15,15) << 7) | (BITS((word),17,23))) << 1)
#define FIELDS9_FLAG(word)     (((BITS(((signed int)word),0,5) << 6) | (BITS((word),6,11))) )

#define PUT_NEXT_WORD_IN(a) {		\
	if (is_limm==1 && !NEXT_WORD(1))       	\
	  mwerror(state, "Illegal limm reference in last instruction!\n"); \
	  if (info->endian == BFD_ENDIAN_LITTLE) { \
	    a = ((state->words[1] & 0xff00) | (state->words[1] & 0xff)) << 16; \
	    a |= ((state->words[1] & 0xff0000) | (state->words[1] & 0xff000000)) >> 16;	\
	  } \
	  else { \
	    a = state->words[1]; \
	  } \
	}

#define CHECK_NULLIFY() do{		\
	state->nullifyMode = BITS(state->words[0],5,5);	\
	}while(0)

#define CHECK_COND_NULLIFY() do {		\
	state->nullifyMode = BITS(state->words[0],5,5);	\
	cond = BITS(state->words[0],0,4);	\
	}while(0)

#define CHECK_FLAG_COND_NULLIFY() do{	\
	if (is_shimm == 0) {			\
	  flag = BIT(state->words[0],15);	\
	  state->nullifyMode = BITS(state->words[0],5,5); \
	  cond = BITS(state->words[0],0,4);	\
	}					\
	}while(0)

#define CHECK_FLAG_COND() {		\
	if (is_shimm == 0) {			\
	  flag = BIT(state->words[0],15);	\
	  cond = BITS(state->words[0],0,4);	\
	}					\
	}

#define CHECK_FLAG() {			\
	flag = BIT(state->words[0],15);	\
	}

#define CHECK_COND() {		                \
	if (is_shimm == 0) {			\
	  cond = BITS(state->words[0],0,4);	\
	}					\
	}

#define CHECK_FIELD(field) {			\
	if (field == 62) {			\
	  is_limm++;				\
	  field##isReg = 0;			\
	  PUT_NEXT_WORD_IN(field);		\
	  limm_value = field;			\
	}					\
	}

#define CHECK_FIELD_A() {			\
	fieldA = FIELDA(state->words[0]);	\
	if (fieldA == 62) {			\
	  fieldAisReg = 0;			\
	  fieldA = 0;				\
	}					\
	}

#define FIELD_B() {				\
	fieldB = (FIELDB(state->words[0]) << 3);\
	fieldB |= FIELDb(state->words[0]);	\
	if (fieldB == 62) {			\
	  fieldBisReg = 0;			\
	  fieldB = 0;				\
	}					\
	}

#define FIELD_C() {				\
	fieldC = FIELDC(state->words[0]);	\
	if (fieldC == 62) {  			\
	  fieldCisReg = 0; 			\
	}					\
	}
/********** Aurora SIMD ARC 8 - bit constant **********/
#define FIELD_U8() {                            \
						\
	  fieldC  = BITS(state->words[0],15,16);\
	  fieldC  = fieldC <<6;                 \
	  fieldC |= FIELDC(state->words[0]);    \
	  fieldCisReg = 0;                      \
	}

#define CHECK_FIELD_B() {			\
	fieldB = (FIELDB(state->words[0]) << 3);\
	fieldB |= FIELDb(state->words[0]);	\
	CHECK_FIELD(fieldB);			\
	}

#define CHECK_FIELD_C() {			\
	fieldC = FIELDC(state->words[0]);	\
	CHECK_FIELD(fieldC);			\
	}

#define FIELD_C_S() {				\
	fieldC_S = (FIELDC_S(state->words[0]) << 3);	\
	}

#define FIELD_B_S() {				\
	fieldB_S = (FIELDB_S(state->words[0]) << 3);	\
	}

#define CHECK_FIELD_H_AC() {			\
	fieldC = ((FIELDA_AC(state->words[0])) << 3);	\
	fieldC |= FIELDC_AC(state->words[0]);	\
	CHECK_FIELD(fieldC);			\
	}

#define CHECK_FIELD_H_EM() {			\
	fieldC = ((FIELDA_AC(state->words[0]) & 0x03) << 3);	\
	fieldC |= FIELDC_AC(state->words[0]);	\
	if (fieldC == 0x1E)			\
	  {					\
	    is_limm++;				\
	    fieldCisReg = 0;			\
	    PUT_NEXT_WORD_IN(fieldC);		\
	    limm_value = fieldC;		\
	  }					\
       }

#define FIELD_H_AC() {				\
	fieldC = ((FIELDA_AC(state->words[0])) << 3);	\
	fieldC |= FIELDC_AC(state->words[0]);	\
	if (fieldC > 60) {  			\
	  fieldCisReg = 0; 			\
	  fieldC = 0;				\
	}					\
	}

#define FIELD_C_AC() {				\
	fieldC = FIELDC_AC(state->words[0]);	\
	if (fieldC > 3) {  			\
	  fieldC += 8;  			\
	}				  	\
	}

#ifdef ARC_NPS_CMDS
#define FIELD_C_AC16() {				\
	fieldC = FIELDC_AC16(state->words[0]);	\
	if (fieldC > 3) {  			\
	  fieldC += 8;  			\
	}				  	\
	}
#endif // #ifdef ARC_NPS_CMDS

#define FIELD_B_AC() {				\
	fieldB = FIELDB_AC(state->words[0]);	\
	if (fieldB > 3) {  			\
	  fieldB += 8; 				\
	}				  	\
	}

#ifdef ARC_NPS_CMDS
#define FIELD_B_AC16() {				\
	fieldB = FIELDB_AC16(state->words[0]);	\
	if (fieldB > 3) {  			\
	  fieldB += 8; 				\
	}				  	\
	}
#endif // #ifdef ARC_NPS_CMDS

#define FIELD_A_AC() {				\
	fieldA = FIELDA_AC(state->words[0]);	\
	if (fieldA > 3) {  			\
	  fieldA += 8; 				\
	}				  	\
	}

/****************** <.T> syntax **************************/
#define CHECK_T(x) {					\
    if (BITS(state->words[0],3,3) == (x))		\
      {							\
	  if (((int) FIELDS9(state->words[0]))> 0)	\
	    {						\
	      branchPrediction = 1;			\
	    }						\
	  else						\
	    {						\
	      branchPrediction = 2;			\
	    }						\
      }							\
    }

#define IS_SMALL(x) (((field##x) < 256) && ((field##x) > -257))
#define IS_REG(x)   (field##x##isReg)
#define IS_SIMD_128_REG(x)  (usesSimdReg##x == 1)
#define IS_SIMD_16_REG(x)   (usesSimdReg##x == 2)
#define IS_SIMD_DATA_REG(x) (usesSimdReg##x == 3)
#define WRITE_FORMAT_LB_Rx_RB(x)     WRITE_FORMAT(x,"[","]","","")
#define WRITE_FORMAT_x_COMMA_LB(x)   WRITE_FORMAT(x,"",",[","",",[")
#define WRITE_FORMAT_COMMA_x_RB(x)   WRITE_FORMAT(x,",","]",",","]")
#define WRITE_FORMAT_x_RB(x)         WRITE_FORMAT(x,"","]","","]")
#define WRITE_FORMAT_COMMA_x(x)      WRITE_FORMAT(x,",","",",","")
#define WRITE_FORMAT_x_COMMA(x)      WRITE_FORMAT(x,"",",","",",")
#define WRITE_FORMAT_x(x)            WRITE_FORMAT(x,"","","","")
#define WRITE_FORMAT(x,cb1,ca1,cb,ca) strcat(formatString,              \
				     (IS_SIMD_128_REG(x) ? cb1"%S"ca1:  \
				      IS_SIMD_16_REG(x)  ? cb1"%I"ca1:  \
				      IS_SIMD_DATA_REG(x)? cb1"%D"ca1:  \
				      IS_REG(x)          ? cb1"%r"ca1:  \
				      usesAuxReg         ?  cb"%a"ca :  \
				      IS_SMALL(x) ? cb"%d"ca : cb"%h"ca))

#define WRITE_FORMAT_LB() strcat(formatString, "[")
#define WRITE_FORMAT_RB() strcat(formatString, "]")
#define WRITE_COMMENT(str)	(state->comm[state->commNum++] = (str))
#define WRITE_NOP_COMMENT() if (!fieldAisReg && !flag) WRITE_COMMENT("nop");

#define NEXT_WORD(x) (offset += 4, state->words[x])

#define NEXT_WORD_AC(x) (offset += 2, state->words[x])

#define add_target(x) 	(state->targets[state->tcnt++] = (x))

static char comment_prefix[] ATTRIBUTE_UNUSED = "\t; ";
static short int enable_simd = 0;
static short int enable_insn_stream = 0;


static const char *
core_reg_name(struct arcDisState *state, int val)
{
  if (state->coreRegName)
    return (*state->coreRegName)(state->_this, val);
  return 0;
}

static const char *
aux_reg_name(struct arcDisState *state, int val)
{
  if (state->auxRegName)
    return (*state->auxRegName)(state->_this, val);
  return 0;
}

static const char *
cond_code_name(struct arcDisState *state, int val)
{
  if (state->condCodeName)
    return (*state->condCodeName)(state->_this, val);
  return 0;
}

static const char *
instruction_name(struct arcDisState *state, int op1, int op2, int *flags)
{
  if (state->instName)
    return (*state->instName)(state->_this, op1, op2, flags);
  return 0;
}

static void
mwerror(struct arcDisState *state, const char *msg)
{
  if (state->err != 0)
    (*state->err)(state->_this, (msg));
}

static const char *
post_address(struct arcDisState *state, int addr)
{
  static char id[3*_NELEM(state->addresses)];
  unsigned int j, i = state->acnt;
  if (i < _NELEM(state->addresses)) {
    state->addresses[i] = addr;
    ++state->acnt;
    j = i*3;
    id[j+0] = '@';
    id[j+1] = '0'+i;
    id[j+2] = 0;
    return id+j;
  }
  return "";
}

static void
my_sprintf (struct arcDisState *state, char *buf, const char*format, ...)
{
  char *bp;
  const char *p;
  int size, leading_zero, regMap[2];
  va_list ap;

  va_start(ap,format);
  bp = buf;
  *bp = 0;
  p = format;
  regMap[0] = 0;
  regMap[1] = 0;
  while (1)
    switch(*p++) {
    case 0: goto DOCOMM; /*(return) */
    default:
      *bp++ = p[-1];
      break;
    case '%':
      size = 0;
      leading_zero = 0;
    RETRY: ;
      switch(*p++)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	  {
	    /* size. */
	    size = p[-1]-'0';
	    if (size == 0) leading_zero = 1; /* e.g. %08x */
	    while (*p >= '0' && *p <= '9')
	      size = size*10+*p-'0', p++;
	    goto RETRY;
	  }
#define inc_bp() bp = bp+strlen(bp)

	case 'h':
	  {
	    unsigned u = va_arg(ap,int);
	    /*
	     * Hex.  We can change the format to 0x%08x in
	     * one place, here, if we wish.
	     * We add underscores for easy reading.
	     */
#define CDT_DEBUG
	    if (u > 65536)
#ifndef CDT_DEBUG
	      sprintf(bp,"0x%x_%04x",u >> 16, u & 0xffff);
#else
	      sprintf(bp,"0x%08x",u);
#endif // CDT_DEBUG
	    else
	      sprintf(bp,"0x%x",u);
	    inc_bp();
	  }
	  break;
	case 'X': case 'x':
	  {
	    int val = va_arg(ap,int);
	    if (size != 0)
	      if (leading_zero) sprintf(bp,"%0*x",size,val);
	      else sprintf(bp,"%*x",size,val);
	    else sprintf(bp,"%x",val);
	    inc_bp();
	  }
	  break;
	case 'd':
	  {
	    int val = va_arg(ap,int);
	    if (size != 0) sprintf(bp,"%*d",size,val);
	    else sprintf(bp,"%d",val);
	    inc_bp();
	  }
	  break;
	case 'r':
	  {
	    /* Register. */
	    int val = va_arg(ap,int);

#define REG2NAME(num, name) case num: sprintf(bp,""name); \
			regMap[(num<32)?0:1] |= 1<<(num-((num<32)?0:32)); break;
	    switch (val)
	      {
		REG2NAME(26, "gp");
		REG2NAME(27, "fp");
		REG2NAME(28, "sp");
		REG2NAME(29, "ilink1");
		REG2NAME(30, "ilink2");
		REG2NAME(31, "blink");
		REG2NAME(60, "lp_count");
		REG2NAME(63, "pcl");
	      default:
		{
		  const char *ext;
		  ext = core_reg_name(state, val);
		  if (ext) sprintf(bp, "%s", ext);
		  else     sprintf(bp,"r%d",val);
		}break;
	      }
	    inc_bp();
	  } break;

	case 'a':
	  {
	  /* Aux Register. */
	    int val = va_arg(ap,int);
	    char *ret;
	    ret = arc_aux_reg_name(val);
	    if(ret)
	      sprintf(bp,"%s",ret);
	    else
	      {
		const char *ext;
		  ext = aux_reg_name(state, val);
		  if (ext) sprintf(bp, "%s", ext);
		  else     my_sprintf(state, bp,"%h",val);
	      }

	    inc_bp();
	  }
	  break;
	case 's':
	  {
	    sprintf(bp,"%s",va_arg(ap,char*));
	    inc_bp();
	  }
	  break;
	case '*':
	  {
#if 0
	    va_arg(ap,char*);
	    inc_bp();
	    break;
#elif 1 /* Used for prefetch to skip an argument.  */
	    va_arg(ap,int);
	    break;
#else
	    extern void abort (void);

	    abort ();
#endif
	  }

	  /* SIMD operands follow*/
	case 'S':
	  {
	    int val = va_arg (ap,int);

	    sprintf (bp, "vr%d",val);
	    inc_bp ();
	    break;
	  }
	case 'I':
	  {
	    int val = va_arg (ap,int);

	    sprintf (bp, "i%d",val);
	    inc_bp ();
	    break;
	  }
	case 'D':
	  {
	    int val = va_arg (ap,int);

	    sprintf (bp, "dr%d",val);
	    inc_bp ();
	    break;
	  }
	  /* SIMD operands end */
	default:
	  fprintf(stderr,"?? format %c\n",p[-1]);
	  break;
	}
    }


 DOCOMM: *bp = 0;

}

static void
write_comments_(struct arcDisState *state, int shimm ATTRIBUTE_UNUSED,
		int is_limm ATTRIBUTE_UNUSED, long limm_value ATTRIBUTE_UNUSED)
{
  assert (state->commentBuffer == 0);
  /*if (state->commentBuffer != 0)
    {
      int i;
      if (is_limm)
	{
	  const char *name = post_address(state, limm_value+shimm);
	  if (*name != 0) WRITE_COMMENT(name);
	}
      for(i = 0; i < state->commNum; i++)
	{
	  if (i == 0) strcpy(state->commentBuffer, comment_prefix);
	  else  strcat(state->commentBuffer, ", ");
	  strncat(state->commentBuffer, state->comm[i], sizeof(state->commentBuffer));
	}
	}*/
}

#define write_comments2(x)	write_comments_(state, x, is_limm, limm_value)
#define write_comments()	write_comments2(0)

static const char *condName[] =
{
#ifdef ARC_NPS_CMDS
  /* 0..31. */
  ""    , "z"  , "nz" , "p"  , "n"  , "c"  , "nc" , "v"  ,
  "nv"  , "gt" , "ge" , "lt" , "le" , "hi" , "ls" , "pnz",
  "ss"  , "sc" , "lb",  "lb2", "olb", "nj" , "at" , "nm",
  "nt" , "?25", "?26", "?27", "?28", "?29", "?30", "?31",
#else	// #ifdef ARC_NPS_CMDS
  /* 0..15. */
  ""   , "z"  , "nz" , "p"  , "n"  , "c"  , "nc" , "v"  ,
  "nv" , "gt" , "ge" , "lt" , "le" , "hi" , "ls" , "pnz",
  "ss" , "sc"
#endif // #ifdef ARC_NPS_CMDS

};

static void
write_instr_name_(struct arcDisState *state,
		  const char *instrName,
		  int cond,
		  int condCodeIsPartOfName,
		  int flag,
		  int signExtend,
		  int addrWriteBack,
		  int directMem,
		  int branchPrediction
		  )
{
  strcpy(state->instrBuffer, instrName);
  if (cond > 0)
    {
      int condlim = 0; /* condition code limit*/
      const char *cc = 0;
      if (!condCodeIsPartOfName) strcat(state->instrBuffer, ".");
#ifdef ARC_NPS_CMDS
      condlim = 32;
#else // #ifdef ARC_NPS_CMDS
      condlim = 18;
#endif // #ifdef ARC_NPS_CMDS
      if (cond < condlim)
	cc = condName[cond];
      else
	cc = cond_code_name(state, cond);
      if (!cc) cc = "???";
      strcat(state->instrBuffer, cc);
    }
#ifdef ARC_NPS_CMDS
  if ( !strcmp(instrName,"crc16") || !strcmp(instrName,"crc32") )
    {
      if (flag) strcat(state->instrBuffer, ".r");
    }
  else if ( !strcmp(instrName,"zncv") )
         if (flag) strcat(state->instrBuffer, ".wr");
         else strcat(state->instrBuffer, ".rd");
  else
#endif
  if (flag) strcat(state->instrBuffer, ".f");
  if (branchPrediction == 1)
    strcat(state->instrBuffer, ".t");
  if (branchPrediction == 2)
    strcat(state->instrBuffer, ".nt");

  if (state->nullifyMode)
    if (strstr(state->instrBuffer, ".d") == NULL)
      strcat(state->instrBuffer, ".d");
  if (signExtend)    strcat(state->instrBuffer, ".x");
  switch (addrWriteBack)
  {
    case 1: strcat(state->instrBuffer, ".a"); break;
    case 2: strcat(state->instrBuffer, ".ab"); break;
    case 3: strcat(state->instrBuffer, ".as"); break;
  }
  if (directMem)     strcat(state->instrBuffer, ".di");
}

#define write_instr_name()	{\
    write_instr_name_(state, instrName,cond, condCodeIsPartOfName, flag, signExtend, addrWriteBack, directMem, branchPrediction); \
 formatString[0] = '\0'; \
}

enum
{
  op_BC = 0, op_BLC = 1, op_LD  = 2, op_ST = 3, op_MAJOR_4  = 4,
  /* START ARC LOCAL */
#ifdef ARC_NPS_CMDS
  op_MAJOR_5 = 5, op_MAJOR_6 = 6, op_USER_7 = 7, op_MAJOR_8 = 8, op_USER_9 = 9, op_USER_10 = 10,
  op_USER_11 = 11, op_LD_ADD = 12, op_ADD_SUB_SHIFT  = 13,
#else
  op_MAJOR_5 = 5, op_MAJOR_6 = 6, op_MAJOR_8 = 8, op_SIMD=9, op_LD_ST = 10,
  op_JLI_EI = 11, op_LD_ADD = 12, op_ADD_SUB_SHIFT  = 13,
#endif  // #ifdef ARC_NPS_CMDS
  /* END ARC LOCAL */
  op_ADD_MOV_CMP = 14, op_S = 15, op_LD_S = 16, op_LDB_S = 17,
  op_LDW_S = 18, op_LDWX_S  = 19, op_ST_S = 20, op_STB_S = 21,
  op_STW_S = 22, op_Su5     = 23, op_SP   = 24, op_GP    = 25, op_Pcl = 26,
  op_MOV_S = 27, op_ADD_CMP = 28, op_BR_S = 29, op_B_S   = 30, op_BL_S = 31
};

extern disassemble_info tm_print_insn_info;

/*
 * bfd_getm32 - To retrieve the upper 16-bits of the ARCtangent-A5
 *              basecase (32-bit) instruction
 */
static bfd_vma
bfd_getm32 (data)
     unsigned int data;
{
   bfd_vma value = 0;

   value = ((data & 0xff00) | (data & 0xff)) << 16;
   value |= ((data & 0xff0000) | (data & 0xff000000)) >> 16;
   return value;
}

/*
 * bfd_getm32_ac - To retrieve the upper 8-bits of the ARCompact
 *                 16-bit instruction
 */
static bfd_vma
bfd_getm32_ac (data)
     unsigned int data;
{
   bfd_vma value = 0;

   value = ((data & 0xff) << 8 | (data & 0xff00) >> 8);
   return value;
}

/*
 * sign_extend - Sign Extend the value
 *
 */
static int
sign_extend (int value, int bits)
{
  if (BIT(value, (bits-1)))
    value |= (0xffffffff << bits);
  return value;
}

#ifdef ARC_NPS_CMDS
static void checkAdd_e(int type,char *str)
{
	switch ( type ) {
		case  3:
		case  7:
		case 12:
		case 13:
		case 14:
		case 15:
			strcat(str,".e");
			break;
		default:
			break;
	}
	return;
}
#endif // #ifdef ARC_NPS_CMDS

/* dsmOneArcInst - This module is used to identify the instruction
 *		   and to decode them based on the ARCtangent-A5
 *                 instruction set architecture.
 *                 First, the major opcode is computed. Based on the
 *		   major opcode and sub opcode, the instruction is
 * 		   identified. The appropriate decoding class is assigned
 *		   based on the instruction.Further subopcode 2 is used in
 *                 cases where decoding upto subopcode1 is not possible.
 *
 *		   The instruction is then decoded accordingly.
 */
static int
dsmOneArcInst (bfd_vma addr, struct arcDisState *state, disassemble_info * info)
{

  int subopcode, mul;
  int condCodeIsPartOfName=0;
  int decodingClass;
  const char *instrName;
  int fieldAisReg=1, fieldBisReg=1, fieldCisReg=1;
  int fieldA=0, fieldB=0, fieldC=0;
  int flag=0, cond=0, is_shimm=0, is_limm=0;
  long limm_value=0;
  int signExtend=0, addrWriteBack=0, directMem=0;
  int branchPrediction = 0;
  int is_linked=0;
  int offset=0;
  int usesAuxReg = 0;
  int usesSimdRegA= 0, usesSimdRegB=0, usesSimdRegC=0,simd_scale_u8=-1;
  int flags = !E_ARC_MACH_A4;
  char formatString[60];
#ifdef ARC_NPS_CMDS
  int  ldbitCmd = 0;
#endif // #ifdef ARC_NPS_CMDS

  state->nullifyMode = BR_exec_when_no_jump;
  state->isBranch = 0;

  state->_mem_load = 0;
  state->_ea_present = 0;
  state->_load_len = 0;
  state->ea_reg1 = no_reg;
  state->ea_reg2 = no_reg;
  state->_offset = 0;

  state->sourceType = ARC_UNDEFINED;

  /* ARCtangent-A5 basecase instruction and little-endian mode */
  if ((info->endian == BFD_ENDIAN_LITTLE) && (state->instructionLen == 4))
    state->words[0] = bfd_getm32(state->words[0]);

  if (state->instructionLen == 4)
  {
    if (!NEXT_WORD(0))
      return 0;
    /* Get the major opcode of the ARCtangent-A5 32-bit instruction. */
    state->_opcode = OPCODE(state->words[0]);
  }
  else
  {
    /* ARCompact 16-bit instruction */
    if (!NEXT_WORD_AC(0))
      return 0;
    /* Get the major opcode of the ARCompact 16-bit instruction. */
    state->_opcode = OPCODE_AC(state->words[0]);
  }

  instrName = 0;
  decodingClass = 0; /* default! */
  mul = 0;
  condCodeIsPartOfName=0;
  state->commNum = 0;
  state->tcnt = 0;
  state->acnt = 0;
  state->flow = noflow;

  if (state->commentBuffer)
    state->commentBuffer[0] = '\0';

  /* Find the match for the opcode. Once the major opcode category is
   * identified, get the subopcode to determine the exact instruction.
   * Based on the instruction identified, select the decoding class.
   * If condition code is part of the instruction name, then set the
   * flag 'condCodeIsPartOfName'.
   * For branch, jump instructions set 'isBranch' (state->isBranch).
   */

  switch (state->_opcode)
  {
    case op_BC:
    /* Branch Conditionally */
      instrName = "b";
      decodingClass = 13;
      condCodeIsPartOfName = 1;
      state->isBranch = 1;
      break;

    case op_BLC:
    /* Branch and Link, Compare and Branch  */
      decodingClass = 9;
      state->isBranch = 1;
      switch (BITS(state->words[0],16,16))
      {
	case 0:
	  if (!instrName)
	    instrName = "bl";
	  decodingClass = 13;
	  condCodeIsPartOfName = 1;
	  break;
	case 1:
	  switch (BITS(state->words[0],0,2))
	  {
	    case 0: instrName = "breq"; break;
	    case 1: instrName = "brne"; break;
	    case 2: instrName = "brlt"; break;
	    case 3: instrName = "brge"; break;
	    case 4: instrName = "brlo"; break;
	    case 5: instrName = "brhs"; break;
	    case 6: instrName = "bbit0"; break;
	    case 7: instrName = "bbit1"; break;
	    default:
	      instrName = "??? (0[3])";
	      state->flow = invalid_instr;
	      break;
	  }
	  break;
	default:
	  instrName = "??? (0[3])";
	  state->flow = invalid_instr;
	  break;
      }
      break;

    case op_LD:
    /* Load register with offset [major opcode 2]  */
      decodingClass = 6;
#ifdef ARC_NPS_CMDS
      ldbitCmd = 0;
#endif // #ifdef ARC_NPS_CMDS
      switch (BITS(state->words[0],7,8))
      {
	case 0: instrName  = "ld";  state->_load_len = 4; break;
	case 1: instrName  = "ldb"; state->_load_len = 1; break;
	case 2:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "ldh";
	  else
	    instrName  = "ldw";
	  state->_load_len = 2;
	  break;
#ifdef ARC_NPS_CMDS
	case 3: instrName  = "ldbit.di";  ldbitCmd = 1; state->_load_len = 4; break;
#else
	case 3: instrName  = "ldd";  state->_load_len = 4; break;
#endif // #ifdef ARC_NPS_CMDS
	default:
	  instrName = "??? (0[3])";
	  state->flow = invalid_instr;
	  break;
      }
      break;

    case op_ST:
    /* Store register with offset [major opcode 0x03] */
      decodingClass = 7;
      switch (BITS(state->words[0],1,2))
      {
	case 0: instrName = "st";  break;
	case 1: instrName = "stb"; break;
	case 2:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "sth";
	  else
	    instrName = "stw";
	  break;
	case 3: instrName = "std";  break;
	default:
	  instrName = "??? (2[3])";
	  state->flow = invalid_instr;
	  break;
      }
      break;

    case op_MAJOR_4:
    /* ARC 32-bit basecase instructions with 3 Operands */
      decodingClass = 0;  /* Default for 3 operand instructions */
      subopcode = BITS(state->words[0],16,21);
      switch (subopcode)
	{
	case 0: instrName = "add";  break;
	case 1: instrName = "adc";  break;
	case 2: instrName = "sub";  break;
	case 3: instrName = "sbc";  break;
	case 4: instrName = "and";  break;
	case 5: instrName = "or";   break;
	case 6: instrName = "bic";  break;
	case 7: instrName = "xor";  break;
	case 8: instrName = "max";  break;
	case 9: instrName = "min";  break;
	case 10:
	  {
	    if(state->words[0] == 0x264a7000)
	      {
		instrName = "nop";
		decodingClass = 26;
	      }
	    else
	      {
		instrName = "mov";
		decodingClass = 12;
	      }
	    break;
	  }
	case 11: instrName = "tst"; decodingClass = 2; break;
	case 12: instrName = "cmp"; decodingClass = 2; break;
	case 13: instrName = "rcmp"; decodingClass = 2; break;
	case 14: instrName = "rsub"; break;
	case 15: instrName = "bset"; break;
	case 16: instrName = "bclr"; break;
	case 17: instrName = "btst"; decodingClass = 2; break;
	case 18: instrName = "bxor"; break;
	case 19: instrName = "bmsk"; break;
	case 20: instrName = "add1"; break;
	case 21: instrName = "add2"; break;
	case 22: instrName = "add3"; break;
	case 23: instrName = "sub1"; break;
	case 24: instrName = "sub2"; break;
	case 25: instrName = "sub3"; break;
	case 26: instrName = "mpy"; break;
	case 27:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "mpym";
	  else
	    instrName = "mpyh";
	  break;
	case 28:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "mpymu";
	  else
	    instrName = "mpyhu";
	  break;
	case 29: instrName = "mpyu"; break;
	case 30: instrName = "mpyw"; break;
	case 31: instrName = "mpyuw"; break;
	case 32:
	case 33:
	  instrName = "j";
	case 34:
	case 35:
	  if (!instrName) instrName = "jl";
	  decodingClass = 4;
	  condCodeIsPartOfName = 1;
	  state->isBranch = 1;
	  break;
	case 36:
	  instrName = "bi";
	  decodingClass = 46;
	  break;
	case 37:
	  instrName = "bih";
	  decodingClass = 46;
	  break;
	case 38:
	  instrName = "ldi";
	  decodingClass = 47;
	  break;
	case 39:
	  decodingClass = 45;
	  instrName = "aex";
	  break;
	case 40:
	  instrName = "lp";
	  decodingClass = 11;
	  condCodeIsPartOfName = 1;
	  state->isBranch = 1;
	  break;
	case 41:
	  if (BITS(state->words[0],15,15))
	    instrName = "kflag";
	  else
	    instrName = "flag";
	  decodingClass = 3;
	  break;
	case 42: instrName = "lr"; decodingClass = 10;  break;
	case 43: instrName = "sr"; decodingClass =  8;  break;
	case 44: instrName = "bmskn"; break;
	case 0x2D: instrName = "xbfu"; break;
	case 47:
	  decodingClass = 1;
	  switch (BITS(state->words[0],0,5)) /* Checking based on Subopcode2 */
	    {
	    case 0: instrName = "asl";  break;
	    case 1: instrName = "asr";  break;
	    case 2: instrName = "lsr";  break;
	    case 3: instrName = "ror";  break;
	    case 4: instrName = "rrc";  break;
	    case 5: instrName = "sexb"; break;
	    case 6:
	      if(info->mach == bfd_mach_arc_arcv2)
		instrName = "sexh";
	      else
		instrName = "sexw";
	      break;
	    case 7: instrName = "extb"; break;
	    case 8:
	       if(info->mach == bfd_mach_arc_arcv2)
		 instrName = "exth";
	       else
		 instrName = "extw";
	       break;
	    case 9: instrName = "abs";  break;
	    case 10: instrName = "not"; break;
	    case 11: instrName = "rlc"; break;
	    case 12:  instrName = "ex";
	      decodingClass = 34;
	      break; // ramana adds

	    case 13: instrName = "rol"; break;
	      /* START ARC LOCAL */
	    case 16: instrName = "llock"; decodingClass = 34; break;
	    case 17: instrName = "scond"; decodingClass = 34; break;
	      /* END ARC LOCAL */

	    case 63:
	      decodingClass = 26;
	      switch (BITS(state->words[0],24,26))
		{
		case 1 : instrName = "sleep"; decodingClass = 32; break;
		case 2 :
		  if((info->mach) == ARC_MACH_ARC7)
		    instrName = "trap0";
		  else
		    {
		       if (BITS(state->words[0],12,13))
			 instrName = "dsync";
		       else
			 instrName = "swi";
                    }
		  break;
		case 3:
		  if(!BITS(state->words[0],12,14))
		    instrName = "sync" ;
		  else
		    instrName = "dmb"; decodingClass = 32;
		  break;
		case 4 : instrName = "rtie" ; break;
		case 5 : instrName = "brk"; break;
		case 6 : instrName = "seti"; decodingClass = 32; break;
		case 7 : instrName = "clri"; decodingClass = 32; break;
		default:

		  instrName = "???";
		  state->flow=invalid_instr;
		  break;
		}
	      break;
	    }
	  break;
	case 0x38: instrName = "seteq"; break;
	case 0x39: instrName = "setne"; break;
	case 0x3A: instrName = "setlt"; break;
	case 0x3B: instrName = "setge"; break;
	case 0x3C: instrName = "setlo"; break;
	case 0x3D: instrName = "seths"; break;
	case 0x3E: instrName = "setle"; break;
	case 0x3F: instrName = "setgt"; break;
   }

      if (!instrName)
      {
	subopcode = BITS(state->words[0],17,21);
	decodingClass = 5;
#ifdef ARC_NPS_CMDS
	ldbitCmd = 0;
#endif // #ifdef ARC_NPS_CMDS
	switch (subopcode)
	{
	  case 24: instrName  = "ld";   state->_load_len = 4; break;
	  case 25: instrName  = "ldb";  state->_load_len = 1; break;
	  case 26: instrName  = "ldw";  state->_load_len = 2; break;
#ifdef ARC_NPS_CMDS
	  case 27: instrName  = "ldbit.di"; ldbitCmd = 1; state->_load_len = 4; break;
#else
	  case 27: instrName  = "ldd";   state->_load_len = 4; break;
#endif // #ifdef ARC_NPS_CMDS
	  default:
	    instrName = "??? (0[3])";
	    state->flow = invalid_instr;
	    break;
	}
      }
      break;

    case op_MAJOR_5:
    /* ARC 32-bit extension instructions */
      decodingClass = 0;  /* Default for Major opcode 5 ... */
      subopcode = BITS(state->words[0],16,21);
      switch (subopcode)
	{
	case 0: instrName = "asl"; break;
	case 1: instrName = "lsr"; break;
	case 2: instrName = "asr"; break;
	case 3: instrName = "ror"; break;
	case 4:
	  if(info->mach == bfd_mach_arc_arcv2)
	    {
	      instrName = "div";
	    }
	  else
	    {
	      instrName = "mul64";
	      mul =1;
	      decodingClass = 2;
	    }
	  break;
	case 5:
	  if((info->mach) == bfd_mach_arc_arcv2)
	    {
	      instrName = "divu";
	    }
	  else
	    {
	      instrName = "mulu64";
	      mul =1;
	      decodingClass = 2;
	    }
	  break;

	  /* ARC A700 */
	case 6: instrName = "adds" ;break; /*Illegal for EM */
	case 7: instrName = "subs"; break; /*Illegal for EM */

	case 8:
	  if((info->mach) == bfd_mach_arc_arcv2)
	    {
	      instrName = "rem";
	    }
	  else
	    {
	      instrName = "divaw";
	    }
	  break;
	case 9: instrName = "remu"; break;
	case 0xA: instrName = "asls"; break; /*Illegal for EM */
	case 0xB: instrName = "asrs"; break; /*Illegal for EM */

	  /* HS */
	case 0x0E: instrName = "mac"; break;
	case 0x0F: instrName = "macu"; break;
	case 0x10: instrName = "dmpyh"; break;
	case 0x11: instrName = "dmpyhu"; break;
	case 0x12: instrName = "dmach"; break;
	case 0x13: instrName = "dmachu"; break;
	case 0x14: instrName = "vadd2h"; break;
	case 0x15: instrName = "vsub2h"; break;
	case 0x16: instrName = "vaddsub2h"; break;
	case 0x17: instrName = "vsubadd2h"; break;
	case 0x18: instrName = "mpyd"; break;
	case 0x19: instrName = "mpydu"; break;
	case 0x1A: instrName = "macd"; break;
	case 0x1B: instrName = "macdu"; break;
	case 0x1C: instrName = "vmpy2h"; break;
	case 0x1D: instrName = "vmpy2hu"; break;
	case 0x1E: instrName = "vmac2h"; break;
	case 0x1F: instrName = "vmac2hu"; break;

	  /* A700's */
	case 0x28: instrName = "addsdw";break;
	case 0x29: instrName = "subsdw"; break;

	case 0x2F:
	  decodingClass = 1;
	  switch(BITS(state->words[0],0,5))
	    {
	    case 0: instrName = "swap"; decodingClass = 1; break;
	    case 1: instrName = "norm"; decodingClass = 1; break;
	      /* ARC A700 DSP Extensions */
	    case 2:instrName = "sat16"; decodingClass = 1; break;
	    case 3: instrName = "rnd16"; decodingClass = 1; break;
	    case 4: instrName = "abssw"; decodingClass = 1; break;
	    case 5: instrName = "abss"; decodingClass = 1; break;
	    case 6: instrName = "negsw"; decodingClass = 1; break;
	    case 7: instrName = "negs"; decodingClass = 1; break;

	    case 8:
	      if(info->mach == bfd_mach_arc_arcv2)
		instrName = "normh";
	      else
		instrName = "normw";
	      decodingClass = 1;
	      break;

	      /* START ARC LOCAL */
	    case 9: instrName = "swape"; decodingClass = 1; break;
	      /* END ARC LOCAL */
	    case 0x0A: instrName = "lsl16"; break;
	    case 0x0B: instrName = "lsr16"; break;
	    case 0x0C: instrName = "asr16"; break;
	    case 0x0D: instrName = "asr8"; break;
	    case 0x0E: instrName = "lsr8"; break;
	    case 0x0F: instrName = "lsl8"; break;
	    case 0x10: instrName = "rol8"; break;
	    case 0x11: instrName = "ror8"; break;
	    case 0x12: instrName = "ffs"; break;
	    case 0x13: instrName = "fls"; break;

	    default:
	      instrName = "???";
	      state->flow =invalid_instr;
	      break;
	    }
	  break;

	  /* HS */
	case 0x30: instrName = "qmpyh"; break;
	case 0x31: instrName = "qmpyhu"; break;
	case 0x32: instrName = "qmpywh"; break;
	case 0x33: instrName = "qmpywhu"; break;
	case 0x34: instrName = "qmach"; break;
	case 0x35: instrName = "qmachu"; break;
	case 0x36: instrName = "dmacwh"; break;
	case 0x37: instrName = "dmacwhu"; break;
	case 0x38: instrName = "vadd4h"; break;
	case 0x39: instrName = "vsub4h"; break;
	case 0x3A: instrName = "vaddsub4h"; break;
	case 0x3B: instrName = "vsubadd4h"; break;
	case 0x3C: instrName = "vadd2"; break;
	case 0x3D: instrName = "vsub2"; break;
	case 0x3E: instrName = "vaddsub"; break;
	case 0x3F: instrName = "vsubadd"; break;

	default:
	  instrName = "??? (2[3])";
	  state->flow = invalid_instr;
	  break;
	}
      break;

  /* START ARC LOCAL */
  case op_MAJOR_6:
      decodingClass = 0;
      subopcode = BITS(state->words[0],16,21);
      switch (subopcode)
	{
	  /* HS */
	case 0x00: instrName = "fsmul"; break;
	case 0x01: instrName = "fsadd"; break;
	case 0x02: instrName = "fssub"; break;
	case 0x03: instrName = "fscmp"; decodingClass = 2; break;
	case 0x04: instrName = "fscmpf"; decodingClass = 2; break;
	case 0x05: instrName = "fsmadd"; break;
	case 0x06: instrName = "fsmsub"; break;
	case 0x07: instrName = "fsdiv"; break;
	case 0x08: instrName = "fcvt32"; break;
	case 0x09: instrName = "fcvt32_64"; break;

	case 0x30: instrName = "fdmul"; break;
	case 0x31: instrName = "fdadd"; break;
	case 0x32: instrName = "fdsub"; break;
	case 0x33: instrName = "fdcmp"; decodingClass = 2; break;
	case 0x34: instrName = "fdcmpf"; decodingClass = 2; break;
	case 0x35: instrName = "fdmadd"; break;
	case 0x36: instrName = "fdmsub"; break;
	case 0x37: instrName = "fddiv"; break;
	case 0x38: instrName = "fcvt64"; break;
	case 0x39: instrName = "fcvt64_32"; break;

	case 32: instrName = "pkqb"; break;
	case 33: instrName = "upkqb"; break;
	case 34: instrName = "xpkqb"; break;
	case 35: instrName = "avgqb"; break;
	case 36: instrName = "addqbs"; break;
	case 37: instrName = "mpyqb"; break;
	case 38: instrName = "fxtr"; break;
	case 39: instrName = "iaddr"; break;
	case 40: instrName = "acm"; break;
	case 41: instrName = "sfxtr"; break;
	case 42: instrName = "clamp"; break;
	case 43: instrName = "mpyu16"; break;
	case 44: instrName = "mpy16"; break;

	  /* HS */
	case 0x2F:
	  decodingClass = 1;
	  switch (BITS(state->words[0],0,5))
	    {
	    case 0x00: instrName = "fssqrt"; break;
	    case 0x01: instrName = "fdsqrt"; break;
	    case 0x1A: instrName = "rtsc"; decodingClass = 44; break;
	    default:
	      instrName = "???";
	      state->flow =invalid_instr;
	      break;
	    }
	  break;

	default:
	  instrName = "??? (2[3])";
	  state->flow = invalid_instr;
	  break;
	}
    break;

  case op_MAJOR_8:
    if (BIT(state->words[0],2))
      {
	instrName = "ld_s";
	decodingClass = 54;
      }
    else
      {
	instrName = "mov_s";
	decodingClass = 53;
      }
    break;
  /* END ARC LOCAL */

    /* Aurora SIMD instruction support*/
#ifndef ARC_NO_SIMD_CMDS
  case op_SIMD:

    if (enable_simd)
      {
	decodingClass = 42;
	subopcode     = BITS(state->words[0], 17, 23);

	switch (subopcode)
	  {

	  case 68:
	    instrName = "vld32";
	    decodingClass = 37;
	    usesSimdRegA=1;
	    usesSimdRegB=2;
	    usesSimdRegC=0;
	    simd_scale_u8 = 2;
	    break;

	  case 72:
	    instrName = "vld64";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 3;
	    break;

	  case 74:
	    instrName = "vld64w";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 3;
	    break;

	  case 70:
	    instrName = "vld32wl";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 2;
	    break;

	  case 66:
	    instrName = "vld32wh";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 2;
	    break;

	  case 76:
	    instrName = "vld128";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 4;
	    break;

	  case 78:
	    {
	      short  sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vld128r";
		  decodingClass = 38;
		  usesSimdRegA  = 1;
		  usesSimdRegB  = usesSimdRegC = 0;
		  break;
		default:
		  instrName = "SIMD";
		  state->flow = invalid_instr;
		}
	    }
	    break;
	  case 71:
	    instrName = "vst16_0";
	    decodingClass = 37;
	    usesSimdRegA = 1;
	    usesSimdRegB = 2;
	    usesSimdRegC = 0;
	    simd_scale_u8 = 1;
	    break;

	  case 81:
	    instrName = "vst16_1";
	    decodingClass = 37;
	    usesSimdRegA = 1;
	    usesSimdRegB = 2;
	    usesSimdRegC = 0;
	    simd_scale_u8 = 1;
	    break;

	  case 67:
	    instrName = "vst16_2";
	    decodingClass = 37;
	    usesSimdRegA = 1;
	    usesSimdRegB = 2;
	    usesSimdRegC = 0;
	    simd_scale_u8 = 1;
	    break;

	  case 75:
	    instrName = "vst16_3";
	    decodingClass = 37;
	    usesSimdRegA = 1;
	    usesSimdRegB = 2;
	    usesSimdRegC = 0;
	    simd_scale_u8 = 1;
	    break;

	  case 83:
	    instrName = "vst16_4";
	    decodingClass = 37;
	    usesSimdRegA = 1;
	    usesSimdRegB = 2;
	    usesSimdRegC = 0;
	    simd_scale_u8 = 1;
	    break;

	  case 89:
	    instrName = "vst16_5";
	    decodingClass = 37;
	    usesSimdRegA = 1;
	    usesSimdRegB = 2;
	    usesSimdRegC = 0;
	    simd_scale_u8 = 1;
	    break;

	  case 91:
	    instrName = "vst16_6";
	    decodingClass = 37;
	    usesSimdRegA = 1;
	    usesSimdRegB = 2;
	    usesSimdRegC = 0;
	    simd_scale_u8 = 1;
	    break;

	  case 93:
	    instrName = "vst16_7";
	    decodingClass = 37;
	    usesSimdRegA = 1;
	    usesSimdRegB = 2;
	    usesSimdRegC = 0;
	    simd_scale_u8 = 1;
	    break;

	  case 69:
	    instrName = "vst32_0";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 2;
	    break;

	  case 82:
	    instrName = "vst32_2";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 2;
	    break;

	  case 86:
	    instrName = "vst32_4";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 2;
	    break;

	  case 88:
	    instrName = "vst32_6";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 2;
	    break;

	  case 73:
	    instrName = "vst64";
	    decodingClass = 37 ;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 3;
	    break;

	  case 77:
	    instrName = "vst128";
	    decodingClass = 37;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = 2;
	    usesSimdRegC  = 0;
	    simd_scale_u8 = 4;
	    break;

	  case 79:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vst128r";
		  decodingClass = 38;
		  usesSimdRegA = 1;
		  usesSimdRegB = usesSimdRegC = 0;
		  break;

		default:
		  instrName = "SIMD";
		  state->flow = invalid_instr;
		}

	    }
	    break;
	  case 80:
	    instrName = "vmvw";
	    usesSimdRegA = usesSimdRegB = 1;
	    usesSimdRegC = 0;
	    decodingClass = 39;
	    break;

	  case 84:
	    instrName = "vmvzw";
	    decodingClass = 39;
	    usesSimdRegA = usesSimdRegB = 1;
	    usesSimdRegC = 0;
	    break;

	  case 90:
	    instrName = "vmovw";
	    decodingClass = 39;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = usesSimdRegC = 0;
	    break;

	  case 94:
	    instrName = "vmovzw";
	    decodingClass = 39;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = usesSimdRegC = 0;
	    break;

	  case 85:
	    instrName = "vmvaw";
	    decodingClass = 39;
	    usesSimdRegA  = usesSimdRegB = 1;
	    usesSimdRegC  = 0;
	    break;

	  case 95:
	    instrName = "vmovaw";
	    decodingClass = 39;
	    usesSimdRegA  = 1;
	    usesSimdRegB  = usesSimdRegC = 0;
	    break;

	  case 10:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vaddw"; decodingClass = 42;
		  usesSimdRegA = usesSimdRegB = usesSimdRegC =1;
		  break;

		case 1:
		  instrName = "vaddaw"; decodingClass = 42;
		  usesSimdRegA = usesSimdRegB = usesSimdRegC =1;
		  break;

		case 2:
		  instrName = "vbaddw"; decodingClass = 42;
		  usesSimdRegA = usesSimdRegB = 1;
		  usesSimdRegC = 0;
		  break;
		}
	      break;
	    }

	  case 11:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vsubw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;

		case 1:
		  instrName = "vsubaw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;

		case 2:
		  instrName = "vbsubw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		}
	    }
	    break;

	  case 12:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vmulw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;

		case 1:
		  instrName = "vmulaw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;

		case 2:
		  instrName = "vbmulw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;

		case 3:
		  instrName = "vbmulaw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		}
	    }
	    break;

	  case 13:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vmulfw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;

		case 1:
		  instrName = "vmulfaw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;

		case 2:
		  instrName = "vbmulfw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		}
	    }
	    break;

	  case 15:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vsummw";
		  decodingClass = 42;
		  usesSimdRegA = usesSimdRegB = usesSimdRegC = 1;
		  break;
		case 2:
		  instrName = "vbrsubw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		}
	    }
	    break;

	  case 23:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vmr7w";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;

		case 1:
		  instrName = "vmr7aw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;


		case 2:
		  switch (BITS(state->words[0], 0, 5))
		    {
		    case 0:
		      instrName = "vaddsuw";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;

		    case 1:
		      instrName = "vabsw";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;

		    case 2:
		      instrName = "vsignw";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;

		    case 3:
		      instrName = "vupbw";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;

		    case 4:
		      instrName = "vexch1";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;

		    case 5:
		      instrName = "vexch2";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;

		    case 6:
		      instrName = "vexch4";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;

		    case 7:
		      instrName = "vupsbw";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;

		    case 8:
		      instrName = "vdirun";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = usesSimdRegA = 0;
		      break;

		    case 9:
		      instrName = "vdorun";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = usesSimdRegA = 0;
		      break;

		    case 10:
		      instrName = "vdiwr";
		      decodingClass = 40;
		      usesSimdRegB  = 3;
		      usesSimdRegA  = usesSimdRegC = 0;
		      fieldCisReg   = 1;
		      break;

		    case 11:
		      instrName = "vdowr";
		      decodingClass = 40;
		      usesSimdRegB  = 3;
		      usesSimdRegA  = usesSimdRegC = 0;
		      fieldCisReg   = 1;
		      break;

		    case 12:
		      instrName = "vdird";
		      decodingClass = 40;
		      usesSimdRegB  = 1;
		      usesSimdRegC  = 3;
		      usesSimdRegA  = 0;
		      break;

		    case 13:
		      instrName = "vdord";
		      decodingClass = 40;
		      usesSimdRegB  = 1;
		      usesSimdRegC  = 3;
		      usesSimdRegA  = 0;
		      break;

		    case 63:
		      {
			switch (BITS(state->words[0], 24, 25))
			  {
			  case 0:
			    instrName = "vrec";
			    decodingClass = 43;
			    usesSimdRegC  = 0;
			    usesSimdRegB  = usesSimdRegA = 0;
			    break;

			  case 1:
			    instrName = "vrecrun";
			    decodingClass = 43;
			    usesSimdRegC  = 0;
			    usesSimdRegA  = usesSimdRegB = 0;
			    break;

			  case 2:
			    instrName = "vrun";
			    decodingClass = 43;
			    usesSimdRegC  = 0;
			    usesSimdRegB  = usesSimdRegA = 0;
			    break;

			  case 3:
			    instrName = "vendrec";
			    decodingClass = 43;
			    usesSimdRegC  = 0;
			    usesSimdRegB  = usesSimdRegA = 0;
			    break;
			  }
		      }
		      break;
		    }
		  break;

		case 3:
		  switch (BITS(state->words[0], 0, 2))
		    {
		    case 1:
		      instrName = "vabsaw";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;
		    case 3:
		      instrName = "vupbaw";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;
		    case 7:
		      instrName = "vupsbaw";
		      decodingClass = 40;
		      usesSimdRegC  = usesSimdRegB = 1;
		      usesSimdRegA  = 0;
		      break;
		    }
		  break;
		}
	    }
	    break;

	  case 16:
	    instrName = "vasrw";
	    decodingClass = 42;
	    usesSimdRegA  = usesSimdRegB = 1;
	    usesSimdRegC  = 2;
	    break;

	  case 48:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vasrwi";
		  decodingClass = 41;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		case 2:
		  instrName = "vasrrwi";
		  decodingClass = 41;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		}
	    }
	    break;

	  case 59:
	    instrName = "vasrsrwi";
	    decodingClass = 41;
	    usesSimdRegA  = usesSimdRegB = 1;
	    usesSimdRegC  = 0;
	    break;

	  case 18:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vmaxw";
		  usesSimdRegC = 1;
		  break;
		case 1:
		  instrName = "vmaxaw";
		  usesSimdRegC = 1;
		  break;
		case 2:
		  instrName = "vbmaxw";
		  usesSimdRegC = 0;
		  break;
		}
	      decodingClass = 42;
	      usesSimdRegA  = usesSimdRegB = 1;
	      break;
	    }

	  case 19:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vminw";
		  usesSimdRegC = 1;
		  break;
		case 1:
		  instrName = "vminaw";
		  usesSimdRegC = 0;
		  break;
		case 2:
		  instrName = "vbminw";
		  usesSimdRegC = 0;
		  break;
		}
	      decodingClass = 42;
	      usesSimdRegA  = usesSimdRegB = 1;
	      break;
	    }

	  case 14:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vdifw";
		  break;
		case 1:
		  instrName = "vdifaw";
		  break;
		case 2:
		  instrName = "vmrb";
		  break;
		}
	      decodingClass = 42;
	      usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	      break;
	    }

	  case 24:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vand";
		  decodingClass = 42;
		  usesSimdRegA = usesSimdRegB = usesSimdRegC = 1;
		  break;
		case 1:
		  instrName = "vandaw";
		  decodingClass = 42;
		  usesSimdRegA = usesSimdRegB = usesSimdRegC = 1;
		  break;
		}
	      break;
	    }

	  case 25:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vor";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
		  break;
		}
	      break;
	    }

	  case 26:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vxor";
		  break;
		case 1:
		  instrName = "vxoraw";
		  break;
		}
	      decodingClass = 42;
	      usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	      break;
	    }

	  case 27:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vbic";
		  break;
		case 1:
		  instrName = "vbicaw";
		  break;
		}
	      decodingClass = 42;
	      usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	      break;
	    }

	  case 4:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vavb";
		  break;
		case 2:
		  instrName = "vavrb";
		  break;
		}
	      decodingClass = 42;
	      usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	      break;
	    }

	  case 28:
	    instrName = "veqw";
	    decodingClass = 42;
	    usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	    break;

	  case 29:
	    instrName = "vnew";
	    decodingClass = 42;
	    usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	    break;

	  case 30:
	    instrName = "vlew";
	    decodingClass = 42;
	    usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	    break;

	  case 31:
	    instrName = "vltw";
	    decodingClass = 42;
	    usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	    break;

	  case 49:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vasrpwbi";
		  decodingClass = 41;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		case 2:
		  instrName = "vasrrpwbi";
		  decodingClass = 41;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		}
	      break;
	    }

	  case 5:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vsr8";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 2;
		  break;

		case 1:
		  instrName = "vsr8aw";
		  decodingClass = 42;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 2;
		  break;
		}
	      break;
	    }

	  case 37:
	    {
	      short sub_subopcode = BITS(state->words[0], 15, 16);
	      switch (sub_subopcode)
		{
		case 0:
		  instrName = "vsr8i";
		  decodingClass = 41;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;

		case 1:
		  instrName = "vsr8awi";
		  decodingClass = 41;
		  usesSimdRegA  = usesSimdRegB = 1;
		  usesSimdRegC  = 0;
		  break;
		}
	      break;
	    }

	  case 20:
	  case 21:
	  case 22:
	    {
	      short subopcode2 = BITS(state->words[0], 15, 18);
	      switch (subopcode2)
		{
		case 0:
		  instrName = "vmr1w";
		  break;

		case 2:
		  instrName = "vmr2w";
		  break;

		case 4:
		  instrName = "vmr3w";
		  break;

		case 6:
		  instrName = "vmr4w";
		  break;

		case 8:
		  instrName = "vmr5w";
		  break;

		case 10:
		  instrName = "vmr6w";
		  break;

		case 1:
		  instrName = "vmr1aw";
		  break;

		case 3:
		  instrName = "vmr2aw";
		  break;

		case 5:
		  instrName = "vmr3aw";
		  break;

		case 7:
		  instrName = "vmr4aw";
		  break;

		case 9:
		  instrName = "vmr5aw";
		  break;

		case 11:
		  instrName = "vmr6aw";
		  break;

		}

	      decodingClass = 42;
	      usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	      break;
	    }


	  case 7:
	  case 6:
	    {
	      switch (BITS(state->words[0], 16, 19))
		{
		case 15:
		  instrName = "vh264ft";
		  break;
		case 14:
		  instrName = "vh264f";
		  break;
		case 13:
		  instrName = "vvc1ft";
		  break;
		case 12:
		  instrName = "vvc1f";
		  break;
		}
	      decodingClass = 42;
	      usesSimdRegA  = usesSimdRegB = usesSimdRegC = 1;
	      break;

	    }

	  case 92:
	    instrName = "vd6tapf";
	    decodingClass = 39;
	    usesSimdRegA  = usesSimdRegB = 1;
	    usesSimdRegC  = 0;
	    break;

	  case 55:
	    instrName = "vinti";
	    decodingClass = 43;
	    usesSimdRegA  = usesSimdRegB = usesSimdRegC = 0;
	    break;

	  default:
	    instrName = "SIMD";
	    state->flow = invalid_instr;
	    break;
	  }
      }
    else
      {
	if (BIT(state->words[0],3))
	  {
	    decodingClass = 50;
	    instrName = "add_s";
	  }
	else
	  {
	    if (BIT(state->words[0],4))
	      {
		decodingClass = 48;
		instrName = "sub_s";
	      }
	    else
	      {
		decodingClass = 49;
		instrName = "ld_s.as";
	      }
	  }
      }
    break;
#endif  // #ifndef ARC_NO_SIMD_CMDS

#ifdef ARC_NPS_CMDS
  case op_USER_7:
  /* ARC 32-bit basecase instructions with 3 Operands */
    decodingClass = 0;  /* Default for 3 operand instructions */
    subopcode = BITS(state->words[0],16,21);
    switch (subopcode)
    {
      case 0x2f:
    	  decodingClass = 1;
          switch (BITS(state->words[0],0,5)) { /* Checking based on Subopcode2 */
    	      case 0: instrName = "dctcp";  break;
    	      case 1: instrName = "dcip";  break;
    	      case 2: instrName = "dcet";  break;
    	      case 3: instrName = "hwschd.restore" ; decodingClass = 73; break;
    	      case 4:
    	    	  decodingClass = 26;
    	    	  switch ( BITS(state->words[0],6,11) ) {
    	    	      case 0: instrName = "schd.rw";  break;
    	    	      case 1: instrName = "schd.wft";  break;
    	    	      case 2: instrName = "schd.rd";  break;
    	    	      default: instrName = "schd.??";  break;
    	    	  }
    	    	  break;
        	  case 5: instrName = "lbdsize";  break;
        	  case 0x24: instrName = "getsti"; decodingClass = 86; break;
        	  case 0x25: instrName = "getrtc"; decodingClass = 86; break;
    	      case 0x20: instrName = "jobget" ; decodingClass = 79; break;
       	      case 0x21: instrName = "jobget.cl" ; decodingClass = 79; break;
    	      case 0x3f:
    	    	  switch ( BITS(state->words[0],6,11) ) {
    	    	  	  case 0:
    	    	  		  decodingClass = 26;
    	    	  		  switch ( BITS(state->words[0],24,26) ) {
    	    	  		  	  case 0: instrName = "sync.rd";  break;
    	    	  		  	  case 1: instrName = "sync.wr";  break;
    	    	  		  	  default: instrName = "sync.??";  break;
    	    	  		  }
    	    	  		  break;
        	    	  case 2: instrName = "hwschd.off" ; decodingClass = 73; break;
        	    	  case 3: instrName = "cnljob" ; decodingClass = 73; break;
    	    	  	  default: instrName = "???3f"; 	state->flow=invalid_instr; 		break;

    	    	  }
    	    	  break;
	          default: 	instrName = "???2f"; 	state->flow=invalid_instr; 		break;
	      }
	      break;

      case 0x10: instrName = "bdalc"; decodingClass = 65; break;
      case 0x11: instrName = "bdfre"; decodingClass = 65; break;
      case 0x12: instrName = "jobdn"; decodingClass = 65; break;
      case 0x13: instrName = "ldjob"; decodingClass = 65; break;
      case 0x14: instrName = "stjob"; decodingClass = 65; break;
      case 0x15: instrName = "whash"; decodingClass = 65; break;
      case 0x16: instrName = "asri"; decodingClass = 69; break;
      case 0x17: instrName = "sbdfre"; decodingClass = 0; break;
      case 0x18: instrName = "bdbgt"; decodingClass = 0; break;
      case 0x19: instrName = "idxbgt"; decodingClass = 0; break;
      case 0x1A: instrName = "jobbgt"; decodingClass = 0; break;

      case 0x20: instrName = "dcet"; decodingClass = 62; break;
      case 0x21: instrName = "tr"; decodingClass = 0; break;
      case 0x22: instrName = "utf8"; decodingClass = 0; break;
      case 0x23: instrName = "addf"; decodingClass = 0; break;
      case 0x25: instrName = "dcacl"; decodingClass = 0; break;
      case 0x26: instrName = "idxalc"; decodingClass = 65; break;
      case 0x27: instrName = "sidxfre"; decodingClass = 65; break;
      case 0x28: instrName = "idxfre"; decodingClass = 65; break;
      case 0x29: instrName = "dcip"; decodingClass = 82; break;
      case 0x2A: instrName = "csma"; decodingClass = 0; break;
      case 0x2B: instrName = "jobalc"; decodingClass = 65; break;
      case 0x2C: instrName = "csms"; decodingClass = 0; break;
      case 0x2D: instrName = "cbba"; decodingClass = 0; break;
      case 0x2E: instrName = "rflt"; decodingClass = 0; break;
      case 0x30:
    	  decodingClass = 26;
    	  switch ( BITS(state->words[0],6,11) ) {
    	  	  case 0: instrName = "wkup"; decodingClass = 69; break;
    	  	  case 4: instrName = "wkup.cl"; break;
    	  	  default: instrName = "wkup.???"; break;
    	  }
    	  break;
      case 0x31: instrName = "lkpitcm"; decodingClass = 84; break;
      case 0x32: instrName = "lkpetcm"; decodingClass = 84; break;
      case 0x33: instrName = "crc16"; decodingClass = 0; break;
      case 0x34: instrName = "crc32"; decodingClass = 0; break;
      case 0x35: instrName = "zncv"; decodingClass = 0; break;
      case 0x36: instrName = "hofs"; decodingClass = 82; break;
      default:
	       instrName = "???7";
	       state->flow=invalid_instr;
	       break;
    }
    break;
  case op_USER_9:
    /* ARCompact 16-bit instructions, General ops/ single ops */
      switch(BITS(state->words[0],16,20))
      {
        case 16:
        case  0:
        	subopcode = BITS(state->words[0],0,4);
        	switch (subopcode) {
      	      case 0:
      		       instrName = "addb"; decodingClass = 60; break;
    	      case 1:
    		       instrName = "andb"; decodingClass = 60; break;
    	      case 2:
    		       instrName = "xorb"; decodingClass = 60; break;
    	      case 3:
    		       instrName = "orb"; decodingClass = 60; break;
    	      case 4:
    		       instrName = "subb"; decodingClass = 60; break;
    	      case 5:
    		       instrName = "adcb"; decodingClass = 60; break;
    	      case 6:
    		       instrName = "sbcb"; decodingClass = 60; break;
    	      case 7:
    		       instrName = "fxorb"; decodingClass = 60; break;
    	      case 8:
    		       instrName = "wxorb"; decodingClass = 60; break;
    	      case 9:
    		       instrName = "notb"; decodingClass = 60; break;
    	      case 10:
    		       instrName = "cntbb"; decodingClass = 60; break;
    	      case 11:
    		       instrName = "shlb"; decodingClass = 60; break;
    	      case 12:
    		       instrName = "shrb"; decodingClass = 60; break;
    	      case 13:
    		       instrName = "div"; decodingClass = 61; break;
    	      case 14:
    		       instrName = "qcmp.ar"; decodingClass = 74; break;
    	      case 15:
    		       instrName = "qcmp.al"; decodingClass = 74; break;
       	      case 16:
        		   instrName = "calcsd"; decodingClass = 60; break;
       	      case 17:
        		   instrName = "andab"; decodingClass = 81; break;
       	      case 18:
        		   instrName = "orab"; decodingClass = 81; break;
       	      case 19:
        		   instrName = "bdlen"; decodingClass = 80; break;
             default:
  	               instrName = "??? (2[3])";
  	               state->flow = invalid_instr;
  	               break;
       	    }
            break;
        case 17:
        case  1:
                  instrName = "movb"; decodingClass = 55; break;
        case 19:
        case  3:
                  instrName = "decode1"; decodingClass = 55; break;
        case 20:
        case  4:
                  instrName = "encode"; decodingClass = 55; break;
        case  5:
        	subopcode = BITS(state->words[0],0,2);
        	switch (subopcode) {
      	      case 0:
      	    	   instrName = "encr"; decodingClass = 78; break;
      	      case 1:
      		       instrName = "decr"; decodingClass = 78; break;
      	      case 2:
      		       instrName = "shash"; decodingClass = 78; break;
      	      case 3:
      		       instrName = "expskey"; decodingClass = 78; break;
      	      case 4:
      		       instrName = "wrkey"; decodingClass = 78; break;
      	      case 5:
      		       instrName = "gcm.fin"; decodingClass = 78; break;
      	      case 6:
      		       instrName = "gensiv"; decodingClass = 78; break;
              default:
  	               instrName = "??? (2[3])";
  	               state->flow = invalid_instr;
  	               break;
       	    }
        	break;
        case  6:
        case 22:
            subopcode = BITS(state->words[0],0,4);
            switch (subopcode) {
            case 0:
          	   instrName = "xex"; decodingClass = 75; break;
            case 1:
          	   instrName = "exc"; decodingClass = 75; break;
            case 2:
          	   instrName = "aadd"; decodingClass = 75; break;
            case 3:
          	   instrName = "adadd"; decodingClass = 75; break;
            case 4:
          	   instrName = "aand"; decodingClass = 75; break;
            case 5:
          	   instrName = "aor"; decodingClass = 75; break;
            case 6:
          	   instrName = "axor"; decodingClass = 75; break;
            case 7:
          	   instrName = "xst"; decodingClass = 75; break;
            case 8:
          	   instrName = "pcrcycl.cl"; decodingClass = 75; break;
            case 9:
           	   instrName = "cbclr"; decodingClass = 75; break;
            case 10:
           	   instrName = "cbset"; decodingClass = 75; break;
            case 11:
           	   instrName = "cbcswp"; decodingClass = 75; break;
            case 12:
           	   instrName = "cbwr"; decodingClass = 75; break;
            case 13:
           	   instrName = "cbrd"; decodingClass = 75; break;
            case 14:
           	   instrName = "crd"; decodingClass = 75; break;
            case 15:
           	   instrName = "cld"; decodingClass = 75; break;
            case 16:
          	   instrName = "atst"; decodingClass = 75; break;
            case 17:
          	   instrName = "ari"; decodingClass = 75; break;
            case 18:
          	   instrName = "ardc"; decodingClass = 75; break;
            case 19:
          	   instrName = "aric"; decodingClass = 75; break;
            case 20:
          	   instrName = "aric.r"; decodingClass = 75; break;
            case 21:
          	   instrName = "arcl"; decodingClass = 75; break;
            case 22:
          	   instrName = "xld"; decodingClass = 75; break;
            case 23:
           	   instrName = "cmld"; decodingClass = 75; break;
            case 24:
           	   instrName = "cmst"; decodingClass = 75; break;
            case 25:
           	   instrName = "cminit"; decodingClass = 75; break;
            case 26:
           	   instrName = "cminit.rst"; decodingClass = 75; break;
            case 27:
           	   instrName = "cwrdb"; decodingClass = 75; break;
            case 28:
           	   instrName = "cwrde"; decodingClass = 75; break;
            case 30:
            	instrName = "cwcfg"; decodingClass = 75; break;
            case 31:
            	instrName = "cwchk"; decodingClass = 75; break;
            default:
	               instrName = "??? (2[3])";
	               state->flow = invalid_instr;
	               break;
            }
            break;
        case 7:
        		  instrName = "ld16.phy"; decodingClass = 77; break;
        case 24:
            	  instrName = "movh.cl"; decodingClass = 54; break;
        case  8:
                  instrName = "movh"; decodingClass = 54; break;
        case 25:
                  instrName = "movl.cl"; decodingClass = 54; break;
        case 9:
                  instrName = "movl"; decodingClass = 54; break;
        case 26:
        case 10:
                  instrName = "addl"; decodingClass = 54; break;
        case 27:
        case 11:
                  instrName = "subl"; decodingClass = 54; break;
        case 28:
        case 12:
                  instrName = "orl"; decodingClass = 54; break;
        case 29:
        case 13:
                  instrName = "andl"; decodingClass = 54; break;
        case 30:
        case 14:
                  instrName = "xorl"; decodingClass = 54; break;
        case 31:
        case 15:
                  instrName = "movbi"; decodingClass = 64; break;
        case 18:
        case  2:
                  instrName = "mcmp"; decodingClass = 68; break;


        default:
	              instrName = "??? (2[3])";
	              state->flow = invalid_instr;
	              break;
      }
      break;

    case op_USER_10:
    /* ARCompact 16-bit instructions, General ops/ single ops */
      switch(BITS(state->words[0],0,4))
      {
/* case 0 for commands dcmac(l),dcipv4(l),dcipv6(l),dcmpls(l),dcudpl,dctcpl,dcgrel,dcmiml */
      case 0:  instrName = "dcmac"; decodingClass = 67; break;
/* case 1 for commands cpb2b,cpb2p,cpp2b,ldbd,stbd,ldjd,stjd */
      case 1:  instrName = "cpb2b"; decodingClass = 66; break;
/* case 2 for commands cpb2b.l,cpb2p.l,cpp2b.l,ldbd.l,stbd.l,cpp2p.l,ldphy.l,stphy.l,ldsd */
      case 2:  instrName = "cpb2b.l"; decodingClass = 66; break;
/* case 3 for commands cp,cpp2p,ldphy,stphy */
      case 3:  instrName = "cpp2p"; decodingClass = 66; break;
/* case 4 for commands cp */
      case 4:  instrName = "cp"; decodingClass = 85; break;
/* case 5 for commands mmnt */
      case 5:  instrName = "mmnt"; decodingClass = 87; break;
/* case 8 for commands gmsg */
      case 8:  instrName = "gmsg"; decodingClass = 83; break;
/* case 23 for commands gets_0 */
      case 23:  instrName = "gets_0"; decodingClass = 76; break;
/* case 25 for statistic commands */
      case 25: instrName = "scrst_0"; decodingClass = 76; break;
/* case 27 for commands add_0 */
      case 27:  instrName = "add_0"; decodingClass = 76; break;
/* case 28 for commands rmv_0 */
      case 28:  instrName = "rmv_0"; decodingClass = 76; break;
/* case 29 for commands upd_0 */
      case 29:  instrName = "upd_0"; decodingClass = 76; break;
/* case 30 for commands lkp_0 */
      case 30:  instrName = "lkp_0"; decodingClass = 76; break;
      default:
	  instrName = "??? (2[3])";
	  state->flow = invalid_instr;
	  break;
      }
      break;

    case op_USER_11:

   /* ARCompact 16-bit instructions, General ops/ single ops */
      if (state->instructionLen == 2) {
       switch(BITS(state->words[0],0,4))
        {
          case  0: instrName = "mov2b"; decodingClass = 57; break;
          case  1: instrName = "ext4b"; decodingClass = 59; break;
          case  2: instrName = "ins4b"; decodingClass = 59; break;
          case  3: instrName = "mrgb";  decodingClass = 56; break;
          default:
	             instrName = "??? (2[3])";
	             state->flow = invalid_instr;
	             break;
       }
      }
      else {
  /* ARCompact 16-bit instructions, General ops/ single ops */
        switch(BITS(state->words[0],16,20))
        {
          case  8: instrName = "xldb"; decodingClass = 54; break;
          case  9: instrName = "xldw"; decodingClass = 54; break;
          case 10: instrName = "xld"; decodingClass = 54; break;
          case 11: instrName = "mxb";  decodingClass = 70; break;
          case 12: instrName = "xstb"; decodingClass = 54; break;
          case 13: instrName = "xstw"; decodingClass = 54; break;
          case 14: instrName = "xst"; decodingClass = 54; break;
          case 16:  instrName = "mov4b"; decodingClass = 58; break;
          case 17:  instrName = "mov4bcl"; decodingClass = 58; break;
          case 24:  instrName = "hash"; decodingClass = 63; break;
          case 25:  instrName = "hash.p0"; decodingClass = 63; break;
          case 26:  instrName = "hash.p1"; decodingClass = 63; break;
          case 27:  instrName = "hash.p2"; decodingClass = 63; break;
          case 28:  instrName = "hash.p3"; decodingClass = 63; break;
          case 29:  instrName = "e4by"; decodingClass = 63; break;

          default:
	             instrName = "??? (2[3])";
	             state->flow = invalid_instr;
	             break;
        }
      }
      break;
#endif // #ifdef ARC_NPS_CMDS


    case op_LD_ADD:
    /* Load/Add resister-register */
      decodingClass = 15;  /* default for Major opcode 12 ... */
      switch(BITS(state->words[0],3,4))
	{
	case 0: instrName = "ld_s"; break;
	case 1: instrName = "ldb_s"; break;
	case 2:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "ldh_s";
	  else
	    instrName = "ldw_s";
	  break;
	case 3: instrName = "add_s"; break;
	default:
	  instrName = "??? (2[3])";
	  state->flow = invalid_instr;
	  break;
	}
      break;

  case op_ADD_SUB_SHIFT:
    /* Add/sub/shift immediate */
    decodingClass = 16;  /* default for Major opcode 13 ... */
    switch(BITS(state->words[0],3,4))
      {
      case 0: instrName = "add_s"; break;
      case 1: instrName = "sub_s"; break;
      case 2: instrName = "asl_s"; break;
      case 3: instrName = "asr_s"; break;
      default:
	instrName = "??? (2[3])";
	state->flow = invalid_instr;
	break;
      }
    break;

#ifdef NPS_ARCv2
  case op_LD_ST:
    if (BIT(state->words[0],3))
      {
	instrName = "ldi_s";
	decodingClass = 58;
      }
    else
      {
	decodingClass = 55;
	if (BIT(state->words[0],4))
	  instrName = "st_s r0,[gp";
	else
	  instrName = "ld_s r1,[gp";
      }
    break;

  case op_JLI_EI:
    decodingClass = 56;
    if (BIT(state->words[0],10))
      {
	instrName = "ei_s";
      }
    else
      {
	instrName = "jli_s";
      }
    break;
#endif /* #ifdef NPS_ARCv2 */

  case op_ADD_MOV_CMP:
    /* One Dest/Source can be any of r0 - r63 */
      decodingClass = 17;  /* default for Major opcode 14 ... */
      if(info->mach == bfd_mach_arc_arcv2)
	switch(BITS(state->words[0],2,4))
	  {
	  case 0: instrName = "add_s"; break;
	  case 1: instrName = "add_s"; decodingClass = 51; break;

	  case 3: instrName = "mov_s"; decodingClass = 52; break;
	  case 4:
	  case 5: instrName = "cmp_s"; decodingClass = 52; break;

	  case 7: instrName = "mov_s.ne"; decodingClass = 52; break;
	  default:
	    instrName = "??? (2[3])";
	    state->flow = invalid_instr;
	    break;
	  }
      else
	switch(BITS(state->words[0],3,4))
	  {
	  case 0: instrName = "add_s"; break;
	  case 1:
	  case 3: instrName = "mov_s"; decodingClass = 18; break;
	  case 2: instrName = "cmp_s"; decodingClass = 18; break;
	  default:
	    instrName = "??? (2[3])";
	    state->flow = invalid_instr;
	    break;
	  }

      break;

    case op_S:
    /* ARCompact 16-bit instructions, General ops/ single ops */
      decodingClass = 22;  /* default for Major opcode 15 ... */
      switch(BITS(state->words[0],0,4))
      {
	case 0:
	  decodingClass = 27;
	  switch(BITS(state->words[0],5,7))
	  {
	    case 0 : instrName = "j_s";
	    case 2 : if (!instrName) instrName = "jl_s";
		     state->isBranch = 1;
		     state->nullifyMode = BR_exec_when_no_jump;
		     /* MJ: map 0..7 to r0..3, r12..15 by doing regno = b2b1b0 | b2 << 3 */
		     state->register_for_indirect_jump = (BITS(state->words[0],8,10))
						       | (BITS(state->words[0],10,10) << 3);
		     break;
	    case 1 : if (!instrName) instrName = "j_s.d";
	    case 3 : if (!instrName) instrName = "jl_s.d";
		     state->isBranch = 1;
		     state->nullifyMode = BR_exec_always;
		     state->register_for_indirect_jump = (BITS(state->words[0],8,10))
						       | (BITS(state->words[0],10,10) << 3);
		     break;
	    case 6 : instrName = "sub_s.ne";
		     decodingClass = 35;
		     break;
	    case 7 :
	      decodingClass = 26;
	      switch(BITS(state->words[0],8,10))
	      {
		case 0 : instrName = "nop_s"; break;

		  /* Unimplemented instruction reserved in ARC700 */
		case 1: instrName = "unimp_s";break;
		case 2: instrName = "swi_s";break;

		case 4: instrName = "jeq_s [blink]";
		case 5: if (!instrName) instrName = "jne_s [blink]";
		case 6:
		  if (!instrName)
		    instrName = "j_s [blink]";
		  state->isBranch = 1;
		  state->nullifyMode = BR_exec_when_no_jump;
		  /* 25-sep-12: Jeremy Bennett <jeremy.bennett@embecosm.com>.
				Implicit register number for BLINK won't be
				picked up below. */
		  state->register_for_indirect_jump = 31;
		  break;
		case 7:
		  if (!instrName)
		    instrName = "j_s.d [blink]";
		  state->isBranch = 1;
		  state->nullifyMode = BR_exec_always;
		  /* 25-sep-12: Jeremy Bennett <jeremy.bennett@embecosm.com>.
				Implicit register number for BLINK won't be
				picked up below. */
		  state->register_for_indirect_jump = 31;
		  break;
		default:
		  instrName = "??? (2[3])";
		  state->flow = invalid_instr;
		  break;
	      }
	      break;
	    default:
	      instrName = "??? (2[3])";
	      state->flow = invalid_instr;
	      break;
	  }
	  break;
	case 2 : instrName = "sub_s"; break;
	case 4 : instrName = "and_s"; break;
	case 5 : instrName = "or_s"; break;
	case 6 : instrName = "bic_s"; break;
	case 7 : instrName = "xor_s"; break;

	case 9 : instrName = "mpyw_s"; break;
	case 10 : instrName = "mpyuw_s"; break;
	case 11: instrName = "tst_s"; decodingClass = 14; break;
	case 12:
	  if(info->mach == bfd_mach_arc_arcv2)
	    {
	      instrName = "mpy_s";
	      mul =1;
	    }
	  else
	    {
	      instrName = "mul64_s";
	      mul =1;
	      decodingClass = 14;
	    }
	  break;
	case 13: instrName = "sexb_s"; decodingClass = 14; break;
	case 14:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "sexh_s";
	  else
	    instrName = "sexw_s";
	  decodingClass = 14;
	  break;
	case 15:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "exth_s";
	  else
	    instrName = "extb_s";
	  decodingClass = 14;
	  break;
	case 16:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "exth_s";
	  else
	    instrName = "extw_s";
	  decodingClass = 14;
	  break;
	case 17: instrName = "abs_s"; decodingClass = 14; break;
	case 18: instrName = "not_s"; decodingClass = 14; break;
	case 19: instrName = "neg_s"; decodingClass = 14; break;
	case 20: instrName = "add1_s"; break;
	case 21: instrName = "add2_s"; break;
	case 22: instrName = "add3_s"; break;
	case 24: instrName = "asl_s"; break;
	case 25: instrName = "lsr_s"; break;
	case 26: instrName = "asr_s"; break;
	case 27: instrName = "asl_s"; decodingClass = 14; break;
	case 28: instrName = "asr_s"; decodingClass = 14; break;
	case 29: instrName = "lsr_s"; decodingClass = 14; break;
      case 30: instrName = "trap_s"; decodingClass = 33; break;
      case 31: instrName = "brk_s"; decodingClass = 26; break;

	default:
	  instrName = "??? (2[3])";
	  state->flow = invalid_instr;
	  break;
      }
      break;

       case op_LD_S:
    /* ARCompact 16-bit Load with offset, Major Opcode 0x10 */
      instrName = "ld_s";
      decodingClass = 28;
      break;

    case op_LDB_S:
    /* ARCompact 16-bit Load with offset, Major Opcode 0x11 */
      instrName = "ldb_s";
      decodingClass = 28;
      break;

    case op_LDW_S:
    /* ARCompact 16-bit Load with offset, Major Opcode 0x12 */
      if(info->mach == bfd_mach_arc_arcv2)
	instrName = "ldh_s";
      else
	instrName = "ldw_s";
      decodingClass = 28;
      break;

    case op_LDWX_S:
    /* ARCompact 16-bit Load with offset, Major Opcode 0x13 */
      if(info->mach == bfd_mach_arc_arcv2)
	instrName = "ldh_s.x";
      else
	instrName = "ldw_s.x";
      decodingClass = 28;
      break;

    case op_ST_S:
    /* ARCompact 16-bit Store with offset, Major Opcode 0x14 */
      instrName = "st_s";
      decodingClass = 28;
      break;

    case op_STB_S:
    /* ARCompact 16-bit Store with offset, Major Opcode 0x15 */
      instrName = "stb_s";
      decodingClass = 28;
      break;

    case op_STW_S:
    /* ARCompact 16-bit Store with offset, Major Opcode 0x16 */
      if(info->mach == bfd_mach_arc_arcv2)
	instrName = "sth_s";
      else
	instrName = "stw_s";
      decodingClass = 28;
      break;

    case op_Su5:
    /* ARCompact 16-bit involving unsigned 5-bit immediate operand */
      decodingClass = 23;  /* default for major opcode 0x17 ... */
      switch (BITS(state->words[0],5,7))
      {
	case 0: instrName = "asl_s"; break;
	case 1: instrName = "lsr_s"; break;
	case 2: instrName = "asr_s"; break;
	case 3: instrName = "sub_s"; break;
	case 4: instrName = "bset_s"; break;
	case 5: instrName = "bclr_s"; break;
	case 6: instrName = "bmsk_s"; break;
	case 7: instrName = "btst_s"; decodingClass = 21; break;
      }
      break;

    case op_SP:
    /* ARCompact 16-bit Stack pointer-based instructions */
      decodingClass = 19;  /* default for Stack pointer-based insns ... */
      switch (BITS(state->words[0],5,7))
      {
	case 0: instrName = "ld_s"; break;
	case 1: instrName = "ldb_s"; break;
	case 2: instrName = "st_s"; break;
	case 3: instrName = "stb_s"; break;
	case 4: instrName = "add_s"; break;
	case 5:
	  if (!BITS(state->words[0],8,8))
	    instrName = "add_s";
	  else
	    instrName = "sub_s";
	  break;
	case 6:
	  if (info->mach == bfd_mach_arc_arcv2
	      && (!BIT(state->words[0],0)))
	    {
	      decodingClass = 57;
	      instrName = "leave_s";
	    }
	  else
	    {
	      instrName = "pop_s";
	      decodingClass = 31;
	    }
	  break;
	case 7:
	  if (info->mach == bfd_mach_arc_arcv2
	      && (!BIT(state->words[0],0)))
	    {
	      decodingClass = 57;
	      instrName = "enter_s";
	    }
	  else
	    {
	      decodingClass = 31;
	      instrName = "push_s";
	    }
	  break;
	default:
	  instrName = "??? (2[3])";
	  state->flow = invalid_instr;
	  break;
      }
    break;

    case op_GP:
    /* ARCompact 16-bit Gp-based ld/add (data aligned offset) */
      decodingClass = 20;  /* default for gp-relative insns ... */
      switch (BITS(state->words[0],9,10))
      {
	case 0: instrName = "ld_s"; break;
	case 1: instrName = "ldb_s"; break;
	case 2:
	  if(info->mach == bfd_mach_arc_arcv2)
	    instrName = "ldh_s";
	  else
	    instrName = "ldw_s";
	  break;
	case 3: instrName = "add_s"; break;
      }
      break;

    case op_Pcl:
    /* ARCompact 16-bit Pcl-based ld (32-bit aligned offset) */
      instrName = "ld_s";
      decodingClass = 29;
      break;

    case op_MOV_S:
    /* ARCompact 16-bit Move immediate */
      instrName = "mov_s";
      decodingClass = 30;
      break;

    case op_ADD_CMP:
    /* ARCompact 16-bit Add/compare immediate */
      decodingClass = 21;  /* default for major opcode 0x1c ... */
      if (BIT(state->words[0],7))
	instrName = "cmp_s";
      else
	instrName = "add_s";
      break;

    case op_BR_S:
    /* ARCompact 16-bit Branch conditionally on reg z/nz */
      decodingClass = 25; /* Default for BR_S instruction ... */
      if (BIT(state->words[0],7))
	instrName = "brne_s";
      else
	instrName = "breq_s";
      state->isBranch = 1;
      break;

    case op_B_S:
    /* ARCompact 16-bit Branch conditionally */
      decodingClass = 24; /* Default for B_S instruction ... */
      state->isBranch = 1;
      switch (BITS(state->words[0],9,10))
      {
	case 0: instrName = "b_s"; break;
	case 1: instrName = "beq_s"; break;
	case 2: instrName = "bne_s"; break;
	case 3:
	  switch (BITS(state->words[0],6,8))
	  {
	    case 0: instrName = "bgt_s"; break;
	    case 1: instrName = "bge_s"; break;
	    case 2: instrName = "blt_s"; break;
	    case 3: instrName = "ble_s"; break;
	    case 4: instrName = "bhi_s"; break;
	    case 5: instrName = "bhs_s"; break;
	    case 6: instrName = "blo_s"; break;
	    case 7: instrName = "bls_s"; break;
	  }
	  break;
      }
      break;

    case op_BL_S:
    /* ARCompact 16-bit Branch and link unconditionally */
      decodingClass = 24; /* Default for B_S instruction ... */
      instrName = "bl_s";
      state->isBranch = 1;
      break;

    default:

      instrName = "???";
      state->flow=invalid_instr;
      break;
  }

  /* Maybe we should be checking for extension instructions over here
   * instead of all over this crazy switch case. */
  if (state->flow == invalid_instr)
    {
#ifndef ARC_NO_SIMD_CMDS
      if (!((state->_opcode == op_SIMD) && enable_simd))
	instrName = instruction_name(state,state->_opcode,
				     state->words[0],
				     &flags);
#endif // #ifndef ARC_NO_SIMD_CMDS

      if (state->instructionLen == 2)
	{
	  switch (flags)
	    {
	    case AC_SYNTAX_3OP:
	      decodingClass = 22;
	      break;
	    case AC_SYNTAX_2OP:
	      decodingClass = 14;
	      break;
	    case AC_SYNTAX_1OP:
	      decodingClass = 36;
	      break;
	    case AC_SYNTAX_NOP:
	      decodingClass = 26;
	      break;
	    default:
	      mwerror(state, "Invalid syntax class\n");
	    }
	}
      else
	{
/* Must do the above for this one too */
	  switch (flags)
	    {
	    case AC_SYNTAX_3OP:
	      decodingClass = 0;
	      break;
	    case AC_SYNTAX_2OP:
	      decodingClass = 1;
	      break;
	    case AC_SYNTAX_1OP:
	      decodingClass = 32;
	      break;
	    case AC_SYNTAX_NOP:
	      break;
#ifndef ARC_NO_SIMD_CMDS
	    case AC_SYNTAX_SIMD:
	      break;
#endif  // #ifndef ARC_NO_SIMD_CMDS
	    default:
	      mwerror(state, "Invalid syntax class\n");
	    }
	}

      if (!instrName)
	{
	  instrName = "???";
	  state->flow=invalid_instr;
	}
    }

  fieldAisReg = fieldBisReg = fieldCisReg = 1; /* assume regs for now */
  flag = cond = is_shimm = is_limm = 0;
  signExtend = addrWriteBack = directMem = 0;
  usesAuxReg = 0;

  /* The following module decodes the instruction */
  switch (decodingClass)
  {
    case 0:

      /* For ARCtangent 32-bit instructions with 3 operands */

      subopcode = BITS(state->words[0],22,23);
      switch (subopcode)
      {
	case 0:

	  /* Either fieldB or fieldC or both can be a limm value;
	   * fieldA can be 0;
	   */

	  CHECK_FIELD_C();
	  if (!is_limm)
	  {
	    /* If fieldC is not a limm, then fieldB may be a limm value */
	    CHECK_FIELD_B();
	  }
	  else
	  {
	    FIELD_B();
	    if (!fieldBisReg)
	      fieldB = fieldC;
	  }
	  CHECK_FIELD_A();
	  CHECK_FLAG();
	  break;

	case 1:

	  /* fieldB may ba a limm value
	   * fieldC is a shimm (unsigned 6-bit immediate)
	   * fieldA can be 0
	   */

	  CHECK_FIELD_B();
	  FIELD_C();
	  fieldCisReg = 0;
	  /* Say ea is not present, so only one of us will do the
	     name lookup. */
	  state->_offset += fieldB, state->_ea_present = 0;
	  CHECK_FIELD_A();
	  CHECK_FLAG();
	  break;

	case 2:

	  /* fieldB may ba a limm value
	   * fieldC is a shimm (signed 12-bit immediate)
	   * fieldA can be 0
	   */

	  fieldCisReg = 0;
	  fieldC = FIELDS(state->words[0]);
	  CHECK_FIELD_B();
	  /* Say ea is not present, so only one of us will do the
	     name lookup. */
	  state->_offset += fieldB, state->_ea_present = 0;
	  if (is_limm)
	    fieldAisReg = fieldA = 0;
	  else
	    fieldA = fieldB;
	  CHECK_FLAG();
	  break;

	case 3:

	  /* fieldB may ba a limm value
	   * fieldC may be a limm or a shimm (unsigned 6-bit immediate)
	   * fieldA can be 0
	   * Conditional instructions
	   */

	  CHECK_FIELD_B();
	  /* fieldC is a shimm (unsigned 6-bit immediate) */
	  if (is_limm)
	  {
	    fieldAisReg = fieldA = 0;
	    FIELD_C();
	    if (BIT(state->words[0],5))
	      fieldCisReg = 0;
	    else if (fieldC == 62)
	    {
	      fieldCisReg = 0;
	      fieldC = fieldB;
	    }
	  }
	  else
	  {
	    fieldA = fieldB;
	    if (BIT(state->words[0],5))
	    {
	      FIELD_C();
	      fieldCisReg = 0;
	    }
	    else
	    {
	      CHECK_FIELD_C();
	    }
	  }
	  CHECK_FLAG_COND();
	  break;
      }
      write_instr_name();
      WRITE_FORMAT_x(A);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      write_comments();
      break;

    case 1:

      /* For ARCtangent 32-bit instructions with 2 operands */

      /* field C is either a register or limm (different!) or u6 */
      if (BITS(state->words[0],22,23) == 1 ) {
	     fieldCisReg = 0;
	     FIELD_C();
      }
      else {
	  CHECK_FIELD_C();
      }
      FIELD_B();
      CHECK_FLAG();

      if (fieldCisReg) state->ea_reg1 = fieldC;
      /* field C is either a shimm (same as fieldC) or limm (different!) */
      /* Say ea is not present, so only one of us will do the name lookup. */
      else state->_offset += fieldB, state->_ea_present = 0;

      write_instr_name();
      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      write_comments();
      break;

    case 2:

      /* For BTST, CMP, MUL64, MULU64 instruction */

      /* field C is either a register or limm (different!) */
      subopcode =  BITS(state->words[0],22,23);
      if (subopcode == 0 || ((subopcode == 3) && (!BIT(state->words[0],5))))
      {
	CHECK_FIELD_C();
	if (is_limm)
	{
	  FIELD_B();
	  if (!fieldBisReg)
	    fieldB = fieldC;
	}
	else
	{
	  CHECK_FIELD_B();
	}
      }
      else if (subopcode == 1 || ((subopcode == 3) && (BIT(state->words[0],5))))
      {
	FIELD_C();
	fieldCisReg = 0;
	CHECK_FIELD_B();
      }
      else if (subopcode == 2)
      {
	CHECK_FIELD_B();
	FIELD_B();
	if (is_limm)
	  fieldB = limm_value;
	fieldC = FIELDS(state->words[0]);
	fieldCisReg = 0;
      }
      if (subopcode == 3)
	CHECK_COND();

      if (fieldCisReg) state->ea_reg1 = fieldC;
      /* field C is either a shimm (same as fieldC) or limm (different!) */
      /* Say ea is not present, so only one of us will do the name lookup. */
      else state->_offset += fieldB, state->_ea_present = 0;

      write_instr_name();
      if (mul)
      {
	/* For Multiply instructions, the first operand is 0 */
	WRITE_FORMAT_x(A);
	WRITE_FORMAT_COMMA_x(B);
	WRITE_FORMAT_COMMA_x(C);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, 0, fieldB, fieldC);
      }
      else
      {
	WRITE_FORMAT_x(B);
	WRITE_FORMAT_COMMA_x(C);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      }
      write_comments();
      break;

    case 3:
      /*
       * For FLAG instruction
       */
      subopcode =  BITS(state->words[0],22,23);

      if (subopcode == 0 || ((subopcode == 3) && (!BIT(state->words[0],5))))
      {
	CHECK_FIELD_C();
      }
      else if (subopcode == 1 || ((subopcode == 3) && (BIT(state->words[0],5))))
      {
	FIELD_C();
	fieldCisReg = 0;
      }
      else if (subopcode == 2)
      {
	fieldC = FIELDS(state->words[0]);
	fieldCisReg = 0;
      }
      if (subopcode == 3)
	CHECK_COND();
      flag = 0;  /* this is the FLAG instruction -- it's redundant */

      write_instr_name();
      WRITE_FORMAT_x(C);
      my_sprintf(state, state->operandBuffer, formatString, fieldC);
      write_comments();
      break;

    case 4:
      /*
       * For op_JC -- jump to address specified.
       *     Also covers jump and link--bit 9 of the instr. word
       *     selects whether linked, thus "is_linked" is set above.
       */
      subopcode =  BITS(state->words[0],22,23);
      if (subopcode == 0 || ((subopcode == 3) && (!BIT(state->words[0],5))))
      {
	CHECK_FIELD_C();
	/* ilink registers */
	if (fieldC == 29 || fieldC == 31)
	  CHECK_FLAG();
      }
      else if (subopcode == 1 || ((subopcode == 3) && (BIT(state->words[0],5))))
      {
	FIELD_C();
	fieldCisReg = 0;
      }
      else if (subopcode == 2)
      {
	fieldC = FIELDS(state->words[0]);
	fieldCisReg = 0;
      }

      if (subopcode == 3)
	CHECK_COND();

      state->nullifyMode = BITS(state->words[0],16,16);

      if (!fieldCisReg)
	{
	  state->flow = is_linked ? direct_call : direct_jump;
	  add_target(fieldC);
	}
      else
	{
	  state->flow = is_linked ? indirect_call : indirect_jump;
	  /*
	   * We should also treat this as indirect call if NOT linked
	   * but the preceding instruction was a "lr blink,[status]"
	   * and we have a delay slot with "add blink,blink,2".
	   * For now we can't detect such.
	   */
	  state->register_for_indirect_jump = fieldC;
	}

      write_instr_name();
      strcat(formatString,
	     IS_REG(C)?"[%r]":"%s"); /* address/label name */

      if (IS_REG(C))
	my_sprintf(state, state->operandBuffer, formatString, fieldC);
      else
	my_sprintf(state, state->operandBuffer, formatString,
		   post_address(state, fieldC));
      write_comments();
      break;

    case 5:
      /* LD instruction.  B and C can be regs, or one or both can be limm. */

      CHECK_FIELD_A();
      CHECK_FIELD_B();

      if (is_limm)
      {
	FIELD_C();
	if (!fieldCisReg)
	  fieldC = fieldB;
      }
      else
      {
	CHECK_FIELD_C();
      }
      if (dbg) printf("5:b reg %d %d c reg %d %d  \n",
		      fieldBisReg,fieldB,fieldCisReg,fieldC);
      state->_offset = 0;
      state->_ea_present = 1;
      if (fieldBisReg) state->ea_reg1 = fieldB; else state->_offset += fieldB;
      if (fieldCisReg) state->ea_reg2 = fieldC; else state->_offset += fieldC;
      state->_mem_load = 1;

#ifndef ARC_NPS_CMDS
      directMem     = BIT(state->words[0],15);
#else
      if (ldbitCmd == 0)
           directMem     = BIT(state->words[0],15);
#endif // #ifndef ARC_NPS_CMDS
      if(FIELDA(state->words[0]) == 62)
       {
	 if (directMem)
	   {
	     instrName = "prefetchw";
	     directMem = 0;
	   }
	 else if (BITS (state->words[0], 17, 21) == 25)
	   instrName = "prefetch.l2";
	 else
	   instrName = "prefetch";
	}

      /* - We should display the instruction as decoded, not some censored
	   version of it
	 - Scaled index is encoded as 'addrWriteBack', even though it isn't
	   actually doing a write back;  it is legitimate with a LIMM.  */
#if 0
      /* Check if address writeback is allowed before decoding the
	 address writeback field of a load instruction.*/
      if (fieldBisReg && (fieldB != 62))
#endif
	addrWriteBack = BITS(state->words[0],22,23);
#ifndef ARC_NPS_CMDS
      signExtend    = BIT(state->words[0],16);
#else
	  if (ldbitCmd == 0) {
		  signExtend    = BIT(state->words[0],16);
	  }
	  else {
		  addrWriteBack = 0;
		  switch (BITS(state->words[0],22,23)) {
	          case 0: instrName  = "ldbit.di"; break;
	          case 1: instrName  = "ldbit.x2.di"; break;
	          case 2: instrName  = "ldbit.x4.di"; break;
	          case 3: instrName  = "ldbit.??.di"; break;
		  }
	  }
#endif // #ifndef ARC_NPS_CMDS

      write_instr_name();
#ifdef ARC_NPS_CMDS
      if ( (ldbitCmd != 0) && ( BIT(state->words[0],16) != 0) ){
    	  strcat(state->instrBuffer,".cl");
      }
#endif // #ifdef ARC_NPS_CMDS

      /* Check for prefetch or ld 0,...*/
      if(IS_REG(A))
	WRITE_FORMAT_x_COMMA_LB(A);
      else
	{
	  strcat(formatString,"%*");
	  WRITE_FORMAT_LB();
	}


      if (fieldBisReg || fieldB != 0)
	WRITE_FORMAT_x(B);
      else
	fieldB = fieldC;

      WRITE_FORMAT_COMMA_x_RB(C);
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      write_comments();
      break;

    case 6:
      /* LD instruction. */
      CHECK_FIELD_B();
      CHECK_FIELD_A();
      /* Support for Prefetch */
      /* Fixme :: Check for A700 within this function */

      fieldC = FIELDD9(state->words[0]);
      fieldCisReg = 0;

      if (dbg) printf_unfiltered("6:b reg %d %d c 0x%x  \n",
				 fieldBisReg,fieldB,fieldC);
      state->_ea_present = 1;
      state->_offset = fieldC;
      state->_mem_load = 1;
#ifndef ARC_NPS_CMDS
      if (fieldBisReg) state->ea_reg1 = fieldB;
      /* field B is either a shimm (same as fieldC) or limm (different!) */
      /* Say ea is not present, so only one of us will do the name lookup. */
      else state->_offset += fieldB, state->_ea_present = 0;

      directMem     = BIT(state->words[0],11);
#else
      if (fieldBisReg) {
    	  state->ea_reg1 = fieldB;
    	  if (ldbitCmd == 0) {
              directMem     = BIT(state->words[0],11);
    	  }
      }
      /* field B is either a shimm (same as fieldC) or limm (different!) */
      /* Say ea is not present, so only one of us will do the name lookup. */
      else {
    	  if (ldbitCmd == 0) {
              state->_offset += fieldB;
              directMem     = BIT(state->words[0],11);
   	      }
    	  else {
              state->_offset = fieldB;
    	  }
   	      state->_ea_present = 0;
      }
#endif // #ifndef ARC_NPS_CMDS
      if(FIELDA(state->words[0]) == 62)
	{
	  if (directMem)
	    {
	      instrName = "prefetchw";
	      directMem = 0;
	    }
	  else if (BITS (state->words[0], 7, 8) == 1)
	    instrName = "prefetch.l2";
	  else
	    instrName = "prefetch";
	}
      /* Check if address writeback is allowed before decoding the
	 address writeback field of a load instruction.*/
      if (fieldBisReg && (fieldB != 62))
	addrWriteBack = BITS(state->words[0],9,10);
#ifndef ARC_NPS_CMDS
      signExtend    = BIT(state->words[0],6);

      write_instr_name();
#else
	  if (ldbitCmd == 0) {
		  signExtend    = BIT(state->words[0],6);
	  }
	  else {
		  addrWriteBack = 0;
		  switch (BITS(state->words[0],9,10)) {
	          case 0: instrName  = "ldbit.di"; break;
	          case 1: instrName  = "ldbit.x2.di"; break;
	          case 2: instrName  = "ldbit.x4.di"; break;
	          case 3: instrName  = "ldbit.??.di"; break;
		  }
	  }
      write_instr_name();
      if ( (ldbitCmd != 0) && ( BIT(state->words[0],6) != 0) ) {
    	  strcat(state->instrBuffer,".cl");
      }
#endif // #ifndef ARC_NPS_CMDS
      if(IS_REG(A))
	WRITE_FORMAT_x_COMMA_LB(A);
      else
	{
	  strcat(formatString,"%*");
	  WRITE_FORMAT_LB();
	}
      if (!fieldBisReg)
	{
	  fieldB = state->_offset;
#ifdef ARC_NPS_CMDS
	  if (ldbitCmd) {
		  WRITE_FORMAT_x(B);
		  if(fieldC != 0)
			  WRITE_FORMAT_COMMA_x_RB(C);
		  else
			  WRITE_FORMAT_RB();
	  }
	  else {
		  WRITE_FORMAT_x_RB(B);
	  }
#else
	  WRITE_FORMAT_x_RB(B);
#endif // #ifdef ARC_NPS_CMDS
	}
      else
	{
	  WRITE_FORMAT_x(B);
	  if(fieldC != 0)
		  WRITE_FORMAT_COMMA_x_RB(C);
	  else
		  WRITE_FORMAT_RB();
	}
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      write_comments();
      break;

    case 7:
      /* ST instruction. */
      CHECK_FIELD_B();
      CHECK_FIELD_C();

      /*ARCv2*/
      if(info->mach == bfd_mach_arc_arcv2
	 && BIT(state->words[0],0))
	{
	  fieldCisReg = 0;
	  is_shimm++;
	  fieldC = (BITS ((signed int) state->words[0], 6, 11));
	}

      state->source_operand.registerNum = fieldC;
      state->sourceType = fieldCisReg ? ARC_REGISTER : ARC_LIMM ;
      fieldA  = FIELDD9(state->words[0]); /* shimm */
      fieldAisReg=0;

      /* [B,A offset] */
      if (dbg) printf_unfiltered("7:b reg %d %x off %x\n",
				 fieldBisReg,fieldB,fieldA);
      state->_ea_present = 1;
      state->_offset = fieldA;
      if (fieldBisReg) state->ea_reg1 = fieldB;
      /*
       * field B is either a shimm (same as fieldA) or limm (different!)
       * Say ea is not present, so only one of us will do the name lookup.
       * (for is_limm we do the name translation here).
       */
      else
	state->_offset += fieldB, state->_ea_present = 0;

      directMem     = BIT(state->words[0],5);
      addrWriteBack = BITS(state->words[0],3,4);

      write_instr_name();
      WRITE_FORMAT_x_COMMA_LB(C);
      if (fieldA == 0)
      {
	WRITE_FORMAT_x_RB(B);
      }
      else
      {
	WRITE_FORMAT_x(B);
	fieldAisReg = 0;
	WRITE_FORMAT_COMMA_x_RB(A);
      }
      my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldA);
      write_comments2(fieldA);
      break;

    case 8:
      /* SR instruction */
      CHECK_FIELD_B();
      switch (BITS(state->words[0],22,23))
      {
	case 0:
	  if (is_limm)
	  {
	    FIELD_C();
	    if (!fieldCisReg)
	      fieldC = fieldB;
	  }
	  else
	  {
	    CHECK_FIELD_C();
	  }
	  break;
	case 1:
	  FIELD_C();
	  fieldCisReg = 0;
	  break;
	case 2:
	  fieldC = FIELDS(state->words[0]);
	  fieldCisReg = 0;
	  break;
      }

      write_instr_name();
      WRITE_FORMAT_x_COMMA_LB(B);
      /* Try to print C as an aux reg if it is not a core reg. */
      usesAuxReg = 1;
      WRITE_FORMAT_x(C);
      WRITE_FORMAT_RB();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      write_comments();
      break;

    case 9:
      /* BBIT0/BBIT1 Instruction */

      CHECK_FIELD_C();
      if (is_limm || BIT(state->words[0],4))
      {
	fieldCisReg = 0;
	FIELD_B();
      }
      else
      {
	CHECK_FIELD_B();
      }
      fieldAisReg = fieldA = 0;
      fieldA = FIELDS9(state->words[0]);
      fieldA += (addr & ~0x3);

      CHECK_NULLIFY();
      if(info->mach == bfd_mach_arc_arcv2)
	CHECK_T(BITS(state->words[0],0,2) < 5? 1:0);

      write_instr_name();

      add_target(fieldA);
      state->flow = state->_opcode == op_BLC ? direct_call : direct_jump;
      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(C);
      strcat(formatString, ",%s"); /* address/label name */
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC, post_address(state, fieldA));
      write_comments();
      break;

    case 10:
      /* LR instruction */
      CHECK_FIELD_B();
      switch (BITS(state->words[0],22,23))
      {
	case 0:
	  CHECK_FIELD_C(); break;
	case 1:
	  FIELD_C();
	  fieldCisReg = 0;
	  break;
	case 2:
	  fieldC = FIELDS(state->words[0]);
	  fieldCisReg = 0;
	  break;
      }

      write_instr_name();
      WRITE_FORMAT_x_COMMA_LB(B);
      /* Try to print B as an aux reg if it is not a core reg. */
      usesAuxReg = 1;
      WRITE_FORMAT_x(C);
      WRITE_FORMAT_RB();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      write_comments();
      break;

    case 11:
      /* lp instruction */

      if (BITS(state->words[0],22,23) == 3)
      {
	FIELD_C();
	CHECK_COND();
      }
      else
      {
	fieldC = FIELDS(state->words[0]);
      }

      fieldC = fieldC << 1;
      fieldC += (addr & ~0x3);

      write_instr_name();

      /* This address could be a label we know.  Convert it. */
      add_target(fieldC);
      state->flow = state->_opcode == op_BLC ? direct_call : direct_jump;

      fieldCisReg = 0;
      strcat(formatString, "%s"); /* address/label name */
      my_sprintf(state, state->operandBuffer, formatString, post_address(state, fieldC));
      write_comments();
      break;

    case 12:
      /* MOV instruction */
      FIELD_B();
      subopcode = BITS(state->words[0],22,23);
      if (subopcode == 0 || ((subopcode == 3) && (!BIT(state->words[0],5))))
      {
	CHECK_FIELD_C();
      }
      else if (subopcode == 1 || ((subopcode == 3) && (BIT(state->words[0],5))))
      {
	FIELD_C();
	fieldCisReg = 0;
      }
      else if (subopcode == 2)
      {
	fieldC = FIELDS(state->words[0]);
	fieldCisReg = 0;
      }
      if (subopcode == 3)
      {
	CHECK_FLAG_COND();
      }
      else
      {
	CHECK_FLAG();
      }

     write_instr_name();
     WRITE_FORMAT_x(B);
     WRITE_FORMAT_COMMA_x(C);
     WRITE_NOP_COMMENT();
     my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
     break;

    case 13:
    /* "B", "BL" instruction */

      fieldA = 0;
      if ((state->_opcode == op_BC  && (BIT(state->words[0],16))) ||
	  (state->_opcode == op_BLC && (BIT(state->words[0],17))))
      {
	/* unconditional branch s25 or branch and link d25 */
	fieldA = (BITS(state->words[0],0,4)) << 10;
      }
      fieldA |= BITS(state->words[0],6,15);

      if (state->_opcode == op_BLC)
      {
	  /* Fix for Bug #553.  A bl unconditional has only 9 bits in the
	   * least order bits. */
	fieldA = fieldA << 9;
	fieldA |= BITS(state->words[0],18,26);
	fieldA = fieldA << 2;
      }
      else
      {
	fieldA = fieldA << 10;
	fieldA |= BITS(state->words[0],17,26);
	fieldA = fieldA << 1;
      }

      if ((state->_opcode == op_BC  && (BIT(state->words[0],16))) ||
	  (state->_opcode == op_BLC && (BIT(state->words[0],17))))
	/* unconditional branch s25 or branch and link d25 */
	fieldA = sign_extend(fieldA, 25);
      else
	/* conditional branch s21 or branch and link d21 */
	fieldA = sign_extend(fieldA, 21);

      fieldA += (addr & ~0x3);

      if (BIT(state->words[0],16) && state->_opcode == op_BC)
	CHECK_NULLIFY();
      else
	/* Checking for bl unconditionally FIX For Bug #553 */
	if((state->_opcode == op_BLC && BITS(state->words[0],16,17) == 2 )
	   ||(state->_opcode == op_BC && (BIT(state->words[0],16))))
	    CHECK_NULLIFY();
	  else
	    CHECK_COND_NULLIFY();



      write_instr_name();
      /* This address could be a label we know.  Convert it. */
      add_target(fieldA); /* For debugger. */
      state->flow = state->_opcode == op_BLC /* BL */
	  ? direct_call
	  : direct_jump;
	/* indirect calls are achieved by "lr blink,[status]; */
	/*      lr dest<- func addr; j [dest]" */

      strcat(formatString, "%s"); /* address/label name */
      my_sprintf(state, state->operandBuffer, formatString, post_address(state, fieldA));
      write_comments();
      break;

    case 14:

      /* Extension Instructions */

      FIELD_C_AC();
      FIELD_B_AC();

      write_instr_name();
      if (mul)
      {
	fieldA = fieldAisReg = 0;
	WRITE_FORMAT_x(A);
	WRITE_FORMAT_COMMA_x(B);
      }
      else
	WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      if (mul)
	my_sprintf(state, state->operandBuffer, formatString, 0, fieldB, fieldC);
      else
	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      break;

    case 15:

      /* ARCompact 16-bit Load/Add resister-register */

      FIELD_C_AC();
      FIELD_B_AC();
      FIELD_A_AC();

      write_instr_name();

      if (BITS(state->words[0],3,4) != 3)
      {
	WRITE_FORMAT_x_COMMA_LB(A);
	WRITE_FORMAT_x(B);
	WRITE_FORMAT_COMMA_x_RB(C);
      }
      else
      {
	WRITE_FORMAT_x(A);
	WRITE_FORMAT_COMMA_x(B);
	WRITE_FORMAT_COMMA_x(C);
      }
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      break;

    case 16:

    /* ARCompact 16-bit Add/Sub/Shift instructions */

      FIELD_C_AC();
      FIELD_B_AC();
      fieldA = FIELDA_AC(state->words[0]);
      fieldAisReg = 0;

      write_instr_name();
      WRITE_FORMAT_x(C);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_FORMAT_COMMA_x(A);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldA);
      break;

    case 17:

      /* add_s instruction, one Dest/Source can be any of r0 - r63 */

      if(info->mach == bfd_mach_arc_arcv2)
	{
	  CHECK_FIELD_H_EM();
	}
      else
	{
	  CHECK_FIELD_H_AC();
	}

      FIELD_B_AC();

      write_instr_name();
      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC);
      break;

    case 18:

      /* mov_s/cmp_s instruction, one Dest/Source can be any of r0 - r63 */

      if ((BITS(state->words[0],3,4) == 1) || (BITS(state->words[0],3,4) == 2))
      {
	CHECK_FIELD_H_AC();
      }
      else if (BITS(state->words[0],3,4) == 3)
      {
	FIELD_H_AC();
      }
      FIELD_B_AC();

      write_instr_name();
      if (BITS(state->words[0],3,4) == 3)
      {
	WRITE_FORMAT_x(C);
	WRITE_FORMAT_COMMA_x(B);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB);
      }
      else
      {
	WRITE_FORMAT_x(B);
	WRITE_FORMAT_COMMA_x(C);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      }
      break;

    case 19:

      /* Stack pointer-based instructions [major opcode 0x18] */

      if (BITS(state->words[0],5,7) == 5)
	fieldA = 28;
      else
      {
	FIELD_B_AC();
	fieldA = fieldB;
      }
      fieldB = 28; /* Field B is the stack pointer register */
      fieldC = (FIELDU_AC(state->words[0])) << 2;
      fieldCisReg = 0;

      write_instr_name();

      switch (BITS(state->words[0],5,7))
      {
	case 0:
	case 1:
	case 2:
	case 3:
	  WRITE_FORMAT_x_COMMA_LB(A);
	  WRITE_FORMAT_x(B);
	  WRITE_FORMAT_COMMA_x_RB(C);
	  break;
	case 4:
	case 5:
	  WRITE_FORMAT_x(A);
	  WRITE_FORMAT_COMMA_x(B);
	  WRITE_FORMAT_COMMA_x(C);
	  break;
      }
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      break;

    case 20:

      /* gp-relative instructions [major opcode 0x19] */

      fieldA = 0;
      fieldB = 26; /* Field B is the gp register */
      fieldC = FIELDS_AC(state->words[0]);
      switch (BITS(state->words[0],9,10))
      {
	case 0:
	case 3:
	  fieldC = fieldC << 2; break;
	case 2:
	  fieldC = fieldC << 1; break;
      }
      fieldCisReg = 0;

      write_instr_name();

      if (BITS(state->words[0],9,10) != 3)
      {
	WRITE_FORMAT_x_COMMA_LB(A);
	WRITE_FORMAT_x(B);
	WRITE_FORMAT_COMMA_x_RB(C);
      }
      else
      {
	WRITE_FORMAT_x(A);
	WRITE_FORMAT_COMMA_x(B);
	WRITE_FORMAT_COMMA_x(C);
      }
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      break;

    case 21:

      /* add/cmp/btst instructions [major opcode 28] */

      FIELD_B_AC();
      if (state->_opcode == op_Su5)
	fieldC = (BITS(state->words[0],0,4));
      else
	fieldC = (BITS(state->words[0],0,6));
      fieldCisReg = 0;
      write_instr_name();

      if (!BIT(state->words[0],7))
      {
	WRITE_FORMAT_x(B);
	WRITE_FORMAT_COMMA_x(B);
	WRITE_FORMAT_COMMA_x(C);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC);
      }
      else
      {
	WRITE_FORMAT_x(B);
	WRITE_FORMAT_COMMA_x(C);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      }
      break;

    case 22:

      /* ARCompact 16-bit instructions, General ops/ single ops */

      FIELD_C_AC();
      FIELD_B_AC();

      write_instr_name();

      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC);
      break;

    case 23:

      /* Shift/subtract/bit immediate instructions [major opcode 23] */

      FIELD_B_AC();
      fieldC = FIELDU_AC(state->words[0]);
      fieldCisReg = 0;
      write_instr_name();
      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC);
      break;

    case 24:

      /* ARCompact 16-bit Branch conditionally */

      if (state->_opcode == op_BL_S)
      {
	fieldA = (BITS(state->words[0],0,10)) << 2;
	fieldA = sign_extend(fieldA, 13);
      }
      else if (BITS(state->words[0],9,10) != 3)
      {
	fieldA = (BITS(state->words[0],0,8)) << 1;
	fieldA = sign_extend(fieldA, 10);
      }
      else
      {
	fieldA = (BITS(state->words[0],0,5)) << 1;
	fieldA = sign_extend(fieldA, 7);
      }
      fieldA += (addr & ~0x3);

      write_instr_name();
      /* This address could be a label we know.  Convert it. */
      add_target(fieldA); /* For debugger. */
      state->flow = state->_opcode == op_BL_S /* BL */
	  ? direct_call
	  : direct_jump;
	/* indirect calls are achieved by "lr blink,[status]; */
	/*      lr dest<- func addr; j [dest]" */

      strcat(formatString, "%s"); /* address/label name */
      my_sprintf(state, state->operandBuffer, formatString, post_address(state, fieldA));
      write_comments();
      break;

    case 25:

      /* ARCompact 16-bit Branch conditionally on reg z/nz */

      FIELD_B_AC();
      fieldC = (BITS(state->words[0],0,6)) << 1;
      fieldC = sign_extend (fieldC, 8);

      fieldC += (addr & ~0x3);
      fieldA = fieldAisReg = fieldCisReg = 0;

      write_instr_name();
      /* This address could be a label we know.  Convert it. */
      add_target(fieldC); /* For debugger. */
      state->flow = direct_jump;

      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(A);
      strcat(formatString, ",%s"); /* address/label name */
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldA, post_address(state, fieldC));
      write_comments();
      break;

    case 26:

      /* Zero operand Instructions */

      write_instr_name();
      state->operandBuffer[0] = '\0';
      break;

    case 27:

      /* j_s instruction */

      FIELD_B_AC();
      write_instr_name();
      strcat(formatString,"[%r]");
      my_sprintf(state, state->operandBuffer, formatString, fieldB);
      break;

    case 28:

      /* Load/Store with offset */

      FIELD_C_AC();
      FIELD_B_AC();
      switch (state->_opcode)
      {
	case op_LD_S :
	case op_ST_S :
	  fieldA = (FIELDU_AC(state->words[0])) << 2;
	  break;
	case op_LDB_S :
	case op_STB_S :
	  fieldA = (FIELDU_AC(state->words[0]));
	  break;
	case op_LDW_S :
	case op_LDWX_S :
	case op_STW_S :
	  fieldA = (FIELDU_AC(state->words[0])) << 1;
	  break;
      }
      fieldAisReg = 0;

      write_instr_name();

      WRITE_FORMAT_x_COMMA_LB(C);
      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(A);
      WRITE_FORMAT_RB();
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldA);
      write_comments();
      break;

    case 29:

      /* Load pc-relative */

      FIELD_B_AC();
      fieldC = 63;
      fieldA = (BITS(state->words[0],0,7)) << 2;
      fieldAisReg = 0;

      write_instr_name();

      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_FORMAT_COMMA_x(A);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC, fieldA);
      write_comments();
      break;

    case 30:

      /* mov immediate */

      FIELD_B_AC();
      fieldC = (BITS(state->words[0],0,7));
      fieldCisReg = 0;

      write_instr_name();

      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      write_comments();
      break;

    case 31:

      /* push/pop instructions */

      if (BITS(state->words[0],0,4) == 1)
      {
	FIELD_B_AC();
      }
      else if (BITS(state->words[0],0,4) == 17)
	fieldB = 31;

      write_instr_name();

      WRITE_FORMAT_x(B);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB);
      break;

    case 32:

      /* Single operand instruction */

      if (!BITS(state->words[0],22,23))
      {
	CHECK_FIELD_C();
      }
      else
      {
        if (BITS(state->words[0],24,26) == 3)
	  fieldC = BITS(state->words[0],6,8);
	else
	  FIELD_C();
	fieldCisReg = 0;
      }

      write_instr_name();

      if (!fieldC)
	state->operandBuffer[0] = '\0';
      else
      {
	WRITE_FORMAT_x(C);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, fieldC);
      }
      break;

  case 33:
    /* For trap_s and the class of instructions that have
       unsigned 6 bits in the fields B and C in A700 16 bit
       instructions */
    fieldC = FIELDC_AC(state->words[0]);
    fieldB = FIELDB_AC(state->words[0]);
    fieldCisReg = 0;
    fieldBisReg = 0;
    write_instr_name();
    strcat(formatString,"%d");
    my_sprintf(state,state->operandBuffer,formatString, ((fieldB << 3) | fieldC));
    break;

  case 34:
    /* For ex.di and its class of instructions within op_major_4
       This class is different from the normal set of instructions
       in op_major_4 because this uses bit 15 as .di and the second
       operand is actually a memory operand.
       This is of the class
       <op>.<di> b,[c] and <op>.<di> b,[limm]
    */


    /* field C is either a register or limm (different!) or u6 */

    FIELD_B();
    directMem = BIT(state->words[0],15);
	if (BITS(state->words[0],22,23) == 1 ) {
	    fieldCisReg = 0;
	    FIELD_C();
	}
	else {
	CHECK_FIELD_C();
    }

    if (fieldCisReg)
      state->ea_reg1 = fieldC;

    write_instr_name();
    WRITE_FORMAT_x_COMMA_LB(B);

    WRITE_FORMAT_x_RB(C);

    WRITE_NOP_COMMENT();
    my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
    write_comments();
    break;

  case 35:

    /* sub_s.ne instruction */

    FIELD_B_AC();
    write_instr_name();
    strcat(formatString,"%r,%r,%r");
    my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldB);
    break;

  case 36:

    FIELD_B_AC();

    write_instr_name();

    WRITE_FORMAT_x(B);
    WRITE_NOP_COMMENT();
    my_sprintf(state, state->operandBuffer, formatString, fieldB);

    break;

  /* START ARC LOCAL */
  case 44:
      /* rtsc instruction */
  	  FIELD_B();
  	  fieldA=fieldB;
      /* The source operand has no use.  */
#ifdef ARC_NPS_CMDS
      FIELD_B();
      fieldC = fieldCisReg = 0;
      write_instr_name();
      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC);
#else
      fieldB = fieldBisReg = 0;

      write_instr_name();
      WRITE_FORMAT_x(A);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldA,
		 fieldB);
#endif
      break;
  /* END ARC LOCAL */

    /*******SIMD instructions decoding follows*************/
  case 37:
  case 39:
  case 41:
    /*fieldA is vr register
      fieldB is I register
      fieldC is a constant
      %*,[%(,%<]
      or
      %*,%(,%<
      or
      %*,%(,%u
    */

    CHECK_FIELD_A();

    CHECK_FIELD_B();
    if (decodingClass == 41)
       {
	 FIELD_C();
       }
    else
      {
	FIELD_U8();

	if (simd_scale_u8>0)
	  fieldC = fieldC << simd_scale_u8;
      }

    fieldCisReg = 0;

    write_instr_name();
    (decodingClass == 37 ? WRITE_FORMAT_x_COMMA_LB(A) :
			   WRITE_FORMAT_x_COMMA(A));
    WRITE_FORMAT_x_COMMA(B);
    (decodingClass == 37 ?  WRITE_FORMAT_x_RB(C):
			    WRITE_FORMAT_x(C));
    WRITE_NOP_COMMENT();
    my_sprintf(state,state->operandBuffer, formatString, fieldA, fieldB, fieldC);


    break;
  case 38:
    /* fieldA is a vr register
       fieldB is a ARC700 basecase register.
       %*,[%b]
    */
    CHECK_FIELD_A();
    CHECK_FIELD_B();

    write_instr_name();
    WRITE_FORMAT_x_COMMA_LB(A);
    WRITE_FORMAT_x_RB(B);
    WRITE_NOP_COMMENT();
    my_sprintf(state,state->operandBuffer, formatString, fieldA, fieldB);

    break;
  case 40:
    /* fieldB & fieldC are vr registers
       %(,%)
       or
       %B,%C
       or
       %(,%C
    */
    CHECK_FIELD_B();
    CHECK_FIELD_C();

    write_instr_name();
    WRITE_FORMAT_x(B);
    WRITE_FORMAT_COMMA_x(C);
    my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
    break;

  case 42:
    /* fieldA, fieldB, fieldC are all vr registers
       %*, %(, %) */
    CHECK_FIELD_A();
    CHECK_FIELD_B();
    FIELD_C();

    write_instr_name();
    WRITE_FORMAT_x(A);
    WRITE_FORMAT_COMMA_x(B);
    WRITE_FORMAT_COMMA_x(C);
    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
    break;

  case 43:
    /* Only fieldC is a register
     %C*/
    CHECK_FIELD_C();

    if (BITS(state->words[0], 17, 23) == 55)
      fieldCisReg = 0;

    write_instr_name();
    WRITE_FORMAT_x(C);
    my_sprintf(state, state->operandBuffer, formatString, fieldC);
    break;

    /***************SIMD decoding ends*********************/

  case 45:
    CHECK_FIELD_B();
    if(BITS(state->words[0],22,23) == 3)
      {
	CHECK_COND();
	if (BITS(state->words[0],5,5))
	  {
	    /*shimm*/
	    FIELD_C();
	    fieldCisReg = 0;
	  }
	else
	  {
	    if (is_limm)
	      {
		FIELD_C();
		if (!fieldCisReg)
		  fieldC = fieldB;
	      }
	    else
	      {
		CHECK_FIELD_C();
	      }
	  }
      }
    else if (BITS(state->words[0],22,23) == 0)
      {
	if (is_limm)
	  {
	    FIELD_C();
	    if (!fieldCisReg)
	      fieldC = fieldB;
	  }
	else
	  {
	    CHECK_FIELD_C();
	  }
      }
    else if (BITS(state->words[0],22,23) == 1)
      {
	FIELD_C();
	fieldCisReg = 0;
      }
    else
      {
	fieldC = FIELDS(state->words[0]);
	fieldCisReg = 0;
      }

    write_instr_name();
    WRITE_FORMAT_x_COMMA_LB(B);
    /* Try to print B as an aux reg if it is not a core reg. */
    usesAuxReg = 1;
    WRITE_FORMAT_x(C);
    WRITE_FORMAT_RB();
    my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
    write_comments();
    break;
  case 46:
    /*For ARCtangent 32-bit instructions with 1 operand*/
    CHECK_FIELD_C();
    write_instr_name();
    WRITE_FORMAT_LB_Rx_RB(C);
    my_sprintf(state, state->operandBuffer, formatString, fieldC);
    write_comments();
    break;
  case 47:
    /* For LDIs*/
    FIELD_B();
    switch(BITS(state->words[0],22,23))
      {
      case 0:
	if (is_limm)
	  {
	    FIELD_C();
	    if (!fieldCisReg)
	      fieldC = fieldB;
	  }
	else
	  {
	    CHECK_FIELD_C();
	  }
	break;
      case 3:
	CHECK_COND();
      case 1:
	FIELD_C();
	fieldCisReg = 0;
	break;
      case 2:
	fieldC = FIELDS(state->words[0]);
	fieldCisReg = 0;
	break;
      }

    write_instr_name();
    WRITE_FORMAT_x_COMMA_LB(B);
    WRITE_FORMAT_x(C);
    WRITE_FORMAT_RB();
    my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
    write_comments();
    break;

  case 48:
    /* ARCompact 16-bit sub_s a,b,c */

      FIELD_C_AC();
      FIELD_B_AC();
      FIELD_A_AC();

      write_instr_name();
      WRITE_FORMAT_x(A);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      break;

  case 49:
    /* ARCompact 16-bit ld_s.as a,[b,c] */

      FIELD_C_AC();
      FIELD_B_AC();
      FIELD_A_AC();

      write_instr_name();
      WRITE_FORMAT_x_COMMA_LB(A);
      WRITE_FORMAT_x(B);
      WRITE_FORMAT_COMMA_x_RB(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      break;

  case 50:
    /* ARCompact 16-bit add_s r,b,u6 */

      FIELD_B_AC();

      fieldAisReg = 0;
      fieldA = (BITS(state->words[0], 4,6) << 3) | BITS(state->words[0], 0,2);
      fieldC = (BITS(state->words[0], 7, 7)) & 0x01;

      write_instr_name();
      WRITE_FORMAT_x(C);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_FORMAT_COMMA_x(A);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldA);
      break;

#ifdef NPS_ARCv2
    case 51:

      /* Only EM!*/

      /* Check H field*/
      fieldB = ((FIELDA_AC(state->words[0]) & 0x03) << 3);
      fieldB |= FIELDC_AC(state->words[0]);
      if (fieldB == 0x1E)
	{
	  is_limm++;
	  fieldBisReg = 0;
	  PUT_NEXT_WORD_IN(fieldB);
	  limm_value = fieldB;

	  /*first argument is 0*/
	  fieldA = 0;
	  fieldAisReg = 0;
	}
      else
	{
	  fieldA = fieldB;
	}

      /*short s3*/
      fieldC = FIELDB_AC(state->words[0]);
      fieldC = (fieldC == 7) ? -1 : fieldC;
      fieldCisReg = 0;

      write_instr_name();
      WRITE_FORMAT_x(A);
      WRITE_FORMAT_COMMA_x(B);
      WRITE_FORMAT_COMMA_x(C);
      WRITE_NOP_COMMENT();
      my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
      break;

    case 52:

      /* mov_s/cmp_s instruction */

      if (BITS(state->words[0],2,4) == 3)
	{
	  /* mov 0/h, s3*/
	  fieldC = ((FIELDA_AC(state->words[0]) & 0x03) << 3);
	  fieldC |= FIELDC_AC(state->words[0]);
	  if (fieldC == 0x1E)
	    {
	      fieldCisReg = 0;
	      fieldC = 0;
	    }
	}
      else
	{
	  CHECK_FIELD_H_EM();
	}

      if (BIT(state->words[0], 2) && (BITS(state->words[0],2,4) != 7))
	{
	  /*short s3*/
	  fieldB = FIELDB_AC(state->words[0]);
	  fieldB = (fieldB == 7) ? -1 : fieldB;
	  fieldBisReg = 0;
	}
      else
	{
	  FIELD_B_AC();
	}

      write_instr_name();
      if (BIT(state->words[0],2) && (BITS(state->words[0],2,4) != 7))
      {
	/*<op> h(limm,0), s3*/
	WRITE_FORMAT_x(C);
	WRITE_FORMAT_COMMA_x(B);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB);
      }
      else
      {
	/*<op> b, h(limm) */
	WRITE_FORMAT_x(B);
	WRITE_FORMAT_COMMA_x(C);
	WRITE_NOP_COMMENT();
	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
      }
      break;

  case 53:
    /*mov_s g,h*/
    CHECK_FIELD_H_EM();

    /*Check field g*/
    fieldB = BITS(state->words[0],3,4) << 3;
    fieldB |= FIELDB_AC(state->words[0]);
    if (fieldB == 30)
      {
	fieldBisReg = 0;
	fieldB = 0;
      }

    write_instr_name();
    WRITE_FORMAT_x(B);
    WRITE_FORMAT_COMMA_x(C);
    WRITE_NOP_COMMENT();
    my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
    break;

  case 54:
    /* ld_s r0/1/2/3, [h,u5]*/
    CHECK_FIELD_H_EM();

    /*Check r-field*/
    fieldA = BITS(state->words[0],8,9);

    /*Check u-field*/
    fieldB = BITS(state->words[0],10,10) << 2;
    fieldB |= BITS(state->words[0],3,4);
    fieldB <<= 2;
    fieldBisReg = 0;

    write_instr_name();
    WRITE_FORMAT_x_COMMA_LB(A);
    WRITE_FORMAT_x(C);
    WRITE_FORMAT_COMMA_x_RB(B);
    WRITE_NOP_COMMENT();
    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldC, fieldB);
    break;

  case 55:
    /*s11 field*/
    fieldC = BITS(((signed int)state->words[0]),5,10) << 3;
    fieldC |= BITS(state->words[0],0,2);
    fieldC <<= 2;
    fieldCisReg = 0;

    write_instr_name();
    WRITE_FORMAT_COMMA_x_RB(C);
    WRITE_NOP_COMMENT();
    my_sprintf(state, state->operandBuffer, formatString, fieldC);
    break;

  case 56:
    /*jli_s/ei_s*/
    fieldC = BITS(state->words[0],0,9);
    fieldCisReg = 0;

    write_instr_name();
    WRITE_FORMAT_x(C);
    my_sprintf(state, state->operandBuffer, formatString, fieldC);
    break;

  case 57:
    /*enter_s/leave_s*/
    fieldC = BITS(state->words[0],8,10) << 4;
    fieldC |= BITS(state->words[0],1,4);
    fieldCisReg = 0;

    write_instr_name();

    /*print*/
    char *bp;
    bp = state->operandBuffer;
    sprintf(bp, "{");
    bp = bp+strlen(bp);

    if (fieldC & 0x0F)
      {
	sprintf(bp, "r13-r%d", (12 + (fieldC & 0x0F)));
	bp = bp+strlen(bp);
      }

    if (fieldC & 0x10)
      {
	sprintf(bp, ",fp");
	bp = bp+strlen(bp);
      }
    if (fieldC & 0x20)
      {
	sprintf(bp, ",blink");
	bp = bp+strlen(bp);
      }

    if (fieldC & 0x40)
      {
	sprintf(bp, ",pcl");
	bp = bp+strlen(bp);
      }

    sprintf(bp, "}");
    break;

  case 58:
    /*ldi_s*/
    fieldC = BITS(state->words[0], 4,7) << 3;
    fieldC |= BITS(state->words[0], 0,2);
    fieldCisReg = 0;

    FIELD_B_AC();

    write_instr_name();
    WRITE_FORMAT_x_COMMA_LB(B);
    WRITE_FORMAT_x_RB(C);
    my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
    break;
#endif /* #ifdef NPS_ARCv2 */

#ifdef ARC_NPS_CMDS
  case 51:
    {
        int v;
      /* ARCompact 16-bit instructions, + short int */

        FIELD_C_AC16();
        FIELD_B_AC16();
        write_instr_name();
        v = state->words[0] & 0x0000FFFF;
        strcat(formatString,"%r,%r,%r,0x%04x");
        my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC,v);
        break;
    }

  case 52:
    {
      /* ARCompact 16-bit instructions, + long int */

        FIELD_C_AC();
        FIELD_B_AC();
        fieldA = 62;
        CHECK_FIELD(fieldA);
        write_instr_name();
        strcat(formatString,"%r,%r,%r,0x%08x");
        my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC,limm_value);
        break;
    }
  case 53:
    {
        int v;
      /* ARCompact 16-bit instructions, + short int */

        FIELD_C_AC16();
        FIELD_B_AC16();
        fieldA = 62;
        CHECK_FIELD(fieldA);
        write_instr_name();
        v = state->words[0] & 0x0000FFFF;
        strcat(formatString,"%r,%r,%r,0x%04x,0x%08x");
        my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC,v,limm_value);
        break;
    }

  case 54:
    {
        int v;
        int r;
        int subOpcode;
        int major;
        FIELD_C_AC16();
        FIELD_B_AC16();
        if ( fieldB > 3 ) fieldB -= 8;
        if ( fieldC > 3 ) fieldC -= 8;
        subOpcode = (state->words[0] & 0x000f0000) >> 16;
        major = (state->words[0] & 0xf8000000) >> 27;
        v = state->words[0] & 0x0000FFFF;
        if ( ( v & 0x00008000 ) != 0 ) v |= 0xffff0000; /* sign extension */
        r = (fieldB<<3) + fieldC;
        if ( major == 9 ) {
            if ( ( (state->words[0] & 0x00100000) != 0 ) && (subOpcode != 8) && (subOpcode != 9) ) flag = 1;
        }
        else { /* for all subopcode in major 0xB */
        	write_instr_name();
        	if (((BITS(state->words[0],16,20)) >= 8) && ((BITS(state->words[0],16,20)) <= 14)) {
        		strcat(formatString,"%r,[0x%x]");
        		v = (VALID_MSB_CCM1 & 0xFFFF0000) | (state->words[0] & 0x0000FFFF);
        	}
        	else
        		strcat(formatString,"%r,[cm:0x%04x]");
        	my_sprintf(state, state->operandBuffer, formatString, r,v);
        	break;
        }
        write_instr_name();
        switch (subOpcode) {
            case 12:  // orl
            case 13:  // andl
            case 14:  // xorl
            	v &= 0x0000FFFF; /* No sign ext. for andl,orl,xorl*/
            case 10:  // addl
            case 11:  // subl
                strcat(formatString,"%r,%r,0x%04x");
                my_sprintf(state, state->operandBuffer, formatString, r, r,v);
                break;
            case 8:  // movh
            case 9:  // movl
            	v &= 0x0000FFFF; /* No sign ext. for movl,movh*/
            	if (strstr(instrName,".cl") == NULL) {
                    strcat(formatString,"%r,%r,0x%04x");
                    my_sprintf(state, state->operandBuffer, formatString, r, r,v);
                    break;
            	}
           default:
                strcat(formatString,"%r,0x%04x");
                my_sprintf(state, state->operandBuffer, formatString, r,v);
                break;
        }
        break;
    }
  case 55:
    {
        int v1;
        int v2;
        int v3;
        int subOp;
        FIELD_C_AC16();
        FIELD_B_AC16();
        if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
        v1 = (state->words[0] & 0x0000001F) >> 0;
        v2 = (state->words[0] & 0x000003E0) >> 5;
        v3 = (state->words[0] & 0x00007C00) >> 10;
        subOp = (state->words[0] & 0x000F0000) >> 16;
        switch (subOp) {
	case 3:
		if ( ((state->words[0] & 0x00000040) >> 6) == 0 ) {
			if ( state->words[0] & 0x00008000 )
				instrName = "fbset";
			else
				instrName = "fbclr";
		}
		else {
			if ( (state->words[0] & 0x00000020) != 0 )
				instrName = "decode1.cl";
		}
		break;
        case 4:
        	if ( state->words[0] & 0x00008000 )
        		instrName = "encode1";
        	else
        		instrName = "encode0";
        	break;
        default:
        	break;
        }
        write_instr_name();
        if ( state->words[0] & 0x00008000 ) {
		if ((subOp != 3) && (subOp != 4))
        		strcat(state->instrBuffer,".cl");
            switch (subOp) {
                 case 3:
                	if ( state->words[0] & 0x00000020 ) {
                		strcat(formatString,"%r,%r,%d");
                		my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,v1);
                	 }
                	 else {
                		 strcat(formatString,"%r,%r,%r,%d,%d");
                		 my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,v1,(v3+1));
                	 }
                     break;
                 case 4:
                     strcat(formatString,"%r,%r,%d,%d");
                     my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,v1,(v3+1));
                     break;
                 default:
                     strcat(formatString,"%r,%r,%d,%d,%d");
                     my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,v2,v1,(v3+1));
                     break;
            }
        }
        else {
            switch (subOp) {
                case 3:
                	strcat(formatString,"%r,%r,%r,%d,%d");
                	my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,v1,(v3+1));
                	break;
                case 4:
                    strcat(formatString,"%r,%r,%d,%d");
                    my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,v1,(v3+1));
                    break;
                default:
                    strcat(formatString,"%r,%r,%r,%d,%d,%d");
                    my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,v2,v1,(v3+1));
                    break;
            }
        }
        break;
    }
  case 56:
    {
        int v1;
        int v2;
        int v3;
        int v4;
        int v5;
        int v6;
        FIELD_C_AC();
        FIELD_B_AC();
        fieldA = 62; // dummy value only to update limm_value
        CHECK_FIELD(fieldA);
        v1 = (limm_value & 0x0000001F) >> 0;
        v2 = (limm_value & 0x000003E0) >> 5;
        v5 = (limm_value & 0x00007C00) >> 10;
        v6 = (limm_value & 0x000F8000) >> 15;
        v3 = (limm_value & 0x01F00000) >> 20;
        v4 = (limm_value & 0x3E000000) >> 25;
        write_instr_name();
        if ( limm_value & 0x80000000 ) {
        	strcat(state->instrBuffer,".cl");
            strcat(formatString,"%r,%r,%r,%d,%d,%d,%d,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,v5,v1,(v3+1),v6,v2,(v4+1));
        }
        else {
            strcat(formatString,"%r,%r,%r,%d,%d,%d,%d,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,v5,v1,(v3+1),v6,v2,(v4+1));
        }
        break;
    }
  case 57:
    {
        int v1;
        int v2;
        int v3;
        int v4;
        int v5;
        int v6;
        FIELD_C_AC();
        FIELD_B_AC();
        fieldA = 62; // dummy value only to update limm_value
        CHECK_FIELD(fieldA);
        v1 = (limm_value & 0x0000001F) >> 0;
        v2 = (limm_value & 0x000003E0) >> 5;
        v3 = (limm_value & 0x00007C00) >> 10;
        v4 = (limm_value & 0x000F8000) >> 15;
        v5 = (limm_value & 0x06000000) >> 25;
        v6 = (limm_value & 0x18000000) >> 27;
        write_instr_name();
        if ( limm_value & 0x80000000 ) {
        	strcat(state->instrBuffer,".cl");
            strcat(formatString,"%r,%r,%d,%d,%d,%d,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,v3,v5,v1,v4,v6,v2);
        }
        else {
            strcat(formatString,"%r,%r,%r,%d,%d,%d,%d,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,v3,v5,v1,v4,v6,v2);
        }
        break;
    }
  case 58:
    {
        int s1;
        int s2;
        int s3;
        int s4;
        int d1;
        int d2;
        int d3;
        int d4;
        int m1;
        int m2;
        int m3;
        int m4;
        int v16;
        int mov3bCase = 0;
        FIELD_C_AC16();
        FIELD_B_AC16();
        fieldA = 62; // dummy value only to update limm_value
        CHECK_FIELD(fieldA);
        v16 = state->words[0] & 0x0000FFFF;
        s1 = (limm_value & 0x0000001F) >> 0;
        s2 = (limm_value & 0x000003E0) >> 5;
        d1 = (limm_value & 0x00007C00) >> 10;
        d2 = (limm_value & 0x000F8000) >> 15;
        s3 = (limm_value & 0x01F00000) >> 20;
        m1 = (limm_value & 0x06000000) >> 25;
        m2 = (limm_value & 0x18000000) >> 27;
        m3 = (limm_value & 0x60000000) >> 29;
        m4 = ( (limm_value & 0x80000000) >> (31-1) ) | ( (v16 & 0x8000) >> 15 );
        s4 = (v16 & 0x001F) >> 0;
        d3 = (v16 & 0x03E0) >> 5;
        d4 = (v16 & 0x7C00) >> 10;
        if ( ( d3 == d4 ) && (m4 == 2) ) {
        	 instrName = (strstr(instrName,"cl") != NULL ) ? "mov3bcl" : "mov3b";
        	 mov3bCase = 1;
        }
        write_instr_name();
        if (strstr(state->instrBuffer,"cl") != NULL ) {
        	if ( mov3bCase == 0 ) {
                strcat(formatString,"%r,%r,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d");
                my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,d1,m1,s1,d2,m2,s2,d3,m3,s3,d4,m4,s4);
        	}
        	else {
                strcat(formatString,"%r,%r,%d,%d,%d,%d,%d,%d,%d,%d,%d");
                my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,d1,m1,s1,d2,m2,s2,d3,m3,s3);
        	}
        }
        else {
           	if ( mov3bCase == 0 ) {
                strcat(formatString,"%r,%r,%r,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d");
                my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,d1,m1,s1,d2,m2,s2,d3,m3,s3,d4,m4,s4);
           	}
           	else {
                strcat(formatString,"%r,%r,%r,%d,%d,%d,%d,%d,%d,%d,%d,%d");
                my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,d1,m1,s1,d2,m2,s2,d3,m3,s3);
           	}
        }
        break;
    }
  case 59:
    {
        int v1;
        int v2;
        int v3;
        int v4;
        int v5;
        int subOpcode;
        FIELD_C_AC();
        FIELD_B_AC();
        fieldA = 62; // dummy value only to update limm_value
        CHECK_FIELD(fieldA);
        v1 = (limm_value & 0x0000001F) >> 0;
        v2 = (limm_value & 0x000003E0) >> 5;
        v3 = (limm_value & 0x00007C00) >> 10;
        v4 = (limm_value & 0x000F8000) >> 15;
        v5 = (limm_value & 0x01F00000) >> 20;
        subOpcode = BITS(state->words[0],0,4);
        write_instr_name();
        if ( subOpcode == 1 ) {
            if ( limm_value & 0x80000000 ) {
        	    strcat(state->instrBuffer,".cl");
                strcat(formatString,"%r,%r,%d,%d,%d,%d,%d");
                my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,v5,v1,v2,v3,v4);
            }
            else {
                strcat(formatString,"%r,%r,%r,%d,%d,%d,%d,%d");
                my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,v5,v1,v2,v3,v4);
            }
        }
        if ( subOpcode == 2 ) {
            if ( limm_value & 0x80000000 ) {
        	    strcat(state->instrBuffer,".cl");
                strcat(formatString,"%r,%r,%d,%d,%d,%d,%d");
                my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,v1,v2,v3,v4,v5);
            }
            else {
                strcat(formatString,"%r,%r,%r,%d,%d,%d,%d,%d");
                my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,v1,v2,v3,v4,v5);
            }
        }
        break;
    }
  case 60:
    {
        int size;
        int s1;
        int s2;
        int d;
        int us;
        int entry;
        int mode;
        FIELD_C_AC16();
        FIELD_B_AC16();
    	subopcode = BITS(state->words[0],0,4);
        if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
        size = (state->words[0] & 0x000003E0) >> 5;
        mode = (state->words[0] & 0x00000060) >> 5;
        s1 =  (state->words[0] & 0x00000C00) >> 10;
        s2 =  (state->words[0] & 0x00003000) >> 12;
        d  =  (state->words[0] & 0x0000C000) >> 14;
        us =  (state->words[0] & 0x00004000) >> 14;
        entry = (state->words[0] & 0x00000F00) >> 8;
        if ( subopcode == 16 ) {
        	switch (mode) {
    	        case 0:
    		        instrName = "calcsd"; break;
    	        case 1:
    		        instrName = "calcbsd"; break;
    	        case 2:
    		        instrName = "calckey"; break;
        	}
        }
        write_instr_name();
        switch (subopcode) {
          case 0: // addb
          case 4: // subb
          case 5: // adcb
          case 6: // sbcb
        	  if ( us != 0 ) strcat(state->instrBuffer,".sx");
              strcat(formatString,"%r,%r,%r,%d,%d,%d");
              my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,s1*8,s2*8,(size+1));
              break;
          case 9: // notb
              strcat(formatString,"%r,%r,%d,%d");
              my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,s2*8,(size+1));
              break;
          case 10: // cntbb
              strcat(formatString,"%r,%r,%d,%d");
              my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,s2*8,(size+1));
              break;
          case 16: // calcsd, calcbsd, calckey
        	  if ( us != 0 ) strcat(state->instrBuffer,".xd");
        	  if ( mode == 0 ) {
                  strcat(formatString,"%r,%r,%r,%d");
                  my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,1 << entry);

        	  }
        	  else {
                  strcat(formatString,"%r,%r,%r");
                  my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC);
        	  }
              break;
         default:
            strcat(formatString,"%r,%r,%r,%d,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldC,s1*8,s2*8,(size+1));
            break;
        }
        break;
    }
  case 61:
    {
        int imm;
        int ri;
        int s1;
        int s2;
        int mode;
        FIELD_C_AC16();
        FIELD_B_AC16();
        if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
    	imm = BITS(state->words[0],5,8);
    	ri = BITS(state->words[0],9,9);
        s1 = BITS(state->words[0],10,11);
        s2 = BITS(state->words[0],12,13);
        mode  =  BITS(state->words[0],14,15);
        switch (mode) {
        case 0:
        	instrName = "divm";
        	break;
        case 1:
        	instrName = "div";
        	break;
        case 2:
        	instrName = "mod";
        	break;
        case 3:
        	instrName = "div???";
        	break;
        }
        write_instr_name();
        if ( ri == 0 ) {
            strcat(formatString,"%r,%r,%r,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,fieldC,s1*8,s2*8);
        }
        else {
            strcat(formatString,"%r,%r,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldB,imm,s1*8);
        }
        break;
    }
  case 62:
    {
	    int t0;
	    int t1;
	    CHECK_FIELD_A();
	    CHECK_FIELD_B();
	    CHECK_FIELD_C();
	    t0 = limm_value & 0xFFFF;
	    t1 = (limm_value >> 16) & 0xFFFF;
	    write_instr_name();
        WRITE_FORMAT_x(A);
	    WRITE_FORMAT_COMMA_x(B);
	    if ( fieldCisReg == 0 ) {
            strcat(formatString,",0x%04x,0x%04x");
            my_sprintf(state, state->operandBuffer, formatString, fieldA,fieldB,t0,t1);
	    }
	    else {
	    	WRITE_FORMAT_COMMA_x(C);
	    	my_sprintf(state, state->operandBuffer, formatString, fieldA,fieldB,fieldC);
	    }
        break;
    }
  case 63:
    {
    	int width5;
    	int perm3;
    	int nonlin1;
    	int base2;
    	int index1_3;
    	int index0_3;
    	int subOpcode;
    	subOpcode = BITS(state->words[0],16,20);
    	fieldB = BITS(state->words[0],24,26);
    	if ( fieldB >=4 ) fieldB += 8;
     	fieldC = BITS(state->words[0],21,23);
    	if ( fieldC >=4 ) fieldC += 8;
    	fieldA = BITS(state->words[0],11,15);
    	width5 = BITS(state->words[0],6,10);
    	nonlin1 = BITS(state->words[0],5,5);
    	perm3 = BITS(state->words[0],2,4);
    	base2 = BITS(state->words[0],0,1);
    	index1_3 = BITS(state->words[0],5,7);
    	index0_3 = BITS(state->words[0],8,10);
        write_instr_name();
        WRITE_FORMAT_x(A);
	    WRITE_FORMAT_COMMA_x(B);
	    WRITE_FORMAT_COMMA_x(C);
        strcat(formatString,",%d,%d,%d,%d");
        switch (subOpcode) {
            case 24:
        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC,width5+1,perm3,nonlin1,base2);
        	    break;
            case 25:
            case 26:
            case 27:
            case 28:
        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC,width5+1,perm3+1,base2,nonlin1);
        	    break;
            case 29:
        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC,index0_3,index1_3,perm3,base2+4);
        	    break;
        }
        break;
    }
  case 64:
    {
        int r;
        int cl;
        int imm;
        int dst;
        int size;
        FIELD_C_AC16();
        FIELD_B_AC16();
        if ( fieldB > 3 ) fieldB -= 8;
        if ( fieldC > 3 ) fieldC -= 8;
        if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
        cl = (state->words[0] & 0x00008000) ? 1 : 0;
        r = (fieldB<<3) + fieldC;
        imm = (state->words[0] & 0x0000001F) | ( (state->words[0] & 0x00007000) >> (12-5));
        dst = (state->words[0] & 0x000003E0) >> 5;
        size = (state->words[0] & 0x00000C00) >> 10;
        size = 1 << size;
        write_instr_name();
        if (cl != 0) {
        	strcat(state->instrBuffer,".cl");
            strcat(formatString,"%r,%d,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString, r,imm,dst,size);
        }
        else {
            strcat(formatString,"%r,%r,%d,%d,%d");
            my_sprintf(state, state->operandBuffer, formatString, r, r,imm,dst,size);
        }
        break;
    }
  case 65:
    {
    	int ib;
    	int c;
       	int num2;
       	int num3;
       	int num4;
       	int num6;
       	int num4active = 0;
       	int subopcode2 = BITS(state->words[0],22,23);
    	subopcode = BITS(state->words[0],16,21);
    	switch (subopcode) {
    	    case 0x12:
    	    case 0x27:
    	    case 0x28:
    		    num4active = 1;
    		    break;
    	}
    	switch (subopcode2) {
    	    case 0:
    	        CHECK_FIELD_C();
     	        CHECK_FIELD_B();
    	      	CHECK_FIELD_A();
    	        write_instr_name();
    	        WRITE_FORMAT_x(A);
    	        switch (subopcode) {
    	        	case 0x12:
    	        		strcat(formatString,",[cjid:%r],%r,%r");
    	        		my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, fieldC);
    	        		break;
    	        	case 0x15:
    	        		strcat(formatString,",[cm:%r],%r");
    	        		my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
    	        		break;
        	        case 0x10:
        	        case 0x11:
        	        case 0x26:
        	        case 0x28:
        	        case 0x2B:
       	        	    strcat(formatString,",[cm:%r],%r,%r");
       	        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, fieldC);
           	        	break;
    	            default:
    	    	        WRITE_FORMAT_COMMA_x(B);
    	    	        WRITE_FORMAT_COMMA_x(C);
    	    	        my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
    	    	        break;
     	        }
    	        break;
    	    case 1:
     	        CHECK_FIELD_B();
    	      	CHECK_FIELD_A();
    	        c = BITS(state->words[0],11,11);
    	        ib = BITS(state->words[0],10,10);
      	        num2 = BITS(state->words[0],6,7);
      	        num3 = BITS(state->words[0],6,8);
      	        num4 = BITS(state->words[0],6,9);
       	        num6 = BITS(state->words[0],6,11);
    	        if ( num2 == 0 )  num2 = 4;
    	        if ( num3 == 0 )  num3 = 8;
    	        if ( num6 == 0 ) num6 = 64;
    	        if ( ( subopcode == 0x26 ) && ( c == 0 ) ) instrName = "sidxalc";
    	        if ( ( subopcode == 0x10 ) && ( num3 == 1 ) ) instrName = "sbdalc";
    	        if ( ( subopcode == 0x2B ) && ( c == 0 ) ) instrName = "sjobalc";
    	        write_instr_name();
    	        WRITE_FORMAT_x(A);
//    	        WRITE_FORMAT_COMMA_x(B);
    	        switch (subopcode) {
    	        case 0x12:
        	        strcat(formatString,",[cjid:%r],%r,%d");
        	        my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, num4);
    	        	break;
    	        case 0x15:
//    	        	strcat(formatString,",%d");
//    	        	my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, num6);
        	        strcat(formatString,",[cm:%r],%d");
        	        my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, num6);
    	        	break;
    	        case 0x26:
   	        	if ( c == 0 ) {
   	           	        WRITE_FORMAT_COMMA_x(B);
        	        	my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB);
    	        	}
    	        	else {
       	        	    strcat(formatString,",[cm:%r],%r,%d");
       	        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, num3);
     	        	}
    	        	break;
    	        case 0x28:
    	        case 0x11:
   	        	    strcat(formatString,",[cm:%r],%r,%d");
   	        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, num3);
       	        	break;
    	        case 0x2B:
    	        	if (c) {
    	        		strcat(formatString,",[cm:%r],%r,%d");
    	        		my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, num2);
    	        	}
    	        	else {
    	        		strcat(formatString,",%r");
    	        		my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB);
    	        	}
       	        	break;
    	        case 0x10:
    	        	if ( num3 == 1 ) {
       	        	    strcat(formatString,",%r,%d");
       	        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, ib);
    	        	}
    	        	else {
       	        	    strcat(formatString,",[cm:%r],%r,%d,%d");
       	        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, ib, num3);
    	        	}
    	        	break;
    	        default:
        	        WRITE_FORMAT_COMMA_x(B);
        	        if ( c == 0 ) {
        	        	strcat(formatString,",%d");
        	        	my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, subopcode != 0x10 ? (num4active != 1 ? num3 : num4) : ib);
        	        }
        	        else {
        	        	strcat(formatString,",%d,%d");
        	        	my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, ib, num4active != 1 ? num3 : num4);
        	        }
    	        	break;
    	        }
    	        break;
    	    default:
    	        write_instr_name();
	        	strcat(formatString,"????");
	        	my_sprintf(state, state->operandBuffer, formatString);
	        	break;
    	}
    	break;
    case 66:
        {
/*  the format 'cp  dst,[dst_addr_type:d1,d2],[src_addr_type:s1,s2],size' */
        	int t;
        	int m;
//        	int ec;
        	int s;
        	int o;
        	int bdOff;
        	int size10;
        	int ccm;
        	int imm;
        	int i2;
        	int subOpc1;
        	int subOpc2;
        	int major;
        	int size;
        	int typeAll;
        	char *dstStr;
        	char *srcStr;
        	int numDp;
        	int numSp;
        	char name[20];
          	fieldA = 62; // dummy value only to update limm_value
            CHECK_FIELD(fieldA);
           	fieldA = BITS(state->words[0],21,26);
           	fieldB = BITS(state->words[0],16,20);
           	fieldC = BITS(state->words[0],11,15);
            fieldAisReg = 1;
            fieldBisReg = 1;
            fieldCisReg = 1;
           	if ( fieldA == 62 ) {
           		fieldA = 0;
           		fieldAisReg = 0;
           	}
            subOpc1 = BITS(state->words[0],0,4);
        	major = BITS(state->words[0],27,31);
        	ccm = limm_value & 0x0000ffff;
        	size = (limm_value & 0x000ffc00) >> 10;
        	subOpc2 = ((limm_value & 0xe0000000) >> 29) & 0x07;
        	m = (limm_value & 0x10000000) >> 28;
        	t = (limm_value & 0x08000000) >> 27;
        	c = (limm_value & 0x04000000) >> 26;
        	i2 = (limm_value & 0x03000000) >> 24;
        	imm = (limm_value & 0x00ff0000) >> 16;
        	s = (limm_value & 0x02000000) >> 25;
        	o = (limm_value & 0x01000000) >> 24;
        	bdOff = limm_value & 0x000003FF;
        	typeAll = (limm_value & 0x0f000000) >> 24;
        	size10 = (limm_value & 0x003FF000) >> 12;
        	if ( size10 == 0 ) size10 = 0x400;
        	if ( ( (typeAll & 0x03) == 0x01 ) && ( imm == 0 ) ) imm = 0x100;
        	dstStr = "??:";
        	srcStr = "??:";
        	numDp = numSp = 1;
        	strcpy(name,"cp");
        	switch (subOpc1) {
        		case 1:
        			switch (subOpc2) {
        				case 0:
        					dstStr = "cbd:";	srcStr = "cbd:";
        					numDp = 2;			numSp = 2;
        					checkAdd_e(typeAll,name);
        					break;
        				case 1:
        					dstStr = "cbd:";	srcStr = "cxa:";
        					numDp = 2;			numSp = 1;
        					break;
        				case 2:
        					dstStr = "cxa:";	srcStr = "cbd:";
        					numDp = 1;			numSp = 2;
        					break;
        				case 3:
        					dstStr = NULL;		srcStr = "cbd:";
        					numDp = 0;			numSp = 2;
        					break;
        				case 4:
        					dstStr = "cm:";		srcStr = "cbd:";
        					numDp = 1;			numSp = 2;
        					break;
        				case 6:
        					dstStr = "cbd:";	srcStr = "cm:";
        					numDp = 2;			numSp = 1;
        					break;
        				case 5:
        					dstStr = "cm:";		srcStr = "cjid:";
        					numDp = 1;			numSp = 1;
        					break;
        				case 7:
        					dstStr = "cjid:";	srcStr = "cm:";
        					numDp = 1;			numSp = 1;
        					break;
        			}
        			break;
        		case 2:
        			switch (subOpc2) {
        				case 0:
        					dstStr = "clbd:";	srcStr = "clbd:";
        					numDp = 2;			numSp = 2;
        					checkAdd_e(typeAll,name);
        					break;
        				case 1:
        					dstStr = "clbd:";	srcStr = "cxa:";
        					numDp = 2;			numSp = 1;
        					break;
        				case 2:
        					dstStr = "cxa:";	srcStr = "clbd:";
        					numDp = 1;			numSp = 2;
        					break;
        				case 3:
        					break;
        				case 4:
        					dstStr = "cm:";		srcStr = "clbd:";
        					numDp = 1;			numSp = 2;
        					break;
        				case 5:
        					dstStr = "cm:";		srcStr = "csd:";
        					numDp = 1;			numSp = 1;
        					break;
        				case 6:
        					dstStr = "clbd:";	srcStr = "cm:";
        					numDp = 2;			numSp = 1;
        					break;
        				case 7:
        					break;
        			}
        			break;
        		case 3:
        			if ((limm_value & 0x08000000) >> 27) strcat(name,".na");
        			if ((limm_value & 0x00800000) >> 23) strcat(name,".x");
        			switch (subOpc2) {
        				case 0:
        					break;
        				case 1:
        					dstStr = NULL;		srcStr = "cxa:";
        					numDp = 0;			numSp = 1;
        					break;
        				case 2:
        					break;
        				case 3:
        					dstStr = "cxa:";	srcStr = "cxa:";
        					numDp = 1;			numSp = 1;
        					break;
        				case 4:
        					break;
        				case 5:
        					dstStr = "cm:";		srcStr = "cxa:";
        					numDp = 1;			numSp = 1;
        					break;
        				case 6:
        					dstStr = NULL;		srcStr = "cm:";
        					numDp = 0;			numSp = 1;
        					break;
        				case 7:
        					dstStr = "cxa:";	srcStr = "cm:";
        					numDp = 1;			numSp = 1;
        					break;
        			}
        			break;
        		default:
        			strcat(name,"??");
        			break;
        	}
        	if ( m != 0 ) strcat(name,".m");
        	instrName = name;
        	write_instr_name();
    	    WRITE_FORMAT_x(A);
        	numDp = numDp * 10 + numSp;
        	switch (typeAll) {
        	    case  8:
        	    case 12:
        	    	switch(numDp) {
        	    	    case 01:
     	    	    		strcat(formatString,",[%s%r],%r");
     	    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, fieldB, fieldC);
     	    	    		break;
	    	    	    case 02:
         	    	    	strcat(formatString,",[%s%r,%r],%r");
         	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, fieldB, fieldC, fieldC);
        	    	    	break;
    	    	    	    case 11:
    	    	    		strcat(formatString,",[%s%r],[%s%r],%r");
    	    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, fieldC);
    	    	    		break;
        	    	    case 12:
         	    	    	strcat(formatString,",[%s%r],[%s%r,%r],%r");
         	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, fieldC, fieldC);
        	    	    	break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s%r,%r],[%s%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, fieldC, srcStr, fieldB, fieldC);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s%r,%r],[%s%r,%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, fieldC, srcStr, fieldB, fieldC, fieldC);
        	    	    	break;
        	    	}
        	    	break;
        	    case  0:
        	    case  3:
        	    	switch(numDp) {
			     case 01:
				strcat(formatString,",[%s0x%04x],%r");
				my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, ccm, fieldC);
				break;
			     case 02:
         	    	    	strcat(formatString,",[%s0x%04x,%r],%r");
         	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, ccm, fieldC, fieldC);
        	    	    	break;
			     case 11:
				if (c) {
					strcat(formatString,",[%s%r],[%s0x%04x],%r");
					my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, ccm, fieldC);
				}
				else {
					if ((subOpc2 == 5) || (subOpc2 == 7)){
						strcat(formatString,",[%s%r],[%s0x%04x],%r");
						my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, ccm, fieldC);
					}
					else {
						strcat(formatString,",[%s%r],[%s%r],%r");
						my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, fieldC);
					}
				}
				break;
        	    	    case 12:
         	    	    	strcat(formatString,",[%s%r],[%s0x%04x,%r],%r");
         	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, ccm, fieldC, fieldC);
        	    	    	break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s%r,%r],[%s0x%04x],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, fieldC, srcStr, ccm, fieldC);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s%r,%r],[%s0x%04x,%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, fieldC, srcStr, ccm, fieldC, fieldC);
        	    	    	break;
        	    	}
        	    	break;
            	case  4:
        	    case  7:
        	    	switch(numDp) {
        	    	    case 11:
         	    	    	strcat(formatString,",[%s0x%04x],[%s%r],%r");
				my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, srcStr, fieldB, fieldC);
				break;
        	    	    case 12:
         	    	    	strcat(formatString,",[%s0x%04x],[%s%r,%r],%r");
         	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, srcStr, fieldB, fieldC, fieldC);
        	    	    	break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s0x%04x,%r],[%s%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, fieldC, srcStr, fieldB, fieldC);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s0x%04x,%r],[%s%r,%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, fieldC, srcStr, fieldB, fieldC, fieldC);
        	    	    	break;
        	    	}
        	    	break;
        	    case  1:
        	    	switch(numDp) {
    	    	    	    case 01:
	    	    	    	if ( subOpc1 != 3) {
    		    	    		strcat(formatString,",[%s0x%04x],%d");
    	    		    		my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, ccm, imm);
    	    	    		}
    	    	    		else {
         	    	    		strcat(formatString,",[%s%r],0x%04x");
        	    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, fieldB, ccm);
    	    	    		}
    	    		    	break;
    	    	    	    case 02:
     	    	    		strcat(formatString,",[%s0x%04x,%r],%d");
     	    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, ccm, fieldC, imm);
    	    	    		break;
    	    	    	    case 11:
    	    	    		if ( subOpc1 != 3) {
    	    	    			strcat(formatString,",[%s%r],[%s0x%04x],%d");
    	    	    			my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, ccm, imm);
    	    	    		}
    	    	    		else {
         	    	    		strcat(formatString,",[%s%r],[%s%r],0x%04x");
        	    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, ccm);
    	    	    		}
    	    		    	break;
    	    	    	    case 12:
     	    	    		strcat(formatString,",[%s%r],[%s0x%04x,%r],%d");
     	    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, ccm, fieldC, imm);
    	    	    		break;
        	    	    case 21:
         	      		strcat(formatString,",[%s%r,%r],[%s0x%04x],%d");
        	       		my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, fieldC, srcStr, ccm, imm);
        	       		break;
        	    	    case 22:
         	      		strcat(formatString,",[%s%r,%r],[%s0x%04x,%r],%d");
        	        	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, fieldC, srcStr, ccm, fieldC, imm);
        	        	break;
        	    	}
        	    	break;
            	case  5:
        	    	switch(numDp) {
        	    	    case 11:
         	    	    	strcat(formatString,",[%s0x%04x],[%s%r],%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, srcStr, fieldB, imm);
        	    		    break;
        	    	    case 12:
         	    	    	strcat(formatString,",[%s0x%04x],[%s%r,%r],%d");
         	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, srcStr, fieldB, fieldC, imm);
        	    	    	break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s0x%04x,%r],[%s%r],%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, fieldC, srcStr, fieldB, imm);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s0x%04x,%r],[%s%r,%r],%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, fieldC, srcStr, fieldB, fieldC, imm);
        	    	    	break;
        	    	}
        	    	break;
        	    case  2:
        	    	switch(numDp) {
		            case 01:
				strcat(formatString,",[%s0x%04x],%r");
				my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, ccm, fieldC);
				break;
			    case 02:
				strcat(formatString,",[%s0x%04x,%d],%r");
				my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, ccm, imm, fieldC);
				break;
			    case 11:
				strcat(formatString,",[%s%r],[%s0x%04x],%r");
				my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, ccm, fieldC);
				break;
			    case 12:
				strcat(formatString,",[%s%r],[%s0x%04x,%d],%r");
				my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, ccm, imm, fieldC);
				break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s%r,%d],[%s0x%04x],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, imm, srcStr, ccm, fieldC);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s%r,%d],[%s0x%04x,%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, imm, srcStr, ccm, fieldC, fieldC);
        	    	    	break;
        	    	}
        	    	break;
        	    case  6:
        	    	switch(numDp) {
        	    	    case 11:
         	    	    	strcat(formatString,",[%s0x%04x],[%s%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, srcStr, fieldB, fieldC);
        	    	    	break;
        	    	    case 12:
         	    	    	strcat(formatString,",[%s0x%04x],[%s%r,%d],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, srcStr, fieldB, imm, fieldC);
        	    	    	break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s0x%04x,%d],[%s%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, imm, srcStr, fieldB, fieldC);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s0x%04x,%d],[%s%r,%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, imm, srcStr, fieldB, fieldC, fieldC);
        	    	    	break;
        	    	}
        	    	break;
        	    case  9:
        	    	switch(numDp) {
        	    	    case 11:
         	    	    	strcat(formatString,",[%s%r],[%s%r],0x%04x");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, ccm);
        	    	    	break;
        	    	    case 12:
         	    	    	strcat(formatString,",[%s%r],[%s%r,%d],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, bdOff, fieldC);
        	    	    	break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s%r,%d],[%s%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, bdOff, srcStr, fieldB, fieldC);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s%r,%d],[%s%r,%r],%r");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, bdOff, srcStr, fieldB, fieldC, fieldC);
        	    	    	break;
        	    	}
        	    	break;
        	    case 11:
        	    	switch(numDp) {
        	    	    case 11:
         	    	    	strcat(formatString,",[%s],[%s],??%d-%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, srcStr, typeAll, numDp);
        	    	    	break;
        	    	    case 12:
         	    	    	strcat(formatString,",[%s%r],[%s%r,%d],%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, bdOff, size10);
        	    	    	break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s%r,%d],[%s%r],%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, bdOff, srcStr, fieldB, size10);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s%r,%d],[%s%r,%r],%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, bdOff, srcStr, fieldB, fieldC, size10);
        	    	    	break;
        	    	}
        	    	break;
            	case 13:
            	    switch(numDp) {
            	    	case 11:
            	    	case 12:
            	    	case 21:
             	    	   	strcat(formatString,",[%s],[%s],??%d-%d");
            	    	   	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, srcStr, typeAll, numDp);
            	    	   	break;
            	    	case 22:
             	    	   	strcat(formatString,",[%s%r,%d],[%s%r,%d],%r");
            	    	   	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, bdOff, srcStr, fieldB, bdOff, fieldC);
            	    	   	break;
            	    }
            	    break;
            	case 15:
            	    switch(numDp) {
            	        case 11:
            	        case 12:
            	        case 21:
             	        	strcat(formatString,",[%s],[%s],??%d-%d");
            	        	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, srcStr, typeAll, numDp);
            	        	break;
           	    	    case 22:
           	    	    	strcat(formatString,",[%s%r,%d],[%s%r,%d],%d");
           	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, bdOff, srcStr, fieldB, bdOff, size10);
           	    	    	break;
           	    	}
           	    	break;
        	    case 10:
        	    case 14:
        	    	switch(numDp) {
		            case 01:
				strcat(formatString,",[%s%r],%d");
				my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, fieldB, size10);
				break;
        	    	    case 02:
         	    	    	strcat(formatString,",[%s%r,%r],%d");
         	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, fieldB, fieldC, size10);
        	    	    	break;
			    case 11:
				if (!c) {
					if (subOpc1 == 3) {
						strcat(formatString,",[%s%r],[%s0x%04x],%r");
						my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, ccm, fieldC);
					}
					else {
						strcat(formatString,",[%s%r],[%s%r],%d");
						my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, size10);
					}
				}
				else {
					strcat(formatString,",[%s0x%04x],[%s%r],%r");
					my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, ccm, srcStr, fieldB, fieldC);
				}
				break;
        	    	    case 12:
         	    	    	strcat(formatString,",[%s%r],[%s%r,%r],%d");
         	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, srcStr, fieldB, fieldC, size10);
        	    	    	break;
        	    	    case 21:
         	    	    	strcat(formatString,",[%s%r,%r],[%s%r],%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, fieldC, srcStr, fieldB, size10);
        	    	    	break;
        	    	    case 22:
         	    	    	strcat(formatString,",[%s%r,%r],[%s%r,%r],%d");
        	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, dstStr, fieldB, fieldC, srcStr, fieldB, fieldC, size10);
        	    	    	break;
        	    	}
        	    	break;
        	}
       }
       break;
    case 67:
        {
         	int size1_64;
        	int ccm;
        	int i2;
        	int subOpc2;
//        	int s1EQs2 = 0;
        	int sizeAndC = 0;
        	fieldA = 62; // dummy value only to update limm_value
            CHECK_FIELD(fieldA);
        	fieldA = BITS(state->words[0],21,26);
        	fieldB = BITS(state->words[0],16,20);
        	fieldC = BITS(state->words[0],11,15);
           	fieldAisReg = 1;
           	fieldBisReg = 1;
           	fieldCisReg = 1;
           	if ( fieldA == 62 ) {
           		fieldA = 0;
           		fieldAisReg = 0;
           	}
        	subOpc2 = ((limm_value & 0xf8000000) >> 27) & 0x1F;
        	i2 = (limm_value & 0x07000000) >> 24;
        	ccm = limm_value & 0x0000ffff;
        	size1_64 = (limm_value & 0x003f0000) >> 16;
        	if (size1_64 == 0) size1_64 = 64;
        	switch (subOpc2) {
    	    case 0x00:
    	    	instrName = "dcipv4";
    	    	sizeAndC = 1;
    	    	break;
    	    case 0x10:
    	    	instrName = "dcipv4l";
    	    	sizeAndC = 1;
    	    	break;
    	    case 0x01:
    	    	instrName = "dcipv6";
    	    	sizeAndC = 1;
    	    	break;
    	    case 0x11:
    	    	instrName = "dcipv6l";
    	    	sizeAndC = 1;
    	    	break;
    	    case 0x04:
    	    	instrName = "dcmac";
    	    	break;
    	    case 0x14:
    	    	instrName = "dcmacl";
    	    	break;
    	    case 0x05:
    	    	instrName = "dcmpls";
    	    	break;
    	    case 0x08:
    	    	instrName = "dcsmpls";
    	    	break;
    	    case 0x15:
    	    	instrName = "dcmplsl";
    	    	break;
    	    case 0x18:
    	    	instrName = "dcsmplsl";
    	    	break;
    	    case 0x13:
    	    	instrName = "dcudpl";
    	    	break;
    	    case 0x12:
    	    	instrName = "dctcpl";
    	    	break;
    	    case 0x17:
    	    	instrName = "dcgrel";
    	    	break;
    	    case 0x16:
    	    	instrName = "dcmiml";
    	    	break;
    	    default:
    	    	instrName = "dc????";
    	    	break;
        	}
    	    write_instr_name();
    	    WRITE_FORMAT_x(A);
    	    switch ( i2 ) {
    	    	case 0:
    	    		if ( sizeAndC == 0 ) {
    	    			strcat(formatString,",[cm:%r],[cm:%r],%d");
    	    			my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, size1_64);
    	    		}
    	    		else {
    	    			strcat(formatString,",[cm:%r],[cm:%r],%r,%d");
    	    			my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, fieldC, size1_64);
    	    		}
    	    		break;
    	    	case 2:
    	    		if ( sizeAndC == 0 ) {
    	    			strcat(formatString,",[cm:%r],[cm:0x%04x],%d");
    	    			my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, ccm, size1_64);
    	    		}
    	    		else {
    	    			strcat(formatString,",[cm:%r],[cm:0x%04x],%r,%d");
    	    			my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, ccm, fieldC, size1_64);
    	    		}
    	    		break;
    	    	case 3:
    	    		if ( sizeAndC == 0 ) {
    	    			strcat(formatString,",[cm:0x%04x],[cm:%r],%d");
    	    			my_sprintf(state, state->operandBuffer, formatString, fieldA, ccm, fieldB, size1_64);
    	    		}
    	    		else {
    	    			strcat(formatString,",[cm:0x%04x],[cm:%r],%r,%d");
    	    			my_sprintf(state, state->operandBuffer, formatString, fieldA, ccm, fieldB, fieldC, size1_64);
    	    		}
    	    		break;
    	    	case 4:
    	    		strcat(formatString,",[cm:%r],[cm:%r],%r");
    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, fieldC);
    	    		break;
    	    	case 6:
    	    		strcat(formatString,",[cm:%r],[cm:0x%04x],%r");
    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, ccm, fieldC);
    	    		break;
    	    	case 7:
    	    		strcat(formatString,",[cm:0x%04x],[cm:%r],%r");
    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA, ccm, fieldB, fieldC);
    	    		break;
    	    	default:
    	    		strcat(formatString,",????");
    	    		my_sprintf(state, state->operandBuffer, formatString, fieldA);
    	    		break;
    	    }
    	    break;
        }
    case 68:
      {
          int s;
          int size;
          int offset;
          int skip;
          char name[20];
          FIELD_C_AC16();
          FIELD_B_AC16();
          s = (state->words[0] & 0x0000C000) >> 14;
          offset = (state->words[0] & 0x00001F00) >> (8-2);
          size = (state->words[0] & 0x0000007F);
          if ( size == 0 ) size = 128;
          strcpy(name,"mcmp");
          if ( (state->words[0] & 0x00002000) != 0 ) strcat(name,".s");
          if ( (state->words[0] & 0x00000080) != 0 ) strcat(name,".m");
          instrName = name;
          write_instr_name();
          switch(s) {
          case 0:
   			  strcat(formatString,"%r,[cm:%r,%r],[cm:%r],%r");
              my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldB,fieldC,fieldC);
        	  break;
          case 1:
        	  if ( offset == 0 ) {
      			  strcat(formatString,"%r,[cm:%r],[cm:%r],%r");
                  my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldC,fieldC);
        	  }
        	  else {
      			  strcat(formatString,"%r,[cm:%r,%d],[cm:%r],%r");
                  my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,offset,fieldC,fieldC);
        	  }
        	  break;
          case 2:
   			  strcat(formatString,"%r,[cm:%r,%r],[cm:%r],%d");
              my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldB,fieldC,size);
        	  break;
          case 3:
        	  if ( offset == 0 ) {
      			  strcat(formatString,"%r,[cm:%r],[cm:%r],%d");
                  my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldC,size);
        	  }
        	  else {
      			  strcat(formatString,"%r,[cm:%r,%d],[cm:%r],%d");
                  my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,offset,fieldC,size);
        	  }
        	  break;
          default:
  			  strcat(formatString,"%r,%r,????");
              my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldC);
              break;
          }
          break;
      }
    case 69:
      {
    	  int subCmnd;
	      CHECK_FIELD_B();
	      CHECK_FIELD_A();
	      subopcode = BITS(state->words[0],16,21);
	      if (subopcode == 0x30)
	    	  instrName = "wkup";
	      else
	      {
	    	  subCmnd = BITS(state->words[0],6,11);
	    	  switch (subCmnd) {
	    	  	  case 0:
	    	  		  break;
   	  	          case 1:
   	  	        	  instrName = "asri.core";
   	  	        	  break;
   	  	          case 2:
   	  	        	  instrName = "asri.clsr";
   	  	        	  break;
   	  	          case 3:
   	  	        	  instrName = "asri.all";
   	  	        	  break;
   	  	          case 4:
   	  	        	  instrName = "asri.gic";
   	  	        	  break;
   	  	          case 5:
   	  	        	  instrName = "rspi.gic";
   	  	        	  break;
   	  	          default:
   	  	        	  instrName = "asri.????";
   	  	      }
	      }
  	      write_instr_name();
  	      WRITE_FORMAT_x(A);
  	      WRITE_FORMAT_COMMA_x(B);
          my_sprintf(state, state->operandBuffer, formatString,fieldA,fieldB);
	      break;
      }
    case 70:
    {
    	int FieldStartPos, FieldSz, ShiftFactor, BitsToScramble, Scramble_en, v;
    	v = state->words[0] & 0x0000FFFF;
    	FieldStartPos = (v & 0x00000038) >> 3;
    	FieldSz = (v & 0x000001C0) >> 6;
    	ShiftFactor = (v & 0x00000E00) >> 9;
    	BitsToScramble = (v & 0x00007000) >> 12;
    	Scramble_en = (v & 0x00008000) >> 15;
    	if (FieldSz == 0) FieldSz = 8;
    	if (ShiftFactor == 0) ShiftFactor = 8;
    	if (BitsToScramble == 0) BitsToScramble = 8;
        FIELD_C_AC16();
        FIELD_B_AC16();
        if (v & 0x00000001)
        	instrName = "imxb";
    	write_instr_name();
    	if ( Scramble_en ) {
    		strcat(state->instrBuffer,".s");
    		strcat(formatString,"%r,%r,%d,%d,%d,%d");
    		my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC, FieldStartPos, FieldSz, ShiftFactor, BitsToScramble);
    	}
    	else {
    		strcat(formatString,"%r,%r,%d,%d,%d");
    		my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC, FieldStartPos, FieldSz, ShiftFactor);
    	}
    	break;
    }
    case 71:
      {
    	  int p = BITS(state->words[0],22,23);
    	  directMem = BIT(state->words[0],15);
    	  fieldC = FIELDC(state->words[0]);
    	  if ( ( p != 1 ) && ( fieldC == 62 ) ) {
             CHECK_FIELD(fieldC);
             fieldC = 62;
    	  }
          CHECK_FIELD_B();
          CHECK_FIELD_A();
          write_instr_name();
          switch(p) {
              case 0:
            	  if ( fieldC != 62 ) {
            	      strcat(formatString,"%r,[%r]");
            	      my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC);
            	  }
            	  else {
            	      strcat(formatString,"%r,[0x%08x]");
            	      my_sprintf(state, state->operandBuffer, formatString, fieldB,limm_value);
     		      }
            	  break;
              case 1:
        	      strcat(formatString,"%r,[%d]");
        	      my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC);
        	      break;
          }
          break;
      }
    case 72:
      {
    	  int size;
      	  subopcode = BITS(state->words[0],0,2);
      	  switch ( subopcode ) {
     	      case 0:  instrName = "atst"; break;
 	          case 1:  instrName = "ari"; break;
 	          case 2:  instrName = "ardc"; break;
 	          case 4:  instrName = "aric"; break;
 	          case 5:  instrName = "aric.rvd"; break;
 	          case 6:  instrName = "arcl"; break;
 	      }
          size = (state->words[0] & 0x00000018) >> 3;
          size = 1 << size;
          CHECK_FIELD_B();
          CHECK_FIELD_C();
          if ( BIT(state->words[0],15) != 0 ) flag = 1;
          write_instr_name();
          if ( BIT(state->words[0],5) != 0 ) {
        	  strcat(state->instrBuffer,".di");
          }
	      strcat(formatString,"%r,[%r],%d");
	      my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC,size);
          break;
      }
    case 73:
      {
    	  int reg = 0;
    	  if ( BITS(state->words[0],0,5) == 0x3f ) {
    		  FIELD_B();
    		  if (fieldBisReg) {
    			  CHECK_FIELD_B();
    			  reg = fieldB;
    		  }
    	  }
    	  else {
    		  CHECK_FIELD_C();
    		  reg = fieldC;
    	  }
          write_instr_name();
	      WRITE_FORMAT_x(B);
	      my_sprintf(state, state->operandBuffer, formatString, reg);
	      break;
      }
    case 74:
      {
          int m3;
          int m1;
          int s2;
          int m2;
          int size;
          FIELD_C_AC16();
          FIELD_B_AC16();
          if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
    	  m3 = BITS(state->words[0],5,8);
          s2 = BITS(state->words[0],12,13);
      	  m1 = BITS(state->words[0],14,14);
          m2 = BITS(state->words[0],15,15);
          size  =  BITS(state->words[0],9,11);
          if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
          write_instr_name();
          s2 <<= 3;
          size++;
          if ( m3 != 15) {
        	  strcat(formatString,"%r,%r,%r,%d,%d,%d,%d,%d");
  	          my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldC,s2,size,m1,m2,m3);
          }
          else {
        	  if ( m2 != 0 ) {
            	  strcat(formatString,"%r,%r,%r,%d,%d,%d,%d");
      	          my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldC,s2,size,m1,m2);
        	  }
        	  else {
        		  if ( m1 != 0 ) {
            	      strcat(formatString,"%r,%r,%r,%d,%d,%d");
      	              my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldC,s2,size,m1);
        		  }
        		  else {
            	      strcat(formatString,"%r,%r,%r,%d,%d");
      	              my_sprintf(state, state->operandBuffer, formatString,fieldB,fieldB,fieldC,s2,size);
        		  }
        	  }
          }
          break;
      }
    case 75:
      {
    	  int type;
    	  int size;
    	  int target_size;
    	  int subOpcode;
    	  char lCommand[30];
    	  int di;
    	  int ps;
    	  int c;
    	  char strType[10];
    	  if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
    	  target_size = (state->words[0] & 0x00001c00) >> 10;
    	  size = (state->words[0] & 0x00000300) >> 8;
    	  type = (state->words[0] & 0x000000E0) >> 5;
    	  subOpcode = (state->words[0] & 0x0000001F);
    	  c = (state->words[0] & 0x00002000) >> 13;
    	  di = (state->words[0] & 0x00004000) >> 14;
    	  ps = (state->words[0] & 0x00008000) >> 15;
    	  strcpy(lCommand,instrName);
    	  switch (target_size) {
    	      case 1:
    	    	  if ( subOpcode != 20 ) {
    	    	      strcat(lCommand,"b"); break;
    	    	  }
    	    	  strcpy(lCommand,"aricb.r");
    	    	  break;
    	      case 2:
    	    	  if ( subOpcode != 20 ) {
    	    	      strcat(lCommand,"w"); break;
    	    	  }
    	    	  strcpy(lCommand,"aricw.r");
    	    	  break;
    	      case 3:
    	    	  break;
    	      case 4:
    	    	  if ((subOpcode != 3) && (subOpcode != 20)) {
    	    		  strcat(lCommand,"l"); break;
    	    	  }
    	    	  break;
    	      case 5:
    	    	  if (subOpcode == 3) {
      	    		  strcat(lCommand,"l"); break;
      	    	  }
    	    	  break;
    	      default:
    	      {
    	    	  char tmp[10];
    	    	  sprintf(tmp,"%d?",size);
    	    	  strcat(lCommand,tmp);
    	    	  break;
    	      }
    	  }
    	  switch (type) {
    	      case 0: strType[0] = 0;
    	      	  switch (subOpcode) {
    	      	  case 19: strcpy(lCommand,"aricl"); break;
    	      	  default: break;
    	      	  }
    	      break;			// NULL string
	          case 1: strcpy(strType,"xa:"); break;
	          case 2: strcpy(strType,"scd:");
	          	  switch(subOpcode) {
	          	  case 1: strcpy(lCommand,"crst"); break;
	          	  case 16: strcpy(lCommand,"cst"); break;
	          	  default: break;
	          	  }
                          break;
	          case 5:
	        	  strcpy(strType,"cd:");
	        	  switch (subOpcode) {
    	  	  	  case 0: strcpy(lCommand,"pcincr"); break;
    	  	  	  case 1: strcpy(lCommand,"pcdecr"); break;
    	  	  	  case 2: strcpy(lCommand,"pcdincr"); break;
    	  	  	  case 3: strcpy(lCommand,"pcinit"); break;
    	  	  	  case 5: strcpy(lCommand,"pcdrcycl.cl"); break;
    	  	  	  case 4: strcpy(lCommand,"pcdcl"); break;
    	  	  	  case 6: strcpy(lCommand,"pcdrprt.cl"); break;
    	  	  	  case 7: strcpy(lCommand,"pccl"); break;
    	  	  	  case 8: strcpy(lCommand,"pcrcycl.cl"); break;
    	  	  	  case 9: strcpy(lCommand,"pcrprt.cl"); break;
    	  	  	  case 10: strcpy(lCommand,"pcrprt"); break;
    	  	  	  case 11: strcpy(lCommand,"pcdrprt"); break;
    	  	  	  case 12: strcpy(lCommand,"pcrcycl"); break;
    	  	  	  case 13: strcpy(lCommand,"pcdrcycl"); break;
    	  	  	  case 14: strcpy(lCommand,"pcincr.of"); break;
    	  	  	  default: break;
	        	  }
	        	  break;
	          case 6:
	        	  strcpy(strType,"cd:");
	        	  switch (subOpcode) {
	        	  	  case 0: strcpy(lCommand,"cinit"); break;
        	  	  	  case 1: strcpy(lCommand,"crst"); break;
        	  	  	  case 2: strcpy(lCommand,"cincr"); break;
        	  	  	  case 3: strcpy(lCommand,"cdecr"); break;
        	  	  	  case 4: strcpy(lCommand,"cgetc"); break;
        	  	  	  case 5: strcpy(lCommand,"cchkc"); break;
        	  	  	  case 6: strcpy(lCommand,"cincr1"); break;
        	  	  	  case 7: strcpy(lCommand,"cdecr1"); break;
        	  	  	  case 8: strcpy(lCommand,"cdincr"); break;
        	  	  	  case 9: strcpy(lCommand,"cbclr"); break;
        	  	  	  case 10: strcpy(lCommand,"cbset"); break;
        	  	  	  case 11: strcpy(lCommand,"cbcswp"); break;
        	  	  	  case 12: strcpy(lCommand,"cbwr"); break;
        	  	  	  case 13: strcpy(lCommand,"cbrd"); break;
        	  	  	  case 14: strcpy(lCommand,"crd"); break;
        	  	  	  case 15: strcpy(lCommand,"cld"); break;
        	  	  	  case 16: strcpy(lCommand,"cst"); break;
        	  	  	  case 17: strcpy(lCommand,"cdecrc"); break;
        	  	  	  case 22: strcpy(lCommand,"cftch"); break;
        	  	  	  case 23: strcpy(lCommand,"cmld"); break;
        	  	  	  case 24: strcpy(lCommand,"cmst"); break;
        	  	  	  case 25: strcpy(lCommand,"cminit"); break;
        	  	  	  case 26: strcpy(lCommand,"cminit.rst"); break;
        	  	  	  case 27: strcpy(lCommand,"cwrdb"); break;
        	  	  	  case 28: strcpy(lCommand,"cwrde"); break;
        	  	  	  default: break;
	        	  }
	        	  break;
	          case 7: strcpy(strType,"mbd:"); break;
    	      default: sprintf(strType,"?%d?:",type);  break;
    	  }
    	  if (di != 0) strcat(lCommand,".di");
    	  instrName = lCommand;
          write_instr_name();
          FIELD_C_AC16();
          FIELD_B_AC16();
          switch ( subOpcode ) {
          	  case 0:
          	  case 2:
          	  case 3:
          	  case 6:
          	  case 7:
          	  case 16:
          	  case 24:
          	  case 25:
          	  case 26:
                  if ( ps == 0 ) {
                	  if (c == 0) {
                		  switch ( subOpcode ) {
                		  case 3:
                    		  strcat(formatString,"%r,%r,[%s%r],%r");
                    		  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC, fieldB);
                			  break;
                		  default:
                			  if ((subOpcode == 2) && (c == 0) && (type == 6)) {
                				  strcat(formatString,"%r,%r,[%s%r],%r");
                				  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC, fieldB);
                			  }
                			  else {
                				  strcat(formatString,"%r,%r,[%s%r]");
                				  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
                			  }
                			  break;
                		  }
                	  }
                	  else {
                		  if ((type == 0) || (type == 7) || (type == 2) || (type == 6))
                    			  	  strcat(formatString,"%r,[cm:GPA2],%r,[%s%r]");
                		  else
                    			  	  strcat(formatString,"%r,[cm:GPA1],%r,[%s%r]");
                		  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
                	  }
                  }
                  else {
                	  if (c == 0) {
                    	  switch (type) {
                    	  case 5:
                    		  if ( subOpcode == 0 || subOpcode == 2 || subOpcode == 3 ) {
                        		  strcat(formatString,"%r,[%s%r]");
                        		  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                    		  }
                    		  else {
                          		  strcat(formatString,"[%s%r]");
                              	  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                    		  }
                    		  break;
                    	  default:
                    		  strcat(formatString,"%r,[%s%r]");
                    		  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                    		  break;
                    	  }
                	  }
                	  else {
                		  if ((subOpcode == 16) && ((type == 0) || (type == 1))) {
                			  if (type == 0)
                				  strcat(formatString,"[cm:GPA2],[%s%r]");
                			  else
                				  strcat(formatString,"[cm:GPA1],[%s%r]");
                			  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                		  }
                		  else {
                			  if ((type == 0) || (type == 7) || (type == 2) || (type == 6))
                				  strcat(formatString,"[cm:GPA2],%r,[%s%r]");
                			  else
                				  strcat(formatString,"[cm:GPA1],%r,[%s%r]");
                			  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                		  }
                	  }
                  }
          		  break;
          	  case 1:
                  if ( ps == 0 ) {
                	  if (c == 0) {
                		  strcat(formatString,"%r,%r,[%s%r]");
                		  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
                	  }
                	  else {
                	  strcat(formatString,"%r,[cm:GPA2],[%s%r]");
                	  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                	  }
            	  }
            	  else {
                	  if (c == 0) {
                    	  switch (type) {
                    	  case 5:
                      		  strcat(formatString,"%r,[%s%r]");
                          	  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                    		  break;
                    	  default:
                        	  strcat(formatString,"[%s%r]");
                        	  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                    		  break;
                    	  }
                	  }
                	  else {
                    	  strcat(formatString,"[cm:GPA2],[%s%r]");
                    	  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                	  }
                  }
          		  break;
          	  case 11:
          	  case 13:
                  if ( ps == 0 ) {
                	  if (c == 0) {
                    	  strcat(formatString,"%r,%r,[%s%r]");
                    	  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
                	  }
                	  else {
                		  strcat(formatString,"%r,[cm:GPA2],%r,[%s%r]");
                		  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
                	  }
            	  }
            	  else {
                	  switch (type) {
                	  case 5:
                  		  strcat(formatString,"[%s%r]");
                      	  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                		  break;
                	  default:
                		  strcat(formatString,"[cm:GPA2],%r,[%s%r]");
                		  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                		  break;
                	  }
                  }
          		  break;
          	  case 4:
          	  case 5:
          	  case 9:
          	  case 10:
          	  case 12:
                  if ( ps == 0 ) {
                	  if ((c == 1) && (type == 6)) {
                    	  strcat(formatString,"%r,[cm:GPA2],%r,[%s%r]");
                		  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
                	  }
                	  else {
                		  strcat(formatString,"%r,%r,[%s%r]");
                		  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
                	  }
            	  }
            	  else {
                	  if (c == 0) {
                    	  switch (type) {
                    	  case 5:
                      		  strcat(formatString,"[%s%r]");
                          	  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                    		  break;
                    	  default:
                    		  strcat(formatString,"%r,[%s%r]");
                    		  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                    		  break;
                    	  }
                	  }
                	  else {
                		  if ((type == 0) || (type == 6) || (type == 7))
                			  strcat(formatString,"[cm:GPA2],%r,[%s%r]");
                		  else
                			  strcat(formatString,"[cm:GPA1],%r,[%s%r]");
                    	  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                	  }
                  }
          		  break;
          	  case 14:
          	  case 15:
          	  case 23:
          	  case 27:
          	  case 28:
          	  case 30:
          	  case 31:
                  if ( ps == 0 ) {
            		  if (subOpcode == 31) {
                		  strcat(formatString,"%r,%r,[cm:GPA2],[%s%r]");
                		  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
            		  }
            		  else if (subOpcode == 30) {
                		  strcat(formatString,"%r,[cm:GPA2],%r,[%s%r]");
                		  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
            		  }
            		  else {
            			  strcat(formatString,"%r,[cm:GPA2],[%s%r]");
            			  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
            		  }
                  }
                  else {
                	  switch (type) {
                	  case 5:
                  		  strcat(formatString,"%r,[%s%r]");
                      	  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                		  break;
                	  default:
                		  if (((subOpcode == 30) || (subOpcode == 31)) && (c == 0)) {
                			  strcat(formatString,"%r,[%s%r]");
                			  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                		  }
                		  else {
                			  if (subOpcode == 31) {
                    			  strcat(formatString,"%r,[cm:GPA2],[%s%r]");
                    			  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                			  }
                			  else if (subOpcode == 30) {
                    			  strcat(formatString,"[cm:GPA2],%r,[%s%r]");
                    			  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                			  }
                			  else {
                				  strcat(formatString,"[cm:GPA2],[%s%r]");
                				  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                			  }
                		  }
            		  break;
                	  }
                  }
           		  break;
          	  case 8:
      	  	  case 17:
      	  		  if (type == 6) {
      	  			  if (ps == 0) {
  				  		strcat(formatString,"%r,[cm:GPA2],%r,[%s%r]");
              	 	  		my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
      	  			  }
      	  			  else {
      	  				  if (c == 0) {
      	  					  switch (type) {
      	  					  case 5:
      	  						  strcat(formatString,"%r,[%s%r]");
      	  						  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
      	  						  break;
      	  					  default:
      	  						  strcat(formatString,"%r,[%s%r]");
      	  						  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
      	  						  break;
      	  					  }
      	  				  }
      	  				  else {
      	  					  if ((type == 0) || (type == 7) || (type == 2) || (type == 6))
      	  						  strcat(formatString,"[cm:GPA2],%r,[%s%r]");
      	  					  else
      	  						  strcat(formatString,"[cm:GPA1],%r,[%s%r]");
      	  					  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
      	  				  }
      	  			  }
      	  		  }
      	  		  else {
	                     if ( ps == 0 ) {
	                    	 if (subOpcode == 17){
	                    		 if ((type == 0) || (type == 7))
	                    			 strcat(formatString,"%r,[cm:GPA2],[%s%r]");
	                    		 else
	                    			 strcat(formatString,"%r,[cm:GPA1],[%s%r]");
	                          	  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
        	            	 }
                	    	 else {
                      			  strcat(formatString,"%r,[%s%r]");
                          		  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
	                    	 }
                      }
                      else {
                    	  if (((subOpcode == 17) && (c == 0)) || ((subOpcode == 8) && (type == 5))){
                      		  strcat(formatString,"[%s%r]");
                          	  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                    	  }
                    	  else {
                    		  if ((type == 0) || (type == 7))
                    			  strcat(formatString,"[cm:GPA2],[%s%r]");
                    		  else
                    			  strcat(formatString,"[cm:GPA1],[%s%r]");
                          	  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                    	  }
                      }
      	  		  }
      	  		  break;
      	  	  case 18:
      	  	  case 19:
      	  	  case 20:
          	  case 21:
          	  case 22:
                  if ( ps == 0 ) {
                	  if (((type == 0) || (type == 7)) && (c == 1))
                			  strcat(formatString,"%r,[cm:GPA2],[%s%r]");
                	  else {
                		  if (((subOpcode == 22) || (subOpcode == 21)) && (c == 0))
                			  strcat(formatString,"%r,[%s%r]");
                		  else
                			  strcat(formatString,"%r,[cm:GPA1],[%s%r]");
                	  	  }
                	  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                  }
                  else {
                	  if ((subOpcode == 18) && (c == 0)) {
                  		  strcat(formatString,"[%s%r]");
                      	  	  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                	  }
                	  else {
                		  if ((subOpcode == 19) || (subOpcode == 20) || (subOpcode == 21) || ((subOpcode == 22) && ((type == 6) || (type == 7)))) {
                			  if (c == 0)
                				  strcat(formatString,"[%s%r]");
                			  else {
                				  if ((type == 0) || (type == 7))
                					  strcat(formatString,"[cm:GPA2],[%s%r]");
                				  else
                					  strcat(formatString,"[cm:GPA1],[%s%r]");
                			  }
            				  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                		  }
                		  else
                			  if (subOpcode == 22) {
                				  if ((c == 1) && (di == 1))
                						  strcat(formatString,"[cm:GPA1],[%s%r]");
                				  else
                					  strcat(formatString,"[%s%r]");
                				  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                			  }
                			  else {
                				  if ((type == 0) || (type == 7))
                					  strcat(formatString,"[cm:GPA2],[%s%r]");
                				  else
                					  strcat(formatString,"[cm:GPA1],[%s%r]");
                				  my_sprintf(state, state->operandBuffer, formatString, strType, fieldC);
                			  }
                	  }
                  }
                  break;
          	  default:
                  if ( ps == 0 ) {
               		  strcat(formatString,"%r,%r,[%s%r]");
                   	  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, strType, fieldC);
                  }
                  else {
              		  strcat(formatString,"%r,[%s%r]");
                  	  my_sprintf(state, state->operandBuffer, formatString, fieldB, strType, fieldC);
                  }
                  break;
          }
    	  break;
      }
    case 76:
        {
        	int subOpc1;
        	int subOpc2;
        	int type;
        	int src2 = 0;
        	int src1 = 1;
        	fieldA = 62; // dummy value only to update limm_value
            CHECK_FIELD(fieldA);
        	fieldA = BITS(state->words[0],21,26);
        	fieldB = BITS(state->words[0],16,20);
        	fieldC = BITS(state->words[0],11,15);
           	fieldAisReg = 1;
           	fieldBisReg = 1;
           	fieldCisReg = 1;
           	if ( fieldA == 62 ) {
           		fieldA = 0;
           		fieldAisReg = 0;
           	}
            subOpc1 = BITS(state->words[0],0,4);
        	type = ((limm_value & 0xe0000000) >> 29) & 0x07;
        	subOpc2 = limm_value & 0x0000000f;
			switch (subOpc1) {
			    case 30:
				case 29:
				case 28:
				case 27:
					src2 = 1;
					break;
				case 23:
					src1 = 0;
					src2 = 1;
					break;
				case 25:
					switch (subOpc2) {
	        	      case 0:
	        	    	switch (type) {
	        	    		case 0: instrName = "scrst_0"; break;
	        	    		case 1: instrName = "dcrst_0"; break;
	        	    		case 2: instrName = "tbcrst_0"; break;
	        	    		default: instrName = "?crst_0"; break;
	        	    	}
	        	    	break;
	        	      case 1:
	        	    	src2 = 1;
	           	    	switch (type) {
	            	    	case 0: instrName = "scadd_0"; break;
	            	    	case 1: instrName = "dcadd_0"; break;
	            	    	case 2: instrName = "tbcadd_0"; break;
	            	    	default: instrName = "?cadd_0"; break;
	            	    }
	        	    	break;
	        	      case 2:
	        	    	switch (type) {
	        	    		case 0: instrName = "scinc_0"; break;
	        	    		case 1: instrName = "dcinc_0"; break;
	        	    		case 2: instrName = "tbcinc_0"; break;
	        	    		default: instrName = "?cinc_0"; break;
	        	    	}
	        	    	break;
	        	      case 3:
	        	    	src2 = 1;
	           	    	switch (type) {
	            	    	case 0: instrName = "scsub_0"; break;
	            	    	case 1: instrName = "dcsub_0"; break;
	            	    	case 2: instrName = "tbcsub_0"; break;
	            	    	default: instrName = "?csub_0"; break;
	            	    }
	        	    	break;
	        	      case 4:
	        	    	switch (type) {
	        	    		case 0: instrName = "scdec_0"; break;
	        	    		case 1: instrName = "dcdec_0"; break;
	        	    		case 2: instrName = "tbcdec_0"; break;
	        	    		default: instrName = "?cdec_0"; break;
	        	    	}
	        	    	break;
	        	      case 5:
	        	    	src2 = 1;
	           	    	switch (type) {
	            	    	case 0: instrName = "scld_0"; break;
	            	    	case 1: instrName = "dcld_0"; break;
	            	    	default: instrName = "?cld_0"; break;
	            	    }
	        	    	break;
	        	      case 6:
	        	    	src2 = 1;
	           	    	switch (type) {
	            	    	case 0: instrName = "scst_0"; break;
	            	    	case 1: instrName = "dcst_0"; break;
	            	    	default: instrName = "?cst_0"; break;
	            	    }
	        	    	break;
	        	      case 7:
	        	    	switch (type) {
	        	    		case 2: instrName = "tbcolor_0"; break;
	        	    		default: instrName = "?color_0"; break;
	        	    	}
	        	    	break;
					}
					break;
			}
    	    write_instr_name();
    	    WRITE_FORMAT_x(A);
    	    WRITE_FORMAT_COMMA_x(B);
    	    if ( src2 == 0 ) {
        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB);
    	    }
    	    else if (src1 == 0)
    	    	my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldC);
    	    else {
        	    WRITE_FORMAT_COMMA_x(C);
        	    my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
            }
    	}
    	break;
    case 77:
      {
          int type;
          int size;
          int off_sel;
          int entry_sel;
          int imm_entry;
          int imm_off;
          int ps;
          char name[20];
          char *dstStr;
          char *srcStr;
          int   numSp;
          int   numDp;
          int   mode;
          int   swap_bc;
          int   na;
          type = (state->words[0] & 0x000000e0) >> (5-1); // include ld/st bit
          type |= (state->words[0] & 0x00000001);
          size = (state->words[0] & 0x00000100) >> 8;
          na = (state->words[0] & 0x00000200) >> 9;
          off_sel = (state->words[0] & 0x00000002) >> 1;
          entry_sel = (state->words[0] & 0x00004000) >> 14;
          imm_entry = (state->words[0] & 0x0000001C) >> 2;
          imm_off = (state->words[0] & 0x00003C00) >> 10;
          ps = (state->words[0] & 0x00008000) >> 15;
          imm_off *= 16;
          imm_entry = 16 << imm_entry;
          FIELD_C_AC16();
          FIELD_B_AC16();
          strcpy(name,"cp");
          switch (size) {
          	  case 0: strcat(name,"16"); break;
          	  case 1: strcat(name,"32"); break;
          }
          if ( na != 0 ) strcat(name,".na");
          dstStr = srcStr = "??:";
          numDp = numSp = 1;
          mode = 0;
          swap_bc = 0;
          switch (type ) {
          	  case 2: // phy + load
          		  dstStr = "cm:";
          		  srcStr = "xa:";
          		  off_sel = 0;
          		  break;
          	  case 3: // phy + store
          		  dstStr = "xa:";
          		  srcStr = "cm:";
          		  off_sel = 0;
          		  swap_bc = 1;
          		  break;
          	  case 4: // jd
          		  dstStr = "cm:";
          		  srcStr = "jid:";
          		  off_sel = 0;
          		  break;
          	  case 5: // jd + store
          		  dstStr = "jid:";
          		  srcStr = "cm:";
          		  off_sel = 0;
          		  swap_bc = 1;
          		  break;
          	  case 6:
          		  mode = 1;
          		  numSp  = 2;
          		  dstStr = "cm:";
          		  srcStr = "sd:";
          		  break;
          	  case 7:
          		  mode = 1;
          		  numDp  = 2;
          		  dstStr = "sd:";
          		  srcStr = "cm:";
          		  swap_bc = 1;
          		  break;
          	  case 8:
          		  mode = 1;
          		  numSp  = 2;
          		  dstStr = "cm:";
          		  srcStr = "xd:";
          		  break;
          	  case 9:
          		  mode = 1;
          		  numDp  = 2;
          		  dstStr = "xd:";
          		  srcStr = "cm:";
          		  swap_bc = 1;
          		  break;
          	  case 10: // uip
          		  strcpy(name,"uip");
          		  if (na) strcat(name,".na");
          		  dstStr = "cm:";
          		  srcStr = "cm:";
          		  off_sel = 0;
          		  entry_sel = 0;
          		  break;

          }
          instrName = name;
 	      write_instr_name();
 	      if ( mode == 0 ) {  // mode = 0
 	          if ( numSp == 2 ) {  // mode = 0 && numSp = 2
 	        	  if ( ps == 0 ) { // mode = 0 && numSp = 2 && ps = 0
 	        		  if ( off_sel == 0 ) {  // mode = 0 && numSp == 2 && ps = 0 && off_sel = 0
 	        			  strcat(formatString,"%r,[%s%r],[%s%r,%r]");
 	        			  my_sprintf(state, state->operandBuffer, formatString, fieldB, dstStr, fieldC, srcStr, fieldB, fieldC);
 	        		  }
 	        		  else { // mode = 0 && numSp = 2 && ps = 0 && off_sel != 0
 	        			  strcat(formatString,"%r,[%s%r],[%s%r,%d]");
 	        			  my_sprintf(state, state->operandBuffer, formatString, fieldB, dstStr, fieldC, srcStr, fieldB, imm_off);
 	        		  }
 	        	  }
 	        	  else {  // mode = 0 && numSp = 2 && ps != 0
 	        		  if ( off_sel == 0 ) { // mode = 0 && numSp == 2 && ps != 0 && off_sel = 0
 	        			  strcat(formatString,"[%s%r],[%s%r,%r]");
 	        			  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldC, srcStr, fieldB, fieldC);
 	        		  }
 	        		  else { // mode = 0 && numSp = 2 && ps != 0 && off_sel != 0
 	        			  strcat(formatString,"[%s%r],[%s%r,%d]");
 	        			  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldC, srcStr, fieldB, imm_off);
 	        		  }
 	        	  }
 	          }
 	          else { // mode = 0 && numSp = 1
 	        	  if ( ps == 0 ) { // mode = 0 && numSp = 1 && ps = 0
 	        		  if ( off_sel == 0 ) { // mode = 0 && numSp = 1 && ps = 0 && off_sel = 0
 	        			  strcat(formatString,"%r,[%s%r],[%s%r]");
 	        			  my_sprintf(state, state->operandBuffer, formatString, fieldB, dstStr, fieldC, srcStr, fieldB);
 	        		  }
 	        		  else { // mode = 0 && numSp = 1 && ps = 0 && off_sel != 0
 	        			  strcat(formatString,"%r,[%s%r],[%s%d]");
 	        			  my_sprintf(state, state->operandBuffer, formatString, fieldB, dstStr, fieldC, srcStr, imm_off);
 	        		  }
 	        	  }
 	        	  else { // mode = 0 && numSp = 1 && ps != 0
 	        		  if ( off_sel == 0 ) { // mode = 0 && numSp = 1 && ps != 0 && off_sel = 0
 	        			  if ( numDp == 1 ) { // mode = 0 && numSp = 1 && ps != 0 && off_sel = 0 && numDp = 1
 	        				  strcat(formatString,"[%s%r],[%s%r]");
 	        				  if ( swap_bc == 0) {
 	        					  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldC, srcStr, fieldB);
 	        				  }
 	        				  else {
 	        					  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldB, srcStr, fieldC);
 	        				  }
 	        			  }
 	        			  else { // mode = 0 && numSp = 1 && ps != 0 && off_sel = 0 && numDp = 2
 	        				  strcat(formatString,"[%s%r,%r],[%s%r]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldB, fieldC, srcStr, fieldC);
 	        			  }
 	        		  }
 	        		  else { // mode = 0 && numSp = 1 && ps != 0 && off_sel != 0
 	        			  if ( numDp == 1 ) { // mode = 0 && numSp = 1 && ps != 0 && off_sel != 0 && numDp = 1
 	        				  strcat(formatString,"[%s%r],[%s%d]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldC, srcStr, imm_off);
 	        			  }
 	        			  else { // mode = 0 && numSp = 1 && ps != 0 && off_sel != 0 && numDp = 2
 	        				  strcat(formatString,"[%s%r,%d],[%s%r]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldB, imm_off, srcStr, fieldC);
 	        			  }
 	        		  }
 	        	  }
 	          }
 	      }
 	      else { // mode = 1
	          if ( ps == 0 ) { // mode = 1 && ps = 0
	        	  if ( off_sel == 0 ) { // mode = 1 && ps = 0 && off_sel = 0
	        		  if ( entry_sel == 0 ) { // mode = 1 && ps = 0 && off_sel = 0 && entry_sel = 0
	        			  strcat(formatString,"%r,[%s%r],[%s%r,%r,%r]");
	        			  my_sprintf(state, state->operandBuffer, formatString, fieldB, dstStr, fieldC, srcStr, fieldB, fieldC, fieldC);
	        		  }
	        		  else { // mode = 1 && ps = 0 && off_sel = 0 && entry_sel != 0
	        			  strcat(formatString,"%r,[%s%r],[%s%r,%d,%r]");
	        			  my_sprintf(state, state->operandBuffer, formatString, fieldB, dstStr, fieldC, srcStr, fieldB, imm_entry, fieldC);
	        		  }
	        	  }
	        	  else { // mode = 1 && ps = 0 && off_sel != 0
	        		  if ( entry_sel == 0 ) { // mode = 1 && ps = 0 && off_sel != 0 && entry_sel = 0
	        			  strcat(formatString,"%r,[%s%r],[%s%r,%r,%d]");
	        			  my_sprintf(state, state->operandBuffer, formatString, fieldB, dstStr, fieldC, srcStr, fieldB, fieldC, imm_off);
	        		  }
	        		  else { // mode = 1 && ps = 0 && off_sel != 0 && entry_sel != 0
	        			  strcat(formatString,"%r,[%s%r],[%s%r,%d,%d]");
	        			  my_sprintf(state, state->operandBuffer, formatString, fieldB, dstStr, fieldC, srcStr, fieldB, imm_entry, imm_off);
	        		  }
	        	  }
	          }
	          else { // mode = 1 && ps != 0
	        	  if ( off_sel == 0 ) { // mode = 1 && ps != 0 && off_sel = 0
	        		  if ( entry_sel == 0 ) { // mode = 1 && ps != 0 && off_sel = 0 && entry_sel = 0
 	        			  if ( numDp == 1 ) { // mode = 1 && ps != 0 && off_sel = 0 && entry_sel = 0 && numDp = 1
 	        				  strcat(formatString,"[%s%r],[%s%r,%r,%r]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldC, srcStr, fieldB, fieldC, fieldC);
 	        			  }
 	        			  else {  // mode = 1 && ps != 0 && off_sel = 0 && entry_sel = 0 && numDp = 2
 	        				  strcat(formatString,"[%s%r,%r,%r],[%s%r]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldB, fieldC, fieldC, srcStr, fieldC);
 	        			  }
	        		  }
	        		  else { // mode = 1 && ps != 0 && off_sel = 0 && entry_sel != 0
 	        			  if ( numDp == 1 ) { // mode = 1 && ps != 0 && off_sel = 0 && entry_sel != 0 && numDp = 1
 	        				  strcat(formatString,"[%s%r],[%s%r,%d,%r]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldC, srcStr, fieldB, imm_entry, fieldC);
 	        			  }
 	        			  else {  // mode = 1 && ps != 0 && off_sel = 0 && entry_sel != 0 && numDp = 2
 	        				  strcat(formatString,"[%s%r,%d,%r],[%s%r]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldB, imm_entry, fieldC, srcStr, fieldC);
 	        			  }
	        		  }
	        	  }
	        	  else { // mode = 1 && ps != 0 && off_sel != 0
	        		  if ( entry_sel == 0 ) { // mode = 1 && ps != 0 && off_sel != 0 && entry_sel = 0
 	        			  if ( numDp == 1 ) { // mode = 1 && ps != 0 && off_sel != 0 && entry_sel = 0 && numDp = 1
 	        				  strcat(formatString,"[%s%r],[%s%r,%r,%d]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldC, srcStr, fieldB, fieldC, imm_off);
 	        			  }
 	        			  else { // mode = 1 && ps != 0 && off_sel != 0 && entry_sel = 0 && numDp = 1
 	        				  strcat(formatString,"[%s%r,%r,%d],[%s%r]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldB, fieldC, imm_off, srcStr, fieldC);
 	        			  }
	        		  }
	        		  else { // mode = 1 && ps != 0 && off_sel != 0 && entry_sel != 0
 	        			  if ( numDp == 1 ) { // mode = 1 && ps != 0 && off_sel != 0 && entry_sel != 0 && numDp = 1
 	        				  strcat(formatString,"[%s%r],[%s%r,%d,%d]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldC, srcStr, fieldB, imm_entry, imm_off);
 	        			  }
 	        			  else {  // mode = 1 && ps != 0 && off_sel != 0 && entry_sel != 0 && numDp = 2
 	        				  strcat(formatString,"[%s%r,%d,%d],[%s%r]");
 	        				  my_sprintf(state, state->operandBuffer, formatString, dstStr, fieldB, imm_entry, imm_off, srcStr, fieldC);
 	        			  }
	        		  }
	        	  }
	          }
 	      }
          break;
      }
    case 78:
      {
    	   int subOpc2;
    	   int sri;
    	   int imm_size;
    	   int shash_pad;
    	   int shash_init;
    	   int type;
    	   int hm;
           char name[20];
           int modePrint;
    	   subOpc2 = (state->words[0] & 0x00000007);
    	   sri = (state->words[0] & 0x00000008) >> 3;
    	   shash_pad = (state->words[0] & 0x00000010) >> 4;
    	   shash_init = (state->words[0] & 0x00002000) >> 13;
    	   imm_size = (state->words[0] & 0x00001FE0) >> 5;
    	   type = (state->words[0] & 0x000000f8) >> 3;
    	   hm  = (state->words[0] & 0x00004000) >> 14;
           FIELD_C_AC16();
           FIELD_B_AC16();
           strcpy(name,instrName);
           if ( subOpc2 == 4 ) {
        	   switch ( type ) {
        	       case 0x08: strcpy(name,"cpssta");   modePrint = 1; break;
        	       case 0x00: strcpy(name,"cpssta");   modePrint = 2; break;
        	       case 0x0a: strcpy(name,"cpsiv") ;   modePrint = 1; break;
        	       case 0x02: strcpy(name,"cpsiv") ;   modePrint = 2; break;
        	       case 0x0c: strcpy(name,"cpsmac");   modePrint = 1; break;
        	       case 0x04: strcpy(name,"cpsmac");   modePrint = 2; break;
        	       case 0x09: strcpy(name,"cpskey");   modePrint = 1; break;
        	       case 0x01: strcpy(name,"cpskey");   modePrint = 2; break;
        	       case 0x0b: strcpy(name,"cpsctx");   modePrint = 1; break;
        	       case 0x03: strcpy(name,"cpsctx");   modePrint = 2; break;
        	       default:
        	    	   sprintf(name,"?%02xsec",type); break;
        	   }
           }
           if ( subOpc2 == 2 ) {
        	   if (hm) {
        		   if (shash_pad)
    					strcpy(name,"hmac.opad");
        		   else
        			   strcpy(name,"hmac.ipad");
        	   }
           }

           instrName = name;
  	       write_instr_name();
    	   switch (subOpc2) {
	       	   case 0:
	       	   case 1:
	       		   if ( sri == 0 ) {
	       			   strcat(formatString,"%r,[cm:%r],[cm:%r],%r");
	       			   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldB, fieldC);
	       		   }
	       		   else {
	       			   strcat(formatString,"%r,[cm:%r],[cm:%r],%d");
	       			   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldB, imm_size);
	       		   }
	       		   break;
	       	   case 2:
	       		   if ( sri == 0 ) {
	       			   if (hm) {
		       			   strcat(formatString,"%r,[cm:%r],%r");
		       			   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldC);
	       			   }
	       			   else {
		       			   strcat(formatString,"%r,[cm:%r],%r,%d,%d");
		       			   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldC, shash_init, shash_pad);
	       			   }
	       		   }
	       		   else {
	       			   if (hm) {
	       				   strcat(formatString,"%r,[cm:%r],%d");
	       				   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, imm_size);
	       			   }
	       			   else {
	       				   strcat(formatString,"%r,[cm:%r],%d,%d,%d");
	       				   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, imm_size, shash_init, shash_pad);
	       			   }
	       		   }
	       		   break;
	       	   case 3:
	       		   strcat(formatString,"[sm:%r]");
   		   		   my_sprintf(state, state->operandBuffer, formatString, fieldC);
   		   		   break;
	       	   case 4:
	       		   switch ( modePrint ) {
	       		   	   case 1:
	       		   		   strcat(formatString,"[sm:%r],[cm:%r]");
	       		   		   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB);
	       		   		   break;
	       		   	   case 2:
	       		   		   strcat(formatString,"[cm:%r],[sm:%r]");
	       		   		   my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
	       		   		   break;
	       		   	   default:
	       		   		   strcat(formatString,"???cps");
	       		   		   my_sprintf(state, state->operandBuffer, formatString);
	       		   		   break;
	       		   }
	       		   break;
	       	   case 5:
	       		   if ( sri == 0 ) {
	       			   strcat(formatString,"%r,[cm:%r],%r");
	       			   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldC);
	       		   }
	       		   else {
       				   strcat(formatString,"%r,[cm:%r],%d");
       				   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, imm_size);
	       		   }
   		   		   break;
	       	   case 6:
	       		   if (sri == 0) {
	       			   strcat(formatString,"%r,[cm:%r],%r,%d,%d");
	       			   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, fieldC,shash_init,shash_pad);
	       		   }
	       		   else {
	       			   strcat(formatString,"%r,[cm:%r],%d,%d,%d");
	       			   my_sprintf(state, state->operandBuffer, formatString, fieldC, fieldB, imm_size,shash_init,shash_pad);
	       		   }
	       		   break;
	       	    default:
    		   	   strcat(formatString,"???secure");
    		   	   my_sprintf(state, state->operandBuffer, formatString);
    		   	   break;
    	   }
    	   break;
      }
    case 79:
        FIELD_C();
        fieldB = 0;
    	write_instr_name();
    	strcat(formatString,"%d,[cjid:%r]");
    	my_sprintf(state, state->operandBuffer, formatString, fieldB,fieldC);
    	break;
    case 80:
      {
    	  int maxlen;
    	  int xri;
    	  int src1;
   	      maxlen = (state->words[0] & 0x00001FE0) >> 5;
   	      xri = (state->words[0] & 0x00004000) >> 14;
   	      src1 = (state->words[0] & 0x00008000) >> 15;
          FIELD_C_AC16();
          FIELD_B_AC16();
          if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
          if ( maxlen == 0 ) maxlen = 256;
 	      write_instr_name();
 	      if ( xri == 0 ) {
 	    	  if ( src1 == 0 ) {
 	    	    	strcat(formatString,"%r,%r,%d");
 	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC, maxlen);
 	    	  }
 	    	  else {
	    	    	strcat(formatString,"%r,%r,%r,%d");
	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC, maxlen);
 	    	  }
 	      }
 	      else {
 	    	  if ( src1 == 0 ) {
 	    	    	strcat(formatString,"%r,%r");
 	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
 	    	  }
 	    	  else {
	    	    	strcat(formatString,"%r,%r,%r");
	    	    	my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC);
 	    	  }
 	      }
      }
      break;
    case 81:
      {
    	  int usrc1;
    	  int pos;
    	  int size;
    	  usrc1 = (state->words[0] & 0x00008000) >> 15;
    	  size = (state->words[0] & 0x00007C00) >> 10;
   	      pos = (state->words[0] & 0x000003E0) >> 5;
          FIELD_C_AC16();
          FIELD_B_AC16();
          if ( (state->words[0] & 0x00100000) != 0 ) flag = 1;
          size++;
 	      write_instr_name();
 	      if ( usrc1 == 0 ) {
    	      strcat(formatString,"%r,%r,%d,%d");
    	      my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC, pos,size);
    	  }
 	      else {
  	    	  strcat(formatString,"%r,%r,%r,%d,%d");
  	    	  my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldB, fieldC, pos,size);
 	      }
      }
      break;
    case 82:
      {
    	  int i0,i1,i2,i3;
    	  int min_hofs,psbc;
    	  int subOpcode;
          CHECK_FIELD_C();
          CHECK_FIELD_B();
          CHECK_FIELD_A();
          CHECK_FLAG();
 	      write_instr_name();
 	 	  WRITE_FORMAT_x(A);
 	 	  WRITE_FORMAT_COMMA_x(B);
 	 	  subOpcode = BITS(state->words[0],16,21);
          if (is_limm) {
        	  switch(subOpcode) {
        	  case 0x36:
        		  min_hofs = (limm_value >> 0) & 0xF;
        		  psbc = (limm_value >> 5) & 0x1;
        		  strcat(formatString,",%d,%d");
        		  my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, min_hofs, psbc);
        		  break;
        	  default:
        		  i0 = (limm_value >>  0) & 0x0FF;
        		  i1 = (limm_value >>  8) & 0x0FF;
        		  i2 = (limm_value >> 16) & 0x0FF;
        		  i3 = (limm_value >> 24) & 0x0FF;
        		  strcat(formatString,",%d,%d,%d,%d");
        		  my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, i0, i1,i2,i3);
        		  break;
        	  }
          }
          else {
  	         switch(subOpcode) {
  	         case 0x36:
  	        	 if (BITS(state->words[0],22,23) == 1) {
  	  	        	 min_hofs = (BITS(state->words[0],6,9)) * 16;
  	  	        	 psbc = BITS(state->words[0],11,11);
  	  	        	 strcat(formatString,",%d,%d");
  	  	        	 my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, min_hofs, psbc);
  	        	 }
  	        	 else {
  	        		 WRITE_FORMAT_COMMA_x(C);
  	        		 my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
  	        	 }
  	        	 break;
  	         default:
  	        	 WRITE_FORMAT_COMMA_x(C);
  	        	 WRITE_NOP_COMMENT();
  	        	 my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldC);
  	        	 break;
  	         }
          }
      }
      break;
    case 83:
      {
    	int subop2;

      	fieldA = 62; // dummy value only to update limm_value
        CHECK_FIELD(fieldA);
      	fieldA = BITS(state->words[0],21,26);
      	fieldB = BITS(state->words[0],16,20);
      	fieldC = BITS(state->words[0],11,15);
       	fieldAisReg = 1;
       	if ( fieldA == 62 ) {
       		fieldA = 0;
       		fieldAisReg = 0;
       	}
       	subop2 = (limm_value & 0xffff0000) >> 16;
       	write_instr_name();
       	WRITE_FORMAT_x(A);
       	if (BITS(limm_value,15,15)) {
        	int size1_64 = (limm_value & 0xff) >> 0;
       		strcat(formatString,",[cm:%r],[cm:%r],%r,%d,%d");
       		my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, fieldC, size1_64, subop2);
       	}
       	else{
       		strcat(formatString,",[cm:%r],[cm:%r],%r,%d");
       		my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, fieldC, subop2);
       	}
      }
      break;
    case 84:
      {
     	  CHECK_FIELD_A();
    	  CHECK_FIELD_B();
    	  CHECK_FIELD_C();

    	  write_instr_name();
    	  WRITE_FORMAT_x(A);

    	  if (is_limm) {
    	   	  int ops, sid, keysize, res_len;

    	   	  sid = (limm_value & 0x7f00) >> 8;
    	   	  keysize = (limm_value & 0x7f) >> 0;
    	   	  if (BITS(state->words[0],16,21) == 0x31){//lkpitcm
    	   		  ops = (limm_value & 0x1ff8000) >> 19;
    	   		  strcat(formatString,",[cm:%r],[cm:%r],%d,%d,%d");
    	   		  my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, sid, ops, keysize);
    	   	  }
       	   	  else {//lkpetcm
       	   		  ops = (limm_value & 0xc00000) >> 22;
       	   		  res_len = (limm_value & 0xff000000) >> 24;
       	   		  strcat(formatString,",[cm:%r],[cm:%r],%d,%d,%d,%d");
    	 		  my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, sid, ops, keysize,res_len);
       	   	  }
    	  }
    	  else {
    		  strcat(formatString,",[cm:%r],[cm:%r],%r");
    		  my_sprintf(state, state->operandBuffer, formatString, fieldA, fieldB, fieldB, fieldC);
    	  }
      }
      break;
    case 85:
      {
    	  int subop2;
    	  int m, p, na, x;
    	  int os, es, ss;
    	  char *dstStr;
    	  char *srcStr;
    	  char name[20] = {"cp"};
    	  int entry_size_val, offset_val, size_val;

    	  fieldA = 62; // dummy value only to update limm_value
    	  CHECK_FIELD(fieldA);
    	  fieldA = BITS(state->words[0],21,26);
    	  fieldB = BITS(state->words[0],16,20);
    	  fieldC = BITS(state->words[0],11,15);
          if ( fieldA == 62 ) {
        	  fieldA = 0;
        	  fieldAisReg = 0;
          }
          else
              fieldAisReg = 1;

          subop2 = (limm_value >> 29) & 0x7;
    	  switch (subop2) {
    	  case 4:
    		  dstStr = "csd"; srcStr = "cm";
    		  break;
    	  case 5:
    		  dstStr = "cxd"; srcStr = "cm";
    		  break;
    	  case 6:
    		  dstStr = "cm"; srcStr = "csd";
    		  break;
    	  case 7:
    		  dstStr = "cm"; srcStr = "cxd";
    		  break;
    	  default:
    		  dstStr = "??"; srcStr = "??";
    		  break;
    	  }

    	  os = (limm_value >> 25) & 0x1;
    	  es = (limm_value >> 26) & 0x1;
    	  ss = (limm_value >> 27) & 0x1;

    	  m = (limm_value & 0x10000000) >> 28;
    	  na = (limm_value & 0x01000000) >> 24;
    	  x = (limm_value & 0x00800000) >> 23;
      	  if ( m != 0 ) strcat(name,".m");
      	  if ( na != 0 ) strcat(name,".na");
      	  if ( x != 0 ) strcat(name,".x");

    	  offset_val = os ? ((limm_value >> 10) & 0x3ff) : fieldC;
    	  entry_size_val = es ? (16 << ((limm_value >> 20) & 0x7)) : fieldC;
    	  size_val = ss ? ((limm_value >> 0) & 0x3ff) : fieldC;

      	  instrName = name;
      	  write_instr_name();
      	  WRITE_FORMAT_x(A);
      	  if (subop2 == 6 || subop2 == 7) {
          	  strcat(formatString,",[%s:%r");
          	  strcat(formatString,es ? ",%d" : ",%r");
          	  strcat(formatString,os ? ",%d]" : ",%r]");
          	  strcat(formatString,",[%s:%r]");
          	  strcat(formatString,ss ? ",%d" : ",%r");
    	  	  my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, fieldB, entry_size_val, offset_val, dstStr, fieldB, size_val);
      	  }
      	  else {
          	  strcat(formatString,",[%s:%r]");
          	  strcat(formatString,",[%s:%r");
          	  strcat(formatString,es ? ",%d" : ",%r");
          	  strcat(formatString,os ? ",%d]" : ",%r]");
          	  strcat(formatString,ss ? ",%d" : ",%r");
          	  my_sprintf(state, state->operandBuffer, formatString, fieldA, srcStr, fieldB, dstStr, fieldB, entry_size_val, offset_val, size_val);
      	  }
      }
      break;
    case 86:
    {
    	FIELD_B();
        FIELD_C();
        write_instr_name();
        WRITE_FORMAT_x_COMMA_LB(B);
        strcat(formatString, "cm:");
        WRITE_FORMAT_COMMA_x_RB(C);
        my_sprintf(state, state->operandBuffer, formatString, fieldB, fieldC);
    }
    break;
    case 87:
    {
    	int addrType;
    	int mcris;
    	int ersis;
    	int oris;
    	int entry_size_val;
    	int offset_val;
    	int mntCode;
    	char *typeStr;

      	fieldA = 62; // dummy value only to update limm_value
        CHECK_FIELD(fieldA);
      	fieldA = BITS(state->words[0],21,26);
      	fieldB = BITS(state->words[0],16,20);
      	fieldC = BITS(state->words[0],11,15);
      	fieldA = 0;
      	fieldAisReg = 0;
       	addrType = (limm_value & 0x7) >> 0;
       	mcris = (limm_value & 0x8) >> 3;
       	ersis = (limm_value & 0x10) >> 4;
       	oris = (limm_value & 0x20) >> 5;
       	entry_size_val =  ((limm_value & 0x3f00) >> 8) << 3;
       	offset_val = (limm_value & 0x3ff0000) >> 16;
       	mntCode = (limm_value & 0xf0000000) >> 28;
       	write_instr_name();
       	WRITE_FORMAT_x(A);

        switch (addrType) {
        case 0:
        	typeStr = "";
        	break;
        case 1:
        	typeStr = "xa:";
        	break;
        case 3:
        	typeStr = "sd:";
        	break;
        case 4:
        	typeStr = "xd:";
        	break;
        default:
        	typeStr = "???";
        	break;
        }
        if ((addrType == 0) || (addrType == 1)){
        	if (mcris == 1) {
        		strcat(formatString,",[%s%r],%d");
        		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, mntCode);
        	}
        	else {
        		strcat(formatString,",[%s%r],%r");
        		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, fieldC);
        	}
        }
        else { //addrType == 3 || addrType == 4
        	if ((ersis == 1) && (oris == 1)) {
        		if (mcris == 1) {
            		strcat(formatString,",[%s%r,%d,%d],%d");
            		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, entry_size_val, offset_val, mntCode);
        		}
        		else { //mcris == 0
            		strcat(formatString,",[%s%r,%d,%d],%r");
            		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, entry_size_val, offset_val, fieldC);
        		}
        	}
        	else if ((ersis == 1) && (oris == 0)) {
        		if (mcris == 1) {
            		strcat(formatString,",[%s%r,%d,%r],%d");
            		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, entry_size_val, fieldC, mntCode);
        		}
        		else { //mcris == 0
            		strcat(formatString,",[%s%r,%d,%r],%r");
            		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, entry_size_val, fieldC, fieldC);
        		}
        	}
        	else if ((ersis == 0) && (oris == 1)) {
        		if (mcris == 1) {
            		strcat(formatString,",[%s%r,%r,%d],%d");
            		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, fieldC, offset_val, mntCode);
        		}
        		else { //mcris == 0
            		strcat(formatString,",[%s%r,%r,%d],%r");
            		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, fieldC, offset_val, fieldC);
        		}
        	}
        	else { //ersis == 0 && oris == 0
        		if (mcris == 1) {
            		strcat(formatString,",[%s%r,%r,%r],%d");
            		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, fieldC, fieldC, mntCode);
        		}
        		else { //mcris == 0
            		strcat(formatString,",[%s%r,%r,%r],%r");
            		my_sprintf(state, state->operandBuffer, formatString, fieldA, typeStr, fieldB, fieldC, fieldC, fieldC);
        		}
        	}
        }
    }
    break;
   }
#endif // #ifdef ARC_NPS_CMDS

  default:
    mwerror(state, "Bad decoding class in ARC disassembler");
    break;
  }

  state->_cond = cond;
  state->_addrWriteBack = addrWriteBack;
  return state->instructionLen = offset;
}


/*
 * _coreRegName - Returns the name the user specified core extension
 *                register.
 */
static const char *
_coreRegName
(
 void *_this ATTRIBUTE_UNUSED, /* C++ this pointer */
 int v                         /* Register value */
 )
{
  return arcExtMap_coreRegName(v);
}

/*
 * _auxRegName - Returns the name the user specified AUX extension
 *               register.
 */
static const char *
_auxRegName
( void *_this ATTRIBUTE_UNUSED, /* C++ this pointer */
  int v                         /* Register value */
  )
{
  return arcExtMap_auxRegName(v);
}


/*
 * _condCodeName - Returns the name the user specified condition code
 *                 name.
 */
static const char *
_condCodeName
(
 void *_this ATTRIBUTE_UNUSED, /* C++ this pointer */
 int v                         /* Register value */
 )
{
  return arcExtMap_condCodeName(v);
}


/*
 * _instName - Returns the name the user specified extension instruction.
 */
static const char *
_instName
(
 void *_this ATTRIBUTE_UNUSED, /* C++ this pointer */
 int op1,                      /* major opcode value */
 int op2,                      /* minor opcode value */
 int *flags                    /* instruction flags */
 )
{
  return arcExtMap_instName(op1, op2, flags);
}

static void
parse_disassembler_options (char *options)
{
  const char *p;
  for (p = options; p != NULL; )
    {
#ifndef ARC_NO_SIMD_CMDS
	  if (CONST_STRNEQ (p, "simd"))
	    {
	      enable_simd = 1;
	    }
#endif // #ifndef ARC_NO_SIMD_CMDS
	  if (CONST_STRNEQ (p, "insn-stream"))
	    {
		  enable_insn_stream = 1;
	    }

	  p = strchr (p, ',');

	  if (p != NULL)
		p++;

    }

}

/* ARCompact_decodeInstr - Decode an ARCompact instruction returning the
   size of the instruction in bytes or zero if unrecognized.  */
int
ARCompact_decodeInstr (bfd_vma           address,    /* Address of this instruction.  */
		       disassemble_info* info)
{
  int status;
  bfd_byte buffer[4];
  struct arcDisState s;	/* ARC Disassembler state */
  void *stream = info->stream; /* output stream */
  fprintf_ftype func = info->fprintf_func;
  int lowbyte, highbyte;
  char buf[256];

  if (info->disassembler_options)
    {
      parse_disassembler_options (info->disassembler_options);

      /* To avoid repeated parsing of these options, we remove them here.  */
      info->disassembler_options = NULL;
    }

  lowbyte = ((info->endian == BFD_ENDIAN_LITTLE) ? 1 : 0);
  highbyte = ((info->endian == BFD_ENDIAN_LITTLE) ? 0 : 1);

  memset(&s, 0, sizeof(struct arcDisState));

  /* read first instruction */
  status = (*info->read_memory_func) (address, buffer, 2, info);

  if (status != 0)
    {
      (*info->memory_error_func) (status, address, info);
      return -1;
    }

#ifdef ARC_NPS_CMDS
  if ((((buffer[lowbyte] & 0xf8) > 0x38) && ((buffer[lowbyte] & 0xf8) != 0x48)
      && ((buffer[lowbyte] & 0xf8) != 0x50) && (((buffer[lowbyte] & 0xf8) != 0x58) || ((buffer[lowbyte+1] & 0x18) == 0x00)))
      || ((info->mach == bfd_mach_arc_arcv2) && ((buffer[lowbyte] & 0xF8) == 0x48)))
#else
  if ((((buffer[lowbyte] & 0xf8) > 0x38) && ((buffer[lowbyte] & 0xf8) != 0x48))
      || ((info->mach == bfd_mach_arc_arcv2) && ((buffer[lowbyte] & 0xF8) == 0x48)))
#endif // #ifdef ARC_NPS_CMDS
  {
    s.instructionLen = 2;
    s.words[0] = (buffer[lowbyte] << 8) | buffer[highbyte];
    status = (*info->read_memory_func) (address + 2, buffer, 4, info);
    if (info->endian == BFD_ENDIAN_LITTLE)
      s.words[1] = bfd_getl32(buffer);
    else
      s.words[1] = bfd_getb32(buffer);
  }
  else
  {
    s.instructionLen = 4;
    status = (*info->read_memory_func) (address + 2, &buffer[2], 2, info);
    if (status != 0)
    {
      (*info->memory_error_func) (status, address + 2, info);
      return -1;
    }
    if (info->endian == BFD_ENDIAN_LITTLE)
      s.words[0] = bfd_getl32(buffer);
    else
      s.words[0] = bfd_getb32(buffer);

    /* always read second word in case of limm */
    /* we ignore the result since last insn may not have a limm */
    status = (*info->read_memory_func) (address + 4, buffer, 4, info);
    if (info->endian == BFD_ENDIAN_LITTLE)
      s.words[1] = bfd_getl32(buffer);
    else
      s.words[1] = bfd_getb32(buffer);
  }

  s._this = &s;
  s.coreRegName = _coreRegName;
  s.auxRegName = _auxRegName;
  s.condCodeName = _condCodeName;
  s.instName = _instName;

  /* disassemble, discarding result which is never used. */
  (void) dsmOneArcInst(address, (void *)&s, info);

  /* display the disassembled instruction */
  {
    char* instr   = s.instrBuffer;
    char* operand = s.operandBuffer;
    char* space   = strchr(instr, ' ');

    if (enable_insn_stream)
      {
	/* Show instruction stream from MSB to LSB*/

	if (s.instructionLen == 2)
	  (*func) (stream, "    %04x ", (unsigned int) s.words[0]);
	else
	  (*func) (stream, "%08x ",     (unsigned int) s.words[0]);

	(*func) (stream, "    ");
      }

    /* if the operand is actually in the instruction buffer */
    if ((space != NULL) && (operand[0] == '\0'))
      {
	  *space  = '\0';
	  operand = space + 1;
      }

    (*func) (stream, "%-10s ", instr);

    if (__TRANSLATION_REQUIRED(s))
      {
	bfd_vma addr;
	char *tmpBuffer;
	int i = 1;

	if (operand[0] != '@')
	{
	  /* Branch instruction with 3 operands, Translation is required
	     only for the third operand. Print the first 2 operands */
	  strcpy(buf, operand);
	  tmpBuffer = strtok(buf,"@");
	  (*func) (stream, "%s", tmpBuffer);
	  i = strlen(tmpBuffer) + 1;
	}

	addr = s.addresses[operand[i] - '0'];
	(*info->print_address_func) ((bfd_vma) addr, info);
	(*func) (stream, "\n");
      }
    else
      (*func) (stream, "%s", operand);
  }

  /* We print max bytes for instruction */
  info->bytes_per_line = 8;

  return s.instructionLen;

}

/*
 * This function is the same as decodeInstr except that this function
 * returns a struct arcDisState instead of the instruction length.
 *
 * This struct contains information useful to the debugger.
 */
struct arcDisState
arcAnalyzeInstr
(
 bfd_vma           address,		/* Address of this instruction */
 disassemble_info* info
 )
{
  int status;
  bfd_byte buffer[4];
  struct arcDisState s;	/* ARC Disassembler state */
  int lowbyte, highbyte;

  lowbyte = ((info->endian == BFD_ENDIAN_LITTLE) ? 1 : 0);
  highbyte = ((info->endian == BFD_ENDIAN_LITTLE) ? 0 : 1);

  memset(&s, 0, sizeof(struct arcDisState));

  /* read first instruction */
  status = (*info->read_memory_func) (address, buffer, 2, info);

  if (status != 0)
    {
      (*info->memory_error_func) (status, address, info);
      s.instructionLen = -1;
      return s;
    }

#ifdef ARC_NPS_CMDS
  if ( ((buffer[lowbyte] & 0xf8) > 0x38) &&
	   ((buffer[lowbyte] & 0xf8) != 0x48) &&
	   ((buffer[lowbyte] & 0xf8) != 0x50) &&
	   ( ((buffer[lowbyte] & 0xf8) != 0x58) || ((buffer[lowbyte+1] & 0x18) == 0x00) ) )
#else
  if (((buffer[lowbyte] & 0xf8) > 0x38) && ((buffer[lowbyte] & 0xf8) != 0x48))
#endif // #ifdef ARC_NPS_CMDS
  {
    s.instructionLen = 2;
    s.words[0] = (buffer[lowbyte] << 8) | buffer[highbyte];
    status = (*info->read_memory_func) (address + 2, buffer, 4, info);
    if (info->endian == BFD_ENDIAN_LITTLE)
      s.words[1] = bfd_getl32(buffer);
    else
      s.words[1] = bfd_getb32(buffer);
  }
  else
  {
    s.instructionLen = 4;
    status = (*info->read_memory_func) (address + 2, &buffer[2], 2, info);
    if (status != 0)
    {
      (*info->memory_error_func) (status, address + 2, info);
      s.instructionLen = -1;
      return s;
    }
    if (info->endian == BFD_ENDIAN_LITTLE)
      s.words[0] = bfd_getl32(buffer);
    else
      s.words[0] = bfd_getb32(buffer);

    /* always read second word in case of limm */
    /* we ignore the result since last insn may not have a limm */
    status = (*info->read_memory_func) (address + 4, buffer, 4, info);
    if (info->endian == BFD_ENDIAN_LITTLE)
      s.words[1] = bfd_getl32(buffer);
    else
      s.words[1] = bfd_getb32(buffer);
  }

  s._this = &s;
  s.coreRegName = _coreRegName;
  s.auxRegName = _auxRegName;
  s.condCodeName = _condCodeName;
  s.instName = _instName;

  /* disassemble, throwing away result which is never used. */
  (void) dsmOneArcInst(address, (void *)&s, info);
  /* We print max bytes for instruction */
  info->bytes_per_line = 8;
  return s;
}


void
arc_print_disassembler_options (FILE *stream)
{
  fprintf (stream, "\n\
 ARC-specific disassembler options:\n\
 use with the -M switch, with options separated by commas\n\n");

  fprintf (stream, "  insn-stream    Show the instruction byte stream from most\n");
  fprintf (stream, "                 significant byte to least significant byte (excluding LIMM).\n");
  fprintf (stream, "                 This option is useful for viewing the actual encoding of instructions.\n");

  fprintf (stream, "  simd           Enable SIMD instructions disassembly.\n\n");
}
