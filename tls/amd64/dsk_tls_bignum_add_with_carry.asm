	.section	__TEXT,__text,regular,pure_instructions


# count, a, b, carry, out
	.globl	_dsk_tls_bignum_add_with_carry
_dsk_tls_bignum_add_with_carry:
        # count=%rdi
        # a=%rsi
        # b=%rdx
        # carry=%rcx
        # out=%r8
        movq $0, %rax
        cmpq %rax, %rdi
        je done

loop_top:
        shrb $1, %cl
        movl (%rsi,%rax,4), %ecx
        adcl (%rdx,%rax,4), %ecx
        movl %ecx, (%r8,%rax,4)
        setc %cl
        incq %rax
        cmpq %rax, %rdi
        jne loop_top
done:
        movzx %cl, %eax                # zeroes top bits too
        ret

