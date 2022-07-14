using System;
using System.Threading.Tasks;

using Copium.Core;

namespace Copium.BuildTool.Core;


internal sealed class TaskPool
{
    public static TaskScheduler Scheduler { get; private set; }
    public static TaskFactory Factory { get; private set; }


    public static void Init()
    {
        Scheduler = new LimitedConcurrencyLevelTaskScheduler(Environment.ProcessorCount);
        Factory = new TaskFactory(Scheduler);
    }
}
