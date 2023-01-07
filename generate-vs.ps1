dotnet build --configuration Release .\Copium\Source\Programs\CopiumBuildTool\HesternalBuildTool.sln
dotnet publish -p:PublishProfile=FolderProfile .\Copium\Source\Programs\CopiumBuildTool\Copium.BuildTool.App\Hesternal.BuildTool.App.csproj
dotnet publish -p:PublishProfile=FolderProfile .\Copium\Source\Programs\CopiumBuildTool\Copium.HeaderTool.Program\Hesternal.HeaderTool.Program.csproj

.\build\bin\dotnet\HesternalBuildTool.exe generate --project-dir=".\Copium\Source\"
