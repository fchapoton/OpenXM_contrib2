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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/struct.c,v 1.3 2000/08/22 05:04:28 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "comp.h"

struct oSS oLSS;
SS LSS = &oLSS;

int structdef(name,member)
char *name;
NODE member;
{
	int i,j,k,type;
	SDEF sdef,s;
	NODE n,ms;
	char *mname,*sname;

	/* search the predefined structure */
	for ( s = LSS->sa, i = 0; i < LSS->n; i++ )
		if ( !strcmp(s[i].name,name) ) {
			fprintf(stderr,"redeclaration of %s\n",name); break;
		}
	if ( !LSS->sa || ((i == LSS->n)&&(LSS->n==LSS->asize)) )
		reallocarray((char **)&LSS->sa,(int *)&LSS->asize,(int *)&LSS->n,(int)sizeof(struct oSDEF));
	/* sdef = room for new structure definition */
	sdef = &LSS->sa[i];
	if ( i == LSS->n )
		LSS->n++;
	/* set the name of structure */
	sdef->name = (char *)MALLOC(strlen(name)+1);
	bcopy(name,sdef->name,strlen(name)+1);
	/* count the total number of members */
	for ( j = 0, n = member; n; n = NEXT(n), j++ );
	sdef->n = j;
	sdef->member = (char **)MALLOC(sdef->n*sizeof(char *));
	/* set the names of members */
	for ( j = 0, n = member; n; n = NEXT(n), j++ ) {
		mname = (char *)MALLOC(strlen((char *)BDY(n))+1);	
		bcopy((char *)BDY(n),mname,strlen((char *)BDY(n))+1);
		sdef->member[j] = mname;
	}
	return i;
}

void newstruct(type,rp)
int type;
COMP *rp;
{
	NEWCOMP(*rp,LSS->sa[type].n); (*rp)->type = type;
}

int structtoindex(name)
char *name;
{
	SDEF s;
	int i;

	for ( s = LSS->sa, i = 0; i < LSS->n; i++ )
		if ( !strcmp(s[i].name,name) )
			break;
	if ( i == LSS->n ) {
		fprintf(stderr,"%s: undefined structure\n",name); return -1; /* XXX */
	} else
		return i;
}

int membertoindex(type,name)
int type;
char *name;
{
	SDEF s;
	char **member;
	int i;

	s = &LSS->sa[type];
	for ( member = s->member, i = 0; i < s->n; i++ )
		if ( !strcmp(member[i],name) )
			break;
	if ( i == s->n )
		return -1;
	else
		return i;
}

int getcompsize(type)
int type;
{
	return LSS->sa[type].n;
}

#if 0
void getmember(expr,memp)
FNODE expr;
Obj *memp;
{
	int i;
	FNODE root;
	COMP t;
	PV v0;
	NODE2 mchain;

	root = (FNODE)FA1(expr); mchain = (NODE2)FA2(expr);
	if ( ID(root) == I_PVAR ) {
		i = (int)FA0(root);
		v0 = i>=0?&CPVS->va[(unsigned int)i]:&GPVS->va[((unsigned int)i)&~MSB];
		t = (COMP)v0->priv;
	} else
		t = (COMP)eval(root);
	for ( ; mchain; mchain = NEXT(mchain) )
		t = (COMP)(t->member[(int)BDY1(mchain)]);
	*memp = (Obj)t;
}

void getmemberp(expr,addrp)
FNODE expr;
Obj **addrp;
{
	int i;
	FNODE root;
	COMP t;
	PV v0;
	COMP *addr;
	NODE2 mchain;

	root = (FNODE)FA1(expr); mchain = (NODE2)FA2(expr);
	if ( ID(root) == I_PVAR ) {
		i = (int)FA0(root);
		v0 = i>=0?&CPVS->va[(unsigned int)i]:&GPVS->va[((unsigned int)i)&~MSB];
		addr = (COMP *)&v0->priv;
	} else {
		t = (COMP)eval(root);
		addr = (COMP *)&t;
	}
	for ( ; mchain; mchain = NEXT(mchain) )
		addr = (COMP *)&((*addr)->member[(int)BDY1(mchain)]);
	*addrp = (Obj *)addr;
}

void getarrayp(a,ind,addrp)
Obj a;
NODE ind;
Obj **addrp;
{
	Obj len,row,col;
	Obj *addr;

	switch ( OID(a) ) {
		case O_VECT:
			len = (Obj)BDY(ind);
			if ( !rangecheck(len,((VECT)a)->len) )
				error("getarrayp : Out of range");
			else
				addr = (Obj *)&(BDY((VECT)a)[QTOS((Q)len)]);
			break;
		case O_MAT:
			row = (Obj)BDY(ind); col = (Obj)BDY(NEXT(ind));
			if ( !rangecheck(row,((MAT)a)->row) 
				|| !rangecheck(col,((MAT)a)->col) )
				error("getarrayp : Out of range");
			else
				addr = (Obj *)&(BDY((MAT)a)[QTOS((Q)row)][QTOS((Q)col)]);
			break;
		default:
				addr = 0;
			break;
	}
	*addrp = addr;
}
#endif

Obj memberofstruct(a,name)
COMP a;
char *name;
{
	NODE2 n,*np;
	int type,ind;
	char buf[BUFSIZ];

	if ( !a || OID(a) != O_COMP )
		error("object is not a structure");
	type = a->type;
	ind = membertoindex(type,name);
	if ( ind < 0 ) {
		sprintf(buf,"structure has no member named `%s'",name);
		error(buf);
	}
	return (Obj)a->member[ind];
}

void assign_to_member(a,name,obj)
COMP a;
char *name;
Obj obj;
{
	NODE2 n,*np;
	int type,ind;
	char buf[BUFSIZ];

	if ( !a || OID(a) != O_COMP )
		error("object is not a structure");
	type = a->type;
	ind = membertoindex(type,name);
	if ( ind < 0 ) {
		sprintf(buf,"structure has no member named `%s'",name);
		error(buf);
	}
	a->member[ind] = obj;
}
