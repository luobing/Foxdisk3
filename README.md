# Foxdisk3
BootLoader Tool for x86(based on Legacy BIOS, like grub)
* Foxdisk3.01起源于Foxdisk1和Foxdisk2
* 最早是公司内部需要这么一个软件：研发部的机器上，有多种操作系统的需求，不能使用Grub，各操作系统间要相互独立
*                             然后八仙过海，各自写。我写的Foxdisk也有几个同事使用，运行了好几年
* 我觉得上一版有很多不足，就自己立了Foxdisk3的计划；
* 采用C&汇编的混合编程模式，以C为主；
* 张佩觉得很有意思，怂恿我把它写成系列博客；
* 因此我把相关的代码全部开源出来了；
* 先声明：Foxdisk3我自己只用了3个多月，后面也发现了一些小BUG，我没有精力再去修改了，何况UEFI当时也出来了，我更没有动力去维护；
* 除去硬盘管理部分，参考了Spfdisk外，其余代码都是我自己写的。有需要哪去用，估计嵌入式开发中可以参考一下；
* 博客地址：
* http://yiiyee.cn/blog/author/luobing/
* csdn:luobing4365

*                                                                                            --20190902 luobing
