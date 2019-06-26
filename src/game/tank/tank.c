
#include <stdio.h>
#include <windows.h>
#include <time.h> 
                           //���񣺳�39*2=78 �������꣩(�������Ϊ39)  ��39
                           //���񣺳�41*2=82 �������꣩(�������Ϊ41)  ��41
#define UP    1
#define DOWN  2
#define LEFT  3
#define RIGHT 4
#define MAX_LEVEL 8
#define BULLET_NUM 40
#define MAX_LIFE 4
 
//������δд�뺯������������δ˵���ı���ֻ��map��ά����,level_info�����level   
 
/*
      �˳������漰��x,y�������ֵ����Ϊ�������֣�                                 
                                                                                                  
�����꣺���������ָ������һ��������Ϊ��λ�����꣬������������coord���� (����map���������)                             
                                                                                                  
�����꣺ͷ�ļ��Դ�������ṹcoord�е����꣨Ҳ����˵�ǿ���̨�����������ֵ��                                 
                                                                                                  
  ����������y��ֵһ�£��ٺ�����xֵ������coord�����꣨�����꣩��ϵ�� x * 2 = coord ������    
                                                                                                            
		  coord�������ָGoTo�����е�x����,��Ϊ��������Ϸ������һ��������Ϊ������λ��                    
                                                                                                  
		  ����˵�漰��coord������ȫ��ż�����ȼ�����Ҫ�������꣨����������ܷ����������ã��������������2                                    
                                                           
*/
typedef struct             //����ĳ��ִ���ָ����һ��AI_tank�����еĴ�����Ϸ�����ĸ�AI_tank����
{                          //���趨ÿ��AI_tankÿ������̹��ֻ����һ�� ��fast_tank & firm_tank �����ִ���������1
    int fast_tank_order;   //fast_tank���ֵĴ���(�ڵ�fast_tank_order�θ������,�ӵ�0�ο�ʼ)����ÿ��AI_tankֻ����һ��
    int firm_tank_order;   //firm_tank���ֵĴ���ͬ��
}LevInfo;                 //�ؿ���Ϣ(׼ȷ˵�Ǹùس��ֵ�̹����Ϣ)
LevInfo level_info [MAX_LEVEL] = {{-1,-1},{3,-1},{-1,3},{2,3},{2,3},{2,3},{2,3},{2,3}};   //��ʼ����-1����û�и�����̹��
 
 
typedef struct      //�ӵ��ṹ��
{
    int x,y;        //�ӵ�����,������
    int direction;  //�ӵ��������
    int exist;     //�ӵ��������ı���,1Ϊ���ڣ�0������
    int initial;   //�ӵ��Ƿ��ڽ�����״̬��ֵ��1Ϊ���ڽ�����״̬��0Ϊ���ڷǽ�����״̬
    int my;        //����AI�ӵ�������ӵ��ı��,0ΪAI�ӵ���1Ϊ��ң��ҵģ��ӵ�
}Bullet;

static Bullet bullet[BULLET_NUM];  //���ǵ���ͼ�ϲ�̫����ͬʱ����20���ӵ�����������Ԫ������20��
 
#if 1
 
typedef struct      //̹�˽ṹ��
{
    int x,y;        //̹����������
    int direction;  //̹�˷���
    int color;      //��ɫ�η�������1��6�ֱ����ͬ��ɫ��������PrintTank����������˵��
    int model;      //̹��ͼ��ģ�ͣ�ֵΪ1,2,3���ֱ����ͬ��̹��ͼ��,0Ϊ�ҵ�̹��ͼ����AI����ʹ��
    int stop;       //ֻ����AI̹��ʹ�õĲ�������0����̹��ֹͣ�߶�,0Ϊ�����߶�
    int revive;     //̹�˸������
    int num;        //AI̹�˱�ţ��̶�ֵ��Ϊ��������ʼ�������ж��£�0~3
    int CD;         //�����ӵ���ȴ��ʱ
    int my;        //�Ƿ�з�̹�˲������ҵ�̹�˴˲���Ϊ1,Ϊ����
    int alive;     //���Ϊ1�������Ϊ0
}Tank;
static Tank AI_tank[4] , my_tank;  //my_tankΪ�ҵ�̹�ˣ�Ai_tank ����AI̹��
//�����еĺ������п��ܶ�ȫ�ֱ���map���ж�д(�ı�)��
//�ຯ���в���˵���Ƿ���ȫ�ֱ���map��д
//������������Ϸ��������
static void GoToxy(int x,int y);    //����ƶ�
static void HideCursor();           //���ع��
static int keyboard ();            //���ܼ�������
static void Initialize();           //��ʼ��(���жԶ�����ݵĶ�д)
static int Stop();                 //��ͣ
static void Getmap();               //��ͼ���ݴ�����ȡ
static void Frame ();               //��ӡ��Ϸ������
static void PrintMap();             //��ӡ��ͼ(��ͼ�ȵ�ͼ�ϰ���)(����level�Ķ�ȡ)
static void SideScreen ();          //����Ļ��ӡ
static void GameCheak();            //�����Ϸ��Ӯ
static int GameOver(int home);  //��Ϸ����
static void ClearMainScreen();      //����Ļ����������system("cls")���ӡ�����һ��������ɿ������һ�еĴ���൥����д��������
static void ColorChoose(int color); //��ɫѡ����
static int NextLevel();            //��һ��(���ж�levelȫ�ֱ����Ķ�д)
 
//�ӵ�����
static void BuildAIBullet(Tank *tank);                //AI̹�˷����ӵ������ж�my_tank�Ķ�ȡ,ֻ��ȡ��my_tank���꣩
static void BuildBullet(Tank tank);                 //�ӵ����䣨���������˻����ã�(��ȫ�ֱ���bullet���޸�)�ҵ�̹�˷����ӵ�ֱ�ӵ��øú���,AIͨ��AIshoot��ӵ���
static void BulletFly(Bullet bullet[BULLET_NUM]); //�ӵ��ƶ��ʹ�����˻����ã�,
static void BulletHit(Bullet* bullet);            //�ӵ���ײ���˻����ã�(��Tankȫ�ֱ������޸�)��ֻͨ��BulletFly���ã��ӵ������ײ���ڱ�����,�ӵ�����ײ����BulletShoot�м�Ⲣ����
static void PrintBullet(int x,int y,int T);         //��ӡ�ӵ����˻����ã�
static void ClearBullet(int x,int y,int T);         //����ӵ����˻����ã�
static int  BulletCheak(int x,int y);               //�ж��ӵ�ǰ��������˻����ã�
 
//̹�˲���
static void BuildAITank (int* position, Tank* AI_tank); //����AI̹��
static void BuildMyTank (Tank* my_tank);                //�����ҵ�̹��
static void MoveAITank  (Tank* AI_tank);                //AI̹���ƶ�
static void MoveMyTank  (int turn);                     //�ҵ�̹���ƶ���ֻͨ��keyboard�������ã��ȼ��̿���
static void ClearTank   (int x,int y);                  //���̹�ˣ��˻����ã�
static void PrintTank   (Tank tank);                    //��ӡ̹�ˣ��˻����ã�
static int TankCheck   (Tank tank,int direction);      //���̹��dirtection������ϰ���,��ֵ1�谭,0 ��ͨ
static int  AIPositionCheak (int position);           //���AI̹�˽���λ���Ƿ����ϰ���AIPositionCheak
 
//DWORD WINAPI InputX(LPVOID lpParameter); //�����̺߳��������ڼ��X�����벢����X����������ȴʱ��
 
 
//ע��map����Ӧ����������ǰ���������ں󣬼�map[y][x]��(�������г�����ǰ���г����ں�)
//map���ֵ: ��λ����ֵΪ��ͼ���鲿�֣���λ����ֵΪ̹��,�ӵ���map��û��ֵ(�ӵ�������һ��������)
//map���ֵ: 0Ϊ��ͨ��½�أ�1Ϊ��ש��2��ש��5Ϊˮ��100~103Ϊ�з�̹�ˣ�200Ϊ�ҵ�̹�ˣ�
 
//ȫ�ֱ���
static int map[41][41];  //��ͼ��ά����
static int key_x;        // X���Ƿ񱻡����롱�ı�����Ҳ���ӵ��Ƿ���Է���ı䣬
static int bul_num;      //�ӵ����
static int position;     //λ�ü���,��ӦAI̹������λ��,-1Ϊ��λ��,0Ϊ�м�,1Ϊ��,2Ϊ�ҵ�̹��λ��
static int speed=5;      //��Ϸ�ٶ�,������
static int level=1;      //��Ϸ�ؿ���
static int score=0;      //��Ϸ����
static int remain_enemy; //ʣ�����(δ���ֵĵ���)
 
static char* tank_figure[4][3][4]=
{
  {
    {"������", "������", "���ר�", "���ר�"},
    {"�ǡ��", "�ǡ��", "����", "����"},
    {"������", "������", "���ߨ�", "���ߨ�"}
  }, 
  {
    {"������", "���ש�", "���ש�", "���ש�"},
    {"�ǡ��", "�ǡ��", "�����", "�ǡ�"},
    {"���ߩ�", "������", "���ߩ�", "���ߩ�"}
  }, 
  {
    {"������", "������", "���ר�", "���ש�"},
    {"�ǡ��", "�ǡ��", "����", "����"},
    {"������", "������", "���ߨ�", "���ߩ�"}
  },
  {
    {"�X���[", "�X�j�[", "�X�j�[", "�X�j�["},
    {"�d���g", "�d���g", "�����g", "�d����"},
    {"�^�m�a", "�^���a", "�^�m�a", "�^�m�a"}
  }
};
 
 
static int keyboard ()
{               // kbhit()   getch()  �÷����õ��ǲ�����            
/* 
   ��������:�ú����ж��ڴ˺���������ʱ,ĳ�����Ǵ���UP״̬���Ǵ���DOWN״̬,��ǰ�ε���GetAsyncKeyState������,
   �Ƿ񰴹��˼�.�������ֵ�����λ����λ,��ô�ü�����DOWN״̬;������λ����λ,��ô��ǰһ�ε��ô˺�����,�˼�������,
   �����ʾ�ü�û������.
   ����GetAsyncKeyState�� kbhit() + getch() ����,������˳��.   GetAsyncKeyState�ķ���ֵ��ʾ�������ݣ�
   һ�������λbit��ֵ������������Ƿ񱻰��¡�һ�������λbit��ֵ,�����ϴε���GetAsyncKeyState��������Ƿ񱻰��¡�
   &Ϊ�������&0x8000�����ж��������ֵ�ĸ�λ�ֽڡ����high-order bit��1,���ǰ���״̬�������ǵ���״̬��Ϊ0
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
	else if (GetAsyncKeyState( 0x1B )& 0x8000)  // Esc��
		return 1;                                //�˳�������
	else if (GetAsyncKeyState( 0x20 )& 0x8000)  //�ո�
		return Stop();
	else if (count++%7==0)            //������Ӽ�������Ϊ�˷�ֹ����ճ�����ܴﵽ΢��Ч��
	{
		if (speed>1 && GetAsyncKeyState( 0x6B )& 0x8000)   // +��
		{
			speed--;
			GoToxy(102,11);           //�ڸ���Ļ��ӡ����ǰ�ٶ�
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_RED);
			printf("%d ",21-speed);   //����Ļ��ʾ���ٶ�Ϊ1~10
		}
		else if (speed<20 && GetAsyncKeyState( 0x6D )& 0x8000)  // - ��
		{
			speed++;
			GoToxy(102,11);           //�ڸ���Ļ��ӡ����ǰ�ٶ�
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_RED);
			printf("%d ",21-speed);   //����Ļ��ʾ���ٶ�Ϊ1~10
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
 
 
 
void BuildAIBullet(Tank *tank)   //AI�ӵ�����(����)���ж�my_tank�Ķ�ȡ
{
    int i,big,smal;
	if(tank->CD==15)
	{
		if(!(rand()%11))     //��ȴ������������ÿ����Ϸ��������10��֮һ�Ŀ��ܷ����ӵ�
		{
			BuildBullet(*tank);
			tank->CD=0;
		}
	}
	else
		tank->CD++;
	if(tank->CD >= 14)       //AIǿ�����֣�����ȴ����һ����Χ����ʹ��
	{
		if(tank->y==38 )     //���̹���ڵײ�(���������)
		{
			if(tank->x < 20) //���ϼ����
			{
				if(tank->direction==RIGHT)  //̹�˷�����
				{
					BuildBullet(*tank);     //�����ӵ�
					tank->CD=0;
				}
			}
			else             //���ϼ��ұ�
				if(tank->direction==LEFT)   //̹�˷�����
				{
					BuildBullet(*tank);     //�����ӵ�
					tank->CD=0;
				}
		}
		else if(tank->x==my_tank.x+1 || tank->x==my_tank.x || tank->x==my_tank.x-1)  //AI̹����������"�ڿ�"��׼�ҵ�̹��
		{
			if(tank->direction==DOWN && my_tank.y > tank->y || tank->direction==UP && my_tank.y < tank->y)
			{                               //����AI���²����ҵ�̹����AI̹���·�(��ֵ���������)����AI�����ҵ�̹����AI�Ϸ�
				big=my_tank.y , smal=tank->y , i; 
				if(my_tank.y < tank->y)
				{
					big=tank->y;
					smal=my_tank.y;
				}
				for(i=smal+2;i<=big-2;i++)  //�ж�AI�ڿڵ�ֱ������̹�˼������ϰ�
					if(map[i][tank->x]!=0 || map[i][tank->x]!=5)      //�����ϰ�
						break;
				if(i==big-1)                //��i�ߵ�big-1˵�����ϰ�
				{
					BuildBullet(*tank);     //�����ӵ�
					tank->CD=0;
				}
			}
		}
		else if(tank->y==my_tank.y+1 || tank->y==my_tank.y || tank->y==my_tank.y-1) //AI̹���ں�����"�ڿ�"��׼�ҵ�̹��
		{
			if(tank->direction==RIGHT && my_tank.x > tank->x || tank->direction==LEFT && my_tank.x < tank->x)
			{                  //����AI���Ҳ����ҵ�̹����AI̹���ҷ�(��ֵ���������)����AI�����ҵ�̹����AI��
				big=my_tank.y , smal=tank->y , i;
				if(my_tank.x < tank->x)
				{
					big=tank->x;
					smal=my_tank.x;
				}
				for(i=smal+2;i<=big-2;i++)  //�ж�AI�ڿڵ�ֱ������̹�˼������ϰ�
					if(map[tank->y][i]!=0 || map[tank->y][i]!=5)      //�����ϰ�
						break;
				if(i==big-1)   //��i�ߵ�big-1˵�����ϰ�
				{
					BuildBullet(*tank);     //�����ӵ�
					tank->CD=0;
				}
			}
		}
	}
}
 
 
 
void BuildBullet(Tank tank)  //�ӵ����䣨������,����ṹ��Tank,��������ı���ȫ�ֱ����ṹ��bullet
{                            //��ʵ�ַ�ʽΪ˳��ѭ�������ӵ���ÿ�ε��øı��bullet����Ԫ�ض���ͬ
	switch(tank.direction)   //��Ϊ�˷���,����bullet�������,bullet��Ϊȫ�ֱ���ʹ��
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
	bullet [bul_num].exist = 1;    //�ӵ�������,��ֵΪ1����ӵ�����
	bullet [bul_num].initial = 1;  //�ӵ����ڳ�����״̬
	bullet [bul_num].my=tank.my;   //������ҵ�̹�˷�����ӵ�bullet.my=1������Ϊ0
	bul_num++;
	if(bul_num==BULLET_NUM)        //����ӵ����������20�ţ���ô��ͷ��ʼ���
		bul_num=0;                 //���ǵ���ͼ�ϲ�����ͬʱ����20���ӵ�����������Ԫ������20��
}
 
 
void BulletFly(Bullet bullet[BULLET_NUM]) //�ӵ��ƶ��ʹ��
{
    int i,j;                                     //����ȫ�ֱ���Bullet�ĸı�
    int collide;
	for(i =0; i<BULLET_NUM;i++)
	{
		if(bullet [i].exist)              //����ӵ�����
		{   
			if(bullet [i].initial==0)     //����ӵ����ǳ�������
			{                           
				if(map[bullet[i].y] [bullet[i].x]==0 || map[bullet[i].y] [bullet[i].x]==5)   //����ӵ����굱ǰλ�����ϰ�
					ClearBullet( bullet[i].x , bullet[i].y , BulletCheak(bullet[i].x , bullet[i].y ));     //Ĩ���ӵ�ͼ��
    			switch(bullet [i].direction)                                      //Ȼ���ӵ�����仯���ӵ��䵽��һ�����꣩
				{
        			case UP    :(bullet [i].y)--;break;
        			case DOWN  :(bullet [i].y)++;break;
        			case LEFT  :(bullet [i].x)--;break;
        			case RIGHT :(bullet [i].x)++;break;
				}
			}
			collide = BulletCheak ( bullet [i].x , bullet [i].y );   //�ж��ӵ���ǰλ�����,�ж��ӵ��Ƿ���ײ,�Ƿ�λ��ˮ���ϡ�
			if( collide )                                                //�����⵽��ǰ�ӵ��������ϰ�(����ײ)�������ڵ���������ˮ���ϣ�
				PrintBullet( bullet[i].x , bullet[i].y , collide);       //���ӡ�ӵ���������ײ�򲻴�ӡ
			else
				BulletHit( & bullet [i] );     //������ײ��ִ���ӵ���ײ����                  
			if(bullet [i].initial)             //���ӵ�����������ѳ��������ȥ��
				bullet [i].initial = 0;
			for(j=0; j< BULLET_NUM ; j++)  //�ӵ������ײ�ж�,�����ҷ��ӵ��͵з��ӵ���ײ��ɾ��,��Ϊ���з��ӵ�������
				if(bullet [j].exist && j!=i && (bullet[i].my || bullet[j].my) && bullet[i].x==bullet[j].x && bullet[i].y==bullet[j].y)
				{                              //ͬ���������ҷ��ӵ������ܲ�����ײ
					bullet [j].exist=0;
					bullet [i].exist=0;
					ClearBullet( bullet[j].x , bullet[j].y , BulletCheak(bullet[j].x , bullet[j].y ));  //Ĩ��j�ӵ�ͼ��,�ӵ�iͼ���ѱ�Ĩ��
					break;
				}
		}
	}
}
 
 
void BulletHit(Bullet* bullet)  //����Tankȫ�ֱ������޸�,�ӵ������ײ���ڱ�����,�ӵ�����ײ����BulletShoot�м�Ⲣ����
{                               //��ÿ�δ��е�̹�˶���һ���������ܰ�����̹�˷��ڲ�������
	int i,num;
	int x=bullet->x;            //�������Tankʹ��ȫ�ֱ���
	int y=bullet->y;            //���ﴫ���ֵ���ӵ�����,������ֵ����Ҫ�ı�
	if(map[y][x]==1 || map[y][x]==2)  //�ӵ�����ש��
	{
		if(bullet->direction==UP || bullet->direction==DOWN)   //����ӵ��������
			for(i = -1 ; i<=1 ; i++)
            	if(map[y][x+i]==1 || map[y][x+i]==2)  //����ӵ�����ש������Ϊש��,��ɾ��ש,������(һ��Ϊ̹�˻���������)�����
				{
		    		map[y][x+i]=0;    //ש����
                 	GoToxy(2*x+2*i,y);
		    		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED); //������ɫ
                 	printf("  ");
				}
		if(bullet->direction==LEFT || bullet->direction==RIGHT)     //���ӵ��Ǻ����  (���ӵ�����ʵ��ͬ��)
			for(i = -1 ; i<=1 ; i++)
				if(map[y+i][x]==1 || map[y+i][x]==2)
				{
		    		map[y+i][x]=0;
                 	GoToxy(2*x,y+i);
	    			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED); //������ɫ
                 	printf("  "); 
				}
		bullet->exist=0;           //����ӵ��Ѿ���������
	}
	else if(map[y][x]==4 || map[y][x]==6 )  //�ӵ������߿���߲��ɴݻٷ���
        bullet->exist=0;
	else if(bullet->my && map[y][x]>=100 && map[y][x]<104 )  //���ҵ��ӵ������˵з�̹��
	{
		num = map[y][x]%100;   //map[y][x]%100 ��ͬ�� tank.num ����ͨ��mapֵ��ȡ��̹����Ϣ
		if(AI_tank[num].model==3 && AI_tank[num].color==2)   //��Ϊfirm tank,��color==2����̹��Ϊ��ɫ,����û���ܵ��˺�
				AI_tank[num].color=3;                        //���ɻ�ɫ��color=3Ϊ��ɫ
		else if (AI_tank[num].model==3 && AI_tank[num].color==3)
				AI_tank[num].color=4;                        //4Ϊ��ɫ
		else                       //�������͵�̹�˻���firm tankΪ��ɫ�����
		{
			AI_tank[num].alive=0;
			ClearTank(AI_tank[num].x , AI_tank[num].y);      //�����̹��
		}
		bullet->exist=0;
		score+=100;
		GoToxy(102,5);             //�ڸ���Ļ�ϴ�ӡ������
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		printf("%d ",score);
	}
	else if(map[y][x]==200 && bullet->my==0 )   //���з��ӵ������ҵ�̹��
	{
		my_tank.alive=0;
		ClearTank(my_tank.x , my_tank.y);
		bullet->exist=0;
		my_tank.revive++;      //�ҵ�̹�˸������+1(���ҵ�̹�˸������������ֵ�йء���������Լ�)
		score-=100;            //��������
		GoToxy(102,5);         //�ڸ���Ļ�ϴ�ӡ������
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
		printf("%d   ",score);
		GoToxy(102,7);         //�ڸ���Ļ��ӡ���ҵ�ʣ������ֵ
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
		printf("%d   ", MAX_LIFE-my_tank.revive);
	}
//	else if(bullet->my==0 && map[y][x]>=100 && map[y][x]<104) //�з��ӵ����ез�̹��,�������������ӵ����з�ʽ,������ʱ����
//		bullet->exist=0;
	else if(map[y][x]==9)      //�ӵ�������(������˭���ӵ�)
	{
		bullet->exist=0;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN);
		GoToxy(38,37);	 printf("      ");
		GoToxy(38,38);	 printf("����  ");
		GoToxy(38,39);	 printf("������");
		GameOver(1);           //��Ϸ����,����1�����ϼұ���
	}
}
 
 
int BulletCheak (int x,int y)  //�ж��ӵ���ǰλ�����,�ж��ӵ��Ƿ���ײ,�Ƿ�λ��ˮ���ϡ�
{                              //���ϰ�����0,���ϰ����ӵ��ڵ��淵��1���ӵ���ˮ���Ϸ���2
	if(map[y][x]==0)
		return 1;
	else if(map[y][x]==5)
		return 2;
	else
		return 0;
}
 
 
void PrintBullet (int x,int y,int T)   //��ǰ����BulletCheak ��ֵ������ T
{
	if(T==1)          //  T==1 ��ʾ�ӵ���ǰ������½����
    	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
	else if(T==2)     //  T==2 ��ʾ�ӵ���ǰ������ˮ����
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY|BACKGROUND_BLUE);
    GoToxy(2*x,y);
    printf("��");
}
 
 
void ClearBullet(int x,int y,int T)   //��ǰ����BulletCheak ��ֵ������ T
{
	GoToxy(2*x,y);
	if(T==2)        //  T==2 ��ʾ�ӵ���ǰ������ˮ����
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|BACKGROUND_BLUE|FOREGROUND_BLUE|FOREGROUND_GREEN);
		printf("��");
	}
	else if(T==1)   //  T==1 ��ʾ�ӵ���ǰ������½����
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE);
		printf("  ");
	}
}
 
 
//positionΪ̹������λ��,-1Ϊ��λ��,0Ϊ�м�,1Ϊ��,2Ϊ�ҵ�̹��λ��
void BuildAITank(int* position, Tank* AI_tank)   //ִ��һ�θú���ֻ����һ��̹��
{                                         //rand������ʽ��0<=rand()%(a+1)<=a  0+m<=rand()%(n-m+1)+m<=n  
                                          //rand����ʵ��1��n��1<=rand()%(n)+1<=n
   	if(AIPositionCheak(*position))        //����λ�����ϰ�,�����ɡ�position�������AIPositionCheak��������
	{
		AI_tank->x= 20 + 18*(*position);  //20 + 18 * position ��Ӧ��������λ�õ�x������
		AI_tank->y=2;
		if(AI_tank->revive==level_info[level-1].firm_tank_order)  //̹�˳���(����)����==�ؿ���Ϣ(level_info)��firm tank�ĳ��ִ���
		{
			AI_tank->model = 3;           //3Ϊfirm tank��ģ��(���)
			AI_tank->color = 2;           //��ɫ����2Ϊ��ɫ�������������ColorChoose
		}
		else if(AI_tank->revive==level_info[level-1].fast_tank_order)  //ͬ��if��������fast_tank��
		{
			AI_tank->model = 2;
			AI_tank->color = rand()%6+1;  //������firm tank�������ɫ����ɫ����Ϊ1~6,�ֱ����ͬ��ɫ,�������ColorChoose
		}
		else      //��̹ͨ��
		{
			AI_tank->model = 1;
	 	  	AI_tank->color = rand()%6+1;  //������firm tank�������ɫ
		}
		AI_tank->alive = 1;       //̹�˱�Ϊ����
		AI_tank->direction = 2 ;  //������
		AI_tank->revive++;        //�������+1
		PrintTank(*AI_tank);
		(*position)++; 
		remain_enemy--;
		GoToxy(102,9);            //�ڸ���Ļ�ϴ�ӡʣ��̹����
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
		printf("%d ",remain_enemy);
		if(*position==2)          //positionֻ��Ϊ0,1��-1������positionѭ������
    		*position = -1;
  	   	return ;                  //��������һ��̹�ˣ�������ú���
	}
}
 
 
int AIPositionCheak( int position )    //positionΪ̹������λ��2Ϊ�ҵ�̹��λ�ã�����ΪAIλ��-1Ϊ��λ��0Ϊ�м�λ�ã�1Ϊ��λ
{
	int	i,j,x,y;
	if(position==2)                    //2Ϊ�ҵ�̹��λ�ã�������ʱ�ò���
		x=15,y=38;
	else
    	y=2 , x= 20 + 18 * position ;  //20 + 18 * position ��Ӧ��������λ�õ�x������
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
			if( map[y+j-1][x+i-1]!=0)  //��������ľŹ��������ϰ���
				return 0;              //�򷵻�0����ʾ������λ�����谭
    return 1;                          //��������1����ʾ������λ�����谭
}
 
 
void MoveAITank(Tank* AI_tank) //AIר�ú������ú�����ҪΪAI��ǿ
{
    int j;
    if(AI_tank->alive)         //���̹�˻���
	{
		if(AI_tank->stop!=0)   //̹���Ƿ�ֹͣ�˶����жϣ���stop������Ϊ0
		{
			AI_tank->stop--;   //���̹�˱��غ�ֹͣ�˶�
			return;
		}
		if( !(rand()%23) )     //22��֮1�ĸ���ִ�з�������
		{
			AI_tank->direction = rand()%4+1;
			if( rand()%3 )     //�ڷ������ú���2��֮1�ĸ���ֹͣ�߶�3����ʱ��
			{
				AI_tank->stop=2;
				return;
			}
		}
		ClearTank (AI_tank->x , AI_tank->y);
		if(TankCheck ( *AI_tank , AI_tank->direction))   //���ǰ�����ϰ�
			switch ( AI_tank->direction )
			{
	       		case UP   : AI_tank->y--; break;  //��ǰ��һ��
        		case DOWN : AI_tank->y++; break;  //��ǰ��һ��
         		case LEFT : AI_tank->x--; break;  //��ǰ��һ��
        		case RIGHT: AI_tank->x++; break;  //��ǰ��һ��
			}
		else                     //ǰ�����ϰ�
		{
			if(!(rand()%4))      //3��֮1�ĸ�����ת
			{
				AI_tank->direction=rand()%4+1;
				AI_tank->stop=2; //��ת֮��ֹͣ�߶�3����ʱ��
				PrintTank(*AI_tank);
				return;          //��continue����������Ĵ�ӡ����,�������ȴ�ӡ
			}
			else                 //����3��֮2�ļ���ѡ����ȷ�ķ���
			{
				
	    		for(j=1;j<=4;j++)
	        		if(TankCheck ( *AI_tank , j ))  //ѭ���ж�̹�����������ϰ�,�˺�����ֵ1Ϊ��ͨ��
	    				break;
    			if(j==5)         //j==5˵����̹�����ܶ����ϰ���޷�ͨ��
				{
					PrintTank(*AI_tank);
	    			return;      //�����������whileѭ���Է�������
				}
    			while(TankCheck ( *AI_tank , AI_tank->direction) == 0)  //���ǰ�������ϰ�
	    			AI_tank->direction=(rand()%4+1);                    //�򻻸����������
			}
		}
		PrintTank(*AI_tank);     //��ӡAI̹��
	}
}
 
 
void BuildMyTank (Tank* my_tank) //�����ҵ�̹��
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
    PrintTank (*my_tank) ;   //��ӡ�ҵ�̹��
}
 
 
void MoveMyTank(int turn )   //���ר�ú�����turnΪkeyboard�����������벻ͬ�����������Ĳ�ͬ��ֵ
{
	ClearTank(my_tank.x , my_tank.y);        //map �����С��ҵ�̹�ˡ���������������ڴ˺��������
	if(my_tank.direction!=turn)
    	my_tank.direction=turn;                  //����������ķ���ֵ�����ҵ�̹�˷���ֵ
	else if(TankCheck ( my_tank , my_tank.direction ))  //����ʱ�ҵ�̹�˵�ǰ���������ϰ�
		switch (turn)
		{
			case UP   : my_tank.y--; break;  //��ǰ��һ��
			case DOWN : my_tank.y++; break;  //��ǰ��һ��
			case LEFT : my_tank.x--; break;  //��ǰ��һ��
			case RIGHT: my_tank.x++; break;  //��ǰ��һ��
	    }                                        //��̹�˵�ǰ���������ϰ�����������仯ֱ�Ӵ�ӡ��ת���̹��
	PrintTank (my_tank);
}
 
 
int TankCheck(Tank tank,int direction)  //���̹��ǰ���ϰ�����,����Ϊ�����ꡣ��ֵ1Ϊ��ͨ��,��ֵ0Ϊ�赲(�˻�����)
{
	switch(direction)                    //direction����   1�ϣ�2�£�3��4��
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
			printf("���󣡣�");
			Sleep(5000);
			return 0;
	}
}
 
 
void ClearTank(int x,int y)   //���̹�˺������˻����ã�
{
	int i,j;
	for(i=0;i<3;i++)
		for(j=0;j<3;j++)
		{                     //��̹��ռ�õĵ�ͼ�ϵľŸ�ȥ��
	     	map[y+j-1][x+i-1]=0;
	        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN);
			GoToxy(2*x+2*j-2,y+i-1);
			printf("  ");
		}
}
 
 
void PrintTank(Tank tank)     //��ӡ̹�ˣ��˻����ã� ���ڶ�ȡ��Tank�����϶�,�ʾͲ�������һһ������
{                             // tank.color������Ӧ��ͬ����ɫ,��Χ 1 ~ 6
    int i,j;
    char** tankF[4];
    ColorChoose(tank.color);  //��ɫѡ����   ����һ��������װ���ַ�ָ��(��װ�ַ���)������ָ��(ָ��һά�����׵�ַ��ָ��)
    for(i = 0;i < 4;i ++)
        tankF[i] = tank_figure[tank.model][i];  //����ά������ַ����ֵ������ָ�� model==0Ϊ�ҵ�̹��,4Ϊ����1̹��,8Ϊ����2,����
	for(i = 0; i < 3; i++)   
	{
        GoToxy((tank.x-1)*2 , tank.y-1+i);        //��̹�������������ߣ����������д�ӡ
        printf("%s", tankF[i][tank.direction-1]); //��ӡ���ǵ�ַ����ַ���ַ���
     	for(j=0;j<3;j++)
			if(tank.my)       //��Ϊ�ҵ�̹��
	            map[tank.y+j-1][tank.x+i-1]=200;  //��map�ϰ�"̹��"�Ÿ������������̹�˵Ĳ������з���ֵΪ100~103,�ҷ�Ϊ200
			else
		    	map[tank.y+j-1][tank.x+i-1]=100+tank.num;  //��������ͨ��mapֵ��ȡ̹�˱��,��ȡ������BulletHit ����
	}
}
 
 
void HideCursor()  //���ع��
{                  //CONSOLE_CURSOR_INFO�ṹ���������̨������Ϣ,DWORD dwSize���ٷֱȺ�ȣ�1~100����BOOL bVisible����Ƿ�ɼ�
	CONSOLE_CURSOR_INFO cursor_info={1,0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cursor_info); //SetConsoleCursorInfo��������ָ���Ŀ���̨���Ĵ�С�Ϳɼ��ԡ�
}
 
 
void GoToxy(int x,int y)  //����ƶ�������X��ʾ�����꣬Y��ʾ�����ꡣ
{
	COORD  coord;         //ʹ��ͷ�ļ��Դ�������ṹ
    HANDLE a;
	coord.X=x;            //���ｫint����ֵ����short,�����������漰������ֵ�����ᳬ��short��Χ
	coord.Y=y;
	a=GetStdHandle(STD_OUTPUT_HANDLE);  //��ñ�׼������
	SetConsoleCursorPosition(a,coord);         //�Ա�׼����ľ��Ϊ�������ÿ���̨�������
}
 
 
void ColorChoose(int color)   //��ɫѡ����
{
	switch(color)
	{
	   	case 1:               //����ɫ(�ҵ�̹����ɫ)
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
			break;
		case 2:               //��ɫ
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);	
			break;
		case 3:               //��ɫ
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
			break;
		case 4:               //��ɫ
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
			break;
		case 5:               //��ɫ
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
			break;
		case 6:               //��ɫ
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE|FOREGROUND_GREEN);
			break;
		case 7:               //����ɫ(����ɫ�������ɫ������ʶ�Ȳ��� ��̹����ɫ��ѡ�ô���ɫ),ֻ����������ɫ��˸��
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE);
			break;
	}
}
 
 
int Stop()    //��ͣ
{
	int color=1,timing=0;
	while(1)
	{
		if(timing++%30==0)
		{
			ColorChoose(color);   //��ɫѡ��
			GoToxy(100,13);       //����Ļ��ӡ
			printf("��Ϸ��ͣ");
			GoToxy(88,17);
			printf("���س����ص���Ϸ");
			GoToxy(88,18);
			printf("�� Esc���˳���Ϸ");
			if(++color==8)
				color=1;
		}
		if (GetAsyncKeyState( 0xD )& 0x8000)      //�س���
		{
			GoToxy(100,13);       //����Ļ��ӡ
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
			printf("���ڽ���");   //���ǵ�ԭ������ʾ
			GoToxy(88,17);
			printf("                     ");
			GoToxy(88,18);
			printf("                     ");
			break;
		}
		else if(GetAsyncKeyState( 0x1B )& 0x8000) //Esc���˳�	
			return -1;
		Sleep(20);
	}
	return 0;
}
 
 
void ClearMainScreen()  //����Ļ������������ʹ��system("cls");�ٴ�ӡ�����һ��������ɿ������һ�еĴ������Ե�����д��������
{
    int i;
	for(i=1;i<40;i++)
	{
		GoToxy(2,i);
		printf("                                                                              ");
	}
}
 
 
void Frame ()     //��ӡ��Ϸ������
{                 //SetConsoleTextAttributeΪ�����ı���ɫ���ı�������ɫ����
	int i;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
	printf("  �x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x�x  ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_BLUE); 
	printf("  �y�y�y�y�y�y�y�y�y�y�y�y�y \n");
	for(i=0;i<14;i++)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_INTENSITY);
	    printf("��                                                                              ��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_BLUE); 
		printf(" |                          |\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_INTENSITY);
	printf("��                                                                              ��");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_BLUE); 
	printf(" |�T�T�T�T�T�T�T�T�T�T�T�T�T|\n");
	for(i=0;i<24;i++)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_INTENSITY);
	    printf("��                                                                              ��");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY|FOREGROUND_BLUE); 
		printf(" |                          |\n");
	}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_INTENSITY);
	printf("  ������������������������������������������������������������������������������  ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY| FOREGROUND_BLUE); 
	printf(" �i�i�i�i�i�i�i�i�i�i�i�i�i�i\n");
	SideScreen ();  //��ӡ����Ļ
}
 
 
void PrintMap()     // ��ӡ��ͼ(��ͼ�ȵ�ͼ�ϰ���)
{
    int i,j;
	for(j=0;j<41;j++)
		for(i=0;i<41;i++)
			if(map[i][j]==6)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN
					|FOREGROUND_RED|FOREGROUND_BLUE|BACKGROUND_GREEN|BACKGROUND_RED|BACKGROUND_BLUE);
				GoToxy(2*j,i);
				printf("��");
			}
			else if(map[i][j]==2)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|BACKGROUND_GREEN|BACKGROUND_RED);
				GoToxy(2*j,i);
				printf("��");
			}
			else if(map[i][j]==1)
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|BACKGROUND_GREEN|BACKGROUND_RED);
				GoToxy(2*j,i);
				printf("��");
			}
			else if(map[i][j]==5)
			{                      
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|BACKGROUND_BLUE|FOREGROUND_BLUE|FOREGROUND_GREEN);
				GoToxy(2*j,i);
				printf("��");
			}
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN);
	GoToxy(38,37);	 printf("���嗀");
	GoToxy(38,38);	 printf("������");    //�����۵�ͼ��ô��,������λ�ò���,�Ҽҵ��ַ�����,��������������ʽ��ӡ
	GoToxy(38,39);	 printf("������");    //��ֱ�Ӵ�ӡ(�Ҽҵ�mapֵ������޹�)
}
 
 
void GetMap()      //��ͼ��ź���
{	               //map���ֵ: ��λ����ֵΪ��ͼ���鲿�֣���λ����ֵΪ̹��
	int i ,j;      //map���ֵ: 0Ϊ��ͨ��½�أ�1Ϊ��ש��2������5Ϊˮ��100Ϊ�з�̹�ˣ�200Ϊ�ҵ�̹�ˣ�
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
	PrintMap();         //��ӡ��ͼ
}
 
 
int GameOver(int home)
{
	int timing=0,color=1;
	while(1)
	{
		if(timing++%30==0)         //��Ϸ����ԭ��Ϊ����ֵΪ0
		{
			ColorChoose(color);    //��ɫѡ��
			if(home)               //��Ϸ����ԭ��Ϊ�ϼұ���,����ӡһ��������ʾ���
			{
				GoToxy(37,19);     //����Ļ���Ĵ�ӡ
				printf("�ϼұ���!");
			}
			GoToxy(37,20);         //����Ļ���Ĵ�ӡ
			printf("��Ϸ����!");
			GoToxy(100,13);        //����Ļ��ӡ
			printf("��Ϸ����");
			GoToxy(88,17);
			printf("�밴�س������¿�ʼ!");
			GoToxy(88,18);
			printf("�� Esc���˳���Ϸ!");
			if(++color==8)
				color=1;
		}
		if (GetAsyncKeyState( 0xD )& 0x8000)  //�س���
		{
//			system("cls");       //����,�����������ٴδ�ӡ�����һ��������ɿ������һ�е�bug�����ѡ���Խ���������
//			Frame ();            //���´�ӡ��Ϸ���
			score-=500;          //����-500
			ClearMainScreen();   //�������������������ٴδ�ӡ���
			Initialize();        //�ӱ������¿�ʼ
			break;
		}
		else if(GetAsyncKeyState( 0x1B )& 0x8000)  //Esc���˳�	
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
				ColorChoose(color);   //��ɫѡ��   
				GoToxy(37,20);        //����Ļ���Ĵ�ӡ
				printf("��ϲ����!");
				GoToxy(100,13);       //����Ļ��ӡ
				printf("�ȴ��¹�");
				GoToxy(87,17);
				printf("�밴�س���������һ��!");
				GoToxy(88,18);
				printf("�� Esc���˳���Ϸ!");
				if(++color==8)	
					color=1;
			}
			if (GetAsyncKeyState( 0xD )& 0x8000)  //�س���
			{
				GoToxy(88,17);        //Ĩ������Ļ�е���ʾ
				printf("                     ");
				GoToxy(88,18);
				printf("                     ");
				ClearMainScreen();   //�������������������ٴδ�ӡ���
				Initialize();        //��ʼ������һ�ؿ�ʼ,level��++
				break;
			}
			else if(GetAsyncKeyState( 0x1B )& 0x8000)  //Esc���˳�	
				return -1;
			Sleep(20);
		}
	else   //level>8 ͨ��
		while(1)
		{
			if(timing++%5==0)
			{
				ColorChoose(color);
				GoToxy(33,20);        //����Ļ���Ĵ�ӡ
				printf("��ϲͨ��ȫ���ؿ�!");
				GoToxy(100,13);       //����Ļ��ӡ
				printf("��ͨȫ��");
				GoToxy(88,17);
				printf("��ϲͨ��ȫ���ؿ�!");
				GoToxy(88,19);
				printf("�� Esc���˳���Ϸ!");
				if(++color==8)	
					color=1;
			}
			if(GetAsyncKeyState( 0x1B )& 0x8000)  //Esc���˳�	
				return -1;
			Sleep(10);
		}
	return 0;
}
 
 
void GameCheak()
{                           //ʣ�����Ϊ0����̹��ȫ�������
	if(remain_enemy<=0 && !AI_tank[0].alive && !AI_tank[1].alive && !AI_tank[2].alive && !AI_tank[3].alive )
		NextLevel();        //������һ��
	if(my_tank.revive>=MAX_LIFE)   //�ҵ�����ֵ(�������)ȫ������ MAX_LIFE
		GameOver(0);        //��Ϸ����������0�����ҵĸ�������ù�(����ֵΪ0)����Ϸ�����������жϣ���һ�����ϼұ���
}
 
 
void SideScreen ()  //����Ļ ��(84��,110ĩ,��˫�ַ�������108��ӡ���һ���֣���(11����ĩ,13������,39����ĩ��Ϊ������ô���38)
{                   // |         ��  d  ��         |   " |                          |\n"
	GoToxy(93,2);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED);
	printf("��     ��");
	GoToxy(92,5);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE);
	printf("��  ����");
	GoToxy(92,7);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	printf("��  ����");
	GoToxy(86,9);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
	printf("ʣ��з�̹�ˣ�");
	GoToxy(86,11);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_BLUE);
	printf("��ǰ��Ϸ�ٶȣ�  %d",21-speed);
	GoToxy(86,13);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_BLUE);
	printf("��ǰ��Ϸ״̬��");
	GoToxy(94,19);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED);
	GoToxy(94,24);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED);
	printf("��  ��");
	GoToxy(86,27);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	printf("�����  ��������  �ƶ�");
	GoToxy(93,29);
	printf("x �� ���");
	GoToxy(89,31);
	printf("+ - ������Ϸ�ٶ�");
	GoToxy(90,33);
	printf("��Ϸ�ٶȷ�Χ1~20");
	GoToxy(90,35);
	printf("�س��� ��ͣ��Ϸ");
	GoToxy(90,37);
	printf("Esc��  �˳���Ϸ");
/*	printf("��  ��");     //���ǵڶ�����ϸ˵������ʽ
	GoToxy(86,21);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	printf("�����  ��������  �ƶ�");
	GoToxy(93,23);
	printf("x �� ���");
	GoToxy(89,25);
	printf("+ - ������Ϸ�ٶ�");
	GoToxy(90,27);
	printf("��Ϸ�ٶȷ�Χ1~20");
	GoToxy(90,29);
	printf("�س��� ��ͣ��Ϸ");
	GoToxy(90,31);
	printf("Esc��  �˳���Ϸ");
	GoToxy(86,33);
	printf("�з�̹��ȫ�����������");
	GoToxy(87,34);
	printf("����̹������ֵΪ0 ��");
	GoToxy(86,35);
	printf("���·����ϼұ�����ʧ��");
	GoToxy(86,36);
	printf("��̹�����̹���ӵ���ײ");
	GoToxy(87,37);
	printf("���������̹�˼��ӵ���");
	GoToxy(86,38);
	printf("ײ�������ҿɴ�����̹��");*/
}
 
 
void Initialize()      //��ʼ��
{
    int i;
    remain_enemy=16;
	my_tank.revive=0;  //�ҵ�̹�˸������Ϊ0
	position=0;
	bul_num=0;
	GetMap();
	BuildMyTank( &my_tank );
	for(i=0;i<12;i++)     //�ӵ���ʼ��
	{
		bullet [i].exist=0;
		bullet [i].initial=0;
	}
	for(i=0;i<=3;i++)         //AI̹�˳�ʼ��
	{
		AI_tank [i].revive=0;
		AI_tank [i].alive=0;  //��ʼ��̹��ȫ�ǲ����ģ�BuildAITank()�Ὠ�����½���������̹��
		AI_tank [i].stop=0;
		AI_tank [i].num=i;
		AI_tank [i].my=0;
		AI_tank [i].CD=0;
	}
	GoToxy(97,2);                        //�ڸ���Ļ�Ϲؿ���
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
	printf("%d",level);
	GoToxy(102,5);                       //�ڸ���Ļ�ϴ�ӡ����
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE);
	printf("%d   ",score);
	GoToxy(102,7);                       //�ڸ���Ļ��ӡ�ҵ�ʣ������ֵ
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_GREEN);
	printf("%d", MAX_LIFE-my_tank.revive);
	GoToxy(102,9);                       //�ڸ���Ļ�ϴ�ӡʣ��̹����
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
	printf("%d ",remain_enemy);
	GoToxy(100,13);                      //�ڸ���Ļ�ϴ�ӡ״̬
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_BLUE|FOREGROUND_GREEN);
	printf("������Ϸ");
}


int tank_main (int argc,char **argv)                               //������
{
	int i,res;
	HANDLE hConsoleOutput;
    CONSOLE_SCREEN_BUFFER_INFO info;
    
	unsigned int interval[12] = {1,1,1,1,1,1,1,1,1,1,1,1};  //������������飬���ڿ����ٶ�
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // ��ȡ����̨������
    GetConsoleScreenBufferInfo(hConsoleOutput, &info);
	srand(time(NULL)); //�������������(�����������Ӷ�����rand��ʹÿ�����е����������һ��)���������ָ:���״ε���rand�õ�1,�ڶ��ε�2,������3,��˴����������Ϊ1,2,3
	HideCursor();                         //���ع��
	system("mode con cols=112 lines=42"); //���ƴ��ڴ�С
	Frame ();                             //��ӡ��Ϸ������
	Initialize();                         //��ʼ����ȫ�ֱ���level��ֵ����1 
	for(;;)
	{
		if(interval[0]++%speed==0)        //�ٶȵ�����,����interval[0]Ϊa, �����Ϊ a % 2==0,a=a+1; 
		{
			GameCheak();                  //��Ϸʤ�����
			BulletFly ( bullet );
			for(i=0 ; i<=3 ; i++)         //AI̹���ƶ�ѭ��
			{
				if(AI_tank[i].model==2 && interval[i+1]++%2==0) //�ĸ�̹���еĿ���̹�˵���ʹ�ü�����1,2,3,4
					MoveAITank( & AI_tank[i]);
				if(AI_tank[i].model!=2 && interval[i+5]++%3==0) //�ĸ�̹���е�����̹�˵���ʹ�ü�����5,6,7,8
					MoveAITank( & AI_tank[i]);
			}
            
			for(i=0;i<=3;i++)                                   //����AI̹�˲���
	  		   	if(AI_tank[i].alive==0 && AI_tank[i].revive<4 && interval[9]++%90==0)  //һ���з�̹��ÿ��ֻ��4����
				{                                               //���̹�˲�����ʱ,ÿ�ν����м��  1750 ms
             	  	BuildAITank( &position, & AI_tank[i] );     //����AI̹�ˣ����
			  		break;                                      //ÿ��ѭ��ֻ����һ��̹��
		  		}
			for(i=0;i<=3;i++)
				if(AI_tank[i].alive)
					BuildAIBullet(&AI_tank[i]);                 //AIshoot�Դ�int��������CD,��ʹ��main�е�CD interval
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
    system("mode con cols=100 lines=50");//���ô��ڴ�С
    SetConsoleTextAttribute(hConsoleOutput, info.wAttributes);
    system("cls"); // ����
	fflush(stdin);
	return 0;
}

#endif
