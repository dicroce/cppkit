
#ifndef cppkit_ck_macro_h_
#define cppkit_ck_macro_h_

#define CK_MACRO_BEGIN do {
#define CK_MACRO_END }while(0)

#define CK_MACRO_END_LOOP_FOREVER }while(1)

#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

#define FULL_MEM_BARRIER __sync_synchronize

#endif
