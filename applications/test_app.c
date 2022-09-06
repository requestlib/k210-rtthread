#include<test_app.h>

// 计算20以内的阶乘
void primary_cal_test(rt_uint64_t* src_array, int factorial_number){
    for(int i=1;i<=factorial_number;i++){
        rt_uint64_t single_result = 1;
        for(int j=1;j<=i;j++){
            single_result*=j;
        }
        src_array[i-1]=single_result;
    }
}