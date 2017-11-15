# Struct implementation example

# ------------------------ DATA
	.data
person:
	.word 5035087   # int id_no;	-> Offset = 0
	.asciiz "LAM"	# char family[15]; -> Offset = 4
	.space 11
	.asciiz "BRIAN" # char given[15];  -> Offset = 19
	.space 9

	.align 2
main_ret_save:
	.space 4
show_ret_save:
	.space 4


# ------------------------ CODE
	.text
	.globl main
main:
	sw $ra, main_ret_save	

	la $s0, person
	
	jal show_person

	li $v0, 10	# return 0
	syscall

show_person:
	sw $ra, show_ret_save

	lw $a0, ($s0)
	li $v0, 1
	syscall

	la $a0, ' '
	li $v0, 11
	syscall

	la $a0, 4($s0)
	li $v0, 4
	syscall

	la $a0, ' '
	li $v0, 11
	syscall

	la $a0, 19($s0)
	li $v0, 4
	syscall	

	jr $ra
