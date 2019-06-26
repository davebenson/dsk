	.section	__TEXT,__text,regular,pure_instructions


# Arguments:                        
#       a_len             unsigned                 [nonzero]
#       a_data            const uint64_t *         [of length a_len]
#       b_len             unsigned                 [nonzero]
#       b_data            const uint64_t *         [of length b_len]
#       out               uint64_t *               [of length a_len+b_len]

#
# On Input: (these are simply from the X86-64 calling conventions order)
#    a_len   %rdi
#    a_data  %rsi
#    b_len   %rdx -> %r12
#    b_data  %rcx -> %r9
#    out     %r8
#
        .globl	_dsk_tls_bignum_multiply64
_dsk_tls_bignum_multiply64:

        pushq   %r12
        movq    %rcx, %r9
        movq    %rdx, %r12

        # First outer loop: initialize the underlying array
        movq    (%r9), %r10          # %r10 := *b_data
        movq    %rdi, %rcx           # %rcx := a_len
        movq    $0, %r11             # carry

inner_loop_b0:
        movq    (%rsi), %rax
        leaq    8(%rsi), %rsi
        mulq    %r10
        addq    %r11, %rax
        adcq    $0, %rdx
        movq    %rax, (%r8)
        leaq    8(%r8), %r8
        movq    %rdx, %r11
        loop    inner_loop_b0

        movq    %r11, (%r8)
        subq    $1, %r12
        jz      done

outer_loop:
        #
        # Arguably, this work should done by the end of loop for b=0
        # and general loop, but we move that to the top so that we can
        # share the work between b=0 and general case.
        #

        # restore r8, rsi
        negq    %rdi
        leaq    (%r8, %rdi, 8), %r8    # out -= a_len
        leaq    (%rsi, %rdi, 8), %rsi  # a_data -= a_len
        negq    %rdi

        # Advance output and 'b_data' by a single qword
        leaq    8(%r8), %r8             # out += 1
        leaq    8(%r9), %r9             # b_data += 1
        movq    $0, %r11                # carry = 0
        movq    %rdi, %rcx              # %rcx := a_len
        movq    (%r9), %r10             # %r10 := *b_data

inner_loop:
        movq    (%rsi), %rax
        leaq    8(%rsi), %rsi
        mulq    %r10
        addq    %r11, %rax
        adcq    $0, %rdx
        addq    %rax, (%r8)
        leaq    8(%r8), %r8
        adcq    $0, %rdx
        movq    %rdx, %r11
        loop    inner_loop

        # emit final carry
        movq    %r11, (%r8)             # this word was previously unassigned,
                                        #     so must use mov, not add

        subq    $1, %r12
        jz      done
        jmp     outer_loop

done:
        popq   %r12
        ret
