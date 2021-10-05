/*
 *  2020年11月1日 14点39分 ☆7
 *  
 *  使用看门狗复位，串口调了好久。。。😔😔😔
 *  
 *  
 *  
**/

#include"CH559.H"
#include<intrins.h>

// 这是一个计算公式，但其中的除以 16 是什么原因我还没弄明白 🤷‍
// 65535 = 0xFFFF 是 16 位计数器的最大计数值
// 12000000 = 12M 是晶振频率
// 9200 是我设置的 波特率(bps)
// 为什么要除以➗16呢？？？
unsigned int code baud = 65535 - 12000000/4/16/1200;

void delay(unsigned int t){
	unsigned int i = 0;
	unsigned int j = 0;
	for(i=0;i<t;i++){
		for(j=0;j<0x0FFF;j++)
			_nop_();
	}
}

void timer2_config(void){

	TH2       = (baud & 0xFF00) >> 8;
	TL2       = baud & 0x00FF;
	RCAP2H    = (baud & 0xFF00) >> 8;
	RCAP2L    = baud & 0x00FF;

	RCLK      = 1;   // 选择 Timer2 产生时钟
	TCLK      = 1;

	C_T2      = 0;  // 使用内部时钟
	CP_RL2    = 0;  // 用作定时而非计数

	TR2 = 1;        // 启动定时器 T2

//  bTMR_CLK = 0;
//  bT2_CLK  = 0;

}

void uart0_config(void){
	SM0 = 0;
	SM1 = 1;
	SM2 = 1;

	REN = 1;
}

void interrupt_config(void){

	EA = 1;
	E_DIS = 0;
	ES = 1;

}

void watchdog_config(void){

    WDOG_COUNT = 0x00;

    SAFE_MOD  = 0x55;
    SAFE_MOD  = 0xAA;
    GLOBAL_CFG = 0x01;
    SAFE_MOD  = 0xFE;

}

unsigned char i = 0;
unsigned char dat = 0x01;
unsigned char send_enable = 0;

void main(void){

	interrupt_config();
	uart0_config();
	timer2_config();
	watchdog_config();
	delay(500);
	while(1){
		send_enable = 1;
		TI = 1;
		delay(20);
		dat++;
	};

}

void uart0_interrupt(void) interrupt 4 {

	if(TI){
		if(send_enable){
			SBUF = dat;
			send_enable = 0;
		}
		TI = 0;
	}
  
	if(RI){
    	RI = 0;
	}

}
