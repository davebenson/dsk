	.section	__TEXT,__text,regular,pure_instructions


# count, a, b, carry, out
	.globl	_dsk_tls_bignum_multiply_2x2
_dsk_tls_bignum_multiply_2x2:
        # a=%rdi
        # b=%rsi
        # out=%rdx
        movq (%rdi), %rax
        movq %rdx, %r8    # about to clobber rdx
        mulq (%rsi)
        movq %rax, (%r8)
        movq %rdx, 8(%r8)
        ret

