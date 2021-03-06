/* 
	2020年10月15日 10点57分 ☆4
 *	定时器中断实验

    定时器是单片机的一个重要的外设，工作原理大致如下：
	定时器首先被设置一个初始值（没有设置的话就是复位值 0x00),定时器启动之后，
	每个计数时钟周期，定时器超时寄存器自动加 1，直至加满（8位的就是加至 0xFF,16 位就是 0xFFFF)
	定时器超时，并产生一个信号(TF = 1),当定时器中断使能时触发中断

	定时器的时钟分为多种情况，标准时钟为 Fsys/12,除标准时钟之外定时器T0/T1还可以使用更快的快速时钟或者外部输入的时钟。

	这里的中断实验简单的使用系统时钟，标准时钟模式，开启定时器 T1 的中断，目的是输出 1Hz 的方波

 *	其中系统时钟为12MHz,12 倍分频后为 1MHz
 *	1M = 1000_000 16 位定时器最大计数范围为 2^16 = 65535,
 *	所以这里的计数器中断频率设为 0.05s，系统时钟周期为 0.000_001s 
 *	所以需要计数到 50_000 即 0xC350
 *   	0xFFFF - 0xC350 = 0x3CAF
 *  
 *	这里选择定时器 1，
 *	因此    TH1 = 0x3C TL1 = 0xAF
 *      
 *	以下是定时器 T1 的相关寄存器
 * 
 *	TCON [7:0] ---- 0x00 (复位值) 定时器控制寄存器
 *		TF1 TR1 TF0 TR0 IE1 IT1 IE0 IT0
 *       7   6   5   4   3   2   1   0
 *      
 *	TF1 ---- T1 溢出中断标识位，进入中断后自动清零
 *	TR1 ---- T1 启动停止位，置 1 启动，置 0 关闭，由软件置位或者清零
 *	TF0 ---- T0 溢出中断标识位，进入中断后自动清零
 *	TR0 ---- T1 启动停止位，置 1 启动，置 0 关闭，由软件置位或者清零
 *	IE1 ---- INT1 (P3.3) 外部中断请求标识位，进入中断后自动清零
 *	IT1 ---- INT1 (P3.3) 外部中断触发方式控制位，置 0 则为低电平触发，置 1 为下降边沿触发
 *	IE0 ---- INT0 (P3.2) 外部中断请求标识位，进入中断后自动清零
 *	IT1 ---- INT0 (P3.2) 外部中断触发方式控制位，置 0 则为低电平触发，置 1 为下降边沿触发

 *	TMOD [7:0] ---- 0x00 (复位值) 定时器方式寄存器 
 *		bT1_GATE bT1_CT bT1_M1 bT1_M0 bT0_GATE bT0_CT bT0_M1 bT0_M0
 *         7        6      5      4      3        2      1      0
 *      bT1_GATE ---- 门控使能位，控制 T1 的是否由外部信号启动，置 1 、TR1 == 1 并且 INT1 输入高电平时启动 T1
 *                    置 0 则忽略 INT1 的输入。
 *      bT1_CT   ---- 定时/计数方式选择位，0 则处于定时方式，1 则处于脉冲计数方式，使用 T1 的下降沿脉冲进行计数
 *      bT1_M1   ---- T1 模式选择高位
 *      bT1_M0   ---- T1 模式选择低位
 *      M1 : M0
 *      0    0		模式 0： 13 位定时/脉冲计时模式，由 TLn 的低 5 位和 THn 的 8 位构成，TLn 的高 3 位无效，
 *            		当计数器从 0b1111_1111_1111_1000 到 0b0000_0000_0000_0000 时，溢出标识位 TFn 置 1，
 *            		溢出后需要重载计数初值。
 *      0    1		模式 1： 16 位定时/脉冲计数模式，由 TLn 和 THn 共 16 位组成，定时器溢出时，溢出标志位 TFn 置 1
 *           		溢出后需要重载计数初值
 *      1    0		模式 2： 8  位自动重载模式，计数单元使用 TLn,THn 作为重载计数单元。自动重载计数。
 *      1    1		模式 3： T0 进入模式 3 会将定时器 T0 分割为两个独立的 8 位计数器，其中 TL0 交由 T0 控制，TH0 交由 T1 控制
 *             		其中 TH0 占用 T1 的全部控制位和中断资源。        
 *          		T1 进入模式 3 ，T1 会停止
 * 
 *	余下 4 位为 T0 的方式寄存器，方式与T1 相同
 * 
 *	中断相关寄存器，INT_NO_TMR1 定时器 T1 中断 的 中断号为 3
 * 
 *	中断使能寄存器 IE[7:0] ---- 0x00 (复位值)
 *  	EA E_DIS ET2 ES ET1 EX1 ET0 EX0
		7   6    5   4  3   2   1   0

		EA:		全局中断使能位，置 1 并且 E_DIS == 0 允许中断，置 0 屏蔽中断
		E_DIS:	全局中断禁用控制位，置 1 屏蔽中断，置 0 并且 EA == 1 允许中断
		ET2:	定时器 T2 中断使能位，置 1 允许 T2 中断，置 0 屏蔽
		ES:		UART0 中断使能位，置 1 允许 UART0 中断，置 0 屏蔽
		ET1:	定时器 T1 中断使能位，同 ET2
		EX1:	外部中断 INT1 使能位，置 1 允许中断，置 0 屏蔽
		ET0:	定时器 T0 中断使能位，同 ET2
		EX0:	外部中断 INT0 使能位，同 EX1

 */

#include"CH559.H"

sbit myLED1 = P1^4;         // P1^4 外接 LED 灯
unsigned char cnt = 0;      // 循环计数器，定时器中断0.05s，计数 10 次就是 1s
unsigned char tmp = 0;
// P1 的设置，设置好 4 个 LED
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


/********************************************************/
/*
1. 定时器 T1 初始化,选择模式 1，16 位手动重载模式 TMOD = 0b0001xxxx
2. 定时器 T1 中断设置，开启全局中断使能 EA = 1，E_DIS = 0
                     开启定时器 T1 中断使能 ET1 = 1
                                                        */
/********************************************************/

void Timer1_Init(void){

	// 设置定时器的数值
	// 由于定时器 T1 的模式 1 为 16 位手动重载模式，溢出后还需手动开启定时器

	TMOD = TMOD & 0x0F;
	TMOD = TMOD | 0x10;

	EA = 1;
	E_DIS = 0;
	ET1 = 1;

	TH1 = 0x3C;
	TL1 = 0xAF;
}

void Timer1_Setup(void){

    TH1 = 0x3C;
    TL1 = 0xAF;
    TR1 = 1;
}

void main(void){
    P1_Configure();
    // 关闭 4 个 LED 
    P1 = 0x00;
    Timer1_Init();
    Timer1_Setup();
    while(1);
}

void Timer1Interrupt(void) interrupt 3 {
	// 进入中断后需再次配置定时器
    Timer1_Setup();
    	if(cnt >= 10){
			tmp = !tmp;
          	cnt = 0;
	// 定时器溢出周期为 0.05s,频率为 10Hz
	// 增加一级计数，每进入 10 次中断，翻转 LED 灯一次
	// 每隔 0.5s 翻转一次，LED 灯的频率为 1Hz  
            myLED1 = tmp;
    	}else
			cnt++;
}



