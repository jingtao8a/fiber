/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-03 19:26:29
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 20:08:58
 * @FilePath: /fiber/tests/testucontext.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE#
 */
#include "../src/uthread.hpp"

#include <stdio.h>

void func1(void * arg)
{
    puts("1");
    puts("11");
    puts("111");
    puts("1111");

}

void func2(void * arg)
{
    puts("22");
    puts("22");
    uthread_yield(*(schedule_t *)arg);
    puts("22");
    puts("22");
}

void func3(void *arg)
{
    puts("3333");
    puts("3333");
    uthread_yield(*(schedule_t *)arg);
    puts("3333");
    puts("3333");

}

void context_test()
{
    char stack[1024*128];
    ucontext_t uc1,ucmain;

    getcontext(&uc1);
    uc1.uc_stack.ss_sp = stack;
    uc1.uc_stack.ss_size = 1024*128;
    uc1.uc_stack.ss_flags = 0;
    uc1.uc_link = &ucmain;
        
    makecontext(&uc1,(void (*)(void))func1,0);

    swapcontext(&ucmain,&uc1);
    puts("main");
}

void schedule_test()
{
    schedule_t s;//定义一个协程调度对象
    
    int id1 = uthread_create(s,func3,&s);//开启协程
    int id2 = uthread_create(s,func2,&s);//开启协程
    
    while(!schedule_finished(s)){
        uthread_resume(s,id2);
        uthread_resume(s,id1);
    }
    puts("main over");

}
int main()
{

    context_test();
    puts("----------------");
    schedule_test();

    return 0;
}

