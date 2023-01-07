using System;
using System.Threading.Tasks;

using Hesternal.CSharp;

namespace Hesternal.CSharp.Net7;


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
