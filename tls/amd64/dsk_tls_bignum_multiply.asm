	.section	__TEXT,__text,regular,pure_instructions


# count, a, b, carry, out
	.globl	_dsk_tls_bignum_multiply
_dsk_tls_bignum_multiply:
        # On Input:
        #    a_len   %rdi
        #    a       %rsi
        #    b_len   %rdx
        #    b       %rcx
        #    out     %r8

        # We will use rbx, and we have to preserve it.
        pushq %rbx

        # We immediate copy b into %r10
        # so that we can use rcx for looping.
        movq %rcx, %r10

        # So a_len, b_len and swap a, b correspondingly so that a_len >= b_len.
        cmp %rdi, %rdx
        jae alen_bigger

        xchgq %rdi, %rdx
        xchgq %rsi, %r10

alen_bigger:
        # Divide a_len and b_len by 2, saving the parity in bl, bh respectively.
        shrq $1, %rdi
        setc %bl
        shrq $1, %rdx
        setc %bh

        # So, now:
        #    a_len/2   %rdi
        #    a_len%2   %bl
        #    a         %rsi
        #    b_len/2   %rdx
        #    a_len%2   %bh
        #    b         %r10
        #    out       %r8

        # We have a special version of this function for b_len==0 or 1.
        testq        %rdx, %rdx
        jz           b_small

        #
        # We unroll the first loop, since
        # we need to initialize the entire array underneath it.
        #
        # Subsequent loops will only need to initialize the highest word.
        #
        movq (%r10), %r9       # %r9 := ((uint64*)b)[0]
        pushq %rsi
        pushq %r8
        movq $0, (%r8)
        movq %rdi, %rcx
b0_loop:
        movq (%rsi), %rax
        addq $8, %rsi

        mulq %r9
        addq %rax, (%r8)
        adcq $0, %rdx
        movq %rdx, 8(%r8)
        addq $8, %r8

        loop b0_loop

        popq %r8
        popq %rsi

        #
        # Handle all subsequent qwords from 'b'.
        #

        # Preserve b_len,b, which will be modified as 
        # the loop proceeds.
        pushq %r10
        pushq %rdx

        # We've already handled the first 'b' qword;
        # is that the only one?
        subq $1, %rdx
        jz   done_64bit_pairs

        addq $8, %r10
        
outer_loop:
        movq (%r10), %r9       # store current word from 'b' in %r9.
        addq $8, %r10
        pushq %rsi             # preserve 'a'
        pushq %r8              # preserve 'out'
        movq $0, %rbx          # carry := 0
        movq %rdi, %rcx

inner_loop:
        # rax := *a++;   (with 'a' as uint64_t*)
        movq (%rsi), %rax
        addq $8, %rsi

        mulq %r9            # rdx:rax = rax * r9
        addq %rbx, %rax     # rdx:rax := carry (rbx)
        adcq $0, %rdx       #    ...
        addq %rax, (%r8)    # *out += rax    (out=r8)
        adcq $0, %rdx       # ... propagate carry from add into high qword
        addq $8, %r8        # out += 8
        movq %rdx, %rbx     # preserve high qword as next carry
        loop inner_loop

        movq %rbx, (%r8)    # initialize word with final carry

        movzx %bl, %rcx
        addb %bh, %cl
        testb %cl, %cl
        jz initialize_fixup_done

        # Zero last words of out.
        addq $8, %r8
zero_loop:
        movl $0, (%r8)
        addq 4, %r8
        loop zero_loop

        popq %r8
        popq %rsi

        # Handle final 32-bit from a
        test %bl, %bl
        jz handle_final_32bit_from_b

        pushq %r8
        pushq %rsi
        movl (%rsi, %rdi, 8), %ebx  # ebx := a[(a_len/2)*2]
        movq %rdx, %rcx
        movq %r10, %r9
        leaq (%r8, %rsi, 8), %r8
        call mul_word32_by_64bitarray_addto
        popq %rsi
        popq %r8

handle_final_32bit_from_b:
        test %bl, %bl
        jz odd_odd_word

        pushq %r8
        pushq %rsi
        movl (%r10, %rdx, 8), %ebx  # ebx := b[(b_len/2)*2]
        movq %rdi, %rcx
        movq %rsi, %r9
        leaq (%r8, %rdi, 8), %r8
        call mul_word32_by_64bitarray_addto
        popq %rsi
        popq %r8

odd_odd_word:
        andb %bl, %bh
        testb %bl, %bh
        jpe cleanup

        # Both a_len and b_len were odd, so there's a final 32x32 mulitply
        # needed.
        pushq %rdx
        movl (%rsi, %rdi, 8), %eax
        movl (%r10, %rdx, 8), %edx
        mulq %rdx
        popq %rdx
        addq %rdx, %rdi
        addq %rax, (%r8, %rdi, 8)

cleanup:
        popq %rbx
        ret

# rbx:   word32
# rcx:   word64 count           [modified]
# r9:    input array            [modified]
# r8:    output array to add to [modified]
# Also modifies rsi which is used as carry.
mul_word32_by_64bitarray_addto:
        movq $0, %rsi
mw32_loop:
        movq (%r9), %rax
        mulq %rbx
        addq %rsi, %rax
        adcq $0, %rdx
        addq %rax, (%r8)
        adcq $0, %rdx
        movq %rdx, %rsi
        loop mw32_loop
        ret
