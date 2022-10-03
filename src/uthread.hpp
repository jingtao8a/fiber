/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-03 19:44:54
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 19:56:38
 * @FilePath: /fiber/src/uthread.hpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AEen
 */
#ifndef _UTHREAD_HPP_
#define _UTHREAD_HPP_

#include <ucontext.h>
#include <vector>

#define DEFAULT_STACK_SZIE (1024*128)
#define MAX_UTHREAD_SIZE   1024

enum ThreadState{FREE,RUNNABLE,RUNNING,SUSPEND};//四种状态

struct schedule_t;

typedef void (*Fun)(void *arg);

//协程类
typedef struct uthread_t
{
    ucontext_t ctx;//对应协程使用的上下文
    Fun func;//指向执行的函数
    void *arg;//指向执行函数的参数
    enum ThreadState state;//该协程的状态
    char stack[DEFAULT_STACK_SZIE];//每个协程使用的各自的栈空间
}uthread_t;

//协程调度类
typedef struct schedule_t
{
    ucontext_t main;//主协程的上下文
    int running_thread;//执行的协程结构体的index
    uthread_t *threads;//指向协程的结构体
    int max_index; // 曾经使用到的最大的index + 1

    schedule_t():running_thread(-1), max_index(0) {
        threads = new uthread_t[MAX_UTHREAD_SIZE];
        for (int i = 0; i < MAX_UTHREAD_SIZE; i++) {
            threads[i].state = FREE;
        }
    }
    
    ~schedule_t() {
        delete [] threads;
    }
}schedule_t;

/*help the thread running in the schedule*/
static void uthread_body(schedule_t *ps);

/*Create a user's thread
*    @param[in]:
*        schedule_t &schedule 
*        Fun func: user's function
*        void *arg: the arg of user's function
*    @param[out]:
*    @return:
*        return the index of the created thread in schedule
*/
int  uthread_create(schedule_t &schedule,Fun func,void *arg);

/* Hang the currently running thread, switch to main thread */
void uthread_yield(schedule_t &schedule);

/* resume the thread which index equal id*/
void uthread_resume(schedule_t &schedule,int id);

/*test whether all the threads in schedule run over
* @param[in]:
*    const schedule_t & schedule 
* @param[out]:
* @return:
*    return 1 if all threads run over,otherwise return 0
*/
int  schedule_finished(const schedule_t &schedule);



#endif