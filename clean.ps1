param(
    [bool] $CppOnly = $true,
    [bool] $KeepVSFiles = $true
)


function RemoveItem ([string]$Path)
{
    Remove-Item $Path -Recurse -Force -ErrorAction Ignore
}

function RemoveItemsExclude ([string]$Path, [string[]]$Exclude)
{
    $items = (Get-ChildItem -Path $Path -Exclude $Exclude).fullname
    if ($items -ne $null)
    {
        Remove-Item -Path $items -Recurse -Force -ErrorAction Ignore
    }
}


$BuildFolder = "$PSScriptRoot\build"
$BuildFolder_Bin          = "$BuildFolder\bin"
$BuildFolder_Generated    = "$BuildFolder\generated"
$BuildFolder_Intermediate = "$BuildFolder\intermediate"
$BuildFolder_Project      = "$BuildFolder\project"
$BuildFolder_Project_VS   = "$BuildFolder_Project\vs"

[string[]] $CppOnlyExcludeList = "dotnet"
[string[]] $KeepVSFilesExcludeList = "Hesternal.Cpp.HeaderTool.props", "Hesternal.Cpp.HeaderTool.targets", "HesternalEditor.vcxproj.user"

RemoveItemsExclude $BuildFolder_Project_VS $KeepVSFilesExcludeList

if ($CppOnly -eq $true)
{
    RemoveItemsExclude $BuildFolder_Bin $CppOnlyExcludeList
    RemoveItemsExclude $BuildFolder_Intermediate $CppOnlyExcludeList
}
else
{
    RemoveItem $BuildFolder_Bin
    RemoveItem $BuildFolder_Intermediate
}
RemoveItem($BuildFolder_Generated)

if ($KeepVSFiles -eq $true)
{
    RemoveItem "$BuildFolder_Project_VS\.vs"
    RemoveItemsExclude $BuildFolder_Project_VS $KeepVSFilesExcludeList
}
else
{
    RemoveItem $BuildFolder_Project_VS
}
