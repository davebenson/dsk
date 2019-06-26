
	.section	__TEXT,__text,regular,pure_instructions

	.globl	_dsk_aes192_decrypt_inplace
        # state=%rdi
        # in_out=%rsi
_dsk_aes192_decrypt_inplace:
        movdqu (%rsi), %xmm0
        pxor (%rdi), %xmm0  //    ...
        aesdec 16(%rdi), %xmm0
        aesdec 32(%rdi), %xmm0
        aesdec 48(%rdi), %xmm0
        aesdec 64(%rdi), %xmm0
        aesdec 80(%rdi), %xmm0
        aesdec 96(%rdi), %xmm0
        aesdec 112(%rdi), %xmm0
        aesdec 128(%rdi), %xmm0
        aesdec 144(%rdi), %xmm0
        aesdec 160(%rdi), %xmm0
        aesdec 176(%rdi), %xmm0
        aesdeclast 192(%rdi), %xmm0
        movdqu %xmm0, (%rsi)
        ret

