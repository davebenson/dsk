
	.section	__TEXT,__text,regular,pure_instructions


	.globl	_dsk_aes128_decrypt_inplace
        # state=%rdi
        # in_out=%rsi
_dsk_aes128_decrypt_inplace:
        movdqu 0(%rsi), %xmm0
        pxor 0(%rdi), %xmm0                 // round key 0
        aesdec 16(%rdi), %xmm0             // round key 1
        aesdec 32(%rdi), %xmm0             // round key 2
        aesdec 48(%rdi), %xmm0             // round key 3
        aesdec 64(%rdi), %xmm0             // round key 4
        aesdec 80(%rdi), %xmm0             // round key 5
        aesdec 96(%rdi), %xmm0             // round key 6
        aesdec 112(%rdi), %xmm0            // round key 7
        aesdec 128(%rdi), %xmm0            // round key 8
        aesdec 144(%rdi), %xmm0            // round key 9
        aesdeclast 160(%rdi), %xmm0        // round key 10
        movdqu %xmm0, 0(%rsi)
        ret

