dotnet build --configuration Release .\Copium\Source\Programs\HesternalBuildTool\HesternalBuildTool.sln
dotnet publish -p:PublishProfile=FolderProfile .\Copium\Source\Programs\HesternalBuildTool\Hesternal.BuildTool.App\Hesternal.BuildTool.App.csproj
dotnet publish -p:PublishProfile=FolderProfile .\Copium\Source\Programs\HesternalBuildTool\Hesternal.HeaderTool.Program\Hesternal.HeaderTool.Program.csproj

.\build\bin\dotnet\HesternalBuildTool.exe generate --project-dir=".\Copium\Source\"
