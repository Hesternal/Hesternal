using System;
using System.Threading.Tasks;

using Hesternal.Core;

namespace Hesternal.Core.Net7;


public static class TaskPool
{
    public static TaskScheduler Scheduler { get; private set; }
    public static TaskFactory Factory { get; private set; }


    public static void Init()
    {
        Scheduler = new LimitedConcurrencyLevelTaskScheduler(Environment.ProcessorCount);
        Factory = new TaskFactory(Scheduler);
    }
}
