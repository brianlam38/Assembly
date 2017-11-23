# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by BRIAN LAM, August 2017

	.data
	.align 2
# test board data
N:
	.word 4
board:
	.byte '1', '1', '1', '1'			# SMALLER SIZE BOARD
	.byte '1', '0', '0', '1'			# EASIER FOR TESTING
	.byte '1', '0', '0', '1'			# REMEMBER TO CHANGE LABEL 'N' TO MATCH #ROWS/#COLS
	.byte '1', '1', '1', '1'
newBoard:
	.space 16
# main data
max_iter:
	.word 1
array_index:
	.word 0
str_iter:
	.asciiz "# Iterations: "
eol:
	.asciiz "\n"
# board data
board_print1:
	.asciiz "=== After iteration "
board_print2:
	.asciiz " ===\n"
# neighbours data
nn_str:
	.asciiz "index = "
nn_cell_val:
	.asciiz "cell value = "
nn_num_neigh:
	.asciiz "neighbours = "
nn_check_alive:
	.asciiz "=== ALIVE / DEAD ===\n"
nn_alive:
	.asciiz " <<<  cell is alive\n"
nn_skip:
	.asciiz "!!! OUT OF BOUNDS - SKIP !!!\n"
nn_same:
	.asciiz "!!! SAME  CELL  -  SKIP  !!!\n"
x_nn:
	.word -1
y_nn:
	.word -1
nn:
	.word 0
# return addresses
	.align 2
main_ret_save:
	.space 4
board_ret_save:
	.space 4
neighbours_ret_save:
	.space 4

################
# MAIN FUNCTION
################

	.text
	.globl main
main:
	sw   $ra, main_ret_save
	# Print "# Iterations: "
	la   $a0, str_iter
	li   $v0, 4
	syscall
	# Scan and store max_iter input
	li   $v0, 5
	syscall
	sw   $v0, max_iter
	# Set up values
	li   $s0, 0			# row ctr
	li   $s1, 0			# col ctr
	li   $s2, 0			# iter ctr
	li   $s5, 0			# array index
	lw   $s3, max_iter	# max_iter
	lw   $s4, N 		# board size N

iter_loop:							# track iterations
	beq  $s2, $s3, end_main

row_loop:							# while row < N
	beq  $s0, $s4, end_row_loop
	li   $s1, 0

col_loop:							# while col < N, perform game
	beq  $s1, $s4, end_col_loop
	# count neighbours
	jal  neighbours
	# do stuff
	lb   $a0, board($s5)		# load cell to $reg
	sb   $a0, newBoard($s5)		# store cell in newBoard
	# next col, array++
	addi $s5, $s5, 1
	addi $s1, $s1, 1
	j 	 col_loop

end_row_loop:					# update board, goto next iteration
	addi $s2, $s2, 1 	# iter ctr++
	jal  board_update	# -> board_outer and link
	j    iter_loop		# -> next iteration

# End of columns
end_col_loop:
	addi $s0, $s0, 1	# row ctr++
	j 	 row_loop 		# -> next row

# End of main
end_main:
	lw   $ra, main_ret_save
	jr   $ra

####################
# copyBackAndShow()
####################

board_update:							# update board and print current state
    sw   $ra, board_ret_save
    # reset counter values
	li   $s0, 0
	li   $s1, 0
	li   $s5, 0
	# print no. iterations
	la   $a0, board_print1
	li   $v0, 4
	syscall
	move $a0, $s2
	li   $v0, 1
	syscall
	la   $a0, board_print2
	li   $v0, 4
	syscall

board_outer:							# board row loop, while row < N
	beq  $s0, $s4, end_board_outer
	li   $s1, 0

board_inner:							# board col loop, while col < N
	beq  $s1, $s4, end_board_inner
	# print newBoard cell
	lb   $a0, newBoard($s5)
	li   $v0, 11
	syscall
	# increment array + col counter
	addi $s5, $s5, 1
	addi $s1, $s1, 1
	j 	 board_inner

end_board_inner:						# end of cols
	# print newline
	la   $a0, eol
	li   $v0, 4
	syscall
	# jump to next row
	addi $s0, $s0, 1
	j 	 board_outer

end_board_outer:						# end of rows
	# reset counters
	li   $s0, 0
	li   $s1, 0
	li   $s5, 0
	# return to next iteration
	lw   $ra, board_ret_save
	jr   $ra

###############
# neighbours()
###############

neighbours:							# update board and print current state
    sw   $ra, neighbours_ret_save

		# ------------------ PRINT ARRAY INDEX
		la   $a0, nn_str
		li   $v0, 4
		syscall
		move $a0, $s5
		li   $v0, 1
		syscall
		la   $a0, eol
		li   $v0, 4
		syscall

    # set up init values
    lw   $s6, x_nn # x rows
    lw   $s7, y_nn # y cols

    lw   $a1, N
    addi $a1, $a1, -1 # a1 = N-1

    li   $a2, 2 	  # const val 1

    # reset nn
    li   $t0, 0
    sw   $t0, nn

n_rows:							# loop LEFT <-> RIGHT
	beq  $s6, $a2, end_n_rows

n_cols:							# loop UP <-> DOWN, perform checks
	beq  $s7, $a2, end_n_cols

	# LHS/RHS: if out of bounds -> skip
	li   $t0, 0
	add  $t0, $s0, $s6 			# t0 = s0(i) + s6(x)
	bltz $t0, skip  			# i+x < 0
	bgt  $t0, $a1, skip 		# i+x > N-1

	# UP/DOWN: if out of bounds -> skip
	li   $t0, 0
	add  $t0, $s1, $s7 			# t0 = s1(j) + s7(y)
	bltz $t0, skip 				# j+y < 0
	bgt  $t0, $a1, skip 		# j+y > N-1

	# SAME SPOT -> skip
	li   $t0, 0
	bne  $s6, $t0, check_alive	 # row != 0, check cell
	bne  $s7, $t0, check_alive   # col != 0, check cell
	j    same

# Continue to next cell
continue:
	addi $s7, $s7, 1
	j    n_cols

skip:
	#la   $a0, nn_skip
	#li   $v0, 4
	#syscall
	addi $s7, $s7, 1
	j    n_cols

same:
	#la   $a0, nn_same
	#li   $v0, 4
	#syscall
	addi $s7, $s7, 1
	j    n_cols

# Jump to next row
end_n_cols:
	addi $s6, $s6, 1	# row ctr++
	li   $s7, -1 		# reset col ctr
	j 	 n_rows

# End of all neighbours check
end_n_rows:

		# ------------------ PRINT CELL VALUE
		#la   $a0, nn_cell_val
		#li   $v0, 4
		#syscall	
		#lb   $a0, board($s5)
		#li   $v0, 11
		#syscall
		#la   $a0, eol
		#li   $v0, 4
		#syscall

		# ------------------ PRINT NUM NEIGHBOURS
		la   $a0, nn_num_neigh
		li   $v0, 4
		syscall	
	    lw   $a0, nn
	    li   $v0, 1
	    syscall
	    la   $a0, eol
	    li   $v0, 4
	    syscall

	# return nn and link back
	lw   $ra, neighbours_ret_save
	jr   $ra

check_alive:
		# ------------------ PRINT ALIVE OR DEAD CHECK
		#la   $a0, nn_check_alive
		#li   $v0, 4
		#syscall

	# cell != 1 -> is dead, skip
	li   $t0, '1'

	# cell pos = array + (x_rows * N) + (y cols)
	lw   $t1, N
	mul  $t2, $s6, $t1  # x_rows * N
		# PRINT X VAL 								# SIDES ARE NOT BEING CHECKED !!! y -1
		#move $a0, $s6
		#li   $v0, 1
		#syscall
		#la   $a0, eol
		#li   $v0, 4
		#syscall
	add  $t2, $t2, $s7  # + y cols
		# PRINT Y VAL
		#move $a0, $s7
		#li   $v0, 1
		#syscall
		#la   $a0, eol
		#li   $v0, 4
		#syscall
	add  $t3, $s5, $t2  # + array
		# print board[i+x][j+y]
		#lb   $a0, board($t3)	
		#li   $v0, 11
		#syscall
	lb   $t4, board($t3)
	bne  $t0, $t4, continue    # if ('1' != cell_val)
		# cell == 1 -> is alive, nn++
		#la   $a0, nn_alive
		#li   $v0, 4
		#syscall
	# increment nn++
    lw   $a3, nn
    addi $a3, $a3, 1
    sw   $a3, nn

    # continue to next cell
	j    continue

