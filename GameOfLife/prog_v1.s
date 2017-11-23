# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by BRIAN LAM, August 2017

	.data
	.align 2
# board data N = 10 x 10 grid
N:
	.word 10  # gives board dimensions
#board:
#	.byte 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
#	.byte 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
#	.byte 0, 0, 0, 1, 0, 0, 0, 0, 0, 0
#	.byte 0, 0, 1, 0, 1, 0, 0, 0, 0, 0
#	.byte 0, 0, 0, 0, 1, 0, 0, 0, 0, 0
#	.byte 0, 0, 0, 0, 1, 1, 1, 0, 0, 0
#	.byte 0, 0, 0, 1, 0, 0, 1, 0, 0, 0
#	.byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
#	.byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
board:
	.byte '1', '0', '0', '0', '0', '0', '0', '0', '0', '0'
	.byte '1', '1', '0', '0', '0', '0', '0', '0', '0', '0'
	.byte '0', '0', '0', '1', '0', '0', '0', '0', '0', '0'
	.byte '0', '0', '1', '0', '1', '0', '0', '0', '0', '0'
	.byte '0', '0', '1', '0', '1', '0', '0', '0', '0', '0'
	.byte '0', '0', '1', '0', '1', '0', '0', '0', '0', '0'
	.byte '0', '0', '1', '0', '1', '0', '0', '0', '0', '0'
	.byte '0', '0', '1', '0', '1', '0', '0', '0', '0', '0'
	.byte '0', '0', '1', '0', '1', '0', '0', '0', '0', '0'
	.byte '0', '0', '1', '0', '1', '0', '0', '0', '0', '0'
newBoard:
	.space 100

# program data
max_iter:
	.word 1
array_index:
	.word 0
str_iter:
	.asciiz "# Iterations: "
eol:
	.asciiz "\n"
	.align 2
main_ret_save:
	.space 4

####################
# Main Program Code
####################s

	.text
	.globl main
main:
	sw   $ra, main_ret_save

	# Print "# Iterations: "
	la   $a0, str_iter			# print "# Iterations: "
	li   $v0, 4
	syscall
	la   $a0, eol				# print newline
	li   $v0, 4
	syscall

	# Scan and store max_iter input
	li   $v0, 5
	syscall
	sw   $v0, max_iter

	# Set up counter and end values
	li   $s0, 0					# row counter
	li   $s1, 0					# col counter
	li   $s2, 0					# iter counter
	li   $s5, 0					# array index

	lw   $s3, max_iter			# store max_iter
	lw   $s4, N 				# store board size N

# Keep track of iterations
iter_loop:
	beq  $s2, $s3, end_main 	# while (iter_ctr != max_iter)
	li   $s0, 0					# 		reset row ctr
	li   $s1, 0					# 		reset col ctr
	li   $s5, 0

# Keep track of row progression
row_loop:
	beq  $s0, $s4, end_row_loop # while (row_ctr != N)
	li   $s1, 0					# 		reset col ctr

# Keep track of col progression + do main work
col_loop:
	beq  $s1, $s4, end_col_loop # while (col_ctr != N)

	# do stuff

	lb   $a0, board($s5)		# 		load byte at board[t0]
	li   $v0, 11				#		print char
	syscall

	addi $s5, $s5, 1 			# array_index++
	addi $s1, $s1, 1    		# col ctr++, goto next col
	j 	 col_loop

# End of rows
end_row_loop:
	addi $s2, $s2, 1 	# iter ctr++
	j    iter_loop		# -> next iteration

# End of columns
end_col_loop:
	addi $s0, $s0, 1	# row ctr++
	la   $a0, eol
	li   $v0, 4
	syscall				# print newline
	j 	 row_loop 		# -> next row

# End of main
end_main:
	lw   $ra, main_ret_save
	jr   $ra

##################
# Other Functions
##################



# Checks curr board and updates state
board_update:



