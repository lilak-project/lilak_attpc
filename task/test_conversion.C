#include "ATConversionTask.cpp"
#include "ATConversionTask.h"

void test_conversion()
{
    auto run = new LKRun();
    run -> Add(new ATConversionTask());
    run -> Init();
}
