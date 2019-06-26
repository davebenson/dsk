
//
// Assert: len <= add_to_len; rv is carry
//
//    uint32_t dsk_tls_bignum_mul64word_addto (uint32_t scalar,
//                                             unsigned len,
//                                             const uint64_t *factor,
//                                             uint64_t *in_out);
//
#    scalar      %rdi
#    len         %rsi -> %rcx
#    factor      %rdx -> %r8
#    out         %rcx -> %rsi

	.section	__TEXT,__text,regular,pure_instructions


# count, a, b, carry, out
	.globl	_dsk_tls_bignum_mul64word_addto
_dsk_tls_bignum_mul64word_addto:
        xchgq    %rcx, %rsi
        movq     %rdx, %r8
        # Now, the registers are as on the right-side of the -> arrows above.

        movq     $0, %r9                # carry
        testq    %rcx, %rcx
        jnz      the_loop
        movq     %r9, %rax
        ret

the_loop:
        movq     (%r8), %rax
        mulq     %rdi
        addq     %r9, %rax
        adcq     $0, %rdx
        addq     %rax, (%rsi)
        adcq     $0, %rdx
        movq     %rdx, %r9
        leaq     8(%r8), %r8
        leaq     8(%rsi), %rsi
        loop     the_loop

        movq     %r9, %rax
        ret




