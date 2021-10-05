/*
 *	2020年10月22日 16点08分 ☆4

	使用外部输入 INT0(P3.2)、INT1(P3.3) 触发中断，
	三个 LED 的初始值为 001， 中断触发的话流水灯就会向左位移或者向右位移

	由于条件限制，没有信号发生器，这里使用下降沿触发中断，实验效果不会太好

	以下寄存器和外部中断相关
	
	定时器控制寄存器 TCON [7:0]
	
	TF1 TR1 TF0 TR0 IE1 IT1 IE0 IT0
		7   6   5   4   3   2   1   0
		
		TF1:		Timer1 溢出标志位，进入中断后清零
		TR1:		Timer1 启动/停止位，置 1 启动定时器，置 0 关闭定时器
		TF0:		Timer0 同 TF1
		TR0:		Timer0 同 TR1
		
		IE1:		外部中断 INT1 外部中断请求标识位，进入中断后自动清零
		IT1:		外部中断 INT1 触发控制，0 表示低电平触发，1 表示下降沿触发。
		
		IE0:		Timer0 同 IE1
		IT0:		Timer0 同 IT1

	中断使能寄存器 IE [7:0]

		EA E_DIS ET2 ES ET1 EX1 ET0 EX0
			7   6    5   4  3   2   1   0

		EA:			全局中断使能位，置 1 并且 E_DIS == 0 允许中断，置 0 屏蔽中断
		E_DIS:		全局中断禁用控制位，置 1 屏蔽中断，置 0 并且 EA == 1 允许中断
		ET2:		定时器 T2 中断使能位，置 1 允许 T2 中断，置 0 屏蔽
		ES:			UART0 中断使能位，置 1 允许 UART0 中断，置 0 屏蔽
		ET1:		定时器 T1 中断使能位，同 ET2
		EX1:		外部中断 INT1 使能位，置 1 允许中断，置 0 屏蔽
		ET0:		定时器 T0 中断使能位，同 ET2
		EX0:		外部中断 INT0 使能位，同 EX1

	CH559 的中断源相当多，这里仅仅使用 INT0 外部中断一个，下面的是中断号表
	
		0	--	外部中断 INT0
		1	--	定时器 T0 中断
		2	--	外部中断 INT1
		3	--	定时器 T1 中断
		4	--	UART0 中断
		5	--	定时器 T2 中断
		6	--	SPIO 中断
		7	--	定时器 T3 中断
		8	--	USB 中断
		9	--	ADC 中断
		10	--	UART1 中断
		<<		中断 11 没有标，未知 
		12	--	GPIO 中断
		13	--	Watchdog 中断

 */
 
#include"CH559.H"

void P1_Configure(void){

    // PORT_CFG[3:0] -- P3、P2、P1、P0
    // P1 为 推挽模式，第二位 PROT_CFG[1]
    // xxxx_xxxx & 1111_1101 = xxxx_xx0x
    PORT_CFG    = PORT_CFG & 0xFD;

	// LED 连接 P1^7、P1^6、P1^5、P1^4 
    // 禁用上拉电阻,PU[7:0] 分别对应 P1^7 - P1^0
    // 每个引脚的上拉可以单独控制
    // xxxx_xxxx & 0000_1111 = 0000_xxxx 
    P1_PU       = P1_PU & 0x0F;
    // DIR 选输出模式，同样的每个引脚都可以单独控制 11110000
	// xxxx_xxxx | 1111_0000 = 1111_xxxx
    P1_DIR      = P1_DIR | 0xF0;

}

void P3_Configure(void){

    // PORT_CFG[3:0] -- P3、P2、P1、P0
    // P3 为 开漏模式，第三位 PORT_CFG[3]
    // xxxx_xxxx | 0000_1000 = xxxx_1xxx
    PORT_CFG    = PORT_CFG | 0x08;

	// INT0 INT1 连接的引脚分别为 P3.2 P3.3
    // 开启上拉电阻，xxxx_xxxx | 0000_1100 = xxxx_11xx
    P3_PU       = P3_PU | 0x0C;
    // DIR 选输出模式，同样的每个引脚都可以单独控制
	// xxxx_xxxx | 0x0000_1100 = xxxx_11xx
    P3_DIR      = P3_DIR | 0x0C;
}

void interrupt_init(void){

	// 开启中断使能
	EA = 1;
	E_DIS = 0;

	// 允许外部输入中断 INT0 和 INT1
	// 低电平触发
	EX1 = 1;
	IT1 = 1;
	EX0 = 1;
	IT0 = 1;
}

signed char a = 0;

void main(void){
	interrupt_init();
	P1_Configure();
	P3_Configure();

	P1 = 0xFF;
	while(1);
}

// 中断函数格式，void 函数名称(void) interrupt 中断号 {}
// 中断号由硬件决定，外部中断 INT0 和 INT1 的中断号分别为 0 和 3
// 当中断触发时，系统就会自动运行中断函数
void INT0_Interrupt(void) interrupt 0 {

	if(a>=3)
		a = 0;
	else	
		a++;

	P1 = (0x10<<a);
	
}	

void INT1_Interrupt(void) interrupt 2{

	if(a<0)
		a = 3;
	else	
		a--;

	P1 = (0x10<<a);
}
