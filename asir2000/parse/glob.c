/* $OpenXM: OpenXM_contrib2/asir2000/parse/glob.c,v 1.2 1999/12/22 07:01:40 noro Exp $ */
#include "ca.h"
#include "al.h"
#include "parse.h"
#if PARI
#include "genpari.h"
#endif
#if !defined(THINK_C) && !defined(VISUAL) && !defined(_PA_RISC1_1) && !defined(linux) && !defined(SYSV)
#include <sgtty.h>
#else
#if defined(THINK_C) && !defined(__MWERKS__)
#include <console.h>
#endif
#endif

#if defined(VISUAL)
#include <io.h>
#include <direct.h>
#endif

#if defined(SYSV)
#include <sys/ttold.h>
#endif

#if defined(THINK_C) || defined(VISUAL)
#if defined(VISUAL)
#define HISTORY asir_history
#else
#define HISTORY "history"
#endif
#endif
#define MAXHIST 100

extern int GC_free_space_divisor;
extern int GC_free_space_numerator;
extern FILE *asir_out;

IN asir_infile;
jmp_buf env,debug_env,timer_env;
int little_endian,debug_mode;
char *asir_libdir;
char *asir_pager;

NODE usrf,sysf,noargsysf,ubinf,parif;
NODE ONENODE;
int main_parser, ox_do_copy, ox_do_count, ox_count_length;
int ox_file_io, ox_need_conv;
char *ox_copy_bptr;
char *parse_strp;
SNODE parse_snode;
FUNC parse_targetf;
FILE *ox_istream,*ox_ostream;
int do_server_in_X11;
Obj LastVal;
char LastError[BUFSIZ];

struct oV oVAR[] = {
	{"x",0,0}, {"y",0,0}, {"z",0,0}, {"u",0,0},
	{"v",0,0}, {"w",0,0}, {"p",0,0}, {"q",0,0},
	{"r",0,0}, {"s",0,0}, {"t",0,0}, {"a",0,0},
	{"b",0,0}, {"c",0,0}, {"d",0,0}, {"e",0,0},
	{"f",0,0}, {"g",0,0}, {"h",0,0}, {"i",0,0},
	{"j",0,0}, {"k",0,0}, {"l",0,0}, {"m",0,0},
	{"n",0,0}, {"o",0,0},
	{"_x",0,0}, {"_y",0,0}, {"_z",0,0}, {"_u",0,0},
	{"_v",0,0}, {"_w",0,0}, {"_p",0,0}, {"_q",0,0},
	{"_r",0,0}, {"_s",0,0}, {"_t",0,0}, {"_a",0,0},
	{"_b",0,0}, {"_c",0,0}, {"_d",0,0}, {"_e",0,0},
	{"_f",0,0}, {"_g",0,0}, {"_h",0,0}, {"_i",0,0},
	{"_j",0,0}, {"_k",0,0}, {"_l",0,0}, {"_m",0,0},
	{"_n",0,0}, {"_o",0,0}
};

struct oVL oVLIST[52];

VL CO = oVLIST;
VL ALG;

struct oVS oGPVS,oAPVS,oEPVS;
VS GPVS = &oGPVS;
VS APVS = &oAPVS;
VS EPVS = &oEPVS;
VS CPVS;

struct oF oF_TRUE,oF_FALSE;
F F_TRUE = &oF_TRUE;  
F F_FALSE = &oF_FALSE;

#if defined(__SVR4) && defined(sun)
char cppname[64] = "/usr/ccs/lib/cpp ";
#else
#if defined(__FreeBSD__) || defined(__NetBSD__)
char cppname[64] = "/usr/bin/cpp ";
#else
#if defined(VISUAL)
char cppname[64] = "c:\\asir\\stdlib\\cpp ";
#else
char cppname[64] = "/lib/cpp ";
#endif
#endif
#endif
char asirname[64];
char displayname[BUFSIZ];

int Verbose;

void glob_init() {
	int i;
	
	for ( i = 0; i < 51; i++ ) {
		VR(&oVLIST[i]) = &oVAR[i]; NEXT(&oVLIST[i]) = &oVLIST[i+1];
	}
	VR(&oVLIST[i]) = &oVAR[i]; NEXT(&oVLIST[i]) = 0;
	reallocarray((char **)&GPVS->va,(int *)&GPVS->asize,(int *)&GPVS->n,(int)sizeof(struct oPV));
	reallocarray((char **)&APVS->va,(int *)&APVS->asize,(int *)&APVS->n,(int)sizeof(struct oPV));
	CPVS = GPVS;
	MKNODE(ONENODE,mkfnode(1,I_FORMULA,ONE),0);
	OID(F_TRUE)=O_F; FOP(F_TRUE)=AL_TRUE; F_TRUE->arg.dummy = 0;
	OID(F_FALSE)=O_F; FOP(F_FALSE)=AL_FALSE; F_FALSE->arg.dummy = 0;
	sprintf(asirname,"%s/asir_symtab",asir_libdir);
#if defined(THINK_C)
	initVol();
#endif
}

void input_init(fp,name)
FILE *fp;
char *name;
{
	asir_infile = (IN)CALLOC(sizeof(struct oIN),1);
	asir_infile->name = name; asir_infile->fp = fp;
	asir_infile->ln = 1; NEXT(asir_infile) = 0;
}

void notdef(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	*c = 0;
	error("undefined arithmetic operation.");
}

int kernelmode;
int do_asirrc;
int do_file;
int do_message;
int do_fep;
int no_prompt;
int read_exec_file;
static int buserr_sav;
static char asir_history[BUFSIZ];

extern int mpi_myid;

/*
 * status = 1 abnormal termination (error() etc.)
 * status = 2 normal termination (end(), quit() etc.)
 */

void asir_terminate(status)
int status;
{
	if ( read_exec_file ) {
		read_exec_file = 0; longjmp(env,status);
	} else {
		tty_reset();
#if INET && !defined(THINK_C)
#if MPI
		if ( !mpi_myid )
			close_allconnections();
		mpi_finalize();
#else
		close_allconnections();
#endif
#endif
		if ( kernelmode )
			fputc(0xff,asir_out);
		if ( asir_out )
			fflush(asir_out);
#if defined(THINK_C) && !defined(__MWERKS__)
		*((int *)0x8) = buserr_sav;
		while ( NEXT(asir_infile) )
		closecurrentinput();
		console_options.pause_atexit = 0;
		resetDir();
		write_hist(HISTORY);
#endif
		ExitAsir();
	}
}

void param_init() {
	unsigned int et = 0xff;
	extern int paristack;
#if defined(THINK_C)	
	char name[BUFSIZ];
	int c;
	int val;
	FILE *fp;
	int stacksize;

#include <Memory.h>
	GC_free_space_divisor = 4; stacksize = 0x40000;
	if ( fp = fopen("params","r") ) {
		while ( 1 ) {
			c = fgetc(fp);
			if ( c == EOF )
				break;
			else
				ungetc(c,fp);
			fscanf(fp,"%s %d",name,&val);
			if ( !strcmp(name,"stacksize") )
				stacksize = val;
			else if ( !strcmp(name,"adj") )
				GC_free_space_divisor = val;
#if PARI
			else if ( !strcmp(name,"paristack") )
				paristack = val;
#endif
		}
		fclose(fp);
	}
#if defined(__MWERKS__)
	{
		Ptr al;
		al = LMGetApplLimit();
		LMSetApplLimit(al-stacksize);
	}
#else
	ApplLimit = (Ptr)((char *)ApplLimit-stacksize);
	console_options.title = "\pAsir";
#endif
#endif
	if ( *((char *)&et) )
		little_endian = 1;
	else
		little_endian = 0;
}
	
void prompt() {
	if ( !no_prompt && !do_fep && asir_infile->fp == stdin )
		fprintf(asir_out,"[%d]%c",APVS->n,kernelmode?0xfe:' ');
	fflush(asir_out);
}

void sprompt(ptr)
char *ptr;
{
	sprintf(ptr,"[%d]%c",APVS->n,kernelmode?0xfe:' ');
}

#if 0 && !defined(THINK_C) && !defined(VISUAL) && !defined(_PA_RISC1_1)
static struct tchars tc;
static char oldeof;
struct winsize wsize;
int ttywidth;
char *upperbuf,*lowerbuf;
#endif

FILE *in_fp;

void process_args(ac,av)
int ac;
char **av;
{
#if !defined(THINK_C)
	do_asirrc = 1;
#if !MPI
	do_message = 1;
#endif
	while ( ac > 0 ) {
		if ( !strcmp(*av,"-heap") && (ac >= 2) ) {
			void GC_expand_hp(int);

			GC_expand_hp(atoi(*(av+1))); av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-adj") && (ac >= 2) ) {
			char *slash;

			slash = strrchr(*(av+1),'/');
			if ( slash ) {
				*slash = 0;
				GC_free_space_numerator = atoi(slash+1);
			}
			GC_free_space_divisor = atoi(*(av+1));
			av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-cpp") && (ac >= 2) ) {
			strcpy(cppname,*(av+1)); av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-f") && (ac >= 2) ) {
			in_fp = fopen(*(av+1),"r");
			if ( !in_fp ) {
				fprintf(stderr,"%s does not exist!",*(av+1));
				asir_terminate(1);
			}
			do_file = 1;
			av += 2; ac -= 2;
		} else if ( !strcmp(*av,"-kernel") ) {
			kernelmode = 1; av++; ac--;
		} else if ( !strcmp(*av,"-norc") ) {
			do_asirrc = 0; av++; ac--;
		} else if ( !strcmp(*av,"-nomessage") ) {
			do_message = 0; av++; ac--;
		} else if ( !strcmp(*av,"-terse") ) {
			no_prompt = 0; av++; ac--;
		} else if ( !strcmp(*av,"-maxheap") && (ac >= 2) ) {
			void GC_set_max_heap_size(int);

			GC_set_max_heap_size(atoi(*(av+1))); av += 2; ac -= 2;
#if !defined(VISUAL)
		} else if ( !strcmp(*av,"-display") && (ac >= 2) ) {
			strcpy(displayname,*(av+1)); av += 2; ac -= 2;
#endif
#if PARI
		} else if ( !strcmp(*av,"-paristack") ) {
			extern int paristack;

			paristack = atoi(*(av+1)); av += 2; ac -= 2;
#endif
		} else {
			fprintf(stderr,"%s : unknown option.\n",*av);
			asir_terminate(1);
		}
	}
#endif
#if (defined(THINK_C) && !defined(__MWERKS__))
	init_hist(MAXHIST);
	read_hist(HISTORY);
#endif
}

#include <signal.h>

void sig_init() {
#if !defined(VISUAL)
	signal(SIGINT,int_handler);
#else
	register_ctrlc_handler();
#endif
	signal(SIGSEGV,segv_handler);

#if defined SIGFPE
	signal(SIGFPE,fpe_handler);
#endif

#if defined SIGPIPE
	signal(SIGPIPE,pipe_handler);
#endif

#if defined SIGILL
	signal(SIGILL,ill_handler);
#endif

#if defined(THINK_C)
	buserr_sav = *((int *)0x8);
	*((int *)0x8) = (int)bus_handler;
#else /* THINK_C */

#if !defined(VISUAL)
	signal(SIGBUS,bus_handler);
#endif

#if 0
#if !defined(VISUAL) && !defined(_PA_RISC1_1)
	signal(SIGWINCH,winch_handler);
#endif
#endif

#endif /* THINK_C */
}

static void (*old_int)(int);

void asir_save_handler() {
	old_int = signal(SIGINT,SIG_IGN);
	signal(SIGINT,old_int);
}

void asir_set_handler() {
	signal(SIGINT,int_handler);
}

void asir_reset_handler() {
	signal(SIGINT,old_int);
}

void resetenv(s)
char *s;
{
	extern FILE *outfile;

	fprintf(stderr,"%s\n",s);
	while ( NEXT(asir_infile) )
		closecurrentinput();
	resetpvs();
#if !defined(VISUAL)
	if ( do_server_in_X11 )
#endif
		show_debug_window(0);
#if defined(VISUAL_LIB)
	w_noflush_stderr(0);
#endif
	asir_out = stdout;
#if PARI
	pari_outfile = stdout;
#endif
	/* restore states */
	reset_engine();
	reset_io();
	longjmp(env,1);
}

void fatal(n)
int n;
{
	resetenv("return to toplevel");
}

FUNC registered_handler;
extern int ox_int_received, critical_when_signal;

void int_handler(sig)
int sig;
{
	extern NODE PVSS;

	if ( do_file ) {
		ExitAsir();
	}
	if ( critical_when_signal ) {
		ox_int_received = 1;
		return;
	}
#if defined(THINK_C) || defined(VISUAL)
	suspend_timer(); signal(SIGINT,SIG_IGN);
#endif
#if defined(_PA_RISC1_1) || defined(linux) || defined(VISUAL) || defined(__svr4__)
	signal(SIGINT,SIG_IGN);
#endif
#if !defined(VISUAL) 
    if ( do_server_in_X11 ) {
		debug(PVSS?((VS)BDY(PVSS))->usrf->f.usrf->body:0);
 		restore_handler();
		return;
	}
#endif
#if defined(linux)
#if 1
	while ( stdin->_IO_read_ptr < stdin->_IO_read_end )
#else
	while ( stdin->_gptr < stdin->_egptr )
#endif
		getchar();
#endif
	while ( 1 ) {
		char buf[BUFSIZ];
		char c;

		fprintf(stderr,"interrupt ?(q/t/c/d/u/w/?) "); fflush(stderr);
		if ( kernelmode )
			fputc('\0',stderr);
		buf[0] = '\n';
		while ( buf[0] == '\n' )
			fgets(buf,BUFSIZ,stdin);
		switch ( c = buf[0] ) {
			case 'q':
				while ( 1 ) {
					fprintf(stderr,"Abort this session? (y or n) "); fflush(stderr);
					fgets(buf,BUFSIZ,stdin);
					if ( !strncmp(buf,"y",1) ) {
						read_exec_file = 0;
						fprintf(stderr,"Bye\n"); asir_terminate(1);
					} else if ( !strncmp(buf,"n",1) ) {
						restore_handler();
						return;
					}
				}
				break;
			case 't':
			case 'u':
				while ( 1 ) {
					fprintf(stderr,"Abort this computation? (y or n) "); fflush(stderr);
					fgets(buf,BUFSIZ,stdin);
					if ( !strncmp(buf,"y",1) )
						break;
					else if ( !strncmp(buf,"n",1) ) {
						restore_handler();
						return;
					}
				}
				if ( debug_mode )
					debug_mode = 0;
				if ( kernelmode )
					fputc('\0',stderr);
				restore_handler();
				if ( c == 'u' ) {
					if ( registered_handler ) {
						fprintf(stderr,
							"Calling the registered exception handler...");
						bevalf(registered_handler,0);
						fprintf(stderr, "done.\n");
					}
				}
				if ( read_exec_file ) {
					read_exec_file = 0;
					resetenv("initialization aborted; return to toplevel");
				} else
					resetenv("return to toplevel");
				break;
			case 'd':
#if 0
				nextbp = 1; nextbplevel = -1;
#endif
				debug(PVSS?((VS)BDY(PVSS))->usrf->f.usrf->body:0);
 				restore_handler();
 				return;
			case 'c': 
				if ( kernelmode )
					fputc('\0',stderr);
				restore_handler();
				return; break;
			case 'w':
				showpos(); break;
			case '?': 
				fprintf(stderr, "q:quit t:toplevel c:continue d:debug u:call registered handler w:where\n");
				break;
			default:
				break;
		}
	}
}

void restore_handler() {
#if defined(THINK_C) || defined(VISUAL)
	resume_timer(); signal(SIGINT,int_handler);
#endif
#if defined(_PA_RISC1_1) || defined(linux) || defined(__svr4__)
	signal(SIGINT,int_handler);
#endif
}

void segv_handler(sig)
int sig;
{
#if defined(THINK_C) || defined(_PA_RISC1_1) || defined(linux) || defined(VISUAL) || defined(__svr4__)
	signal(SIGSEGV,segv_handler);
#endif
	error("internal error (SEGV)");
}

void ill_handler(sig)
int sig;
{
#if defined(THINK_C) || defined(_PA_RISC1_1) || defined(linux) || defined(VISUAL) || defined(__svr4__)
	signal(SIGILL,ill_handler);
#endif
	error("illegal instruction (ILL)");
}

void alrm_handler(sig)
int sig;
{
	fprintf(stderr,"interval timer expired (VTALRM)\n");
	longjmp(timer_env,1);
}

void bus_handler(sig)
int sig;
{
#if defined(SIGBUS)
	signal(SIGBUS,bus_handler);
	error("internal error (BUS ERROR)");
#endif
}

void fpe_handler(sig)
int sig;
{
#if defined(THINK_C) || defined(_PA_RISC1_1) || defined(linux) || defined(VISUAL) || defined(__svr4__)
	signal(SIGFPE,fpe_handler);
#endif
	error("internal error (FPE)");
}

void winch_handler(sig)
int sig;
{
#if 0
#if !defined(THINK_C) && !defined(VISUAL) && !defined(_PA_RISC1_1) && !defined(__svr4__)
	if ( isatty(1) ) {
		struct winsize t;

		if ( ioctl(1,TIOCGWINSZ,&t) == -1 )
			perror("TIOCGWINSZ");
		if ( t.ws_col != wsize.ws_col || t.ws_row != wsize.ws_row ) {
			resize_buffer();
			if ( killpg(getpgrp(0),SIGWINCH) == -1 )
				perror("killpg");
		}
	}
#endif
#endif
}

void pipe_handler(sig)
int sig;
{
#if defined(SIGPIPE)
	signal(SIGPIPE,pipe_handler);
	error("internal error (BROKEN PIPE)");
#endif
}

void resize_buffer()
{
#if 0 && !defined(THINK_C) && !defined(VISUAL) && !defined(_PA_RISC1_1)
	if ( isatty(1) ) {
		if ( ioctl(1,TIOCGWINSZ,&wsize) == -1 )
			perror("TIOCGWINSZ");
		if ( wsize.ws_col > 2 ) {
			ttywidth = wsize.ws_col - 2;
			upperbuf = (char *)MALLOC(wsize.ws_col);
			lowerbuf = (char *)MALLOC(wsize.ws_col);
		}
	}
#endif
}

void tty_init() {
#if 0 && !defined(THINK_C) && !defined(VISUAL) && !defined(_PA_RISC1_1)
	if ( isatty(0) ) {
		if ( ioctl(0,TIOCGETC,&tc) == -1 )
			perror("TIOCGETC"); 
		oldeof = tc.t_eofc; tc.t_eofc = 0xff; 
		if ( ioctl(0,TIOCSETC,&tc) == -1 )
			perror("TIOCSETC");
		setpgrp(0, getpid());
	}
	resize_buffer();
#endif
}

void tty_reset() {
#if 0 && !defined(THINK_C) && !defined(VISUAL) && !defined(_PA_RISC1_1)
	if ( oldeof ) {
		tc.t_eofc = oldeof; 
		if ( ioctl(0,TIOCSETC,&tc) == -1 )
			perror("TIOCSETC");
	}
#endif
}

extern int evalstatline;

void set_lasterror(s)
char *s;
{
	strncpy(LastError,s,BUFSIZ);
	LastError[BUFSIZ-1] = 0;
}

SNODE error_snode;

void error(s)
char *s;
{
	SNODE *snp;

	fprintf(stderr,"%s\n",s);
	set_lasterror(s);
	if ( CPVS != GPVS ) {
		searchsn(&BDY(CPVS->usrf->f.usrf),evalstatline,&snp);
		error_snode = *snp;
	} else
		error_snode = 0;
	if ( do_file ) {
		char errbuf[BUFSIZ*5]; /* sufficient to store stack information ? */

		sprintf(errbuf,"%s\n",s);
		showpos_to_string(errbuf+strlen(errbuf));
		set_lasterror(errbuf);
		ExitAsir();
	}
	if ( debug_mode )
		longjmp(debug_env,1);
	if ( CPVS != GPVS )
		if ( do_server_in_X11 || isatty(0) )
			bp(error_snode);
	if ( read_exec_file )
		read_exec_file = 0;
	resetenv("return to toplevel");
}

#if !defined(VISUAL)
#include <sys/time.h>

void set_timer(interval)
int interval;
{
	struct itimerval it;

	it.it_interval.tv_sec = interval;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = interval;
	it.it_value.tv_usec = 0;
	setitimer(ITIMER_VIRTUAL,&it,0);
	signal(SIGVTALRM,alrm_handler);
}

void reset_timer()
{
	struct itimerval it;

	it.it_interval.tv_sec = 0;
	it.it_interval.tv_usec = 0;
	it.it_value.tv_sec = 0;
	it.it_value.tv_usec = 0;
	setitimer(ITIMER_VIRTUAL,&it,0);
	signal(SIGVTALRM,SIG_IGN);
}
#endif

unsigned int get_asir_version();

void copyright() {
	printf("This is Risa/Asir, Version %d.\n",get_asir_version());
	printf("Copyright (C) FUJITSU LABORATORIES LIMITED.\n");
	printf("1994-1999. All rights reserved.\n");
}
