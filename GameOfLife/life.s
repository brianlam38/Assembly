# board.s ... Game of Life on a 10x10 grid

   .data

N: .word 10  # gives board dimensions

board:
   .byte 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
   .byte 1, 1, 0, 0, 0, 0, 0, 0, 0, 0
   .byte 0, 0, 0, 1, 0, 0, 0, 0, 0, 0
   .byte 0, 0, 1, 0, 1, 0, 0, 0, 0, 0
   .byte 0, 0, 0, 0, 1, 0, 0, 0, 0, 0
   .byte 0, 0, 0, 0, 1, 1, 1, 0, 0, 0
   .byte 0, 0, 0, 1, 0, 0, 1, 0, 0, 0
   .byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
   .byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0
   .byte 0, 0, 1, 0, 0, 0, 0, 0, 0, 0

newBoard: .space 100
# prog.s ... Game of Life on a NxN grid
#
# Needs to be combined with board.s
# The value of N and the board data
# structures come from board.s
#
# Written by BRIAN LAM, August 2017

	.data
	.align 2
### Main data
max_iter:
	.word 1
array_index:
	.word 0
str_iter:
	.asciiz "# Iterations: "
eol:
	.asciiz "\n"
#### Show board data
board_print1:
	.asciiz "=== After iteration "
board_print2:
	.asciiz " ===\n"
alive:
	.asciiz "#"
dead:
	.asciiz "."
### Neighbours data
byte:
	.byte 1
byte_0:
	.byte 0
x_nn:
	.word -1
y_nn:
	.word -1
nn:
	.word 0
### Return addresses
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
	# printf("# Iterations: ");
	la   $a0, str_iter		
	li   $v0, 4
	syscall
	# scan and store max_iter input
	li   $v0, 5				
	syscall
	sw   $v0, max_iter
	# set up values
	li   $s0, 0					# row ctr
	li   $s1, 0					# col ctr
	li   $s2, 0					# iter ctr
	li   $s5, 0					# array index
	lw   $s3, max_iter			# max_iter
	lw   $s4, N 				# board size N
# for (iter < max_iter)
iter_loop:							
	beq  $s2, $s3, end_main
# for (row < N)
row_loop:
	beq  $s0, $s4, end_row_loop
	li   $s1, 0
# for (col < N), run Game of Life
col_loop:
	beq  $s1, $s4, end_col_loop
	# grab num neighbours
	jal  neighbours 			
	lb   $a0, board($s5)
	lb   $t0, byte
	# if (cell == 1) -> living
	beq  $a0, $t0, living   
	# else -> not_living	
	j    not_living				
not_living:
	li   $t0, 3
	lw   $t1, nn
	# if (nn == 3) -> reproduce
	beq  $t1, $t0, reproduce
	# else -> kill
	j    kill				  	
living:
	li   $t0, 2
	lw   $t1, nn
	blt  $t1, $t0, kill
	beq  $t1, $t0, reproduce
	li   $t0, 3
	beq  $t1, $t0, reproduce
	j    kill
# if (nn == 2 || nn == 3) -> reproduce
reproduce:
	lb   $t0, byte
	sb   $t0, newBoard($s5)
	j    continue_loop
# if (nn < 2 || nn > 3) -> kill
kill:
	lb   $t0, byte_0
	sb   $t0, newBoard($s5)
	j 	 continue_loop
# continue to next cell
continue_loop:
	addi $s5, $s5, 1
	addi $s1, $s1, 1
	j 	 col_loop
# iteration complete
end_row_loop:
	addi $s2, $s2, 1
	# copyBackAndShow()
	jal  board_update
	# next iteration
	j    iter_loop
# end of columns
end_col_loop:
	addi $s0, $s0, 1
	j 	 row_loop
# end of main, exit program
end_main:
	lw   $ra, main_ret_save
	jr   $ra

###############
# neighbours()
###############

# check for neighbours, then return nn.
neighbours:
    sw   $ra, neighbours_ret_save
    # x rows, y cols
    lw   $s6, x_nn
    lw   $s7, y_nn
    # N-1
    lw   $a1, N
    addi $a1, $a1, -1
    # stop val = 2
    li   $a2, 2
    # reset nn
    li   $t0, 0
    sw   $t0, nn
# loop LEFT <-> RIGHT
n_rows:
	beq  $s6, $a2, end_n_rows
# loop UP <-> DOWN, perform boundary checks
n_cols:
	beq  $s7, $a2, end_n_cols
	# LHS/RHS: if out of bounds -> skip
	li   $t0, 0
	add  $t0, $s0, $s6
	bltz $t0, continue 			 # i+x < 0
	bgt  $t0, $a1, continue		 # i+x > N-1
	# UP/DOWN: if out of bounds -> skip
	li   $t0, 0
	add  $t0, $s1, $s7 
	bltz $t0, continue 			 # j+y < 0
	bgt  $t0, $a1, continue 	 # j+y > N-1
	# SAME SPOT -> skip
	li   $t0, 0
	bne  $s6, $t0, check_alive	 # row != 0, check cell
	bne  $s7, $t0, check_alive   # col != 0, check cell
	j    continue
# continue to next cell
continue:
	addi $s7, $s7, 1
	j    n_cols
# jump to next row
end_n_cols:
	addi $s6, $s6, 1
	li   $s7, -1
	j 	 n_rows
# end of all neighbours check, return nn
end_n_rows:
	lw   $ra, neighbours_ret_save
	jr   $ra
# alive or dead cell check
check_alive:
	lb   $t0, byte
	# new cell pos = (x_rows * N) + y_cols + array
	lw   $t1, N
	mul  $t2, $s6, $t1  	# x_rows * N
	add  $t2, $t2, $s7  	# + y cols
	add  $t3, $s5, $t2  	# + array
	lb   $t4, board($t3)
	# if (board[i][j] == 0) continue
	bne  $t0, $t4, continue
	# else nn++
    lw   $a3, nn
    addi $a3, $a3, 1
    sw   $a3, nn
	j    continue

####################
# copyBackAndShow()
####################
# update board and print current state
board_update:
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
# for (row < N)
board_outer:
	beq  $s0, $s4, end_board_outer
	li   $s1, 0
# for (col < N)
board_inner:
	beq  $s1, $s4, end_board_inner
	# board[i][j] = newboard[i][j];
	lb   $a0, newBoard($s5)
	sb   $a0, board($s5)
	lb   $t0, byte
	beq  $a0, $t0, print_alive
# putchar '.' byte
print_dead:
	lb   $a0, dead
	li   $v0, 11
	syscall
	j    board_continue
# putchar '#' byte
print_alive:
	lb   $a0, alive
	li   $v0, 11
	syscall
# continue
board_continue:
	addi $s5, $s5, 1
	addi $s1, $s1, 1
	j 	 board_inner
# end of cols
end_board_inner:
	# print newline
	la   $a0, eol
	li   $v0, 4
	syscall
	# jump to next row
	addi $s0, $s0, 1
	j 	 board_outer
# end of rows
end_board_outer:
	# reset counters
	li   $s0, 0
	li   $s1, 0
	li   $s5, 0
	# ret to next Game of Life iteration
	lw   $ra, board_ret_save
	jr   $ra

