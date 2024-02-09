.global sum 
# pointer to list in %rdi
sum:
	mov $0 ,%rax		# Set %rax to zero, this will be the total
loop:
	cmp $0 ,%rdi		# Compare 0 and the current node.
	je done			# If null, we are done

	addq 8(%rdi), %rax	# Add the current value (the 8th byte of rdi) to total
	movq 16(%rdi),%rdi	# Set rdi equal to the *next (16th byte of rdi)

	jmp loop		# Loop again

done:
    ret				# Return %rax
    
# struct ll{
#	char* name;
#	long value;
#	struct ll *next;
#};

# sum of the elements of a linkedlist in C
# long sum(struct ll *list){
#    long total = 0;
#    struct ll *ptr = list;
#    while(ptr){
#        total += ptr->value;
#        ptr = ptr->next;
#    }
#    return total;
# }
