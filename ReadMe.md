### 《计算机系统要素》学习记录

##
chapter1 基本逻辑门

Nand -- 基本的与非门，最底层的逻辑单元，所有其他的逻辑门都可以用与非门实现
Not -- 非门，~x
And -- 与门，x & y
Or -- 或门，x | y
Xor -- 异或门，同 0 异 1
Mux -- (Multiplexor)选择器，输入 a，b，sel；sel 为 0，输出 a，否则输出 b
DMux -- 反向选择器，输入 in，sel；sel 为 0，信号从 a 管脚输出，否则从 b 管脚输出
Not16 -- 16 位非门
And16 -- 16 位与门
Or16 -- 16 位或门
Mux16 -- 16 位选择器，sel 仍为 1 位
Or8Way -- 8 通道或门，8 位输入数据中任意一位为 1，输出为 1，否则输出为 0
Mux4Way16 -- 4 通道 16 位选择器，sel 为两位表示 0~4；sel 为 00 ，输出 a；sel 为 01 输出 b，依次类推
Mux8Way16 -- 8 通道 16 位选择器，sel 为三位表示 0~8；可选的有 8 个输入
DMux4Way -- 4 通道反向选择器，sel 有两位可以选择 4 种输出管脚位置
DMux8Way -- 8 通道反向选择器，sel 3 位，8 个输出管脚

##
chapter2

HalfAdder -- 半加器，用于 2 位二进制数加法
FullAdder -- 全加器，用于 3 位二进制数加法
Add16 -- 16 位加法器
Inc16 -- 16 位整数 + 1
ALU -- 算数逻辑单元，本章的核心

##
chapter3 时序芯片

Bit -- 1比特位寄存器，用于存储信息的1个比特位
Register -- 16 位寄存器
RAM8 -- 8 寄存器，这算是一个小型的内存单元，有地址的概念，可读写，可寻址，可记忆





