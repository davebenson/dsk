
	.section	__TEXT,__text,regular,pure_instructions

	.globl	_dsk_aes256_decrypt_inplace
        # state=%rdi
        # in_out=%rsi
_dsk_aes256_decrypt_inplace:
        movdqu (%rsi), %xmm1
        pxor (%rdi), %xmm1  //    ...
        aesdec 16(%rdi), %xmm1
        aesdec 32(%rdi), %xmm1
        aesdec 48(%rdi), %xmm1
        aesdec 64(%rdi), %xmm1
        aesdec 80(%rdi), %xmm1
        aesdec 96(%rdi), %xmm1
        aesdec 112(%rdi), %xmm1
        aesdec 128(%rdi), %xmm1
        aesdec 144(%rdi), %xmm1
        aesdec 160(%rdi), %xmm1
        aesdec 176(%rdi), %xmm1
        aesdec 192(%rdi), %xmm1
        aesdec 208(%rdi), %xmm1
        aesdeclast 224(%rdi), %xmm1
        movdqu %xmm1, (%rsi)
        ret

