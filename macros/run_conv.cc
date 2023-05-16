#include "LKLogger.hpp"
#include "LKRun.hpp"
#include "ATMFMConversionTask.hpp"

int main()
{
    lk_logger("data/log");

    auto run = LKRun::GetRun();
    run -> SetDataPath("data");
    run -> SetTag("conv");
    run -> AddPar("config_good.json");

    run -> Add(new ATMFMConversionTask());

    run -> Init();
    run -> Print();
    run -> Run();

    return 0;
}
