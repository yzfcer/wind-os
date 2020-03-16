#include "wind_config.h"
#if (WIND_MODULE_GAME_SUPPORT)
#include<stdio.h>
#include<time.h>
#include<windows.h>
#include<stdlib.h>

#define U 1
#define D 2
#define L 3 
#define R 4       //蛇的状态，U：上 ；D：下；L:左 R：右

typedef struct SNAKE //蛇身的一个节点
{
    int x;
    int y;
    struct SNAKE *next;
}snake;

typedef struct __snake_ctx_s
{
    int score; 
    int add;//总得分与每次吃食物得分。
    int status;
    int movetime;  //运行时间
    snake *head;//蛇头指针
    snake *food;//食物指针
    snake *q;   //遍历蛇的时候用到的指针
    int end_status; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。
}snake_ctx_s;

snake_ctx_s ctx;


//声明全部函数//
static void param_init(void);
static void Pos();
static void creatMap();
static void initSnake();
static int biteSelf();
static void createFood();
static int cantCrossWall();
static int snakeMove();
static void pause();
static int runGame();
static void initGame();
static int endGame();
static void gameStart();

static void param_init(void)
{
    ctx.score = 0;//总得分
    ctx.add = 12;//每次吃食物得分。
    ctx.status = 0;
    ctx.head = NULL;//蛇头指针
    ctx.food = NULL;//食物指针
    ctx.movetime = 200;
    ctx.q = NULL;//遍历蛇的时候用到的指针
    ctx.end_status = 0; //游戏结束的情况，1：撞到墙；2：咬到自己；3：主动退出游戏。
}

static void Pos(int x, int y)//设置光标位置
{
    COORD pos;
    HANDLE hOutput;
    pos.X = x;
    pos.Y = y;
    hOutput = GetStdHandle(STD_OUTPUT_HANDLE);//返回标准的输入、输出或错误的设备的句柄，也就是获得输入、输出/错误的屏幕缓冲区的句柄
    SetConsoleCursorPosition(hOutput, pos);
}

static void creatMap()//创建地图
{
    int i;
    for (i = 0; i<58; i += 2)//打印上下边框
    {
        Pos(i, 0);
        printf("■");//一个方块占两个位置
        Pos(i, 26);
        printf("■");
    }
    for (i = 1; i<26; i++)//打印左右边框
    {
        Pos(0, i);
        printf("■");
        Pos(56, i);
        printf("■");
    }
}

static void initSnake()//初始化蛇身
{
    snake *tail;
    int i;
    tail = (snake*)malloc(sizeof(snake));//从蛇尾开始，头插法，以x,y设定开始的位置//
    tail->x = 24;
    tail->y = 5;
    tail->next = NULL;
    for (i = 1; i <= 4; i++)//初始长度为4
    {
        ctx.head = (snake*)malloc(sizeof(snake));
        ctx.head->next = tail;
        ctx.head->x = 24 + 2 * i;
        ctx.head->y = 5;
        tail = ctx.head;
    }
    while (tail != NULL)//从头到为，输出蛇身
    {
        Pos(tail->x, tail->y);
        printf("■");
        tail = tail->next;
    }
}
//??
static int biteSelf()//判断是否咬到了自己
{
    snake *self;
    self = ctx.head->next;
    while (self != NULL)
    {
        if (self->x == ctx.head->x && self->y == ctx.head->y)
        {
            return 1;
        } 
        self = self->next;
    }
    return 0;
}

static void createFood()//随机出现食物
{
    snake *food_1;
    srand((unsigned)time(NULL));//为了防止每次产生的随机数相同，种子设置为time
    food_1 = (snake*)malloc(sizeof(snake));
    while ((food_1->x % 2) != 0)    //保证其为偶数，使得食物能与蛇头对其
    {
        food_1->x = rand() % 52 + 2;
    }
    food_1->y = rand() % 24 + 1;
    ctx.q = ctx.head;
    while (ctx.q->next == NULL)
    {
        if (ctx.q->x == food_1->x && ctx.q->y == food_1->y) //判断蛇身是否与食物重合
        {
            free(food_1);
            createFood();
        }
        ctx.q = ctx.q->next;
    }
    Pos(food_1->x, food_1->y);
    ctx.food = food_1;
    printf("■");
}

static int cantCrossWall()//不能穿墙
{
    if (ctx.head->x == 0 || ctx.head->x == 56 || ctx.head->y == 0 || ctx.head->y == 26)
    {
        ctx.end_status = 1;
        //return endGame();
        return -1;
    }
    return 0;
}

static int snakeMove()//蛇前进,上U,下D,左L,右R
{
    int res;
    snake * nexthead;
    res = cantCrossWall();
    if(res != 0)
        return res;

    nexthead = (snake*)malloc(sizeof(snake));
    if (ctx.status == U)
    {
        nexthead->x = ctx.head->x;
        nexthead->y = ctx.head->y - 1;
        if (nexthead->x == ctx.food->x && nexthead->y == ctx.food->y)//如果下一个有食物//
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("■");
                ctx.q = ctx.q->next;
            }
            ctx.score = ctx.score + ctx.add;
            createFood();
        }
        else                                               //如果没有食物//
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q->next->next != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("■");
                ctx.q = ctx.q->next;
            }
            Pos(ctx.q->next->x, ctx.q->next->y);
            printf("  ");
            free(ctx.q->next);
            ctx.q->next = NULL;
        }
    }
    if (ctx.status == D)
    {
        nexthead->x = ctx.head->x;
        nexthead->y = ctx.head->y + 1;
        if (nexthead->x == ctx.food->x && nexthead->y == ctx.food->y)  //有食物
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("■");
                ctx.q = ctx.q->next;
            }
            ctx.score = ctx.score + ctx.add;
            createFood();
        }
        else                               //没有食物
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q->next->next != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("■");
                ctx.q = ctx.q->next;
            }
            Pos(ctx.q->next->x, ctx.q->next->y);
            printf("  ");
            free(ctx.q->next);
            ctx.q->next = NULL;
        }
    }
    if (ctx.status == L)
    {
        nexthead->x = ctx.head->x - 2;
        nexthead->y = ctx.head->y;
        if (nexthead->x == ctx.food->x && nexthead->y == ctx.food->y)//有食物
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("■");
                ctx.q = ctx.q->next;
            }
            ctx.score = ctx.score + ctx.add;
            createFood();
        }
        else                                //没有食物
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q->next->next != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("■");
                ctx.q = ctx.q->next;
            }
            Pos(ctx.q->next->x, ctx.q->next->y);
            printf("  ");
            free(ctx.q->next);
            ctx.q->next = NULL;
        }
    }
    if (ctx.status == R)
    {
        nexthead->x = ctx.head->x + 2;
        nexthead->y = ctx.head->y;
        if (nexthead->x == ctx.food->x && nexthead->y == ctx.food->y)//有食物
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("■");
                ctx.q = ctx.q->next;
            }
            ctx.score = ctx.score + ctx.add;
            createFood();
        }
        else                                         //没有食物
        {
            nexthead->next = ctx.head;
            ctx.head = nexthead;
            ctx.q = ctx.head;
            while (ctx.q->next->next != NULL)
            {
                Pos(ctx.q->x, ctx.q->y);
                printf("■");
                ctx.q = ctx.q->next;
            }
            Pos(ctx.q->next->x, ctx.q->next->y);
            printf("  ");
            free(ctx.q->next);
            ctx.q->next = NULL;
        }
    }
    if (biteSelf() == 1)       //判断是否会咬到自己
    {
        ctx.end_status = 2;
        //endGame();
        return -1;
    }
    return 0;
}

static void pause()//暂停
{
    while (1)
    {
        Sleep(300);
        if (GetAsyncKeyState(VK_SPACE))
        {
            break;
        }

    }
}

static void readKey(void)
{
    if (GetAsyncKeyState( 0x1B )& 0x8000)
    {
        ctx.status = 0x1B;
    }
    else if (GetAsyncKeyState(VK_UP) && ctx.status != D)
    {
        ctx.status = U;
    }
    else if (GetAsyncKeyState(VK_DOWN) && ctx.status != U)
    {
        ctx.status = D;
    }
    else if (GetAsyncKeyState(VK_LEFT) && ctx.status != R)
    {
        ctx.status = L;
    }
    else if (GetAsyncKeyState(VK_RIGHT) && ctx.status != L)
    {
        ctx.status = R;
    }
    else if (GetAsyncKeyState(VK_SPACE))
    {
        pause();
    }
    else if (GetAsyncKeyState(VK_ESCAPE))
    {
        ctx.end_status = 3;
        ctx.status = 0x1B;
    }
    else if (GetAsyncKeyState(VK_F1))
    {
        if (ctx.movetime > 50)
        {
            ctx.movetime = ctx.movetime - 30;
            ctx.add = ctx.add + 2;
            Sleep(500);
        }
        
    }
    else if (GetAsyncKeyState(VK_F2))
    {
        if (ctx.movetime < 350)
        {
            ctx.movetime = ctx.movetime + 30;
            ctx.add = ctx.add - 2;
            if (ctx.add < 1)
                ctx.add = 1;  //保证最低分为1
            Sleep(500);
        }
    }
}


static int runGame()//控制游戏        
{
    int res;
    int time = 0;
    Pos(64, 15);
    printf("不能穿墙，不能咬到自己\n");
    Pos(64, 16);
    printf("用↑.↓.←.→分别控制蛇的移动.");
    Pos(64, 17);
    printf("F1 为加速，F2 为减速\n");
    Pos(64, 18);
    printf("ESC ：退出游戏.space：暂停游戏.");
    Pos(64, 20);
    printf("C语言研究中心 www.clang.cc");
    ctx.status = R;
    while (1)
    {
        Pos(64, 10);
        printf("得分：%d  ", ctx.score);
        Pos(64, 11);
        printf("每个食物得分：%d分", ctx.add);

        readKey();
        if(ctx.status == 0x1B)
            return -1;
        Sleep(10);
        time += 10;
        if(time >= ctx.movetime)
        {
            time = 0;
            res = snakeMove();
            if(res != 0)
                return 0;
        }
            
        
    }
    return 0;
}

static void initGame()//开始界面
{
    Pos(40, 12);

    system("title C语言研究中心   www.clang.cc");
    printf("欢迎来到贪食蛇游戏！");
    Pos(40, 25);
    printf("              C语言研究中心  www.clang.cc.\n");
    system("pause");
    system("cls");
    Pos(25, 12);
    printf("用↑.↓.←.→分别控制蛇的移动\n");
    Pos(25, 13);
    printf("F1 为加速，F2 为减速\n");
    Pos(25, 14);
    printf("加速将能得到更高的分数!!!\n");
    system("pause");
    system("cls");
}

static int endGame()//结束游戏
{
    system("cls");
    Pos(24, 12);
    if (ctx.end_status == 1)
    {
        printf("对不起，您撞到墙了。游戏结束.");
    }
    else if (ctx.end_status == 2)
    {
        printf("对不起，您咬到自己了。游戏结束.");
    }
    else if (ctx.end_status == 3)
    {
        printf("您的已经结束了游戏。");
    }
    Pos(24, 13);
    printf("您的得分是%d\n", ctx.score);
	system("pause");
    //printf("再来一次?[y]");
    //if (getchar() != 'y')
    //    return -1;
    return 0;
}

static void gameStart()//游戏初始化
{
    system("mode con cols=100 lines=30");
    initGame();
    creatMap();
    initSnake();
    createFood();
}

int snake_main(int argc,char **argv)
{
    int res;
    while(1)
    {
        param_init();
        gameStart();
        res = runGame();
        if(res != 0)
            break;
        endGame();
    }
    system("mode con cols=100 lines=50");//设置窗口大小
    return 0;
}
#endif
