# build.ps1

# Set paths
$SolutionPath = ".\ProtobufLib.sln"
$OutputDir = ".\output"
$IncludeDir = "$OutputDir\include"
$LibDir = "$OutputDir\lib"

# Create output directories
New-Item -ItemType Directory -Force -Path $OutputDir
New-Item -ItemType Directory -Force -Path $IncludeDir
New-Item -ItemType Directory -Force -Path "$LibDir\x86"
New-Item -ItemType Directory -Force -Path "$LibDir\x64"

# Build configurations
$Configs = @(
    @{Platform="x86"; Config="Release"},
    @{Platform="x64"; Config="Release"}
)

foreach ($cfg in $Configs) {
    # Build using MSBuild
    & "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
        $SolutionPath `
        /p:Configuration=$($cfg.Config) `
        /p:Platform=$($cfg.Platform) `
        /t:Rebuild `
        /m

    # Copy headers
    Copy-Item "src\*.h" -Destination $IncludeDir -Force
    
    # Copy libs based on platform
    if ($cfg.Platform -eq "x86") {
        Copy-Item "Release\ProtobufLib.lib" -Destination "$LibDir\x86" -Force
        Copy-Item "Release\ProtobufLib.dll" -Destination "$LibDir\x86" -Force
    } else {
        Copy-Item "x64\Release\ProtobufLib.lib" -Destination "$LibDir\x64" -Force
        Copy-Item "x64\Release\ProtobufLib.dll" -Destination "$LibDir\x64" -Force
    }
}