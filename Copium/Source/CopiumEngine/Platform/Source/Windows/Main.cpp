import CopiumEngine.Application;
import CopiumEngine.Core.CoreTypes;

#include <memory>

using namespace Copium;


int32 main(int32 argc, const char* const* argv)
{
    std::unique_ptr<Application> app = CreateApplication(argc, argv);

    app->Run();

    return 0;
}
