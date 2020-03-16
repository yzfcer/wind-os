#include "wind_config.h"
#if (WIND_MODULE_GAME_SUPPORT)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
 
#ifdef _MSC_VER // M$的编译器要给予特殊照顾
  #if _MSC_VER <= 1200 // VC6及以下版本
    #error 你是不是还在用VC6？！
  #else // VC6以上版本
    #if _MSC_VER >= 1600 // 据说VC10及以上版本有stdint.h了
      #include <stdint.h>
    #else // VC10以下版本，自己定义int8_t和uint16_t
      typedef signed char int8_t;
      typedef unsigned short uint16_t;
    #endif
    #ifndef __cplusplus // 据说VC都没有stdbool.h，不用C++编译，自己定义bool
      typedef int bool;
      #define true 1
      #define false 0
    #endif
  #endif
#else // 其他的编译器都好说
  #include <stdint.h>
  #ifndef __cplusplus // 不用C++编译，需要stdbool.h里的bool
    #include <stdbool.h>
  #endif
#endif
 
// =============================================================================
// 7种方块的4旋转状态（4位为一行）
static const uint16_t gs_uTetrisTable[7][4] =
{
  { 0x00F0U, 0x2222U, 0x00F0U, 0x2222U }, // I型
  { 0x0072U, 0x0262U, 0x0270U, 0x0232U }, // T型
  { 0x0223U, 0x0074U, 0x0622U, 0x0170U }, // L型
  { 0x0226U, 0x0470U, 0x0322U, 0x0071U }, // J型
  { 0x0063U, 0x0264U, 0x0063U, 0x0264U }, // Z型
  { 0x006CU, 0x0462U, 0x006CU, 0x0462U }, // S型
  { 0x0660U, 0x0660U, 0x0660U, 0x0660U }  // O型
};
 
// =============================================================================
// 初始状态的游戏池
// 每个元素表示游戏池的一行，下标大的是游戏池底部
// 两端各置2个1，底部2全置为1，便于进行碰撞检测
// 这样一来游戏池的宽度为12列
// 如果想要传统的10列，只需多填两个1即可（0xE007），当然显示相关部分也要随之改动
// 当某个元素为0xFFFFU时，说明该行已被填满
// 顶部4行用于给方块，不显示出来
// 再除去底部2行，显示出来的游戏池高度为22行
static const uint16_t gs_uInitialTetrisPool[28] =
{
  0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U,
  0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U,
  0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U,
  0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xFFFFU, 0xFFFFU
};
 
#define COL_BEGIN 2
#define COL_END 14
#define ROW_BEGIN 4
#define ROW_END 26
 
// =============================================================================
typedef struct TetrisManager // 这个结构体存储游戏相关数据
{
  uint16_t pool[28]; // 游戏池
  int8_t x; // 当前方块x坐标，此处坐标为方块左上角坐标
  int8_t y; // 当前方块y坐标
  int8_t type[3]; // 当前、下一个和下下一个方块类型
  int8_t orientation[3]; // 当前、下一个和下下一个方块旋转状态
  unsigned score; // 得分
  unsigned erasedCount[4]; // 消行数
  unsigned erasedTotal; // 消行总数
  unsigned tetrisCount[7]; // 各方块数
  unsigned tetrisTotal; // 方块总数
  bool dead; // 挂
} TetrisManager;
 
// =============================================================================
typedef struct TetrisControl // 这个结构体存储控制相关数据
{
  bool pause; // 暂停
  bool clockwise; // 旋转方向：顺时针为true
  int8_t direction; // 移动方向：0向左移动 1向右移动
  // 游戏池内每格的颜色
  // 由于此版本是彩色的，仅用游戏池数据无法存储颜色信息
  // 当然，如果只实现单色版的，就没必要用这个数组了
  int8_t color[28][16];
} TetrisControl;
 
HANDLE g_hConsoleOutput; // 控制台输出句柄
 
// =============================================================================
// 函数声明
// 如果使用全局变量方式实现，就没必要传参了
void initGame(TetrisManager *manager, TetrisControl *control); // 初始化游戏
void restartGame(TetrisManager *manager, TetrisControl *control); // 重新开始游戏
void giveTetris(TetrisManager *manager); // 给一个方块
bool checkCollision(const TetrisManager *manager); // 碰撞检测
void insertTetris(TetrisManager *manager); // 插入方块
void removeTetris(TetrisManager *manager); // 移除方块
void horzMoveTetris(TetrisManager *manager, TetrisControl *control); // 水平移动方块
void moveDownTetris(TetrisManager *manager, TetrisControl *control); // 向下移动方块
void rotateTetris(TetrisManager *manager, TetrisControl *control); // 旋转方块
void dropDownTetris(TetrisManager *manager, TetrisControl *control); // 方块直接落地
bool checkErasing(TetrisManager *manager, TetrisControl *control); // 消行检测
void keydownControl(TetrisManager *manager, TetrisControl *control, int key); // 键按下
void setPoolColor(const TetrisManager *manager, TetrisControl *control); // 设置颜色
void gotoxyWithFullwidth(short x, short y); // 以全角定位
void printPoolBorder(); // 显示游戏池边界
void printTetrisPool(const TetrisManager *manager, const TetrisControl *control); // 显示游戏池
void printCurrentTetris(const TetrisManager *manager, const TetrisControl *control); // 显示当前方块
void printNextTetris(const TetrisManager *manager); // 显示下一个和下下一个方块
void printScore(const TetrisManager *manager); // 显示得分信息
int runGame(TetrisManager *manager, TetrisControl *control); // 运行游戏
void printPrompting(); // 显示提示信息
bool ifPlayAgain(); // 再来一次
 
// =============================================================================

// =============================================================================
// 初始化游戏
void initGame(TetrisManager *manager, TetrisControl *control)
{
  CONSOLE_CURSOR_INFO cursorInfo = { 1, FALSE }; // 光标信息
 
  g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // 获取控制台输出句柄
  SetConsoleCursorInfo(g_hConsoleOutput, &cursorInfo); // 设置光标隐藏
  SetConsoleTitleA("俄罗斯方块");
 
  restartGame(manager, control);
}
 
// =============================================================================
// 重新开始游戏
void restartGame(TetrisManager *manager, TetrisControl *control)
{
  memset(manager, 0, sizeof(TetrisManager)); // 全部置0
 
  // 初始化游戏池
  memcpy(manager->pool, gs_uInitialTetrisPool, sizeof(uint16_t [28]));
  srand((unsigned)time(NULL)); // 设置随机种子
 
  manager->type[1] = rand() % 7; // 下一个
  manager->orientation[1] = rand() & 3;
 
  manager->type[2] = rand() % 7; // 下下一个
  manager->orientation[2] = rand() & 3;
 
  memset(control, 0, sizeof(TetrisControl)); // 全部置0
 
  giveTetris(manager); // 给下一个方块
  setPoolColor(manager, control); // 设置颜色
}
 
// =============================================================================
// 给一个方块
void giveTetris(TetrisManager *manager)
{
  uint16_t tetris;
 
  manager->type[0] = manager->type[1]; // 下一个方块置为当前
  manager->orientation[0] = manager->orientation[1];
 
  manager->type[1] = manager->type[2];// 下下一个置方块为下一个
  manager->orientation[1] = manager->orientation[2];
 
  manager->type[2] = rand() % 7;// 随机生成下下一个方块
  manager->orientation[2] = rand() & 3;
 
  tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]]; // 当前方块
 
  // 设置当前方块y坐标，保证刚给出时只显示方块最下面一行
  // 这种实现使得玩家可以以很快的速度将方块落在不显示出来的顶部4行内
  if (tetris & 0xF000)
  {
    manager->y = 0;
  }
  else
  {
    manager->y = (tetris & 0xFF00) ? 1 : 2;
  }
  manager->x = 6; // 设置当前方块x坐标
 
  if (checkCollision(manager)) // 检测到碰撞
  {
    manager->dead = true; // 标记游戏结束
  }
  else // 未检测到碰撞
  {
    insertTetris(manager); // 将当前方块加入游戏池
  }
 
  ++manager->tetrisTotal; // 方块总数
  ++manager->tetrisCount[manager->type[0]]; // 相应方块数
 
  printNextTetris(manager); // 显示下一个方块
  printScore(manager); // 显示得分信息
}
 
// =============================================================================
// 碰撞检测
bool checkCollision(const TetrisManager *manager)
{
  // 当前方块
  uint16_t tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]];
  uint16_t dest = 0;
 
  // 获取当前方块在游戏池中的区域：
  // 游戏池坐标x y处小方格信息，按低到高存放在16位无符号数中
  dest |= (((manager->pool[manager->y + 0] >> manager->x) << 0x0) & 0x000F);
  dest |= (((manager->pool[manager->y + 1] >> manager->x) << 0x4) & 0x00F0);
  dest |= (((manager->pool[manager->y + 2] >> manager->x) << 0x8) & 0x0F00);
  dest |= (((manager->pool[manager->y + 3] >> manager->x) << 0xC) & 0xF000);
 
  // 若当前方块与目标区域存在重叠（碰撞），则位与的结果不为0
  return ((dest & tetris) != 0);
}
 
// =============================================================================
// 插入方块
void insertTetris(TetrisManager *manager)
{
  // 当前方块
  uint16_t tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]];
 
  // 当前方块每4位取出，位或到游戏池相应位置，即完成插入方块
  manager->pool[manager->y + 0] |= (((tetris >> 0x0) & 0x000F) << manager->x);
  manager->pool[manager->y + 1] |= (((tetris >> 0x4) & 0x000F) << manager->x);
  manager->pool[manager->y + 2] |= (((tetris >> 0x8) & 0x000F) << manager->x);
  manager->pool[manager->y + 3] |= (((tetris >> 0xC) & 0x000F) << manager->x);
}
 
// =============================================================================
// 移除方块
void removeTetris(TetrisManager *manager)
{
  // 当前方块
  uint16_t tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]];
 
  // 当前方块每4位取出，按位取反后位与到游戏池相应位置，即完成移除方块
  manager->pool[manager->y + 0] &= ~(((tetris >> 0x0) & 0x000F) << manager->x);
  manager->pool[manager->y + 1] &= ~(((tetris >> 0x4) & 0x000F) << manager->x);
  manager->pool[manager->y + 2] &= ~(((tetris >> 0x8) & 0x000F) << manager->x);
  manager->pool[manager->y + 3] &= ~(((tetris >> 0xC) & 0x000F) << manager->x);
}
 
// =============================================================================
// 设置颜色
void setPoolColor(const TetrisManager *manager, TetrisControl *control)
{
  // 由于显示游戏池时，先要在游戏池里判断某一方格有方块才显示相应方格的颜色
  // 这里只作设置即可，没必要清除
  // 当移动方块或给一个方块时调用
 
  int8_t i, x, y;
 
  // 当前方块
  uint16_t tetris = gs_uTetrisTable[manager->type[0]][manager->orientation[0]];
 
  for (i = 0; i < 16; ++i)
  {
    y = (i >> 2) + manager->y; // 待设置的列
    if (y > ROW_END) // 超过底部限制
    {
      break;
    }
    x = (i & 3) + manager->x; // 待设置的行
    if ((tetris >> i) & 1) // 检测的到小方格属于当前方块区域
    {
      control->color[y][x] = (manager->type[0] | 8); // 设置颜色
    }
  }
}
 
// =============================================================================
// 旋转方块
void rotateTetris(TetrisManager *manager, TetrisControl *control)
{
  int8_t ori = manager->orientation[0]; // 记录原旋转状态
 
  removeTetris(manager); // 移走当前方块
 
  // 顺/逆时针旋转
  manager->orientation[0] = (control->clockwise) ? ((ori + 1) & 3) : ((ori + 3) & 3);
 
  if (checkCollision(manager)) // 检测到碰撞
  {
    manager->orientation[0] = ori; // 恢复为原旋转状态
    insertTetris(manager); // 放入当前方块。由于状态没改变，不需要设置颜色
  }
  else
  {
    insertTetris(manager); // 放入当前方块
    setPoolColor(manager, control); // 设置颜色
    printCurrentTetris(manager, control); // 显示当前方块
  }
}
 
// =============================================================================
// 水平移动方块
void horzMoveTetris(TetrisManager *manager, TetrisControl *control)
{
  int x = manager->x; // 记录原列位置
 
  removeTetris(manager); // 移走当前方块
  control->direction == 0 ? (--manager->x) : (++manager->x); // 左/右移动
 
  if (checkCollision(manager)) // 检测到碰撞
  {
    manager->x = x; // 恢复为原列位置
    insertTetris(manager); // 放入当前方块。由于位置没改变，不需要设置颜色
  }
  else
  {
    insertTetris(manager); // 放入当前方块
    setPoolColor(manager, control); // 设置颜色
    printCurrentTetris(manager, control); // 显示当前方块
  }
}
 
// =============================================================================
// 向下移动方块
void moveDownTetris(TetrisManager *manager, TetrisControl *control)
{
  int8_t y = manager->y; // 记录原行位置
 
  removeTetris(manager); // 移走当前方块
  ++manager->y; // 向下移动
 
  if (checkCollision(manager)) // 检测到碰撞
  {
    manager->y = y; // 恢复为原行位置
    insertTetris(manager); // 放入当前方块。由于位置没改变，不需要设置颜色
    if (checkErasing(manager, control)) // 检测到消行
    {
      printTetrisPool(manager, control); // 显示游戏池
    }
  }
  else
  {
    insertTetris(manager); // 放入当前方块
    setPoolColor(manager, control); // 设置颜色
    printCurrentTetris(manager, control); // 显示当前方块
  }
}
 
// =============================================================================
// 方块直接落地
void dropDownTetris(TetrisManager *manager, TetrisControl *control)
{
  removeTetris(manager); // 移走当前方块
  for (; manager->y < ROW_END; ++manager->y) // 从上往下
  {
    if (checkCollision(manager)) // 检测到碰撞
    {
      break;
    }
  }
  --manager->y; // 上移一格当然没有碰撞
 
  insertTetris(manager); // 放入当前方块
  setPoolColor(manager, control); // 设置颜色
 
  checkErasing(manager, control); // 检测消行
  printTetrisPool(manager, control); // 显示游戏池
}
 
// =============================================================================
// 消行检测
bool checkErasing(TetrisManager *manager, TetrisControl *control)
{
  static const unsigned scores[5] = { 0, 10, 30, 90, 150 }; // 消行得分
  int8_t count = 0;
  int8_t k = 0, y = manager->y + 3; // 从下往上检测
 
  do
  {
    if (y < ROW_END && manager->pool[y] == 0xFFFFU) // 有效区域内且一行已填满
    {
      ++count;
      // 消除一行方块
      memmove(manager->pool + 1, manager->pool, sizeof(uint16_t) * y);
      // 颜色数组的元素随之移动
      memmove(control->color[1], control->color[0], sizeof(int8_t [16]) * y);
    }
    else
    {
      --y;
      ++k;
    }
  } while (y >= manager->y && k < 4);
 
  manager->erasedTotal += count; // 消行总数
  manager->score += scores[count]; // 得分
 
  if (count > 0)
  {
    ++manager->erasedCount[count - 1]; // 消行
  }
 
  giveTetris(manager); // 给下一个方块
  setPoolColor(manager, control); // 设置颜色
 
  return (count > 0);
}
 
// =============================================================================
// 键按下
void keydownControl(TetrisManager *manager, TetrisControl *control, int key)
{
  if (key == 13) // 暂停/解除暂停
  {
    control->pause = !control->pause;
  }
 
  if (control->pause) // 暂停状态，不作处理
  {
    return;
  }
 
  switch (key)
  {
  case 'w': case 'W': case '8': case 72: // 上
    control->clockwise = true; // 顺时针旋转
    rotateTetris(manager, control); // 旋转方块
    break;
  case 'a': case 'A': case '4': case 75: // 左
    control->direction = 0; // 向左移动
    horzMoveTetris(manager, control); // 水平移动方块
    break;
  case 'd': case 'D': case '6': case 77: // 右
    control->direction = 1; // 向右移动
    horzMoveTetris(manager, control); // 水平移动方块
    break;
  case 's': case 'S': case '2': case 80: // 下
    moveDownTetris(manager, control); // 向下移动方块
    break;
  case ' ': // 直接落地
    dropDownTetris(manager, control);
    break;
  case '0': // 反转
    control->clockwise = false; // 逆时针旋转
    rotateTetris(manager, control); // 旋转方块
    break;
  default:
    break;
  }
}
 
// =============================================================================
// 以全角定位
void gotoxyWithFullwidth(short x, short y)
{
  static COORD cd;
 
  cd.X = (short)(x << 1);
  cd.Y = y;
  SetConsoleCursorPosition(g_hConsoleOutput, cd);
}
 
// =============================================================================
// 显示游戏池边界
void printPoolBorder()
{
  int8_t y;
 
  SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
  gotoxyWithFullwidth(10, 0); // 底部边界
  printf("%28s", "");
  for (y = ROW_BEGIN; y < ROW_END; ++y) // 不显示顶部4行和底部2行
  {
    gotoxyWithFullwidth(10, y - 3);
    printf("%2s", "");
    gotoxyWithFullwidth(23, y - 3);
    printf("%2s", "");
  }
 
  gotoxyWithFullwidth(10, y - 3); // 底部边界
  printf("%28s", "");
}
 
// 定位到游戏池中的方格
#define gotoxyInPool(x, y) gotoxyWithFullwidth(x + 9, y - 3)
 
// =============================================================================
// 显示游戏池
void printTetrisPool(const TetrisManager *manager, const TetrisControl *control)
{
  int8_t x, y;
 
  for (y = ROW_BEGIN; y < ROW_END; ++y) // 不显示顶部4行和底部2行
  {
    gotoxyInPool(2, y); // 定点到游戏池中的方格
    for (x = COL_BEGIN; x < COL_END; ++x) // 不显示左右边界
    {
      if ((manager->pool[y] >> x) & 1) // 游戏池该方格有方块
      {
        // 用相应颜色，显示一个实心方块
        SetConsoleTextAttribute(g_hConsoleOutput, control->color[y][x]);
        printf("■");
      }
      else // 没有方块，显示空白
      {
        SetConsoleTextAttribute(g_hConsoleOutput, 0);
        printf("%2s", "");
      }
    }
  }
}
 
// =============================================================================
// 显示当前方块
void printCurrentTetris(const TetrisManager *manager, const TetrisControl *control)
{
  int8_t x, y;
 
  // 显示当前方块是在移动后调用的，为擦去移动前的方块，需要扩展显示区域
  // 由于不可能向上移动，故不需要向下扩展
  y = (manager->y > ROW_BEGIN) ? (manager->y - 1) : ROW_BEGIN; // 向上扩展一格
  for (; y < ROW_END && y < manager->y + 4; ++y)
  {
    x = (manager->x > COL_BEGIN) ? (manager->x - 1) : COL_BEGIN; // 向左扩展一格
    for (; x < COL_END && x < manager->x + 5; ++x) // 向右扩展一格
    {
      gotoxyInPool(x, y); // 定点到游戏池中的方格
      if ((manager->pool[y] >> x) & 1) // 游戏池该方格有方块
      {
        // 用相应颜色，显示一个实心方块
        SetConsoleTextAttribute(g_hConsoleOutput, control->color[y][x]);
        printf("■");
      }
      else // 没有方块，显示空白
      {
        SetConsoleTextAttribute(g_hConsoleOutput, 0);
        printf("%2s", "");
      }
    }
  }
}
 
// =============================================================================
// 显示下一个和下下一个方块
void printNextTetris(const TetrisManager *manager)
{
  int8_t i;
  uint16_t tetris;
 
  // 边框
  SetConsoleTextAttribute(g_hConsoleOutput, 0xF);
  gotoxyWithFullwidth(26, 1);
  printf("┏━━━━┳━━━━┓");
  gotoxyWithFullwidth(26, 2);
  printf("┃%8s┃%8s┃", "", "");
  gotoxyWithFullwidth(26, 3);
  printf("┃%8s┃%8s┃", "", "");
  gotoxyWithFullwidth(26, 4);
  printf("┃%8s┃%8s┃", "", "");
  gotoxyWithFullwidth(26, 5);
  printf("┃%8s┃%8s┃", "", "");
  gotoxyWithFullwidth(26, 6);
  printf("┗━━━━┻━━━━┛");
 
  // 下一个，用相应颜色显示
  tetris = gs_uTetrisTable[manager->type[1]][manager->orientation[1]];
  SetConsoleTextAttribute(g_hConsoleOutput, manager->type[1] | 8);
  for (i = 0; i < 16; ++i)
  {
    gotoxyWithFullwidth((i & 3) + 27, (i >> 2) + 2);
    ((tetris >> i) & 1) ? printf("■") : printf("%2s", "");
  }
 
  // 下下一个，不显示彩色
  tetris = gs_uTetrisTable[manager->type[2]][manager->orientation[2]];
  SetConsoleTextAttribute(g_hConsoleOutput, 8);
  for (i = 0; i < 16; ++i)
  {
    gotoxyWithFullwidth((i & 3) + 32, (i >> 2) + 2);
    ((tetris >> i) & 1) ? printf("■") : printf("%2s", "");
  }
}
 
// =============================================================================
// 显示得分信息
void printScore(const TetrisManager *manager)
{
  static const char *tetrisName = "ITLJZSO";
  int8_t i;
 
  SetConsoleTextAttribute(g_hConsoleOutput, 0xE);
 
  gotoxyWithFullwidth(2, 2);
  printf("■得分：%u", manager->score);
 
  gotoxyWithFullwidth(1, 6);
  printf("■消行总数：%u", manager->erasedTotal);
  for (i = 0; i < 4; ++i)
  {
    gotoxyWithFullwidth(2, 8 + i);
    printf("□消%d：%u", i + 1, manager->erasedCount[i]);
  }
 
  gotoxyWithFullwidth(1, 15);
  printf("■方块总数：%u", manager->tetrisTotal);
 
  for (i = 0; i < 7; ++i)
  {
    gotoxyWithFullwidth(2, 17 + i);
    printf("□%c形：%u", tetrisName[i], manager->tetrisCount[i]);
  }
}
 
// =============================================================================
// 显示提示信息
void printPrompting()
{
  SetConsoleTextAttribute(g_hConsoleOutput, 0xB);
  gotoxyWithFullwidth(26, 10);
  printf("■控制：");
  gotoxyWithFullwidth(27, 12);
  printf("□向左移动：← A 4");
  gotoxyWithFullwidth(27, 13);
  printf("□向右移动：→ D 6");
  gotoxyWithFullwidth(27, 14);
  printf("□向下移动：↓ S 2");
  gotoxyWithFullwidth(27, 15);
  printf("□顺时针转：↑ W 8");
  gotoxyWithFullwidth(27, 16);
  printf("□逆时针转：0");
  gotoxyWithFullwidth(27, 17);
  printf("□直接落地：空格");
  gotoxyWithFullwidth(27, 18);
  printf("□暂停游戏：回车");
  gotoxyWithFullwidth(25, 23);
}
 
// =============================================================================
// 运行游戏
int runGame(TetrisManager *manager, TetrisControl *control)
{
  char ch; 
  clock_t clockLast, clockNow;
 
  clockLast = clock(); // 计时
  printTetrisPool(manager, control); // 显示游戏池
 
  while (!manager->dead) // 没挂
  {
    while (_kbhit()) // 有键按下
    {
      ch = _getch();
      if(ch == 27)
        return -1;
      keydownControl(manager, control,ch); // 处理按键
    }
 
    if (!control->pause) // 未暂停
    {
      clockNow = clock(); // 计时
      // 两次记时的间隔超过0.45秒
      if (clockNow - clockLast > 0.45F * CLOCKS_PER_SEC)
      {
        clockLast = clockNow;
        keydownControl(manager, control, 80); // 方块往下移
      }
    }
  }
  return 0;
}
 
// =============================================================================
// 再来一次
bool ifPlayAgain()
{
  int ch;
 
  SetConsoleTextAttribute(g_hConsoleOutput, 0xF0);
  gotoxyWithFullwidth(15, 10);
  printf("游戏结束");
  gotoxyWithFullwidth(13, 11);
  printf("按Y重玩，按N退出");
 
  do
  {
    ch = _getch();
    if (ch == 'Y' || ch == 'y')
    {
      return true;
    }
    else if (ch == 'N' || ch == 'n')
    {
      return false;
    }
  } while (1);
}

// 主函数
int tetris_main(int argc,char **argv)
{
  int res;
  CONSOLE_SCREEN_BUFFER_INFO info;
  TetrisManager tetrisManager;
  TetrisControl tetrisControl;
  CONSOLE_CURSOR_INFO cursorInfo = { 1, TRUE }; // 光标信息
  
  g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // 获取控制台输出句柄
  GetConsoleScreenBufferInfo(g_hConsoleOutput, &info);
  initGame(&tetrisManager, &tetrisControl); // 初始化游戏

  do
  {
    printPrompting(); // 显示提示信息
    printPoolBorder(); // 显示游戏池边界
    res = runGame(&tetrisManager, &tetrisControl); // 运行游戏
    if(res != 0)
        break;
    if (ifPlayAgain()) // 再来一次
    {
      SetConsoleTextAttribute(g_hConsoleOutput, 0x7);
      system("cls"); // 清屏
      restartGame(&tetrisManager, &tetrisControl); // 重新开始游戏
    }
    else
    {
      break;
    }
  } while (1);
  gotoxyWithFullwidth(0, 0);
  SetConsoleTextAttribute(g_hConsoleOutput, info.wAttributes);
  system("cls"); // 清屏
  system("mode con cols=100 lines=50");//设置窗口大小
  g_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE); // 获取控制台输出句柄
  SetConsoleCursorInfo(g_hConsoleOutput, &cursorInfo); // 设置光标隐藏
  return 0;
}
#endif

