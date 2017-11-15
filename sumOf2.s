# MIPS program - Sum two numbers


	.data
main_ret_save:
	.space 4
sum_ret_save:
	.space 4

	.text
	.globl main
main:
	sw $ra main_ret_save
	li $a0, 10  # arg1 = 10
	li $a1, 50  # arg2 = 50
	jal sumTwo

	addi $a0, $v1, 0  # a0 = v1
	li  $v0, 1        # print integer
	syscall

	li $v0, 10  # end of program, exit
	syscall

sumTwo:
	sw $ra, sum_ret_save
	add $v1, $a0, $a1  # v1 = a0 + a1
	jr $ra
