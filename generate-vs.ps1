dotnet build --configuration Release .\Hesternal\Source\Tools\HesternalTools.sln
dotnet publish -p:PublishProfile=FolderProfile .\Hesternal\Source\Tools\Hesternal.BuildTool.App\Hesternal.BuildTool.App.csproj
dotnet publish -p:PublishProfile=FolderProfile .\Hesternal\Source\Tools\Hesternal.HeaderTool.Program\Hesternal.HeaderTool.Program.csproj

.\build\bin\dotnet\HesternalBuildTool.exe generate --project-dir=".\Hesternal\Source\"
