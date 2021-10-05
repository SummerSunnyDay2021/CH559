/*
 *  2020年10月29日 16点24分 ☆4
 * 
 *  串口通讯实验2 —— 使用定时器 2 产生串口时钟
 *  
 *  我首先试验过串口模式 0，几乎都已失败告终，由于没有例程，也无法检查出错误的根源。
 *  使用定时器 T1 驱动串口时发现计数周期短了，几乎无法生成低波特率的时钟。
 *  而定时器 T2 是一个自动重载的定时器，非常适合用作 串口Uart0 的时钟。
 * 
 * 
 *  实验结果，20点01分 终于折腾完了，
 *  虽然距离实用的程序还有些差距，但是基本实现了字符串发送问题。😫 好累啊。。。
 *  1200bps 下没有问题，其他波特率没有测试。。。
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

unsigned char i = 0;
unsigned char txd_dat[64] = "ABCDEFG,HIJKLMN,OPQRST,UVWXYZ\n";
unsigned char send_enable = 0;
void main(void){

  interrupt_config();
  uart0_config();
  timer2_config();
  send_enable = 1;
  TI = 1;
  while(1);

}

void uart0_interrupt(void) interrupt 4 {

	if(TI){
    if(send_enable){
      if(txd_dat[i]!=NULL){
        SBUF = txd_dat[i];
        i++;
      }else{
        i = 0;
        send_enable = 0;
      }

    }
		  TI = 0;
  }
  
  if(RI){
    RI = 0;
  }

}
