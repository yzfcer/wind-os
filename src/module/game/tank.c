
#include <stdio.h>
#include <windows.h>
#include <time.h> 
                           //里规格：长39*2=78 （真坐标）(假坐标宽为39)  高39
                           //外规格：长41*2=82 （真坐标）(假坐标宽为41)  高41
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
#define MAX_LEVEL 8
#define BULLET_NUM 40
#define MAX_LIFE 4
 
//程序中未写入函数参数表中且未说明的变量只有map二维数组,level_info数组和level   
 
/*
      此程序中涉及的x,y类的坐标值，分为以下两种：                                 
                                                                                                  
假坐标：这里的坐标指的是以一个■长度为单位的坐标，而不是真正的coord坐标 (用于map数组的坐标)                             
                                                                                                  
真坐标：头文件自带的坐标结构coord中的坐标（也可以说是控制台里的真正坐标值）                                 
                                                                                                  
  区别：纵坐标y两值一致，假横坐标x值与真正coord横坐标（真坐标）关系是 x * 2 = coord 横坐标    
                                                                                                            
		  coord横坐标既指GoTo函数中的x参数,因为本程序游戏界面以一个■长度为基本单位，                    
                                                                                                  
		  可以说涉及的coord横坐标全是偶数。既假坐标要变真坐标（变真坐标才能发挥真正作用），横坐标须乘以2                                    
                                                           
*/
typedef struct             //这里的出现次序指的是一个AI_tank变量中的次序，游戏共有四个AI_tank变量
{                          //∵设定每个AI_tank每种特殊坦克只出现一次 ∴fast_tank & firm_tank 最多出现次数不超过1
    int fast_tank_order;   //fast_tank出现的次序(在第fast_tank_order次复活出现,从第0次开始)，且每个AI_tank只出现一次
    int firm_tank_order;   //firm_tank出现的次序，同上
}LevInfo;                 //关卡信息(准确说是该关出现的坦克信息)
LevInfo level_info [MAX_LEVEL] = {{-1,-1},{3,-1},{-1,3},{2,3},{2,3},{2,3},{2,3},{2,3}};   //初始化，-1代表没有该类型坦克
 
 
typedef struct      //子弹结构体
{
    int x,y;        //子弹坐标,假坐标
    int direction;  //子弹方向变量
    int exist;     //子弹存在与否的变量,1为存在，0不存在
    int initial;   //子弹是否处于建立初状态的值，1为处于建立初状态，0为处于非建立初状态
    int my;        //区分AI子弹与玩家子弹的标记,0为AI子弹，1为玩家（我的）子弹
}Bullet;

static Bullet bullet[BULLET_NUM];  //考虑到地图上不太可能同时存在20颗子弹，所以数组元素设置20个
 
#if 1
 
typedef struct      //坦克结构体
{
    int x,y;        //坦克中心坐标
    int direction;  //坦克方向
    int color;      //颜色参方向数，1到6分别代表不同颜色，具体在PrintTank函数定义有说明
    int model;      //坦克图案模型，值为1,2,3，分别代表不同的坦克图案,0为我的坦克图案，AI不能使用
    int stop;       //只能是AI坦克使用的参数，非0代表坦克停止走动,0为可以走动
    int revive;     //坦克复活次数
    int num;        //AI坦克编号（固定值，为常量，初始化函数中定下）0~3
    int CD;         //发射子弹冷却计时
    int my;        //是否敌方坦克参数，我的坦克此参数为1,为常量
    int alive;     //存活为1，不存活为0
}Tank;
static Tank AI_tank[4] , my_tank;  //my_tank为我的坦克，Ai_tank 代表AI坦克
//∵所有的函数都有可能对全局变量map进行读写(改变)，
//∴函数中不另说明是否会对全局变量map读写
//基本操作与游戏辅助函数
static void GoToxy(int x,int y);    //光标移动
static void HideCursor();           //隐藏光标
static int keyboard ();            //接受键盘输入
static void Initialize();           //初始化(含有对多个数据的读写)
static int Stop();                 //暂停
static void Getmap();               //地图数据存放与获取
static void Frame ();               //打印游戏主体框架
static void PrintMap();             //打印地图(地图既地图障碍物)(含对level的读取)
static void SideScreen ();          //副屏幕打印
static void GameCheak();            //检测游戏输赢
static int GameOver(int home);  //游戏结束
static void ClearMainScreen();      //主屏幕清屏函数∵system("cls")后打印框架有一定几率造成框架上移一行的错误∴单独编写清屏函数
static void ColorChoose(int color); //颜色选择函数
static int NextLevel();            //下一关(含有对level全局变量的读写)
 
//子弹部分
static void BuildAIBullet(Tank *tank);                //AI坦克发射子弹（含有对my_tank的读取,只读取了my_tank坐标）
static void BuildBullet(Tank tank);                 //子弹发射（建立）（人机共用）(含全局变量bullet的修改)我的坦克发射子弹直接调用该函数,AI通过AIshoot间接调用
static void BulletFly(Bullet bullet[BULLET_NUM]); //子弹移动和打击（人机共用）,
static void BulletHit(Bullet* bullet);            //子弹碰撞（人机共用）(含Tank全局变量的修改)，只通过BulletFly调用，子弹间的碰撞不在本函数,子弹间碰撞已在BulletShoot中检测并处理
static void PrintBullet(int x,int y,int T);         //打印子弹（人机共用）
static void ClearBullet(int x,int y,int T);         //清除子弹（人机共用）
static int  BulletCheak(int x,int y);               //判断子弹前方情况（人机共用）
 
//坦克部分
static void BuildAITank (int* position, Tank* AI_tank); //建立AI坦克
static void BuildMyTank (Tank* my_tank);                //建立我的坦克
static void MoveAITank  (Tank* AI_tank);                //AI坦克移动
static void MoveMyTank  (int turn);                     //我的坦克移动，只通过keyboard函数调用，既键盘控制
static void ClearTank   (int x,int y);                  //清除坦克（人机共用）
static void PrintTank   (Tank tank);                    //打印坦克（人机共用）
static int TankCheck   (Tank tank,int direction);      //检测坦克dirtection方向的障碍物,返值1阻碍,0 畅通
static int  AIPositionCheak (int position);           //检测AI坦克建立位置是否有障碍物AIPositionCheak
 
//DWORD WINAPI InputX(LPVOID lpParameter); //声明线程函数，用于检查X键输入并设置X键的输入冷却时间
 
 
//注意map数组应是纵坐标在前，横坐标在后，既map[y][x]，(∵数组行长度在前，列长度在后)
//map里的值: 个位数的值为地图方块部分，百位数的值为坦克,子弹在map上没有值(子弹仅仅是一个假坐标)
//map里的值: 0为可通过陆地，1为红砖，2黄砖，5为水，100~103为敌方坦克，200为我的坦克，
 
//全局变量
static int map[41][41];  //地图二维数组
static int key_x;        // X键是否被“读入”的变量，也是子弹是否可以发射的变，
static int bul_num;      //子弹编号
static int position;     //位置计数,对应AI坦克生成位置,-1为左位置,0为中间,1为右,2为我的坦克位置
static int speed=5;      //游戏速度,调整用
static int level=1;      //游戏关卡数
static int score=0;      //游戏分数
static int remain_enemy; //剩余敌人(未出现的敌人)
 
static char* tank_figure[4][3][4]=
{
  {
    {"◢┃◣", "◢━◣", "◢┳◣", "◢┳◣"},
    {"┣●┫", "┣●┫", "━●┃", "┃●━"},
    {"◥━◤", "◥┃◤", "◥┻◤", "◥┻◤"}
  }, 
  {
    {"┏┃┓", "┏┳┓", "┏┳┓", "┏┳┓"},
    {"┣●┫", "┣●┫", "━●┫", "┣●━"},
    {"┗┻┛", "┗┃┛", "┗┻┛", "┗┻┛"}
  }, 
  {
    {"┏┃┓", "◢━◣", "┏┳◣", "◢┳┓"},
    {"┣●┫", "┣●┫", "━●┃", "┃●━"},
    {"◥━◤", "┗┃┛", "┗┻◤", "◥┻┛"}
  },
  {
    {"╔┃╗", "╔╦╗", "╔╦╗", "╔╦╗"},
    {"╠█╣", "╠█╣", "━█╣", "╠█━"},
    {"╚╩╝", "╚┃╝", "╚╩╝", "╚╩╝"}
  }
};
 
 
static int keyboard ()
{               // kbhit()   getch()  用法可用但是不好用            
/* 
   函数功能:该函数判断在此函数被调用时,某个键是处于UP状态还是处于DOWN状态,及前次调用GetAsyncKeyState函数后,
   是否按过此键.如果返回值的最高位被置位,那么该键处于DOWN状态;如果最低位被置位,那么在前一次调用此函数后,此键被按过,
   否则表示该键没被按过.
   这里GetAsyncKeyState比 kbhit() + getch() 好用,操作更顺畅.   GetAsyncKeyState的返回值表示两个内容，
   一个是最高位bit的值，代表这个键是否被按下。一个是最低位bit的值,代表上次调用GetAsyncKeyState后，这个键是否被按下。
   &为与操作，&0x8000就是判断这个返回值的高位字节。如果high-order bit是1,则是按下状态，否则是弹起状态，为0
*/
	int count=0;
	if (GetAsyncKeyState(VK_UP)& 0x8000)  
		MoveMyTank( UP );
	else if (GetAsyncKeyState(VK_DOWN)& 0x8000)  
		MoveMyTank( DOWN );
	else if (GetAsyncKeyState(VK_LEFT)& 0x8000)
        MoveMyTank( LEFT );
	else if (GetAsyncKeyState(VK_RIGHT)& 0x8000)  
		MoveMyTank( RIGHT );
	else if (GetAsyncKeyState( 0x1B )& 0x8000)  // Esc键
		return 1;                                //退出程序函数
	else if (GetAsyncKeyState( 0x20 )& 0x8000)  //空格
		return Stop();
	else if (count++%7==0)            //这里添加计数器是为了防止按键粘连不能达到微调效果
	{
		if (speed>1 && GetAsyncKeyState( 0x6B )& 0x8000)   // +键
		{
			speed--;
			GoToxy(102,11);           //在副屏幕打印出当前速度
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_RED);
			printf("%d ",21-speed);   //副屏幕显示的速度为1~10
		}
		else if (speed<20 && GetAsyncKeyState( 0x6D )& 0x8000)  // - 键
		{
			speed++;
			GoToxy(102,11);           //在副屏幕打印出当前速度
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_RED);
			printf("%d ",21-speed);   //副屏幕显示的速度为1~10
		}
	}
	if(my_tank.CD>=2)
	{
		if(GetAsyncKeyState( 88 )& 0x8000)
		{
			BuildBullet(my_tank);
			my_tank.CD=0;
		}
	}
	else
		my_tank.CD++;
    return 0;
}
 
 
 
void BuildAIBullet(Tank *tank)   //AI子弹发射(建立)含有对my_tank的读取
{
    int i,big,smal;
	if(tank->CD==15)
	{
		if(!(rand()%11))     //冷却结束后在随后的每个游戏周期中有10分之一的可能发射子弹
		{
			BuildBullet(*tank);
			tank->CD=0;
		}
	}
	else
		tank->CD++;
	if(tank->CD >= 14)       //AI强化部分，在冷却到达一定范围即可使用
	{
		if(tank->y==38 )     //如果坦克在底部(这个最优先)
		{
			if(tank->x < 20) //在老家左边
			{
				if(tank->direction==RIGHT)  //坦克方向朝左
				{
					BuildBullet(*tank);     //发射子弹
					tank->CD=0;
				}
			}
			else             //在老家右边
				if(tank->direction==LEFT)   //坦克方向朝右
				{
					BuildBullet(*tank);     //发射子弹
					tank->CD=0;
				}
		}
		else if(tank->x==my_tank.x+1 || tank->x==my_tank.x || tank->x==my_tank.x-1)  //AI坦克在纵向上"炮口"对准我的坦克
		{
			if(tank->direction==DOWN && my_tank.y > tank->y || tank->direction==UP && my_tank.y < tank->y)
			{                               //若是AI朝下并且我的坦克在AI坦克下方(数值大的在下面)或者AI朝上我的坦克在AI上方
				big=my_tank.y , smal=tank->y , i; 
				if(my_tank.y < tank->y)
				{
					big=tank->y;
					smal=my_tank.y;
				}
				for(i=smal+2;i<=big-2;i++)  //判断AI炮口的直线上两坦克间有无障碍
					if(map[i][tank->x]!=0 || map[i][tank->x]!=5)      //若有障碍
						break;
				if(i==big-1)                //若i走到big-1说明无障碍
				{
					BuildBullet(*tank);     //则发射子弹
					tank->CD=0;
				}
			}
		}
		else if(tank->y==my_tank.y+1 || tank->y==my_tank.y || tank->y==my_tank.y-1) //AI坦克在横向上"炮口"对准我的坦克
		{
			if(tank->direction==RIGHT && my_tank.x > tank->x || tank->direction==LEFT && my_tank.x < tank->x)
			{                  //若是AI朝右并且我的坦克在AI坦克右方(数值大的在下面)或者AI朝左我的坦克在AI左方
				big=my_tank.y , smal=tank->y , i;
				if(my_tank.x < tank->x)
				{
					big=tank->x;
					smal=my_tank.x;
				}
				for(i=smal+2;i<=big-2;i++)  //判断AI炮口的直线上两坦克间有无障碍
					if(map[tank->y][i]!=0 || map[tank->y][i]!=5)      //若有障碍
						break;
				if(i==big-1)   //若i走到big-1说明无障碍
				{
					BuildBullet(*tank);     //则发射子弹
					tank->CD=0;
				}
			}
		}
	}
}
 
 
 
void BuildBullet(Tank tank)  //子弹发射（建立）,传入结构体Tank,这里包含改变了全局变量结构体bullet
{                            //∵实现方式为顺序循环建立子弹，每次调用改变的bullet数组元素都不同
	switch(tank.direction)   //∴为了方便,不将bullet放入参数,bullet作为全局变量使用
	{
    	case UP    :
				bullet [bul_num].x = tank.x;
				bullet [bul_num].y = tank.y-2;
				bullet [bul_num].direction=1;
				break;
		case DOWN  :
				bullet [bul_num].x = tank.x;
				bullet [bul_num].y = tank.y+2;
				bullet [bul_num].direction=2;
				break;
		case LEFT  :
				bullet [bul_num].x = tank.x-2;
				bullet [bul_num].y = tank.y;
				bullet [bul_num].direction=3;
	    		break;
		case RIGHT :
				bullet [bul_num].x = tank.x+2;                   		          
				bullet [bul_num].y = tank.y; 
				bullet [bul_num].direction=4;
				break; 
	}     
	bullet [bul_num].exist = 1;    //子弹被建立,此值为1则此子弹存在
	bullet [bul_num].initial = 1;  //子弹处于初建立状态
	bullet [bul_num].my=tank.my;   //如果是我的坦克发射的子弹bullet.my=1，否则为0
	bul_num++;
	if(bul_num==BULLET_NUM)        //如果子弹编号增长到20号，那么重头开始编号
		bul_num=0;                 //考虑到地图上不可能同时存在20颗子弹，所以数组元素设置20个
}
 
 
void BulletFly(Bullet bullet[BULLET_NUM]) //子弹移动和打击
{
    int i,j;                                     //含有全局变量Bullet的改变
    int collide;
	for(i =0; i<BULLET_NUM;i++)
	{
		if(bullet [i].exist)              //如果子弹存在
		{   
			if(bullet [i].initial==0)     //如果子弹不是初建立的
			{                           
				if(map[bullet[i].y] [bullet[i].x]==0 || map[bullet[i].y] [bullet[i].x]==5)   //如果子弹坐标当前位置无障碍
					ClearBullet( bullet[i].x , bullet[i].y , BulletCheak(bullet[i].x , bullet[i].y ));     //抹除子弹图形
    			switch(bullet [i].direction)                                      //然后子弹坐标变化（子弹变到下一个坐标）
				{
        			case UP    :(bullet [i].y)--;break;
        			case DOWN  :(bullet [i].y)++;break;
        			case LEFT  :(bullet [i].x)--;break;
        			case RIGHT :(bullet [i].x)++;break;
				}
			}
			collide = BulletCheak ( bullet [i].x , bullet [i].y );   //判断子弹当前位置情况,判断子弹是否碰撞,是否位于水面上。
			if( collide )                                                //如果检测到当前子弹坐标无障碍(无碰撞)（包括在地面上与在水面上）
				PrintBullet( bullet[i].x , bullet[i].y , collide);       //则打印子弹，若有碰撞则不打印
			else
				BulletHit( & bullet [i] );     //若有碰撞则执行子弹碰撞函数                  
			if(bullet [i].initial)             //若子弹初建立，则把初建立标记去除
				bullet [i].initial = 0;
			for(j=0; j< BULLET_NUM ; j++)  //子弹间的碰撞判断,若是我方子弹和敌方子弹碰撞则都删除,若为两敌方子弹则无视
				if(bullet [j].exist && j!=i && (bullet[i].my || bullet[j].my) && bullet[i].x==bullet[j].x && bullet[i].y==bullet[j].y)
				{                              //同样的两颗我方子弹不可能产生碰撞
					bullet [j].exist=0;
					bullet [i].exist=0;
					ClearBullet( bullet[j].x , bullet[j].y , BulletCheak(bullet[j].x , bullet[j].y ));  //抹除j子弹图形,子弹i图形已被抹除
					break;
				}
		}
	}
}
 
 
void BulletHit(Bullet* bullet)  //含有Tank全局变量的修改,子弹间的碰撞不在本函数,子弹间碰撞已在BulletShoot中检测并处理
{                               //∵每次打中的坦克都不一样，不可能把所有坦克放在参数表中
	int i,num;
	int x=bullet->x;            //∴这里的Tank使用全局变量
	int y=bullet->y;            //这里传入的值是子弹坐标,这两个值不需要改变
	if(map[y][x]==1 || map[y][x]==2)  //子弹碰到砖块
	{
		if(bullet->direction==UP || bullet->direction==DOWN)   //如果子弹是纵向的
			for(i = -1 ; i<=1 ; i++)
            	if(map[y][x+i]==1 || map[y][x+i]==2)  //如果子弹打中砖块两旁为砖块,则删除砖,若不是(一旁为坦克或其他地形)则忽略
				{
		    		map[y][x+i]=0;    //砖块碎
                 	GoToxy(2*x+2*i,y);
		    		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED); //背景黑色
                 	printf("  ");
				}
		if(bullet->direction==LEFT || bullet->direction==RIGHT)     //若子弹是横向的  (与子弹纵向实现同理)
			for(i = -1 ; i<=1 ; i++)
				if(map[y+i][x]==1 || map[y+i][x]==2)
				{
		    		map[y+i][x]=0;
                 	GoToxy(2*x,y+i);
	    			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED); //背景黑色
                 	printf("  "); 
				}
		bullet->exist=0;           //这颗子弹已经不存在了
	}
	else if(map[y][x]==4 || map[y][x]==6 )  //子弹碰到边框或者不可摧毁方块
        bullet->exist=0;
	else if(bullet->my && map[y][x]>=100 && map[y][x]<104 )  //若我的子弹碰到了敌方坦克
	{
		num = map[y][x]%100;   //map[y][x]%100 等同于 tank.num ，可通过map值读取该坦克信息
		if(AI_tank[num].model==3 && AI_tank[num].color==2)   //若为firm tank,且color==2。该坦克为绿色,表明没有受到伤害
				AI_tank[num].color=3;                        //则变成黄色，color=3为黄色
		else if (AI_tank[num].model==3 && AI_tank[num].color==3)
				AI_tank[num].color=4;                        //4为红色
		else                       //其他类型的坦克或者firm tank为红色的情况
		{
			AI_tank[num].alive=0;
			ClearTank(AI_tank[num].x , AI_tank[num].y);      //清除该坦克
		}
		bullet->exist=0;
		score+=100;
		GoToxy(102,5);             //在副屏幕上打印出分数
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		printf("%d ",score);
	}
	else if(map[y][x]==200 && bullet->my==0 )   //若敌方子弹击中我的坦克
	{
		my_tank.alive=0;
		ClearTank(my_tank.x , my_tank.y);
		bullet->exist=0;
		my_tank.revive++;      //我的坦克复活次数+1(∵我的坦克复活次数与生命值有关∴放在这里自减)
		score-=100;            //分数减少
		GoToxy(102,5);         //在副屏幕上打印出分数
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		printf("%d   ",score);
		GoToxy(102,7);         //在副屏幕打印出我的剩余生命值
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
		printf("%d   ", MAX_LIFE-my_tank.revive);
	}
//	else if(bullet->my==0 && map[y][x]>=100 && map[y][x]<104) //敌方子弹击中敌方坦克,可以设置两种子弹运行方式,这种暂时不用
//		bullet->exist=0;
	else if(map[y][x]==9)      //子弹碰到家(无论是谁的子弹)
	{
		bullet->exist=0;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN);
		GoToxy(38,37);	 printf("      ");
		GoToxy(38,38);	 printf("◢◣  ");
		GoToxy(38,39);	 printf("███");
		GameOver(1);           //游戏结束,传入1代表老家被毁
	}
}
 
 
int BulletCheak (int x,int y)  //判断子弹当前位置情况,判断子弹是否碰撞,是否位于水面上。
{                              //有障碍返回0,无障碍且子弹在地面返回1，子弹在水面上返回2
	if(map[y][x]==0)
		return 1;
	else if(map[y][x]==5)
		return 2;
	else
		return 0;
}
 
 
void PrintBullet (int x,int y,int T)   //当前坐标BulletCheak 的值做参量 T
{
	if(T==1)          //  T==1 表示子弹当前坐标在陆地上
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
	else if(T==2)     //  T==2 表示子弹当前坐标在水面上
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_BLUE);
    GoToxy(2*x,y);
    printf("☉");
}
 
 
void ClearBullet(int x,int y,int T)   //当前坐标BulletCheak 的值做参量 T
{
	GoToxy(2*x,y);
	if(T==2)        //  T==2 表示子弹当前坐标在水面上
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|BACKGROUND_BLUE|FOREGROUND_BLUE|FOREGROUND_GREEN);
		printf("～");
	}
	else if(T==1)   //  T==1 表示子弹当前坐标在陆地上
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE);
		printf("  ");
	}
}
 
 
//position为坦克生成位置,-1为左位置,0为中间,1为右,2为我的坦克位置
void BuildAITank(int* position, Tank* AI_tank)   //执行一次该函数只建立一个坦克
{                                         //rand函数公式：0<=rand()%(a+1)<=a  0+m<=rand()%(n-m+1)+m<=n  
                                          //rand函数实现1到n：1<=rand()%(n)+1<=n
   	if(AIPositionCheak(*position))        //若此位置无障碍,可生成。position参数详见AIPositionCheak函数定义
	{
		AI_tank->x= 20 + 18*(*position);  //20 + 18 * position 对应三个生成位置的x假坐标
		AI_tank->y=2;
		if(AI_tank->revive==level_info[level-1].firm_tank_order)  //坦克出现(复活)次序==关卡信息(level_info)中firm tank的出现次序
		{
			AI_tank->model = 3;           //3为firm tank的模型(外观)
			AI_tank->color = 2;           //颜色参数2为绿色，具体详见函数ColorChoose
		}
		else if(AI_tank->revive==level_info[level-1].fast_tank_order)  //同上if，这里是fast_tank的
		{
			AI_tank->model = 2;
			AI_tank->color = rand()%6+1;  //若不是firm tank则随机颜色，颜色参数为1~6,分别代表不同颜色,详见函数ColorChoose
		}
		else      //普通坦克
		{
			AI_tank->model = 1;
	 	  	AI_tank->color = rand()%6+1;  //若不是firm tank则随机颜色
		}
		AI_tank->alive = 1;       //坦克变为存在
		AI_tank->direction = 2 ;  //方向朝下
		AI_tank->revive++;        //复活次数+1
		PrintTank(*AI_tank);
		(*position)++; 
		remain_enemy--;
		GoToxy(102,9);            //在副屏幕上打印剩余坦克数
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
		printf("%d ",remain_enemy);
		if(*position==2)          //position只能为0,1，-1，这里position循环重置
    		*position = -1;
  	   	return ;                  //若生成了一辆坦克，则结束该函数
	}
}
 
 
int AIPositionCheak( int position )    //position为坦克生成位置2为我的坦克位置，其余为AI位，-1为左位，0为中间位置，1为右位
{
	int	i,j,x,y;
	if(position==2)                    //2为我的坦克位置，现在暂时用不到
		x=15,y=38;
	else
    	y=2 , x= 20 + 18 * position ;  //20 + 18 * position 对应三个生成位置的x假坐标
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			if( map[y+j-1][x+i-1]!=0)  //如果遍历的九宫格里有障碍物
				return 0;              //则返回0，表示此生成位置有阻碍
    return 1;                          //否则生成1，表示此生成位置无阻碍
}
 
 
void MoveAITank(Tank* AI_tank) //AI专用函数，该函数主要为AI加强
{
    int j;
    if(AI_tank->alive)         //如果坦克活着
	{
		if(AI_tank->stop!=0)   //坦克是否停止运动的判断，若stop参数不为0
		{
			AI_tank->stop--;   //则此坦克本回合停止运动
			return;
		}
		if( !(rand()%23) )     //22分之1的概率执行方向重置
		{
			AI_tank->direction = rand()%4+1;
			if( rand()%3 )     //在方向重置后有2分之1的概率停止走动3步的时间
			{
				AI_tank->stop=2;
				return;
			}
		}
		ClearTank (AI_tank->x , AI_tank->y);
		if(TankCheck ( *AI_tank , AI_tank->direction))   //如果前方无障碍
			switch ( AI_tank->direction )
			{
	       		case UP   : AI_tank->y--; break;  //上前进一格
        		case DOWN : AI_tank->y++; break;  //下前进一格
         		case LEFT : AI_tank->x--; break;  //左前进一格
        		case RIGHT: AI_tank->x++; break;  //右前进一格
			}
		else                     //前方有障碍
		{
			if(!(rand()%4))      //3分之1的概率乱转
			{
				AI_tank->direction=rand()%4+1;
				AI_tank->stop=2; //乱转之后停止走动3步的时间
				PrintTank(*AI_tank);
				return;          //∵continue会跳过下面的打印函数,∴这里先打印
			}
			else                 //另外3分之2的几率选择正确的方向
			{
				
	    		for(j=1;j<=4;j++)
	        		if(TankCheck ( *AI_tank , j ))  //循环判断坦克四周有无障碍,此函数返值1为可通过
	    				break;
    			if(j==5)         //j==5说明此坦克四周都有障碍物，无法通行
				{
					PrintTank(*AI_tank);
	    			return;      //则跳过下面的while循环以防程序卡死
				}
    			while(TankCheck ( *AI_tank , AI_tank->direction) == 0)  //如果前方仍有障碍
	    			AI_tank->direction=(rand()%4+1);                    //则换个随机方向检测
			}
		}
		PrintTank(*AI_tank);     //打印AI坦克
	}
}
 
 
void BuildMyTank (Tank* my_tank) //建立我的坦克
{
	my_tank->x=15;
   	my_tank->y=38;
   	my_tank->stop=NULL;
   	my_tank->direction=1;
    my_tank->model=0;
    my_tank->color=1;
    my_tank->alive=1;
    my_tank->my=1;
	my_tank->CD=7;
    PrintTank (*my_tank) ;   //打印我的坦克
}
 
 
void MoveMyTank(int turn )   //玩家专用函数，turn为keyboard函数里因输入不同方向键而传入的不同的值
{
	ClearTank(my_tank.x , my_tank.y);        //map 数组中“我的坦克”参数清除工作已在此函数中完成
	if(my_tank.direction!=turn)
    	my_tank.direction=turn;                  //将键盘输入的方向值传入我的坦克方向值
	else if(TankCheck ( my_tank , my_tank.direction ))  //若此时我的坦克当前方向上无障碍
		switch (turn)
		{
			case UP   : my_tank.y--; break;  //上前进一格
			case DOWN : my_tank.y++; break;  //下前进一格
			case LEFT : my_tank.x--; break;  //左前进一格
			case RIGHT: my_tank.x++; break;  //右前进一格
	    }                                        //若坦克当前方向上有障碍则跳过坐标变化直接打印该转向的坦克
	PrintTank (my_tank);
}
 
 
int TankCheck(Tank tank,int direction)  //检测坦克前方障碍函数,参量为假坐标。返值1为可通过,返值0为阻挡(人机共用)
{
	switch(direction)                    //direction变量   1上，2下，3左，4右
	{
    	case UP:
			if (map[tank.y-2][tank.x]==0 && map[tank.y-2][tank.x-1]==0 && map[tank.y-2][tank.x+1]==0)
				return 1;
			else
				return 0;
        case DOWN:
			if (map[tank.y+2][tank.x]==0 && map[tank.y+2][tank.x-1]==0 && map[tank.y+2][tank.x+1]==0)
				return 1;
			else
				return 0;
		case LEFT:
			if (map[tank.y][tank.x-2]==0 && map[tank.y-1][tank.x-2]==0 && map[tank.y+1][tank.x-2]==0)
				return 1;
			else
				return 0;
		case RIGHT:
			if (map[tank.y][tank.x+2]==0 && map[tank.y-1][tank.x+2]==0 && map[tank.y+1][tank.x+2]==0)
				return 1;
			else
				return 0;
		default:
			printf("错误！！");
			Sleep(5000);
			return 0;
	}
}
 
 
void ClearTank(int x,int y)   //清除坦克函数（人机共用）
{
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{                     //将坦克占用的地图上的九格去掉
	     	map[y+j-1][x+i-1]=0;
	        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
			GoToxy(2*x+2*j-2,y+i-1);
			printf("  ");
		}
}
 
 
void PrintTank(Tank tank)     //打印坦克（人机共用） 由于读取的Tank参数较多,故就不将参数一一传入了
{                             // tank.color参数对应不同的颜色,范围 1 ~ 6
    int i,j;
    char** tankF[4];
    ColorChoose(tank.color);  //颜色选择函数   定义一个数组里装着字符指针(既装字符串)的数组指针(指向一维数组首地址的指针)
    for(i = 0;i < 4;i ++)
        tankF[i] = tank_figure[tank.model][i];  //将二维数组首址赋初值给数组指针 model==0为我的坦克,4为电脑1坦克,8为电脑2,类推
	for(i = 0; i < 3; i++)   
	{
        GoToxy((tank.x-1)*2 , tank.y-1+i);        //在坦克中心坐标的左边，上中下三行打印
        printf("%s", tankF[i][tank.direction-1]); //打印的是地址，地址既字符串
     	for(j=0;j<3;j++)
			if(tank.my)       //若为我的坦克
	            map[tank.y+j-1][tank.x+i-1]=200;  //在map上把"坦克"九格填满代表敌我坦克的参数。敌方此值为100~103,我方为200
			else
		    	map[tank.y+j-1][tank.x+i-1]=100+tank.num;  //这样可以通过map值读取坦克编号,读取操作在BulletHit 函数
	}
}
 
 
void HideCursor()  //隐藏光标
{                  //CONSOLE_CURSOR_INFO结构体包含控制台光标的信息,DWORD dwSize光标百分比厚度（1~100）和BOOL bVisible光标是否可见
	CONSOLE_CURSOR_INFO cursor_info={1,0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info); //SetConsoleCursorInfo用来设置指定的控制台光标的大小和可见性。
}
 
 
void GoToxy(int x,int y)  //光标移动函数，X表示横坐标，Y表示纵坐标。
{
	COORD  coord;         //使用头文件自带的坐标结构
    HANDLE a;
	coord.X=x;            //这里将int类型值传给short,不过程序中涉及的坐标值均不会超过short范围
	coord.Y=y;
	a=GetStdHandle(STD_OUTPUT_HANDLE);  //获得标准输出句柄
	SetConsoleCursorPosition(a,coord);         //以标准输出的句柄为参数设置控制台光标坐标
}
 
 
void ColorChoose(int color)   //颜色选择函数
{
	switch(color)
	{
	   	case 1:               //天蓝色(我的坦克颜色)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
			break;
		case 2:               //绿色
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);	
			break;
		case 3:               //黄色
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
			break;
		case 4:               //红色
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
			break;
		case 5:               //紫色
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
			break;
		case 6:               //白色
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN);
			break;
		case 7:               //深蓝色(∵颜色深难与黑色背景辨识度不高 ∴坦克颜色不选用此颜色),只用在字体颜色闪烁中
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE);
			break;
	}
}
 
 
int Stop()    //暂停
{
	int color=1,timing=0;
	while(1)
	{
		if(timing++%30==0)
		{
			ColorChoose(color);   //颜色选择
			GoToxy(100,13);       //副屏幕打印
			printf("游戏暂停");
			GoToxy(88,17);
			printf("按回车键回到游戏");
			GoToxy(88,18);
			printf("或按 Esc键退出游戏");
			if(++color==8)
				color=1;
		}
		if (GetAsyncKeyState( 0xD )& 0x8000)      //回车键
		{
			GoToxy(100,13);       //副屏幕打印
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
			printf("正在进行");   //覆盖掉原来的提示
			GoToxy(88,17);
			printf("                     ");
			GoToxy(88,18);
			printf("                     ");
			break;
		}
		else if(GetAsyncKeyState( 0x1B )& 0x8000) //Esc键退出	
			return -1;
		Sleep(20);
	}
	return 0;
}
 
 
void ClearMainScreen()  //主屏幕清屏函数，因使用system("cls");再打印框架有一定几率造成框架上移一行的错误，所以单独编写清屏函数
{
    int i;
	for(i=1;i<40;i++)
	{
		GoToxy(2,i);
		printf("                                                                              ");
	}
}
 
 
void Frame ()     //打印游戏主体框架
{                 //SetConsoleTextAttribute为设置文本颜色和文本背景颜色函数
	int i;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
	printf("  ▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁▁  ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_BLUE); 
	printf("  ▂▂▂▂▂▂▂▂▂▂▂▂▂ \n");
	for(i=0;i<14;i++)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_INTENSITY);
	    printf("▕                                                                              ▏");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_BLUE); 
		printf(" |                          |\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_INTENSITY);
	printf("▕                                                                              ▏");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_BLUE); 
	printf(" |═════════════|\n");
	for(i=0;i<24;i++)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_INTENSITY);
	    printf("▕                                                                              ▏");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_BLUE); 
		printf(" |                          |\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_INTENSITY);
	printf("  ▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔▔  ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY| FOREGROUND_BLUE); 
	printf(" ﹊﹊﹊﹊﹊﹊﹊﹊﹊﹊﹊﹊﹊﹊\n");
	SideScreen ();  //打印副屏幕
}
 
 
void PrintMap()     // 打印地图(地图既地图障碍物)
{
    int i,j;
	for(j=0;j<41;j++)
		for(i=0;i<41;i++)
			if(map[i][j]==6)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN
					|FOREGROUND_RED|FOREGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE);
				GoToxy(2*j,i);
				printf("■");
			}
			else if(map[i][j]==2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|BACKGROUND_GREEN|BACKGROUND_RED);
				GoToxy(2*j,i);
				printf("▓");
			}
			else if(map[i][j]==1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|BACKGROUND_GREEN|BACKGROUND_RED);
				GoToxy(2*j,i);
				printf("▓");
			}
			else if(map[i][j]==5)
			{                      
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|BACKGROUND_BLUE|FOREGROUND_BLUE|FOREGROUND_GREEN);
				GoToxy(2*j,i);
				printf("～");
			}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN);
	GoToxy(38,37);	 printf("◣★◢");
	GoToxy(38,38);	 printf("███");    //∵无论地图怎么变,家所在位置不变,且家的字符多种,不方便用上述方式打印
	GoToxy(38,39);	 printf("◢█◣");    //∴直接打印(且家的map值与符号无关)
}
 
 
void GetMap()      //地图存放函数
{	               //map里的值: 个位数的值为地图方块部分，百位数的值为坦克
	int i ,j;      //map里的值: 0为可通过陆地，1为红砖，2待定，5为水，100为敌方坦克，200为我的坦克，
	int Map[8][41][41]=
	{
		{
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4}, 
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4}, 
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4}, 
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4}, 
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4}, 
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,6,6,6,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,6,6,6,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,6,6,6,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4}, 
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4}, 
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,4}, 
			{4,6,6,6,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4}, 
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
		},
		{
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,2,2,2,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,2,2,2,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,2,2,2,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,4},
			{4,1,1,1,2,2,2,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,6,6,6,0,0,0,4},
			{4,1,1,1,2,2,2,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,6,6,6,0,0,0,4},
			{4,1,1,1,2,2,2,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,1,1,1,6,6,6,6,6,6,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,1,1,1,6,6,6,6,6,6,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,1,1,1,6,6,6,6,6,6,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,6,6,6,1,1,1,2,2,2,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,6,6,6,1,1,1,2,2,2,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,6,6,6,1,1,1,2,2,2,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,6,6,6,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,6,6,6,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,6,6,6,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,6,6,6,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,6,6,6,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,6,6,6,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,1,1,1,6,6,6,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,1,1,1,6,6,6,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,2,2,2,1,1,1,6,6,6,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,6,6,6,6,6,6,6,6,6,2,2,2,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,6,6,6,6,6,6,6,6,6,2,2,2,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,6,6,6,6,6,6,6,6,6,2,2,2,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,6,6,6,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,6,6,6,6,6,6,0,0,0,0,0,0,4},
			{4,6,6,6,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,6,6,6,6,6,6,0,0,0,0,0,0,4},
			{4,6,6,6,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,6,6,6,6,6,6,0,0,0,0,0,0,4},
			{4,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,1,1,1,1,1,1,0,0,0,4},
			{4,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,1,1,1,1,1,1,0,0,0,4},
			{4,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,1,1,1,1,1,1,0,0,0,4},
			{4,2,2,2,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,6,6,6,6,6,6,4},
			{4,2,2,2,6,6,6,6,6,6,6,6,6,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,6,6,6,6,6,6,4},
			{4,2,2,2,6,6,6,6,6,6,6,6,6,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,6,6,6,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,4},
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
		},
		{
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,1,1,1,6,6,6,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,1,1,1,6,6,6,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,6,6,6,0,0,0,1,1,1,5,5,5,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,5,5,5,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,5,5,5,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,5,5,5,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,5,5,5,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,5,5,5,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,1,1,1,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,1,1,1,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,6,6,6,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,5,5,5,0,0,0,1,1,1,1,1,1,4},
			{4,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,5,5,5,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,5,5,5,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},	
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
		},
		{
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,4},
			{4,6,6,6,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,1,1,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,5,5,5,0,0,0,0,1,1,1,0,0,0,6,0,0,0,0,0,6,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,4},
			{4,5,5,5,0,0,0,0,1,1,1,0,0,0,6,0,0,0,0,0,6,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,4},
			{4,5,5,5,0,0,0,0,1,1,1,0,0,0,6,0,0,0,0,0,6,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,4},
			{4,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,4},
			{4,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,4},
			{4,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,4},
			{4,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,4},
			{4,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,4},
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
		},
		{
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,4},
			{4,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,1,1,1,0,0,0,5,5,5,4},
			{4,0,0,0,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,1,1,1,0,0,0,5,5,5,4},
			{4,0,0,0,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,1,1,1,0,0,0,5,5,5,4},
			{4,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,4},
			{4,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,4},
			{4,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,0,0,0,5,5,5,1,1,1,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,0,0,0,5,5,5,1,1,1,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,4},
			{4,5,5,5,5,5,5,0,0,0,5,5,5,5,5,5,0,0,0,5,5,5,1,1,1,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,0,0,0,5,5,5,4},
			{4,0,0,0,5,5,5,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,4},
			{4,0,0,0,5,5,5,5,5,5,5,5,5,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,4},
			{4,0,0,0,5,5,5,5,5,5,5,5,5,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,5,5,5,0,0,0,5,5,5,5,5,5,4},
			{4,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,5,5,5,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
		},
		{
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,4},
			{4,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,1,1,1,1,0,0,0,1,1,0,4},
			{4,0,0,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,2,2,2,2,2,2,0,0,0,0,1,1,1,0,0,0,0,0,0,1,1,4},
			{4,0,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,2,2,2,2,2,2,2,2,0,0,0,1,1,0,0,0,0,0,0,0,1,1,4},
			{4,0,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,2,2,2,2,2,2,2,2,0,0,1,1,1,0,0,0,0,0,0,0,1,1,4},
			{4,1,1,0,0,0,0,0,0,0,0,0,0,1,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,1,1,4},
			{4,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,1,1,4},
			{4,1,1,0,0,0,0,0,0,0,0,0,0,0,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,0,0,0,0,0,0,0,0,1,1,4},
			{4,1,1,0,0,0,0,0,0,0,0,0,1,1,1,2,2,2,2,6,6,6,6,6,6,2,2,2,2,1,1,1,0,0,0,0,0,1,1,1,4},
			{4,1,1,0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,6,6,6,6,6,6,2,2,2,2,1,1,1,1,0,0,0,0,1,1,1,4},
			{4,1,1,0,0,0,0,0,0,0,1,1,1,1,1,2,2,2,2,6,6,6,6,6,6,2,2,2,2,1,1,1,1,0,0,0,1,1,1,1,4},
			{4,0,1,1,0,0,0,0,0,0,1,1,1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,1,1,0,0,1,1,1,1,4},
			{4,0,1,1,1,0,0,0,0,1,1,1,1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,1,1,1,1,1,1,1,1,4},
			{4,0,0,1,1,1,1,1,1,1,1,1,1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,1,1,1,1,1,1,1,1,4},
			{4,0,0,0,1,1,1,1,1,1,1,1,1,6,6,6,6,6,6,1,1,1,6,6,6,6,6,6,1,1,1,1,1,1,1,1,1,1,1,0,4},
			{4,0,0,0,1,1,1,1,1,1,1,1,1,6,6,6,6,6,6,1,1,1,6,6,6,6,6,6,1,1,1,1,1,1,1,1,1,1,1,0,4},
			{4,0,0,0,0,1,1,1,1,1,1,1,1,6,6,6,6,6,6,1,1,1,6,6,6,6,6,6,1,1,1,1,1,1,1,1,1,1,0,0,4},
			{4,0,0,0,0,0,1,1,1,1,1,1,1,6,6,6,0,0,0,1,1,1,0,0,0,6,6,6,1,1,1,1,1,1,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,6,6,6,0,0,0,1,1,1,0,0,0,6,6,6,1,1,1,1,1,1,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,6,6,6,0,0,0,1,1,1,0,0,0,6,6,6,1,1,1,1,1,1,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,4},
			{4,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1,6,6,6,6,6,6,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,6,6,6,6,6,6,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,4},
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
		},
		{
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
			{4,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,6,6,6,6,6,6,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,6,6,6,6,6,6,4},
			{4,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,0,0,0,6,6,6,6,6,6,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,1,1,1,0,0,0,0,0,0,0,0,0,6,6,6,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,1,1,1,0,0,0,0,0,0,0,0,0,6,6,6,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,1,1,1,0,0,0,0,0,0,0,0,0,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,4},
			{4,6,6,6,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,1,1,1,0,0,0,4},
			{4,6,6,6,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,1,1,1,0,0,0,4},
			{4,6,6,6,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,4},
			{4,0,0,0,6,6,6,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,4},
			{4,0,0,0,6,6,6,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,4},
			{4,0,0,0,6,6,6,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,1,1,1,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,4},
			{4,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,6,6,6,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,6,6,6,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,4},
			{4,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,1,1,1,6,6,6,0,0,0,6,6,6,0,0,0,0,0,0,1,1,1,4},
			{4,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,4},
			{4,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,4},
			{4,1,1,1,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,1,1,1,6,6,6,0,0,0,0,0,0,4},
			{4,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,4},
			{4,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,4},
			{4,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,4},
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
		},
		{
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,5,5,5,5,5,5,1,1,1,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,0,0,0,5,5,5,5,5,5,1,1,1,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,0,0,0,5,5,5,5,5,5,1,1,1,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,6,6,6,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,1,1,1,0,0,0,0,0,0,4},
			{4,0,0,0,6,6,6,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,6,6,6,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,6,6,6,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,5,5,5,5,5,5,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,5,5,5,5,5,5,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,5,5,5,5,5,5,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,6,6,6,4},
			{4,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,1,1,1,5,5,5,5,5,5,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,6,6,6,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,4},
			{4,6,6,6,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,4},
			{4,6,6,6,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,4},
			{4,0,0,0,0,0,0,0,0,0,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,5,5,5,5,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,0,0,0,0,0,0,1,1,1,0,0,0,1,1,1,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4},
			{4,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,4},
			{4,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,4},
			{4,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,9,9,9,1,1,0,0,0,0,0,0,0,1,1,1,6,6,6,0,0,0,4},
			{4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4}
		},
	};
		for(i=0;i<41;i++)
			for(j=0;j<41;j++)
					map[i][j]=Map[level-1][i][j];
	PrintMap();         //打印地图
}
 
 
int GameOver(int home)
{
	int timing=0,color=1;
	while(1)
	{
		if(timing++%30==0)         //游戏结束原因为生命值为0
		{
			ColorChoose(color);    //颜色选择
			if(home)               //游戏结束原因为老家被毁,则多打印一行字以提示玩家
			{
				GoToxy(37,19);     //主屏幕中心打印
				printf("老家被毁!");
			}
			GoToxy(37,20);         //主屏幕中心打印
			printf("游戏结束!");
			GoToxy(100,13);        //副屏幕打印
			printf("游戏结束");
			GoToxy(88,17);
			printf("请按回车键重新开始!");
			GoToxy(88,18);
			printf("或按 Esc键退出游戏!");
			if(++color==8)
				color=1;
		}
		if (GetAsyncKeyState( 0xD )& 0x8000)  //回车键
		{
//			system("cls");       //清屏,这里清屏后再次打印框架有一定几率造成框架上移一行的bug，因此选用自建清屏函数
//			Frame ();            //重新打印游戏框架
			score-=500;          //分数-500
			ClearMainScreen();   //主屏清屏函数，无需再次打印框架
			Initialize();        //从本关重新开始
			break;
		}
		else if(GetAsyncKeyState( 0x1B )& 0x8000)  //Esc键退出	
			return -1;
		Sleep(20);
	}
    return 0;
}
 
 
int NextLevel()
{
	int timing=0,color=1;
	level++;
	if(level<=MAX_LEVEL)
		while(1)
		{
			if(timing++%10==0)
			{
				ColorChoose(color);   //颜色选择   
				GoToxy(37,20);        //主屏幕中心打印
				printf("恭喜过关!");
				GoToxy(100,13);       //副屏幕打印
				printf("等待下关");
				GoToxy(87,17);
				printf("请按回车键进入下一关!");
				GoToxy(88,18);
				printf("或按 Esc键退出游戏!");
				if(++color==8)	
					color=1;
			}
			if (GetAsyncKeyState( 0xD )& 0x8000)  //回车键
			{
				GoToxy(88,17);        //抹除副屏幕中的提示
				printf("                     ");
				GoToxy(88,18);
				printf("                     ");
				ClearMainScreen();   //主屏清屏函数，无需再次打印框架
				Initialize();        //初始化从下一关开始,level已++
				break;
			}
			else if(GetAsyncKeyState( 0x1B )& 0x8000)  //Esc键退出	
				return -1;
			Sleep(20);
		}
	else   //level>8 通关
		while(1)
		{
			if(timing++%5==0)
			{
				ColorChoose(color);
				GoToxy(33,20);        //主屏幕中心打印
				printf("恭喜通过全部关卡!");
				GoToxy(100,13);       //副屏幕打印
				printf("已通全关");
				GoToxy(88,17);
				printf("恭喜通过全部关卡!");
				GoToxy(88,19);
				printf("按 Esc键退出游戏!");
				if(++color==8)	
					color=1;
			}
			if(GetAsyncKeyState( 0x1B )& 0x8000)  //Esc键退出	
				return -1;
			Sleep(10);
		}
	return 0;
}
 
 
void GameCheak()
{                           //剩余敌人为0且四坦克全部不存活
	if(remain_enemy<=0 && !AI_tank[0].alive && !AI_tank[1].alive && !AI_tank[2].alive && !AI_tank[3].alive )
		NextLevel();        //进入下一关
	if(my_tank.revive>=MAX_LIFE)   //我的生命值(复活次数)全部用完 MAX_LIFE
		GameOver(0);        //游戏结束，传入0代表我的复活次数用光(生命值为0)。游戏结束有两种判断，另一种是老家被毁
}
 
 
void SideScreen ()  //副屏幕 行(84起,110末,若双字符宽则在108打印最后一个字）列(11上屏末,13下屏初,39下屏末。为美观最好打在38)
{                   // |         第  d  关         |   " |                          |\n"
	GoToxy(93,2);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED);
	printf("第     关");
	GoToxy(92,5);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE);
	printf("分  数：");
	GoToxy(92,7);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	printf("生  命：");
	GoToxy(86,9);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
	printf("剩余敌方坦克：");
	GoToxy(86,11);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
	printf("当前游戏速度：  %d",21-speed);
	GoToxy(86,13);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
	printf("当前游戏状态：");
	GoToxy(94,19);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED);
	GoToxy(94,24);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED);
	printf("帮  助");
	GoToxy(86,27);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	printf("方向键  ←↑→↓  移动");
	GoToxy(93,29);
	printf("x 键 射击");
	GoToxy(89,31);
	printf("+ - 调整游戏速度");
	GoToxy(90,33);
	printf("游戏速度范围1~20");
	GoToxy(90,35);
	printf("回车键 暂停游戏");
	GoToxy(90,37);
	printf("Esc键  退出游戏");
/*	printf("帮  助");     //这是第二种详细说明的样式
	GoToxy(86,21);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	printf("方向键  ←↑→↓  移动");
	GoToxy(93,23);
	printf("x 键 射击");
	GoToxy(89,25);
	printf("+ - 调整游戏速度");
	GoToxy(90,27);
	printf("游戏速度范围1~20");
	GoToxy(90,29);
	printf("回车键 暂停游戏");
	GoToxy(90,31);
	printf("Esc键  退出游戏");
	GoToxy(86,33);
	printf("敌方坦克全部消灭则过关");
	GoToxy(87,34);
	printf("己方坦克生命值为0 或");
	GoToxy(86,35);
	printf("正下方的老家被毁则失败");
	GoToxy(86,36);
	printf("己坦克与敌坦克子弹碰撞");
	GoToxy(87,37);
	printf("则抵消，敌坦克间子弹碰");
	GoToxy(86,38);
	printf("撞不抵消且可穿过敌坦克");*/
}
 
 
void Initialize()      //初始化
{
    int i;
    remain_enemy=16;
	my_tank.revive=0;  //我的坦克复活次数为0
	position=0;
	bul_num=0;
	GetMap();
	BuildMyTank( &my_tank );
	for(i=0;i<12;i++)     //子弹初始化
	{
		bullet [i].exist=0;
		bullet [i].initial=0;
	}
	for(i=0;i<=3;i++)         //AI坦克初始化
	{
		AI_tank [i].revive=0;
		AI_tank [i].alive=0;  //初始化坦克全是不存活的，BuildAITank()会建立重新建立不存活的坦克
		AI_tank [i].stop=0;
		AI_tank [i].num=i;
		AI_tank [i].my=0;
		AI_tank [i].CD=0;
	}
	GoToxy(97,2);                        //在副屏幕上关卡数
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
	printf("%d",level);
	GoToxy(102,5);                       //在副屏幕上打印分数
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE);
	printf("%d   ",score);
	GoToxy(102,7);                       //在副屏幕打印我的剩余生命值
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	printf("%d", MAX_LIFE-my_tank.revive);
	GoToxy(102,9);                       //在副屏幕上打印剩余坦克数
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
	printf("%d ",remain_enemy);
	GoToxy(100,13);                      //在副屏幕上打印状态
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_GREEN);
	printf("正在游戏");
}


int tank_main (int argc,char **argv)                               //主函数
{
	int i,res;
	HANDLE hConsoleOutput;
    CONSOLE_SCREEN_BUFFER_INFO info;
    
	unsigned int interval[12] = {1,1,1,1,1,1,1,1,1,1,1,1};  //间隔计数器数组，用于控制速度
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // 获取控制台输出句柄
    GetConsoleScreenBufferInfo(hConsoleOutput, &info);
	srand(time(NULL)); //设置随机数种子(若不设置种子而调用rand会使每次运行的随机数序列一致)随机数序列指:如首次调用rand得到1,第二次得2,第三次3,则此次随机数序列为1,2,3
	HideCursor();                         //隐藏光标
	system("mode con cols=112 lines=42"); //控制窗口大小
	Frame ();                             //打印游戏主体框架
	Initialize();                         //初始化，全局变量level初值便是1 
	for(;;)
	{
		if(interval[0]++%speed==0)        //速度调整用,假设interval[0]为a, 语句意为 a % 2==0,a=a+1; 
		{
			GameCheak();                  //游戏胜负检测
			BulletFly ( bullet );
			for(i=0 ; i<=3 ; i++)         //AI坦克移动循环
			{
				if(AI_tank[i].model==2 && interval[i+1]++%2==0) //四个坦克中的快速坦克单独使用计数器1,2,3,4
					MoveAITank( & AI_tank[i]);
				if(AI_tank[i].model!=2 && interval[i+5]++%3==0) //四个坦克中的慢速坦克单独使用计数器5,6,7,8
					MoveAITank( & AI_tank[i]);
			}
            
			for(i=0;i<=3;i++)                                   //建立AI坦克部分
	  		   	if(AI_tank[i].alive==0 && AI_tank[i].revive<4 && interval[9]++%90==0)  //一个敌方坦克每局只有4条命
				{                                               //如果坦克不存活。计时,每次建立有间隔  1750 ms
             	  	BuildAITank( &position, & AI_tank[i] );     //建立AI坦克（复活）
			  		break;                                      //每次循环只建立一个坦克
		  		}
			for(i=0;i<=3;i++)
				if(AI_tank[i].alive)
					BuildAIBullet(&AI_tank[i]);                 //AIshoot自带int自增计数CD,不使用main中的CD interval
			if(my_tank.alive && interval[10]++%2==0 )
            {
                res = keyboard();
                if(res != 0)
                    break;
                fflush(stdin);
            }         
            
    		if(my_tank.alive==0 && interval[11]++%30==0 && my_tank.revive < MAX_LIFE)
     			BuildMyTank( &my_tank );
		}
		Sleep(5);
	}
    system("mode con cols=100 lines=50");//设置窗口大小
    SetConsoleTextAttribute(hConsoleOutput, info.wAttributes);
    system("cls"); // 清屏
	fflush(stdin);
	return 0;
}

#endif
