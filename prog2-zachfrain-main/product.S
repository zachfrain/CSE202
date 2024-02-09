.global product
# a in %rdi, b in %rsi, c in %rdx, size in %ecx
product:
	mov $0 ,%rax				# set %rax to zero (use this as a loop counter)
loop:
	cmp %rax, %rcx				# compare rax to size
	je done					# if equal, we are done

	movss (%rdi, %rax, 4), %xmm0		# move the current value in a, to %xmm0. %rax and 4 are there in order to move the current element to the register. (rax*4) = position
	mulss (%rsi, %rax, 4), %xmm0		# multiply %xmm0 by the current value in b.
	movss %xmm0, (%rdx,%rax,4)		# move %xmm0 to the correct value in c.

	add $1, %rax				# increment %rax by 1
	jmp loop				# jump back into loop

done:
	ret

# Product of two vectors in C
# void product(float *a, float *b, float *c, unsigned size){
#    for(unsigned i=0; i<size; i++){
#        c[i] = a[i] * b[i];
#    }
# }
