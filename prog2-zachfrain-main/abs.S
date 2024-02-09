.global abs
.abs:
    # x in %edi
    movl %edi, %eax
    testl %eax, %eax
    jge .end
    negl %eax
.end:
    ret

# absolute value of a number in C
# int abs(int a){
#    if(a < 0)
#        a = -a;
#    return a;
# }