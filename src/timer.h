
#ifndef __TIMER_H
#define __TIMER_H

#define timer_push_lo(desc) char __push = rand()%10; if (!__push) timer_push(desc "(10x)");
#define timer_pop_lo(desc) if (!__push) timer_pop();

extern void timer_push(char * desc);
extern void timer_pop();
extern void timer_dump();
extern void timer_reset();
extern void timer_frame();
extern void timer_purge();

#endif

