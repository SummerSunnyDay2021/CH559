/*
 * 2020年10月22日 15点20分 ☆4
 * 
 * 高阻输入
 * 
 * 默认点亮推挽 LED，如果输入低电平就翻转一次 LED 输出 
 * 
 */

/* 
 * 实验结果
 * 
 * P0^0 P0^1 P0^2 接入 GND 之后 对应的 LED 熄灭
 * 
*/


#include"CH559.H"

sbit IN_1  = P0^0;
sbit IN_2  = P0^1;
sbit IN_3  = P0^2;

sbit LED_1 = P1^4;
sbit LED_2 = P1^5;
sbit LED_3 = P1^6;

void P1_Configure(void){

    // PORT_CFG[3:0] -- 1101
    // P1 为 推挽模式，为 PROT_CFG[1]
    // xxxx_xxxx & 1111_1101 = xxxx_xx0x
    PORT_CFG    = PORT_CFG & 0xFD;

    // 禁用上拉电阻,PU[7:0] 分别对应 P1^7 - P1^0
    // 每个引脚的上拉可以单独控制
    // 譬如 P1_PU = 0xFE，表示 P1^0 禁用上拉，其他引脚开启
    P1_PU       = 0x00;
    // DIR 选输出模式，同样的每个引脚都可以单独控制
    P1_DIR      = 0xFF;
}

void P0_Configure(void){

    // xxxx_xxxx | 0000_0001 = xxxx_xxx1
    PORT_CFG = PORT_CFG | 0x01;
    // P0 上拉
    P0_PU    = 0xFF;
    // 输出模式
    P0_DIR   = 0x00;
}

void main(void){
	
	P1_Configure();
    P0_Configure();
	
    LED_1 = 1;
    LED_2 = 1;
    LED_3 = 1;

    while(1){
        if(!IN_1)
            LED_1 = 0;
        else
            LED_1 = 1;
        if(!IN_2)
            LED_2 = 0;
        else
            LED_2 = 1;
        if(!IN_3)
            LED_3 = 0;
        else
            LED_3 = 1;
    }
}