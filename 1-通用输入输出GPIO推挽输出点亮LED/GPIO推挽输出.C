/* 
 * 2020年10月22日 11点17分 星期四
 * 
 * GPIO 推挽输出模式
 * 
 * GPIO 折磨了我很久，一直是块心病，这次将严格按照 CH559.H 库文件的方式使用 GPIO
 * 
 * GPIO 分为多个模式，本次实验使用推挽模式控制三个 LED 小灯。
 */

/* 实验结果 
 * 
 * 使用 GPIO 控制了 三个灯，其中 LED_1 和 LED_3 熄灭
 * LED_2 非常明亮，远比使用上拉电阻的开漏模式明亮
 * 
**/

#include"CH559.H"

sbit LED_1 = P1^4;
sbit LED_2 = P1^5;
sbit LED_3 = P1^6;

void P1_Configure(void){

    // PORT_CFG[3:0] -- 1101
    // P1 为 推挽模式，为 PROT_CFG[1]
    PORT_CFG    = 0x0D;

    // 禁用上拉电阻
    P1_PU       = 0x00;
    // DIR 输出模式
    P1_DIR      = 0xFF;

}

void main(void){
	
	P1_Configure();
	
    LED_1 = 0;
    LED_2 = 1;
    LED_3 = 0;

    while(1);
}