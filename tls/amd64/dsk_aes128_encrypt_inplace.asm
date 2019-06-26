
	.section	__TEXT,__text,regular,pure_instructions


	.globl	_dsk_aes128_encrypt_inplace
        # state=%rdi
        # in_out=%rsi
_dsk_aes128_encrypt_inplace:
        movdqu 0(%rsi), %xmm0
        pxor 0(%rdi), %xmm0                 // round key 0
        aesenc 16(%rdi), %xmm0             // round key 1
        aesenc 32(%rdi), %xmm0             // round key 2
        aesenc 48(%rdi), %xmm0             // round key 3
        aesenc 64(%rdi), %xmm0             // round key 4
        aesenc 80(%rdi), %xmm0             // round key 5
        aesenc 96(%rdi), %xmm0             // round key 6
        aesenc 112(%rdi), %xmm0            // round key 7
        aesenc 128(%rdi), %xmm0            // round key 8
        aesenc 144(%rdi), %xmm0            // round key 9
        aesenclast 160(%rdi), %xmm0        // round key 10
        movdqu %xmm0, 0(%rsi)
        ret

