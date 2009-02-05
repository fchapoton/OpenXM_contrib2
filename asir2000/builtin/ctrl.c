/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/ctrl.c,v 1.38 2007/01/25 16:19:40 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pctrl();

struct ftab ctrl_tab[] = {
	{"ctrl",Pctrl,-2},
	{"asir_env",Pctrl,-2},
	{0,0,0},
};

extern int error_in_timer;
extern int prtime,nez,echoback,bigfloat;
extern int debug_up;
extern int GC_max_heap_size,Verbose,hideargs,hex_output,do_server_in_X11;
extern int do_message,do_terse;
extern int ox_batch,ox_check,ox_exchange_mathcap;
extern int f4_nocheck;
extern int StrassenSize;
extern int outputstyle;
extern int fortran_output;
extern int real_digit;
extern int real_binary;
extern int allow_laurent;
#if defined(INTERVAL)
extern int zerorewrite;
extern int Itvplot;
#endif
extern int double_output;
extern int use_new_hensel;
extern int print_quote;
extern int show_crossref;
extern Obj user_defined_prompt;
extern int asir_setenv;
extern int show_orderspec;
extern int no_debug_on_error;
extern int diag_period;

static struct {
	char *key;
	int *val;
} ctrls[] = {
	{"error_in_timer",&error_in_timer},
	{"cputime",&prtime},
	{"nez",&nez},
	{"echo",&echoback},
#if defined(PARI)
	{"bigfloat",&bigfloat},
#endif
	{"verbose",&Verbose},
	{"quiet_mode",&do_quiet},
	{"hideargs",&hideargs},
	{"hex",&hex_output},
	{"debug_window",&do_server_in_X11},
	{"message",&do_message},
	{"terse",&do_terse},
	{"debug_up",&debug_up},
	{"no_prompt",&do_quiet},
	{"asir_setenv",&asir_setenv},
	{"ox_batch",&ox_batch},
	{"ox_check",&ox_check},
	{"ox_exchange_mathcap",&ox_exchange_mathcap},
	{"f4_nocheck",&f4_nocheck},
	{"StrassenSize",&StrassenSize},
	{"outputstyle",&outputstyle},
	{"double_output",&double_output},
	{"real_digit",&real_digit},
	{"real_binary",&real_binary},
	{"fortran_output",&fortran_output},
	{"new_hensel",&use_new_hensel},
	{"print_quote",&print_quote},
	{"show_crossref",&show_crossref},
	{"allow_laurent",&allow_laurent},
	{"show_orderspec",&show_orderspec},
	{"no_debug_on_error",&no_debug_on_error},
	{"diag_period",&diag_period},
#if defined(INTERVAL)
	{"zerorewrite",&zerorewrite},
	{"itvplotsize",&Itvplot},
#endif
	{0,0},
};

void Pctrl(arg,rp)
NODE arg;
Q *rp;
{
	int t,i,n;
	int nm,dv;
	N num,den;
	Q c;
	char *key;
	char buf[BUFSIZ];
	char *str;

	if ( !arg ) {
		*rp = 0;
		return;
	}
	key = BDY((STRING)ARG0(arg));
	if ( !strcmp(key,"adj") ) {
		/* special treatment is necessary for "adj" */
		if ( argc(arg) == 1 ) {
			Risa_GC_get_adj(&nm,&dv);
			UTON(dv,num);
			UTON(nm,den);
		} else {
			c = (Q)ARG1(arg);
			if ( !c )
				error("ctrl : adj : invalid argument");
			num = NM(c);
			den = !DN(c)?ONEN:DN(c);
			dv = BD(num)[0];
			nm = BD(den)[0];
			Risa_GC_set_adj(nm,dv);
		}
		NDTOQ(num,den,1,*rp);
		return;
	} else if ( !strcmp(key,"prompt") ) {
		/* special treatment is necessary for "prompt" */
		if ( argc(arg) == 1 ) {
		  if ((!do_quiet) && (user_defined_prompt == (Obj)0)) *rp=ONE;
          else *rp = (Q)user_defined_prompt;
		} else {
			c = (Q)ARG1(arg);
			if ( !c ) {
				do_quiet = 1;
				user_defined_prompt = 0;
				*rp = 0;
			} else if ( OID(c) == O_STR ) {
				str = BDY((STRING)c);
			 	for ( i = 0, n = 0; str[i]; i++ )
					if ( str[i] == '%' )
						n++;
				if ( n >= 2 )
					error("ctrl : prompt : invalid prompt specification");	
				do_quiet = 1;
				user_defined_prompt = (Obj)c;
				*rp = c;
			} else if ( NUM(c) && RATN(c) && UNIQ(c) ) {
				user_defined_prompt = 0;
				do_quiet = 0;
			} else {
				error("ctrl : prompt : invalid argument");	
			}
		}
		return;
	}
	for ( i = 0; ctrls[i].key; i++ )
		if ( !strcmp(key,ctrls[i].key) )
			break;
	if ( ctrls[i].key ) {
		if ( argc(arg) == 1 )
			t = *ctrls[i].val;
		else
			*ctrls[i].val = t = QTOS((Q)ARG1(arg));
		STOQ(t,*rp);
	} else {
		sprintf(buf,"ctrl : %s : no such key",key);
		error(buf);
	}
}
