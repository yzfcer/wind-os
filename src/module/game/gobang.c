#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <malloc.h>
#include <windows.h>

struct rcd;//声明节点结构
typedef struct rcd* Record;//节点指针别名
typedef struct rcd record;//节点别名

#define MAXIMUS 15 //定义棋盘大小

static int p[MAXIMUS][MAXIMUS];//存储对局信息
static char buff[MAXIMUS*2+1][MAXIMUS*4+3];//输出缓冲器
static int Cx,Cy;//当前光标位置
static int Now;//当前走子的玩家，1代表黑，2代表白
static int wl,wp;//当前写入缓冲器的列数和行数位置
static char* showText;//在棋盘中央显示的文字信息
static int count;//回合数
static int Putable;//指示当前是否可以走棋
static int Exiting;//1为当场上无子并按ESC时询问是否退出程序的状态，2为非此状态
static int ExiRep;//1为当回放到最后一回合并按向后时询问是否退出回放的状态，2为非此状态
static Record RecBeg,RecNow;//记录的开始节点和当前节点

struct rcd//记录节点结构,双链表形式
{
    int X;//此记录走棋的X坐标
    int Y;//此记录走棋的Y坐标
    Record Next;//前一个记录
    Record Back;//后一个记录
};

static Record newRecord()//记录节点构造函数
{
    Record r=(Record)malloc(sizeof(record));//申请一个节点对象
    r->Next=NULL;//给予前后节点初值NULL
    r->Back=NULL;
    return r;
}

static void Exit()//检查退出程序
{
    //int input;
    if(Exiting)//如果是第二次按下ESC
    {
        exit(0);
    }
    else//如果是第一次按下ESC则询问是否退出程序
    {
        showText="是否退出？再次按下ESC退出，其他键返回";
        Exiting=1;//指示已经按下过ESC
    }
}

static void ExitRep()//检查退出回放
{
    //int input;
    if(ExiRep)//如果是第二次后移
    {
        ExiRep=3;
    }
    else//如果是第一次后移则询问是否退出回放
    {
        showText="是否退出？再次后移退出回放，其他键返回";
        ExiRep=1;//指示已经按下过后移
    }
}

static void AddRecord()//添加记录
{
    RecNow->X=Cx;//记录坐标
    RecNow->Y=Cy;
    RecNow->Next=newRecord();//创建下一个记录节点
    RecNow->Next->Back=RecNow;//完成双链表
    RecNow=RecNow->Next;//当前记录推至下一个记录节点
}

static int DelRecord()//删除当前记录节点，1为删除成功，0为删除失败
{
    Record b;//上一个节点
    if(RecNow->Back!=NULL)//越界检查
    {
        b=RecNow->Back;//缓存上一个节点
        free(RecNow);//释放当前节点
        RecNow=b;//当前记录回至上一个记录节点
        return 1;
    }
    else
    {
        return 0;//没有节点可删除时
    }
}

static void CleanRecord()//清理所有记录
{
    Record n;//下一个节点
    while(RecBeg->Next!=NULL)//删除所有记录，直到越界前为止
    {
        n=RecBeg->Next;//记下下一个节点
        free(RecBeg);//释放当前节点
        RecBeg=n;//当前记录推至下一个记录节点
    }
}

static char* Copy(char* strDest,const char* strSrc)//修改过的字符串复制函数，会忽略末端的\0
{
    char* strDestCopy = strDest;
    while (*strSrc!='\0')
    {
        *strDest++=*strSrc++;
    }
    return strDestCopy;
}

static void Initialize()//初始化一个对局函数
{
    int i,j;//循环变量
    system("title 对局中(按方向键控制光标，回车走子，空格撤销)，Esc退出");
    showText="";//重置显示信息
    count=0;//回合数归零
    RecNow=RecBeg=newRecord();
    Exiting=0;
    for(i=0;i<MAXIMUS;i++)//重置对局数据
    {
        for(j=0;j<MAXIMUS;j++)
        {
            p[i][j]=0;
        }
    }
    Cx=Cy=MAXIMUS/2;//重置光标到中央
    Now=1;//重置当前为黑方
}

static char* getStyle(int i,int j)//获得棋盘中指定坐标交点位置的字符，通过制表符拼成棋盘
{
    if(p[i][j]==1)//1为黑子
        return "●";
    else if(p[i][j]==2)//2为白子
        return "○";
    else if(i==0&&j==0)//以下为边缘棋盘样式
        return "┏";
    else if(i==MAXIMUS-1&&j==0)
        return "┓";
    else if(i==MAXIMUS-1&&j==MAXIMUS-1)
        return "┛";
    else if(i==0&&j==MAXIMUS-1)
        return "┗";
    else if(i==0)
        return "┠";
    else if(i==MAXIMUS-1)
        return "┨";
    else if(j==0)
        return "┯";
    else if(j==MAXIMUS-1)
        return "┷";
    return "┼";//中间的空位
}

static char* getCurse(int i,int j){//获得指定坐标交点位置左上格的样式，通过制表符来模拟光标的显示
    if(Putable)//可走棋时光标为粗线
    {
        if(i==Cx){
            if(j==Cy)
                return "┏";
            else if (j==Cy+1)
                return "┗";
        }
        else if(i==Cx+1)
        {
            if(j==Cy)
                return "┓";
            else if (j==Cy+1)
                return "┛";
        }
    }
    else//不可走棋时光标为虚线
    {
        if(i==Cx){
            if(j==Cy)
                return "┌";
            else if (j==Cy+1)
                return "└";
        }
        else if(i==Cx+1)
        {
            if(j==Cy)
                return "┐";
            else if (j==Cy+1)
                return "┘";
        }
    }
    return "　";//如果不在光标附近则为空
}

static void write(char* c)//向缓冲器写入字符串
{
    Copy(buff[wl]+wp,c);
    wp+=strlen(c);
}

static void ln()//缓冲器写入位置提行
{
    wl+=1;
    wp=0;
}

static void Display()//将缓冲器内容输出到屏幕
{
    int i,l=strlen(showText);//循环变量，中间文字信息的长度
    int Offset=MAXIMUS*2+2-l/2;//算出中间文字信息居中显示所在的横坐标位置
    if(Offset%2==1)//如果位置为奇数，则移动到偶数，避免混乱
    {
        Offset--;
    }
    Copy(buff[MAXIMUS]+Offset,showText);//讲中间文字信息复制到缓冲器
    if(l%2==1)//如果中间文字长度为半角奇数，则补上空格，避免混乱
    {
        *(buff[MAXIMUS]+Offset+l)=0x20;
    }
    system("cls");//清理屏幕，准备写入
    for(i=0;i<MAXIMUS*2+1;i++){//循环写入每一行
        printf("%s",buff[i]);
        if(i<MAXIMUS*2)//写入完每一行需要换行
            printf("\n");
    }
}

static void Print()//将整个棋盘算出并储存到缓冲器，然后调用Display函数显示出来
{
    int i,j;//循环变量
    wl=0;
    wp=0;
    for(j=0;j<=MAXIMUS;j++)//写入出交点左上角的字符，因为需要打印棋盘右下角，所以很以横纵各多一次循环
    {
        for(i=0;i<=MAXIMUS;i++)
        {
            write(getCurse(i,j));//写入左上角字符
            if(j==0||j==MAXIMUS)//如果是棋上下盘边缘则没有连接的竖线，用空格填充位置
            {
                if(i!=MAXIMUS)
                    write("　");
            }
            else//如果在棋盘中间则用竖线承接上下
            {
                if(i==0||i==MAXIMUS-1)//左右边缘的竖线更粗
                    write("┃");
                else if(i!=MAXIMUS)//中间的竖线
                    write("│");
            }
        }
        if(j==MAXIMUS)//如果是最后一次循环，则只需要处理边侧字符，交点要少一排
        {
            break;
        }
        ln();//提行开始打印交点内容
        write("　");//用空位补齐位置
        for(i=0;i<MAXIMUS;i++)//按横坐标循环正常的次数
        {
            write(getStyle(i,j));//写入交点字符
            if(i!=MAXIMUS-1)//如果不在最右侧则补充一个横线承接左右
            {
                if(j==0||j==MAXIMUS-1)
                {
                    write("━");//上下边缘的横线更粗
                }
                else
                {
                    write("─");//中间的横线
                }
            }
        }
        ln();//写完一行后提行
    }
    Display();//将缓冲器内容输出到屏幕
}

static int Put(){//在当前光标位置走子，如果非空，则返回0表示失败
    if(Putable)
    {
        p[Cx][Cy]=Now;//改变该位置数据
        AddRecord();
        return 1;//返回1表示成功
    }
    else
    {
        return 0;
    }
}

static int Check()//胜负检查，即判断当前走子位置有没有造成五连珠的情况
{
    int w=1,x=1,y=1,z=1,i;//累计横竖正斜反邪四个方向的连续相同棋子数目
    for(i=1;i<5;i++)if(Cy+i<MAXIMUS&&p[Cx][Cy+i]==Now)w++;else break;//向下检查
    for(i=1;i<5;i++)if(Cy-i>0&&p[Cx][Cy-i]==Now)w++;else break;//向上检查
    if(w>=5)return Now;//若果达到5个则判断当前走子玩家为赢家
    for(i=1;i<5;i++)if(Cx+i<MAXIMUS&&p[Cx+i][Cy]==Now)x++;else break;//向右检查
    for(i=1;i<5;i++)if(Cx-i>0&&p[Cx-i][Cy]==Now)x++;else break;//向左检查
    if(x>=5)return Now;//若果达到5个则判断当前走子玩家为赢家
    for(i=1;i<5;i++)if(Cx+i<MAXIMUS&&Cy+i<MAXIMUS&&p[Cx+i][Cy+i]==Now)y++;else break;//向右下检查
    for(i=1;i<5;i++)if(Cx-i>0&&Cy-i>0&&p[Cx-i][Cy-i]==Now)y++;else break;//向左上检查
    if(y>=5)return Now;//若果达到5个则判断当前走子玩家为赢家
    for(i=1;i<5;i++)if(Cx+i<MAXIMUS&&Cy-i>0&&p[Cx+i][Cy-i]==Now)z++;else break;//向右上检查
    for(i=1;i<5;i++)if(Cx-i>0&&Cy+i<MAXIMUS&&p[Cx-i][Cy+i]==Now)z++;else break;//向左下检查
    if(z>=5)return Now;//若果达到5个则判断当前走子玩家为赢家
    return 0;//若没有检查到五连珠，则返回0表示还没有玩家达成胜利
}
static void ReplayMode(){
    int i,j;//循环变量
    system("title 回放中(按左键后退，右键或空格前进)，Esc退出");
    showText="";//重置显示信息
    count=0;//回合数归零
    Putable=0;//不可走棋状态
    RecBeg->Back=newRecord();
    RecBeg->Back->Next=RecBeg;
    RecBeg=RecBeg->Back;
    for(i=0;i<MAXIMUS;i++)//重置对局数据
    {
        for(j=0;j<MAXIMUS;j++)
        {
            p[i][j]=0;
        }
    }
    Now=1;//重置当前为黑方
}

static void RepForward()//回放模式前进
{
    if(RecNow->Next->Next!=NULL)//越界检查
    {
        RecNow=RecNow->Next;//当前节点推至下一个记录节点
        p[RecNow->X][RecNow->Y]=Now;//按照记录还原一个回合
        Cx=RecNow->X;//设置光标位置
        Cy=RecNow->Y;
        Now=3-Now;//转换当前的黑白方
    }
    else//若已达到最后则询问退出
    {
        ExitRep();
    }
}

static void RepBackward()//回放模式后退
{
    if(RecNow->Back!=NULL)//越界检查
    {
        p[RecNow->X][RecNow->Y]=0;//按照记录撤销一个回合
        if(RecNow->Back->Back==NULL)//在整个棋盘没有棋子时隐藏光标
        {
            Cx=-2;
            Cy=-2;
        }
        else if(RecNow->Back==NULL)//在只有一个棋子时移动光标到这个棋子的位置
        {
            Cx=RecNow->X;
            Cy=RecNow->Y;
        }
        else//正常情况下移动光标到上一回合的位置
        {
            Cx=RecNow->Back->X;
            Cy=RecNow->Back->Y;
        }
        RecNow=RecNow->Back;//当前节点后退至上一个记录节点
        Now=3-Now;//转换当前的黑白方
        
    }
}

static void ShowReplay()
{
    int input;//输入变量
    ReplayMode();//初始化回放模式
    RecNow=RecBeg;//当前观察从头开始
    RepForward();//显示第一次走棋
    while(1)//开始无限回合的死循环，直到Esc退出
    {
        if(ExiRep==3)
        {
            ExiRep=0;
            break;
        }
        Print();//打印棋盘
        input=_getch();//等待键盘按下一个字符
        if(input==27)//如果是ESC则退出回放
        {
            return;
        }
        else if(input==0x20)//如果是空格则前进
        {
            RepForward();
            continue;
        }
        else if(input==0xE0)//如果按下的是方向键，会填充两次输入，第一次为0xE0表示按下的是控制键
        {
            input=_getch();//获得第二次输入信息
            switch(input)//判断方向键方向并移动光标位置
            {
            case 0x4B:
                RepBackward();//向左后退
                break;
            case 0x4D:
                RepForward();//向右前进
                continue;
            }
        }
        ExiRep=0;//未再次按后移则不准备退出
        showText="";
    }
}


static int Regret()//悔棋撤销,如果棋盘上没有子即为退出
{
    if(DelRecord()){//尝试删除当前节点，如果有节点可以删除则
        p[RecNow->X][RecNow->Y]=0;//撤除当前回合
        if(RecNow->Back==NULL)//如果删除的是第一颗子则将光标移动到第一颗子原来的位置上
        {
            Cx=RecNow->X;
            Cy=RecNow->Y;
        }
        else//否则将光标移动到上一颗子上
        {
            Cx=RecNow->Back->X;
            Cy=RecNow->Back->Y;
        }
        Now=3-Now;//反转当前黑白方
        return 0;
    }
    else
    {
        //Exit();//如果没有棋子可以撤销，则询问退出
        return -1;
    }
}

static int RunGame()//进行整个对局，返回赢家信息(虽然有用上)
{
    int input;//输入变量
    int victor;//赢家信息
    Initialize();//初始化对局
    while(1){//开始无限回合的死循环，直到出现胜利跳出
        Putable=p[Cx][Cy]==0;
        Print();//打印棋盘
        input=_getch();//等待键盘按下一个字符
        if(input==0x1B) //如果是ESC则退出
            return 0;
        if(input==' ')//如果是空格则悔棋
        {
            Regret();
            Print();
            continue;
        }
        else if(input=='\r')//如果是回车则开始走子
        {
            if(Put())//如果走子成功则判断胜负
            {
                victor=Check();
                Now=3-Now;//轮换当前走子玩家
                count++;
                if(victor==1)//如果黑方达到胜利，显示提示文字并等待一次按键，返回胜利信息
                {
                    showText="黑方胜利!按R查看回放,按其他键重新开局";
                    Print();
                    input=_getch();
                    if(input==0xE0)
                    {
                        _getch();
                    }
                    else if(input=='R'||input=='r')
                    {
                        ShowReplay();
                    }
                    return Now;
                }
                else if(victor==2)//如果白方达到胜利，显示提示文字并等待一次按键，返回胜利信息
                {
                    showText="白方胜利!按R查看回放,按其他键重新开局";
                    Print();
                    input=_getch();
                    if(input==0xE0)
                    {
                        _getch();
                    }
                    else if(input=='R'||input=='r')
                    {
                        ShowReplay();
                    }
                    return Now;
                }else if(count==MAXIMUS*MAXIMUS)//如果回合数达到了棋盘总量，即棋盘充满，即为平局
                {
                    showText="平局!按R查看回放,按其他键重新开局";
                    Print();
                    input=_getch();
                    if(input==0xE0)
                    {
                        _getch();
                    }
                    else if(input=='R'||input=='r')
                    {
                        ShowReplay();
                    }
                    CleanRecord();
                    return 0;
                }
            }
        }
        else if(input==0xE0)//如果按下的是方向键，会填充两次输入，第一次为0xE0表示按下的是控制键
        {
            input=_getch();//获得第二次输入信息
            switch(input)//判断方向键方向并移动光标位置
            {
            case 0x4B://
                Cx--;
                break;
            case 0x48:
                Cy--;
                break;
            case 0x4D:
                Cx++;
                break;
            case 0x50:
                Cy++;
                break;
            }
            if(Cx<0)Cx=MAXIMUS-1;//如果光标位置越界则移动到对侧
            if(Cy<0)Cy=MAXIMUS-1;
            if(Cx>MAXIMUS-1)Cx=0;
            if(Cy>MAXIMUS-1)Cy=0;
        }
        Exiting=0;//未再次按下ESC则不准备退出
        showText="";
    }
}

int gobang_main(int argc,char **argv)
{
    int res;
	HANDLE hConsoleOutput;
    CONSOLE_SCREEN_BUFFER_INFO info;
    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // 获取控制台输出句柄
    GetConsoleScreenBufferInfo(hConsoleOutput, &info);

    system("mode con cols=63 lines=32");//设置窗口大小
    system("color E8");//设置颜色
    while(1){//循环执行游戏
        res = RunGame();
        if(res == 0)
            break;
    }
    SetConsoleTextAttribute(hConsoleOutput, info.wAttributes);
    system("cls"); // 清屏
    system("mode con cols=100 lines=50");//设置窗口大小
	fflush(stdin);
    return 0;
}
