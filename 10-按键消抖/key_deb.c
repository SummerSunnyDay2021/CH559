/*
    2021年3月7日 16点48分
    按键消抖

    之前的 V1 版没有消抖，V2 版本采用不精确的 延时函数来消抖，效果非常的差
    这个版本将采用精确的定时器来消抖。
		
		目测消抖效果极佳。。。

**/

#include"CH559.H"

sbit KEYIN0 = P0^0;
sbit LED_0  = P1^4;
sbit TEST		= P1^5;

unsigned char   i = 0;
unsigned char   j = 0;
unsigned char		d0 = 0;
unsigned char		d1 = 0;

// 5000 为每秒中断次数，
unsigned int freq = 255 - 12000000/12/5000;

void Timer1_Init(void){

	// 设置定时器的数值

		TMOD = TMOD & 0x0F;
		TMOD = TMOD | 0x20;

		EA = 1;
		E_DIS = 0;
		ET1 = 1;

		TH1 = (freq & 0xFF00) >> 8  ;
		TL1 = (freq & 0x00FF)       ;
	
		TR1 = 1;
}

void main(void){
    Timer1_Init();
    while(1);

}

void Timer1Interrupt(void) interrupt 3 {

    if(KEYIN0 == 0)
        i = (j<<1)|0x01;
    else
        i = (j<<1)&0xFE;

    if( (i==0xFF)&&(j==0x7F) ){
        d0 = !d0;
		}
		
		d1 = !d1;
		
		TEST  = d1;
		LED_0 = d0;
        
    j = i;
}
