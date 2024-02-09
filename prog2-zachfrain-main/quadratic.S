.global quadratic
quadratic:
    # duplicate all arguments (will be used later)
    movl %edi, %r9d     # r9d = a
    movl %esi, %r10d    # r10d = b
    movl %ecx, %r11d    # r11d = c
    movq %rcx, %r14	# r14 = x1
    movq %r8, %r15	# r15 = x2
    # d = (b*b) - 4ac
    movl %esi, %eax     # Set eax = b
    movl $4, %r12d      # Set r12d = 4
    imull %eax, %eax    # eax = b * b
    imull %edi, %r12d   # r12d = 4a
    imull %edx, %r12d   # r12d = 4ac
    subl %r12d, %eax    # eax = b*b - 4ac = d

    # check if d < 0 or d == 0
    cmpl $0, %eax
    jl .quadratic_less
    je .quadratic_zero
 
    movl %eax, %edi     # edi = d
    call r_sqrt         # Call to r_sqrt
    
    movl %eax, %edi     # edi = rd
    shll $1, %r9d       # r9d = 2a

    movl %r10d, %eax    # eax = b
    subl %edi, %eax     # eax = b - rd
    negl %eax           # eax = -b + rd
    cltd                # convert eax to double
    idivl %r9d          # eax = -b + rd / (2a)
    movl %eax, (%r14)   # put eax into x1

    movl %r10d, %eax    # eax = b
    addl %edi, %eax     # eax = b + rd
    negl %eax           # eax = -b -rd
    cltd                # convert eax to double
    idivl %r9d          # eax = -b - rd / (2a)
    movl %eax, (%r15)   # put eax into x2

    movl $1, %eax       # eax = 1
    ret            

.quadratic_less: 
    movl $1, %eax
    neg %eax
    ret

.quadratic_zero: 
    movl %esi, %eax     # eax = b 
    negl %eax           # eax = -b
    movl %edi, %r9d     # r9d = a 
    shll $1, %r9d       # r9d = 2a
    cltd                # convert from long to double
    idivl %r9d          # eax = -b / 2a
    movl %eax, (%rcx)   # put eax into x1
    movl $0, %eax       # eax = 0
    ret


#root square function
r_sqrt:
    cmpl $0, %edi       # Check x < 0
    jl r_sqrt_zero      
    pushq %rbx          # Push rbx
    movl %edi, %ebx     # Move edi to ebx
    cmpl $2, %edi       # Check x < 2
    jl r_sqrt_two

    sarl $2, %edi       # x = x >> 2
    call r_sqrt
    shll $1, %eax       # eax = (x >> 2) << 1
    movl %eax, %esi     # esi = eax 
    movl %eax, %edx     # edx = eax
    addl $1, %edx       # edx = edx + 1
    movl %edx, %ecx     # ecx = edx
    imull %ecx, %ecx    # ecx = (ecx * ecx)
    cmpl %ebx, %ecx     # Compare ebx and ecx
    jg small_cand       
    jle large_cand      

r_sqrt_zero: 
    movl $1, %eax
    negl %eax
    ret
r_sqrt_two:
    movl %edi, %eax
    popq %rbx
    ret
small_cand: 
    movl %esi, %eax 
    popq %rbx
    ret
large_cand: 
    movl %edx, %eax
    popq %rbx
    ret



#
# find the solutions of a quadratic equation in C
# int quadratic(int a, int b, int c, int *x1, int *x2){
#    int d = (b * b) - 4 * a * c; //discriminant
#    if(d < 0)
#        return -1;
#    if(d == 0){
#        *x1 = -b / (2 * a);
#        return 0;
#    }
#    int rd = r_sqrt(d);
#    *x1 = (-b - rd) / (2 * a);
#    *x2 = (-b + rd) / (2 * a);
#    return 1;
# }

# calculate recursively the square root of an int (in C) 
# int r_sqrt(int x){
#    if(x < 0)
#        return -1;
#    if (x < 2)
#        return x;
#   /* recursive call */
#    int small_cand = r_sqrt(x >> 2) << 1;
#    int large_cand = small_cand + 1;
#    if (large_cand * large_cand > x)
#        return small_cand;
#    else
#        return large_cand;
# }
