
	.section	__TEXT,__text,regular,pure_instructions

	.globl	_dsk_aes192_encrypt_inplace
        # state=%rdi
        # in_out=%rsi
_dsk_aes192_encrypt_inplace:
        movdqu (%rsi), %xmm0
        pxor (%rdi), %xmm0  //    ...
        AESENC 16(%rdi), %xmm0
        AESENC 32(%rdi), %xmm0
        AESENC 48(%rdi), %xmm0
        AESENC 64(%rdi), %xmm0
        AESENC 80(%rdi), %xmm0
        AESENC 96(%rdi), %xmm0
        AESENC 112(%rdi), %xmm0
        AESENC 128(%rdi), %xmm0
        AESENC 144(%rdi), %xmm0
        AESENC 160(%rdi), %xmm0
        AESENC 176(%rdi), %xmm0
        AESENCLAST 192(%rdi), %xmm0
        movdqu %xmm0, (%rsi)
        ret

