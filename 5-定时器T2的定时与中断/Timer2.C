
/* 
   2020年10月26日 16点41分 ☆1
   CH449 的定时器 T2

 * 定时器T0/T1 基本相同:
 * 唯一区别在于定时器 T0 的模式 3，T0 的模式 3
 * 当 T0 处于 模式 3 时，定时器 T0 的高八位和低八位 分别当作两个定时器来使用，
 * 其中 TL0 使用定时器 T0 的寄存器和中断，TH0 占用定时器 T1 的寄存器中中断资源。

 * 而定时器 2 是 16 位自动重载定时器，无需手动重载定时值
 * 
 * TH2 [7:0] 计数器高 8 位
 * TL2 [7:0] 计数器低 8 位
 * 
 * RCAP2H [7:0] 定时/计数 或者 重载 数据高 8 位
 * RCAP2L [7:0] 定时/计数 或者 重载 数据低 8 位
 * 
 * T2CON [7:0] ---- 定时器2 控制寄存器
 *      TF2/CAP1F    EXF2    RCLK    TCLK    EXEN2   TR2    C_T2    CP_RL2
 *            7       6       5       4       3       2       1       0
 * 
 *      TF2:    定时器2溢出中断标识位，从 0xFF 溢出到 0x00 时自动置 1，需要软件清零
 *      CAP1F:  定时器2捕获中断标识位，同上 TF2
 * 
 *      EXF2:   外部触发，这里不管，哼(￢︿̫̿￢☆)
 *      
 *      RCLK:   UART0 RX 接收端的时钟选择，置 0 使用 Timer1，置 1 则为 Timer2 
 *      TCLK:   UART0 TX 发送端的时钟选择，同上
 * 
 *      EXEN2:  T2EX 引脚(P2.5)触发使能，置 1 则在 T2EX 有效边沿触发定时器重载或者捕获，置 0 忽略
 *      
 *      TR2:    定时器2 启动/停止位，置 1 启动，置 0 关闭
 *      
 *      C_T2:   定时/脉冲计数2功能选择，置 0 内部时钟定时，置 1 使用外部引脚 T2(P1.0) 下降沿计数
 *      CP_RL2: 定时/脉冲计数 和 捕获 功能选择，置 0 使用定时/脉冲计数功能，置 1 使用捕获功能
 * 
 * T2MOD [7:0] ---- 定时器2 方式寄存器
 *      bTMR_CLK    bT2_CLK    bT1_CLK    bT0_CLK    bT2_CAP_M1    bT2_CAP_M0    T2OE    bT2_CAP1_EN 
 *          7           6          5        4              3            2          1          0
 *      
 *      bTMR_CLK:   已选择快速时钟的 T0/T1/T2 定时器的最快时钟使能，置 1 使用 Fsys，置 0 使用分频率时钟，本位的选择不影响选择标准时钟的定时器
 *      
 *      bT2_CLK:    T2 内部时钟选择，0 为标准时钟 Fsys/12，1 为快速时钟 
        *                  定时/计数模式下：Fsys/4(bTMR_CLK==0) 或 Fsys(bTMR_CLK==1)
        *                  UART0 时钟模式下：Fsys/2(bTMR_CLK==0) 或 Fsys(bTMR_CLK==1)
        * 
        
 *      bT1_CLK:    !!!注意，这是 定时器 T1 定时器 T1 定时器 T1 的时钟控制位，不是 定时器 T2 的 
 *      bT0_CLK:    !!!注意，这是 定时器 T0 定时器 T0 定时器 T0 的时钟控制位，不是 定时器 T2 的
                    0 为标准时钟，Fsys/12
                    1 为快速时钟，Fsys/4(bTMR_CLK==0) 或 Fsys(bTMR_CLK==1)

 *      bT2_CAP_M1: 捕捉方式高位 
 *      bT2_CAP_M0: 捕捉方式低位
                    x0:  下降沿到下降沿
                    01： 任意沿到任意沿，即电平变化
                    11： 上升沿到上升沿

 *      T2OE:       T2 时钟输出使能位，0 禁止输出，1 在 T2 引脚输出，频率为  T2 溢出频率的一半

 *      bT2_CAP1_EN:    捕捉模式使能位，当 RCLK==0、TCLK==0、CP_RL2==1、C_T2==0、T2OE==0 时使能捕获模，捕获 T2(P1.0) 有效边沿
 *
 * 
 * 		中断使能寄存器 IE [7:0]

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
 *  
 *      CH559 的中断源相当多，这里仅仅使用 INT0 外部中断一个，下面的是中断号表
	
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
 *
**/

#include"CH559.H"

sbit LED_1 = P1^4;
unsigned char tmp = 0;

// 50 为每秒中断次数，范围在 16 - 1000_000 之间
unsigned int freq = 65535 - 12000000/12/50;

// 定时器中断频率为 50Hz,每个中断间隔 20ms,肉眼看不见效果
// 增加一级计数，每隔 25 个中断触发一次，翻转 LED 灯
// LED 的翻转周期为 500ms,频率为 1Hz 
unsigned int i = 0;

void Timer2_Init(void){

    EA = 1;
    E_DIS = 0;
    ET2 = 1;
    
    // 定时器 T2 拥有更快的时钟
    // 开启快速时钟模式，计数器频率是标准模式下的 3 倍，而最快时钟模式下频率可达标准模式的 12 倍
    T2MOD = 0x00;

    // 定时器 T2 超时周期计数器
    TH2     =   (freq & 0xFF00) >> 8  ;
    TL2     =   (freq & 0x00FF)       ;
    // 定时器 T2 自动重载周期计数器
    RCAP2H  =   (freq & 0xFF00) >> 8  ;
    RCAP2L  =   (freq & 0x00FF)       ;

    TR2 = 1;
}

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

void main(void){
    P1_Configure();
    Timer2_Init();
    P1 = 0x00;
    while(1);
}

void Timer2_Interrupt(void) interrupt 5{
    // CH559 的定时器中断不会清除定时器超时标识位
    // TF2 需要手动清零，否则定时器中断会一直触发
    TF2 = 0;
    // 0 - 24 --> 25
    if(i>=24){
        i = 0;
        tmp = !tmp;
        LED_1 = tmp;
    }else 
        i++;

}

