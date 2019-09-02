	unsigned char *savepart_str[]={
		"分区状态错误,不能存储您的改动!",
		"分区有改动,您要存储吗(Y/N)?: %c",
		"您没有设置活动分区,仍然继续存储并退出分区管理吗(Y/N)?:%c",
		"抱歉,意外错误导致写入失败!",
		"保存成功!"
	};
	"十六剩常余",
	unsigned char *paramenu_str[]={  
		"      设置倒计时 ",  
		"      取消倒计时 ",  
		"      设置缺省网 ",  
		"      更 换 壁 纸",  
		"      隐 藏 壁 纸",
		"     返回系统菜单",	
	};
	unsigned char *exitPara_str[]={
		"参数设定-退出处理",
		"您已经改变参数,是否确定保存(Y/N)?:%d",
		"保存成功,重启计算机后生效!",
		"保存失败,硬盘可能有坏道存在!"
	};
	unsigned char *cntdown_str[]={
		"注意:取消倒计时后,在设置中输入新的时间即意味着打开倒计时.",
		"当前倒计时状态: ","打开","关闭",
		"当前倒计时时间(秒): ","无效",
		"请输入新的时间数(3--30): %d",
		"您确定要关闭倒计时吗(Y/N)?:%c",
		"已接受您的输入,按任意键退出...",	
		"%d"
	};
	unsigned char *setdefos_str[]={
		"所有系统信息如下:",
		"系统%d: %s","不存在",
		"当前缺省系统编号: 系统%d.%s",
		"抱歉,当前只有不大于一个的系统存在,不能设置!",
		"请选择您要设置为缺省启动的系统(1--4):%d ",
		"设置成功! 任意键退出..."
	};
	unsigned char *setwallp_str[]={
		"无法即时显示壁纸,非常抱歉.此功能列入了下一版本的开发计划中",
		"当前壁纸数: %d","隐藏",
		"当前使用的壁纸编号: ",
		"没有可用的壁纸,无法执行.",
		"请选择新的壁纸(1--%d):","%d",
		"已接受您的输入,任意键退出...",
		"%s",
		"您确定要执行吗(Y/N)?:%c"   
	};
unsigned char *cntdown_str[]={
		"注意:取消倒计时后,在设置中输入新的时间即意味着打开倒计时.",
		"当前倒计时状态: ","打开","关闭",
		"当前倒计时时间(秒): ","无效",
		"请输入新的时间数(3~30): %d",
		"您确定要关闭倒计时吗(Y/N)?:%c",
		"已接受您的输入,按任意键退出...",	
		"%d"
	};
	unsigned char *about_str[]={
		"知更鸟的小屋(R) Foxdisk",
		"版本号3.01.02",
		"版权所有(C)2008-2108",
		"罗冰2008.11撰于南京 江宁枫彩园蜗居",
		"时值冬至,秦淮河水在晚上散发着奇怪的味道,空负了个水月秦淮的美名.",
	};
unsigned char *delPartn_str[]={																/* 删除分区所用的字符串 */
		"分区信息: 编号:%d   起始磁道:%ld  结束磁道:%ld",
		"您确定要删除此分区(Y/N)?: %c"
	};	
unsigned char helpTitle[]="帮助文档";
unsigned char *fdisk_str[]={
	"发现分区重迭,你使用的分区工具有问题,找他去,别找我!",
	"分区表存在错误,本软件没有提供类似的容错机制!",
	"硬盘访问错误,有可能出现坏道了,使用工具集中的磁盘检测看看.",
	"    按任意键退出显示...",
	"分区号  活动  起始磁道  结束磁道  容量(M)   分区类型  分区类型描述",
	" %d      %c   %ld       %ld       %ld        %x           %s"
};
unsigned char *chkpart_str[]=
	{
		"未检测","正在检测","坏道",
		"起始磁道:%ld 结束磁道:%ld","总磁道数:%ld  总坏道数:%ld  当前磁道:%ld",
		"提示:您确定要进行分区的表面坏道检测吗(Y/N)?: %c",
		"提示:检测完毕,恭喜,没有发现坏道^^",
		"提示:检测完毕,发现坏道数%ld,您要显示详情吗(Y/N)?",
		"提示:[ESC]键退出检测 空格键暂停检测",
		"提示:按[ESC]键退出显示."
	};
unsigned char *fdisk_str[]={
	"发现分区重迭,你使用的分区工具有问题,找他去,别找我!",
	"分区表存在错误,本软件没有提供类似的容错机制!",
	"硬盘访问错误,有可能出现坏道了,使用工具集中的磁盘检测看看.",
	"    按任意键退出显示...",
	"分区号 活动  起始磁道  结束磁道  容量(M)  分区类型  分区类型描述",
	" %d     %c   %ld       %ld       %ld       %x           %s",
	"[逻辑分区]"
};
unsigned char *chkpart_str[]=
	{
		"未检测","正在检测","坏道",
		"总磁道数:%ld  总坏道数:%ld  当前磁道:%ld",
		"提示:您确定要进行分区的表面坏道检测吗(Y/N)?: ",
		"提示:检测完毕,恭喜,没有发现坏道^^",
		"提示:检测完毕,发现坏道数%ld,您要显示详情吗(Y/N)?",
		"提示:按ESC键退出显示."
	};
unsigned char *part_fun[]={
	" 创建主分区 ",
	"创建扩展分区",
	"创建逻辑分区",
	"清除活动标志",
	"设置活动标志",
	"设置分区类型",
	"删 除 分 区 ",
	"分区表面检测",
	"  退    出  "
};

char ID_00[] = "none";
char ID_01[] = "DOS FAT12";
char ID_02[] = "XENIX root";
char ID_03[] = "XENIX usr";
char ID_04[] = "DOS FAT16 <32M";
char ID_05[] = "Extended";
char ID_06[] = "DOS FAT16 >=32M";
char ID_07[] = "HPFS/NTFS";
char ID_08[] = "AIX";
char ID_09[] = "AIX bootable";
char ID_0A[] = "OS/2 Boot Manag";
char ID_0B[] = "DOS FAT32";
char ID_0C[] = "DOS FAT32(LBA)";
char ID_0E[] = "DOS FATs(LBA)";
char ID_0F[] = "DOS Extend(LBA)";
char ID_10[] = "OPUS";
char ID_12[] = "Compaq diagnost";
char ID_18[] = "AST WindowsSwap";
char ID_20[] = "SPF Boot Manage";
char ID_24[] = "NECDOS";
char ID_39[] = "Plan9";
char ID_3C[] = "Partition Magic";
char ID_40[] = "Venix 80286";
char ID_41[] = "PPC PReP Boot";
char ID_42[] = "SFS";
char ID_4D[] = "QNX4.x";
char ID_4E[] = "QNX4.x 2nd part";
char ID_4F[] = "QNX4.x 3nd part";
char ID_50[] = "OnTrack DM";
char ID_51[] = "OnTrack DM6 Aux";
char ID_52[] = "CP/M";
char ID_53[] = "OnTrack DM6 Aux";
char ID_54[] = "OnTrack DM6";
char ID_55[] = "EZ-Drive";
char ID_56[] = "Golden Bow";
char ID_5C[] = "Priam Edisk";
char ID_61[] = "SpeedStor";
char ID_63[] = "GNU HURD";
char ID_64[] = "Novell Netware";
char ID_65[] = "Novell Netware";
char ID_70[] = "DiskSecure Mult";
char ID_75[] = "PC/IX";
char ID_80[] = "Old MINIX";
char ID_81[] = "MINIX/old Linux";
char ID_82[] = "Linux swap";
char ID_83[] = "Linux native";
char ID_84[] = "hidden OS/2 C:";
char ID_85[] = "Linux extended";
char ID_86[] = "NTFS volume set";
char ID_87[] = "NTFS volume set";
char ID_8E[] = "Linux LVM";
char ID_93[] = "Amoeba";
char ID_94[] = "Amoeba BBT";
char ID_9F[] = "BSD/OS";
char ID_A0[] = "IBM Thinkpad hi";
char ID_A5[] = "FreeBSD";
char ID_A6[] = "Open BSD";
char ID_A7[] = "NEXTSTEP";
char ID_A9[] = "NetBSD";
char ID_B7[] = "BSDI fs";
char ID_B8[] = "BSDI swap";
char ID_C1[] = "DRDOS/sec FAT";
char ID_C4[] = "DRDOS/sec FAT";
char ID_C6[] = "DRDOS/sec FAT";
char ID_C7[] = "Syrinx";
char ID_DA[] = "Non-FS data";
char ID_DB[] = "CP/M";
char ID_DE[] = "Dell Utility";
char ID_E1[] = "DOS access";
char ID_E3[] = "DOS R/O";
char ID_E4[] = "SpeedStor";
char ID_EB[] = "BeOS fs";
char ID_EE[] = "EFI GPT";
char ID_EF[] = "EFI (FAT-12/16)";
char ID_F1[] = "SpeedStor";
char ID_F2[] = "DOS secondary";
char ID_F4[] = "SpeedStor";
char ID_FE[] = "LAN step";
char ID_FF[] = "BBT";

char ID_UN[] = "未知类型";
char ID_FP[] = "软盘";
char ID_BD[] = "备份ID遗失";
char ID_NE[] = "[无法启动]";

unsigned char *helpStr[]={
	"                 Foxdisk3.01 帮助文档",
	"一 简述 ",
	"    本程序由五个部分组成:帮助文档(F1) 系统菜单(F2) 参数设定(F3) 工",
	"具集(F4) 关于(F5).",
	"帮助文档:在系统菜单出现的时候使用'F1'可以调出,对程序的简单说明    ",
	"系统菜单:提供对系统进行分区管理 更改系统菜单名称 调整菜单顺序 以及",
	"         设置密码的功能.",
	"参数设定:提供设置倒计时和取消倒计时 设置缺省系统 更换壁纸的功能",
	"工具集:  全盘坏道检测 扇区信息显示 快速格式化 虚拟软盘 LBOS1.0 ",
	"         在这个版本中还没有实现这些功能,这是一个督促自己的计划.",
	"关于:    程序的版本和作者信息",
	"二 名词解释",
	"    缺省系统:当前最多可引导4个不同的系统,可以由用户设定启动的时候",
	"             进入其中一个,这个就是缺省系统.",
	"    系统菜单:指可以引导的四个系统的显示菜单.",
	"    壁纸:启动的过程中的背景图案,可以在安装的时候指定,必须是256色的",
	"         BMP图.可以安装多个,在参数设定中使用更换功能来选择",
	"三 键盘使用说明",
	"    可以使用方向键中的上下键和Tab键来更换选定菜单,使用方向键中的右",
	"键来唤出功能菜单(当然是有功能菜单提供的情况下.所以,在能够使用上下",
	"键的菜单中试试用方向键的右移看看,说不定就有新的发现^^)",
	"    注意:所有更改均在内存中进行,退出Foxdisk前才会存档!!! ",
	"其他功能自己慢慢试验吧,祝使用愉快!",
	"                                                ---知更鸟的小屋---"
	};
	unsigned char *sysMenu_Sort[]={
	"提示: 通过互换两个不同系统的方法来实现调换菜单显示顺序.",
	"当前菜单:1 %s 2 %s",
	"         3 %s 4 %s",
	"请输入欲调换的源菜单号(1~4): %d",
	"请输入欲调换的目的菜单号(1~4): %d",
	"更换成功,您还需要继续更换吗(Y/N)?: %c"
	"更换结果为:  1.%s  2.%s",
	"             3.%s  4.%s",
	"    按任意键盘结束..."
		"    操作完成,按任意键结束..."
		"提示: 为当前系统设置密码或者取消密码,取消密码可以通过输入空字符实",
	"      现.只允许最大12字节长的由数字或字母组成的密码.",
	"当前系统名: %s",
	"请输入原来的密码: %s",
	"您的输入不正确,还要继续尝试吗?(Y/N): ",
	"请输入新的密码: %s",
	"请确认新密码:   %s",
	"输入不匹配,继续设置密码吗:(Y/N)?: %c",
	"密码设置成功,按任意键结束...",
	"密码取消成功,按任意键结束...",
};
unsigned char *hideFun_str[]={
	"luobing",
	"隐藏的功能菜单",
	"请输入管理员密码: %s",
	"错误! 您没有获取敏感信息的权限.按任意键退出...",
	"系统%d: 名称为%s  密码为%s  对应MBR为0x%lx",
	"    显示完毕,按任意键盘退出..."
};
/* 系统功能菜单 */
#define SYSFUN_NUM 5							/* 系统菜单中的功能菜单数目 */
unsigned char *sysFun_str[]={"分 区 管 理 ","系统菜单命名","系统菜单排序","设 置 密 码 ","   退  出   "};
/* 更名系统菜单 */
unsigned char *sysFun_Name[]={
	"提示: 最大支持12个字符名称,请尽量简化自己的系统名称.",
	"      并且接受空字符,所以取什么名字完全由你自己决定.",
	"  当前系统名: %s",
	"  请输入新的系统名: %s",
	"  新的系统名为: %s",
	"已经更改成功,按任意键结束..."
};
unsigned char *sysFun_Name[]={
	"提示: 最大支持12个字符名称,尽量简化自己的系统名称.可以接受",
	"      空字符(相当于清除了系统名),选择你喜欢的名字填入吧.",
	"  当前系统名: %s",
	"  请输入新的系统名: %s",
	"  新的系统名为: %s头",
	"已经更改成功,按任意键结束..."
};
"分区号 当前所属编辑框的行数活动分区 起始磁道 结束磁道 容量 分区ID 分区类型 硬盘类型分区管理系统菜单命名系统菜单排序退出帮助参数设定工具集 关于 取消倒计时设置缺省网设置时间显示设置音效返回系统菜单分 区 管理陈林圈圈我老婆ｗｉｎｘｐｆｅｄｏｒａＦＷＩＮＸＰＥＤＯＲＡＣＯＲＥ３５１４ＡＢＣ棋魂如风而来,如雨而至,君如暖风煦煦,伴我夜寐晨醒知更鸟的小屋"