//#include "TestLayer.h"
#include "JobSystem/JobScheduler.h"
#include "Windowing/WindowProperties.h"
#include "Core/Application.h"
#include "Core/EntryPoint.h"
//#include "BeeEngine.h"
using namespace BeeEngine;
class Game: public BeeEngine::Application
{
public:
    JobScheduler m_Scheduler{};
    Game(const WindowProperties& properties)
    : Application(properties)
    {

    }

    void Update() override
    {
        static uint32_t m_FrameCount = 0;
        BeeTrace("Hello from frame {}!", ++m_FrameCount);
        for (int i = 0; i < 1; ++i)
        {
            m_Scheduler.Schedule([i, this]()
                                 {
                                     //BeeInfo("Hello from job {}!", i);
                                     Counter counter;
                                     for (int j = 0; j < 1; ++j)
                                     {
                                         m_Scheduler.Schedule([i, j, this]()
                                                              {
                                                                  uint32_t a = 0;
                                                                  Counter counter;
                                                                  //BeeInfo("Hello from job {} {}!", i, j);
                                                                  for(int k = 0; k < 10; ++k)
                                                                  {
                                                                      m_Scheduler.Schedule([i, j, k]()
                                                                                           {
                                                                                               uint32_t a = 0;
                                                                                               for(int k = 0; k < 1000000; ++k)
                                                                                               {
                                                                                                   a ++;
                                                                                               }
                                                                                               //BeeInfo("Hello from job {} {} {}!", i, j, k);
                                                                                           }, counter);
                                                                  }
                                                                  m_Scheduler.WaitForJobsToComplete(counter);
                                                                  for(int k = 0; k < 10; ++k)
                                                                  {
                                                                      m_Scheduler.Schedule([i, j, k]()
                                                                                           {
                                                                                               uint32_t a = 0;
                                                                                               for(int k = 0; k < 10000000; ++k)
                                                                                               {
                                                                                                   a ++;
                                                                                               }
                                                                                               //BeeInfo("Zhu from job {} {} {}!", i, j, k);
                                                                                           }, counter);
                                                                  }
                                                                  m_Scheduler.WaitForJobsToComplete(counter);
                                                                  //BeeInfo("Bye from job {} {}!", i, j);
                                                              }, counter);
                                     }
                                     m_Scheduler.WaitForJobsToComplete(counter);
                                     //BeeInfo("Bye from job {}!", i);
                                 });
        }
    }

    virtual ~Game() override
    {

    }
};

gsl::not_null<BeeEngine::Application*> BeeEngine::CreateApplication(const BeeEngine::ApplicationArgs& args)
{
    constexpr static WindowProperties properties = {1280, 720, "Pochemu", VSync::On, RenderAPI::WebGPU};
    return new Game(properties);
}
