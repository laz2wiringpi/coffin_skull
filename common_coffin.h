/*
Common_coffin_h 
Version .01

*/
#ifndef Common_coffin_h
#define Common_coffin_h

//#define DEBUG


#include "Arduino.h"


#define NOVALUEINPUT -1



enum pr_stats {
	pr_none,
	pr_before_open_loop,
	pr_open,
	pr_wake_up,
	pr_situp,
	pr_talk,
	pr_sitdown,
	pr_sleep,
	pr_close,
	pr_after_close,
	stat_openandsitdone,
	stat_closeandsitdone

};

enum loop_stats : byte {
	LOOP_START,
	LOOP_RUN,
	LOOP_FINISH


};

#endif


