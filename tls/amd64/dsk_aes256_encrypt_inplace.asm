
	.section	__TEXT,__text,regular,pure_instructions

	.globl	_dsk_aes256_encrypt_inplace
        # state=%rdi
        # in_out=%rsi
_dsk_aes256_encrypt_inplace:
        movdqu (%rsi), %xmm1
        pxor (%rdi), %xmm1  //    ...
        AESENC 16(%rdi), %xmm1
        AESENC 32(%rdi), %xmm1
        AESENC 48(%rdi), %xmm1
        AESENC 64(%rdi), %xmm1
        AESENC 80(%rdi), %xmm1
        AESENC 96(%rdi), %xmm1
        AESENC 112(%rdi), %xmm1
        AESENC 128(%rdi), %xmm1
        AESENC 144(%rdi), %xmm1
        AESENC 160(%rdi), %xmm1
        AESENC 176(%rdi), %xmm1
        AESENC 192(%rdi), %xmm1
        AESENC 208(%rdi), %xmm1
        AESENCLAST 224(%rdi), %xmm1
        movdqu %xmm1, (%rsi)
        ret

