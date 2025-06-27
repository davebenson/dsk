	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 13
	.file	1 "/usr/include/_types" "_uint64_t.h"
	.globl	_dsk_tls_bignum_multiply ## -- Begin function dsk_tls_bignum_multiply
	.p2align	4, 0x90
_dsk_tls_bignum_multiply:               ## @dsk_tls_bignum_multiply
Lfunc_begin0:
	.file	2 "dsk_tls_bignum_multiply.c"
	.loc	2 32 0                  ## dsk_tls_bignum_multiply.c:32:0
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$144, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movl	%edx, -20(%rbp)
	movq	%rcx, -32(%rbp)
	movq	%r8, -40(%rbp)
Ltmp0:
	.loc	2 34 7 prologue_end     ## dsk_tls_bignum_multiply.c:34:7
	movl	-4(%rbp), %edx
	.loc	2 34 13 is_stmt 0       ## dsk_tls_bignum_multiply.c:34:13
	cmpl	-20(%rbp), %edx
Ltmp1:
	.loc	2 34 7                  ## dsk_tls_bignum_multiply.c:34:7
	jae	LBB0_2
## %bb.1:
Ltmp2:
	.loc	2 36 26 is_stmt 1       ## dsk_tls_bignum_multiply.c:36:26
	movl	-4(%rbp), %eax
	.loc	2 36 16 is_stmt 0       ## dsk_tls_bignum_multiply.c:36:16
	movl	%eax, -44(%rbp)
	.loc	2 36 41                 ## dsk_tls_bignum_multiply.c:36:41
	movl	-20(%rbp), %eax
	.loc	2 36 39                 ## dsk_tls_bignum_multiply.c:36:39
	movl	%eax, -4(%rbp)
	.loc	2 36 56                 ## dsk_tls_bignum_multiply.c:36:56
	movl	-44(%rbp), %eax
	.loc	2 36 54                 ## dsk_tls_bignum_multiply.c:36:54
	movl	%eax, -20(%rbp)
	.loc	2 37 34 is_stmt 1       ## dsk_tls_bignum_multiply.c:37:34
	movq	-16(%rbp), %rcx
	.loc	2 37 23 is_stmt 0       ## dsk_tls_bignum_multiply.c:37:23
	movq	%rcx, -56(%rbp)
	.loc	2 37 51                 ## dsk_tls_bignum_multiply.c:37:51
	movq	-32(%rbp), %rcx
	.loc	2 37 49                 ## dsk_tls_bignum_multiply.c:37:49
	movq	%rcx, -16(%rbp)
	.loc	2 37 68                 ## dsk_tls_bignum_multiply.c:37:68
	movq	-56(%rbp), %rcx
	.loc	2 37 66                 ## dsk_tls_bignum_multiply.c:37:66
	movq	%rcx, -32(%rbp)
Ltmp3:
LBB0_2:
	.loc	2 42 13 is_stmt 1       ## dsk_tls_bignum_multiply.c:42:13
	cmpl	$0, -20(%rbp)
Ltmp4:
	.loc	2 42 7 is_stmt 0        ## dsk_tls_bignum_multiply.c:42:7
	jne	LBB0_4
## %bb.3:
	.loc	2 0 7                   ## dsk_tls_bignum_multiply.c:0:7
	xorl	%esi, %esi
	movq	$-1, %rcx
Ltmp5:
	.loc	2 44 7 is_stmt 1        ## dsk_tls_bignum_multiply.c:44:7
	movq	-40(%rbp), %rax
	movl	-4(%rbp), %edx
	shll	$2, %edx
	movl	%edx, %edx
                                        ## kill: def %rdx killed %edx
	movq	%rax, %rdi
	callq	___memset_chk
	.loc	2 45 7                  ## dsk_tls_bignum_multiply.c:45:7
	movq	%rax, -96(%rbp)         ## 8-byte Spill
	jmp	LBB0_17
Ltmp6:
LBB0_4:
	.loc	2 47 13                 ## dsk_tls_bignum_multiply.c:47:13
	cmpl	$1, -20(%rbp)
Ltmp7:
	.loc	2 47 7 is_stmt 0        ## dsk_tls_bignum_multiply.c:47:7
	jne	LBB0_9
## %bb.5:
Ltmp8:
	.loc	2 49 51 is_stmt 1       ## dsk_tls_bignum_multiply.c:49:51
	movq	-32(%rbp), %rax
	movl	(%rax), %edi
	.loc	2 50 51                 ## dsk_tls_bignum_multiply.c:50:51
	movl	-4(%rbp), %ecx
	.loc	2 50 57 is_stmt 0       ## dsk_tls_bignum_multiply.c:50:57
	shrl	$1, %ecx
	.loc	2 51 64 is_stmt 1       ## dsk_tls_bignum_multiply.c:51:64
	movq	-16(%rbp), %rax
	.loc	2 52 64                 ## dsk_tls_bignum_multiply.c:52:64
	movq	-40(%rbp), %rdx
	.loc	2 49 26                 ## dsk_tls_bignum_multiply.c:49:26
	movl	%ecx, %esi
	movq	%rdx, -104(%rbp)        ## 8-byte Spill
	movq	%rax, %rdx
	movq	-104(%rbp), %rcx        ## 8-byte Reload
	callq	_dsk_tls_bignum_mul64word
	.loc	2 49 16 is_stmt 0       ## dsk_tls_bignum_multiply.c:49:16
	movl	%eax, -60(%rbp)
Ltmp9:
	.loc	2 53 11 is_stmt 1       ## dsk_tls_bignum_multiply.c:53:11
	movl	-4(%rbp), %eax
	.loc	2 53 17 is_stmt 0       ## dsk_tls_bignum_multiply.c:53:17
	andl	$1, %eax
	cmpl	$0, %eax
Ltmp10:
	.loc	2 53 11                 ## dsk_tls_bignum_multiply.c:53:11
	je	LBB0_7
## %bb.6:
Ltmp11:
	.loc	2 55 26 is_stmt 1       ## dsk_tls_bignum_multiply.c:55:26
	movq	-32(%rbp), %rax
	.loc	2 55 15 is_stmt 0       ## dsk_tls_bignum_multiply.c:55:15
	movl	(%rax), %ecx
	movl	%ecx, %eax
	.loc	2 55 38                 ## dsk_tls_bignum_multiply.c:55:38
	movq	-16(%rbp), %rdx
	.loc	2 55 45                 ## dsk_tls_bignum_multiply.c:55:45
	movl	-4(%rbp), %ecx
	.loc	2 55 50                 ## dsk_tls_bignum_multiply.c:55:50
	subl	$1, %ecx
	.loc	2 55 38                 ## dsk_tls_bignum_multiply.c:55:38
	movl	%ecx, %ecx
	movl	%ecx, %esi
	movl	(%rdx,%rsi,4), %ecx
	movl	%ecx, %edx
	.loc	2 55 36                 ## dsk_tls_bignum_multiply.c:55:36
	imulq	%rdx, %rax
	.loc	2 55 56                 ## dsk_tls_bignum_multiply.c:55:56
	movl	-60(%rbp), %ecx
	movl	%ecx, %edx
	.loc	2 55 54                 ## dsk_tls_bignum_multiply.c:55:54
	addq	%rdx, %rax
	.loc	2 54 22 is_stmt 1       ## dsk_tls_bignum_multiply.c:54:22
	movq	-40(%rbp), %rdx
	.loc	2 54 27 is_stmt 0       ## dsk_tls_bignum_multiply.c:54:27
	movl	-4(%rbp), %ecx
	.loc	2 54 32                 ## dsk_tls_bignum_multiply.c:54:32
	shrl	$1, %ecx
	.loc	2 54 9                  ## dsk_tls_bignum_multiply.c:54:9
	movl	%ecx, %ecx
	movl	%ecx, %esi
	.loc	2 55 13 is_stmt 1       ## dsk_tls_bignum_multiply.c:55:13
	movq	%rax, (%rdx,%rsi,8)
	.loc	2 54 9                  ## dsk_tls_bignum_multiply.c:54:9
	jmp	LBB0_8
LBB0_7:
	.loc	2 57 22                 ## dsk_tls_bignum_multiply.c:57:22
	movl	-60(%rbp), %eax
	.loc	2 57 9 is_stmt 0        ## dsk_tls_bignum_multiply.c:57:9
	movq	-40(%rbp), %rcx
	movl	-4(%rbp), %edx
	movl	%edx, %esi
	.loc	2 57 20                 ## dsk_tls_bignum_multiply.c:57:20
	movl	%eax, (%rcx,%rsi,4)
Ltmp12:
LBB0_8:
	.loc	2 58 7 is_stmt 1        ## dsk_tls_bignum_multiply.c:58:7
	jmp	LBB0_17
Ltmp13:
LBB0_9:
	.loc	2 66 30                 ## dsk_tls_bignum_multiply.c:66:30
	movl	-4(%rbp), %eax
	.loc	2 66 35 is_stmt 0       ## dsk_tls_bignum_multiply.c:66:35
	shrl	$1, %eax
	.loc	2 66 58                 ## dsk_tls_bignum_multiply.c:66:58
	movq	-16(%rbp), %rcx
	.loc	2 67 30 is_stmt 1       ## dsk_tls_bignum_multiply.c:67:30
	movl	-20(%rbp), %edx
	.loc	2 67 35 is_stmt 0       ## dsk_tls_bignum_multiply.c:67:35
	shrl	$1, %edx
	.loc	2 67 58                 ## dsk_tls_bignum_multiply.c:67:58
	movq	-32(%rbp), %rsi
	.loc	2 68 43 is_stmt 1       ## dsk_tls_bignum_multiply.c:68:43
	movq	-40(%rbp), %rdi
	.loc	2 66 3                  ## dsk_tls_bignum_multiply.c:66:3
	movq	%rdi, -112(%rbp)        ## 8-byte Spill
	movl	%eax, %edi
	movq	%rsi, -120(%rbp)        ## 8-byte Spill
	movq	%rcx, %rsi
	movq	-120(%rbp), %rcx        ## 8-byte Reload
	movq	-112(%rbp), %r8         ## 8-byte Reload
	callq	_dsk_tls_bignum_multiply64
Ltmp14:
	.loc	2 77 9                  ## dsk_tls_bignum_multiply.c:77:9
	movl	-4(%rbp), %eax
	.loc	2 77 15 is_stmt 0       ## dsk_tls_bignum_multiply.c:77:15
	orl	-20(%rbp), %eax
	.loc	2 77 24                 ## dsk_tls_bignum_multiply.c:77:24
	andl	$1, %eax
	.loc	2 77 29                 ## dsk_tls_bignum_multiply.c:77:29
	cmpl	$0, %eax
Ltmp15:
	.loc	2 77 7                  ## dsk_tls_bignum_multiply.c:77:7
	jne	LBB0_11
## %bb.10:
Ltmp16:
	.loc	2 78 5 is_stmt 1        ## dsk_tls_bignum_multiply.c:78:5
	jmp	LBB0_17
Ltmp17:
LBB0_11:
	.loc	2 88 7                  ## dsk_tls_bignum_multiply.c:88:7
	movl	-4(%rbp), %eax
	.loc	2 88 13 is_stmt 0       ## dsk_tls_bignum_multiply.c:88:13
	andl	$1, %eax
	cmpl	$0, %eax
Ltmp18:
	.loc	2 88 7                  ## dsk_tls_bignum_multiply.c:88:7
	je	LBB0_16
## %bb.12:
Ltmp19:
	.loc	2 91 9 is_stmt 1        ## dsk_tls_bignum_multiply.c:91:9
	movq	-16(%rbp), %rax
	.loc	2 91 16 is_stmt 0       ## dsk_tls_bignum_multiply.c:91:16
	movl	-4(%rbp), %ecx
	.loc	2 91 22                 ## dsk_tls_bignum_multiply.c:91:22
	subl	$1, %ecx
	.loc	2 91 9                  ## dsk_tls_bignum_multiply.c:91:9
	movl	%ecx, %ecx
	movl	%ecx, %edx
	movl	(%rax,%rdx,4), %edi
	.loc	2 92 9 is_stmt 1        ## dsk_tls_bignum_multiply.c:92:9
	movl	-20(%rbp), %ecx
	.loc	2 92 15 is_stmt 0       ## dsk_tls_bignum_multiply.c:92:15
	shrl	$1, %ecx
	.loc	2 93 28 is_stmt 1       ## dsk_tls_bignum_multiply.c:93:28
	movq	-32(%rbp), %rax
	.loc	2 94 23                 ## dsk_tls_bignum_multiply.c:94:23
	movq	-40(%rbp), %rdx
	.loc	2 94 29 is_stmt 0       ## dsk_tls_bignum_multiply.c:94:29
	movl	-4(%rbp), %esi
	.loc	2 94 35                 ## dsk_tls_bignum_multiply.c:94:35
	shrl	$1, %esi
	.loc	2 94 39                 ## dsk_tls_bignum_multiply.c:94:39
	shll	$1, %esi
	.loc	2 94 27                 ## dsk_tls_bignum_multiply.c:94:27
	movl	%esi, %esi
	movl	%esi, %r8d
	shlq	$2, %r8
	addq	%r8, %rdx
	.loc	2 90 26 is_stmt 1       ## dsk_tls_bignum_multiply.c:90:26
	movl	%ecx, %esi
	movq	%rdx, -128(%rbp)        ## 8-byte Spill
	movq	%rax, %rdx
	movq	-128(%rbp), %rcx        ## 8-byte Reload
	callq	_dsk_tls_bignum_mul64word_addto
	.loc	2 90 16 is_stmt 0       ## dsk_tls_bignum_multiply.c:90:16
	movl	%eax, -64(%rbp)
Ltmp20:
	.loc	2 96 11 is_stmt 1       ## dsk_tls_bignum_multiply.c:96:11
	movl	-20(%rbp), %eax
	.loc	2 96 17 is_stmt 0       ## dsk_tls_bignum_multiply.c:96:17
	andl	$1, %eax
	cmpl	$0, %eax
Ltmp21:
	.loc	2 96 11                 ## dsk_tls_bignum_multiply.c:96:11
	je	LBB0_14
## %bb.13:
Ltmp22:
	.loc	2 99 13 is_stmt 1       ## dsk_tls_bignum_multiply.c:99:13
	movq	-32(%rbp), %rax
	.loc	2 99 20 is_stmt 0       ## dsk_tls_bignum_multiply.c:99:20
	movl	-20(%rbp), %ecx
	.loc	2 99 26                 ## dsk_tls_bignum_multiply.c:99:26
	subl	$1, %ecx
	.loc	2 99 13                 ## dsk_tls_bignum_multiply.c:99:13
	movl	%ecx, %ecx
	movl	%ecx, %edx
	movl	(%rax,%rdx,4), %edi
	.loc	2 100 13 is_stmt 1      ## dsk_tls_bignum_multiply.c:100:13
	movl	-4(%rbp), %ecx
	.loc	2 100 19 is_stmt 0      ## dsk_tls_bignum_multiply.c:100:19
	shrl	$1, %ecx
	.loc	2 101 32 is_stmt 1      ## dsk_tls_bignum_multiply.c:101:32
	movq	-16(%rbp), %rax
	.loc	2 102 27                ## dsk_tls_bignum_multiply.c:102:27
	movq	-40(%rbp), %rdx
	.loc	2 102 33 is_stmt 0      ## dsk_tls_bignum_multiply.c:102:33
	movl	-20(%rbp), %esi
	.loc	2 102 39                ## dsk_tls_bignum_multiply.c:102:39
	shrl	$1, %esi
	.loc	2 102 43                ## dsk_tls_bignum_multiply.c:102:43
	shll	$1, %esi
	.loc	2 102 31                ## dsk_tls_bignum_multiply.c:102:31
	movl	%esi, %esi
	movl	%esi, %r8d
	shlq	$2, %r8
	addq	%r8, %rdx
	.loc	2 98 30 is_stmt 1       ## dsk_tls_bignum_multiply.c:98:30
	movl	%ecx, %esi
	movq	%rdx, -136(%rbp)        ## 8-byte Spill
	movq	%rax, %rdx
	movq	-136(%rbp), %rcx        ## 8-byte Reload
	callq	_dsk_tls_bignum_mul64word_addto
	.loc	2 98 20 is_stmt 0       ## dsk_tls_bignum_multiply.c:98:20
	movl	%eax, -68(%rbp)
	.loc	2 104 39 is_stmt 1      ## dsk_tls_bignum_multiply.c:104:39
	movq	-16(%rbp), %rcx
	.loc	2 104 46 is_stmt 0      ## dsk_tls_bignum_multiply.c:104:46
	movl	-4(%rbp), %eax
	.loc	2 104 52                ## dsk_tls_bignum_multiply.c:104:52
	subl	$1, %eax
	.loc	2 104 39                ## dsk_tls_bignum_multiply.c:104:39
	movl	%eax, %eax
	movl	%eax, %edx
	.loc	2 104 28                ## dsk_tls_bignum_multiply.c:104:28
	movl	(%rcx,%rdx,4), %eax
	movl	%eax, %ecx
	.loc	2 104 59                ## dsk_tls_bignum_multiply.c:104:59
	movq	-32(%rbp), %rdx
	.loc	2 104 66                ## dsk_tls_bignum_multiply.c:104:66
	movl	-20(%rbp), %eax
	.loc	2 104 72                ## dsk_tls_bignum_multiply.c:104:72
	subl	$1, %eax
	.loc	2 104 59                ## dsk_tls_bignum_multiply.c:104:59
	movl	%eax, %eax
	movl	%eax, %r8d
	movl	(%rdx,%r8,4), %eax
	movl	%eax, %edx
	.loc	2 104 57                ## dsk_tls_bignum_multiply.c:104:57
	imulq	%rdx, %rcx
	.loc	2 105 28 is_stmt 1      ## dsk_tls_bignum_multiply.c:105:28
	movl	-64(%rbp), %eax
	movl	%eax, %edx
	.loc	2 105 26 is_stmt 0      ## dsk_tls_bignum_multiply.c:105:26
	addq	%rdx, %rcx
	.loc	2 106 28 is_stmt 1      ## dsk_tls_bignum_multiply.c:106:28
	movl	-68(%rbp), %eax
	movl	%eax, %edx
	.loc	2 106 26 is_stmt 0      ## dsk_tls_bignum_multiply.c:106:26
	addq	%rdx, %rcx
	.loc	2 104 20 is_stmt 1      ## dsk_tls_bignum_multiply.c:104:20
	movq	%rcx, -80(%rbp)
	.loc	2 107 51                ## dsk_tls_bignum_multiply.c:107:51
	movq	-80(%rbp), %rcx
	.loc	2 107 25 is_stmt 0      ## dsk_tls_bignum_multiply.c:107:25
	movq	-40(%rbp), %rdx
	.loc	2 107 30                ## dsk_tls_bignum_multiply.c:107:30
	movl	-4(%rbp), %eax
	.loc	2 107 35                ## dsk_tls_bignum_multiply.c:107:35
	shrl	$1, %eax
	.loc	2 107 40                ## dsk_tls_bignum_multiply.c:107:40
	movl	-20(%rbp), %esi
	.loc	2 107 45                ## dsk_tls_bignum_multiply.c:107:45
	shrl	$1, %esi
	.loc	2 107 38                ## dsk_tls_bignum_multiply.c:107:38
	addl	%esi, %eax
	.loc	2 107 11                ## dsk_tls_bignum_multiply.c:107:11
	movl	%eax, %eax
	movl	%eax, %r8d
	.loc	2 107 49                ## dsk_tls_bignum_multiply.c:107:49
	movq	%rcx, (%rdx,%r8,8)
	.loc	2 108 9 is_stmt 1       ## dsk_tls_bignum_multiply.c:108:9
	jmp	LBB0_15
Ltmp23:
LBB0_14:
	.loc	2 111 36                ## dsk_tls_bignum_multiply.c:111:36
	movl	-64(%rbp), %eax
	.loc	2 111 11 is_stmt 0      ## dsk_tls_bignum_multiply.c:111:11
	movq	-40(%rbp), %rcx
	.loc	2 111 15                ## dsk_tls_bignum_multiply.c:111:15
	movl	-4(%rbp), %edx
	.loc	2 111 21                ## dsk_tls_bignum_multiply.c:111:21
	addl	-20(%rbp), %edx
	.loc	2 111 29                ## dsk_tls_bignum_multiply.c:111:29
	subl	$1, %edx
	.loc	2 111 11                ## dsk_tls_bignum_multiply.c:111:11
	movl	%edx, %edx
	movl	%edx, %esi
	.loc	2 111 34                ## dsk_tls_bignum_multiply.c:111:34
	movl	%eax, (%rcx,%rsi,4)
Ltmp24:
LBB0_15:
	.loc	2 113 5 is_stmt 1       ## dsk_tls_bignum_multiply.c:113:5
	jmp	LBB0_17
Ltmp25:
LBB0_16:
	.loc	2 118 9                 ## dsk_tls_bignum_multiply.c:118:9
	movq	-32(%rbp), %rax
	.loc	2 118 16 is_stmt 0      ## dsk_tls_bignum_multiply.c:118:16
	movl	-20(%rbp), %ecx
	.loc	2 118 22                ## dsk_tls_bignum_multiply.c:118:22
	subl	$1, %ecx
	.loc	2 118 9                 ## dsk_tls_bignum_multiply.c:118:9
	movl	%ecx, %ecx
	movl	%ecx, %edx
	movl	(%rax,%rdx,4), %edi
	.loc	2 119 9 is_stmt 1       ## dsk_tls_bignum_multiply.c:119:9
	movl	-4(%rbp), %ecx
	.loc	2 119 15 is_stmt 0      ## dsk_tls_bignum_multiply.c:119:15
	shrl	$1, %ecx
	.loc	2 120 28 is_stmt 1      ## dsk_tls_bignum_multiply.c:120:28
	movq	-16(%rbp), %rax
	.loc	2 121 23                ## dsk_tls_bignum_multiply.c:121:23
	movq	-40(%rbp), %rdx
	.loc	2 121 29 is_stmt 0      ## dsk_tls_bignum_multiply.c:121:29
	movl	-20(%rbp), %esi
	.loc	2 121 35                ## dsk_tls_bignum_multiply.c:121:35
	shrl	$1, %esi
	.loc	2 121 39                ## dsk_tls_bignum_multiply.c:121:39
	shll	$1, %esi
	.loc	2 121 27                ## dsk_tls_bignum_multiply.c:121:27
	movl	%esi, %esi
	movl	%esi, %r8d
	shlq	$2, %r8
	addq	%r8, %rdx
	.loc	2 117 26 is_stmt 1      ## dsk_tls_bignum_multiply.c:117:26
	movl	%ecx, %esi
	movq	%rdx, -144(%rbp)        ## 8-byte Spill
	movq	%rax, %rdx
	movq	-144(%rbp), %rcx        ## 8-byte Reload
	callq	_dsk_tls_bignum_mul64word_addto
	.loc	2 117 16 is_stmt 0      ## dsk_tls_bignum_multiply.c:117:16
	movl	%eax, -84(%rbp)
	.loc	2 123 32 is_stmt 1      ## dsk_tls_bignum_multiply.c:123:32
	movl	-84(%rbp), %eax
	.loc	2 123 7 is_stmt 0       ## dsk_tls_bignum_multiply.c:123:7
	movq	-40(%rbp), %rcx
	.loc	2 123 11                ## dsk_tls_bignum_multiply.c:123:11
	movl	-4(%rbp), %esi
	.loc	2 123 17                ## dsk_tls_bignum_multiply.c:123:17
	addl	-20(%rbp), %esi
	.loc	2 123 25                ## dsk_tls_bignum_multiply.c:123:25
	subl	$1, %esi
	.loc	2 123 7                 ## dsk_tls_bignum_multiply.c:123:7
	movl	%esi, %esi
	movl	%esi, %edx
	.loc	2 123 30                ## dsk_tls_bignum_multiply.c:123:30
	movl	%eax, (%rcx,%rdx,4)
Ltmp26:
LBB0_17:
	.loc	2 125 1 is_stmt 1       ## dsk_tls_bignum_multiply.c:125:1
	addq	$144, %rsp
	popq	%rbp
	retq
Ltmp27:
Lfunc_end0:
	.cfi_endproc
                                        ## -- End function
	.file	3 "/usr/include/_types" "_uint32_t.h"
	.section	__DWARF,__debug_str,regular,debug
Linfo_string:
	.asciz	"Apple LLVM version 10.0.0 (clang-1000.11.45.5)" ## string offset=0
	.asciz	"dsk_tls_bignum_multiply.c" ## string offset=47
	.asciz	"/Users/daveb/progs/dsk/tls/amd64" ## string offset=73
	.asciz	"uint64_t"              ## string offset=106
	.asciz	"long long unsigned int" ## string offset=115
	.asciz	"dsk_tls_bignum_multiply" ## string offset=138
	.asciz	"a_len"                 ## string offset=162
	.asciz	"unsigned int"          ## string offset=168
	.asciz	"a_data"                ## string offset=181
	.asciz	"uint32_t"              ## string offset=188
	.asciz	"b_len"                 ## string offset=197
	.asciz	"b_data"                ## string offset=203
	.asciz	"out"                   ## string offset=210
	.asciz	"tmp_len"               ## string offset=214
	.asciz	"tmp_data"              ## string offset=222
	.asciz	"carry_a"               ## string offset=231
	.asciz	"carry_b"               ## string offset=239
	.asciz	"fixup"                 ## string offset=247
	.section	__DWARF,__debug_abbrev,regular,debug
Lsection_abbrev:
	.byte	1                       ## Abbreviation Code
	.byte	17                      ## DW_TAG_compile_unit
	.byte	1                       ## DW_CHILDREN_yes
	.byte	37                      ## DW_AT_producer
	.byte	14                      ## DW_FORM_strp
	.byte	19                      ## DW_AT_language
	.byte	5                       ## DW_FORM_data2
	.byte	3                       ## DW_AT_name
	.byte	14                      ## DW_FORM_strp
	.byte	16                      ## DW_AT_stmt_list
	.byte	23                      ## DW_FORM_sec_offset
	.byte	27                      ## DW_AT_comp_dir
	.byte	14                      ## DW_FORM_strp
	.byte	17                      ## DW_AT_low_pc
	.byte	1                       ## DW_FORM_addr
	.byte	18                      ## DW_AT_high_pc
	.byte	6                       ## DW_FORM_data4
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	2                       ## Abbreviation Code
	.byte	15                      ## DW_TAG_pointer_type
	.byte	0                       ## DW_CHILDREN_no
	.byte	73                      ## DW_AT_type
	.byte	19                      ## DW_FORM_ref4
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	3                       ## Abbreviation Code
	.byte	22                      ## DW_TAG_typedef
	.byte	0                       ## DW_CHILDREN_no
	.byte	73                      ## DW_AT_type
	.byte	19                      ## DW_FORM_ref4
	.byte	3                       ## DW_AT_name
	.byte	14                      ## DW_FORM_strp
	.byte	58                      ## DW_AT_decl_file
	.byte	11                      ## DW_FORM_data1
	.byte	59                      ## DW_AT_decl_line
	.byte	11                      ## DW_FORM_data1
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	4                       ## Abbreviation Code
	.byte	36                      ## DW_TAG_base_type
	.byte	0                       ## DW_CHILDREN_no
	.byte	3                       ## DW_AT_name
	.byte	14                      ## DW_FORM_strp
	.byte	62                      ## DW_AT_encoding
	.byte	11                      ## DW_FORM_data1
	.byte	11                      ## DW_AT_byte_size
	.byte	11                      ## DW_FORM_data1
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	5                       ## Abbreviation Code
	.byte	38                      ## DW_TAG_const_type
	.byte	0                       ## DW_CHILDREN_no
	.byte	73                      ## DW_AT_type
	.byte	19                      ## DW_FORM_ref4
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	6                       ## Abbreviation Code
	.byte	46                      ## DW_TAG_subprogram
	.byte	1                       ## DW_CHILDREN_yes
	.byte	17                      ## DW_AT_low_pc
	.byte	1                       ## DW_FORM_addr
	.byte	18                      ## DW_AT_high_pc
	.byte	6                       ## DW_FORM_data4
	.byte	64                      ## DW_AT_frame_base
	.byte	24                      ## DW_FORM_exprloc
	.byte	3                       ## DW_AT_name
	.byte	14                      ## DW_FORM_strp
	.byte	58                      ## DW_AT_decl_file
	.byte	11                      ## DW_FORM_data1
	.byte	59                      ## DW_AT_decl_line
	.byte	11                      ## DW_FORM_data1
	.byte	39                      ## DW_AT_prototyped
	.byte	25                      ## DW_FORM_flag_present
	.byte	63                      ## DW_AT_external
	.byte	25                      ## DW_FORM_flag_present
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	7                       ## Abbreviation Code
	.byte	5                       ## DW_TAG_formal_parameter
	.byte	0                       ## DW_CHILDREN_no
	.byte	2                       ## DW_AT_location
	.byte	24                      ## DW_FORM_exprloc
	.byte	3                       ## DW_AT_name
	.byte	14                      ## DW_FORM_strp
	.byte	58                      ## DW_AT_decl_file
	.byte	11                      ## DW_FORM_data1
	.byte	59                      ## DW_AT_decl_line
	.byte	11                      ## DW_FORM_data1
	.byte	73                      ## DW_AT_type
	.byte	19                      ## DW_FORM_ref4
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	8                       ## Abbreviation Code
	.byte	11                      ## DW_TAG_lexical_block
	.byte	1                       ## DW_CHILDREN_yes
	.byte	17                      ## DW_AT_low_pc
	.byte	1                       ## DW_FORM_addr
	.byte	18                      ## DW_AT_high_pc
	.byte	6                       ## DW_FORM_data4
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	9                       ## Abbreviation Code
	.byte	52                      ## DW_TAG_variable
	.byte	0                       ## DW_CHILDREN_no
	.byte	2                       ## DW_AT_location
	.byte	24                      ## DW_FORM_exprloc
	.byte	3                       ## DW_AT_name
	.byte	14                      ## DW_FORM_strp
	.byte	58                      ## DW_AT_decl_file
	.byte	11                      ## DW_FORM_data1
	.byte	59                      ## DW_AT_decl_line
	.byte	11                      ## DW_FORM_data1
	.byte	73                      ## DW_AT_type
	.byte	19                      ## DW_FORM_ref4
	.byte	0                       ## EOM(1)
	.byte	0                       ## EOM(2)
	.byte	0                       ## EOM(3)
	.section	__DWARF,__debug_info,regular,debug
Lsection_info:
Lcu_begin0:
	.long	368                     ## Length of Unit
	.short	4                       ## DWARF version number
Lset0 = Lsection_abbrev-Lsection_abbrev ## Offset Into Abbrev. Section
	.long	Lset0
	.byte	8                       ## Address Size (in bytes)
	.byte	1                       ## Abbrev [1] 0xb:0x169 DW_TAG_compile_unit
	.long	0                       ## DW_AT_producer
	.short	12                      ## DW_AT_language
	.long	47                      ## DW_AT_name
Lset1 = Lline_table_start0-Lsection_line ## DW_AT_stmt_list
	.long	Lset1
	.long	73                      ## DW_AT_comp_dir
	.quad	Lfunc_begin0            ## DW_AT_low_pc
Lset2 = Lfunc_end0-Lfunc_begin0         ## DW_AT_high_pc
	.long	Lset2
	.byte	2                       ## Abbrev [2] 0x2a:0x5 DW_TAG_pointer_type
	.long	47                      ## DW_AT_type
	.byte	3                       ## Abbrev [3] 0x2f:0xb DW_TAG_typedef
	.long	58                      ## DW_AT_type
	.long	106                     ## DW_AT_name
	.byte	1                       ## DW_AT_decl_file
	.byte	31                      ## DW_AT_decl_line
	.byte	4                       ## Abbrev [4] 0x3a:0x7 DW_TAG_base_type
	.long	115                     ## DW_AT_name
	.byte	7                       ## DW_AT_encoding
	.byte	8                       ## DW_AT_byte_size
	.byte	2                       ## Abbrev [2] 0x41:0x5 DW_TAG_pointer_type
	.long	70                      ## DW_AT_type
	.byte	5                       ## Abbrev [5] 0x46:0x5 DW_TAG_const_type
	.long	47                      ## DW_AT_type
	.byte	6                       ## Abbrev [6] 0x4b:0x107 DW_TAG_subprogram
	.quad	Lfunc_begin0            ## DW_AT_low_pc
Lset3 = Lfunc_end0-Lfunc_begin0         ## DW_AT_high_pc
	.long	Lset3
	.byte	1                       ## DW_AT_frame_base
	.byte	86
	.long	138                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	29                      ## DW_AT_decl_line
                                        ## DW_AT_prototyped
                                        ## DW_AT_external
	.byte	7                       ## Abbrev [7] 0x60:0xe DW_TAG_formal_parameter
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	124
	.long	162                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	29                      ## DW_AT_decl_line
	.long	338                     ## DW_AT_type
	.byte	7                       ## Abbrev [7] 0x6e:0xe DW_TAG_formal_parameter
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	112
	.long	181                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	29                      ## DW_AT_decl_line
	.long	345                     ## DW_AT_type
	.byte	7                       ## Abbrev [7] 0x7c:0xe DW_TAG_formal_parameter
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	108
	.long	197                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	30                      ## DW_AT_decl_line
	.long	338                     ## DW_AT_type
	.byte	7                       ## Abbrev [7] 0x8a:0xe DW_TAG_formal_parameter
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	96
	.long	203                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	30                      ## DW_AT_decl_line
	.long	345                     ## DW_AT_type
	.byte	7                       ## Abbrev [7] 0x98:0xe DW_TAG_formal_parameter
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	88
	.long	210                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	31                      ## DW_AT_decl_line
	.long	366                     ## DW_AT_type
	.byte	8                       ## Abbrev [8] 0xa6:0x2a DW_TAG_lexical_block
	.quad	Ltmp2                   ## DW_AT_low_pc
Lset4 = Ltmp3-Ltmp2                     ## DW_AT_high_pc
	.long	Lset4
	.byte	9                       ## Abbrev [9] 0xb3:0xe DW_TAG_variable
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	84
	.long	214                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	36                      ## DW_AT_decl_line
	.long	338                     ## DW_AT_type
	.byte	9                       ## Abbrev [9] 0xc1:0xe DW_TAG_variable
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	72
	.long	222                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	37                      ## DW_AT_decl_line
	.long	345                     ## DW_AT_type
	.byte	0                       ## End Of Children Mark
	.byte	8                       ## Abbrev [8] 0xd0:0x1c DW_TAG_lexical_block
	.quad	Ltmp8                   ## DW_AT_low_pc
Lset5 = Ltmp13-Ltmp8                    ## DW_AT_high_pc
	.long	Lset5
	.byte	9                       ## Abbrev [9] 0xdd:0xe DW_TAG_variable
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	68
	.long	231                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	49                      ## DW_AT_decl_line
	.long	355                     ## DW_AT_type
	.byte	0                       ## End Of Children Mark
	.byte	8                       ## Abbrev [8] 0xec:0x48 DW_TAG_lexical_block
	.quad	Ltmp19                  ## DW_AT_low_pc
Lset6 = Ltmp25-Ltmp19                   ## DW_AT_high_pc
	.long	Lset6
	.byte	9                       ## Abbrev [9] 0xf9:0xe DW_TAG_variable
	.byte	2                       ## DW_AT_location
	.byte	145
	.byte	64
	.long	231                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	90                      ## DW_AT_decl_line
	.long	355                     ## DW_AT_type
	.byte	8                       ## Abbrev [8] 0x107:0x2c DW_TAG_lexical_block
	.quad	Ltmp22                  ## DW_AT_low_pc
Lset7 = Ltmp23-Ltmp22                   ## DW_AT_high_pc
	.long	Lset7
	.byte	9                       ## Abbrev [9] 0x114:0xf DW_TAG_variable
	.byte	3                       ## DW_AT_location
	.byte	145
	.ascii	"\274\177"
	.long	239                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	98                      ## DW_AT_decl_line
	.long	355                     ## DW_AT_type
	.byte	9                       ## Abbrev [9] 0x123:0xf DW_TAG_variable
	.byte	3                       ## DW_AT_location
	.byte	145
	.ascii	"\260\177"
	.long	247                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	104                     ## DW_AT_decl_line
	.long	47                      ## DW_AT_type
	.byte	0                       ## End Of Children Mark
	.byte	0                       ## End Of Children Mark
	.byte	8                       ## Abbrev [8] 0x134:0x1d DW_TAG_lexical_block
	.quad	Ltmp25                  ## DW_AT_low_pc
Lset8 = Ltmp26-Ltmp25                   ## DW_AT_high_pc
	.long	Lset8
	.byte	9                       ## Abbrev [9] 0x141:0xf DW_TAG_variable
	.byte	3                       ## DW_AT_location
	.byte	145
	.ascii	"\254\177"
	.long	239                     ## DW_AT_name
	.byte	2                       ## DW_AT_decl_file
	.byte	117                     ## DW_AT_decl_line
	.long	355                     ## DW_AT_type
	.byte	0                       ## End Of Children Mark
	.byte	0                       ## End Of Children Mark
	.byte	4                       ## Abbrev [4] 0x152:0x7 DW_TAG_base_type
	.long	168                     ## DW_AT_name
	.byte	7                       ## DW_AT_encoding
	.byte	4                       ## DW_AT_byte_size
	.byte	2                       ## Abbrev [2] 0x159:0x5 DW_TAG_pointer_type
	.long	350                     ## DW_AT_type
	.byte	5                       ## Abbrev [5] 0x15e:0x5 DW_TAG_const_type
	.long	355                     ## DW_AT_type
	.byte	3                       ## Abbrev [3] 0x163:0xb DW_TAG_typedef
	.long	338                     ## DW_AT_type
	.long	188                     ## DW_AT_name
	.byte	3                       ## DW_AT_decl_file
	.byte	31                      ## DW_AT_decl_line
	.byte	2                       ## Abbrev [2] 0x16e:0x5 DW_TAG_pointer_type
	.long	355                     ## DW_AT_type
	.byte	0                       ## End Of Children Mark
	.section	__DWARF,__debug_ranges,regular,debug
Ldebug_range:
	.section	__DWARF,__debug_macinfo,regular,debug
Ldebug_macinfo:
Lcu_macro_begin0:
	.byte	0                       ## End Of Macro List Mark
	.section	__DWARF,__apple_names,regular,debug
Lnames_begin:
	.long	1212240712              ## Header Magic
	.short	1                       ## Header Version
	.short	0                       ## Header Hash Function
	.long	1                       ## Header Bucket Count
	.long	1                       ## Header Hash Count
	.long	12                      ## Header Data Length
	.long	0                       ## HeaderData Die Offset Base
	.long	1                       ## HeaderData Atom Count
	.short	1                       ## DW_ATOM_die_offset
	.short	6                       ## DW_FORM_data4
	.long	0                       ## Bucket 0
	.long	399563513               ## Hash in Bucket 0
Lset9 = LNames0-Lnames_begin            ## Offset in Bucket 0
	.long	Lset9
LNames0:
	.long	138                     ## dsk_tls_bignum_multiply
	.long	1                       ## Num DIEs
	.long	75
	.long	0
	.section	__DWARF,__apple_objc,regular,debug
Lobjc_begin:
	.long	1212240712              ## Header Magic
	.short	1                       ## Header Version
	.short	0                       ## Header Hash Function
	.long	1                       ## Header Bucket Count
	.long	0                       ## Header Hash Count
	.long	12                      ## Header Data Length
	.long	0                       ## HeaderData Die Offset Base
	.long	1                       ## HeaderData Atom Count
	.short	1                       ## DW_ATOM_die_offset
	.short	6                       ## DW_FORM_data4
	.long	-1                      ## Bucket 0
	.section	__DWARF,__apple_namespac,regular,debug
Lnamespac_begin:
	.long	1212240712              ## Header Magic
	.short	1                       ## Header Version
	.short	0                       ## Header Hash Function
	.long	1                       ## Header Bucket Count
	.long	0                       ## Header Hash Count
	.long	12                      ## Header Data Length
	.long	0                       ## HeaderData Die Offset Base
	.long	1                       ## HeaderData Atom Count
	.short	1                       ## DW_ATOM_die_offset
	.short	6                       ## DW_FORM_data4
	.long	-1                      ## Bucket 0
	.section	__DWARF,__apple_types,regular,debug
Ltypes_begin:
	.long	1212240712              ## Header Magic
	.short	1                       ## Header Version
	.short	0                       ## Header Hash Function
	.long	4                       ## Header Bucket Count
	.long	4                       ## Header Hash Count
	.long	20                      ## Header Data Length
	.long	0                       ## HeaderData Die Offset Base
	.long	3                       ## HeaderData Atom Count
	.short	1                       ## DW_ATOM_die_offset
	.short	6                       ## DW_FORM_data4
	.short	3                       ## DW_ATOM_die_tag
	.short	5                       ## DW_FORM_data2
	.short	4                       ## DW_ATOM_type_flags
	.short	11                      ## DW_FORM_data1
	.long	-1                      ## Bucket 0
	.long	0                       ## Bucket 1
	.long	3                       ## Bucket 2
	.long	-1                      ## Bucket 3
	.long	290711645               ## Hash in Bucket 1
	.long	-1304652851             ## Hash in Bucket 1
	.long	-69895251               ## Hash in Bucket 1
	.long	290821634               ## Hash in Bucket 2
Lset10 = Ltypes2-Ltypes_begin           ## Offset in Bucket 1
	.long	Lset10
Lset11 = Ltypes1-Ltypes_begin           ## Offset in Bucket 1
	.long	Lset11
Lset12 = Ltypes0-Ltypes_begin           ## Offset in Bucket 1
	.long	Lset12
Lset13 = Ltypes3-Ltypes_begin           ## Offset in Bucket 2
	.long	Lset13
Ltypes2:
	.long	188                     ## uint32_t
	.long	1                       ## Num DIEs
	.long	355
	.short	22
	.byte	0
	.long	0
Ltypes1:
	.long	168                     ## unsigned int
	.long	1                       ## Num DIEs
	.long	338
	.short	36
	.byte	0
	.long	0
Ltypes0:
	.long	115                     ## long long unsigned int
	.long	1                       ## Num DIEs
	.long	58
	.short	36
	.byte	0
	.long	0
Ltypes3:
	.long	106                     ## uint64_t
	.long	1                       ## Num DIEs
	.long	47
	.short	22
	.byte	0
	.long	0

.subsections_via_symbols
	.section	__DWARF,__debug_line,regular,debug
Lsection_line:
Lline_table_start0:
