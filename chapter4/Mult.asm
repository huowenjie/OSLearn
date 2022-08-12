// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[3], respectively.)

// Put your code here.
// i = 0
// R2 = 0
// if R0 <= 0 goto end
// if R1 <= 0 goto end
// while (i < R1) {
//     R2 = R2 + R0
//     i += 1

@i
M = 0
@R2
M = 0

@R0
D = M

@END
D;JLE

@R1
D = M

@END
D;JLE

(LOOP)
@R1
D = M

@i
D = M - D

@END
D;JGE

@R0
D = M

@R2
M = M + D

@i
M = M + 1

@LOOP
0;JMP

(END)
@END
0;JMP






