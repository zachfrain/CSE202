.global average

# a in %esi
# b in %edi
# *cptr in %rdx

average:
movl %edi, %eax		# sum = b
addl %esi, %eax		# sum = a+b
cmpl %esi, %eax		# compare sum and a to find if overload happens		
jl	.L2		# if sum is less than a, jump to .L2 (overload)
movb $0, (%rdx)		# since no overload, set cptr to 0
movl %eax, %ecx		# set ecx equal to sum (in order to do arithmetic on sum)
sarl $1, %ecx		# shift ecx right 1 (divide by 2)
movl %ecx, %eax		# set eax equal to ecx, so sum/2
ret
.L2:			# case if overload
movb $1, (%rdx)		# set cptr to 1
movl %eax, %ecx		# set ecx equal to sum
sarl $1, %ecx		# shift ecx right 1 (divide by 2)
movl %ecx, %eax		# set eax equal to ecx, so sum/2
ret


# average of two unsigned integers
# unsigned average(unsigned a, unsigned b, char* cptr){
#    int sum = a + b;
#    if(sum < a)
#       *cptr = 1;
#    else
#       *cptr = 0;
#    return sum/2;
# }
