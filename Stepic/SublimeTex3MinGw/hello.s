	.file	"hello.cpp"
	.section .rdata,"dr"
LC0:
	.ascii "hello\0"
	.text
	.globl	__Z5hellov
	.def	__Z5hellov;	.scl	2;	.type	32;	.endef
__Z5hellov:
LFB28:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$24, %esp
	movl	$LC0, (%esp)
	call	_printf
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
LFE28:
	.ident	"GCC: (i686-posix-dwarf-rev2, Built by MinGW-W64 project) 4.9.1"
	.def	_printf;	.scl	2;	.type	32;	.endef
