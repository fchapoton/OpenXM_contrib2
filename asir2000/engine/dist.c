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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/dist.c,v 1.43 2007/09/17 12:47:45 noro Exp $ 
*/
#include "ca.h"

#define ORD_REVGRADLEX 0
#define ORD_GRADLEX 1
#define ORD_LEX 2
#define ORD_BREVGRADLEX 3
#define ORD_BGRADLEX 4
#define ORD_BLEX 5
#define ORD_BREVREV 6
#define ORD_BGRADREV 7
#define ORD_BLEXREV 8
#define ORD_ELIM 9
#define ORD_WEYL_ELIM 10
#define ORD_HOMO_WW_DRL 11
#define ORD_DRL_ZIGZAG 12
#define ORD_HOMO_WW_DRL_ZIGZAG 13

int cmpdl_drl_zigzag(), cmpdl_homo_ww_drl_zigzag();
int cmpdl_top_weight();

int (*cmpdl)()=cmpdl_revgradlex;
int (*cmpdl_tie_breaker)();
int (*primitive_cmpdl[3])() = {cmpdl_revgradlex,cmpdl_gradlex,cmpdl_lex};

int do_weyl;

int dp_nelim,dp_fcoeffs;
struct order_spec *dp_current_spec;
struct modorder_spec *dp_current_modspec;
int *dp_dl_work;

void comm_muld_trunc(VL vl,DP p1,DP p2,DL dl,DP *pr);
void comm_quod(VL vl,DP p1,DP p2,DP *pr);
void muldm_trunc(VL vl,DP p,MP m0,DL dl,DP *pr);
void muldc_trunc(VL vl,DP p,P c,DL dl,DP *pr);

void order_init()
{
	struct order_spec *spec;

	create_order_spec(0,0,&spec);
	initd(spec);
	create_modorder_spec(0,0,&dp_current_modspec);
}

int has_sfcoef(DP f)
{
	MP t;

	if ( !f )
		return 0;
	for ( t = BDY(f); t; t = NEXT(t) )
		if ( has_sfcoef_p(t->c) )
			break;
	return t ? 1 : 0;
}

int has_sfcoef_p(P f)
{
	DCP dc;

	if ( !f )
		return 0;
	else if ( NUM(f) )
		return (NID((Num)f) == N_GFS) ? 1 : 0;
	else {
		for ( dc = DC(f); dc; dc = NEXT(dc) )
			if ( has_sfcoef_p(COEF(dc)) )
				return 1;
		return 0;
	}
}

extern N *current_top_weight_vector;
static int current_top_weight_len;

void initd(struct order_spec *spec)
{
	int len,i;

	switch ( spec->id ) {
		case 3:
			cmpdl = cmpdl_composite;
			dp_dl_work = (int *)MALLOC_ATOMIC(spec->nv*sizeof(int));	
			break;
		case 2:
			cmpdl = cmpdl_matrix;
			dp_dl_work = (int *)MALLOC_ATOMIC(spec->nv*sizeof(int));	
			break;
		case 1:
			cmpdl = cmpdl_order_pair;
			break;
		default:
			switch ( spec->ord.simple ) {
				case ORD_REVGRADLEX:
					cmpdl = cmpdl_revgradlex; break;
				case ORD_GRADLEX:
					cmpdl = cmpdl_gradlex; break;
				case ORD_BREVGRADLEX:
					cmpdl = cmpdl_brevgradlex; break;
				case ORD_BGRADLEX:
					cmpdl = cmpdl_bgradlex; break;
				case ORD_BLEX:
					cmpdl = cmpdl_blex; break;
				case ORD_BREVREV:
					cmpdl = cmpdl_brevrev; break;
				case ORD_BGRADREV:
					cmpdl = cmpdl_bgradrev; break;
				case ORD_BLEXREV:
					cmpdl = cmpdl_blexrev; break;
				case ORD_ELIM:
					cmpdl = cmpdl_elim; break;
				case ORD_WEYL_ELIM:
					cmpdl = cmpdl_weyl_elim; break;
				case ORD_HOMO_WW_DRL:
					cmpdl = cmpdl_homo_ww_drl; break;
				case ORD_DRL_ZIGZAG:
					cmpdl = cmpdl_drl_zigzag; break;
				case ORD_HOMO_WW_DRL_ZIGZAG:
					cmpdl = cmpdl_homo_ww_drl_zigzag; break;
				case ORD_LEX: default:
					cmpdl = cmpdl_lex; break;
			}
			break;
	}
	if ( current_top_weight_vector ) {
		cmpdl_tie_breaker = cmpdl;
		cmpdl = cmpdl_top_weight;
		for ( len = 0, i = 0; i < spec->nv; i++ )
			if ( current_top_weight_vector[i] )
				len = MAX(PL(current_top_weight_vector[i]),len);
		current_top_weight_len = len;
	}
	dp_current_spec = spec;
}

void ptod(VL vl,VL dvl,P p,DP *pr)
{
	int isconst = 0;
	int n,i,j,k;
	VL tvl;
	V v;
	DL d;
	MP m;
	DCP dc;
	DCP *w;
	DP r,s,t,u;
	P x,c;

	if ( !p )
		*pr = 0;
	else {
		for ( n = 0, tvl = dvl; tvl; tvl = NEXT(tvl), n++ );
		if ( NUM(p) ) {
			NEWDL(d,n);
			NEWMP(m); m->dl = d; C(m) = p; NEXT(m) = 0; MKDP(n,m,*pr); (*pr)->sugar = 0;
		} else {
			for ( i = 0, tvl = dvl, v = VR(p);
				tvl && tvl->v != v; tvl = NEXT(tvl), i++ );
			if ( !tvl ) {
				for ( dc = DC(p), k = 0; dc; dc = NEXT(dc), k++ );
				w = (DCP *)ALLOCA(k*sizeof(DCP));
				for ( dc = DC(p), j = 0; j < k; dc = NEXT(dc), j++ )
					w[j] = dc;

				for ( j = k-1, s = 0, MKV(v,x); j >= 0; j-- ) {
					ptod(vl,dvl,COEF(w[j]),&t); pwrp(vl,x,DEG(w[j]),&c);
					muldc(vl,t,c,&r); addd(vl,r,s,&t); s = t;
				}
				*pr = s;
			} else {
				for ( dc = DC(p), k = 0; dc; dc = NEXT(dc), k++ );
				w = (DCP *)ALLOCA(k*sizeof(DCP));
				for ( dc = DC(p), j = 0; j < k; dc = NEXT(dc), j++ )
					w[j] = dc;

				for ( j = k-1, s = 0; j >= 0; j-- ) {
					ptod(vl,dvl,COEF(w[j]),&t);
					NEWDL(d,n); d->d[i] = QTOS(DEG(w[j]));
					d->td = MUL_WEIGHT(d->d[i],i);
					NEWMP(m); m->dl = d; C(m) = (P)ONE; NEXT(m) = 0; MKDP(n,m,u); u->sugar = d->td;
					comm_muld(vl,t,u,&r); addd(vl,r,s,&t); s = t;
				}
				*pr = s;
			}
		}
	}
#if 0
	if ( !dp_fcoeffs && has_sfcoef(*pr) )
		dp_fcoeffs = N_GFS;
#endif
}

void dtop(VL vl,VL dvl,DP p,P *pr)
{
	int n,i,j,k;
	DL d;
	MP m;
	MP *a;
	P r,s,t,u,w;
	Q q;
	VL tvl;

	if ( !p )
		*pr = 0;
	else {
		for ( k = 0, m = BDY(p); m; m = NEXT(m), k++ );
		a = (MP *)ALLOCA(k*sizeof(MP));
		for ( j = 0, m = BDY(p); j < k; m = NEXT(m), j++ )
			a[j] = m;

		for ( n = p->nv, j = k-1, s = 0; j >= 0; j-- ) {
			m = a[j];
			t = C(m);
			if ( NUM(t) && NID((Num)t) == N_M ) {
				mptop(t,&u); t = u;
			}
			for ( i = 0, d = m->dl, tvl = dvl;
				i < n; tvl = NEXT(tvl), i++ ) {
				MKV(tvl->v,r); STOQ(d->d[i],q); pwrp(vl,r,q,&u);
				mulp(vl,t,u,&w); t = w;
			}
			addp(vl,s,t,&u); s = u;
		}
		*pr = s;
	}
}

void nodetod(NODE node,DP *dp)
{
	NODE t;
	int len,i,td;
	Q e;
	DL d;
	MP m;
	DP u;

	for ( t = node, len = 0; t; t = NEXT(t), len++ );
	NEWDL(d,len);
	for ( t = node, i = 0, td = 0; i < len; t = NEXT(t), i++ ) {
		e = (Q)BDY(t);
		if ( !e )
			d->d[i] = 0;
		else if ( !NUM(e) || !RATN(e) || !INT(e) )
			error("nodetod : invalid input");
		else {
			d->d[i] = QTOS((Q)e); td += MUL_WEIGHT(d->d[i],i);
		}
	}
	d->td = td;
	NEWMP(m); m->dl = d; C(m) = (P)ONE; NEXT(m) = 0;
	MKDP(len,m,u); u->sugar = td; *dp = u;
}

int sugard(MP m)
{
	int s;

	for ( s = 0; m; m = NEXT(m) )
		s = MAX(s,m->dl->td);
	return s;
}

void addd(VL vl,DP p1,DP p2,DP *pr)
{
	int n;
	MP m1,m2,mr,mr0;
	P t;
	DL d;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		if ( OID(p1) <= O_R ) {
			n = NV(p2);	NEWDL(d,n);
			NEWMP(m1); m1->dl = d; C(m1) = (P)p1; NEXT(m1) = 0;
			MKDP(n,m1,p1); (p1)->sugar = 0;
		}
		if ( OID(p2) <= O_R ) {
			n = NV(p1);	NEWDL(d,n);
			NEWMP(m2); m2->dl = d; C(m2) = (P)p2; NEXT(m2) = 0;
			MKDP(n,m2,p2); (p2)->sugar = 0;
		}
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					addp(vl,C(m1),C(m2),&t);
					if ( t ) {
						NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = t;
					}
					m1 = NEXT(m1); m2 = NEXT(m2); break;
				case 1:
					NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = C(m1);
					m1 = NEXT(m1); break;
				case -1:
					NEXTMP(mr0,mr); mr->dl = m2->dl; C(mr) = C(m2);
					m2 = NEXT(m2); break;
			}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		MKDP(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

/* for F4 symbolic reduction */

void symb_addd(DP p1,DP p2,DP *pr)
{
	int n;
	MP m1,m2,mr,mr0;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			NEXTMP(mr0,mr); C(mr) = (P)ONE;
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					mr->dl = m1->dl;
					m1 = NEXT(m1); m2 = NEXT(m2); break;
				case 1:
					mr->dl = m1->dl;
					m1 = NEXT(m1); break;
				case -1:
					mr->dl = m2->dl;
					m2 = NEXT(m2); break;
			}
		}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		MKDP(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

/* 
 * destructive merge of two list
 *
 * p1, p2 : list of DL
 * return : a merged list
 */

NODE symb_merge(NODE m1,NODE m2,int n)
{
	NODE top,prev,cur,m,t;
	int c,i;
	DL d1,d2;

	if ( !m1 )
		return m2;
	else if ( !m2 )
		return m1;
	else {
		switch ( (*cmpdl)(n,(DL)BDY(m1),(DL)BDY(m2)) ) {
			case 0:
				top = m1; m = NEXT(m2);
				break;
			case 1:
				top = m1; m = m2;
				break;
			case -1:
				top = m2; m = m1;
				break;
		}
		prev = top; cur = NEXT(top);
		/* BDY(prev) > BDY(m) always holds */
		while ( cur && m ) {
			d1 = (DL)BDY(cur);
			d2 = (DL)BDY(m);
#if 1
			switch ( (*cmpdl)(n,(DL)BDY(cur),(DL)BDY(m)) ) {
#else
			/* XXX only valid for DRL */
			if ( d1->td > d2->td )
				c = 1;
			else if ( d1->td < d2->td )
				c = -1;
			else {
				for ( i = n-1; i >= 0 && d1->d[i] == d2->d[i]; i-- );
				if ( i < 0 )
					c = 0;
				else if ( d1->d[i] < d2->d[i] )
					c = 1;
				else
					c = -1;
			}
			switch ( c ) {
#endif
				case 0:
					m = NEXT(m);
					prev = cur; cur = NEXT(cur);
					break;
				case 1:
					t = NEXT(cur); NEXT(cur) = m; m = t;
					prev = cur; cur = NEXT(cur);
					break;
				case -1:
					NEXT(prev) = m; m = cur;
					prev = NEXT(prev); cur = NEXT(prev);
					break;
			}
		}
		if ( !cur )
			NEXT(prev) = m;
		return top;
	}
}

void _adddl(int n,DL d1,DL d2,DL d3)
{
	int i;

	d3->td = d1->td+d2->td;
	for ( i = 0; i < n; i++ )
		d3->d[i] = d1->d[i]+d2->d[i];
}

/* m1 <- m1 U dl*f, destructive */

NODE mul_dllist(DL dl,DP f);

NODE symb_mul_merge(NODE m1,DL dl,DP f,int n)
{
	NODE top,prev,cur,n1;
	DP g;
	DL t,s;
	MP m;

	if ( !m1 )
		return mul_dllist(dl,f);
	else if ( !f )
		return m1;
	else {
		m = BDY(f);
		NEWDL_NOINIT(t,n);
		_adddl(n,m->dl,dl,t);
		top = m1; prev = 0; cur = m1;
		while ( m ) {
			switch ( (*cmpdl)(n,(DL)BDY(cur),t) ) {
				case 0:
					prev = cur; cur = NEXT(cur);
					if ( !cur ) {
						MKDP(n,m,g);
						NEXT(prev) = mul_dllist(dl,g);
						return;
					}
					m = NEXT(m);
					if ( m ) _adddl(n,m->dl,dl,t);
					break;
				case 1:
					prev = cur; cur = NEXT(cur); 
					if ( !cur ) {
						MKDP(n,m,g);
						NEXT(prev) = mul_dllist(dl,g);
						return;
					}
					break;
				case -1:
					NEWDL_NOINIT(s,n);
					s->td = t->td;
					bcopy(t->d,s->d,n*sizeof(int));
					NEWNODE(n1);
					n1->body = (pointer)s;
					NEXT(n1) = cur;
					if ( !prev ) {
						top = n1; cur = n1;
					} else {
						NEXT(prev) = n1; prev = n1;
					}
					m = NEXT(m);
					if ( m ) _adddl(n,m->dl,dl,t);
					break;
			}
		}
		return top;
	}
}

DLBUCKET symb_merge_bucket(DLBUCKET m1,DLBUCKET m2,int n)
{
	DLBUCKET top,prev,cur,m,t;

	if ( !m1 )
		return m2;
	else if ( !m2 )
		return m1;
	else {
		if ( m1->td == m2->td ) {
			top = m1;
			BDY(top) = symb_merge(BDY(top),BDY(m2),n);
			m = NEXT(m2);
		} else if ( m1->td > m2->td ) {
			top = m1; m = m2;
		} else {
			top = m2; m = m1;
		}
		prev = top; cur = NEXT(top);
		/* prev->td > m->td always holds */
		while ( cur && m ) {
			if ( cur->td == m->td ) {
				BDY(cur) = symb_merge(BDY(cur),BDY(m),n);
				m = NEXT(m);
				prev = cur; cur = NEXT(cur);
			} else if ( cur->td > m->td ) {
				t = NEXT(cur); NEXT(cur) = m; m = t;
				prev = cur; cur = NEXT(cur);
			} else {
				NEXT(prev) = m; m = cur;
				prev = NEXT(prev); cur = NEXT(prev);
			}
		}
		if ( !cur )
			NEXT(prev) = m;
		return top;
	}
}

void subd(VL vl,DP p1,DP p2,DP *pr)
{
	DP t;

	if ( !p2 )
		*pr = p1;
	else {
		chsgnd(p2,&t); addd(vl,p1,t,pr);
	}
}

void chsgnd(DP p,DP *pr)
{
	MP m,mr,mr0;
	Obj r;

	if ( !p )
		*pr = 0;
	else if ( OID(p) <= O_R ) {
		chsgnr((Obj)p,&r); *pr = (DP)r;
	} else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); chsgnp(C(m),&C(mr)); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void muld(VL vl,DP p1,DP p2,DP *pr)
{
	if ( ! do_weyl )
		comm_muld(vl,p1,p2,pr);
	else
		weyl_muld(vl,p1,p2,pr);
}

void comm_muld(VL vl,DP p1,DP p2,DP *pr)
{
	MP m;
	DP s,t,u;
	int i,l,l1;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else if ( OID(p1) <= O_P )
		muldc(vl,p2,(P)p1,pr);
	else if ( OID(p2) <= O_P )
		muldc(vl,p1,(P)p2,pr);
	else {
		for ( m = BDY(p1), l1 = 0; m; m = NEXT(m), l1++ );
		for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
		if ( l1 < l ) {
			t = p1; p1 = p2; p2 = t;
			l = l1;
		}
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			muldm(vl,p1,w[i],&t); addd(vl,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

/* discard terms which is not a multiple of dl */

void comm_muld_trunc(VL vl,DP p1,DP p2,DL dl,DP *pr)
{
	MP m;
	DP s,t,u;
	int i,l,l1;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else if ( OID(p1) <= O_P )
		muldc_trunc(vl,p2,(P)p1,dl,pr);
	else if ( OID(p2) <= O_P )
		muldc_trunc(vl,p1,(P)p2,dl,pr);
	else {
		for ( m = BDY(p1), l1 = 0; m; m = NEXT(m), l1++ );
		for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
		if ( l1 < l ) {
			t = p1; p1 = p2; p2 = t;
			l = l1;
		}
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			muldm_trunc(vl,p1,w[i],dl,&t); addd(vl,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

void comm_quod(VL vl,DP p1,DP p2,DP *pr)
{
	MP m,m0;
	DP s,t;
	int i,n,sugar;
	DL d1,d2,d;
	Q a,b;

	if ( !p2 )
		error("comm_quod : invalid input");
	if ( !p1 )
		*pr = 0;
	else {
		n = NV(p1);
		d2 = BDY(p2)->dl;
		m0 = 0;
		sugar = p1->sugar;
		while ( p1 ) {
			d1 = BDY(p1)->dl;
			NEWDL(d,n);
			d->td = d1->td - d2->td;
			for ( i = 0; i < n; i++ )
				d->d[i] = d1->d[i]-d2->d[i];
			NEXTMP(m0,m);
			m->dl = d;
			divq((Q)BDY(p1)->c,(Q)BDY(p2)->c,&a); chsgnq(a,&b);
			C(m) = (P)b;
			muldm_trunc(vl,p2,m,d2,&t);
			addd(vl,p1,t,&s); p1 = s;
			C(m) = (P)a;
		}
		if ( m0 ) {
			NEXT(m) = 0; MKDP(n,m0,*pr);
		} else
			*pr = 0;
		/* XXX */
		if ( *pr )
			(*pr)->sugar = sugar - d2->td;
	}
}

void muldm(VL vl,DP p,MP m0,DP *pr)
{
	MP m,mr,mr0;
	P c;
	DL d;
	int n;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p), c = C(m0), d = m0->dl, n = NV(p); 
			m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			if ( NUM(C(m)) && RATN(C(m)) && NUM(c) && RATN(c) )
				mulq((Q)C(m),(Q)c,(Q *)&C(mr));
			else
				mulp(vl,C(m),c,&C(mr));
			adddl(n,m->dl,d,&mr->dl);
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar + m0->dl->td;
	}
}

void muldm_trunc(VL vl,DP p,MP m0,DL dl,DP *pr)
{
	MP m,mr,mr0;
	P c;
	DL d,tdl;
	int n,i;

	if ( !p )
		*pr = 0;
	else {
		n = NV(p);
		NEWDL(tdl,n);
		for ( mr0 = 0, m = BDY(p), c = C(m0), d = m0->dl; 
			m; m = NEXT(m) ) {
			_adddl(n,m->dl,d,tdl);
			for ( i = 0; i < n; i++ )
				if ( tdl->d[i] < dl->d[i] )
					break;
			if ( i < n )
				continue;
			NEXTMP(mr0,mr);
			mr->dl = tdl;
			NEWDL(tdl,n);
			if ( NUM(C(m)) && RATN(C(m)) && NUM(c) && RATN(c) )
				mulq((Q)C(m),(Q)c,(Q *)&C(mr));
			else
				mulp(vl,C(m),c,&C(mr));
		}
		if ( mr0 ) {
			NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		} else
			*pr = 0;
		if ( *pr )
			(*pr)->sugar = p->sugar + m0->dl->td;
	}
}

void weyl_muld(VL vl,DP p1,DP p2,DP *pr)
{
	MP m;
	DP s,t,u;
	int i,l;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else if ( OID(p1) <= O_P )
		muldc(vl,p2,(P)p1,pr);
	else if ( OID(p2) <= O_P )
		muldc(vl,p1,(P)p2,pr);
	else {
		for ( m = BDY(p1), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p1), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			weyl_muldm(vl,w[i],p2,&t); addd(vl,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

/* monomial * polynomial */

void weyl_muldm(VL vl,MP m0,DP p,DP *pr)
{
	DP r,t,t1;
	MP m;
	DL d0;
	int n,n2,l,i,j,tlen;
	static MP *w,*psum;
	static struct cdl *tab;
	static int wlen;
	static int rtlen;

	if ( !p )
		*pr = 0;
	else {
		for ( m = BDY(p), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;

		n = NV(p); n2 = n>>1;
		d0 = m0->dl;
		for ( i = 0, tlen = 1; i < n2; i++ )
			tlen *= d0->d[n2+i]+1;
		if ( tlen > rtlen ) {
			if ( tab ) GC_free(tab);
			if ( psum ) GC_free(psum);
			rtlen = tlen;
			tab = (struct cdl *)MALLOC(rtlen*sizeof(struct cdl));
			psum = (MP *)MALLOC(rtlen*sizeof(MP));
		}
		bzero(psum,tlen*sizeof(MP));
		for ( i = l-1; i >= 0; i-- ) {
			bzero(tab,tlen*sizeof(struct cdl));
			weyl_mulmm(vl,m0,w[i],n,tab,tlen);
			for ( j = 0; j < tlen; j++ ) {
				if ( tab[j].c ) {
					NEWMP(m); m->dl = tab[j].d; C(m) = tab[j].c; NEXT(m) = psum[j];
					psum[j] = m;
				}
			}
		}
		for ( j = tlen-1, r = 0; j >= 0; j-- ) 
			if ( psum[j] ) {
				MKDP(n,psum[j],t); addd(vl,r,t,&t1); r = t1;
			}
		if ( r )
			r->sugar = p->sugar + m0->dl->td;
		*pr = r;
	}
}

/* m0 = x0^d0*x1^d1*... * dx0^e0*dx1^e1*... */
/* rtab : array of length (e0+1)*(e1+1)*... */

void weyl_mulmm(VL vl,MP m0,MP m1,int n,struct cdl *rtab,int rtablen)
{
	P c,c0,c1;
	DL d,d0,d1,dt;
	int i,j,a,b,k,l,n2,s,min,curlen;
	struct cdl *p;
	static Q *ctab;
	static struct cdl *tab;
	static int tablen;
	static struct cdl *tmptab;
	static int tmptablen;

	
	if ( !m0 || !m1 ) {
		rtab[0].c = 0;
		rtab[0].d = 0;
		return;
	}
	c0 = C(m0); c1 = C(m1);
	mulp(vl,c0,c1,&c);
	d0 = m0->dl; d1 = m1->dl;
	n2 = n>>1;
	curlen = 1;
	NEWDL(d,n);
	if ( n & 1 )
		/* offset of h-degree */
	 	d->td = d->d[n-1] = d0->d[n-1]+d1->d[n-1];
	else
		d->td = 0;
	rtab[0].c = c;
	rtab[0].d = d;

	if ( rtablen > tmptablen ) {
		if ( tmptab ) GC_free(tmptab);
		tmptab = (struct cdl *)MALLOC(rtablen*sizeof(struct cdl));
		tmptablen = rtablen;
	}
	for ( i = 0; i < n2; i++ ) {
		a = d0->d[i]; b = d1->d[n2+i];
		k = d0->d[n2+i]; l = d1->d[i];

		/* degree of xi^a*(Di^k*xi^l)*Di^b */
		a += l;
		b += k;
		s = MUL_WEIGHT(a,i)+MUL_WEIGHT(b,n2+i);

		if ( !k || !l ) {
			for ( j = 0, p = rtab; j < curlen; j++, p++ ) {
				if ( p->c ) {
					dt = p->d;
					dt->d[i] = a;
					dt->d[n2+i] = b;
					dt->td += s;
				}
			}
			curlen *= k+1;
			continue;
		}
		if ( k+1 > tablen ) {
			if ( tab ) GC_free(tab);
			if ( ctab ) GC_free(ctab);
			tablen = k+1;
			tab = (struct cdl *)MALLOC(tablen*sizeof(struct cdl));
			ctab = (Q *)MALLOC(tablen*sizeof(Q));
		}
		/* compute xi^a*(Di^k*xi^l)*Di^b */
		min = MIN(k,l);
		mkwc(k,l,ctab);
		bzero(tab,(k+1)*sizeof(struct cdl));
		if ( n & 1 )
			for ( j = 0; j <= min; j++ ) {
				NEWDL(d,n);
				d->d[i] = a-j; d->d[n2+i] = b-j;
				d->td = s;
				d->d[n-1] = s-(MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i));
				tab[j].d = d;
				tab[j].c = (P)ctab[j];
			}
		else
			for ( j = 0; j <= min; j++ ) {
				NEWDL(d,n);
				d->d[i] = a-j; d->d[n2+i] = b-j;
				d->td = MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i); /* XXX */
				tab[j].d = d;
				tab[j].c = (P)ctab[j];
			}
		bzero(ctab,(min+1)*sizeof(Q));
		comm_muld_tab(vl,n,rtab,curlen,tab,k+1,tmptab);
		curlen *= k+1;
		bcopy(tmptab,rtab,curlen*sizeof(struct cdl));
	}
}

/* direct product of two cdl tables
  rt[] = [
    t[0]*t1[0],...,t[n-1]*t1[0],
    t[0]*t1[1],...,t[n-1]*t1[1],
    ...
    t[0]*t1[n1-1],...,t[n-1]*t1[n1-1]
  ]
*/

void comm_muld_tab(VL vl,int nv,struct cdl *t,int n,struct cdl *t1,int n1,struct cdl *rt)
{
	int i,j;
	struct cdl *p;
	P c;
	DL d;

	bzero(rt,n*n1*sizeof(struct cdl));
	for ( j = 0, p = rt; j < n1; j++ ) {
		c = t1[j].c;
		d = t1[j].d;
		if ( !c )
			break;
		for ( i = 0; i < n; i++, p++ ) {
			if ( t[i].c ) {
				mulp(vl,t[i].c,c,&p->c);
				adddl(nv,t[i].d,d,&p->d);
			}
		}
	}
}

void muldc(VL vl,DP p,P c,DP *pr)
{
	MP m,mr,mr0;

	if ( !p || !c )
		*pr = 0;
	else if ( NUM(c) && UNIQ((Q)c) )
		*pr = p;
	else if ( NUM(c) && MUNIQ((Q)c) )
		chsgnd(p,pr);
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			if ( NUM(C(m)) && RATN(C(m)) && NUM(c) && RATN(c) )
				mulq((Q)C(m),(Q)c,(Q *)&C(mr));
			else
				mulp(vl,C(m),c,&C(mr));
			mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void muldc_trunc(VL vl,DP p,P c,DL dl,DP *pr)
{
	MP m,mr,mr0;
	DL mdl;
	int i,n;

	if ( !p || !c ) {
		*pr = 0; return;
	}
	n = NV(p);
	for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
		mdl = m->dl;
		for ( i = 0; i < n; i++ )
			if ( mdl->d[i] < dl->d[i] )
				break;
		if ( i < n )
			break;
		NEXTMP(mr0,mr);
		if ( NUM(C(m)) && RATN(C(m)) && NUM(c) && RATN(c) )
			mulq((Q)C(m),(Q)c,(Q *)&C(mr));
		else
			mulp(vl,C(m),c,&C(mr));
		mr->dl = m->dl;
	}
	NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
	if ( *pr )
		(*pr)->sugar = p->sugar;
}

void divsdc(VL vl,DP p,P c,DP *pr)
{
	MP m,mr,mr0;

	if ( !c )
		error("disvsdc : division by 0");
	else if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); divsp(vl,C(m),c,&C(mr)); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void adddl(int n,DL d1,DL d2,DL *dr)
{
	DL dt;
	int i;

	*dr = dt = (DL)MALLOC_ATOMIC((n+1)*sizeof(int));
	dt->td = d1->td + d2->td;
	for ( i = 0; i < n; i++ )
		dt->d[i] = d1->d[i]+d2->d[i];
}

/* d1 += d2 */

void adddl_destructive(int n,DL d1,DL d2)
{
	int i;

	d1->td += d2->td;
	for ( i = 0; i < n; i++ )
		d1->d[i] += d2->d[i];
}

int compd(VL vl,DP p1,DP p2)
{
	int n,t;
	MP m1,m2;

	if ( !p1 )
		return p2 ? -1 : 0;
	else if ( !p2 )
		return 1;
	else if ( NV(p1) != NV(p2) )
		error("compd : size mismatch");
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2);
			m1 && m2; m1 = NEXT(m1), m2 = NEXT(m2) )
			if ( (t = (*cmpdl)(n,m1->dl,m2->dl)) ||
				(t = compp(vl,C(m1),C(m2)) ) )
				return t;
		if ( m1 )
			return 1;
		else if ( m2 )
			return -1;
		else
			return 0;
	}
}

int cmpdl_lex(int n,DL d1,DL d2)
{
	int i;

	for ( i = 0; i < n && d1->d[i] == d2->d[i]; i++ );
	return i == n ? 0 : (d1->d[i] > d2->d[i] ? 1 : -1);
}

int cmpdl_revlex(int n,DL d1,DL d2)
{
	int i;

	for ( i = n - 1; i >= 0 && d1->d[i] == d2->d[i]; i-- );
	return i < 0 ? 0 : (d1->d[i] < d2->d[i] ? 1 : -1);
}

int cmpdl_gradlex(int n,DL d1,DL d2)
{
	if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;
	else
		return cmpdl_lex(n,d1,d2);
}

int cmpdl_revgradlex(int n,DL d1,DL d2)
{
	register int i,c;
	register int *p1,*p2;

	if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;
	else {
		i = n-1;
		p1 = d1->d+n-1;
		p2 = d2->d+n-1;
		while ( i >= 7 ) {
			c = (*p1--) - (*p2--); if ( c ) goto LAST;
			c = (*p1--) - (*p2--); if ( c ) goto LAST;
			c = (*p1--) - (*p2--); if ( c ) goto LAST;
			c = (*p1--) - (*p2--); if ( c ) goto LAST;
			c = (*p1--) - (*p2--); if ( c ) goto LAST;
			c = (*p1--) - (*p2--); if ( c ) goto LAST;
			c = (*p1--) - (*p2--); if ( c ) goto LAST;
			c = (*p1--) - (*p2--); if ( c ) goto LAST;
			i -= 8;
		}
		switch ( i ) {
			case 6:
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				return 0;
			case 5:
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				return 0;
			case 4:
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				return 0;
			case 3:
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				return 0;
			case 2:
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				return 0;
			case 1:
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				return 0;
			case 0:
				c = (*p1--) - (*p2--); if ( c ) goto LAST;
				return 0;
			default:
				return 0;
		}
LAST:
		if ( c > 0 ) return -1;
		else return 1;
	}
}

int cmpdl_blex(int n,DL d1,DL d2)
{
	int c;

	if ( c = cmpdl_lex(n-1,d1,d2) )
		return c;
	else {
		c = d1->d[n-1] - d2->d[n-1];
		return c > 0 ? 1 : c < 0 ? -1 : 0;
	}
}

int cmpdl_bgradlex(int n,DL d1,DL d2)
{
	int e1,e2,c;

	e1 = d1->td - d1->d[n-1]; e2 = d2->td - d2->d[n-1];
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else {
		c = cmpdl_lex(n-1,d1,d2);
		if ( c )
			return c;
		else
			return d1->td > d2->td ? 1 : d1->td < d2->td ? -1 : 0;
	}
}

int cmpdl_brevgradlex(int n,DL d1,DL d2)
{
	int e1,e2,c;

	e1 = d1->td - d1->d[n-1]; e2 = d2->td - d2->d[n-1];
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else {
		c = cmpdl_revlex(n-1,d1,d2);
		if ( c )
			return c;
		else
			return d1->td > d2->td ? 1 : d1->td < d2->td ? -1 : 0;
	}
}

int cmpdl_brevrev(int n,DL d1,DL d2)
{
	int e1,e2,f1,f2,c,i;

	for ( i = 0, e1 = 0, e2 = 0; i < dp_nelim; i++ ) {
		e1 += d1->d[i]; e2 += d2->d[i];
	}
	f1 = d1->td - e1; f2 = d2->td - e2;
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else {
		c = cmpdl_revlex(dp_nelim,d1,d2);
		if ( c )
			return c;
		else if ( f1 > f2 )
			return 1;
		else if ( f1 < f2 )
			return -1;
		else {
			for ( i = n - 1; i >= dp_nelim && d1->d[i] == d2->d[i]; i-- );
			return i < dp_nelim ? 0 : (d1->d[i] < d2->d[i] ? 1 : -1);
		}
	}
}

int cmpdl_bgradrev(int n,DL d1,DL d2)
{
	int e1,e2,f1,f2,c,i;

	for ( i = 0, e1 = 0, e2 = 0; i < dp_nelim; i++ ) {
		e1 += d1->d[i]; e2 += d2->d[i];
	}
	f1 = d1->td - e1; f2 = d2->td - e2;
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else {
		c = cmpdl_lex(dp_nelim,d1,d2);
		if ( c )
			return c;
		else if ( f1 > f2 )
			return 1;
		else if ( f1 < f2 )
			return -1;
		else {
			for ( i = n - 1; i >= dp_nelim && d1->d[i] == d2->d[i]; i-- );
			return i < dp_nelim ? 0 : (d1->d[i] < d2->d[i] ? 1 : -1);
		}
	}
}

int cmpdl_blexrev(int n,DL d1,DL d2)
{
	int e1,e2,f1,f2,c,i;

	for ( i = 0, e1 = 0, e2 = 0; i < dp_nelim; i++ ) {
		e1 += d1->d[i]; e2 += d2->d[i];
	}
	f1 = d1->td - e1; f2 = d2->td - e2;
	c = cmpdl_lex(dp_nelim,d1,d2);
	if ( c )
		return c;
	else if ( f1 > f2 )
		return 1;
	else if ( f1 < f2 )
		return -1;
	else {
		for ( i = n - 1; i >= dp_nelim && d1->d[i] == d2->d[i]; i-- );
		return i < dp_nelim ? 0 : (d1->d[i] < d2->d[i] ? 1 : -1);
	}
}

int cmpdl_elim(int n,DL d1,DL d2)
{
	int e1,e2,i;

	for ( i = 0, e1 = 0, e2 = 0; i < dp_nelim; i++ ) {
		e1 += d1->d[i]; e2 += d2->d[i];
	}
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else 
		return cmpdl_revgradlex(n,d1,d2);
}

int cmpdl_weyl_elim(int n,DL d1,DL d2)
{
	int e1,e2,i;

	for ( i = 1, e1 = 0, e2 = 0; i <= dp_nelim; i++ ) {
		e1 += d1->d[n-i]; e2 += d2->d[n-i];
	}
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;
	else return -cmpdl_revlex(n,d1,d2);
}

/*
	a special ordering
	1. total order
	2. (-w,w) for the first 2*m variables
	3. DRL for the first 2*m variables
*/

extern int *current_weyl_weight_vector;

int cmpdl_homo_ww_drl(int n,DL d1,DL d2)
{
	int e1,e2,m,i;
	int *p1,*p2;

	if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;

	m = n>>1;
	for ( i = 0, e1 = e2 = 0, p1 = d1->d, p2 = d2->d; i < m; i++ ) {
		e1 += current_weyl_weight_vector[i]*(p1[m+i] - p1[i]);
		e2 += current_weyl_weight_vector[i]*(p2[m+i] - p2[i]);
	}
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;

	e1 = d1->td - d1->d[n-1];
	e2 = d2->td - d2->d[n-1];
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;

	for ( i= n - 1, p1 = d1->d+n-1, p2 = d2->d+n-1;
		i >= 0 && *p1 == *p2; i--, p1--, p2-- );
	return i < 0 ? 0 : (*p1 < *p2 ? 1 : -1);
}

int cmpdl_drl_zigzag(int n,DL d1,DL d2)
{
	int i,t,m;
	int *p1,*p2;

	if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;
	else {
		m = n>>1;
		for ( i= m - 1, p1 = d1->d, p2 = d2->d; i >= 0; i-- ) {
			if ( t = p1[m+i] - p2[m+i] ) return t > 0 ? -1 : 1;
			if ( t = p1[i] - p2[i] ) return t > 0 ? -1 : 1;
		}
		return 0;
	}
}

int cmpdl_homo_ww_drl_zigzag(int n,DL d1,DL d2)
{
	int e1,e2,m,i,t;
	int *p1,*p2;

	if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;

	m = n>>1;
	for ( i = 0, e1 = e2 = 0, p1 = d1->d, p2 = d2->d; i < m; i++ ) {
		e1 += current_weyl_weight_vector[i]*(p1[m+i] - p1[i]);
		e2 += current_weyl_weight_vector[i]*(p2[m+i] - p2[i]);
	}
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;

	e1 = d1->td - d1->d[n-1];
	e2 = d2->td - d2->d[n-1];
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;

	for ( i= m - 1, p1 = d1->d, p2 = d2->d; i >= 0; i-- ) {
		if ( t = p1[m+i] - p2[m+i] ) return t > 0 ? -1 : 1;
		if ( t = p1[i] - p2[i] ) return t > 0 ? -1 : 1;
	}
	return 0;
}

int cmpdl_order_pair(int n,DL d1,DL d2)
{
	int e1,e2,i,j,l;
	int *t1,*t2;
	int len,head;
	struct order_pair *pair;

	len = dp_current_spec->ord.block.length;
	if ( n != dp_current_spec->nv )
		error("cmpdl_order_pair : incompatible order specification");
	pair = dp_current_spec->ord.block.order_pair;

	head = 0;
	for ( i = 0, t1 = d1->d, t2 = d2->d; i < len; i++ ) {
		l = pair[i].length;
		switch ( pair[i].order ) {
			case 0:
				for ( j = 0, e1 = e2 = 0; j < l; j++ ) {
					e1 += MUL_WEIGHT(t1[j],head+j);
					e2 += MUL_WEIGHT(t2[j],head+j);
				}
				if ( e1 > e2 )
					return 1;
				else if ( e1 < e2 )
					return -1;
				else {
					for ( j = l - 1; j >= 0 && t1[j] == t2[j]; j-- );
					if ( j >= 0 )
						return t1[j] < t2[j] ? 1 : -1;
				}
				break;
			case 1:
				for ( j = 0, e1 = e2 = 0; j < l; j++ ) {
					e1 += MUL_WEIGHT(t1[j],head+j);
					e2 += MUL_WEIGHT(t2[j],head+j);
				}
				if ( e1 > e2 )
					return 1;
				else if ( e1 < e2 )
					return -1;
				else {
					for ( j = 0; j < l && t1[j] == t2[j]; j++ );
					if ( j < l )
						return t1[j] > t2[j] ? 1 : -1;
				}
				break;
			case 2:
				for ( j = 0; j < l && t1[j] == t2[j]; j++ );
				if ( j < l )
					return t1[j] > t2[j] ? 1 : -1;
				break;
			default:
				error("cmpdl_order_pair : invalid order"); break;
		}
		t1 += l; t2 += l; head += l;
	}
	return 0;
}

int cmpdl_composite(int nv,DL d1,DL d2)
{
	int n,i,j,k,start,s,len;
	int *dw;
	struct sparse_weight *sw;
	struct weight_or_block *worb;
	int *w,*t1,*t2;

	n = dp_current_spec->ord.composite.length;
	worb = dp_current_spec->ord.composite.w_or_b;
	w = dp_dl_work;
	for ( i = 0, t1 = d1->d, t2 = d2->d; i < nv; i++ )
		w[i] = t1[i]-t2[i];
	for ( i = 0; i < n; i++, worb++ ) {
		len = worb->length;
		switch ( worb->type ) {
			case IS_DENSE_WEIGHT:
				dw = worb->body.dense_weight;
				for ( j = 0, s = 0; j < len; j++ )
					s += dw[j]*w[j];
				if ( s > 0 ) return 1;
				else if ( s < 0 ) return -1;
				break;
			case IS_SPARSE_WEIGHT:
				sw = worb->body.sparse_weight;
				for ( j = 0, s = 0; j < len; j++ )
					s += sw[j].value*w[sw[j].pos];
				if ( s > 0 ) return 1;
				else if ( s < 0 ) return -1;
				break;
			case IS_BLOCK:
				start = worb->body.block.start;
				switch ( worb->body.block.order ) {
					case 0:
						for ( j = 0, k = start, s = 0; j < len; j++, k++ ) {
							s += MUL_WEIGHT(w[k],k);
						}
						if ( s > 0 ) return 1;
						else if ( s < 0 ) return -1;
						else {
							for ( j = k-1; j >= start && w[j] == 0; j-- );
							if ( j >= start )
								return w[j] < 0 ? 1 : -1;
						}
						break;
					case 1:
						for ( j = 0, k = start, s = 0; j < len; j++, k++ ) {
							s += MUL_WEIGHT(w[k],k);
						}
						if ( s > 0 ) return 1;
						else if ( s < 0 ) return -1;
						else {
							for ( j = 0, k = start;  j < len && w[j] == 0; j++, k++ );
							if ( j < len )
								return w[j] > 0 ? 1 : -1;
						}
						break;
					case 2:
						for ( j = 0, k = start;  j < len && w[j] == 0; j++, k++ );
						if ( j < len )
							return w[j] > 0 ? 1 : -1;
						break;
				}
				break;
		}
	}
	return 0;
}

int cmpdl_matrix(int n,DL d1,DL d2)
{
	int *v,*w,*t1,*t2;
	int s,i,j,len;
	int **matrix;

	for ( i = 0, t1 = d1->d, t2 = d2->d, w = dp_dl_work; i < n; i++ )
		w[i] = t1[i]-t2[i];
	len = dp_current_spec->ord.matrix.row;
	matrix = dp_current_spec->ord.matrix.matrix;
	for ( j = 0; j < len; j++ ) {
		v = matrix[j];
		for ( i = 0, s = 0; i < n; i++ )
			s += v[i]*w[i];	
		if ( s > 0 )
			return 1;
		else if ( s < 0 )
			return -1;
	}
	return 0;
}

int cmpdl_top_weight(int n,DL d1,DL d2)
{
	int *w;
	N sum,wm,wma,t;
	N *a;
	struct oN tn;
	int len,i,sgn,tsgn;
	int *t1,*t2;

	w = (int *)ALLOCA(n*sizeof(int));
	len = current_top_weight_len+3;
	t1 = d1->d; t2 = d2->d;
	for ( i = 0; i < n; i++ ) w[i] = t1[i]-t2[i];
	sum = (N)W_ALLOC(len); sgn = 0;
	wm = (N)W_ALLOC(len);
	wma = (N)W_ALLOC(len);
	a = current_top_weight_vector;
	for ( i = 0; i < n; i++ ) {
		if ( !a[i] || !w[i] ) continue;
		tn.p = 1;
		if ( w[i] > 0 ) {
			tn.b[0] = w[i]; tsgn = 1;
		} else {
			tn.b[0] = -w[i]; tsgn = -1;
		}
		_muln(a[i],&tn,wm);
		if ( !sgn ) {
			sgn = tsgn;
			t = wm; wm = sum; sum = t;
		} else if ( sgn == tsgn ) {
			_addn(sum,wm,wma);
			if ( !PL(wma) )
				sgn = 0;
			t = wma; wma = sum; sum = t;
		} else {
			sgn *= _subn(sum,wm,wma);
			t = wma; wma = sum; sum = t;
		}
	}
	if ( sgn > 0 ) return 1;
	else if ( sgn < 0 ) return -1;
	else return (*cmpdl_tie_breaker)(n,d1,d2);
}

GeoBucket create_bucket()
{
	GeoBucket g;

	g = CALLOC(1,sizeof(struct oGeoBucket));
	g->m = 32;
	return g;
}

void add_bucket(GeoBucket g,NODE d,int nv)
{
	int l,k,m;

	l = length(d);
	for ( k = 0, m = 1; l > m; k++, m <<= 1 );
	/* 2^(k-1) < l <= 2^k */
	d = symb_merge(g->body[k],d,nv);
	for ( ; length(d) > (1<<(k)); k++ ) {
		g->body[k] = 0;
		d = symb_merge(g->body[k+1],d,nv);
	}
	g->body[k] = d;
	g->m = MAX(g->m,k);
}

DL remove_head_bucket(GeoBucket g,int nv)
{
	int j,i,c,m;
	DL d;

	j = -1;
	m = g->m;
	for ( i = 0; i <= m; i++ ) {
		if ( !g->body[i] )
			continue;
		if ( j < 0 ) j = i;
		else {
			c = (*cmpdl)(nv,g->body[i]->body,g->body[j]->body);
			if ( c > 0 )
				j = i;
			else if ( c == 0 )
				g->body[i] = NEXT(g->body[i]);	
		}
	}
	if ( j < 0 )
		return 0;
	else {
		d = g->body[j]->body;
		g->body[j] = NEXT(g->body[j]);
		return d;
	}
}

/*  DPV functions */

void adddv(VL vl,DPV p1,DPV p2,DPV *pr)
{
	int i,len;
	DP *e;

	if ( !p1 || !p2 )
		error("adddv : invalid argument");
	else if ( p1->len != p2->len )
		error("adddv : size mismatch");
	else {
		len = p1->len;
		e = (DP *)MALLOC(p1->len*sizeof(DP));
		for ( i = 0; i < len; i++ )
			addd(vl,p1->body[i],p2->body[i],&e[i]);
		MKDPV(len,e,*pr);
		(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

void subdv(VL vl,DPV p1,DPV p2,DPV *pr)
{
	int i,len;
	DP *e;

	if ( !p1 || !p2 )
		error("subdv : invalid argument");
	else if ( p1->len != p2->len )
		error("subdv : size mismatch");
	else {
		len = p1->len;
		e = (DP *)MALLOC(p1->len*sizeof(DP));
		for ( i = 0; i < len; i++ )
			subd(vl,p1->body[i],p2->body[i],&e[i]);
		MKDPV(len,e,*pr);
		(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

void chsgndv(DPV p1,DPV *pr)
{
	int i,len;
	DP *e;

	if ( !p1 )
		error("subdv : invalid argument");
	else {
		len = p1->len;
		e = (DP *)MALLOC(p1->len*sizeof(DP));
		for ( i = 0; i < len; i++ )
			chsgnd(p1->body[i],&e[i]);
		MKDPV(len,e,*pr);
		(*pr)->sugar = p1->sugar;
	}
}

void muldv(VL vl,DP p1,DPV p2,DPV *pr)
{
	int i,len;
	DP *e;

	len = p2->len;
	e = (DP *)MALLOC(p2->len*sizeof(DP));
	if ( !p1 ) {
		MKDPV(len,e,*pr);
		(*pr)->sugar = 0;
	} else {
		for ( i = 0; i < len; i++ )
			muld(vl,p1,p2->body[i],&e[i]);
		MKDPV(len,e,*pr);
		(*pr)->sugar = p1->sugar + p2->sugar;	
	}
}

int compdv(VL vl,DPV p1,DPV p2)
{
	int i,t,len;

	if ( p1->len != p2->len )
		error("compdv : size mismatch");
	else {
		len = p1->len;
		for ( i = 0; i < len; i++ )
			if ( t = compd(vl,p1->body[i],p2->body[i]) )
				return t;
		return 0;
	}
}

int ni_next(int *a,int n)
{
	int i,j,k,kj;

	/* find the first nonzero a[j] */
	for ( j = 0; j < n && a[j] == 0; j++ );
	/* find the first zero a[k] after a[j] */
	for ( k = j; k < n && a[k] == 1; k++ );
	if ( k == n ) return 0;
	/* a[0] = 0, ... , a[j-1] = 0, a[j] = 1, ..., a[k-1] = 1, a[k] = 0 */
	/* a[0] = 1,..., a[k-j-2] = 1, a[k-j-1] = 0, ..., a[k-1] = 0, a[k] = 1 */
	kj = k-j-1;
	for ( i = 0; i < kj; i++ ) a[i] = 1;
	for ( ; i < k; i++ ) a[i] = 0;
	a[k] = 1;
	return 1;
}

int comp_nbm(NBM a,NBM b)
{
	int d,i,w,ai,bi;
	int *ab,*bb;

	if ( a->d > b->d ) return 1;
	else if ( a->d < b->d ) return -1;
	else {
		d = a->d; ab = a->b; bb = b->b;
#if 0
		w = (d+31)/32;
		for ( i = 0; i < w; i++ ) 
			if ( ab[i] > bb[i] ) return 1;
			else if ( ab[i] < bb[i] ) return -1;
#else
		for ( i = 0; i < d; i++ ) {
			ai = NBM_GET(ab,i);
			bi = NBM_GET(bb,i);
			if ( ai > bi ) return 1;
			else if ( ai < bi ) return -1;
		}
#endif
		return 0;
	}
}

NBM mul_nbm(NBM a,NBM b)
{
	int ad,bd,d,i,j;
	int *ab,*bb,*mb;
	NBM m;
	NODE r;
	NBP u;

	ad = a->d; bd = b->d; ab = a->b; bb = b->b;
	d = ad + bd;
	NEWNBM(m); NEWNBMBDY(m,d);
	m->d = d; mulp(CO,a->c,b->c,&m->c); mb = m->b; 
	j = 0;
	for ( i = 0; i < ad; i++, j++ )
		if ( NBM_GET(ab,i) ) NBM_SET(mb,j);
		else NBM_CLR(mb,j);
	for ( i = 0; i < bd; i++, j++ )
		if ( NBM_GET(bb,i) ) NBM_SET(mb,j);
		else NBM_CLR(mb,j);
	return m;
}

NBP nbmtonbp(NBM m)
{
	NODE n;
	NBP u;

	MKNODE(n,m,0);
	MKNBP(u,n);
	return u;
}

/* a=c*x*rest -> a0= x*rest, ah=x, ar=rest */

P separate_nbm(NBM a,NBP *a0,NBP *ah,NBP *ar)
{
	int i,d1;
	NBM t;

	if ( !a->d ) error("separate_nbm : invalid argument");

	if ( a0 ) {
		NEWNBM(t); t->d = a->d; t->b = a->b; t->c = (P)ONE;
		*a0 = nbmtonbp(t);
	}

	if ( ah ) {
		NEWNBM(t); NEWNBMBDY(t,1); t->d = 1; t->c = (P)ONE;
		if ( NBM_GET(a->b,0) ) NBM_SET(t->b,0);
		else NBM_CLR(t->b,0);
		*ah = nbmtonbp(t);
	}

	if ( ar ) {
		d1 = a->d-1;
		NEWNBM(t); NEWNBMBDY(t,d1); t->d = d1; t->c = (P)ONE;
		for ( i = 0; i < d1; i++ ) {
			if ( NBM_GET(a->b,i+1) ) NBM_SET(t->b,i);
			else NBM_CLR(t->b,i);
		}
		*ar = nbmtonbp(t);
	}

	return a->c;
}

/* a=c*rest*x -> a0= rest*x, ar=rest, at=x */

P separate_tail_nbm(NBM a,NBP *a0,NBP *ar,NBP *at)
{
	int i,d,d1;
	NBM t;

	if ( !(d=a->d) ) error("separate_tail_nbm : invalid argument");

	if ( a0 ) {
		NEWNBM(t); t->d = a->d; t->b = a->b; t->c = (P)ONE;
		*a0 = nbmtonbp(t);
	}

	d1 = a->d-1;
	if ( at ) {
		NEWNBM(t); NEWNBMBDY(t,1); t->d = 1; t->c = (P)ONE;
		if ( NBM_GET(a->b,d1) ) NBM_SET(t->b,0);
		else NBM_CLR(t->b,0);
		*at = nbmtonbp(t);
	}

	if ( ar ) {
		NEWNBM(t); NEWNBMBDY(t,d1); t->d = d1; t->c = (P)ONE;
		for ( i = 0; i < d1; i++ ) {
			if ( NBM_GET(a->b,i) ) NBM_SET(t->b,i);
			else NBM_CLR(t->b,i);
		}
		*ar = nbmtonbp(t);
	}

	return a->c;
}

NBP make_xky(int k)
{
	int k1,i;
	NBM t;

	NEWNBM(t); NEWNBMBDY(t,k); t->d = k; t->c = (P)ONE;
	k1 = k-1;
	for ( i = 0; i < k1; i++ ) NBM_SET(t->b,i);
	NBM_CLR(t->b,i);
	return nbmtonbp(t);
}

/* a=c*x^(k-1)*y*rest -> a0= x^(k-1)*y*rest, ah=x^(k-1)*y, ar=rest */

P separate_xky_nbm(NBM a,NBP *a0,NBP *ah,NBP *ar)
{
	int i,d1,k,k1;
	NBM t;

	if ( !a->d )
		error("separate_nbm : invalid argument");
	for ( i = 0; i < a->d && NBM_GET(a->b,i); i++ );
	if ( i == a->d )
		error("separate_nbm : invalid argument");
	k1 = i;
	k = i+1;

	if ( a0 ) {
		NEWNBM(t); t->d = a->d; t->b = a->b; t->c = (P)ONE;
		*a0 = nbmtonbp(t);
	}

	if ( ah ) {
		NEWNBM(t); NEWNBMBDY(t,k); t->d = k; t->c = (P)ONE;
		for ( i = 0; i < k1; i++ ) NBM_SET(t->b,i);
		NBM_CLR(t->b,i);
		*ah = nbmtonbp(t);
	}

	if ( ar ) {
		d1 = a->d-k;
		NEWNBM(t); NEWNBMBDY(t,d1); t->d = d1; t->c = (P)ONE;
		for ( i = 0; i < d1; i++ ) {
			if ( NBM_GET(a->b,i+k) ) NBM_SET(t->b,i);
			else NBM_CLR(t->b,i);
		}
		*ar = nbmtonbp(t);
	}

	return a->c;
}

void shuffle_mulnbp(VL vl,NBP p1,NBP p2, NBP *rp);
void harmonic_mulnbp(VL vl,NBP p1,NBP p2, NBP *rp);
void mulnbmnbp(VL vl,NBM m,NBP p, NBP *rp);
void mulnbpnbm(VL vl,NBP p,NBM m, NBP *rp);

NBP shuffle_mul_nbm(NBM a,NBM b)
{
	NBP u,a0,ah,ar,b0,bh,br,a1,b1,t;
	P ac,bc,c;

	if ( !a->d || !b->d )
		u = nbmtonbp(mul_nbm(a,b));
	else {
		ac = separate_nbm(a,&a0,&ah,&ar);
		bc = separate_nbm(b,&b0,&bh,&br);
		mulp(CO,ac,bc,&c);
		shuffle_mulnbp(CO,ar,b0,&t); mulnbp(CO,ah,t,&a1);
		shuffle_mulnbp(CO,a0,br,&t); mulnbp(CO,bh,t,&b1);
		addnbp(CO,a1,b1,&t); mulnbp(CO,(NBP)c,t,&u);
	}
	return u;
}

NBP harmonic_mul_nbm(NBM a,NBM b)
{
	NBP u,a0,ah,ar,b0,bh,br,a1,b1,t,s,abk,ab1;
	P ac,bc,c;

	if ( !a->d || !b->d )
		u = nbmtonbp(mul_nbm(a,b));
	else {
		mulp(CO,a->c,b->c,&c);
		ac = separate_xky_nbm(a,&a0,&ah,&ar);
		bc = separate_xky_nbm(b,&b0,&bh,&br);
		mulp(CO,ac,bc,&c);
		harmonic_mulnbp(CO,ar,b0,&t); mulnbp(CO,ah,t,&a1);
		harmonic_mulnbp(CO,a0,br,&t); mulnbp(CO,bh,t,&b1);
		abk = make_xky(((NBM)BDY(BDY(ah)))->d+((NBM)BDY(BDY(bh)))->d);
		harmonic_mulnbp(CO,ar,br,&t); mulnbp(CO,abk,t,&ab1);
		addnbp(CO,a1,b1,&t); addnbp(CO,t,ab1,&s); mulnbp(CO,(NBP)c,s,&u);
	}
	return u;

}

void addnbp(VL vl,NBP p1,NBP p2, NBP *rp)
{
	NODE b1,b2,br,br0;
	NBM m1,m2,m;
	P c;

	if ( !p1 )
		*rp = p2;
	else if ( !p2 )
		*rp = p1;
	else {
		for ( b1 = BDY(p1), b2 = BDY(p2), br0 = 0; b1 && b2; ) {
			m1 = (NBM)BDY(b1); m2 = (NBM)BDY(b2);
			switch ( comp_nbm(m1,m2) ) {
				case 0:
					addp(CO,m1->c,m2->c,&c);
					if ( c ) {
						NEXTNODE(br0,br);
						NEWNBM(m); m->d = m1->d; m->c = c; m->b = m1->b;
						BDY(br) = (pointer)m;
					}
					b1 = NEXT(b1); b2 = NEXT(b2); break;
				case 1:
					NEXTNODE(br0,br); BDY(br) = BDY(b1);
					b1 = NEXT(b1); break;
				case -1:
					NEXTNODE(br0,br); BDY(br) = BDY(b2);
					b2 = NEXT(b2); break;
			}
		}
		if ( !br0 )
			if ( b1 )
				br0 = b1;
			else if ( b2 )
				br0 = b2;
			else {
				*rp = 0;
				return;
			}
		else if ( b1 )
			NEXT(br) = b1;
		else if ( b2 )
				NEXT(br) = b2;
		else
			NEXT(br) = 0;
		MKNBP(*rp,br0);
	}
}

void subnbp(VL vl,NBP p1,NBP p2, NBP *rp)
{
	NBP t;

	chsgnnbp(p2,&t);
	addnbp(vl,p1,t,rp);
}

void chsgnnbp(NBP p,NBP *rp)
{
	NODE r0,r,b;
	NBM m,m1;

	for ( r0 = 0, b = BDY(p); b; b = NEXT(b) ) {
		NEXTNODE(r0,r);
		m = (NBM)BDY(b);
		NEWNBM(m1); m1->d = m->d; m1->b = m->b; chsgnp(m->c,&m1->c);
		BDY(r) = m1;
	}
	if ( r0 ) NEXT(r) = 0;
	MKNBP(*rp,r0);
}

void mulnbp(VL vl,NBP p1,NBP p2, NBP *rp)
{
	NODE b,n;
	NBP r,t,s;
	NBM m;

	if ( !p1 || !p2 ) {
		*rp = 0; return;
	}
	if ( OID(p1) != O_NBP ) {
		if ( !POLY(p1) )
			error("mulnbp : invalid argument");
		NEWNBM(m); m->d = 0; m->b = 0; m->c = (P)p1;	
		MKNODE(n,m,0); MKNBP(p1,n);
	}
	if ( OID(p2) != O_NBP ) {
		if ( !POLY(p2) )
			error("mulnbp : invalid argument");
		NEWNBM(m); m->d = 0; m->b = 0; m->c = (P)p2;	
		MKNODE(n,m,0); MKNBP(p2,n);
	}
	if ( length(BDY(p1)) < length(BDY(p2)) ) {
		for ( r = 0, b = BDY(p1); b; b = NEXT(b) ) {
			mulnbmnbp(vl,(NBM)BDY(b),p2,&t);
			addnbp(vl,r,t,&s); r = s;
		}
		*rp = r;
	} else {
		for ( r = 0, b = BDY(p2); b; b = NEXT(b) ) {
			mulnbpnbm(vl,p1,(NBM)BDY(b),&t);
			addnbp(vl,r,t,&s); r = s;
		}
		*rp = r;
	}
}

void mulnbmnbp(VL vl,NBM m,NBP p, NBP *rp)
{
	NODE b,r0,r;

	if ( !p ) *rp = 0;
	else {
		for ( r0 = 0, b = BDY(p); b; b = NEXT(b) ) {
			NEXTNODE(r0,r);
			BDY(r) = mul_nbm(m,(NBM)BDY(b));
		}
		if ( r0 ) NEXT(r) = 0;
		MKNBP(*rp,r0);
	}
}

void mulnbpnbm(VL vl,NBP p,NBM m, NBP *rp)
{
	NODE b,r0,r;

	if ( !p ) *rp = 0;
	else {
		for ( r0 = 0, b = BDY(p); b; b = NEXT(b) ) {
			NEXTNODE(r0,r);
			BDY(r) = mul_nbm((NBM)BDY(b),m);
		}
		if ( r0 ) NEXT(r) = 0;
		MKNBP(*rp,r0);
	}
}

void pwrnbp(VL vl,NBP a,Q q,NBP *c)
{
	int t;
	NBP a1,a2;
	N n1;
	Q q1;
	NBM m;
	NODE r;

	if ( !q ) {
		 NEWNBM(m); m->d = 0; m->c = (P)ONE; m->b = 0;
		 MKNODE(r,m,0); MKNBP(*c,r);
	} else if ( !a )
		*c = 0;
	else if ( UNIQ(q) )
		*c = a;
	else {
		t = divin(NM(q),2,&n1); NTOQ(n1,1,q1);
		pwrnbp(vl,a,q1,&a1);
		mulnbp(vl,a1,a1,&a2);
		if ( t )
			mulnbp(vl,a2,a,c);
		else
			*c = a2;
	}
}

int compnbp(VL vl,NBP p1,NBP p2)
{
	NODE n1,n2;
	NBM m1,m2;
	int t;

	if ( !p1 )
		return p2 ? -1 : 0;
	else if ( !p2 )
		return 1;
	else {
		for ( n1 = BDY(p1), n2 = BDY(p2);
			n1 && n2; n1 = NEXT(n1), n2 = NEXT(n2) ) {
			m1 = (NBM)BDY(n1); m2 = (NBM)BDY(n2);
			if ( (t = comp_nbm(m1,m2)) || (t = compp(CO,m1->c,m2->c) ) )
				return t;
		}
		if ( n1 )
			return 1;
		else if ( n2 )
			return -1;
		else
			return 0;
	}
}

void shuffle_mulnbp(VL vl,NBP p1,NBP p2, NBP *rp)
{
	NODE b1,b2,n;
	NBP r,t,s;
	NBM m;

	if ( !p1 || !p2 ) {
		*rp = 0; return;
	}
	if ( OID(p1) != O_NBP ) {
		if ( !POLY(p1) )
			error("shuffle_mulnbp : invalid argument");
		NEWNBM(m); m->d = 0; m->b = 0; m->c = (P)p1;	
		MKNODE(n,m,0); MKNBP(p1,n);
	}
	if ( OID(p2) != O_NBP ) {
		if ( !POLY(p2) )
			error("shuffle_mulnbp : invalid argument");
		NEWNBM(m); m->d = 0; m->b = 0; m->c = (P)p2;	
		MKNODE(n,m,0); MKNBP(p2,n);
	}
	for ( r = 0, b1 = BDY(p1); b1; b1 = NEXT(b1) )
		for ( m = BDY(b1), b2 = BDY(p2); b2; b2 = NEXT(b2) ) {
			t = shuffle_mul_nbm(m,(NBM)BDY(b2));
			addnbp(vl,r,t,&s); r = s;
		}
	*rp = r;
}

void harmonic_mulnbp(VL vl,NBP p1,NBP p2, NBP *rp)
{
	NODE b1,b2,n;
	NBP r,t,s;
	NBM m;

	if ( !p1 || !p2 ) {
		*rp = 0; return;
	}
	if ( OID(p1) != O_NBP ) {
		if ( !POLY(p1) )
			error("harmonic_mulnbp : invalid argument");
		NEWNBM(m); m->d = 0; m->b = 0; m->c = (P)p1;	
		MKNODE(n,m,0); MKNBP(p1,n);
	}
	if ( OID(p2) != O_NBP ) {
		if ( !POLY(p2) )
			error("harmonic_mulnbp : invalid argument");
		NEWNBM(m); m->d = 0; m->b = 0; m->c = (P)p2;	
		MKNODE(n,m,0); MKNBP(p2,n);
	}
	for ( r = 0, b1 = BDY(p1); b1; b1 = NEXT(b1) )
		for ( m = BDY(b1), b2 = BDY(p2); b2; b2 = NEXT(b2) ) {
			t = harmonic_mul_nbm(m,(NBM)BDY(b2));
			addnbp(vl,r,t,&s); r = s;
		}
	*rp = r;
}

#if 0
NBP shuffle_mul_nbm(NBM a,NBM b)
{
	int ad,bd,d,i,ai,bi,bit,s;
	int *ab,*bb,*wmb,*w;
	NBM wm,tm;
	P c,c1;
	NODE r,t,t1,p;
	NBP u;

	ad = a->d; bd = b->d; ab = a->b; bb = b->b;
	d = ad + bd;
	w = (int *)ALLOCA(d*sizeof(int));
	NEWNBM(wm); NEWNBMBDY(wm,d); wmb = wm->b;
	for ( i = 0; i < ad; i++ ) w[i] = 1;
	for ( ; i < d; i++ ) w[i] = 0;
	mulp(CO,a->c,b->c,&c);
	r = 0;
	do {
		wm->d = d; wm->c = c;
		ai = 0; bi = 0;
		for ( i = 0; i < d; i++ ) {
			if ( w[i] ) { bit = NBM_GET(ab,ai); ai++; }
			else { bit = NBM_GET(bb,bi); bi++; }
			if ( bit ) NBM_SET(wmb,i);
			else NBM_CLR(wmb,i);
		}
		for ( p = 0, t = r; t; p = t, t = NEXT(t) ) {
			tm = (NBM)BDY(t);
			s = comp_nbm(tm,wm);
			if ( s < 0 ) {
				/* insert */
				MKNODE(t1,wm,t);
				if ( !p ) r = t1;
				else NEXT(p) = t1;
				NEWNBM(wm); NEWNBMBDY(wm,d); wmb = wm->b;
				break;
			} else if ( s == 0 ) {
				/* add coefs */
				addp(CO,tm->c,c,&c1);
				if ( c1 ) tm->c = c1;
				else NEXT(p) = NEXT(t);
				break;
			}
		}
		if ( !t ) {
			/* append */
			MKNODE(t1,wm,t);
			if ( !p ) r = t1;
			else NEXT(p) = t1;
			NEWNBM(wm); NEWNBMBDY(wm,d); wmb = wm->b;
		}
	} while ( ni_next(w,d) );
	MKNBP(u,r);
	return u;
}

int nbmtoxky(NBM a,int *b)
{
	int d,i,j,k;
	int *p;

	d = a->d; p = a->b;
	for ( i = j = 0, k = 1; i < d; i++ ) {
		if ( !NBM_GET(p,i) ) {
			b[j++] = k;
			k = 1;
		} else k++;
	}
	return j;
}

NBP harmonic_mul_nbm(NBM a,NBM b)
{
	int da,db,d,la,lb,lmax,lmin,l,lab,la1,lb1,lab1;
	int i,j,k,ia,ib,s;
	int *wa,*wb,*w,*wab,*wa1,*wmb;
	P c,c1;
	NBM wm,tm;
	NODE r,t1,t,p;
	NBP u;

	da = a->d; db = b->d; d = da+db;
	wa = (int *)ALLOCA(da*sizeof(int));
	wb = (int *)ALLOCA(db*sizeof(int));
	la = nbmtoxky(a,wa);
	lb = nbmtoxky(b,wb);
	mulp(CO,a->c,b->c,&c);
	/* wa[0],..,wa[la-1] <-> x^wa[0]y x^wa[1]y .. */	
	/* lmax : total length */
	lmax = la+lb;
	lmin = la>lb?la:lb;	
	w = (int *)ALLOCA(lmax*sizeof(int));
	/* position of a+b */
	wab = (int *)ALLOCA(lmax*sizeof(int));
	/* position of a */
	wa1 = (int *)ALLOCA(lmax*sizeof(int));
	NEWNBM(wm); NEWNBMBDY(wm,d); wmb = wm->b;
	for ( l = lmin, r = 0; l <= lmax; l++ ) {
		lab = lmax - l;
		la1 = la - lab;
		lb1 = lb - lab;
		lab1 = l-lab;
		/* partion l into three parts: a, b, a+b */
		/* initialize wab */
		for ( i = 0; i < lab; i++ ) wab[i] = 1;
		for ( ; i < l; i++ ) wab[i] = 0;
		do {
			/* initialize wa1 */
			for ( i = 0; i < la1; i++ ) wa1[i] = 1;
			for ( ; i < lab1; i++ ) wa1[i] = 0;
			do {
				ia = 0; ib = 0;
				for ( i = j = 0; i < l; i++ )
					if ( wab[i] ) w[i] = wa[ia++]+wb[ib++];
					else if ( wa1[j++] ) w[i] = wa[ia++];
					else w[i] = wb[ib++];
				for ( i = j = 0; i < l; i++ ) {
					for ( k = w[i]-1; k > 0; k--, j++ ) NBM_SET(wmb,j);
					NBM_CLR(wmb,j); j++;
				}
				wm->d = j; wm->c = c;
				for ( p = 0, t = r; t; p = t, t = NEXT(t) ) {
					tm = (NBM)BDY(t);
					s = comp_nbm(tm,wm);
					if ( s < 0 ) {
						/* insert */
						MKNODE(t1,wm,t);
						if ( !p ) r = t1;
						else NEXT(p) = t1;
						NEWNBM(wm); NEWNBMBDY(wm,d); wmb = wm->b;
						break;
					} else if ( s == 0 ) {
						/* add coefs */
						addp(CO,tm->c,c,&c1);
						if ( c1 ) tm->c = c1;
						else NEXT(p) = NEXT(t);
						break;
					}
				}
				if ( !t ) {
					/* append */
					MKNODE(t1,wm,t);
					if ( !p ) r = t1;
					else NEXT(p) = t1;
					NEWNBM(wm); NEWNBMBDY(wm,d); wmb = wm->b;
				}
			} while ( ni_next(wa1,lab1) );
		} while ( ni_next(wab,l) );
	}
	MKNBP(u,r);
	return u;
}
#endif
