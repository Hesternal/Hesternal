using System.Threading;

using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.Text;

namespace Hesternal.BuildTool.SourceGenerator;


[Generator(LanguageNames.CSharp)]
internal sealed partial class CompilerOptionsSourceGenerator : IIncrementalGenerator
{
    // NOTE(v.matushkin): This counters doesn't work for this generator like for ProjectSourceGenerator
    //   because it runs for every enum individually
    private static int s_InitCount;
    private static int s_EmitCount;


    public void Initialize(IncrementalGeneratorInitializationContext context)
    {
        Interlocked.Increment(ref s_InitCount);

        var compilerOptionsFiles = context.AdditionalTextsProvider
            .Where(static additionalText => additionalText.Path.EndsWith(".xml"));

        context.RegisterSourceOutput(compilerOptionsFiles, _Execute);
    }

    private static void _Execute(SourceProductionContext context, AdditionalText compilerOptionsFile)
    {
        SourceText? compilerOptionsText = compilerOptionsFile.GetText();
        if (compilerOptionsText is not null)
        {
            //Debugger.Launch();
            OptionsSpec? spec = Parser.Parse(context, compilerOptionsText);
            if (spec is not null)
            {
                Emitter.Emit(context, spec);
            }
        }
    }
}
