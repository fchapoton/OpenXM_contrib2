/* $OpenXM: OpenXM_contrib2/asir2000/parse/lex.c,v 1.1.1.1 1999/12/03 07:39:12 noro Exp $ */
#include <ctype.h>
#include "ca.h"
#include "al.h"
#include "base.h"
#include "parse.h"
#if !defined(THINK_C)
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "y.tab.h"

extern IN asir_infile;
extern struct oTKWD kwd[];

int afternl();
int myatoi();
int aftercomment();

extern int main_parser;
extern char *parse_strp;

static int skipspace();
static int Getc();
static void Ungetc();
static void Gets();

#define NBUFSIZ (BUFSIZ*10)
#define TBUFSIZ (BUFSIZ)

#define REALLOC_NBUF \
if ( i >= nbufsize ) {\
	nbufsize += NBUFSIZ;\
	if ( nbuf == nbuf0 ) {\
		nbuf = (char *)MALLOC_ATOMIC(nbufsize);\
		bcopy(nbuf0,nbuf,nbufsize-NBUFSIZ);\
	} else\
		nbuf = REALLOC(nbuf,nbufsize);\
}

#define REALLOC_TBUF \
if ( i >= tbufsize ) {\
	tbufsize += TBUFSIZ;\
	if ( tbuf == tbuf0 ) {\
		tbuf = (char *)MALLOC_ATOMIC(tbufsize);\
		bcopy(tbuf0,tbuf,tbufsize-TBUFSIZ);\
	} else\
		tbuf = REALLOC(tbuf,tbufsize);\
}

#define READ_ALNUM_NBUF \
while ( 1 ) {\
	c = Getc();\
	if ( isalnum(c) ) {\
		REALLOC_NBUF nbuf[i++] = c;\
	} else\
		break;\
}

#define READ_DIGIT_NBUF \
while ( 1 ) {\
	c = Getc();\
	if ( isdigit(c) ) {\
		REALLOC_NBUF nbuf[i++] = c;\
	} else\
		break;\
}

yylex()
{
#define yylvalp (&yylval)
	register int c,c1;
	register int *ptr;
	char *cptr;
	int d,i,j;
	char nbuf0[NBUFSIZ],tbuf0[TBUFSIZ];
	char *nbuf, *tbuf;
	int nbufsize, tbufsize;
	N n,n1;
	Q q;
	Obj r;
	
	/* initialize buffer pointers */
	nbuf = nbuf0; tbuf = tbuf0;
	nbufsize = NBUFSIZ; tbufsize = TBUFSIZ;

	switch ( c = skipspace() ) {
		case EOF :	
			asir_terminate(2); break;
		case '0' :
			while ( ( c = Getc() ) == '0' );
			if ( c == '.' ) {
				Ungetc(c); c = '0';
			} else if ( c == 'x' ) {
				for ( i = 0; i < 8; i++ ) 
					nbuf[i] = '0';
				READ_ALNUM_NBUF
				Ungetc(c); REALLOC_NBUF nbuf[i] = 0;
				hexton(nbuf,&n1); 
				NTOQ(n1,1,q); r = (Obj)q;
				yylvalp->p = (pointer)r;
				return ( FORMULA );
			} else if ( c == 'b' ) {
				for ( i = 0; i < 32; i++ ) 
					nbuf[i] = '0';
				READ_ALNUM_NBUF
				Ungetc(c); REALLOC_NBUF nbuf[i] = 0;
				binaryton(nbuf,&n1); 
				NTOQ(n1,1,q); r = (Obj)q;
				yylvalp->p = (pointer)r;
				return ( FORMULA );
			} else if ( !isdigit(c) ) {
				yylvalp->p = 0; Ungetc(c); 
				return ( FORMULA );
			}
			break;
		case '\'' :
			for ( i = 0; ; i++ ) {
				c = Getc();
				if ( c == '\'' )
					break;
				if ( c == '\\' )
					c = Getc();
				REALLOC_TBUF tbuf[i] = c;
			}
			REALLOC_TBUF tbuf[i] = 0;
			cptr = (char *)MALLOC(strlen(tbuf)+1); strcpy(cptr,tbuf);
			yylvalp->p = (pointer)cptr;
			return LCASE; break;
		case '"' :
			i = 0;
			do {
				c = Getc();
				if ( c == '\\' ) {
					c1 = Getc();
					if ( c1 == 'n' )
						c1 = '\n';
					REALLOC_NBUF nbuf[i++] = c1;
				} else {
					REALLOC_NBUF nbuf[i++] = c;
				}
			} while ( c != '"' );
			nbuf[i-1] = 0; /* REALLOC_NBUF is not necessary */
			cptr = (char *)MALLOC(strlen(nbuf)+1);
			strcpy(cptr,nbuf); yylvalp->p = (pointer) cptr;
			return ( STR ); break;
		case '>': case '<': case '=': case '!':
			if ( (c1 = Getc()) == '=' )
				switch ( c ) {
					case '>': yylvalp->i = (int)C_GE; break;
					case '<': yylvalp->i = (int)C_LE; break;
					case '=': yylvalp->i = (int)C_EQ; break;
					case '!': yylvalp->i = (int)C_NE; break;
					default: break;
				}
			else if ( (c == '<' && c1 == '<') || (c == '>' && c1 == '>') )
				return c;
			else {
				Ungetc(c1);
				switch ( c ) {
					case '>': yylvalp->i = (int)C_GT; break;
					case '<': yylvalp->i = (int)C_LT; break;
					default: return c; break;
				}
			}
			return CMP; break;
		case '+': case '-': case '*': case '/': case '%': case '^': 
		case '|': case '&':
			switch ( c ) {
				case '+': yylvalp->p = (pointer)addfs; break;
				case '-': yylvalp->p = (pointer)subfs; break;
				case '*': yylvalp->p = (pointer)mulfs; break;
				case '/': yylvalp->p = (pointer)divfs; break;
				case '%': yylvalp->p = (pointer)remfs; break;
				case '^': yylvalp->p = (pointer)pwrfs; break;
				default: break;
			}
			if ( (c1 = Getc()) == c )
				switch ( c ) {
					case '+': case '-': return SELF; break;
					case '|': return OR; break;
					case '&': return AND; break;
					default: Ungetc(c1); return c; break;
				}
			else if ( c1 == '=' ) 
				return BOPASS;
			else if ( (c == '-') && (c1 == '>') )
				return POINT;
			else {
				Ungetc(c1); return c;
			}
			break;
		default :
			break;
	}
	if ( isdigit(c) ) {
		for ( i = 0; i < DLENGTH; i++ ) 
			nbuf[i] = '0';
		REALLOC_NBUF nbuf[i++] = c;
		READ_DIGIT_NBUF
		if ( c == '.' ) {
			double dbl;
			Real real;
			double atof();
			extern int bigfloat;

			REALLOC_NBUF nbuf[i++] = c;
			READ_DIGIT_NBUF
			if ( c == 'e' ) {
				REALLOC_NBUF nbuf[i++] = c;
				c = Getc();
				if ( (c == '+') || (c == '-') ) {
					REALLOC_NBUF nbuf[i++] = c;
				} else
					Ungetc(c);
				READ_DIGIT_NBUF
			}
			Ungetc(c); REALLOC_NBUF nbuf[i] = 0;
#if PARI
			if ( !bigfloat ) {
				dbl = (double)atof(nbuf+DLENGTH);
				MKReal(dbl,real); r = (Obj)real;
			} else
				strtobf(nbuf,(BF *)&r);
#else
			dbl = (double)atof(nbuf+DLENGTH);
			MKReal(dbl,real); r = (Obj)real;
#endif
		} else {
			Ungetc(c);
			i -= DLENGTH; d = (i%DLENGTH?i/DLENGTH+1:i/DLENGTH);
			n = NALLOC(d); PL(n) = d;
			for ( j = 0, ptr = BD(n); j < d; j++ ,i -= DLENGTH ) 
				ptr[j] = myatoi(nbuf+i);
			bnton(DBASE,n,&n1); 
			NTOQ(n1,1,q); r = (Obj)q;
/*			optobj(&r); */
		}
		yylvalp->p = (pointer)r;
		return ( FORMULA );
	} else if ( isalpha(c) ) {
		i = 0;
		tbuf[i++] = c;
		while ( 1 ) {
			c = Getc();
			if ( isalpha(c)||isdigit(c)||(c=='_') ) {
				REALLOC_TBUF tbuf[i++] = c;
			} else
				break;
		}
		REALLOC_TBUF tbuf[i] = 0; Ungetc(c); 
		if ( isupper(tbuf[0]) ) {
			cptr = (char *)MALLOC(strlen(tbuf)+1); strcpy(cptr,tbuf);
			yylvalp->p = (pointer)cptr;
			return UCASE;
		} else {
			for ( i = 0; kwd[i].name && strcmp(tbuf,kwd[i].name); i++ );
			if ( kwd[i].name ) {
				yylvalp->i = asir_infile->ln;
				return kwd[i].token;
			} else {
				cptr = (char *)MALLOC(strlen(tbuf)+1); strcpy(cptr,tbuf);
				yylvalp->p = (pointer)cptr;
				return LCASE;
			}
		}
	} else if ( c == '@' ) {
		if ( isdigit(c = Getc()) ) {
			i = 0;
			nbuf[i++] = c;
			READ_DIGIT_NBUF
			Ungetc(c); REALLOC_NBUF nbuf[i] = 0;
			yylvalp->i = atoi(nbuf);
			return ANS;
		} else if ( c == '@' ) {
			yylvalp->i = MAX(0,APVS->n-1);
			return ANS;
		} else if ( c == '>' ||  c == '<' ||  c == '=' || c == '!' ) {
			if ( (c1 = Getc()) == '=' )
				switch ( c ) {
					case '>': yylvalp->i = (int)L_GE; break;
					case '<': yylvalp->i = (int)L_LE; break;
					case '=': yylvalp->i = (int)L_EQ; break;
					case '!': yylvalp->i = (int)L_NE; break;
					default: break;
				}
			else {
				Ungetc(c1);
				switch ( c ) {
					case '>': yylvalp->i = (int)L_GT; break;
					case '<': yylvalp->i = (int)L_LT; break;
					case '=': yylvalp->i = (int)L_EQ; break;
					case '!': yylvalp->i = (int)L_NOT; return FOP_NOT; break;
					default: break;
				}
			}
			return LOP;
		} else if ( c == '|' ||  c == '&' ) {
			if ( (c1 = Getc()) != c )
				Ungetc(c1);
			switch ( c ) {
				case '|': yylvalp->i = (int)L_OR;
					return FOP_OR; break;
				case '&': yylvalp->i = (int)L_AND;
					return FOP_AND; break;
			}
		} else if ( isalpha(c) ) {
			i = 0;
			tbuf[i++] = '@';
			tbuf[i++] = c;
			while ( 1 ) {
				c = Getc();
				if ( isalpha(c) ) {
					REALLOC_TBUF tbuf[i++] = c;
				} else
					break;
			}
			Ungetc(c); REALLOC_TBUF tbuf[i] = 0;
			if ( !strcmp(tbuf,"@p") )
				return GFPNGEN;
			else if ( !strcmp(tbuf,"@i") ) {
				extern pointer IU;

				yylvalp->p = IU;
				return FORMULA;
			} else if ( !strcmp(tbuf,"@true") ) {
				yylvalp->p = F_TRUE;
				return FORMULA;
			} else if ( !strcmp(tbuf,"@false") ) {
				yylvalp->p = F_FALSE;
				return FORMULA;
			} else if ( !strcmp(tbuf,"@impl") ) {
				yylvalp->i = (int)L_IMPL;
				return FOP_IMPL;
			} else if ( !strcmp(tbuf,"@repl") ) {
				yylvalp->i = (int)L_REPL;
				return FOP_REPL;
			} else if ( !strcmp(tbuf,"@equiv") ) {
				yylvalp->i = (int)L_EQUIV;
				return FOP_EQUIV;
			} else {
				cptr = (char *)MALLOC(strlen(tbuf)+1); strcpy(cptr,tbuf);
				yylvalp->p = (pointer)cptr;
				return LCASE;
			}
		} else {
			Ungetc(c);
			return GF2NGEN;
		}
	} else
		return ( c );
}

static int skipspace() {
	int c,c1;

	for ( c = Getc(); ; )
		switch ( c ) {
			case ' ': case '\t':
				c = Getc(); break;
			case '\n':
				c = afternl();	break;
			case '/':
				if ( (c1 = Getc()) == '*' )
					c = aftercomment();
				else {
					Ungetc(c1); return c;
				}
				break;
			default:
				return c; break;
		}
}

int afternl() {
	int c,ac,i,quote;
	char *ptr;
	char *av[BUFSIZ];
	static int ilevel = 0;
	char buf[BUFSIZ];

	if ( !ilevel )
		asir_infile->ln++;
	while ( (c = Getc()) == '#' ) {
		Gets(buf);
		for ( quote = 0, ptr = buf; *ptr; ptr++ )
			if ( *ptr == '"' )
				quote = quote ? 0 : 1;
			else if ( quote && (*ptr == ' ') )
				*ptr = '_';
		stoarg(buf,&ac,av);
		if ( ac == 3 )
			if ( (i = atoi(av[2])) == 1 )
				ilevel++;
			else if ( i == 2 )
				ilevel--;
		if ( !ilevel )
			asir_infile->ln = atoi(av[0]);
	}
	return c;
}

int aftercomment() {
	int c,c1;

	for ( c = Getc(); ; ) {
		c1 = Getc();
		if ( (c == '*') && (c1 == '/') )
			return Getc();
		else
			c = c1;
	}
}
			
int myatoi(s)
char *s;
{
	int i,r;
	for ( i = 0, r = 0; i < DLENGTH; i++ ) r = r * 10 + ( s[i] - '0' );
	return ( r );
}

extern int ox_do_copy;

void yyerror(s)
char *s;
{
	if ( main_parser )
		if ( ox_do_copy ) {
			/* push errors to DebugStack */
		} else {
			if ( asir_infile->fp == stdin )
				fprintf(stderr,"%s\n",s);
			else
				fprintf(stderr,"\"%s\", near line %d: %s\n",asir_infile->name,asir_infile->ln,s);
		}
	else
		fprintf(stderr,"exprparse : %s\n",s);
}

int echoback;

extern int read_exec_file, do_fep, do_file;

int readline_getc();
void readline_ungetc();
int Egetc();
void Eungetc();

unsigned char encrypt_char(unsigned char);
unsigned char decrypt_char(unsigned char);

int Egetc(fp)
FILE *fp;
{
	int c;

	if ( fp ) {
		c = getc(fp);
		if ( c == EOF )
			return c;
		if ( asir_infile->encoded )
			c = decrypt_char((unsigned char)c);
		return c;
	} else {
		c = *parse_strp++;
		if ( !c )
			return EOF;
		else
			return c;
	}
}

void Eungetc(c,fp)
int c;
FILE *fp;
{
	if ( fp ) {
		if ( asir_infile->encoded )
			c = (int)encrypt_char((unsigned char)c);
		ungetc(c,fp);
	} else
		*--parse_strp = c;
}

static int Getc() {
	int c;

	if ( main_parser ) {
		while ( 1 ) {
			if ((c = Egetc(asir_infile->fp)) == EOF)
				if ( NEXT(asir_infile) ) {
					closecurrentinput();
					c = Getc();
					break;
				} else if ( read_exec_file || do_file )
					asir_terminate(2);
				else {
					if ( asir_infile->fp )
						clearerr(asir_infile->fp);
				}
			else
				break;
		}
		if ( echoback )
			fputc(c,asir_out);
	} else
		c = *parse_strp++;
	return ( c );
}

static void Ungetc(c) {
	if ( main_parser ) {
		Eungetc(c,asir_infile->fp);
		if ( echoback )
			fputc('',asir_out);
	} else
		*--parse_strp = c;
}

static void Gets(s)
char *s;
{
	int c;

	while ( (c = Getc()) != '\n' )
		*s++ = c;
	*s = 0;
}
