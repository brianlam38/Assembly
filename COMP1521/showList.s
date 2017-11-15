   .data
node1:
   .word 1       # value
   .word node2   # next
node2:
   .word 2
   .word node3
node3:
   .word 4
   .word 0

   .text
   .globl main
main:
   # prologue
   add  $sp, $sp, -4
   sw   $fp, ($sp)        # push $fp
   la   $fp, ($sp)        # reset $fp
   add  $sp, $sp, -4
   sw   $ra, ($sp)        # push $ra
   # function body
   la   $a0, node1
   jal  showList
   # epilogue
   lw   $ra, -4($fp)      # $ra = pop
   lw   $fp, ($fp)        # $fp = pop
   add  $sp, $sp, 8
   j    $ra

# void showList(Node *L)
# {
#    Node *cur;
#    for (cur = L; cur != NULL; cur = cur->next) {
#       printf("%d", cur->value);
#       if (cur->next != NULL) printf("%c",',');
#    }
#    printf("%c",'\n');
# }
showList:
   # prologue
   add  $sp, $sp, -4
   sw   $fp, ($sp)       # push $fp
   la   $fp, ($sp)       # reset $fp
   add  $sp, $sp, -4
   sw   $ra, ($sp)       # push $ra
   add  $sp, $sp, -4
   sw   $s0, ($sp)       # push $s0
   # function body
#  Node *cur;
#  for (cur = L; cur != NULL; cur = cur->next) {
#     printf("%d", cur->value);
#     if (cur->next != NULL) printf("%c",',');
#  }
#  printf("%c",'\n');
   move $s0, $a0         # Node *cur = $a0 (i.e. L)
for_loop:
   beq  $s0, $0, end_for # if (curr == NULL) break
   lw   $a0, 0($s0)      # cur->value
   li   $v0, 1           # printf "%d"
   syscall               
   lw   $s0, 4($s0)      # cur = cur->next
   beq  $s0, $0, end_for # if (curr == NULL) break
   li   $a0, ','
   li   $v0, 11          # printf ","
   syscall
   j    for_loop
end_for:
   li   $a0, '\n'
   li   $v0, 11          # printf ","
   syscall
   # epilogue
   lw   $s0, -8($fp)     # $s0 = pop
   lw   $ra, -4($fp)     # $ra = pop
   lw   $fp, ($fp)       # $fp = pop
   add  $sp, $sp, 12
   jr   $ra
