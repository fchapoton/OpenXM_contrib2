! $OpenXM$
#include "base.h"

	.seg	"text"			! [internal]
	.proc	14
	.global	dm
dm:
	save	%sp,-104,%sp
	mov	%i1,%o1
	call	.umul,2
	mov	%i0,%o0
	
	sll	%o1,CBSH,%o1
	srl	%o0,BSH,%o2
	or	%o1,%o2,%o1
	sethi	%hi(BMASK),%o2
	add	%o2,%lo(BMASK),%o2
	and	%o0,%o2,%o0

	st	%o1,[%i2]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dmb
dmb:
	save	%sp,-104,%sp
	mov	%i2,%o1
	call	.umul,2
	mov	%i1,%o0

	call	edr,3
	mov	%i0,%o2

	st	%o1,[%i3]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dma
dma:
	save	%sp,-104,%sp
	mov	%i1,%o1
	call	.umul,2
	mov	%i0,%o0

	addcc	%o0,%i2,%o0
	addx	%g0,%o1,%o1

	sll	%o1,CBSH,%o1
	srl	%o0,BSH,%o2
	or	%o1,%o2,%o1
	sethi	%hi(BMASK),%o2
	add	%o2,%lo(BMASK),%o2
	and	%o0,%o2,%o0

	st	%o1,[%i3]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dmab
dmab:
	save	%sp,-104,%sp
	mov	%i2,%o1
	call	.umul,2
	mov	%i1,%o0

	addcc	%o0,%i3,%o0
	addx	%g0,%o1,%o1

	call	edr,3
	mov	%i0,%o2

	st	%o1,[%i4]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dsab
dsab:
	save	%sp,-104,%sp
	srl	%i1,CBSH,%o1
	sll	%i1,BSH,%o0
	or	%i2,%o0,%o0

	call	edr,3
	mov	%i0,%o2

	st	%o1,[%i3]
	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dmar
dmar:
	save	%sp,-104,%sp
	mov	%i1,%o1
	call	.umul,2
	mov	%i0,%o0

	addcc	%o0,%i2,%o0
	addx	%g0,%o1,%o1

	call	edr,3
	mov	%i3,%o2

	ret
	restore	%g0,%o0,%o0

	.proc	14
	.global	dqr
dqr:
	mov %o7,%g5 !save %o7
	call .udr,2
	mov %o2,%g6 !save %o2

	st %o1,[%g6]
	mov %g5,%o7

	retl
	nop
	.seg	"data"
