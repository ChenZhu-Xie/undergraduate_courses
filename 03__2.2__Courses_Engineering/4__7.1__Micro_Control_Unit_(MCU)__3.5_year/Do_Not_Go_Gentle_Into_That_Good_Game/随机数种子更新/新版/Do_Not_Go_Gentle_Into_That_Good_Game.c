//-----------------------------------------------------------------------------//原则上，新建的project和.c文件的名字中均不能出现空格，需要用下划线代替之（不过中文是允许的）；特别是.c文件必须这么命名，否则编译时总是target not created；而新建project时，命名中其实可以存在空格，虽然仍然编译不过，但只要将Options for Target1——Output——Name of Executable中，空格用“_”下划线代替即可，重新编译就通过了，主要是project中有几个文件不能允许空格的存在，其他的可以。这一点keil做得不够好，写程序时多加一对双引号的事情。
// Do_Not_Go_Gentle_Into_That_Good_Game.c
//-----------------------------------------------------------------------------//STARTUP.A51不是必需的，可从group中删除。
// AUTH: 谢尘竹
// DATE: 2019/10/22
// 程序说明：4阶10维解密游戏
//-----------------------------------------------------------------------------//压缩的时候要用第三个——“添加到压缩文件(Bandzip)”，而不是“添加到“...””，否则即使创建了文件夹，解压后也是散装的，将没有那个文件夹。
// Includes
//-----------------------------------------------------------------------------
#define uchar unsigned char  //宏定义，将uchar认为是unsigned char
#include<c8051f350.h>        // SFR declarations
#include<stdio.h>            //引用标准输入输出函数库，如printf、getchar等，需要用到printf()。
#include<stdlib.h>           //引用系统函数库，里面有指针相关函数，我需要动态分配内存区地址数量or空间，即要用到malloc()和realloc()；同时也要用到里面的rand() 、srand()函数来生成随机数——在c++的头文件cstdlib.h中也有这个rand()，但好像c中只有stdlib.h中有它俩。
//-----------------------------------------------------------------------------
// Global CONSTANTS
//-----------------------------------------------------------------------------
sbit LED20 = P2^0;           // LED='1' means ON. //=0, 熄灭! //=1, 点亮!
sbit Pin01 = P0^1;           // 端口P0.1
uchar code TAB[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00};
//数码管为共阴极      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   A,   B,   C,   D,   E,   F,   不显示  
uchar code P0fake[]={7,6,3,2,1,0};       //懒的搞用白老师的cTemp，不如直接P0fake[0]~P0fake[5]来调用从左到右的6个P0口P0.7、P0.6、P0.3、P0.2、P0.1、P0.0.
uchar code zuoxiu[6][28]={{3,2,6,5,0,1,6,4},{3,3,2,6,6,5,0,0,1,6,6,4},{3,3,3,2,6,6,6,5,0,0,0,1,6,6,6,4},{3,3,3,3,2,6,6,6,6,5,0,0,0,0,1,6,6,6,6,4},{3,3,3,3,3,2,6,6,6,6,6,5,0,0,0,0,0,1,6,6,6,6,6,4},{3,3,3,3,3,3,2,6,6,6,6,6,6,5,0,0,0,0,0,0,1,6,6,6,6,6,6,4}};
uchar code xiuzuo[6][28]={{0,0,0,0,0,0,0,0},{0,1,1,1,0,0,0,1,1,1,0,0},{0,1,2,2,2,1,0,0,0,1,2,2,2,1,0,0},{0,1,2,3,3,3,2,1,0,0,0,1,2,3,3,3,2,1,0,0},{0,1,2,3,4,4,4,3,2,1,0,0,0,1,2,3,4,4,4,3,2,1,0,0},{0,1,2,3,4,5,5,5,4,3,2,1,0,0,0,1,2,3,4,5,5,5,4,3,2,1,0,0}};
long WaitSetting1=9000,WaitCounter;    //理论上的1s1次，但与程序有关
int WaitStorer=0;
uchar xdata liehang[66][6];  //uchar xdata ** xdata liehang;xdata类型数据无法作为局部变量放进main里面，只能作为全局变量；所以无法为它节约空间。——第三个数即使是500也不会报错，但即使你写255，程序也不会正常运行，但编译不会报错，没有超出xdata的大小，且程序能运行，但就是不会正常运行。——根据调试，liehang最多只能是liehang[65][6]，一旦counthang变成了66，则counthang会变成10（也不知道是为什么要变成10）。
//-----------------------------------------------------------------------------
// Function PROTOTYPES
//-----------------------------------------------------------------------------
void Timer_Init()
{
    TMOD      = 0x21;        //不是0x01而是0x21：20:定时器1作为波特率发生器,8位自动重装；01:定时器0作为16位时钟。
    TCON      = 0x40;
    //CKCON     = 0x01;      //无->9600      0x01->19200
    TH1       = 0x96;        //0x96->9600   0x61->19200
    TI0       = 1;
    //19200BPS          
    //CKCON     = 0x08;
    //TH1       = 0xB1;
}

void UART_Init()    
{
   SCON0 = 0x10;             //SCON0: 8-bit variable bit rate
}

void Port_IO_Init()
{
    // P0.0  -  Skipped,     Push-Pull,  Digital
    // P0.1  -  Skipped,     Push-Pull,  Digital
    // P0.2  -  Skipped,     Push-Pull,  Digital
    // P0.3  -  Skipped,     Push-Pull,  Digital
    // P0.4  -  TX0 (UART0), Open-Drain, Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  Skipped,     Push-Pull,  Digital
    // P0.7  -  Skipped,     Push-Pull,  Digital

    // P1.0  -  Unassigned,  Open-Drain, Digital
    // P1.1  -  Unassigned,  Open-Drain, Digital
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Unassigned,  Open-Drain, Analog
    // P1.7  -  Unassigned,  Open-Drain, Analog

    P0MDOUT   = 0x30;        //P0.4, 0.5引脚设为推挽方式.
    P1MDOUT   = 0xFF;        //全部引脚设为推挽方式.
//    P1MDIN    = 0x3F;
//    P0MDOUT   = 0xCF;
    P2MDOUT   = 0x01;
    P0SKIP    = 0xCF;
    XBR0      = 0x01;
    XBR1      = 0x40;
}

void Oscillator_Init()
{
    OSCICN    = 0x83;
    RSTSRC    = 0x04;        // enable missing clock detector
}

// Initialization function for device,
// Call Init_Device() from your main program
void Init_Device()
{
   Oscillator_Init();        // Initialize system clock
   Port_IO_Init();           // Initialize crossbar and GPIO
   Timer_Init();
   UART_Init();
}//-----------------------------------------------------------------------------
// Other Functions
//-----------------------------------------------------------------------------
void delayms(int time)   {
unsigned char i, j;

  while (time>1)  {
     for(i=0; i<218; i++)   for(j=0; j<20; j++);
     time--;
  }
}
//-----------------------------------------------------------------------------
// MAIN Routine
//-----------------------------------------------------------------------------
//"Warning：Here comes the problem !!!"——
//
//                \************* 《4阶10维解密游戏》_v1.03_beta *************/
//
// "In order to get the right code, you have to test your wrong codes."
//
//                                                             —— Coded By xcz. (2019/10/22)
//
//                          \--------------游戏规则--------------/
//
//  1.4阶10维玩法：0~9共10个数中，随机4位数密码（两两不同）已生成，请输入4个的数字（两两不同）试探其正确性。
//  2.输入完毕不用按回车键，每键入n个数字，程序均会自动与the standard code比较并提示。
//  3.第5、6位数分别提示你有多少个数字位置错误但数值是n位密码之一、数值和位置均正确（完全正确）。
//  4.错了则继续输入新的一组数字，直到正确，看看你的逻辑帮你到达真理需要多少步，以及花费多少时间。
//  5.游戏胜利后：前2个数显示你用了多少步、接着3个数显示你用了多少时间（/s），最后一位是系统为你的综合表现评定的等级，分为579abcdef和无等级。
//  
//                           \--------------功能键--------------/         ——“乱按键盘是允许的，但程序是金刚不坏的。”
//                                                                             
//  a.WS键（大写）：翻页查看历史记录。 
//  b.退格键：删除你输入的上一个数字，以便重新输入该数字。
//  c.ESC键：退出，即结束程序；任何时候都可以退出。
//  d.猜对密码后：输入0~9中任意数字键重玩4阶的游戏，也可按WS键查看历史记录。
//  e.K键：激活/关闭作弊。
//
//       \---You can hardly get through this door, before you really understand sth.---/
//
//-----------------------------------------------------------------------------
// Codes
//-----------------------------------------------------------------------------code上限大概在6140~6142之间。
//————————————————————————————————————————————————————————————————————————————————————Generated by xcz.
void main()
{
   uchar key,counthang,lieshu[4],i,countlie,fakehang,tag,locationcorrect,numbercorrect,j,randnum,TAGG,timer,length=4,shift=0,start=0,finish=0,k,counthang65,seconds,steps,TIMER=0;//350.h里好像已经定义了TAG了，要用TAGG，倒是LAB还没用...虽然LAB不是个变量。——这个是可以定义在main里的，之前之所以不行，是因为定义在了赋值语句后面，比如它就不能放在诸如RI0=0后面。——毕竟要是局部变量无法定义，那就真的完蛋了。——之前变量互相赋值，就是因为全局变量太占空间了（定义为局部变量后，lieshu[j]还真就不给counthang赋值了），所以要定义为局部变量，我tm终于找对了定义变量应该放的位置了。
   P0=0;
   P1=0;
   RI0=0;
// TI0=1;                                   //事实证明TI在之前=0之后，以后就不要去动他了；之前已经令它=1了；它与RI不冲突。
//————————————————————————————————————————————————————————————————————————————————————disable watchdog timer禁用狗	
   // disable watchdog timer
   PCA0MD &= ~0x40;                         // WDTE = 0 (clear watchdog timer) 
                                            // WDTE = 1 (enable)
   Init_Device();
//---------------------作秀（灯光表演、炫酷开场！）
   P0=0xff;                                 //0xff=111111
   P1=1<<0;

// P0=1<<P0fake[0];                         //2.也不能写成P0=(1<<P0fake[0])|(1<<P0fake[5]);？——不不不，应该是Exceeded Code Space.的缘故，即代码量过大。        
// P0=P0|(1<<P0fake[5]);        
// delayms(200);
// P0=P0|(1<<P0fake[1]);                    //1.不能写成3项连位或P0|(1<<P0fake[1])|(1<<P0fake[4])——不不不，应该是Exceeded Code Space.的缘故，即代码量过大。
// P0=P0|(1<<P0fake[4]);
// delayms(100);
// P0=P0|(1<<P0fake[2]);
// P0=P0|(1<<P0fake[3]);
   delayms(200);

   P1=P1|(1<<1);
   P1=P1|(1<<5);
   delayms(100);
   P1=P1|(1<<6);
   delayms(100);
   P1=P1|(1<<2);
   P1=P1|(1<<4);
   delayms(100);
   P1=P1|(1<<3);
   delayms(100);
   P1=P1|(1<<7);
   delayms(100);
 
   P1=P1^(1<<0);
   delayms(100);
   P1=P1^(1<<1);
   P1=P1^(1<<5);
   delayms(100);
   P1=P1^(1<<6);
   delayms(100);
   P1=P1^(1<<2);
   P1=P1^(1<<4);
   delayms(100);

   P0=P0^(1<<P0fake[2]);
   P0=P0^(1<<P0fake[3]);
   delayms(100);
   P0=P0^(1<<P0fake[1]);
   P0=P0^(1<<P0fake[4]);
   delayms(100);
// P0=P0^(1<<P0fake[0]);
// P0=P0^(1<<P0fake[5]);
//   delayms(100);                          //一方面为了对称，刚开始上面就有两条杠，而结束时，下面也应有两条杠；另一方面，如果加了这句话，则code=6143，超出范围。delayms里面的数值不大增加code，反而是delayms的出现次数，增加code增加的多（所以最好用for循环里加delayms？）。
//---------------------作秀1
   P0=1<<P0fake[0];
   for(i=0;i<6;i++)   
   {
      P1=TAB[10+i]|(1<<7);      
      P0=P0|(1<<P0fake[i]);  
      delayms(100);                                
   }
   for(i=6;i>0;i--)                         //&(按位与)、|(按位或)、^(按位异或)、~ (按位取反) ，按位是指以二进制的方式，对每一位进行01变换；&& 逻辑与运算符； || 逻辑或运算符；非！
   {                                        
      P0=P0^(1<<P0fake[i-1]);  
      delayms(100);                                
   }
//---------------------作秀2
   for(j=0;j<6;j++)
   {
      for(i=0;i<8+4*j;i++)   
      {
         if(j<3)                            //不让第一、二、三次显示有两条尾巴
         {
            P1=1<<zuoxiu[j][i];             //1<<7=0x80=10000000;1<<0=0x01=00000001
//          if(zuoxiu[j][i]==j)
//             P1=P1|0x80;  
            P0=1<<P0fake[xiuzuo[j][i]];
         }
         else
         {
            if(i==0)                        //i=0时，不进行比较。
            {
               P1=1<<zuoxiu[j][i];
//             if(zuoxiu[j][i]==j)
//                P1=P1|0x80;  
               P0=1<<P0fake[xiuzuo[j][i]];
            }
            else
            {
               if(xiuzuo[j][i]==xiuzuo[j][i-1])
               {
                  P1=1<<zuoxiu[j][i];
//                  P1=P1|(1<<7);                             //working
//                  SBUF0=zuoxiu[j][i]+48;                    //working
//                  SBUF0=zuoxiu[j][i-1]+48;                  //working
//                  P1=1<<zuoxiu[j][i-1];                     //working
//                  P1=P1|(1<<zuoxiu[j][i-1]);                //not working——我知道为啥了,...之前对TAB、P0fake、zuoxiu、xiuzuo的数组的定义都是unsigned code，这tm是int！而不是char！！这数据很大！，可能就是因为数组太大占用空间而互相赋值了...；或者是因为：由于int不能左右移。——unsigned code不是unsigned char code即uchar code，而在左右移的时候，只能对16进制的变量左右移，而uchar就是16进制的。
                  P0=1<<P0fake[xiuzuo[j][i]];
               }
               else
               {
                  P1=1<<zuoxiu[j][i];
                  P0=1<<P0fake[xiuzuo[j][i]];
//                  P0=P0|(1<<7);                             //working
//                  P0=P0|(1<<P0fake[0]);                     //working
//                  SBUF0=xiuzuo[j][i]+48;                    //working
//                  SBUF0=xiuzuo[j][i-1]+48;                  //working
//                  SBUF0=P0fake[xiuzuo[j][i]]+48;            //working
//                  SBUF0=P0fake[xiuzuo[j][i-1]]+48;          //not working
                  P0=P0|(1<<P0fake[xiuzuo[j][i-1]]);
               }
            }
         }
//       switch(j)
//       {
//          case 0:                         //00000000
//                                          //P1111111——pattern与j无关。——为什么这种显示方式最多只能显示一条两格长的尾巴？一条三格的不能？哈哈，你得动脑子想：P1、P0口均是可以同时显示多个字段or给多个高电平的，但无论如何，即使P1P0都在进行并行显示，不同P0口的两组字段的组合仍然是一样的，因为它们同时都受到同一个P1口的控制。——这反过来说明，我之后采用视觉暂留的办法，以显示不同的数字，方法是对的，且是唯一的办法。

//          case 1:                         //011100011100
//                                          //P01101101101

//          case 2:                         //0122210001222100
//                                          //P001100110011001

//          case 3:                         //01233321000123332100
//          case 4:                         //012344432100012344432100
//          case 5:                         //0123455543210001234555432100 
//       }        
         delayms(20);
      }
   }   						
//---------------------作秀3                //code=6133左右应该是代码量的上限，反正6133是可以的，6155是不行的。
   P1=1<<3;
   for(i=0;i<6;i++)   
   {
      P0=1<<P0fake[i];  
      delayms(20);                                
   }
   P0=1<<P0fake[5];
   for(i=2;i>0;i--)   
   {
      P1=1<<i;  
      delayms(20);
   }						
   P1=1<<0;
   for(i=6;i>=1;i--)                        //这个比较特殊，它不能写作i=5;i>=0（i>-1更不行）;i--，因为i是unsigned char，不能为负数，否则从0→-1会变到255去。 
   {
      P0=1<<P0fake[i-1];  
      delayms(20);
   }
   P0=1<<P0fake[0];
   for(i=5;i>3;i--)   
   {
      P1=1<<i;  
      delayms(20);
   }
//---------------------作秀4                                          
// printf("PP\n");                          //让程序先吃一个P再说
// init_mempool(0x1000,100);
//————————————————————————————————————————————————————————————————————————————————————disable watchdog timer禁用狗
Restart:
//————————————————————————————————————————————————————————————————————————————————————第一个while：新游戏
//// SBUF0=48;
//// SBUF0=49;                              //不知道为什么这个无法显示，四个杠表示再也不启用此注释
   while(1)                                 //重新开始玩新的一局游戏！
	{
      timer=0;                                       //计时器归零
      TAGG=0;                                        //若不加这一行，则上一局的最后4位数输入完毕后，进入下一局时，TAGG还保持着=1，因为默认是输入错误并且另起炉灶新的一行。
		counthang=0;                                   //初始化行数（不是简单地重置行序号为0，而是清空所有行，并取行序号为0。——这体现在*传送门1*那里）
		counthang65=0;
//====================
      if(TIMER!=0)                                   //取双随机数也没有用，必须要进入某个循环来counter++，以取运行时间，且只有第一次运行到这里才不进入这个循环，否则进入这个循环；
      {

      srand((unsigned)WaitCounter);                  //每次进入该循环，都以当前距离1970年1月1日0时0分0秒，过的秒数，播种种子序列。——注：time(0)与time(null)若在同一时刻调用，返回的值一样【比较time(NULL)和time(0),没有多大意义】；然后将其强转为unsigned  int类型，其取值范围缩减为了0~length+25535，再作为参数给srand()，它的参数需要是unsigned int类型。
      for(j=0;j<length;j++)                          //我需要在0~9间，随机找length个不重复的数字
		{
			while(1)                                             //需要随机到与储存的随机数字不同的数字，才给我出来
			{
				randnum=rand()%10;                                         //获取一个0~9之间的随机数，临时储存下来。——rand()：根据紧邻的上一个srand()的函数值，在0~327length+27之间返回随机数
//          printf("randnum=%bd | ",randnum);                          //调试用；亦或是作弊按钮。——注意第一次用print会丢失掉一个字符，所以要print两个P作为分隔符。
				for(i=0;i<j;i++)                                           //查看randnum是否与0~j-1对应的，list[i]中前j-1个数中，任何一个相同。
				   if(lieshu[i]==randnum)
                  break;                                                                 //若有重复的，则提前break掉for循环，并且不满足j==i条件，而重新进入while(1)的开头，继续随机，直到随机到一个与之前存储的没有重复的。
				if(i==j)                                                   //若能进入该if，则i会加到j（j的最大值是3），且该randnum与list[i]中i=j-1及其之前的数字都不重复的，因此将其保存到最新的list[j]中。
			   {
				   lieshu[j]=randnum;                                               //将length个0~9的、且每一个都与前面所有的不同的，随机数，存入lieshu[0]、lieshu[1]、lieshu[2]、lieshu[3]...中。
////           TI0=1;
//             printf("lieshu[%bd]=%bd\n",j,lieshu[j]);                         //调试用；亦或是作弊按钮。
////           TI0=0;                                                           //不要加这行，准确地说，“TI0=1+(printf的循环)”的模式，不能写为“(TI0=1+printf+TI0=0)的循环”的模式！
////           SBUF0=randnum+48;                                                //不知道为什么，在这里SBUFO无法输出任何东西，下面倒可以。
					break;                                                           //这个break是break掉while，开始执行for循环中下一个j值的while，直到下一个不重复的随机数被找出来。
				}
			}   
//         printf("\ncounthang=%bd | ",counthang);
		}
//		counthang=0;                                   //我是真的服，赋给lieshu[j]的值，同时会自动赋值给counthang...？

      }
//====================
//————————————————————————————————————————————————————————————————————————————————————第二个while：新行
		while(1)								        //重新猜，猜错了继续猜！
		{
			countlie=0;                                          //重置列序号为0（不是初始化列数）
			if(counthang>=65)
         {
            counthang=0;
            counthang65++;
            TAGG=0;                                           //coungthang=0后不能再让fakehang=counthang后再fakehang--了，否则会fakehang=-1、并且一直往上-2、-3...并且此时输完了第65行，就直接显示counthang的6位数，而不是fake行的6位数。
         }
         counthang++;                                         //行序号+1         
//       printf("\ncounthang=%bd | ",counthang);
//			liehang=(uchar data **)realloc(liehang,counthang*sizeof(uchar data *));//在此行序号对应的行处，开辟1个新行。（*传送门1*）——这里用malloc不知道为什么不行，按理说malloc也不会改变以前已经分配了的地址的值，因为我的其他程序中它就没有问题；我是真的服，同样的含义，realloc就不存在这个问题。——按理说直接写liehang=(int**)malloc(counthang*sizeof(int*))也可，只不过相对于realloc最后需要释放指针罢了，不过程序运行结束会自动释放指针，所以这方面用malloc代码长度也没有增加，甚至比realloc更少：它不需要在前面定义变量时引入malloc，只需要写个**liehang定义一个数组就行。
//			liehang[counthang-1]=(uchar data *)malloc((length+2)*sizeof(uchar));//每一个行开辟length+2个列，拿来存整数：六列数字对应六个数码管，length+2个列表示在时间流中的一行行length+2个数字的一维数组。——这个不涉及到以前分配的地址，所以不管malloc对以前的地址的值有没有初始化，这句话都可以不修改。
			for(i=0;i<length+2;i++)
			{
//          printf("\nlength=%bd",length);
            liehang[counthang-1][i]=10;                                //初始化每个尚且还没有数字的空格的数值为10，用以之后（*传送门2*）判断这里“有没有这个数字”：即若对应单元格的数值等于10，则肯定没有输入数字或者没有给出对错判断(最后两位数)，若没有则不显示这个数字。
//          printf("\nliehang[%bd][%bd]=%bd",counthang-1,i,liehang[counthang-1][i]);								
         }
//       printf("\ncounthang=%bd | ",counthang);         
//————————————————————————————————————————————————————————————————————————————————————第三个while：新列
			while(1)                                             //开始捕获用户输入，直到输入length个数字（输入完毕后不用按enter，程序自动对比randnum，判断是否正确，不正确则break，返回上一个while，新建一个行，重新猜）
			{
				tag=0;                                                     //每输入一个数字，都要将下一个数字的tag设为0，即默认下一次键入的按键不是数字，而是功能键、功能键中的上下键or字母；只有当按了数字，才能让tag=1，并因此不执行goto lab重新键入，而因输入了有效按键而来到这里又使tag=0，以期输入下一个数字按键。
//				printf("countlie=%bd | ",countlie);
            countlie++;                                                //列序号+1
//				printf("countlie=%bd | ",countlie);
            if(countlie>length)				                             //如果countlie>length了，则已经输入完了，则进入if的另一个分支，与lieshu[0]~lieshu[length-1]进行对比并break出当前所在的while循环。——我直接采用了输入length个数字就自动判断的游戏模式，无需按enter，加快游戏节奏+懒人福音+提高游戏难度。
				{                                                                                                                     
					locationcorrect=0;				                                   //挨个比较输入的length个数与所生成的length个随机数在位置和正确性上均匹配的数的个数。
					for(i=0;i<length;i++)                                    
					{
						if(liehang[counthang-1][i]==lieshu[i])
							locationcorrect++;
					}
					liehang[counthang-1][length+1]=locationcorrect;
					numbercorrect=0;				                                      //挨个比较输入的length个数与所生成的length个随机数，这两个集合的交集个数。
					for(j=0;j<length;j++)
					{
						for(i=0;i<length;i++)
						{
							if(liehang[counthang-1][i]==lieshu[j])                                       //实际上如果在集合1中找到了一个与集合2相同的数，则接下来对集合1的遍历（j）照常往下取数进行，但对集合2（j）的遍历，就不需要再与相同j值的lieshu[j]比较了。——但这样写比较简单，直接0~3的j++；不然还要加条件判断以跳过相应的j和lieshu[j]，麻烦。——由于两个集合中，每个集合中的元素都是互不相同的，所以不存在i往后去还会使得numbercorrect++的可能。
							{
								numbercorrect++;
								break;                                                                             //为了减小计算机资源占用量，直接break掉第一层for循环，直接第二层for循环中j++。
							}
						}
					}
					liehang[counthang-1][length]=numbercorrect;
               if(locationcorrect==length)                                      //the final test——检验你娃娃输入的最终结果是否正确！——如果正确了，那么它会再flash一次（加上这次总共闪了2次了），加入一行新的行，告诉你，加上最后一步你用了多少步才得到正确结果。并且暂时保留历史记录，以便你可以回溯你过去都干过什么傻事= =，按enter键继续下一场新的解谜/密游戏。
					{
						finish=WaitStorer;                                                     //一旦length个数字都正确，就马上记录下时间
//---------------------作秀
   P1=1<<0;
   for(i=0;i<6;i++)   
   {
      P0=1<<P0fake[i];  
      delayms(20);                                
   }
   P0=1<<P0fake[5];
   for(i=1;i<3;i++)   
   {
      P1=1<<i;  
      delayms(20);
   }
   P1=1<<3;
   for(i=6;i>=1;i--)  //这个比较特殊，它不能写作i=5;i>=0（i>-1更不行）;i--，因为i是unsigned char，不能为负数，否则从0→-1会变到255去。 
   {
      P0=1<<P0fake[i-1];  
      delayms(20);
   }
   P0=1<<P0fake[0];
   for(i=4;i<6;i++)   
   {
      P1=1<<i;  
      delayms(20);
   }						
//---------------------作秀
                  counthang++;                                                           //沿用第二层while的初始语句，但不需要重置列序号为0，因为游戏已经结束了。
//						liehang=(uchar data **)realloc(liehang,counthang*sizeof(uchar data *));
//						liehang[counthang-1]=(uchar data *)malloc((length+2)*sizeof(uchar));
						steps=65*counthang65+counthang-1;
                  seconds=finish-start;
                  liehang[counthang-1][0]=steps/10%10;                                   //要用counthang-1，因为这多生成的一行是拿来显示信息的，而不是实际的失败次数；第一位改成counthang-1的十位，它可以是0
						liehang[counthang-1][1]=steps/1%10;                                    //第二位改成counthang-1的个位——我不关心数码管的第三位，即你如果100步以内都没有猜出来...这...我们就只显示你所用的十位数和个位数吧...
						liehang[counthang-1][2]=seconds/100%10;                                //第三位改成秒数的百位
						liehang[counthang-1][3]=seconds/10%10;                                 //第四位改成秒数的十位
						liehang[counthang-1][4]=seconds/1%10;                                  //第五位改成秒数的个位——不关心秒数的千位，即不关心数码管的最后一位，龟速了都，没有意义。
                  liehang[counthang-1][5]=16;                                            //20步之外，或，960s以外----class5__Stupid (or busy playing other games?)——为了节省代码空间，留出一点来用于delayms，我把else后面的东西放在这里了，节约了大概16的code。

                  if(steps<=2&&seconds<=4)                                               //2步之内，4s以内
						   liehang[counthang-1][5]=5;
//                else if(steps<=3&&seconds<=8)                                          //3步之内，8s以内
//                   liehang[counthang-1][5]=6;
                  else if(steps<=4&&seconds<=12)                                         //4步之内，12s以内----class1__Top
                     liehang[counthang-1][5]=7;
//                else if(steps<=6&&seconds<=18)                                         //6步之内，18s以内
//                   liehang[counthang-1][5]=8;
                  else if(steps<=8&&seconds<=24)                                         //8步之内，24s以内
                     liehang[counthang-1][5]=9;
                  else if((steps)<=10&&seconds<=30)                                      //10步之内，30s以内----class2__Excellent
                     liehang[counthang-1][5]=10;
                  else if((steps)<=10&&seconds<=60)                                      //10步之内，60s以内
                     liehang[counthang-1][5]=11;
                  else if((steps)<=10&&seconds<=120)                                     //10步之内，120s以内
                     liehang[counthang-1][5]=12;
                  else if((steps)<=10&&seconds<=240)                                     //10步之内，240s以内
                     liehang[counthang-1][5]=13;
                  else if((steps)<=10&&seconds<=480)                                     //10步之内，480s以内----class3__Good
                     liehang[counthang-1][5]=14;
                  else if((steps)<=20&&seconds<=960)                                     //20步之内，960s以内----class4__Just-so-so
                     liehang[counthang-1][5]=15;
//————————————————————————————————————————————————————————————————————————————————————已经加入查看历史记录、退出、重开一局机制
						fakehang=counthang;
LAB:                  
                  while(RI0==0)
                  {
                     WaitCounter++;
                     if(WaitCounter<WaitSetting1/2)                                               //猜密码成功后不需要计时；但好像计时也无所谓？不行，finish会出问题？哦也不会，finish会一直增大，反正它是int，够大了（但好像也只有2个字节？他说keil c里面的int是2个字节，c里面的int是4个字节——我也怕超过极限溢出后成负数or 0？。。）。
                        LED20 = 0; //=0, 熄灭!
                     else
                        LED20 = 1; //=1, 点亮!

                     switch(key)
                     {
                        case 87:
                        case 83:
                           if (WaitCounter%(WaitSetting1/1000)==0)  
                           {
                              k++;                                                                                  //不能用for循环，因此没有赋初始值，因此若用i,j会继承之前在其他地方运行过的值，所以必须用k。
                              k=k%(length+2);
                           }
 									if(liehang[fakehang-1][k]!=10)                                                           //不等于10的相应列才亮，即有东西可显示(P0口给高电平)，有数字的列和相应数码管才给高电平。
                  			{
									   P1=TAB[liehang[fakehang-1][k]];                                                                //把该列所对应的数字字段的P1显示方式给加载上
                     		   P0=1<<P0fake[k];                                                                               //对应列、即从左到右的对应P0口，给高电平。若=10即没有有效的数字可显示，则对应的i、对应列不给高电平。
                  			}
                           else
                           {
                              P1=1<<3;
                              P0=1<<P0fake[k];
                           }
                           if(fakehang==counthang)
                           {
                              if(k==1||k==4)                                                                        //第2个数字和第5个数字后面的小数点点亮
                                 P1=P1|0x80;
                           }
                           else
                           {
                              if(k==3)                                                                              //第4个数字后面的小数点点亮
                                 P1=P1|0x80;                                
                           }
                           break;
                        default:
                           if (WaitCounter%(WaitSetting1/1000)==0)
                           {
                              k++;                                                                                     
                              k=k%(length+2);
                           }
									if(liehang[counthang-1][k]!=10)                                                          //不等于10的相应列才亮，即有东西可显示(P0口给高电平)，有数字的列和相应数码管才给高电平。
                  			{
										P1=TAB[liehang[counthang-1][k]];                                                               //把该列所对应的数字字段的P1显示方式给加载上
                     		   P0=1<<P0fake[k];                                                                               //对应列、即从左到右的对应P0口，给高电平。若=10即没有有效的数字可显示，则对应的i、对应列不给高电平。
                  			}
                           else
                           {
                              P1=1<<3;                                                                                       //划个杠（下划线）
                              P0=1<<P0fake[k];
                           }
                           if(k==1||k==4)                                                                           //第2个数字和第5个数字后面的小数点点亮
                              P1=P1|0x80;
                           break;                        
                     }
                  }
                  key=SBUF0;                                                             //线程并未暂停，每次运行到这里就路过串口接收端看一看，获取按下并传输过来的热键
//                printf("\nLAB=%bd | ",key);                                            //调试
						if(key==87||key==83)                                                   //用getch读取方向键和功能键时，第一次读是0或者0xE0（这里好像永远是224），第二次读才返回真正的键值，所以需要读取两次；在第一次读的时候，要么返回0要么返回224；——getch一次只能读一个字节，读不了两个字节的功能键。
						{
							switch(key)
							{
								case 87:                                                                           //若你按了W键（按键码87）
									fakehang--;                                                                              //必须新建一个fake行，用以查看而非编辑的视角查看以前的行；而不是改变counthang。
//									printf("%d",fakehang);                                                                   //调试
									if(fakehang<1)                                                                           //防止减过度了，若counthang=1，或者fakehang自己减到1了，则得将变成0的fake行加回1来。
										fakehang++;                                                                                    //只有当当前视角所在行数>0时候才能往前跳转；等于0不能再往前跳了
//									printf("%d",fakehang);                                                                   //调试
//               			   for(i=0;i<length+2;i++)                                                                  //打印该行所在的length+2个列所在的数字。——注意：在这之前是刚刚才打印了4个数字，所以现在是立马再打印length+2个数字，有一个flash。——我懒的搞增加判断语句以代码长度，或者为了增加“一闪”这种揭晓答案的紧张感和神秘感，所以我保留这个特性。
//                              printf("\nliehang[%bd][%bd]=%bd",fakehang-1,i,liehang[fakehang-1][i]);                           
                           RI0=0;                                                                                   //若没有按下新的键，则让程序去时钟里循环，除了LED的亮灭和计数之外，啥也不干，直到输入了什么东西，串口发来了啥，以至于RI0≠0；不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个功能键！！！
									SBUF0=key;                                                                               //超级终端回显
                           goto LAB;                                                                                //这里不用break，而用goto lab，因为break后会到countlie++之前开始执行，这不行，我们还没输入有效数字呢！所以要在countlie++之后开始执行。——若要用break是指跳出switch而不是跳出for！
								case 83:                                                                           //若你按了S键
									fakehang++;
									if(fakehang>counthang)                                                                   //防止加过度了，最多加到counthang即当前要输入的行。
										fakehang--;
//               			   for(i=0;i<length+2;i++)                                                                  //打印该行所在的length+2个列所在的数字。——注意：在这之前是刚刚才打印了4个数字，所以现在是立马再打印length+2个数字，有一个flash。——我懒的搞增加判断语句以代码长度，或者为了增加“一闪”这种揭晓答案的紧张感和神秘感，所以我保留这个特性。
//                              printf("\nliehang[%bd][%bd]=%bd",fakehang-1,i,liehang[fakehang-1][i]);
                           RI0=0;                                                                                   //若没有按下新的键，则让程序默认按下j键（74）=0x28，不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个功能键！！！
									SBUF0=key;                                                                               //超级终端回显
                           goto LAB;
								default:goto LAB;                                                                  //在这里没有用，这里的default的功能被整合到了其所在的if最后的else内的数字键功能中的goto LAB中去。
							}
						}
						else if(key==0x1b)                                                     //若你按下了ESC键（27）
						{
                  	SBUF0=key;
                     RSTSRC = RSTSRC | 0x10;
                  }
						else
						{
							for(i=0;i<10;i++)                                                            //看看热键是哪个数字
							{
								if(key==48+i)                                                                      //看看是否输入了数字键0~9
								{
									P0=0;                                                                                    //同志，记着清行。printf(“\r”)系列都改成P0=0——\r+8个空白字符再+\r，好东西，真的：先把光标移到最前，然后覆盖输入8个字符（擦除），然后再把光标移到最前。
									goto Restart;
								}
							}
                     RI0=0;                                                                       //若没有按下新的键，则让程序默认按下j键（74）=0x28，不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个功能键！！！
							SBUF0=key;
                     goto LAB;                                                                    //否则若输入了其他非功能键字符，给我重新输入!
						}
//————————————————————————————————————————————————————————————————————————————————————已经加入查看历史记录、退出、重开一局机制
					}
					else
					{
//               	for(i=0;i<length+2;i++)                  
//                   printf("\nliehang[%bd][%bd]=%bd",counthang-1,i,liehang[counthang-1][i]);	  //在它之前，若用了SBUF0会导致吃掉这里的1个字符\n。						                  
                  TAGG=1;                                                                //跳出while后counthang会++，但显示的仍然是length+2个有数的当前行而不是length+2个空白的新行，所以需要抵消掉这一差距，在这里设置存档点，往回时光追溯fakehang--。
//						RI0=0;                                                                 //这里的RI=0不要忘了加，否则之后不会闪灯了，一直执行程序，没有暂停。
                  break;                                                                 //你还是输错了，break掉它所在的while，继续暗无天日地输入新的行。
					}
				}
				else
				{
					fakehang=counthang;                                              //要在lab之前设置一个fakehang=counthang，否则每次跳到lab，往下走必然遇到fakehang=counthang，这样导致每次fakehang--之后又回到counthang了；而在fakehang--之外设置这个fakehang=counthang，就是为了每次输入了有效数字之后，屏幕会刷新显示最新的行，于是查看视角也得从之前的行挪回来，否则不人性化。
					if(TAGG==1)                                                      //若输入完了某一行，并且没有成功猜对密码，那么counthang会++并储存空数，但显示的仍然是length+2位数的错误结果（以供给玩家查看，不然结果一下就跳过去了）；那么在fakehang获取到的counthang不是当前行，而是当前行+1的新行序列号，需要往回追溯即-1；不用担心刚开始输进来的时候counthang=1的事情，会不会减为-1然后又加为0，因为只有当完成了第一次判断后，TAGG才会变成1，在此之前没有输入完成4个数字时，都是0。——只有输入了4个数字，该条件才能触发该语句，即视角行上移，这是因为只有当输入了4个数字后，视角行才与counthang错位了。——所以其他按键、其他条件，均不能触发它。——所以只需要在触发完毕后，在其后面加一个TAGG=0即可，其他地方不需要加TAGG=0，是多余的；也没有地方能加TAGG=1。因为只能有这一个触发开关or机制。
						fakehang--;
//					printf("fakehang %bd | ",fakehang);                              //调试
lab:           while(RI0==0)
               {
                  WaitCounter++;                                                         //时钟启动。重开一盘游戏时以非零值进来也没问题，对记录时间的真实情况影响程度不大，而且反正超过set就重置。——而且我们需要这个来用WaitCounter做种
//                printf("Counter %ld | ",WaitSetting1);                                 //注意ld不能写作bd，否则输出的全是0，且因为printf %bd的存在，会导致waitcounter一直都是0，即会对她赋值；51单片机的特点，bd针对uchar or char or int??。
                  if(WaitCounter>WaitSetting1)                                           //1.若名字有冲突，应该早在定义的时候就不能用WaitSetting这个名字作为变量，必须要用WaitSetting1等其他名字，我是真的服（否则无法初始化其值，会一直是个很大的值）。
                  {                                                                      //2.WaitCounter和WaitStorer都无所谓，需要注意的是，WaitStorer需要用%d或者%hd才能正确显示其数值，另两个Wait需要用%ld。
                     WaitCounter=0;                                                               //3.即使是在其他地方，只要有printf语句存在，就会运行得特别缓慢，任何地方，包括这里的while；似乎printf不仅只在printf处运行的慢。与TI=0无关。
                     WaitStorer++;
                  }

                  if(WaitCounter<WaitSetting1/2)   
                     LED20 = 0; //=0, 熄灭!
                  else
                     LED20 = 1; //=1, 点亮!
     
                  if(TAGG==1)
					      key=87;                                                                      //若刚输完4个字回来，则显示上一个fakehang而不是新的一行空白行。

                  switch(key)
                  {
                     case 87:
                     case 83:
                        if (WaitCounter%(WaitSetting1/1000)==0)  
                        {
                           k++;                                                                                     //不能用for循环，因此没有赋初始值，因此若用i,j会继承之前在其他地方运行过的值，所以必须用k。
                           k=k%(length+2);
                        }
								if(liehang[fakehang-1][k]!=10)                                                     //不等于10的相应列才亮，即有东西可显示(P0口给高电平)，有数字的列和相应数码管才给高电平。
                 			{
									P1=TAB[liehang[fakehang-1][k]];                                                          //把该列所对应的数字字段的P1显示方式给加载上
                    		   P0=1<<P0fake[k];                                                                         //对应列、即从左到右的对应P0口，给高电平。若=10即没有有效的数字可显示，则对应的i、对应列不给高电平。
                 			}
                        else
                        {
                           P1=1<<3;
                           P0=1<<P0fake[k];
                        }                                                                                 
                        if(k==3)                                                                           //第4个数字后面的小数点点亮——但若第四个数字是10，则由于P1是“不显示”，所以无法显示P1及其小数点。
                           P1=P1|0x80;                                                                     //同时显示字段和相应字段所在数码管的小数点。——当然它也可加上P0来控制，同一个套路，将P1和P0写在该if下的同一个大括号内。
//                      SBUF0=liehang[fakehang-1][k]+48;
                        break;
                     case 8:
                        if (WaitCounter%(WaitSetting1/1000)==0)    
                        {
                           k++;                                                                                     
                           k=k%(length+2);
                        }
								if(liehang[counthang-1][k]!=10)                                                    //不等于10的相应列才亮，即有东西可显示(P0口给高电平)，有数字的列和相应数码管才给高电平。
                  		{
									P1=TAB[liehang[counthang-1][k]];                                                         //把该列所对应的数字字段的P1显示方式给加载上
                     		P0=1<<P0fake[k];                                                                         //对应列、即从左到右的对应P0口，给高电平。若=10即没有有效的数字可显示，则对应的i、对应列不给高电平。
                  		}
                        else
                        {
                           P1=1<<3;
                           P0=1<<P0fake[k];
                        }                                                                                 
                        if(k==3)                                                                           //第4个数字后面的小数点点亮
                           P1=P1|0x80;
//                      SBUF0=liehang[counthang-1][k]+48;
                        break;
                     case 75:
                        if(shift==1)
                        {
                           if (WaitCounter%(WaitSetting1/1000)==0)
                           {
                              k++;                                                                                     
                              k=k%(length);
                           }
									P1=TAB[lieshu[k]];                                                                       //把该列所对应的数字字段的P1显示方式给加载上
                     		P0=1<<P0fake[k];                                                                                  //对应列、即从左到右的对应P0口，给高电平。若=10即没有有效的数字可显示，则对应的i、对应列不给高电平。
                           if(k==3)                                                                                 //第4个数字后面的小数点点亮
                              P1=P1|0x80;
//                         SBUF0=lieshu[k]+48;
                        }
                        else
                        {
                           if (WaitCounter%(WaitSetting1/1000)==0)  
                           {
                              k++;                                                                                  //不能用for循环，因此没有赋初始值，因此若用i,j会继承之前在其他地方运行过的值，所以必须用k。
                              k=k%(length+2);
                           }
 									if(liehang[fakehang-1][k]!=10)                                                           //不等于10的相应列才亮，即有东西可显示(P0口给高电平)，有数字的列和相应数码管才给高电平。
                  			{
										P1=TAB[liehang[fakehang-1][k]];                                                                //把该列所对应的数字字段的P1显示方式给加载上
                     		   P0=1<<P0fake[k];                                                                               //对应列、即从左到右的对应P0口，给高电平。若=10即没有有效的数字可显示，则对应的i、对应列不给高电平。
                  			}
                           else
                           {                                                                                        
                              P1=1<<3;
                              P0=1<<P0fake[k];
                           }
                           if(k==3)                                                                                 //第4个数字后面的小数点点亮
                              P1=P1|0x80;
//                         SBUF0=liehang[fakehang-1][k]+48;
                        }
                        break;
                     case 48:
                     case 49:
                     case 50:
                     case 51:
                     case 52:
                     case 53:
                     case 54:
                     case 55:
                     case 56:
                     case 57:
                        if (WaitCounter%(WaitSetting1/1000)==0)
                        {
                           k++;                                                                                     
                           k=k%(length+2);
                        }
								if(liehang[counthang-1][k]!=10)                                                    //不等于10的相应列才亮，即有东西可显示(P0口给高电平)，有数字的列和相应数码管才给高电平。
                  		{
									P1=TAB[liehang[counthang-1][k]];                                                         //把该列所对应的数字字段的P1显示方式给加载上
                     		P0=1<<P0fake[k];                                                                         //对应列、即从左到右的对应P0口，给高电平。若=10即没有有效的数字可显示，则对应的i、对应列不给高电平。
                  		}
                        else
                        {
                           P1=1<<3;
                           P0=1<<P0fake[k];
                        }
                        if(k==3)                                                                           //第4个数字后面的小数点点亮
                           P1=P1|0x80;
                        break;                                             
                     default:                        
                        if (WaitCounter%(WaitSetting1/1000)==0)  
                        {
                           k++;                                                                                     //不能用for循环，因此没有赋初始值，因此若用i,j会继承之前在其他地方运行过的值，所以必须用k。
                           k=k%(length+2);
                        }
								if(liehang[fakehang-1][k]!=10)                                                     //不等于10的相应列才亮，即有东西可显示(P0口给高电平)，有数字的列和相应数码管才给高电平。
                 			{
									P1=TAB[liehang[fakehang-1][k]];                                                          //把该列所对应的数字字段的P1显示方式给加载上
                    		   P0=1<<P0fake[k];                                                                         //对应列、即从左到右的对应P0口，给高电平。若=10即没有有效的数字可显示，则对应的i、对应列不给高电平。
                 			}
                        else
                        {
                           P1=1<<3;
                           P0=1<<P0fake[k];
                        }                                                                                 
                        if(k==3)                                                                           //第4个数字后面的小数点点亮——但若第四个数字是10，则由于P1是“不显示”，所以无法显示P1及其小数点。
                           P1=P1|0x80;                                                                     //同时显示字段和相应字段所在数码管的小数点。——当然它也可加上P0来控制，同一个套路，将P1和P0写在该if下的同一个大括号内。
//                      SBUF0=liehang[fakehang-1][k]+48;
                        break;
                  }

               }
               TAGG=0;                                                          //及时清零，否则在输入完length个字后，连按4次↑，会往上跳1+2+2+2行...——注：以上这三行要放在lab:后面，不能放在lab前面、fakehang=counthang后面，否则由于键入数字和键入退格那里用的是goto lab，则其中的TAGG=0虽然确实改变了TAGG的值，但无法sendmessage到lab上面的if(TAGG=1)中去执行fakehang--，结果就是TAGG确实变成了1，但fakehang仍然没有--...emm...下面TAGG=0就是为了不要让fakehang--（因为按了数字或backspace后，视角所在的行fakehang已经是新行了，二者重合了没有错位），所以这3句还是可以拿到lab上面去的，并且为了百分百不让数字和退格引起fakehang--而最好拿上去。甚至我们还可以把下面的两个TAGG=0删了，没用，只要下面不出现TAGG=1就行。
					key=SBUF0;                                                       //线程并未暂停，每次运行到这里就路过串口接收端看一看，获取按下并传输过来的热键
//					SBUF0=key;
//               printf("\nlab=%bd | ",key);
//====================
      if(TIMER==0)                                   //取双随机数也没有用，必须要进入某个循环来counter++，以取运行时间，且只有第一次运行到这里才不进入这个循环，否则进入这个循环；
      {

      srand((unsigned)WaitCounter);                  //每次进入该循环，都以当前距离1970年1月1日0时0分0秒，过的秒数，播种种子序列。——注：time(0)与time(null)若在同一时刻调用，返回的值一样【比较time(NULL)和time(0),没有多大意义】；然后将其强转为unsigned  int类型，其取值范围缩减为了0~length+25535，再作为参数给srand()，它的参数需要是unsigned int类型。
      for(j=0;j<length;j++)                          //我需要在0~9间，随机找length个不重复的数字
		{
			while(1)                                             //需要随机到与储存的随机数字不同的数字，才给我出来
			{
				randnum=rand()%10;                                         //获取一个0~9之间的随机数，临时储存下来。——rand()：根据紧邻的上一个srand()的函数值，在0~327length+27之间返回随机数
//          printf("randnum=%bd | ",randnum);                          //调试用；亦或是作弊按钮。——注意第一次用print会丢失掉一个字符，所以要print两个P作为分隔符。
				for(i=0;i<j;i++)                                           //查看randnum是否与0~j-1对应的，list[i]中前j-1个数中，任何一个相同。
				   if(lieshu[i]==randnum)
                  break;                                                                 //若有重复的，则提前break掉for循环，并且不满足j==i条件，而重新进入while(1)的开头，继续随机，直到随机到一个与之前存储的没有重复的。
				if(i==j)                                                   //若能进入该if，则i会加到j（j的最大值是3），且该randnum与list[i]中i=j-1及其之前的数字都不重复的，因此将其保存到最新的list[j]中。
			   {
				   lieshu[j]=randnum;                                               //将length个0~9的、且每一个都与前面所有的不同的，随机数，存入lieshu[0]、lieshu[1]、lieshu[2]、lieshu[3]...中。
////           TI0=1;
//             printf("lieshu[%bd]=%bd\n",j,lieshu[j]);                         //调试用；亦或是作弊按钮。
////           TI0=0;                                                           //不要加这行，准确地说，“TI0=1+(printf的循环)”的模式，不能写为“(TI0=1+printf+TI0=0)的循环”的模式！
////           SBUF0=randnum+48;                                                //不知道为什么，在这里SBUFO无法输出任何东西，下面倒可以。
					break;                                                           //这个break是break掉while，开始执行for循环中下一个j值的while，直到下一个不重复的随机数被找出来。
				}
			}   
//         printf("\ncounthang=%bd | ",counthang);
		}
//		counthang=0;                                   //我是真的服，赋给lieshu[j]的值，同时会自动赋值给counthang...？

      }
      TIMER=1;                                       //第一次进入这里之后，再也不进入这里
//====================                  
               timer++;                                                         //每次运行到这里，timer就累加。0进来时，才触发/进入下面的函数，之后一直是1进来。
					if(timer==1)                                                     //只有当第一次来到这里时，才能进入if进行时间启动
               {
     					start=WaitStorer;                                                      //开始计时
                  finish=start;                                                          //防止finish比start小以至于之后的finish-start<0，输出不了负数。
               }
               timer=1;                                                         //让timer一直=1，也可写作timer=1或timer=2；不能写作timer--，不知道为什么timer---反而会导致countlie--，它俩是同义词？再说timer只是从1减到0，没有从0减到负数溢出。
//					printf("%d",key);                                                //调试
					if(key==87||key==83)                                             //用getch读取方向键和功能键时，第一次读是0或者0xE0（这里好像永远是224），第二次读才返回真正的键值，所以需要读取两次；在第一次读的时候，要么返回0要么返回224；——getch一次只能读一个字节，读不了两个字节的功能键。
					{
//						printf("%d",KEY);                                                      //调试
						switch(key)                     
						{
							case 87:                                                                     //若你按了上键
								fakehang--;                                                                        //必须新建一个fake行，用以查看而非编辑的视角查看以前的行；而不是改变counthang。
//			               printf("%d",fakehang);                                                             //调试
								if(fakehang<1)                                                                     //防止减过度了，若counthang=1，或者fakehang自己减到1了，则得将变成0的fake行加回1来。
									fakehang++;                                                                              //只有当当前视角所在行数>0时候才能往前跳转；等于0不能再往前跳了
//								printf("%d",fakehang);                                                             //调试
//               			for(i=0;i<length+2;i++)                                                            //打印该行所在的length+2个列所在的数字。——注意：在这之前是刚刚才打印了4个数字，所以现在是立马再打印length+2个数字，有一个flash。——我懒的搞增加判断语句以代码长度，或者为了增加“一闪”这种揭晓答案的紧张感和神秘感，所以我保留这个特性。
//                         printf("\nliehang[%bd][%bd]=%bd",fakehang-1,i,liehang[fakehang-1][i]);
								shift=0;														                          //下一次按k则显示密码
                        RI0=0;                                                                    //若没有按下新的键，则让程序默认按下j键（74）=0x28，不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个功能键！！！
                        SBUF0=key;
                        goto lab;                                                                 //这里不用break，而用goto lab，因为break后会到countlie++之前开始执行，这不行，我们还没输入有效数字呢！所以要在countlie++之后开始执行。——若要用break是指跳出switch而不是跳出for！
							case 83:                                                                     //若你按了下键
								fakehang++;
			               if(fakehang>counthang)                                                             //防止加过度了，最多加到counthang即当前要输入的行。
									fakehang--;
//               			for(i=0;i<length+2;i++)                                                            //打印该行所在的length+2个列所在的数字。——注意：在这之前是刚刚才打印了4个数字，所以现在是立马再打印length+2个数字，有一个flash。——我懒的搞增加判断语句以代码长度，或者为了增加“一闪”这种揭晓答案的紧张感和神秘感，所以我保留这个特性。
//                         printf("\nliehang[%bd][%bd]=%bd",fakehang-1,i,liehang[fakehang-1][i]);
								shift=0;														                                   //下一次按k则显示密码
                        RI0=0;                                                                             //若没有按下新的键，则让程序默认按下j键（74）=0x28，不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个功能键！！！
								SBUF0=key;
                        goto lab;
							default:goto lab;                                                            //在这里没有用，这里的default的功能被整合到了其所在的if最后的else内的数字键功能中的goto LAB中去。
						}
					}
					else if(key==8)                                                  //若你按下了退格键——————算了算了，我不用16进制了
					{
						fakehang=counthang;                                                    //按了退格键后，下次调换视角的时候从最后一行开始（往上）调
//多余			   TAGG=0;                                                                //只要按了退格键，则已经在最新的一行（要么在新行删不动第一列，要么在原行删前三位数）操作数字了，所以counthang就是现在的行，没有问题；所以此时必须把TAGG修正回来；否则会出现之前TAGG=1时若没有马上用掉它，即若刚输完4个数字，没有马上按上下键查看的话，之后在下一行输入字母且往回查看的时候，第一次按↑会多退回一行...。
						if(countlie>1)                                                         //若countlie=1。第一个数字都没有输入，在等待输入第一个，则啥也不干，只管打印输出，然后goto lab继续等待输入；若TAGG=1，即刚输入完4位数，则其实此时的光标早就移动到下一行的首列了，此时按退格相当于进入另起炉灶下一行，对下一行的首列按退格，即显示下一列的所有空格= =哈哈，被自己的眼睛骗了吧。——当然不能对已判断了的结果再进行更改，不然这就相当于作弊，在试错！每次已经输入了的4位数，任何一位都不允许被更改，他将作为失败+1，被暂时地永久保存于历史记录清单里。
						{
							liehang[counthang-1][countlie-2]=10;                                         //同一行的，将要输入的空的列，的上一个列，恢复为缺省值10，即相当于其没有输入
							countlie--;                                                                  //将要输入的空的列，变为上一个列
						}
//               	for(i=0;i<length+2;i++)                  
//                  	printf("\nliehang[%bd][%bd]=%bd",counthang-1,i,liehang[counthang-1][i]);
                  shift=0;														                       //下一次按k则显示密码
                  RI0=0;                                                                 //若没有按下新的键，则让程序默认按下j键（74）=0x28，不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个功能键！！！
						SBUF0=key;
                  goto lab;                                                              //回到标签处；不需要额外加入tag=0，tag会保持0不变。因为只有在输入非功能键的时候会找数字，找数字的时候才会有是否找到了真数字一说，才是否有tag=1=0一说。
					}
					else if(key==27)                                                 //若你按下了ESC键，则结束程序——你在玩游戏的时候也允许退出，毕竟出于人道主义，可能你妈妈真的喊你吃饭了。
					{
               	SBUF0=key;
                  RSTSRC = RSTSRC | 0x10;
               }
					else if(key==75)                                                 //若你按下了K(Key)键
					{
						if(shift%2==0)                                                         //如果k键被按奇数次，则在视角行显示正确的密码。
               	{
//                   for(i=0;i<length+2;i++)                  
//                      printf("\nlieshu[%bd]=%bd",i,lieshu[i]);
							shift=1;                                                                     //下一次按k则复原
						}
                  else                                                                   //如果k键被按偶数次，则恢复视角行所在的原来的数字。——也可通过其他方法来恢复视角行原数字。
						{
//               		for(i=0;i<length+2;i++)                                                                
//                     	printf("\nliehang[%bd][%bd]=%bd",fakehang-1,i,liehang[fakehang-1][i]);
                  	shift=0;                                                                     //下一次按k则显示密码
                  }
                  RI0=0;                                                                 //若没有按下新的键，则让程序默认按下j键（74）=0x28，不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个功能键！！！
						SBUF0=key;
                  goto lab;                                                              //记住不要忘了加上goto lab;
					}
					else
					{
						for(i=0;i<10;i++)                                                      //看看热键是哪个数字
						{
							if(key==48+i)                                                                //看看是否输入了数字键（有可能会输入其他如退格键、字母等非功能按键；我tm好奇退格键居然不是功能键）
							{                                                                      
								for(j=0;j<countlie-1;j++)
								{
									if(key==liehang[counthang-1][j]+48)                                                      //看看这些数字键是否与旧有的重合，若是同一行中，以前输入的数字子集，则goto lab重新输入；否则若与每个之前的都不同，则for循环进行完毕，接下来顺其自然进行录入工作。
                           {
                              RI0=0;                                                                                //若没有按下新的键，则让程序默认按下j键（74）=0x28，不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个键！！！
                              goto lab;

                           } 
								}
								liehang[counthang-1][countlie-1]=key-48;                                           //若匹配到了对应的数字，则贮存在对应行、列的数组里，并且马上退出for循环，不用去匹配10个数字中剩下的，以免浪费计算机资源——并马上跳转到goto lab上面的countlie++处，而不是进入goto lab。
//               	      for(i=0;i<length+2;i++)                  
//                        	printf("\nliehang[%bd][%bd]=%bd",counthang-1,i,liehang[counthang-1][i]);								
                        tag=1;                                                                             //若真找到了数字，则标记上“找到了数字”。
								shift=0;													                                      //下一次按k则显示密码
//想多了是因if(TAGG=1)造成的后果 TAGG=0;                                                                   //按退格键或输入其他数字时，反正只要没有在输入上下键，只要在操作数字而不是其他按键，则已经在新的一行操作数字了，所以counthang就是现在的行，没有问题；所以此时必须把TAGG修正回来；否则会出现之前TAGG=1时若没有马上用掉它，即若刚输完4个数字，没有马上按上下键查看的话，之后在下一行输入字母且往回查看的时候，第一次按↑会多退回一行...。
                        RI0=0;                                                                             //若没有按下新的键，则让程序默认按下j键（74）=0x28，不能让程序反复执行这一段，它不会反复显示相同的东西，而是会一直按这个功能键！！！
                        SBUF0=key;                                                                         //由于printf里面自带SBUFO，所以SBUFO都没用了？？现象：只要用了printf，不管哪里的SBUF0都没用了。
                        break;                                                                             //注意这里break只对最近一层/其所处层的for有用，对if或者上一层的for没用。
							}                                                                            
						}
						if(tag==0)                                                             //若“没有找到数字”，则继续找数字
                  {
                     RI0=0;                                                                       //每一个goto lab和goto LAB前面都得加RI0=0;上一个break前面也得加。
                     goto lab;                                                                    //若运行到这里，必然上面的for循环是运行到i=10运行完了的，没有找到数字，没有break。因此此时没有找到数字键，因此输入的按键，是既是功能键外，又是数字键外的按键，比如字母字符。
					   }
               }
				}
			}                                                                                      
		}
	}
}
//之所以写这么多注释，一方面体现逻辑的力量；另一方面，我之前写的12个c程序由于没有注释，我自己也很难再看懂了，也没有后继者来看我的程序（尽管即使详细地标记了注释，人们也很难看懂，因为写程序不是从上往下写的，而是往里插的，每一条注释的标注时间没有给出，也就没有给出它们的生成顺序，就不能像小说一样读了；但即使顺序看上去杂乱无章，每一步背后的逻辑的顺序仍然是必要、最简且严密的，当然，只是努力做到最简罢了.最简的代码不易读，但很聪明）。
//注释格式是缩进的，但是注释的缩进是用空格缩进的，这一点做得不是很好，但毕竟写了这么多了，我就懒的搞改了。以后的程序我会注意这一点，注释的缩进尽量与代码的缩进保持一致，形成分明的层级。
//1.02_beta更新日志：4替换成length（但要注意48这种键代码），6替换成length+2（这个直接替换即可）——3和5也不用管，因为我没有用<=，而一直用<。
