dotnet build --configuration Release .\Copium\Source\Programs\CopiumBuildTool\CopiumBuildTool.sln
dotnet publish -p:PublishProfile=FolderProfile .\Copium\Source\Programs\CopiumBuildTool\Copium.BuildTool\Copium.BuildTool.csproj
dotnet publish -p:PublishProfile=FolderProfile .\Copium\Source\Programs\CopiumBuildTool\Copium.HeaderTool.Program\Copium.HeaderTool.Program.csproj

.\build\bin\dotnet\CopiumBuildTool.exe generate
