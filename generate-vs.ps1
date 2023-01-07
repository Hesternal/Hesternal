dotnet build --configuration Release .\Hesternal\Source\Tools\HesternalBuildTool\HesternalBuildTool.sln
dotnet publish -p:PublishProfile=FolderProfile .\Hesternal\Source\Tools\HesternalBuildTool\Hesternal.BuildTool.App\Hesternal.BuildTool.App.csproj
dotnet publish -p:PublishProfile=FolderProfile .\Hesternal\Source\Tools\HesternalBuildTool\Hesternal.HeaderTool.Program\Hesternal.HeaderTool.Program.csproj

.\build\bin\dotnet\HesternalBuildTool.exe generate --project-dir=".\Hesternal\Source\"
