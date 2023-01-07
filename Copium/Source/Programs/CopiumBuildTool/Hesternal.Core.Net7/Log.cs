using Microsoft.Extensions.Logging;

namespace Copium.Core.Net7;


public static class Log
{
    private static ILoggerFactory s_Factory;

    public static ILogger Logger;


    public static void Init(LogLevel logLevel)
    {
        s_Factory = LoggerFactory.Create(builder =>
        {
            builder.SetMinimumLevel(logLevel)
                .AddSimpleConsole(options =>
                {
                    options.IncludeScopes = true;
                    options.SingleLine = true;
                });
        });

        Logger = s_Factory.CreateLogger("CopiumBuildTool");
    }

    public static void Shutdown()
    {
        s_Factory.Dispose();
    }


    public static ILogger<TCategoryName> CreateLogger<TCategoryName>()
    {
        return s_Factory.CreateLogger<TCategoryName>();
    }

    public static ILogger CreateLogger(string categoryType)
    {
        return s_Factory.CreateLogger(categoryType);
    }
}
