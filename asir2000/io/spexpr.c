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
 * $OpenXM: OpenXM_contrib2/asir2000/io/spexpr.c,v 1.30 2004/03/17 02:10:32 noro Exp $
*/
#include "ca.h"
#include "al.h"
#include "parse.h"
#include "comp.h"
#include "base.h"

#if defined(PARI)
#include "genpari.h"
#endif

#ifndef SPRINT
#define SPRINT
#endif

#define PRINTHAT (fortran_output?PUTS("**"):PUTS("^"))
extern int outputstyle;

#ifdef FPRINT
FILE *asir_out;
#define OUT asir_out
char DFORMAT[BUFSIZ];
int hex_output;
int fortran_output;
int double_output;
int real_digit;
int real_binary;
int print_quote;

#define TAIL
#define PUTS(s) fputs(s,OUT)
#define PRINTF fprintf
#define PRINTN printn
#define PRINTBF printbf
#define PRINTCPLX printcplx
#define PRINTLM printlm
#define PRINTUP2 printup2
#define PRINTV printv
#define PRINTEXPR printexpr
#define PRINTNUM printnum
#define PRINTP asir_printp
#define PRINTR printr
#define PRINTLIST printlist
#define PRINTVECT printvect
#define PRINTMAT printmat
#define PRINTSTR printstr
#define PRINTCOMP printcomp
#define PRINTDP printdp
#define PRINTUI printui
#define PRINTGF2MAT printgf2mat
#define PRINTGFMMAT printgfmmat
#define PRINTBYTEARRAY printbytearray
#define PRINTQUOTE printquote
#define PRINTERR printerr
#define PRINTLF printlf
#define PRINTLOP printlop
#define PRINTFOP printfop
#define PRINTEOP printeop
#define PRINTQOP printqop
#define PRINTUP printup
#define PRINTUM printum
#define PRINTSF printsf
#define PRINTSYMBOL printsymbol
#define PRINTRANGE printrange
#define PRINTTB printtb
#define PRINTFNODE printfnode
#define PRINTFNODENODE printfnodenode
#define PRINTFARGS printfargs
#endif

#ifdef SPRINT
static char *buf;
#define OUT buf
extern char DFORMAT[BUFSIZ];
extern int hex_output;
extern int fortran_output;
extern int double_output;
extern int real_digit;
extern int real_binary;
extern int print_quote;


#define TAIL while ( *OUT ) OUT++;
#define PUTS(s) strcat(OUT,s)
#define PRINTF sprintf
#define PRINTN sprintn
#define PRINTBF sprintbf
#define PRINTCPLX sprintcplx
#define PRINTLM sprintlm
#define PRINTUP2 sprintup2
#define PRINTV sprintv
#define PRINTEXPR sprintexpr
#define PRINTNUM sprintnum
#define PRINTP asir_sprintp
#define PRINTR sprintr
#define PRINTLIST sprintlist
#define PRINTVECT sprintvect
#define PRINTMAT sprintmat
#define PRINTSTR sprintstr
#define PRINTCOMP sprintcomp
#define PRINTDP sprintdp
#define PRINTUI sprintui
#define PRINTGF2MAT sprintgf2mat
#define PRINTGFMMAT sprintgfmmat
#define PRINTBYTEARRAY sprintbytearray
#define PRINTQUOTE sprintquote
#define PRINTERR sprinterr
#define PRINTLF sprintlf
#define PRINTLOP sprintlop
#define PRINTFOP sprintfop
#define PRINTEOP sprinteop
#define PRINTQOP sprintqop
#define PRINTUP sprintup
#define PRINTUM sprintum
#define PRINTSF sprintsf
#define PRINTSYMBOL sprintsymbol
#define PRINTRANGE sprintrange
#define PRINTTB sprinttb
#define PRINTFNODE sprintfnode
#define PRINTFNODENODE sprintfnodenode
#define PRINTFARGS sprintfargs
#endif

void PRINTEXPR();
void PRINTNUM();
void PRINTN();
void PRINTV();
void PRINTP();
void PRINTR();
void PRINTLIST();
void PRINTVECT();
void PRINTMAT();
void PRINTSTR();
void PRINTCOMP();
void PRINTDP();
void PRINTUI();
void PRINTGF2MAT();
void PRINTGFMMAT();
void PRINTBYTEARRAY();
void PRINTQUOTE();
void PRINTERR();
void PRINTCPLX();
void PRINTLM();
void PRINTLF();
void PRINTUP2();
void PRINTUP();
void PRINTUM();
void PRINTFOP();
void PRINTEOP();
void PRINTLOP();
void PRINTQOP();
void PRINTSF();
void PRINTSYMBOL();
void PRINTRANGE();
void PRINTTB();
void PRINTFNODE();

#ifdef FPRINT
void output_init() {
	OUT = stdout;
	sprintf(DFORMAT,"%%0%dd",DLENGTH);
}

int mmono(p)
P p;
{
	if ( NUM(p) )
#if defined(INTERVAL)
		if ( NID(p) != N_IP && NID(p) != N_IntervalDouble && NID(p) != N_IntervalQuad && NID(p) != N_IntervalBigFloat
			&& compnum(CO,(Num)p,0) < 0 ) 
#else
		if ( compnum(CO,(Num)p,0) < 0 ) 
#endif
			return ( 1 );
		else
			return ( 0 );
	else if ( NEXT(DC(p)) )
		return ( 0 );
	else
		return (mmono(COEF(DC(p))));
}

#if defined(PARI)
void printbf(a)
BF a;
{
	sor(a->body,double_output ? 'f' : 'g',-1,0);
}
#endif
#endif

#ifdef SPRINT
void soutput_init(s)
char *s;
{
	s[0] = 0; buf = s;
}

#if defined(PARI)
void myoutbrute(g)
GEN g;
{
# if PARI_VERSION_CODE > 131588
	brute(g, 'f', -1);
# else
	bruteall(g,'f',-1,1);
# endif
}

void sprintbf(a)
BF a;
{
	char *str;
	char *GENtostr0();

	if ( double_output ) {
		str = GENtostr0(a->body,myoutbrute);
	} else {
		str = GENtostr(a->body);
	}
	TAIL PRINTF(OUT,"%s",str);
	free(str);
}
#endif
#endif

void PRINTEXPR(vl,p)
VL vl;
Obj p;
{
	if ( !p ) {
		PRINTR(vl,(R)p);
		return;
	}

	switch ( OID(p) ) {
		case O_N:
			PRINTNUM((Num)p); break;
		case O_P:
			PRINTP(vl,(P)p); break;
		case O_R:
			PRINTR(vl,(R)p); break;
		case O_LIST:
			PRINTLIST(vl,(LIST)p); break;
		case O_VECT:
			PRINTVECT(vl,(VECT)p); break;
		case O_MAT:
			PRINTMAT(vl,(MAT)p); break;
		case O_STR:
			PRINTSTR((STRING)p); break;
		case O_COMP:
			PRINTCOMP(vl,(COMP)p); break;
		case O_DP:
			PRINTDP(vl,(DP)p); break;
		case O_USINT:
			PRINTUI(vl,(USINT)p); break;
		case O_GF2MAT:
			PRINTGF2MAT(vl,(GF2MAT)p); break;
		case O_ERR:
			PRINTERR(vl,(ERR)p); break;
		case O_MATHCAP:
			PRINTLIST(vl,((MATHCAP)p)->body); break;
		case O_F:
			PRINTLF(vl,(F)p); break;
		case O_GFMMAT:
			PRINTGFMMAT(vl,(GFMMAT)p); break;
		case O_BYTEARRAY:
			PRINTBYTEARRAY(vl,(BYTEARRAY)p); break;
		case O_QUOTE:
			PRINTQUOTE(vl,(QUOTE)p); break;
		case O_SYMBOL:
			PRINTSYMBOL((SYMBOL)p); break;
		case O_RANGE:
			PRINTRANGE(vl,(RANGE)p); break;
		case O_TB:
			PRINTTB(vl,(TB)p); break;
		default:
			break;
	}
}

void PRINTN(n)
N n;
{
	register int i,*ptr;
	N tn;

	if ( !n ) {
		PUTS("0");
		return;
	}
	if ( hex_output ) {
		ptr = BD(n);
		TAIL PRINTF(OUT,"0x%x",ptr[PL(n)-1]);
		if ( hex_output < 0 )
			for ( i = PL(n) - 2; i >= 0; i-- ) {
				TAIL PRINTF(OUT,"|%08x",ptr[i]);
			}
		else
			for ( i = PL(n) - 2; i >= 0; i-- ) {
				TAIL PRINTF(OUT,"%08x",ptr[i]);
			}
	} else {
		ntobn(DBASE,n,&tn);
		ptr = BD(tn);
		TAIL PRINTF(OUT,"%d",ptr[PL(tn) - 1]);
		for ( i = PL(tn) - 2; i >= 0; i-- ) {
			TAIL PRINTF(OUT,DFORMAT,ptr[i]);
		}
	}
}

#if defined(FPRINT)
#if defined(ITVDEBUG)
void printbin(double z)
{
	int	i, j, mask;
	union {
		double  x;
		char    c[8];
	} a;

	a.x = z;
	for(i=7;i>=0;i--) {
		mask = 0x80;
		for(j=0;j<8;j++) {
			if (a.c[i] & mask) fprintf(stderr,"1");
			else fprintf(stderr,"0");
			mask >>= 1;
		}
	}
	fprintf(stderr,"\n");
}
#endif
#endif

#if defined(FPRINT)
#if 0
int	printmode = PRINTF_E;
#else
int	printmode = PRINTF_G;
#endif
#else
extern int printmode;
#endif

void PRINTNUM(q)
Num q;
{
	char real_format[20];

	if ( !q ) {
		PUTS("0");
		return;
	}
	switch ( NID(q) ) {
		case N_Q:
			if ( SGN((Q)q) == -1 ) 
				PUTS("-");
			PRINTN(NM((Q)q));
			if ( !INT((Q)q) ) {
				PUTS("/"); PRINTN(DN((Q)q));
			}
			break;
		case N_R:
			switch (printmode) {
				case PRINTF_E:
#if defined(INTERVAL)
				case MID_PRINTF_E:
#endif
					TAIL PRINTF(OUT,"%.16e",BDY((Real)q));
					break;
				case PRINTF_G:
#if defined(INTERVAL)
				case MID_PRINTF_G:
#endif
				default:
					if ( real_binary ) {
						unsigned int *m;
						unsigned int u,l,mask;
						int i,expo;

						m = (unsigned int *)&BDY((Real)q);
#if defined(__i386__) || defined(MIPSEL) || defined(VISUAL) || defined(__alpha) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__x86_64)
						u = m[1]; l = m[0];
#else
						u = m[0]; l = m[1];
#endif
						if (u&0x80000000) {
							TAIL PRINTF(OUT,"-");
						}
						u &= 0x7fffffff;
						expo = ((int)(u>>20));
						u &= 0xfffff;
						if ( expo == 2047 ) {
							if ( u || l ) {
								TAIL PRINTF(OUT,"NaN");
							} else {
								TAIL PRINTF(OUT,"Inf");
							}
						} else if ( expo == 0 ) {
							if ( u || l ) {
								TAIL PRINTF(OUT,"0b0.");
								for ( i = 0, mask = 0x80000; i < 20; 
									i++, mask >>= 1) {
									TAIL
									if ( u&mask )
										PRINTF(OUT,"1");
									else
										PRINTF(OUT,"0");
								}
								for ( i = 0, mask = 0x80000000; i < 32;
									i++, mask >>= 1) {
									TAIL
									if ( l&mask )
										PRINTF(OUT,"1");
									else
										PRINTF(OUT,"0");
								}
								TAIL PRINTF(OUT,"*2^%d",-1022);
							} else {
								TAIL PRINTF(OUT,"0");
							}
						} else {
							expo -= 1023;
							TAIL PRINTF(OUT,"0b1.");
							for ( i = 0, mask = 0x80000; i < 20; 
								i++, mask >>= 1) {
								TAIL
								if ( u&mask )
									PRINTF(OUT,"1");
								else
									PRINTF(OUT,"0");
							}
							for ( i = 0, mask = 0x80000000; i < 32;
								i++, mask >>= 1) {
								TAIL
								if ( l&mask )
									PRINTF(OUT,"1");
								else
									PRINTF(OUT,"0");
							}
							if ( expo ) {
								TAIL PRINTF(OUT,"*2^%d",expo);
							}
						}
					} else if ( real_digit ) {
						sprintf(real_format,
							double_output?"%%.%df":"%%.%dg",real_digit);
						TAIL PRINTF(OUT,real_format,BDY((Real)q));
					} else {
						TAIL PRINTF(OUT,double_output?"%f":"%g",BDY((Real)q));
					}
					break;
			}
			break;
		case N_A:
			PUTS("("); PRINTR(ALG,(R)BDY((Alg)q)); PUTS(")");
			break;
#if defined(PARI)
		case N_B:
			PRINTBF((BF)q); break;
#endif
#if defined(INTERVAL)
		case N_IP:
		case N_IntervalBigFloat:
			switch ( outputstyle ) {
				case 1:
					PUTS("intval(");
					PRINTNUM(INF((Itv)q));
					PUTS(",");
					PRINTNUM(SUP((Itv)q));
					PUTS(")");
					break;
				case 0:
				default:
					PUTS("[");
					PRINTNUM(INF((Itv)q));
					PUTS(",");
					PRINTNUM(SUP((Itv)q));
					PUTS("]");
					break;
			}
			break;
		case N_IntervalDouble:
			switch (printmode) {
				case PRINTF_E:
					switch ( outputstyle ) {
						case 1:
							TAIL PRINTF(OUT, "intval(%.16e,%.16e)",
								INF((IntervalDouble)q),SUP((IntervalDouble)q));
							break;
						case 0:
						default:
							TAIL PRINTF(OUT, "[%.16e,%.16e]",
								INF((IntervalDouble)q),SUP((IntervalDouble)q));
							break;
					}
#if defined(ITVDEBUG)
					printbin(INF((IntervalDouble)q));
					printbin(SUP((IntervalDouble)q));
#endif
					break;
				case MID_PRINTF_G:
					switch ( outputstyle ) {
						case 1:
							TAIL PRINTF(OUT, "intvalm(%g,%g)",
								(SUP((IntervalDouble)q)+INF((IntervalDouble)q))*0.5,
								(SUP((IntervalDouble)q)-INF((IntervalDouble)q))*0.5);
							break;
						case 0:
						default:
							TAIL PRINTF(OUT, "<%g,%g>",
								(SUP((IntervalDouble)q)+INF((IntervalDouble)q))*0.5,
								(SUP((IntervalDouble)q)-INF((IntervalDouble)q))*0.5);
							break;
					}
					break;
				case MID_PRINTF_E:
					switch ( outputstyle ) {
						case 1:
							TAIL PRINTF(OUT, "intvalm(%.16e,%.16e)",
							(SUP((IntervalDouble)q)+INF((IntervalDouble)q))*0.5,
							(SUP((IntervalDouble)q)-INF((IntervalDouble)q))*0.5);
							break;
						case 0:
						default:
							TAIL PRINTF(OUT, "<%.16e,%.16e>",
							(SUP((IntervalDouble)q)+INF((IntervalDouble)q))*0.5,
							(SUP((IntervalDouble)q)-INF((IntervalDouble)q))*0.5);
							break;
					}
					break;
				case PRINTF_G:
				default:
					switch ( outputstyle ) {
						case 1:
							TAIL PRINTF(OUT, "intval(%g,%g)",
								INF((IntervalDouble)q),SUP((IntervalDouble)q));
							break;
						case 0:
						default:
							TAIL PRINTF(OUT, "[%g,%g]",
								INF((IntervalDouble)q),SUP((IntervalDouble)q));
							break;
					}
				break;
			}
			break;
#endif
		case N_C:
			PRINTCPLX((C)q); break;
		case N_M:
			TAIL PRINTF(OUT,"%d",CONT((MQ)q)); break;
		case N_LM:
			PRINTN(((LM)q)->body); break;
		case N_GF2N:
			if ( hex_output )
				PRINTN((N)(((GF2N)q)->body));
			else
				PRINTUP2(((GF2N)q)->body);
			break;
		case N_GFPN:
			PRINTUP((UP)(((GFPN)q)->body));
			break;
		case N_GFS:
			TAIL PRINTF(OUT,"@_%d",CONT((GFS)q));
			break;
		case N_GFSN:
			PRINTUM(BDY((GFSN)q));
			break;
	}
}

void PRINTCPLX(a)
C a;
{
	PUTS("(");
	if ( a->r )
		PRINTNUM(a->r);
	if ( a->i ) {
#if defined(INTERVAL)
		if ( a->r && ((compnum(0,a->i,0) > 0)
			|| NID(a->i) == N_IP || NID(a->i) == N_IntervalDouble
			|| NID(a->i) == N_IntervalQuad || NID(a->i) == N_IntervalBigFloat) )
#else
		if ( a->r && (compnum(0,a->i,0) > 0) )
#endif
			PUTS("+");
		PRINTNUM(a->i); PUTS("*@i");
	}
	PUTS(")");
}

void PRINTP(vl,p)
VL vl;
P p;
{
	V v;
	DCP dc;

	if ( !p )
		PUTS("0");
	else if ( NUM(p) ) 
		PRINTNUM((Num)p);
	else
		for ( dc = DC(p), v = VR(p); dc; dc = NEXT(dc) ) {
			if ( !DEG(dc) )
				PRINTP(vl,COEF(dc));
			else {
				if ( NUM(COEF(dc)) && UNIQ((Q)COEF(dc)) ) {
					;
				} else if ( NUM(COEF(dc)) && MUNIQ((Q)COEF(dc)) )
					PUTS("-"); 
				else if ( NUM(COEF(dc)) || !NEXT(DC(COEF(dc)))) {
					PRINTP(vl,COEF(dc)); PUTS("*"); 
				} else {
					PUTS("("); PRINTP(vl,COEF(dc)); PUTS(")*"); 
				}
				PRINTV(vl,v); 
				if ( cmpq(DEG(dc),ONE) ) {
					PRINTHAT;
					if ( INT(DEG(dc)) && SGN(DEG(dc))>0 )
						PRINTNUM((Num)DEG(dc));
					else {
						PUTS("("); PRINTNUM((Num)DEG(dc)); PUTS(")");
					}
				}
			}
			if ( NEXT(dc) ) {
				P t;

				t = COEF(NEXT(dc));
				if (!DEG(NEXT(dc))) {
					if ( NUM(t) ) {
						if ( !mmono(t) )
							PUTS("+");
					} else {
						if (!mmono(COEF(DC(t))))
							PUTS("+");
					}
				} else {
					if ( !mmono(t) )
						PUTS("+");
				}
			}
		}
}

int hideargs;

void PRINTV(vl,v)
VL vl;
V v;
{
	PF pf;
	PFAD ad;
	int i;

	if ( NAME(v) )
		PUTS(NAME(v));
	else if ( (vid)v->attr == V_PF ) {
		pf = ((PFINS)v->priv)->pf; ad = ((PFINS)v->priv)->ad;
		if ( !strcmp(NAME(pf),"pow") ) {
			PUTS("(("); PRINTR(vl,(R)ad[0].arg); PUTS(")"); PRINTHAT; PUTS("(");
			PRINTR(vl,(R)ad[1].arg); PUTS("))");
		} else if ( !pf->argc ) {
			TAIL PRINTF(OUT,"%s",NAME(pf));
		} else {
			if ( hideargs ) {
				for ( i = 0; i < pf->argc; i++ )
					if ( ad[i].d )
						break;
				if ( i < pf->argc ) {
					TAIL PRINTF(OUT,"%s{%d",NAME(pf),ad[0].d);
					for ( i = 1; i < pf->argc; i++ ) {
						TAIL PRINTF(OUT,",%d",ad[i].d);
					}
					PUTS("}");
				} else {
					TAIL PRINTF(OUT,"%s",NAME(pf));
				}
			} else {
				for ( i = 0; i < pf->argc; i++ )
					if ( ad[i].d )
						break;
				if ( i < pf->argc ) {
					TAIL PRINTF(OUT,"%s{%d",NAME(pf),ad[0].d);
					for ( i = 1; i < pf->argc; i++ ) {
						TAIL PRINTF(OUT,",%d",ad[i].d);
					}
					PUTS("}(");
				} else {
					TAIL PRINTF(OUT,"%s(",NAME(pf));
				}
				PRINTR(vl,(R)ad[0].arg);
				for ( i = 1; i < pf->argc; i++ ) {
					PUTS(","); PRINTR(vl,(R)ad[i].arg);
				}
				PUTS(")");
			}
		}
	}
}

void PRINTR(vl,a)
VL vl;
R a;
{
	if ( !a ) 
		PUTS("0");
	else
		switch (OID(a)) {
			case O_N: case O_P:
				PRINTP(vl,(P)a); break;
			default:
				PUTS("("); PRINTP(vl,NM((R)a)); PUTS(")/("); PRINTP(vl,DN((R)a)); PUTS(")");
				break;
		}
}

void PRINTVECT(vl,vect)
VL vl;
VECT vect;
{
	int i;
	pointer *ptr;

	switch ( outputstyle ) {
		case 1:
			PUTS("vect(");
			for ( i = 0, ptr = BDY(vect); i < vect->len; i++ ) {
				if ( i != 0 ) PUTS(",");
				PRINTEXPR(vl,ptr[i]);
			}
			PUTS(")");
			break;
		case 0:
		default:
			PUTS("[ ");
			for ( i = 0, ptr = BDY(vect); i < vect->len; i++ ) {
				PRINTEXPR(vl,ptr[i]); PUTS(" ");
			}
			PUTS("]");
			break;
	}
}

void PRINTMAT(vl,mat)
VL vl;
MAT mat;
{
	int i,j,r,c;
	pointer *ptr;

	switch ( outputstyle ) {
		case 1:
			PUTS("mat(\n");
			for ( i = 0, r = mat->row, c = mat->col; i < r; i++ ) {
				if ( i != 0 ) PUTS(",\n");
				PUTS("[ ");
				for ( j = 0, ptr = BDY(mat)[i]; j < c; j++ ) {
					if ( j != 0 ) PUTS(",");
					PRINTEXPR(vl,ptr[j]);
				}
				PUTS(" ]");
			}
			PUTS(")");
			break;
		case 0:
		default:
			for ( i = 0, r = mat->row, c = mat->col; i < r; i++ ) {
				PUTS("[ ");
				for ( j = 0, ptr = BDY(mat)[i]; j < c; j++ ) {
					PRINTEXPR(vl,ptr[j]); PUTS(" ");
				}
				PUTS("]");
				if ( i < r - 1 )
					PUTS("\n");
			}
			break;
	}
}

void PRINTLIST(vl,list)
VL vl;
LIST list;
{
	NODE tnode;

	PUTS("[");
	for ( tnode = (NODE)list->body; tnode; tnode = NEXT(tnode) ) {
		PRINTEXPR(vl,tnode->body);
		if ( NEXT(tnode) )
			PUTS(",");
	}
	PUTS("]");
}

void PRINTSTR(str)
STRING str;
{
	char *p;

	for ( p = BDY(str); *p; p++ )
		if ( *p == '"' )
			PUTS("\"");
		else {
			TAIL PRINTF(OUT,"%c",*p);
		}
}

void PRINTCOMP(vl,c)
VL vl;
COMP c;
{
	int n,i;

	n = getcompsize((int)c->type);
	PUTS("{");
	for ( i = 0; i < n; i++ ) {
		PRINTEXPR(vl,(pointer)c->member[i]);
		if ( i < n-1 )
			PUTS(",");
	}
	PUTS("}");
}

void PRINTDP(vl,d)
VL vl;
DP d;
{
	int n,i;
	MP m;
	DL dl;

	for ( n = d->nv, m = BDY(d); m; m = NEXT(m) ) {
		PUTS("("); PRINTEXPR(vl,(pointer)m->c); PUTS(")*<<");
		for ( i = 0, dl = m->dl; i < n-1; i++ ) {
			TAIL PRINTF(OUT,"%d,",dl->d[i]);
		}
		TAIL PRINTF(OUT,"%d",dl->d[i]);
		PUTS(">>");
		if ( NEXT(m) )
			PUTS("+");
	}
}

void PRINTUI(vl,u)
VL vl;
USINT u;
{
	TAIL PRINTF(OUT,"%u",BDY(u));
}

void PRINTGF2MAT(vl,mat)
VL vl;
GF2MAT mat;
{
	int row,col,w,i,j,k,m;
	unsigned int t;
	unsigned int **b;

	row = mat->row;
	col = mat->col;
	w = (col+BSH-1)/BSH;
	b = mat->body;
	for ( i = 0; i < row; i++ ) {
		for ( j = 0, m = 0; j < w; j++ ) {
			t = b[i][j];
			for ( k = 0; m < col && k < BSH; k++, m++ )
				if ( t & (1<<k) )
					PUTS("1");
				else
					PUTS("0");
		}
		PUTS("\n");
	}
}

void PRINTGFMMAT(vl,mat)
VL vl;
GFMMAT mat;
{
	int row,col,i,j;
	unsigned int **b;

	row = mat->row;
	col = mat->col;
	b = mat->body;
	for ( i = 0; i < row; i++ ) {
		PUTS("[");
		for ( j = 0; j < col; j++ ) {
			TAIL PRINTF(OUT,"%8d",b[i][j]);
		}
		PUTS("]\n");
	}
}

void PRINTBYTEARRAY(vl,array)
VL vl;
BYTEARRAY array;
{
	int len,i;
	unsigned char *b;

	len = array->len;
	b = array->body;
	PUTS("|");
	for ( i = 0; i < len-1; i++ ) {
		TAIL PRINTF(OUT,"%02x ",(unsigned int)b[i]);
	}
	TAIL PRINTF(OUT,"%02x",(unsigned int)b[i]);
	PUTS("|");
}

void PRINTQUOTE(vl,quote)
VL vl;
QUOTE quote;
{
	LIST list;

	if ( print_quote == 1 ) {
		fnodetotree(BDY(quote),&list);
		PRINTEXPR(vl,(Obj)list);
	} else if ( print_quote == 2 ) {
		PRINTFNODE(BDY(quote),0);
	} else {
		PUTS("<...quoted...>");
	}
}

void PRINTERR(vl,e)
VL vl;
ERR e;
{
	PUTS("error("); PRINTEXPR(vl,e->body); PUTS(")");
}

void PRINTUP2(p)
UP2 p;
{
	int d,i;

	if ( !p ) {
		TAIL PRINTF(OUT,"0");
	} else {
		d = degup2(p);
		TAIL PRINTF(OUT,"(");
		if ( !d ) {
			TAIL PRINTF(OUT,"1");
		} else if ( d == 1 ) {
			TAIL PRINTF(OUT,"@");
		} else {
			PUTS("@"); PRINTHAT; TAIL PRINTF(OUT,"%d",d);
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( p->b[i/BSH] & (1<<(i%BSH)) )
				if ( !i ) {
					TAIL PRINTF(OUT,"+1");
				} else if ( i == 1 ) {
					TAIL PRINTF(OUT,"+@");
				} else {
					PUTS("+@"); PRINTHAT; TAIL PRINTF(OUT,"%d",i);
				}
		}
		TAIL PRINTF(OUT,")");
	}
}

void PRINTLF(vl,f)
VL vl;
F f;
{
	switch ( FOP(f) ) {
		case AL_TRUE:
			TAIL PRINTF(OUT,"@true");
			break;
		case AL_FALSE:
			TAIL PRINTF(OUT,"@false");
			break;

		case AL_OR: case AL_AND:
			PRINTFOP(vl,f); break;
		case AL_NOT: case AL_IMPL: case AL_REPL: case AL_EQUIV:
			PRINTEOP(vl,f); break;

		case AL_EQUAL: case AL_NEQ: case AL_LESSP:
		case AL_GREATERP: case AL_LEQ: case AL_GEQ:
			PRINTLOP(vl,f); break;

		case AL_EX: case AL_ALL:
			PRINTQOP(vl,f); break;
		default:
			break;
	}
}

void PRINTFOP(vl,f)
VL vl;
F f;
{
	char *op;
	NODE n;

	op = FOP(f)==AL_OR?" @|| ":" @&& ";
	n = FJARG(f);
	PUTS("("); PRINTEXPR(vl,BDY(n)); PUTS(")");
	for ( n = NEXT(n); n; n = NEXT(n) ) {
		PUTS(op); PUTS("("); PRINTEXPR(vl,BDY(n)); PUTS(")");
	}
}

void PRINTEOP(vl,f)
VL vl;
F f;
{
	oFOP op;
	char *sop;

	if ( (op = FOP(f)) == AL_NOT ) {
		PUTS("(@! "); PRINTEXPR(vl,(Obj)FARG(f)); PUTS(")"); return;
	}
	switch ( op ) {
		case AL_IMPL:
			sop = " @impl "; break;
		case AL_REPL:
			sop = " @repl "; break;
		case AL_EQUIV:
			sop = " @equiv "; break;
		default:
			break;
	}
	PUTS("("); 
	PRINTEXPR(vl,(Obj)FLHS(f)); 
	PUTS(sop);
	PRINTEXPR(vl,(Obj)FRHS(f)); 
	PUTS(")");
}

void PRINTLOP(vl,f)
VL vl;
F f;
{
	char *op;

	switch ( FOP(f) ) {
		case AL_EQUAL:
			op = " @== "; break;
		case AL_NEQ:
			op = " @!= "; break;
		case AL_LESSP:
			op = " @< "; break;
		case AL_GREATERP:
			op = " @> "; break;
		case AL_LEQ:
			op = " @<= "; break;
		case AL_GEQ:
			op = " @>= "; break;
		default:
			error("PRINTLOP : invalid operator");
			break;
	}
	PRINTEXPR(vl,(Obj)FPL(f)); PUTS(op); PUTS("0");
}

void PRINTQOP(vl,f)
VL vl;
F f;
{
	char *op;

	op = FOP(f)==AL_EX?"ex":"all";
	TAIL PRINTF(OUT,"%s(%s,",op,NAME(FQVR(f)));
	PRINTEXPR(vl,(Obj)FQMAT(f)); PUTS(")");
}

void PRINTUP(n)
UP n;
{
	int i,d;

	if ( !n )
		PUTS("0");
	else if ( !n->d )
		PRINTNUM(n->c[0]);
	else {
		d = n->d;
		PUTS("(");
		if ( !d ) {
			PRINTNUM(n->c[d]);
		} else if ( d == 1 ) {
			PRINTNUM(n->c[d]);
			PUTS("*@p");
		} else {
			PRINTNUM(n->c[d]);
			PUTS("*@p"); PRINTHAT; TAIL PRINTF(OUT,"%d",d);
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( n->c[i] ) {
				PUTS("+("); PRINTNUM(n->c[i]); PUTS(")");
				if ( i >= 2 ) {
					PUTS("*@p"); PRINTHAT; TAIL PRINTF(OUT,"%d",i);
				} else if ( i == 1 )
					PUTS("*@p");
			}
		}
		PUTS(")");
	}
}

void PRINTUM(n)
UM n;
{
	int i,d;

	if ( !n )
		PUTS("0");
	else if ( !n->d )
		PRINTSF(n->c[0]);
	else {
		d = n->d;
		PUTS("(");
		if ( !d ) {
			PRINTSF(n->c[d]);
		} else if ( d == 1 ) {
			PRINTSF(n->c[d]);
			PUTS("*@s");
		} else {
			PRINTSF(n->c[d]);
			PUTS("*@s"); PRINTHAT; TAIL PRINTF(OUT,"%d",d);
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( n->c[i] ) {
				PUTS("+("); PRINTSF(n->c[i]); PUTS(")");
				if ( i >= 2 ) {
					PUTS("*@s"); PRINTHAT; TAIL PRINTF(OUT,"%d",i);
				} else if ( i == 1 )
					PUTS("*@s");
			}
		}
		PUTS(")");
	}
}

void PRINTSF(i)
unsigned int i;
{
	if ( !i ) {
		PUTS("0");
	} else {
		TAIL PRINTF(OUT,"@_%d",IFTOF(i));
	}
}

void PRINTSYMBOL(SYMBOL sym)
{
	PUTS(sym->name);
}

void PRINTRANGE(VL vl,RANGE p)
{
	PUTS("range(");
	PRINTEXPR(vl,p->start);
	PUTS(",");
	PRINTEXPR(vl,p->end);
	PUTS(")");
}

void PRINTTB(VL vl,TB p)
{
	int i;

	for ( i = 0; i < p->next; i++ ) {
		PUTS(p->body[i]);
	}
}

void PRINTFNODENODE(NODE n)
{
	for ( ; n; n = NEXT(n) ) {
		PRINTFNODE((FNODE)BDY(n),0);
		if ( NEXT(n) ) PUTS(",");
	}
}

void PRINTFARGS(FNODE f)
{
	NODE n;

	if ( f->id == I_LIST ) {
		n = (NODE)FA0(f);
		PRINTFNODENODE(n);
	} else
		PRINTFNODE(f,0);
}

void PRINTFNODE(FNODE f,int paren)
{
	NODE n,t,t0;
	char vname[BUFSIZ],prefix[BUFSIZ];
	char *opname,*vname_conv,*prefix_conv;
	Obj obj;
	int i,len,allzero,elen,elen2;
	C cplx;
	char *r;
	FNODE fi,f2;

	if ( !f ) {
		PUTS("(0)");
		return;
	}
	if ( paren ) PUTS("(");
	switch ( f->id ) {
		/* unary operators */
		case I_NOT: PUTS("!"); PRINTFNODE((FNODE)FA0(f),1); break;
		case I_PAREN: PRINTFNODE((FNODE)FA0(f),0); break;
		case I_MINUS: PUTS("-"); PRINTFNODE((FNODE)FA0(f),1); break;
		/* binary operators */
		/* arg list */
		/* I_AND, I_OR => FA0(f), FA1(f) */
		/* otherwise   => FA1(f), FA2(f) */
		case I_BOP:
			PRINTFNODE((FNODE)FA1(f),1);
			PUTS(((ARF)FA0(f))->name);
			PRINTFNODE((FNODE)FA2(f),1);
			break;
		case I_COP:
			switch( (cid)FA0(f) ) {
				case C_EQ: opname = ("=="); break;
				case C_NE: opname = ("!="); break;
				case C_GT: opname = (">"); break;
				case C_LT: opname = ("<"); break;
				case C_GE: opname = (">="); break;
				case C_LE: opname = ("<="); break;
			}
			PRINTFNODE((FNODE)FA1(f),1);
			PUTS(opname);
			PRINTFNODE((FNODE)FA2(f),1);
			break;
		case I_LOP:
			switch( (lid)FA0(f) ) {
				case L_EQ: opname = ("@=="); break;
				case L_NE: opname = ("@!="); break;
				case L_GT: opname = ("@>"); break;
				case L_LT: opname = ("@<"); break;
				case L_GE: opname = ("@>="); break;
				case L_LE: opname = ("@<="); break;
				case L_AND: opname = ("@&&"); break;
				case L_OR: opname = ("@||"); break;
				case L_NOT: opname = ("@!"); break;
			}
			if ( (lid)FA0(f)==L_NOT ) {
				PUTS(opname); PRINTFNODE((FNODE)FA1(f),1);
			} else {
				PRINTFNODE((FNODE)FA1(f),1);
				PUTS(opname);
				PRINTFNODE((FNODE)FA2(f),1);
			}
			break;
		case I_AND:
			PRINTFNODE((FNODE)FA0(f),1);
			PUTS("&&");
			PRINTFNODE((FNODE)FA1(f),1);
			break;
		case I_OR:
			PRINTFNODE((FNODE)FA0(f),1);
			PUTS("!!");
			PRINTFNODE((FNODE)FA1(f),1);
			break;
		/* ternary operators */
		case I_CE:
			PRINTFNODE((FNODE)FA0(f),1); PUTS("?"); PRINTFNODE((FNODE)FA1(f),1);
			PUTS(":"); PRINTFNODE((FNODE)FA2(f),1);
			break;
		/* lists */
		case I_LIST: PUTS("["); PRINTFNODENODE((NODE)FA0(f)); PUTS("]"); break;
		/* function */
		case I_FUNC:
			if ( !strcmp(((FUNC)FA0(f))->name,"@pi") ) PUTS("@pi");
			else if ( !strcmp(((FUNC)FA0(f))->name,"@e") ) PUTS("@e");
			else {
				PUTS(((FUNC)FA0(f))->name);
				PUTS("("); PRINTFARGS(FA1(f)); PUTS(")");
			}
			break;
		/* XXX */
		case I_CAR: PUTS("car("); PRINTFNODE(FA0(f),0); PUTS(")"); break;
		case I_CDR: PUTS("cdr("); PRINTFNODE(FA0(f),0); PUTS(")"); break;
		/* exponent vector */
		case I_EV: PUTS("<<"); PRINTFNODENODE((NODE)FA0(f)); PUTS(">>"); break;
		/* string */
		case I_STR: PUTS((char *)FA0(f)); break;
		/* internal object */
		case I_FORMULA: obj = (Obj)FA0(f); PRINTEXPR(CO,obj); break;
		/* program variable */
		case I_PVAR:
			if ( FA1(f) )
				error("printfnode : not implemented yet");
			GETPVNAME(FA0(f),opname);
			PUTS(opname);
			break;
		default: error("printfnode : not implemented yet");
	}
	if ( paren ) PUTS(")");
}
