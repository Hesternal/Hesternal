$env:CONAN_USER_HOME=$(Get-Location)
$env:CONAN_USER_HOME_SHORT="None"

conan config set general.revisions_enabled=1

conan install . -if build\conan\vs -s build_type=Debug -o development=False
conan install . -if build\conan\vs -s build_type=Release -o development=True
conan install . -if build\conan\vs -s build_type=Release -o development=False
