/*
 *  2020年10月16日 21点00分 ☆5
 *  看门狗实验
 * 
 *  看门狗（WatchDog), 功能如下：
 *  在开启看门狗之后，如果没有及时的重载定时器，看门狗就会触发复位，
 *  在程序中加入重载看门狗定时器的语句，一旦程序遇到干扰，跑飞，看门狗没有及时的被重载，系统就会自动复位。
 *  这样系统出现错的时候，看门狗可以及时复位程序，让程序从指定地址重新运行。用以防止芯片控制的系统出现故障。
 * 
 *  在 CH559 内部，看门狗是一个 8 位的定时器，要使用 CH559 的看门狗，需要操作以下寄存器。
 *  
 *  WDOG_COUNT [7:0] 看门狗计数寄存器，计数满 0xFF，溢出 0x00 时自动将看门狗中断标志 bWDOG_IF_T0 置 1
 *  WDOG_COUNT 的计数时钟频率为 Fsys/2622144,
 *  以下是经验值，向 WDOG_COUNT 写入 0x00 ,12M 主频下定时周期约为 5.9s
 *              向 WDOG_COUNT 写入 0x80, 12M 主频下定时周期约为 2.8s
 * 
 *  IE_EX [7:0] 扩展中断使能寄存器
 *  IE_WDOG IE_GPIO N/C IE_UART1 IE_ADC IE_USB IE_TMR3 IE_SPI0
 *      7     6      5    4        3      2      1       0
 *  
 *  IE_WDOG:    看门狗定时器中断使能位，置 1 允许 WDOG 中断，清 0 屏蔽
 *  IE_GPIO:    GPIO 中断使能
 *  IE_UART1:   UART1 中断使能
 *  IE_ADC:     ADC 模数转化中断使能
 *  IE_USB:     USB 中断使能
 *  IE_TMR3:    定时器 3 中断使能
 *  IE_SPI0:    SPI0 中断使能
 * 
 * GLOBAL_CFG [7:0] 全局配置寄存器
 * N/C N/C bBOOT_LOAD bSW_RESET bCODE_WE bDATA_WE bXIR_XSFR bWDOG_EN
 *  7   6       5        4           3        2       1          0
 * 
     * bBOOT_LOAD   ---- BL(BootLoad)状态位，区分 ISP 引导程序状态和应用程序状态，电源上电时置 1，软件复位时清 0
     * bSW_RESET    ---- 软件复位控制位，置 1 可以让程序复位，硬件自动清 0
     * bCODE_WE     ---- Flash-ROM 写保护位，置 1 可以擦写 Flash-ROM，清 0 写保护
     * bDATA_WE     ---- Flash-ROM 的 DataFlash 写保护，置 1 可擦写，清 0 写保护
     * bXIR_XSFR    ---- MOXA _@R0/R1 指令范围访问控制位，清 0 允许访问全部 xdata 区域（xRAM xBUS xSFR)
     *                                                 置 1 专用于访问 xSFR,不能访问 xRAM xBUS
     * bWDOG_EN     ---- 看门狗复位使能，清 0 看门狗用作定时器，置 1 允许看门狗产生复位
 *   
 * 
 * SAFE_MODE [7:0] 安全模式控制寄存器
 * CH559 提供了一个安全模式，一部分关键的操作只能在安全模式下进行，GLOBAL_CFG、PLL_CFG、CLOCK_CFG、SLEEP_CFG、WAKE_CFG.
 * 这些关键的寄存器控制着系统的时钟、休眠、唤醒和复位等等信息的配置。
 * 进入安全模式操作方法如下：
 *  
    // 写入两个密码数值，进入安全模式
    * SAFE_MODE  = 0x55;
    * SAFE_MODE  = 0xAA;
    // 进入安全模式后，会有一个 13 到 23 个时钟周期的时间用于修改 关键SFR
    * GLOBAL_CFG = GLOBAL_CFG & 0x01;   // 使能 看门狗复位
    // 超出有效期，会自动推出安全模式
    // 向该寄存器在写入任意值会提前推出安全模式
    * GLOBAL_CFG = 0x88;
 * 
 * CHIP_ID [7:0] 芯片 ID 识别码，CH558 为 0x58，CH559 为 0x59
 *  
 */
 
/*
 * 2020年10月16日 23点17分 ☆5
 * WatchDog 中断实验结果
 * 忘记打开中断使能 EA，结果弄了老半天也没有进入看门狗中断
 * 目测大约 2、3秒 LED 翻转一次。
 * 
 */

#include"CH559.H"

sbit myLED1 = P1^4;

void P1_Config(void){

    // PORT_CFG[3:0] -- 1101
    // P1 为 推挽模式，为 PROT_CFG[1]
    PORT_CFG    = 0x0D;

    // 禁用上拉电阻
    P1_PU       = 0x00;
    // DIR 输出模式
    P1_DIR      = 0xFF;

}

unsigned char tmp = 0;

void WatchDog_Init(void){
	
		EA = 1;
    IE_EX = 0x80;
    WDOG_COUNT = 0x80;
}

void main(void){
		P1_Config();
    P1 = 0x00;
		WatchDog_Init();
    while(1);
}

void WatchDog_Interrupt(void) interrupt 13 {
    tmp = !tmp;
    myLED1 = tmp;
		WDOG_COUNT = 0x80;
}
