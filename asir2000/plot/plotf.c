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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/plotf.c,v 1.20 2006/11/15 01:28:44 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

void Pifplot(), Pconplot(), Pplotover(), Pplot(), Parrayplot(), Pdrawcircle();
void Ppolarplot();
void Pmemory_ifplot(),Pmemory_conplot(),Pmemory_plot();
void Popen_canvas(), Pclear_canvas(), Pdraw_obj(), Pdraw_string();
void Pox_rpc();
void Pox_cmo_rpc();

struct ftab plot_tab[] = {
	{"ifplot",Pifplot,-7},
	{"memory_ifplot",Pmemory_ifplot,-6},
	{"conplot",Pconplot,-8},
	{"memory_conplot",Pmemory_conplot,-7},
	{"plot",Pplot,-6},
	{"memory_plot",Pmemory_plot,-5},
	{"polarplot",Ppolarplot,-6},
	{"plotover",Pplotover,-4},
	{"drawcircle",Pdrawcircle,6},
	{"open_canvas",Popen_canvas,-3},
	{"clear_canvas",Pclear_canvas,2},
	{"draw_obj",Pdraw_obj,-4},
	{"draw_string",Pdraw_string,-5},
/*
	{"arrayplot",Parrayplot,2},
*/
	{0,0,0},
};

void Popen_canvas(NODE arg,Q *rp)
{
	Q w300,s_id;
	LIST geom;
	int stream;
	NODE n,n0;
	STRING fname,wname;

	geom = 0; wname = 0; stream = -1;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_LIST:
				geom = (LIST)BDY(arg);
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("open_canvas : invalid argument"); break;
		}
	/* open_canvas in ox_plot requires 
	   [s_id (Q),
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/

	stream = validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	MKSTR(fname,"open_canvas");
	arg = mknode(4,s_id,fname,geom,wname);
	Pox_cmo_rpc(arg,rp);
	*rp = s_id;
}

void ifplot_main(NODE arg,int is_memory, Obj *rp);

void Pifplot(NODE arg,Obj *rp)
{
	ifplot_main(arg,0,rp);
}

void Pmemory_ifplot(NODE arg,Obj *rp)
{
	ifplot_main(arg,1,rp);
}

void ifplot_main(NODE arg,int is_memory, Obj *rp)
{
	Q m2,p2,w300,s_id;
	NODE defrange;
	LIST xrange,yrange,range[2],list,geom;
	VL vl,vl0;
	V v[2],av[2];
	int stream,ri,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;
	Obj t;

	STOQ(-2,m2); STOQ(2,p2);
	MKNODE(n,p2,0); MKNODE(defrange,m2,n); 
	poly = 0; vl = 0; geom = 0; wname = 0; stream = -1; ri = 0;
	v[0] = v[1] = 0;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_P:
				poly = (P)BDY(arg); 
				get_vars_recursive((Obj)poly,&vl);
				for ( vl0 = vl, i = 0; vl0; vl0 = NEXT(vl0) )
					if ( vl0->v->attr == (pointer)V_IND )
						if ( i >= 2 )
							error("ifplot : invalid argument");
						else
							v[i++] = vl0->v;
				break;
			case O_LIST:
				list = (LIST)BDY(arg);
				if ( OID(BDY(BDY(list))) == O_P )
					if ( ri > 1 )
						error("ifplot : invalid argument");
					else
						range[ri++] = list;
				else
					geom = list;
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("ifplot : invalid argument"); break;
		}
	if ( !poly )
		error("ifplot : invalid argument");
	switch ( ri ) {
		case 0:
			if ( !v[1] )
				error("ifplot : please specify all variables");
			MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
			MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
			break;
		case 1:
			if ( !v[1] )
				error("ifplot : please specify all variables");
			av[0] = VR((P)BDY(BDY(range[0])));
			if ( v[0] == av[0] ) {
				xrange = range[0];
				MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
			} else if ( v[1] == av[0] ) {
				MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
				yrange = range[0];
			} else
				error("ifplot : invalid argument");
			break;
		case 2:
			av[0] = VR((P)BDY(BDY(range[0])));
			av[1] = VR((P)BDY(BDY(range[1])));
			if ( ((v[0] == av[0]) && (!v[1] || v[1] == av[1])) ||
				 ((v[0] == av[1]) && (!v[1] || v[1] == av[0])) ) {
					xrange = range[0]; yrange = range[1];
			} else
					error("ifplot : invalid argument");
			break;
		default:
			error("ifplot : cannot happen"); break;
	}
	/* ifplot in ox_plot requires 
	   [s_id (Q),
	   	formula (Obj),
	   	xrange=[x,xmin,xmax] (LIST),
	   	yrange=[y,ymin,ymax] (LIST),
	   	zrange=0,
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/

	stream = validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	if ( is_memory ) {
		MKSTR(fname,"memory_plot");
		arg = mknode(7,s_id,fname,poly,xrange,yrange,NULLP,geom);
		Pox_rpc(arg,&t);
		arg = mknode(1,s_id);
		Pox_pop_cmo(arg,rp);
	} else {
		MKSTR(fname,"plot");
		arg = mknode(8,s_id,fname,poly,xrange,yrange,NULLP,geom,wname);
		Pox_rpc(arg,&t);
		*rp = (Obj)s_id;
	}
}

void conplot_main(NODE arg,int is_memory, Obj *rp);

void Pconplot(NODE arg,Obj *rp)
{
	conplot_main(arg,0,rp);
}

void Pmemory_conplot(NODE arg,Obj *rp)
{
	conplot_main(arg,1,rp);
}

void conplot_main(NODE arg,int is_memory,Obj *rp)
{
	Q m2,p2,w300,s_id;
	NODE defrange;
	LIST xrange,yrange,zrange,range[3],list,geom;
	VL vl,vl0;
	V v[2],av[2];
	int stream,ri,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;
	Obj t;

	STOQ(-2,m2); STOQ(2,p2);
	MKNODE(n,p2,0); MKNODE(defrange,m2,n); 
	poly = 0; vl = 0; geom = 0; wname = 0; stream = -1; ri = 0;
	v[0] = v[1] = 0;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_P:
				poly = (P)BDY(arg);
				get_vars_recursive((Obj)poly,&vl);
				for ( vl0 = vl, i = 0; vl0; vl0 = NEXT(vl0) )
					if ( vl0->v->attr == (pointer)V_IND )
						if ( i >= 2 )
							error("ifplot : invalid argument");
						else
							v[i++] = vl0->v;
				break;
			case O_LIST:
				list = (LIST)BDY(arg);
				if ( OID(BDY(BDY(list))) == O_P )
					if ( ri > 2 )
						error("ifplot : invalid argument");
					else
						range[ri++] = list;
				else
					geom = list;
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("ifplot : invalid argument"); break;
		}
	if ( !poly )
		error("ifplot : invalid argument");
	switch ( ri ) {
		case 0:
			if ( !v[1] )
				error("ifplot : please specify all variables");
			MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
			MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
			MKNODE(n,0,defrange); MKLIST(zrange,n);
			break;
		case 1:
			if ( !v[1] )
				error("ifplot : please specify all variables");
			av[0] = VR((P)BDY(BDY(range[0])));
			if ( v[0] == av[0] ) {
				xrange = range[0];
				MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
				MKNODE(n,0,defrange); MKLIST(zrange,n);
			} else if ( v[1] == av[0] ) {
				MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
				yrange = range[0];
				MKNODE(n,0,defrange); MKLIST(zrange,n);
			} else {
				MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
				MKV(v[1],var); MKNODE(n,var,defrange); MKLIST(yrange,n);
				zrange = range[0];
			}
			break;
		case 2: case 3:
			av[0] = VR((P)BDY(BDY(range[0])));
			av[1] = VR((P)BDY(BDY(range[1])));
			if ( ((v[0] == av[0]) && (!v[1] || v[1] == av[1])) ||
				 ((v[0] == av[1]) && (!v[1] || v[1] == av[0])) ) {
				xrange = range[0]; yrange = range[1];
				if ( ri == 3 )
					zrange = range[2];
				else {
					MKNODE(n,0,defrange); MKLIST(zrange,n);
				}
			} else
				error("ifplot : invalid argument");
			break;
		default:
			error("ifplot : cannot happen"); break;
	}
	/* conplot in ox_plot requires 
	   [s_id (Q),
	   	formula (Obj),
	   	xrange=[x,xmin,xmax] (LIST),
	   	yrange=[y,ymin,ymax] (LIST),
	   	zrange=[z,zmin,zmax] (LIST),
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/

	stream = validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	if ( is_memory ) {
		MKSTR(fname,"memory_plot");
		arg = mknode(7,s_id,fname,poly,xrange,yrange,zrange,geom);
		Pox_rpc(arg,&t);
		arg = mknode(1,s_id);
		Pox_pop_cmo(arg,rp);
	} else {
		MKSTR(fname,"plot");
		arg = mknode(8,s_id,fname,poly,xrange,yrange,zrange,geom,wname);
		Pox_rpc(arg,&t);
		*rp = (Obj)s_id;
	}
}

void plot_main(NODE arg,int is_memory,Obj *rp);

void Pplot(NODE arg,Obj *rp)
{
	plot_main(arg,0,rp);
}

void Pmemory_plot(NODE arg,Obj *rp)
{
	plot_main(arg,1,rp);
}

void plot_main(NODE arg,int is_memory,Obj *rp)
{
	Q m2,p2,w300,s_id;
	NODE defrange;
	LIST xrange,range[1],list,geom;
	VL vl,vl0;
	V v[1],av[1];
	int stream,ri,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;
	Obj t;

	STOQ(-2,m2); STOQ(2,p2);
	MKNODE(n,p2,0); MKNODE(defrange,m2,n); 
	poly = 0; vl = 0; geom = 0; wname = 0; stream = -1; ri = 0;
	v[0] = 0;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_P: case O_R:
				poly = (P)BDY(arg);
				get_vars_recursive((Obj)poly,&vl);
				for ( vl0 = vl, i = 0; vl0; vl0 = NEXT(vl0) )
					if ( vl0->v->attr == (pointer)V_IND )
						if ( i >= 1 )
							error("ifplot : invalid argument");
						else
							v[i++] = vl0->v;
				if ( i != 1 )
					error("ifplot : invalid argument");
				break;
			case O_LIST:
				list = (LIST)BDY(arg);
				if ( OID(BDY(BDY(list))) == O_P )
					if ( ri > 0 )
						error("plot : invalid argument");
					else
						range[ri++] = list;
				else
					geom = list;
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("plot : invalid argument"); break;
		}
	if ( !poly )
		error("plot : invalid argument");
	switch ( ri ) {
		case 0:
			MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(xrange,n);
			break;
		case 1:
			av[0] = VR((P)BDY(BDY(range[0])));
			if ( v[0] == av[0] )
				xrange = range[0];
			else
				error("plot : invalid argument");
			break;
		default:
			error("plot : cannot happen"); break;
	}
	/* conplot in ox_plot requires 
	   [s_id (Q),
	   	formula (Obj),
	   	xrange=[x,xmin,xmax] (LIST),
	   	yrange=0,
	   	zrange=0,
	   	geom=[xsize,ysize] (LIST),
	   	wname=name (STRING)]
	*/
	stream = validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	if ( is_memory ) {
		MKSTR(fname,"memory_plot");
		arg = mknode(7,s_id,fname,poly,xrange,NULLP,NULLP,geom);
		Pox_rpc(arg,&t);
		arg = mknode(1,s_id);
		Pox_pop_cmo(arg,rp);
	} else {
		MKSTR(fname,"plot");
		arg = mknode(8,s_id,fname,poly,xrange,NULLP,NULLP,geom,wname);
		Pox_rpc(arg,&t);
		*rp = (Obj)s_id;
	}
}

#define Pi 3.14159265358979323846264

void Ppolarplot(NODE arg,Q *rp)
{
	Q m2,p2,w300,s_id;
	NODE defrange;
	LIST zrange,range[1],list,geom;
	VL vl,vl0;
	V v[1],av[1];
	int stream,ri,i;
	P poly;
	P var;
	NODE n,n0;
	STRING fname,wname;
	Real pi2;
	Obj t;

	MKReal(2*Pi,pi2);
	MKNODE(n,pi2,0); MKNODE(defrange,0,n); 
	poly = 0; vl = 0; geom = 0; wname = 0; stream = -1; ri = 0;
	v[0] = 0;
	for ( ; arg; arg = NEXT(arg) )
		if ( !BDY(arg) )
			stream = 0;
		else
		switch ( OID(BDY(arg)) ) {
			case O_P: case O_R:
				poly = (P)BDY(arg);
				get_vars_recursive((Obj)poly,&vl);
				for ( vl0 = vl, i = 0; vl0; vl0 = NEXT(vl0) )
					if ( vl0->v->attr == (pointer)V_IND )
						if ( i >= 1 )
							error("polarplot : invalid argument");
						else
							v[i++] = vl0->v;
				if ( i != 1 )
					error("polarplot : invalid argument");
				break;
			case O_LIST:
				list = (LIST)BDY(arg);
				if ( OID(BDY(BDY(list))) == O_P )
					if ( ri > 0 )
						error("polarplot : invalid argument");
					else
						range[ri++] = list;
				else
					geom = list;
				break;
			case O_N:
				stream = QTOS((Q)BDY(arg)); break;
			case O_STR:
				wname = (STRING)BDY(arg); break;
			default:
				error("polarplot : invalid argument"); break;
		}
	if ( !poly )
		error("polarplot : invalid argument");
	switch ( ri ) {
		case 0:
			MKV(v[0],var); MKNODE(n,var,defrange); MKLIST(zrange,n);
			break;
		case 1:
			av[0] = VR((P)BDY(BDY(range[0])));
			if ( v[0] == av[0] )
				zrange = range[0];
			else
				error("polarplot : invalid argument");
			break;
		default:
			error("polarplot : cannot happen"); break;
	}
	stream = validate_ox_plot_stream(stream);
	STOQ(stream,s_id);
	if ( !geom ) {
		STOQ(300,w300);
		MKNODE(n0,w300,0); MKNODE(n,w300,n0); MKLIST(geom,n);
	}
	MKSTR(fname,"plot");
	arg = mknode(8,s_id,fname,poly,NULLP,NULLP,zrange,geom,wname);
	Pox_rpc(arg,&t);
	*rp = s_id;
}

void Pplotover(NODE arg,Q *rp)
{
	Q s_id,index;
	P poly;
	STRING fname;
	Obj t;

	poly = (P)ARG0(arg);
	s_id = (Q)ARG1(arg);
	index = (Q)ARG2(arg);
	MKSTR(fname,"plotover");
	if ( argc(arg) == 4 )
		arg = mknode(5,s_id,fname,index,poly,(Q)ARG3(arg));
	else
		arg = mknode(4,s_id,fname,index,poly);
	Pox_rpc(arg,&t);
	*rp = s_id;
}

/* arg = [x,y,r,c,s_id,index] */

void Pdrawcircle(NODE arg,Q *rp)
{
	Q s_id,index;
	Obj x,y,r,c;
	STRING fname;
	NODE n;
	LIST pos;
	Obj t;

	x = (Obj)ARG0(arg);
	y = (Obj)ARG1(arg);
	r = (Obj)ARG2(arg);
	c = (Obj)ARG3(arg);
	s_id = (Q)ARG4(arg);
	index = (Q)ARG5(arg);
	MKSTR(fname,"drawcircle");
	n = mknode(3,x,y,r); MKLIST(pos,n);
	arg = mknode(5,s_id,fname,index,pos,c);
	Pox_rpc(arg,&t);
	*rp = s_id;
}

/* draw_obj(s_id,cindex,point|line); point = [x,y], line = [xa,ya,xb,yb] */
void Pdraw_obj(NODE arg,Q *rp)
{
	static STRING fname;
	Q s_id,index;
	LIST obj;
	Obj t;

	if ( !fname ) {
		MKSTR(fname,"draw_obj");
	}
	s_id = (Q)ARG0(arg);
	index = (Q)ARG1(arg);
	obj = (LIST)ARG2(arg);
	/* ARG3(arg) = color */
	if ( argc(arg) == 4 )
		arg = mknode(5,s_id,fname,index,obj,ARG3(arg));
	else
		arg = mknode(4,s_id,fname,index,obj);
	Pox_cmo_rpc(arg,&t);
	*rp = s_id;
}

/* draw_string(s_id,cindex,pos,string,[,color]); pos=[x,y] */
void Pdraw_string(NODE arg,Q *rp)
{
	static STRING fname;
	STRING str;
	Q s_id,index;
	LIST pos;
	Obj t;

	if ( !fname ) {
		MKSTR(fname,"draw_string");
	}
	s_id = (Q)ARG0(arg);
	index = (Q)ARG1(arg);
	pos = (LIST)ARG2(arg);
	str = (STRING)ARG3(arg);
	/* ARG4(arg) = color */
	if ( argc(arg) == 5 )
		arg = mknode(6,s_id,fname,index,pos,str,ARG4(arg));
	else
		arg = mknode(5,s_id,fname,index,pos,str);
	Pox_cmo_rpc(arg,&t);
	*rp = s_id;
}

void Pclear_canvas(NODE arg,Q *rp)
{
	static STRING fname;
	Q s_id,index;
	Obj t;

	if ( !fname ) {
		MKSTR(fname,"clear_canvas");
	}
	s_id = (Q)ARG0(arg);
	index = (Q)ARG1(arg);
	arg = mknode(3,s_id,fname,index);
	Pox_cmo_rpc(arg,&t);
	*rp = s_id;
}

#if 0
void Parrayplot(NODE arg,Obj *rp)
{
	int s;
	int id;

	if ( ID((Obj)ARG0(arg)) == O_VECT && ID((Obj)ARG1(arg)) == O_LIST ) {
		s = current_s;
		gensend(s,C_APLOT,0);
		gensend(s,C_OBJ,ARG0(arg)); gensend(s,C_OBJ,ARG1(arg)); genflush(s);
	}
	genrecv(s,&id,rp);
}
#endif
