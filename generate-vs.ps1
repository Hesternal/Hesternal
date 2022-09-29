dotnet build --configuration Release .\Copium\Source\Programs\CopiumBuildTool\CopiumBuildTool.sln
dotnet publish -p:PublishProfile=FolderProfile .\Copium\Source\Programs\CopiumBuildTool\Copium.BuildTool.App\Copium.BuildTool.App.csproj
dotnet publish -p:PublishProfile=FolderProfile .\Copium\Source\Programs\CopiumBuildTool\Copium.HeaderTool.Program\Copium.HeaderTool.Program.csproj

.\build\bin\dotnet\CopiumBuildTool.exe generate --project-dir=".\Copium\Source\"
