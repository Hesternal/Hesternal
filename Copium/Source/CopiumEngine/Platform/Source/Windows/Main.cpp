import CopiumEngine.Core.Application;
import CopiumEngine.Core.CoreTypes;

import <memory>;

using namespace Copium;


int32 main(int32 argc, const char* const* argv)
{
    Application* const app = Application::Get();

    app->Init(argc, argv);
    app->Run();
    // TODO(v.matushkin): Shutdown will break if there was an unhandled exception thrown,
    //  cause Application and Systems are static, so the order of destructors is undefined.
    // I can just wrap Application pointer into some dumy object I guess.
    app->Shutdown();

    return 0;
}
