
# VARIABLES
	.data
Node1:		  # n0 = head
	.word 1
	.word Node2
Node2:
	.word 2
	.word Node3
Node3:
	.word 3
	.word 0

# CODE
	.text
	.globl main

main:
	la $a0, Node1
	jal showList

	# return
	li $v0, 10
	syscall

showList:
	move $s0, $a0		# s0 = a0

for:	
	beq  $s0, $0, end	# if (curr == NULL) break;
	lw   $a0, 0($s0)	# curr->value;
	li   $v0, 1		# printf;
	syscall

	lw   $s0, 4($s0)	# curr = curr-next;

	beq  $s0, $0, end
	li   $a0, ','		# ','
	li   $v0, 11		# printf(a0)
	syscall
	
	j    for

end:
	jr   $ra
