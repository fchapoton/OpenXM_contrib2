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
 * $OpenXM: OpenXM_contrib2/asir2000/io/cpexpr.c,v 1.18 2004/03/03 09:25:30 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "al.h"
#include "base.h"
#if PARI
#include "genpari.h"
#endif

extern int hex_output,fortran_output,double_output,real_digit;

#define PRINTHAT (fortran_output?PUTS("**"):PUTS("^"))

#define TAIL
#define PUTS(s) (total_length+=strlen(s))
#define PRINTN length_n
#define PRINTBF length_bf
#define PRINTCPLX length_cplx
#define PRINTLM length_lm
#define PRINTUP2 length_up2
#define PRINTV length_v
#define PRINTEXPR length_expr
#define PRINTNUM length_num
#define PRINTP length_p
#define PRINTR length_r
#define PRINTLIST length_list
#define PRINTVECT length_vect
#define PRINTMAT length_mat
#define PRINTSTR length_str
#define PRINTCOMP length_comp
#define PRINTDP length_dp
#define PRINTUI length_ui
#define PRINTGF2MAT length_gf2mat
#define PRINTGFMMAT length_gfmmat
#define PRINTBYTEARRAY length_bytearray
#define PRINTQUOTE length_QUOTE
#define PRINTSYMBOL length_SYMBOL
#define PRINTRANGE length_RANGE
#define PRINTTB length_TB
#define PRINTERR length_err
#define PRINTLF length_lf
#define PRINTLOP length_lop
#define PRINTFOP length_fop
#define PRINTEOP length_eop
#define PRINTQOP length_qop
#define PRINTUP length_up
#define PRINTUM length_um
#define PRINTSF length_sf
#define PRINTFARGS length_fargs
#define PRINTFNODENODE length_fnodenode
#define PRINTFNODE length_fnode

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
void PRINTSYMBOL();
void PRINTRANGE();
void PRINTTB();
void PRINTERR();
void PRINTCPLX();
void PRINTLM();
void PRINTLF();
void PRINTUP();
void PRINTUM();
void PRINTUP2();
void PRINTFOP();
void PRINTEOP();
void PRINTLOP();
void PRINTQOP();
void PRINTSF();
void PRINTFNODE();

static int total_length;

int estimate_length(VL vl,pointer p)
{
	total_length = 0;
	PRINTEXPR(vl,p);
	return total_length;
}

#if defined(PARI)
void PRINTBF(BF a)
{
	char *str;
	char *GENtostr0();
	void myoutbrute();

	if ( double_output ) {
		str = GENtostr0(a->body,myoutbrute);
	} else {
		str = GENtostr(a->body);
	}
	total_length += strlen(str);
	free(str);
}
#endif

void PRINTEXPR(VL vl,pointer p)
{
	if ( !p ) {
		total_length++;
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

void PRINTN(N n)
{
	double ceil();

	if ( !n )
		PUTS("0");
	else if ( hex_output )
		total_length += 2+(int)(PL(n)*8);
	else
		total_length += (int)(ceil(0.31*((double)(BSH*PL(n))))+1);
}

void PRINTNUM(Num q)
{
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
			if ( double_output )
				total_length += 400+real_digit; /* XXX */
			else
				total_length += 20+real_digit; /* XXX */
			break;
		case N_A:
			PUTS("("); PRINTR(ALG,(R)BDY((Alg)q)); PUTS(")");
			break;
#if defined(PARI)
		case N_B:
			PRINTBF((BF)q); break;
#endif
		case N_C:
			PRINTCPLX((C)q); break;
		case N_M:
			total_length += 11; /* XXX */
			break;
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
			total_length += 13; /* XXX */
			break;
		case N_GFSN:
			PRINTUM(BDY((GFSN)q));
			break;
		default:
			break;
	}
}

void PRINTCPLX(C a)
{
	PUTS("(");
	if ( a->r )
		PRINTNUM(a->r);
	if ( a->i ) {
		if ( a->r && (compnum(0,a->i,0) > 0) )
			PUTS("+");
		PRINTNUM(a->i); PUTS("*@i");
	}
	PUTS(")");
}

void PRINTP(VL vl,P p)
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

extern int hideargs;

void PRINTV(VL vl,V v)
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
		} else if ( !pf->argc )
			PUTS(NAME(pf));
		else {
			if ( hideargs ) {
				for ( i = 0; i < pf->argc; i++ )
					if ( ad[i].d )
						break;
				if ( i < pf->argc ) {
					PUTS(NAME(pf));	
					total_length += 11; /* XXX */
					for ( i = 1; i < pf->argc; i++ ) {
						total_length += 11; /* XXX */
					}
					PUTS("}");
				} else {
					PUTS(NAME(pf));
					total_length += 1; /* XXX */
				}
			} else {
				for ( i = 0; i < pf->argc; i++ )
					if ( ad[i].d )
						break;
				if ( i < pf->argc ) {
					PUTS(NAME(pf));
					total_length += 11; /* XXX */
					for ( i = 1; i < pf->argc; i++ ) {
						total_length += 11; /* XXX */
					}
					PUTS(")(");
				} else {
					PUTS(NAME(pf));
					total_length += 1; /* XXX */
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

void PRINTR(VL vl,R a)
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

void PRINTVECT(VL vl,VECT vect)
{
	int i;
	pointer *ptr;

	PUTS("[ ");
	for ( i = 0, ptr = BDY(vect); i < vect->len; i++ ) {
		PRINTEXPR(vl,ptr[i]); PUTS(" ");
	}
	PUTS("]");
}

void PRINTMAT(VL vl,MAT mat)
{
	int i,j,r,c;
	pointer *ptr;

	for ( i = 0, r = mat->row, c = mat->col; i < r; i++ ) {
		PUTS("[ ");
		for ( j = 0, ptr = BDY(mat)[i]; j < c; j++ ) {
			PRINTEXPR(vl,ptr[j]); PUTS(" ");
		}
		PUTS("]");
		if ( i < r - 1 )
			PUTS("\n");
	}
}

void PRINTLIST(VL vl,LIST list)
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

void PRINTSTR(STRING str)
{
	char *p;

	for ( p = BDY(str); *p; p++ )
		if ( *p == '"' )
			PUTS("\"");
		else {
			total_length += 1;
		}
}

void PRINTCOMP(VL vl,COMP c)
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

void PRINTDP(VL vl,DP d)
{
	int n,i;
	MP m;
	DL dl;

	for ( n = d->nv, m = BDY(d); m; m = NEXT(m) ) {
		PUTS("("); PRINTEXPR(vl,(pointer)m->c); PUTS(")*<<");
		for ( i = 0, dl = m->dl; i < n-1; i++ ) {
			total_length += 11;
		}
		total_length += 10;
		PUTS(">>");
		if ( NEXT(m) )
			PUTS("+");
	}
}

void PRINTUI(VL vl,USINT u)
{
	total_length += 10;
}

void PRINTGF2MAT(VL vl,GF2MAT mat)
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

void PRINTGFMMAT(VL vl,GFMMAT mat)
{
	int row,col,i,j;
	unsigned int **b;

	row = mat->row;
	col = mat->col;
	b = mat->body;
	for ( i = 0; i < row; i++ ) {
		PUTS("[");
		for ( j = 0; j < col; j++ ) {
			total_length += 10; /* XXX */
		}
		PUTS("]\n");
	}
}

void PRINTBYTEARRAY(VL vl,BYTEARRAY array)
{
	/* |xx xx ... xx| */
	total_length += 1+3*array->len;
}

extern int print_quote;

void PRINTQUOTE(VL vl,QUOTE quote)
{
	LIST list;

	if ( print_quote == 2 ) {
		PRINTFNODE(BDY(quote));
	} else if ( print_quote == 1 ) {
		/* XXX */
		fnodetotree(BDY(quote),&list);
		PRINTEXPR(vl,(Obj)list);
	} else {
		/* <...quoted...> */
		total_length += 20;
	}
}

void PRINTSYMBOL(SYMBOL sym)
{
	total_length += strlen(sym->name);
}

void PRINTRANGE(VL vl,RANGE r)
{
	PUTS("range("); PRINTEXPR(vl,r->start);
	PUTS(","); PRINTEXPR(vl,r->end); PUTS(")");
}

void PRINTTB(VL vl,TB p)
{
	int i;

	for ( i = 0; i < p->next; i++ ) {
		total_length += strlen(p->body[i]);
	}
}

void PRINTERR(VL vl,ERR e)
{
	PUTS("error("); PRINTEXPR(vl,e->body); PUTS(")");
}

void PRINTUP2(UP2 p)
{
	int d,i;

	if ( !p ) {
		PUTS("0");
	} else {
		d = degup2(p);
		PUTS("(");
		if ( !d ) {
			PUTS("1");
		} else if ( d == 1 ) {
			PUTS("@");
		} else {
			PRINTHAT;
			total_length += 11;
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( p->b[i/BSH] & (1<<(i%BSH)) )
				if ( !i ) {
					PUTS("+1");
				} else if ( i == 1 ) {
					PUTS("+@");
				} else {
					PRINTHAT;
					total_length += 12;
				}
		}
		PUTS(")");
	}
}

void PRINTLF(VL vl,F f)
{
	switch ( FOP(f) ) {
		case AL_TRUE:
			PUTS("@true");
			break;
		case AL_FALSE:
			PUTS("@false");
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

void PRINTFOP(VL vl,F f)
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

void PRINTEOP(VL vl,F f)
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

void PRINTLOP(VL vl,F f)
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

void PRINTQOP(VL vl,F f)
{
	char *op;

	op = FOP(f)==AL_EX?"ex":"all";
	PUTS(op); PUTS(NAME(FQVR(f)));
	total_length += 2;
	PRINTEXPR(vl,(Obj)FQMAT(f)); PUTS(")");
}

void PRINTUP(UP n)
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
			PRINTHAT;
			total_length += 13;
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( n->c[i] ) {
				PUTS("+("); PRINTNUM(n->c[i]); PUTS(")");
				if ( i >= 2 ) {
					PRINTHAT;
					total_length += 13;
				} else if ( i == 1 )
					PUTS("*@p");
			}
		}
		PUTS(")");
	}
}

void PRINTUM(UM n)
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
			PUTS("*@s"); PRINTHAT; total_length += 13;
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( n->c[i] ) {
				PUTS("+("); PRINTSF(n->c[i]); PUTS(")");
				if ( i >= 2 ) {
					PUTS("*@s"); PRINTHAT; total_length += 13;
				} else if ( i == 1 )
					PUTS("*@s");
			}
		}
		PUTS(")");
	}
}

void PRINTSF(unsigned int i)
{
	if ( !i ) {
		PUTS("0");
	} else
		total_length += 15;
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
		case I_NOT: PRINTFNODE((FNODE)FA0(f),1); break;
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
				error("length_fnode : not implemented yet");
			GETPVNAME(FA0(f),opname);
			PUTS(opname);
			break;
		default: error("length_fnode : not implemented yet");
	}
	if ( paren ) PUTS(")");
}
