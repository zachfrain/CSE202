.global min
# list in %rdi, n in %esi
min:
  mov $0 ,%rcx		# loop counter to zero
  mov (%rdi), %ax	# load the first value of list in %ax

loop_start:
  add $2, %rdi		# move to next element (short is 2 bytes to add 2)
  subl $1, %esi		# subtract 1 from n
  jz done		# if n is zero, exit

  mov (%rdi), %dx	# move current element into %dx
  cmp %dx, %ax		# compare current to min
  jge update		# if min is greater than or equal to, go to update

  jmp loop_start	# start loop again

update:
 mov %dx,%ax		# replace min with the newly discovered minimum
 jmp loop_start		# jump back to loop

done:
  ret			# return %ax, which holds the minimum value

# For some reason, the test isn't giving me credit for this function despite
# it returning the correct value.


# short min(short *list, unsigned n){
#     unsigned i = 0;
#     short min = list[i];
#     while (i < n){
#         if(list[i] < min)
#             min = list[i];
#         i++;
#     }
#     return min;
# }
