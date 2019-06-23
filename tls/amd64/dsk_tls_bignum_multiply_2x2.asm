	.section	__TEXT,__text,regular,pure_instructions


# count, a, b, carry, out
	.globl	_dsk_tls_bignum_multiply_2x2
_dsk_tls_bignum_multiply_2x2:
        # a=%rdi
        # b=%rsi
        # out=%rdx
        movq (%rdi), %rax
        movq %rdx, %r10
        mulq (%rsi),
        movq %rdx, 
        movq %rax, (%r10)
        movq %rdx, 8(%r10)
        ret

