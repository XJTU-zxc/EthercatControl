#ifndef LIB_MYSTACK_H
#define LIB_MYSTACK_H

#include<stdio.h>
#include<stdlib.h>
using namespace std;

#define MAXSIZE 100
typedef uint32_t ElemType;

typedef struct {
    ElemType data[MAXSIZE];
    int top;
}MyStack;

MyStack mystack;
extern MyStack mystack;
/* 栈初始化 */
void Init_Stack(MyStack& S)
{
    S.top = -1;
}


/* 栈判空 */
bool isEmpty(MyStack S)
{
    if (S.top==-1)
    {
        return true;
    }
    return false;
}

/* 栈判满 */
bool isFull(MyStack S)
{
    if (S.top == MAXSIZE - 1)
    {
        return true;
    }
    return false;
}
/* 入栈  */
bool Push(MyStack& S, ElemType e)
{
    bool flag = isFull(S);
    if (flag==true)
    {
        return false;
    }
    S.top++;
    S.data[S.top] = e;
    return true;

}

/* 出 栈 */
bool Pop(MyStack& S, ElemType& e)
{
    bool flag = isEmpty(S);
    if (flag)
    {
        return false;
    }
    e = S.data[S.top];
    S.top--;
    return true;
}
/*  取栈顶  */
bool GetTop(MyStack& S, ElemType& e)
{
    bool flag = isEmpty(S);
    if (flag)
    {
        return false;
    }
    e = S.data[S.top];
    return true;
}

#endif