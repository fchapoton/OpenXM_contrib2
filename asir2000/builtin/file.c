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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/file.c,v 1.7 2000/11/08 06:21:17 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "base.h"
#include "unistd.h"
#if PARI
#include "genpari.h"
#endif

#if defined(VISUAL)
#include <windows.h>
/* #define ECGEN_KEYNAME "SoftWare\\Fujitsu\\WinECgen\\1.00.000" */
#define ECGEN_KEYNAME "SoftWare\\Fujitsu\\FSEcParamGen\\V1.0L10"
#define ASIR_KEYNAME "SoftWare\\Fujitsu\\Asir\\1999.03.31"
#endif

void Pget_rootdir();
void Paccess(),Premove_file();
void Pbsave_enc(), Pbload_enc();

void Pload(), Pwhich(), Ploadfiles(), Poutput();
void Pbsave(), Pbload(), Pbload27();
void Pbsave_compat(), Pbload_compat();
void Pbsave_cmo(), Pbload_cmo();
void Popen_file(), Pclose_file(), Pget_line(), Pget_byte();

extern int des_encryption;
extern char *asir_libdir;

struct ftab file_tab[] = {
	{"open_file",Popen_file,1},
	{"close_file",Pclose_file,1},
	{"get_byte",Pget_byte,1},
	{"get_line",Pget_line,1},
	{"remove_file",Premove_file,1},
	{"access",Paccess,1},
	{"load",Pload,-1},
	{"which",Pwhich,1},
	{"loadfiles",Ploadfiles,1},
	{"output",Poutput,-1},
	{"bsave",Pbsave,2},
	{"bload",Pbload,1},
	{"get_rootdir",Pget_rootdir,0},
#if defined(VISUAL) && defined(DES_ENC)
	{"bsave_enc",Pbsave_enc,2},
	{"bload_enc",Pbload_enc,1},
#endif
	{"bload27",Pbload27,1},
	{"bsave_compat",Pbsave_compat,2},
	{"bload_compat",Pbload_compat,1},
	{"bsave_cmo",Pbsave_cmo,2},
	{"bload_cmo",Pbload_cmo,1},
	{0,0,0},
};

static FILE *file_ptrs[BUFSIZ];

void Popen_file(arg,rp)
NODE arg;
Q *rp;
{
	char *name;
	FILE *fp;
	char errbuf[BUFSIZ];
	int i;

	asir_assert(ARG0(arg),O_STR,"open_file");
	for ( i = 0; i < BUFSIZ && file_ptrs[i]; i++ );
	if ( i == BUFSIZ )
		error("open_file : too many open files");
	name = BDY((STRING)ARG0(arg));
	fp = fopen(name,"r");
	if ( !fp ) {
		sprintf(errbuf,"open_file : \"%s\" not found",name);
		error(errbuf);
	}
	file_ptrs[i] = fp;
	STOQ(i,*rp);
}

void Pclose_file(arg,rp)
NODE arg;
Q *rp;
{
	int i;

	asir_assert(ARG0(arg),O_N,"close_file");
	i = QTOS((Q)ARG0(arg));
	if ( file_ptrs[i] ) {
		fclose(file_ptrs[i]);
		file_ptrs[i] = 0;
	} else
		error("close_file : invalid argument");
	*rp = ONE;
}

void Pget_line(arg,rp)
NODE arg;
STRING *rp;
{
	int i,j,c;
	FILE *fp;
	fpos_t head;
	char *str;

	asir_assert(ARG0(arg),O_N,"get_line");
	i = QTOS((Q)ARG0(arg));
	if ( fp = file_ptrs[i] ) {
		if ( feof(fp) ) {
			*rp = 0;
			return;
		}
		fgetpos(fp,&head);
		j = 0;
		while ( 1 ) {
			c = getc(fp);
			if ( c == EOF ) {
				if ( !j ) {
					*rp = 0;
					return;
				} else
					break;
			}
			j++;
			if ( c == '\n' )
				break;
		}
		fsetpos(fp,&head);
		str = (char *)MALLOC_ATOMIC(j+1);	
		fgets(str,j+1,fp);	
		MKSTR(*rp,str);
	} else
		error("get_line : invalid argument");
}

void Pget_byte(arg,rp)
NODE arg;
Q *rp;
{
	int i,c;
	FILE *fp;

	asir_assert(ARG0(arg),O_N,"get_byte");
	i = QTOS((Q)ARG0(arg));
	if ( fp = file_ptrs[i] ) {
		if ( feof(fp) ) {
			STOQ(-1,*rp);
			return;
		}
		c = getc(fp);
		STOQ(c,*rp);
	} else
		error("get_byte : invalid argument");
}

void Pload(arg,rp)
NODE arg;
Q *rp;
{
	int ret = 0;
	char *name,*name0;
	char errbuf[BUFSIZ];

#if defined THINK_C
	if ( !arg ) {
		ret = finder_loadfile();
	} else
#endif
	if ( ARG0(arg) ) {
		switch (OID(ARG0(arg))) {
			case O_STR:
				name0 = BDY((STRING)ARG0(arg));
				searchasirpath(name0,&name);
				if ( !name ) {
					sprintf(errbuf,"load : \"%s\" not found in the search path",name0);
					error(errbuf);
				}
				ret = loadfile(name);
				if ( !ret ) {
					sprintf(errbuf,"load : \"%s\" could not be loaded",name);
					error(errbuf);
				}
				break;
			default:
				error("load : invalid argument");
				break;
		}
	}
	STOQ(ret,*rp);
}

void Pwhich(arg,rp)
NODE arg;
STRING *rp;
{
	char *name;
	STRING str;

	switch (OID(ARG0(arg))) {
		case O_STR:
			searchasirpath(BDY((STRING)ARG0(arg)),&name);
			break;
		default:
			name = 0;
			break;
	}
	if ( name ) {
		MKSTR(str,name); *rp = str;
	} else
		*rp = 0;
}

void Ploadfiles(arg,rp)
NODE arg;
Q *rp;
{
	int ret;

	if ( ARG0(arg) )
		if ( OID(ARG0(arg)) != O_LIST )
			ret = 0;
		else
			ret = loadfiles(BDY((LIST)ARG0(arg)));
	else
		ret = 0;
	STOQ(ret,*rp);
}

void Poutput(arg,rp)
NODE arg;
Q *rp;
{
#if PARI
	extern FILE *outfile;
#endif
	FILE *fp;

	fflush(asir_out);
	if ( asir_out != stdout )
		fclose(asir_out);
	switch ( argc(arg) ) {
		case 0:
			fp = stdout; break;
		case 1:
			asir_assert(ARG0(arg),O_STR,"output");
			fp = fopen(((STRING)ARG0(arg))->body,"a+");
			if ( !fp )
				error("output : invalid filename");
			break;
	}
#if PARI
	pari_outfile =
#endif
	asir_out = fp;
	*rp = ONE;
}

extern int ox_file_io;

void Pbsave(arg,rp)
NODE arg;
Q *rp;
{
	FILE *fp;
	VL vl,t;

	asir_assert(ARG1(arg),O_STR,"bsave");
	get_vars_recursive(ARG0(arg),&vl);
	for ( t = vl; t; t = NEXT(t) )
		if ( t->v->attr == (pointer)V_UC )
			error("bsave : not implemented");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG1(arg)),"wb");
#else
	fp = fopen(BDY((STRING)ARG1(arg)),"w");
#endif
	if ( !fp )
		error("bsave : invalid filename");
	ox_file_io = 1; /* network byte order is used */
	savevl(fp,vl);
	saveobj(fp,ARG0(arg));
	fclose(fp);
	ox_file_io = 0;
	*rp = ONE;
}

void Pbload(arg,rp)
NODE arg;
Obj *rp;
{
	FILE *fp;

	asir_assert(ARG0(arg),O_STR,"bload");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG0(arg)),"rb");
#else
	fp = fopen(BDY((STRING)ARG0(arg)),"r");
#endif
	if ( !fp )
		error("bload : invalid filename");
	ox_file_io = 1; /* network byte order is used */
	loadvl(fp);
	loadobj(fp,rp);
	fclose(fp);
	ox_file_io = 0;
}

void Pbsave_cmo(arg,rp)
NODE arg;
Q *rp;
{
	FILE *fp;
	VL vl,t;

	asir_assert(ARG1(arg),O_STR,"bsave_cmo");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG1(arg)),"wb");
#else
	fp = fopen(BDY((STRING)ARG1(arg)),"w");
#endif
	if ( !fp )
		error("bsave_cmo : invalid filename");
	ox_file_io = 1; /* network byte order is used */
	write_cmo(fp,ARG0(arg));
	fclose(fp);
	ox_file_io = 0;
	*rp = ONE;
}

void Pbload_cmo(arg,rp)
NODE arg;
Obj *rp;
{
	FILE *fp;

	asir_assert(ARG0(arg),O_STR,"bload_cmo");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG0(arg)),"rb");
#else
	fp = fopen(BDY((STRING)ARG0(arg)),"r");
#endif
	if ( !fp )
		error("bload_cmo : invalid filename");
	ox_file_io = 1; /* network byte order is used */
	read_cmo(fp,rp);
	fclose(fp);
	ox_file_io = 0;
}

static struct oSTRING rootdir;

#if defined(VISUAL)
void get_rootdir(name,len)
char *name;
int len;
{
	LONG	ret;
	HKEY	hOpenKey;
	DWORD	Type;
	char	*slash;
		
	if ( rootdir.body ) {
		strcpy(name,rootdir.body);
		return;
	}
	name[0] = 0;
	ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, ECGEN_KEYNAME, 0,
		KEY_QUERY_VALUE, &hOpenKey);
	if ( ret != ERROR_SUCCESS )
		ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, ASIR_KEYNAME, 0,
			KEY_QUERY_VALUE, &hOpenKey);
	if( ret == ERROR_SUCCESS ) {
		RegQueryValueEx(hOpenKey, "Directory", NULL, &Type, name, &len);
		RegCloseKey(hOpenKey);
	} else {
		GetCurrentDirectory(len,name);
		slash = strrchr(name,'\\');
		if ( slash )
			*slash = 0;
	}
}

void set_rootdir(name)
char *name;
{
	static char DirName[BUFSIZ];

	strcpy(DirName,name);
	rootdir.id = O_STR;
	rootdir.body = DirName;
	asir_libdir = DirName;
	/* XXX */
	env_init();
}

#else
void get_rootdir(name,len)
char *name;
int len;
{
	strcpy(name,asir_libdir);
}

void set_rootdir(name)
char *name;
{
	static char DirName[BUFSIZ];

	strcpy(DirName,name);
	asir_libdir = DirName;
	/* XXX */
	env_init();
}

#endif

void Pget_rootdir(rp)
STRING *rp;
{
	static char DirName[BUFSIZ];
	int DirNameLen;

	if ( !rootdir.body ) {
		get_rootdir(DirName,sizeof(DirName));
		rootdir.id = O_STR;
		rootdir.body = DirName;
	}
	*rp = &rootdir;
}

#if defined(VISUAL) && defined(DES_ENC)
void Pbsave_enc(arg,rp)
NODE arg;
Obj *rp;
{
	init_deskey();
	des_encryption = 1;
	Pbsave(arg,rp);
	des_encryption = 0;
}

void Pbload_enc(arg,rp)
NODE arg;
Obj *rp;
{
	init_deskey();
	des_encryption = 1;
	Pbload(arg,rp);
	des_encryption = 0;
}
#endif

void Pbload27(arg,rp)
NODE arg;
Obj *rp;
{
	FILE *fp;
	Obj r;

	asir_assert(ARG0(arg),O_STR,"bload27");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG0(arg)),"rb");
#else
	fp = fopen(BDY((STRING)ARG0(arg)),"r");
#endif
	if ( !fp )
		error("bload : invalid filename");
	loadvl(fp);
	loadobj(fp,&r);
	fclose(fp);
	bobjtoobj(BASE27,r,rp);
}

void Pbsave_compat(arg,rp)
NODE arg;
Q *rp;
{
	FILE *fp;
	VL vl,t;

	asir_assert(ARG1(arg),O_STR,"bsave_compat");
	get_vars_recursive(ARG0(arg),&vl);
	for ( t = vl; t; t = NEXT(t) )
		if ( t->v->attr == (pointer)V_UC )
			error("bsave : not implemented");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG1(arg)),"wb");
#else
	fp = fopen(BDY((STRING)ARG1(arg)),"w");
#endif
	if ( !fp )
		error("bsave : invalid filename");
	/* indicator of an asir32 file */
	putw(0,fp); putw(0,fp);
	savevl(fp,vl);
	saveobj(fp,ARG0(arg));
	fclose(fp);
	*rp = ONE;
}

void Pbload_compat(arg,rp)
NODE arg;
Obj *rp;
{
	FILE *fp;
	unsigned int hdr[2];
	Obj r;
	int c;

	asir_assert(ARG0(arg),O_STR,"bload_compat");
#if defined(THINK_C) || defined(VISUAL)
	fp = fopen(BDY((STRING)ARG0(arg)),"rb");
#else
	fp = fopen(BDY((STRING)ARG0(arg)),"r");
#endif
	if ( !fp )
		error("bload : invalid filename");
	fread(hdr,sizeof(unsigned int),2,fp);
	if ( !hdr[0] && !hdr[1] ) {
		/* asir32 file or asir27 0 */
		c = fgetc(fp);
		if ( c == EOF ) {
			/* asir27 0 */
			*rp = 0;
		} else {
			/* asir32 file */
			ungetc(c,fp);
			loadvl(fp);
			loadobj(fp,rp);
		}
	} else {
		/* asir27 file */
		rewind(fp);
		loadvl(fp);
		loadobj(fp,&r);
		bobjtoobj(BASE27,r,rp);
	}
	fclose(fp);
}

void Premove_file(arg,rp)
NODE arg;
Q *rp;
{
	unlink((char *)BDY((STRING)ARG0(arg)));
	*rp = ONE;
}

void Paccess(arg,rp)
NODE arg;
Q *rp;
{
	char *name;
	STRING str;

#if defined(VISUAL)
	if ( access(BDY((STRING)ARG0(arg)),04) >= 0 )
#else
	if ( access(BDY((STRING)ARG0(arg)),R_OK) >= 0 )
#endif
		*rp = ONE;
	else
		*rp = 0;
}
