/*
 * @Author: yuxintao 1921056015@qq.com
 * @Date: 2022-10-03 19:44:47
 * @LastEditors: yuxintao 1921056015@qq.com
 * @LastEditTime: 2022-10-03 20:05:55
 * @FilePath: /fiber/src/uthread.cpp
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */

#include "uthread.hpp"

void uthread_resume(schedule_t &schedule , int id)//切换到指定的协程
{
    if(id < 0 || id >= schedule.max_index){
        return;
    }

    uthread_t *t = &(schedule.threads[id]);

    if (t->state == SUSPEND) {
        swapcontext(&(schedule.main),&(t->ctx));
    }
}

void uthread_yield(schedule_t &schedule)//挂起正在运行的协程，切换为主线程
{
    if(schedule.running_thread != -1 ){
        uthread_t *t = &(schedule.threads[schedule.running_thread]);
        t->state = SUSPEND;
        schedule.running_thread = -1;

        swapcontext(&(t->ctx),&(schedule.main));
    }
}

void uthread_body(schedule_t *ps)
{
    int id = ps->running_thread;

    if(id != -1){
        uthread_t *t = &(ps->threads[id]);

        t->func(t->arg);

        t->state = FREE;
        
        ps->running_thread = -1;
    }
}

int uthread_create(schedule_t &schedule,Fun func,void *arg)
{
    int id = 0;
    
    for(id = 0; id < schedule.max_index; ++id ){
        if(schedule.threads[id].state == FREE){//找到一个空闲的threads单元
            break;
        }
    }
    
    if (id == schedule.max_index) {//每找到，往后加一个
        schedule.max_index++;
    }

    uthread_t *t = &(schedule.threads[id]);

    t->state = RUNNABLE;//READY
    t->func = func;
    t->arg = arg;

    getcontext(&(t->ctx));
    
    t->ctx.uc_stack.ss_sp = t->stack;
    t->ctx.uc_stack.ss_size = DEFAULT_STACK_SZIE;
    t->ctx.uc_stack.ss_flags = 0;
    t->ctx.uc_link = &(schedule.main);
    schedule.running_thread = id;
    
    makecontext(&(t->ctx),(void (*)(void))(uthread_body),1,&schedule);
    swapcontext(&(schedule.main), &(t->ctx));
    
    return id;
}

int schedule_finished(const schedule_t &schedule)
{
    if (schedule.running_thread != -1){
        return 0;
    }else{
        for(int i = 0; i < schedule.max_index; ++i){
            if(schedule.threads[i].state != FREE){
                return 0;
            }
        }
    }

    return 1;
}
