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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/dp.c,v 1.24 2002/01/28 00:54:41 noro Exp $ 
*/
#include "ca.h"
#include "base.h"
#include "parse.h"

extern int dp_fcoeffs;
extern int dp_nelim;
extern int dp_order_pair_length;
extern struct order_pair *dp_order_pair;
extern struct order_spec dp_current_spec;

int do_weyl;

void Pdp_ord(), Pdp_ptod(), Pdp_dtop();
void Pdp_ptozp(), Pdp_ptozp2(), Pdp_red(), Pdp_red2(), Pdp_lcm(), Pdp_redble();
void Pdp_sp(), Pdp_hm(), Pdp_ht(), Pdp_hc(), Pdp_rest(), Pdp_td(), Pdp_sugar();
void Pdp_cri1(),Pdp_cri2(),Pdp_subd(),Pdp_mod(),Pdp_red_mod(),Pdp_tdiv();
void Pdp_prim(),Pdp_red_coef(),Pdp_mag(),Pdp_set_kara(),Pdp_rat();
void Pdp_nf(),Pdp_true_nf();
void Pdp_nf_mod(),Pdp_true_nf_mod();
void Pdp_criB(),Pdp_nelim();
void Pdp_minp(),Pdp_sp_mod();
void Pdp_homo(),Pdp_dehomo();
void Pdp_gr_mod_main(),Pdp_gr_f_main();
void Pdp_gr_main(),Pdp_gr_hm_main(),Pdp_gr_d_main(),Pdp_gr_flags();
void Pdp_f4_main(),Pdp_f4_mod_main(),Pdp_f4_f_main();
void Pdp_gr_print();
void Pdp_mbase(),Pdp_lnf_mod(),Pdp_nf_tab_mod(),Pdp_mdtod();
void Pdp_vtoe(), Pdp_etov(), Pdp_dtov(), Pdp_idiv(), Pdp_sep();
void Pdp_cont();
void Pdp_gr_checklist();

void Pdp_weyl_red();
void Pdp_weyl_sp();
void Pdp_weyl_nf(),Pdp_weyl_nf_mod();
void Pdp_weyl_gr_main(),Pdp_weyl_gr_mod_main(),Pdp_weyl_gr_f_main();
void Pdp_weyl_f4_main(),Pdp_weyl_f4_mod_main(),Pdp_weyl_f4_f_main();
void Pdp_weyl_mul(),Pdp_weyl_mul_mod();
void Pdp_weyl_set_weight();
void Pdp_set_weight();
void Pdp_nf_f(),Pdp_weyl_nf_f();
void Pdp_lnf_f();

LIST remove_zero_from_list(LIST);

struct ftab dp_tab[] = {
	/* content reduction */
	{"dp_ptozp",Pdp_ptozp,1},
	{"dp_ptozp2",Pdp_ptozp2,2},
	{"dp_prim",Pdp_prim,1},
	{"dp_red_coef",Pdp_red_coef,2},
	{"dp_cont",Pdp_cont,1},

/* polynomial ring */
	/* s-poly */
	{"dp_sp",Pdp_sp,2},
	{"dp_sp_mod",Pdp_sp_mod,3},

	/* m-reduction */
	{"dp_red",Pdp_red,3},
	{"dp_red_mod",Pdp_red_mod,4},

	/* normal form */
	{"dp_nf",Pdp_nf,4},
	{"dp_nf_f",Pdp_nf_f,4},
	{"dp_true_nf",Pdp_true_nf,4},
	{"dp_nf_mod",Pdp_nf_mod,5},
	{"dp_true_nf_mod",Pdp_true_nf_mod,5},
	{"dp_lnf_mod",Pdp_lnf_mod,3},
	{"dp_nf_tab_mod",Pdp_nf_tab_mod,3},
	{"dp_lnf_f",Pdp_lnf_f,2},

	/* Buchberger algorithm */
	{"dp_gr_main",Pdp_gr_main,5},
	{"dp_gr_mod_main",Pdp_gr_mod_main,5},
	{"dp_gr_f_main",Pdp_gr_f_main,4},
	{"dp_gr_checklist",Pdp_gr_checklist,2},

	/* F4 algorithm */
	{"dp_f4_main",Pdp_f4_main,3},
	{"dp_f4_mod_main",Pdp_f4_mod_main,4},

/* weyl algebra */
	/* multiplication */
	{"dp_weyl_mul",Pdp_weyl_mul,2},
	{"dp_weyl_mul_mod",Pdp_weyl_mul_mod,3},

	/* s-poly */
	{"dp_weyl_sp",Pdp_weyl_sp,2},

	/* m-reduction */
	{"dp_weyl_red",Pdp_weyl_red,3},

	/* normal form */
	{"dp_weyl_nf",Pdp_weyl_nf,4},
	{"dp_weyl_nf_mod",Pdp_weyl_nf_mod,5},
	{"dp_weyl_nf_f",Pdp_weyl_nf_f,4},

	/* Buchberger algorithm */
	{"dp_weyl_gr_main",Pdp_weyl_gr_main,5},
	{"dp_weyl_gr_mod_main",Pdp_weyl_gr_mod_main,5},
	{"dp_weyl_gr_f_main",Pdp_weyl_gr_f_main,4},

	/* F4 algorithm */
	{"dp_weyl_f4_main",Pdp_weyl_f4_main,3},
	{"dp_weyl_f4_mod_main",Pdp_weyl_f4_mod_main,4},

	/* misc */
	{"dp_set_weight",Pdp_set_weight,-1},
	{"dp_weyl_set_weight",Pdp_weyl_set_weight,-1},
	{0,0,0},
};

struct ftab dp_supp_tab[] = {
	/* setting flags */
	{"dp_ord",Pdp_ord,-1},
	{"dp_set_kara",Pdp_set_kara,-1},
	{"dp_nelim",Pdp_nelim,-1},
	{"dp_gr_flags",Pdp_gr_flags,-1},
	{"dp_gr_print",Pdp_gr_print,-1},

	/* converters */
	{"dp_ptod",Pdp_ptod,2},
	{"dp_dtop",Pdp_dtop,2},
	{"dp_homo",Pdp_homo,1},
	{"dp_dehomo",Pdp_dehomo,1},
	{"dp_etov",Pdp_etov,1},
	{"dp_vtoe",Pdp_vtoe,1},
	{"dp_dtov",Pdp_dtov,1},
	{"dp_mdtod",Pdp_mdtod,1},
	{"dp_mod",Pdp_mod,3},
	{"dp_rat",Pdp_rat,1},

	/* criteria */
	{"dp_cri1",Pdp_cri1,2},
	{"dp_cri2",Pdp_cri2,2},
	{"dp_criB",Pdp_criB,3},

	/* simple operation */
	{"dp_subd",Pdp_subd,2},
	{"dp_lcm",Pdp_lcm,2},
	{"dp_hm",Pdp_hm,1},
	{"dp_ht",Pdp_ht,1},
	{"dp_hc",Pdp_hc,1},
	{"dp_rest",Pdp_rest,1},

	/* degree and size */
	{"dp_td",Pdp_td,1},
	{"dp_mag",Pdp_mag,1},
	{"dp_sugar",Pdp_sugar,1},

	/* misc */
	{"dp_mbase",Pdp_mbase,1},
	{"dp_redble",Pdp_redble,2},
	{"dp_sep",Pdp_sep,2},
	{"dp_idiv",Pdp_idiv,2},
	{"dp_tdiv",Pdp_tdiv,2},
	{"dp_minp",Pdp_minp,2},

	{0,0,0}
};

void Pdp_mdtod(arg,rp)
NODE arg;
DP *rp;
{
	MP m,mr,mr0;
	DP p;
	P t;

	p = (DP)ARG0(arg);
	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			mptop(m->c,&t); NEXTMP(mr0,mr); mr->c = t; mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
	}
}

void Pdp_sep(arg,rp)
NODE arg;
VECT *rp;
{
	DP p,r;
	MP m,t;
	MP *w0,*w;
	int i,n,d,nv,sugar;
	VECT v;
	pointer *pv;

	p = (DP)ARG0(arg); m = BDY(p);
	d = QTOS((Q)ARG1(arg));
	for ( t = m, n = 0; t; t = NEXT(t), n++ );
	if ( d > n )
		d = n;
	MKVECT(v,d); *rp = v;
	pv = BDY(v); nv = p->nv; sugar = p->sugar;
	w0 = (MP *)MALLOC(d*sizeof(MP)); bzero(w0,d*sizeof(MP));
	w = (MP *)MALLOC(d*sizeof(MP)); bzero(w,d*sizeof(MP));
	for ( t = BDY(p), i = 0; t; t = NEXT(t), i++, i %= d  ) {
		NEXTMP(w0[i],w[i]); w[i]->c = t->c; w[i]->dl = t->dl;
	}
	for ( i = 0; i < d; i++ ) {
		NEXT(w[i]) = 0; MKDP(nv,w0[i],r); r->sugar = sugar;
		pv[i] = (pointer)r;
	}
}

void Pdp_idiv(arg,rp)
NODE arg;
DP *rp;
{
	dp_idiv((DP)ARG0(arg),(Q)ARG1(arg),rp);
}

void Pdp_cont(arg,rp)
NODE arg;
Q *rp;
{
	dp_cont((DP)ARG0(arg),rp);
}

void Pdp_dtov(arg,rp)
NODE arg;
VECT *rp;
{
	dp_dtov((DP)ARG0(arg),rp);
}

void Pdp_mbase(arg,rp)
NODE arg;
LIST *rp;
{
	NODE mb;

	asir_assert(ARG0(arg),O_LIST,"dp_mbase");
	dp_mbase(BDY((LIST)ARG0(arg)),&mb);
	MKLIST(*rp,mb);
}

void Pdp_etov(arg,rp)
NODE arg;
VECT *rp;
{
	DP dp;
	int n,i;
	int *d;
	VECT v;
	Q t;

	dp = (DP)ARG0(arg);
	asir_assert(dp,O_DP,"dp_etov");
	n = dp->nv; d = BDY(dp)->dl->d;
	MKVECT(v,n);
	for ( i = 0; i < n; i++ ) {
		STOQ(d[i],t); v->body[i] = (pointer)t;
	}
	*rp = v;
}

void Pdp_vtoe(arg,rp)
NODE arg;
DP *rp;
{
	DP dp;
	DL dl;
	MP m;
	int n,i,td;
	int *d;
	VECT v;

	v = (VECT)ARG0(arg);
	asir_assert(v,O_VECT,"dp_vtoe");
	n = v->len;
	NEWDL(dl,n); d = dl->d;
	for ( i = 0, td = 0; i < n; i++ ) {
		d[i] = QTOS((Q)(v->body[i])); td += MUL_WEIGHT(d[i],i);
	}
	dl->td = td;
	NEWMP(m); m->dl = dl; m->c = (P)ONE; NEXT(m) = 0;
	MKDP(n,m,dp); dp->sugar = td;
	*rp = dp;
}

void Pdp_lnf_mod(arg,rp)
NODE arg;
LIST *rp;
{
	DP r1,r2;
	NODE b,g,n;
	int mod;

	asir_assert(ARG0(arg),O_LIST,"dp_lnf_mod");
	asir_assert(ARG1(arg),O_LIST,"dp_lnf_mod");
	asir_assert(ARG2(arg),O_N,"dp_lnf_mod");
	b = BDY((LIST)ARG0(arg)); g = BDY((LIST)ARG1(arg));
	mod = QTOS((Q)ARG2(arg));
	dp_lnf_mod((DP)BDY(b),(DP)BDY(NEXT(b)),g,mod,&r1,&r2);
	NEWNODE(n); BDY(n) = (pointer)r1;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)r2;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_lnf_f(arg,rp)
NODE arg;
LIST *rp;
{
	DP r1,r2;
	NODE b,g,n;

	asir_assert(ARG0(arg),O_LIST,"dp_lnf_f");
	asir_assert(ARG1(arg),O_LIST,"dp_lnf_f");
	b = BDY((LIST)ARG0(arg)); g = BDY((LIST)ARG1(arg));
	dp_lnf_f((DP)BDY(b),(DP)BDY(NEXT(b)),g,&r1,&r2);
	NEWNODE(n); BDY(n) = (pointer)r1;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)r2;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_nf_tab_mod(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_nf_tab_mod");
	asir_assert(ARG1(arg),O_VECT,"dp_nf_tab_mod");
	asir_assert(ARG2(arg),O_N,"dp_nf_tab_mod");
	dp_nf_tab_mod((DP)ARG0(arg),(LIST *)BDY((VECT)ARG1(arg)),
		QTOS((Q)ARG2(arg)),rp);
}

void Pdp_ord(arg,rp)
NODE arg;
Obj *rp;
{
	struct order_spec spec;

	if ( !arg )
		*rp = dp_current_spec.obj;
	else if ( !create_order_spec((Obj)ARG0(arg),&spec) )
		error("dp_ord : invalid order specification");
	else {
		initd(&spec); *rp = spec.obj;
	}
}

void Pdp_ptod(arg,rp)
NODE arg;
DP *rp;
{
	NODE n;
	VL vl,tvl;

	asir_assert(ARG0(arg),O_P,"dp_ptod");
	asir_assert(ARG1(arg),O_LIST,"dp_ptod");
	for ( vl = 0, n = BDY((LIST)ARG1(arg)); n; n = NEXT(n) ) {
		if ( !vl ) {
			NEWVL(vl); tvl = vl;
		} else {
			NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
		}
		VR(tvl) = VR((P)BDY(n));
	}
	if ( vl )
		NEXT(tvl) = 0;
	ptod(CO,vl,(P)ARG0(arg),rp);
}

void Pdp_dtop(arg,rp)
NODE arg;
P *rp;
{
	NODE n;
	VL vl,tvl;

	asir_assert(ARG0(arg),O_DP,"dp_dtop");
	asir_assert(ARG1(arg),O_LIST,"dp_dtop");
	for ( vl = 0, n = BDY((LIST)ARG1(arg)); n; n = NEXT(n) ) {
		if ( !vl ) {
			NEWVL(vl); tvl = vl;
		} else {
			NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
		}
		VR(tvl) = VR((P)BDY(n));
	}
	if ( vl )
		NEXT(tvl) = 0;
	dtop(CO,vl,(DP)ARG0(arg),rp);
}

extern LIST Dist;

void Pdp_ptozp(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_ptozp");
	dp_ptozp((DP)ARG0(arg),rp);
}
	
void Pdp_ptozp2(arg,rp)
NODE arg;
LIST *rp;
{
	DP p0,p1,h,r;
	NODE n0;

	p0 = (DP)ARG0(arg); p1 = (DP)ARG1(arg);
	asir_assert(p0,O_DP,"dp_ptozp2");
	asir_assert(p1,O_DP,"dp_ptozp2");
	dp_ptozp2(p0,p1,&h,&r);
	NEWNODE(n0); BDY(n0) = (pointer)h;
	NEWNODE(NEXT(n0)); BDY(NEXT(n0)) = (pointer)r;
	NEXT(NEXT(n0)) = 0;
	MKLIST(*rp,n0);
}

void Pdp_prim(arg,rp)
NODE arg;
DP *rp;
{
	DP t;

	asir_assert(ARG0(arg),O_DP,"dp_prim");
	dp_prim((DP)ARG0(arg),&t); dp_ptozp(t,rp);
}
	
void Pdp_mod(arg,rp)
NODE arg;
DP *rp;
{
	DP p;
	int mod;
	NODE subst;

	asir_assert(ARG0(arg),O_DP,"dp_mod");
	asir_assert(ARG1(arg),O_N,"dp_mod");
	asir_assert(ARG2(arg),O_LIST,"dp_mod");
	p = (DP)ARG0(arg); mod = QTOS((Q)ARG1(arg));
	subst = BDY((LIST)ARG2(arg));
	dp_mod(p,mod,subst,rp);
}

void Pdp_rat(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_rat");
	dp_rat((DP)ARG0(arg),rp);
}

extern int DP_Multiple;

void Pdp_nf(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP *ps;
	DP g;
	int full;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_nf");
	asir_assert(ARG1(arg),O_DP,"dp_nf");
	asir_assert(ARG2(arg),O_VECT,"dp_nf");
	asir_assert(ARG3(arg),O_N,"dp_nf");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = (Q)ARG3(arg) ? 1 : 0;
	dp_nf_z(b,g,ps,full,DP_Multiple,rp);
}

void Pdp_weyl_nf(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP *ps;
	DP g;
	int full;

	asir_assert(ARG0(arg),O_LIST,"dp_weyl_nf");
	asir_assert(ARG1(arg),O_DP,"dp_weyl_nf");
	asir_assert(ARG2(arg),O_VECT,"dp_weyl_nf");
	asir_assert(ARG3(arg),O_N,"dp_weyl_nf");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = (Q)ARG3(arg) ? 1 : 0;
	do_weyl = 1;
	dp_nf_z(b,g,ps,full,DP_Multiple,rp);
	do_weyl = 0;
}

/* nf computation using field operations */

void Pdp_nf_f(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP *ps;
	DP g;
	int full;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_nf_f");
	asir_assert(ARG1(arg),O_DP,"dp_nf_f");
	asir_assert(ARG2(arg),O_VECT,"dp_nf_f");
	asir_assert(ARG3(arg),O_N,"dp_nf_f");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = (Q)ARG3(arg) ? 1 : 0;
	dp_nf_f(b,g,ps,full,rp);
}

void Pdp_weyl_nf_f(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP *ps;
	DP g;
	int full;

	asir_assert(ARG0(arg),O_LIST,"dp_weyl_nf_f");
	asir_assert(ARG1(arg),O_DP,"dp_weyl_nf_f");
	asir_assert(ARG2(arg),O_VECT,"dp_weyl_nf_f");
	asir_assert(ARG3(arg),O_N,"dp_weyl_nf_f");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = (Q)ARG3(arg) ? 1 : 0;
	do_weyl = 1;
	dp_nf_f(b,g,ps,full,rp);
	do_weyl = 0;
}

void Pdp_nf_mod(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP g;
	DP *ps;
	int mod,full,ac;
	NODE n,n0;

	do_weyl = 0;
	ac = argc(arg);
	asir_assert(ARG0(arg),O_LIST,"dp_nf_mod");
	asir_assert(ARG1(arg),O_DP,"dp_nf_mod");
	asir_assert(ARG2(arg),O_VECT,"dp_nf_mod");
	asir_assert(ARG3(arg),O_N,"dp_nf_mod");
	asir_assert(ARG4(arg),O_N,"dp_nf_mod");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = QTOS((Q)ARG3(arg)); mod = QTOS((Q)ARG4(arg));
	for ( n0 = n = 0; b; b = NEXT(b) ) {
		NEXTNODE(n0,n);
		BDY(n) = (pointer)QTOS((Q)BDY(b));
	}
	if ( n0 )
		NEXT(n) = 0;
	dp_nf_mod(n0,g,ps,mod,full,rp);
}

void Pdp_true_nf(arg,rp)
NODE arg;
LIST *rp;
{
	NODE b,n;
	DP *ps;
	DP g;
	DP nm;
	P dn;
	int full;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_true_nf");
	asir_assert(ARG1(arg),O_DP,"dp_true_nf");
	asir_assert(ARG2(arg),O_VECT,"dp_true_nf");
	asir_assert(ARG3(arg),O_N,"dp_nf");
	if ( !(g = (DP)ARG1(arg)) ) {
		nm = 0; dn = (P)ONE;
	} else {
		b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
		full = (Q)ARG3(arg) ? 1 : 0;
		dp_true_nf(b,g,ps,full,&nm,&dn);
	}
	NEWNODE(n); BDY(n) = (pointer)nm;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)dn;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_weyl_nf_mod(arg,rp)
NODE arg;
DP *rp;
{
	NODE b;
	DP g;
	DP *ps;
	int mod,full,ac;
	NODE n,n0;

	ac = argc(arg);
	asir_assert(ARG0(arg),O_LIST,"dp_weyl_nf_mod");
	asir_assert(ARG1(arg),O_DP,"dp_weyl_nf_mod");
	asir_assert(ARG2(arg),O_VECT,"dp_weyl_nf_mod");
	asir_assert(ARG3(arg),O_N,"dp_weyl_nf_mod");
	asir_assert(ARG4(arg),O_N,"dp_weyl_nf_mod");
	if ( !(g = (DP)ARG1(arg)) ) {
		*rp = 0; return;
	}
	b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
	full = QTOS((Q)ARG3(arg)); mod = QTOS((Q)ARG4(arg));
	for ( n0 = n = 0; b; b = NEXT(b) ) {
		NEXTNODE(n0,n);
		BDY(n) = (pointer)QTOS((Q)BDY(b));
	}
	if ( n0 )
		NEXT(n) = 0;
	do_weyl = 1;
	dp_nf_mod(n0,g,ps,mod,full,rp);
	do_weyl = 0;
}

void Pdp_true_nf_mod(arg,rp)
NODE arg;
LIST *rp;
{
	NODE b;
	DP g,nm;
	P dn;
	DP *ps;
	int mod,full;
	NODE n;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_nf_mod");
	asir_assert(ARG1(arg),O_DP,"dp_nf_mod");
	asir_assert(ARG2(arg),O_VECT,"dp_nf_mod");
	asir_assert(ARG3(arg),O_N,"dp_nf_mod");
	asir_assert(ARG4(arg),O_N,"dp_nf_mod");
	if ( !(g = (DP)ARG1(arg)) ) {
		nm = 0; dn = (P)ONEM;
	} else {
		b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
		full = QTOS((Q)ARG3(arg)); mod = QTOS((Q)ARG4(arg));
		dp_true_nf_mod(b,g,ps,mod,full,&nm,&dn);
	}
	NEWNODE(n); BDY(n) = (pointer)nm;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)dn;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_tdiv(arg,rp)
NODE arg;
DP *rp;
{
	MP m,mr,mr0;
	DP p;
	Q c;
	N d,q,r;
	int sgn;

	asir_assert(ARG0(arg),O_DP,"dp_tdiv");
	asir_assert(ARG1(arg),O_N,"dp_tdiv");
	p = (DP)ARG0(arg); d = NM((Q)ARG1(arg)); sgn = SGN((Q)ARG1(arg));
	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			divn(NM((Q)m->c),d,&q,&r);
			if ( r ) {
				*rp = 0; return;
			} else {
				NEXTMP(mr0,mr); NTOQ(q,SGN((Q)m->c)*sgn,c);
				mr->c = (P)c; mr->dl = m->dl;
			}
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
	}
}

void Pdp_red_coef(arg,rp)
NODE arg;
DP *rp;
{
	MP m,mr,mr0;
	P q,r;
	DP p;
	P mod;

	p = (DP)ARG0(arg); mod = (P)ARG1(arg);
	asir_assert(p,O_DP,"dp_red_coef");
	asir_assert(mod,O_P,"dp_red_coef");
	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			divsrp(CO,m->c,mod,&q,&r);
			if ( r ) {
				NEXTMP(mr0,mr); mr->c = r; mr->dl = m->dl;
			}
		}
		if ( mr0 ) {
			NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
		} else
			*rp = 0;
	}
}

void Pdp_redble(arg,rp)
NODE arg;
Q *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_redble");
	asir_assert(ARG1(arg),O_DP,"dp_redble");
	if ( dp_redble((DP)ARG0(arg),(DP)ARG1(arg)) )
		*rp = ONE;
	else
		*rp = 0;
}

void Pdp_red_mod(arg,rp)
NODE arg;
LIST *rp;
{
	DP h,r;
	P dmy;
	NODE n;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_DP,"dp_red_mod");
	asir_assert(ARG1(arg),O_DP,"dp_red_mod");
	asir_assert(ARG2(arg),O_DP,"dp_red_mod");
	asir_assert(ARG3(arg),O_N,"dp_red_mod");
	dp_red_mod((DP)ARG0(arg),(DP)ARG1(arg),(DP)ARG2(arg),QTOS((Q)ARG3(arg)),
		&h,&r,&dmy);
	NEWNODE(n); BDY(n) = (pointer)h;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)r;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_subd(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_subd");
	asir_assert(p2,O_DP,"dp_subd");
	dp_subd(p1,p2,rp);
}

void Pdp_weyl_mul(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_weyl_mul"); asir_assert(p2,O_DP,"dp_mul");
	do_weyl = 1;
	muld(CO,p1,p2,rp);
	do_weyl = 0;
}

void Pdp_weyl_mul_mod(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;
	Q m;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg); m = (Q)ARG2(arg);
	asir_assert(p1,O_DP,"dp_weyl_mul_mod");
	asir_assert(p2,O_DP,"dp_mul_mod");
	asir_assert(m,O_N,"dp_mul_mod");
	do_weyl = 1;
	mulmd(CO,QTOS(m),p1,p2,rp);
	do_weyl = 0;
}

void Pdp_red(arg,rp)
NODE arg;
LIST *rp;
{
	NODE n;
	DP head,rest,dmy1;
	P dmy;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_DP,"dp_red");
	asir_assert(ARG1(arg),O_DP,"dp_red");
	asir_assert(ARG2(arg),O_DP,"dp_red");
	dp_red((DP)ARG0(arg),(DP)ARG1(arg),(DP)ARG2(arg),&head,&rest,&dmy,&dmy1);
	NEWNODE(n); BDY(n) = (pointer)head;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)rest;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_weyl_red(arg,rp)
NODE arg;
LIST *rp;
{
	NODE n;
	DP head,rest,dmy1;
	P dmy;

	asir_assert(ARG0(arg),O_DP,"dp_weyl_red");
	asir_assert(ARG1(arg),O_DP,"dp_weyl_red");
	asir_assert(ARG2(arg),O_DP,"dp_weyl_red");
	do_weyl = 1;
	dp_red((DP)ARG0(arg),(DP)ARG1(arg),(DP)ARG2(arg),&head,&rest,&dmy,&dmy1);
	do_weyl = 0;
	NEWNODE(n); BDY(n) = (pointer)head;
	NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)rest;
	NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_sp(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;

	do_weyl = 0;
	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_sp"); asir_assert(p2,O_DP,"dp_sp");
	dp_sp(p1,p2,rp);
}

void Pdp_weyl_sp(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_weyl_sp"); asir_assert(p2,O_DP,"dp_sp");
	do_weyl = 1;
	dp_sp(p1,p2,rp);
	do_weyl = 0;
}

void Pdp_sp_mod(arg,rp)
NODE arg;
DP *rp;
{
	DP p1,p2;
	int mod;

	do_weyl = 0;
	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_sp_mod"); asir_assert(p2,O_DP,"dp_sp_mod");
	asir_assert(ARG2(arg),O_N,"dp_sp_mod");
	mod = QTOS((Q)ARG2(arg));
	dp_sp_mod(p1,p2,mod,rp);
}

void Pdp_lcm(arg,rp)
NODE arg;
DP *rp;
{
	int i,n,td;
	DL d1,d2,d;
	MP m;
	DP p1,p2;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_lcm"); asir_assert(p2,O_DP,"dp_lcm");
	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	NEWDL(d,n);
	for ( i = 0, td = 0; i < n; i++ ) {
		d->d[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(d->d[i],i);
	}
	d->td = td;
	NEWMP(m); m->dl = d; m->c = (P)ONE; NEXT(m) = 0;
	MKDP(n,m,*rp); (*rp)->sugar = td;	/* XXX */
}

void Pdp_hm(arg,rp)
NODE arg;
DP *rp;
{
	DP p;

	p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_hm");
	dp_hm(p,rp);
}

void Pdp_ht(arg,rp)
NODE arg;
DP *rp;
{
	DP p;
	MP m,mr;

	p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_ht");
	if ( !p )
		*rp = 0;
	else {
		m = BDY(p);
		NEWMP(mr); mr->dl = m->dl; mr->c = (P)ONE; NEXT(mr) = 0;
		MKDP(p->nv,mr,*rp); (*rp)->sugar = mr->dl->td;	/* XXX */
	}
}

void Pdp_hc(arg,rp)
NODE arg;
P *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_hc");
	if ( !ARG0(arg) )
		*rp = 0;
	else
		*rp = BDY((DP)ARG0(arg))->c;
}

void Pdp_rest(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_rest");
	if ( !ARG0(arg) )
		*rp = 0;
	else
		dp_rest((DP)ARG0(arg),rp);
}

void Pdp_td(arg,rp)
NODE arg;
Q *rp;
{
	DP p;

	p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_td");
	if ( !p )
		*rp = 0;
	else
		STOQ(BDY(p)->dl->td,*rp);
}

void Pdp_sugar(arg,rp)
NODE arg;
Q *rp;
{
	DP p;

	p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_sugar");
	if ( !p )
		*rp = 0;
	else
		STOQ(p->sugar,*rp);
}

void Pdp_cri1(arg,rp)
NODE arg;
Q *rp;
{
	DP p1,p2;
	int *d1,*d2;
	int i,n;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_cri1"); asir_assert(p2,O_DP,"dp_cri1");
	n = p1->nv; d1 = BDY(p1)->dl->d; d2 = BDY(p2)->dl->d;
	for ( i = 0; i < n; i++ )
		if ( d1[i] > d2[i] )
			break;
	*rp = i == n ? ONE : 0;
}

void Pdp_cri2(arg,rp)
NODE arg;
Q *rp;
{
	DP p1,p2;
	int *d1,*d2;
	int i,n;

	p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
	asir_assert(p1,O_DP,"dp_cri2"); asir_assert(p2,O_DP,"dp_cri2");
	n = p1->nv; d1 = BDY(p1)->dl->d; d2 = BDY(p2)->dl->d;
	for ( i = 0; i < n; i++ )
		if ( MIN(d1[i],d2[i]) >= 1 )
			break;
	*rp = i == n ? ONE : 0;
}

void Pdp_minp(arg,rp)
NODE arg;
LIST *rp;
{
	NODE tn,tn1,d,dd,dd0,p,tp;
	LIST l,minp;
	DP lcm,tlcm;
	int s,ts;

	asir_assert(ARG0(arg),O_LIST,"dp_minp");
	d = BDY((LIST)ARG0(arg)); minp = (LIST)BDY(d);
	p = BDY(minp); p = NEXT(NEXT(p)); lcm = (DP)BDY(p); p = NEXT(p);
	if ( !ARG1(arg) ) {
		s = QTOS((Q)BDY(p)); p = NEXT(p);
		for ( dd0 = 0, d = NEXT(d); d; d = NEXT(d) ) {
			tp = BDY((LIST)BDY(d)); tp = NEXT(NEXT(tp));
			tlcm = (DP)BDY(tp); tp = NEXT(tp);
			ts = QTOS((Q)BDY(tp)); tp = NEXT(tp);
			NEXTNODE(dd0,dd);
			if ( ts < s ) {
				BDY(dd) = (pointer)minp;
				minp = (LIST)BDY(d); lcm = tlcm; s = ts;
			} else if ( ts == s ) {
				if ( compd(CO,lcm,tlcm) > 0 ) {
					BDY(dd) = (pointer)minp;
					minp = (LIST)BDY(d); lcm = tlcm; s = ts;
				} else
					BDY(dd) = BDY(d);
			} else
				BDY(dd) = BDY(d);
		}
	} else {
		for ( dd0 = 0, d = NEXT(d); d; d = NEXT(d) ) {
			tp = BDY((LIST)BDY(d)); tp = NEXT(NEXT(tp));
			tlcm = (DP)BDY(tp);
			NEXTNODE(dd0,dd);
			if ( compd(CO,lcm,tlcm) > 0 ) {
				BDY(dd) = (pointer)minp; minp = (LIST)BDY(d); lcm = tlcm;
			} else
				BDY(dd) = BDY(d);
		}
	}
	if ( dd0 )
		NEXT(dd) = 0;
	MKLIST(l,dd0); MKNODE(tn,l,0); MKNODE(tn1,minp,tn); MKLIST(*rp,tn1);
}

void Pdp_criB(arg,rp)
NODE arg;
LIST *rp;
{
	NODE d,ij,dd,ddd;
	int i,j,s,n;
	DP *ps;
	DL ts,ti,tj,lij,tdl;

	asir_assert(ARG0(arg),O_LIST,"dp_criB"); d = BDY((LIST)ARG0(arg));
	asir_assert(ARG1(arg),O_N,"dp_criB"); s = QTOS((Q)ARG1(arg));
	asir_assert(ARG2(arg),O_VECT,"dp_criB"); ps = (DP *)BDY((VECT)ARG2(arg));
	if ( !d )
		*rp = (LIST)ARG0(arg);
	else {
		ts = BDY(ps[s])->dl;
		n = ps[s]->nv;
		NEWDL(tdl,n);
		for ( dd = 0; d; d = NEXT(d) ) {
			ij = BDY((LIST)BDY(d));
			i = QTOS((Q)BDY(ij)); ij = NEXT(ij);
			j = QTOS((Q)BDY(ij)); ij = NEXT(ij);
			lij = BDY((DP)BDY(ij))->dl;
			ti = BDY(ps[i])->dl; tj = BDY(ps[j])->dl;
			if ( lij->td != lcm_of_DL(n,lij,ts,tdl)->td
				|| !dl_equal(n,lij,tdl)
				|| (lij->td == lcm_of_DL(n,ti,ts,tdl)->td
					&& dl_equal(n,tdl,lij))
				|| (lij->td == lcm_of_DL(n,tj,ts,tdl)->td
					&& dl_equal(n,tdl,lij)) ) {
				MKNODE(ddd,BDY(d),dd);
				dd = ddd;
			}
		}
		MKLIST(*rp,dd);
	}
}

void Pdp_nelim(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"dp_nelim");
		dp_nelim = QTOS((Q)ARG0(arg));
	}
	STOQ(dp_nelim,*rp);
}

void Pdp_mag(arg,rp)
NODE arg;
Q *rp;
{
	DP p;
	int s;
	MP m;

	p = (DP)ARG0(arg);
	asir_assert(p,O_DP,"dp_mag");
	if ( !p )
		*rp = 0;
	else {
		for ( s = 0, m = BDY(p); m; m = NEXT(m) )
			s += p_mag(m->c);
		STOQ(s,*rp);
	}
}

extern int kara_mag;

void Pdp_set_kara(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"dp_set_kara");
		kara_mag = QTOS((Q)ARG0(arg));
	}
	STOQ(kara_mag,*rp);
}

void Pdp_homo(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_homo");
	dp_homo((DP)ARG0(arg),rp);
}

void Pdp_dehomo(arg,rp)
NODE arg;
DP *rp;
{
	asir_assert(ARG0(arg),O_DP,"dp_dehomo");
	dp_dehomo((DP)ARG0(arg),rp);
}

void Pdp_gr_flags(arg,rp)
NODE arg;
LIST *rp;
{
	Obj name,value;
	NODE n;

	if ( arg ) {
		asir_assert(ARG0(arg),O_LIST,"dp_gr_flags");
		n = BDY((LIST)ARG0(arg));
		while ( n ) {
			name = (Obj)BDY(n); n = NEXT(n);
			if ( !n )
				break;
			else {
				value = (Obj)BDY(n); n = NEXT(n);
			}
			dp_set_flag(name,value);
		}
	}
	dp_make_flaglist(rp);
}

extern int DP_Print;

void Pdp_gr_print(arg,rp)
NODE arg;
Q *rp;
{
	Q q;

	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"dp_gr_print");
		q = (Q)ARG0(arg); DP_Print = QTOS(q);
	} else
		STOQ(DP_Print,q);
	*rp = q;
}

void Pdp_gr_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	Num homo;
	Q m;
	int modular;
	struct order_spec ord;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_gr_main");
	asir_assert(ARG1(arg),O_LIST,"dp_gr_main");
	asir_assert(ARG2(arg),O_N,"dp_gr_main");
	asir_assert(ARG3(arg),O_N,"dp_gr_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	homo = (Num)ARG2(arg);
	m = (Q)ARG3(arg);
	if ( !m )
		modular = 0;
	else if ( PL(NM(m))>1 || (PL(NM(m)) == 1 && BD(NM(m))[0] >= 0x80000000) )
		error("dp_gr_main : too large modulus");
	else
		modular = QTOS(m);
	create_order_spec(ARG4(arg),&ord);
	dp_gr_main(f,v,homo,modular,0,&ord,rp);
}

void Pdp_gr_f_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	Num homo;
	struct order_spec ord;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_gr_f_main");
	asir_assert(ARG1(arg),O_LIST,"dp_gr_f_main");
	asir_assert(ARG2(arg),O_N,"dp_gr_f_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	homo = (Num)ARG2(arg);
	create_order_spec(ARG3(arg),&ord);
	dp_gr_main(f,v,homo,0,1,&ord,rp);
}

void Pdp_f4_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	struct order_spec ord;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_f4_main");
	asir_assert(ARG1(arg),O_LIST,"dp_f4_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	create_order_spec(ARG2(arg),&ord);
	dp_f4_main(f,v,&ord,rp);
}

/* dp_gr_checklist(list of dp) */

void Pdp_gr_checklist(arg,rp)
NODE arg;
LIST *rp;
{
	VECT g;
	LIST dp;
	NODE r;
	int n;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_gr_checklist");
	asir_assert(ARG1(arg),O_N,"dp_gr_checklist");
	n = QTOS((Q)ARG1(arg));
	gbcheck_list(BDY((LIST)ARG0(arg)),n,&g,&dp);
	r = mknode(2,g,dp);
	MKLIST(*rp,r);
}

void Pdp_f4_mod_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	int m;
	struct order_spec ord;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_f4_mod_main");
	asir_assert(ARG1(arg),O_LIST,"dp_f4_mod_main");
	asir_assert(ARG2(arg),O_N,"dp_f4_mod_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg); m = QTOS((Q)ARG2(arg));
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	if ( !m )
		error("dp_f4_mod_main : invalid argument");
	create_order_spec(ARG3(arg),&ord);
	dp_f4_mod_main(f,v,m,&ord,rp);
}

void Pdp_gr_mod_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	Num homo;
	int m;
	struct order_spec ord;

	do_weyl = 0;
	asir_assert(ARG0(arg),O_LIST,"dp_gr_mod_main");
	asir_assert(ARG1(arg),O_LIST,"dp_gr_mod_main");
	asir_assert(ARG2(arg),O_N,"dp_gr_mod_main");
	asir_assert(ARG3(arg),O_N,"dp_gr_mod_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	homo = (Num)ARG2(arg); m = QTOS((Q)ARG3(arg));
	if ( !m )
		error("dp_gr_mod_main : invalid argument");
	create_order_spec(ARG4(arg),&ord);
	dp_gr_mod_main(f,v,homo,m,&ord,rp);
}

/* for Weyl algebra */

void Pdp_weyl_gr_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	Num homo;
	Q m;
	int modular;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_weyl_gr_main");
	asir_assert(ARG1(arg),O_LIST,"dp_weyl_gr_main");
	asir_assert(ARG2(arg),O_N,"dp_weyl_gr_main");
	asir_assert(ARG3(arg),O_N,"dp_weyl_gr_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	homo = (Num)ARG2(arg);
	m = (Q)ARG3(arg);
	if ( !m )
		modular = 0;
	else if ( PL(NM(m))>1 || (PL(NM(m)) == 1 && BD(NM(m))[0] >= 0x80000000) )
		error("dp_gr_main : too large modulus");
	else
		modular = QTOS(m);
	create_order_spec(ARG4(arg),&ord);
	do_weyl = 1;
	dp_gr_main(f,v,homo,modular,0,&ord,rp);
	do_weyl = 0;
}

void Pdp_weyl_gr_f_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	Num homo;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_weyl_gr_main");
	asir_assert(ARG1(arg),O_LIST,"dp_weyl_gr_main");
	asir_assert(ARG2(arg),O_N,"dp_weyl_gr_main");
	asir_assert(ARG3(arg),O_N,"dp_weyl_gr_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	homo = (Num)ARG2(arg);
	create_order_spec(ARG3(arg),&ord);
	do_weyl = 1;
	dp_gr_main(f,v,homo,0,1,&ord,rp);
	do_weyl = 0;
}

void Pdp_weyl_f4_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_weyl_f4_main");
	asir_assert(ARG1(arg),O_LIST,"dp_weyl_f4_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	create_order_spec(ARG2(arg),&ord);
	do_weyl = 1;
	dp_f4_main(f,v,&ord,rp);
	do_weyl = 0;
}

void Pdp_weyl_f4_mod_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	int m;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_weyl_f4_main");
	asir_assert(ARG1(arg),O_LIST,"dp_weyl_f4_main");
	asir_assert(ARG2(arg),O_N,"dp_f4_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg); m = QTOS((Q)ARG2(arg));
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	if ( !m )
		error("dp_weyl_f4_mod_main : invalid argument");
	create_order_spec(ARG3(arg),&ord);
	do_weyl = 1;
	dp_f4_mod_main(f,v,m,&ord,rp);
	do_weyl = 0;
}

void Pdp_weyl_gr_mod_main(arg,rp)
NODE arg;
LIST *rp;
{
	LIST f,v;
	Num homo;
	int m;
	struct order_spec ord;

	asir_assert(ARG0(arg),O_LIST,"dp_weyl_gr_mod_main");
	asir_assert(ARG1(arg),O_LIST,"dp_weyl_gr_mod_main");
	asir_assert(ARG2(arg),O_N,"dp_weyl_gr_mod_main");
	asir_assert(ARG3(arg),O_N,"dp_weyl_gr_mod_main");
	f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
	f = remove_zero_from_list(f);
	if ( !BDY(f) ) {
		*rp = f; return;
	}
	homo = (Num)ARG2(arg); m = QTOS((Q)ARG3(arg));
	if ( !m )
		error("dp_weyl_gr_mod_main : invalid argument");
	create_order_spec(ARG4(arg),&ord);
	do_weyl = 1;
	dp_gr_mod_main(f,v,homo,m,&ord,rp);
	do_weyl = 0;
}

static VECT current_dl_weight_vector_obj;
int *current_dl_weight_vector;

void Pdp_set_weight(arg,rp)
NODE arg;
VECT *rp;
{
	VECT v;
	int i,n;

	if ( !arg )
		*rp = current_dl_weight_vector_obj;
	else if ( !ARG0(arg) ) {
		current_dl_weight_vector_obj = 0;
		current_dl_weight_vector = 0;
		*rp = 0;
	} else {
		asir_assert(ARG0(arg),O_VECT,"dp_set_weight");
		v = (VECT)ARG0(arg);
		current_dl_weight_vector_obj = v;
		n = v->len;
		current_dl_weight_vector = (int *)CALLOC(n,sizeof(int));
		for ( i = 0; i < n; i++ )
			current_dl_weight_vector[i] = QTOS((Q)v->body[i]);
		*rp = v;
	}
}

static VECT current_weyl_weight_vector_obj;
int *current_weyl_weight_vector;

void Pdp_weyl_set_weight(arg,rp)
NODE arg;
VECT *rp;
{
	VECT v;
	int i,n;

	if ( !arg )
		*rp = current_weyl_weight_vector_obj;
	else {
		asir_assert(ARG0(arg),O_VECT,"dp_weyl_set_weight");
		v = (VECT)ARG0(arg);
		current_weyl_weight_vector_obj = v;
		n = v->len;
		current_weyl_weight_vector = (int *)CALLOC(n,sizeof(int));
		for ( i = 0; i < n; i++ )
			current_weyl_weight_vector[i] = QTOS((Q)v->body[i]);
		*rp = v;
	}
}

LIST remove_zero_from_list(LIST l)
{
	NODE n,r0,r;
	LIST rl;

	asir_assert(l,O_LIST,"remove_zero_from_list");
	n = BDY(l);
	for ( r0 = 0; n; n = NEXT(n) )
		if ( BDY(n) ) {
			NEXTNODE(r0,r);
			BDY(r) = BDY(n);
		}
	if ( r0 )
		NEXT(r) = 0;
	MKLIST(rl,r0);
	return rl;
}
