// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put your code here.

// 16384 为屏幕左上角起始地址，256 * 512 像素
// 24576 为键盘的内存地址
// RAM[16384] = 65535
// while (i < 8192)
//     clear
// if RAM[24576] then 
//    while ...
//        render

(LOOP)

@i
M = 0

@24576
D = M

@RENDER
D;JGT

(CLEAR)

@24576
D = M

@LOOP
D;JGT

@8192
D = A

@i
D = M - D

@LOOP
D;JGE

@i
D = M

@SCREEN
A = A + D
M = 0

@i
M = M + 1

@CLEAR
0;JMP

(RENDER)

@24576
D = M

@LOOP
D;JLE

@8192
D = A

@i
D = M - D

@LOOP
D;JGE

@i
D = M

@SCREEN
A = A + D
M = -1

@i
M = M + 1

@RENDER
0;JMP

@LOOP
0;JMP
