
# VARIABLES / DATA

	.data
main_ret_save:
	.space 4
sum_ret_save:
	.space 4
lo:
	.word 0
array:			# array[10] un-initialised
	.space 40
total:
	.word 0
message:
	.asciiz "Current Total:"

# CODE AREA
	.text
	.globl main
main:
	sw $ra main_ret_save

	# LOAD ARRAY VALUES
	la $t0 array
	li $t1 5
	sw $t1 0($t0)	# a[0] = 5
	li $t1 4
	sw $t1 4($t0)	# a[1] = 4
	li $t1 7
	sw $t1 8($t0)	# a[2] = 7
	li $t1 6
	sw $t1 12($t0)	# a[3] = 6
	li $t1 8
	sw $t1 16($t0)	# a[4] = 8
	li $t1 9
	sw $t1 20($t0)	# a[5] = 9
	li $t1 1
	sw $t1 24($t0)	# a[6] = 1
	li $t1 2
	sw $t1 28($t0)	# a[7] = 2
	li $t1 3
	sw $t1 32($t0)	# a[8] = 3
	li $t1 0
	sw $t1 36($t0)	# a[9] = 0

	# PERFORM SUMOF FUNCTION
	j sum_of
	
sum_of:
	sw $ra sum_ret_save
	
	# SET UP ARGS + COUNTERS
	la $t0 array
	lw $a1 lo	# lo
	li $a2 36	# hi
	
	# IF LO > HI -> goto return
	bgt $a1 $a2, return

	# else a[lo] + sumOf(a,lo+1,hi)
	lw   $t2 array($a1)  # t2 = a[lo]

	# LO++
	addi $t1 $a1 4
	sw   $t1 lo

	# TOTAL += a[1]
	lw   $t3 total
	add  $t3 $t3 $t2
	sw   $t3 total

	# CURRENT TOTAL = TOTAL
	la  $a0 message
	li  $v0 4
	syscall
	move $a0 $t3
	li   $v0 1
	syscall
	la  $a0 '\n'
	li  $v0 11
	syscall
		
	j sum_of

return:
	# PRINT CURRENT TOTAL
	lw $a0 total
	li $v0 1
	syscall

	# RETURN 0
	li $v0, 10
	syscall
