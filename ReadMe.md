### 《计算机系统要素》学习记录

##
chapter1 布尔逻辑  

Nand：基本的与非门，最底层的逻辑单元，所有其他的逻辑门都可以用与非门实现  
Not：非门  
And：与门  
Or：或门  
Xor：异或门，同 0 异 1  
Mux：(Multiplexor)选择器，输入 a，b，sel；sel 为 0，输出 a，否则输出 b  
DMux：反向选择器，输入 in，sel；sel 为 0，信号从 a 管脚输出，否则从 b 管脚输出  
Not16：16 位非门  
And16：16 位与门  
Or16：16 位或门  
Mux16：16 位选择器，sel 仍为 1 位  
Or8Way：8 通道或门，8 位输入数据中任意一位为 1，输出为 1，否则输出为 0  
Mux4Way16：4 通道 16 位选择器，sel 为两位表示 0 到 4；sel 为 00 ，输出 a；sel 为 01 输出 b，依次类推  
Mux8Way16：8 通道 16 位选择器，sel 为三位表示 0 到 8；可选的有 8 个输入  
DMux4Way：4 通道反向选择器，sel 有两位可以选择 4 种输出管脚位置  
DMux8Way：8 通道反向选择器，sel 3 位，8 个输出管脚  

##
chapter2 布尔运算  

HalfAdder：半加器，用于 2 位二进制数加法  
FullAdder：全加器，用于 3 位二进制数加法  
Add16：16 位加法器  
Inc16：16 位整数 + 1  
ALU：算数逻辑单元，本章的核心  

##
chapter3 时序逻辑  

Bit：1比特位寄存器，用于存储信息的1个比特位  
Register：16 位寄存器  
RAM8：8 寄存器，这算是一个小型的内存单元，有地址的概念，可读写，可寻址，可记忆  
RAM64：64 寄存器，由 8 个 RAM8 组成  
PC：16 位计数器  

##
chapter4 机器语言  

Mult.asm：乘法器  
Fill.asm：按键渲染，放键清屏  

##
chapter5 计算机体系结构  

Memory.hdl：内存  
CPU.hdl：Hack 计算机的中央处理器实现  
Computer：Hack 计算机

##
chapter6 汇编编译器  
这个 HackAsm 编译器我用 C 语言实现，仅仅简单测了一下其编译功能。  
用法：  
1.执行 make 编译项目；  
2.执行 ./HackAsm XXX.asm 即可输出对应的机器码文件 XXX.hack
