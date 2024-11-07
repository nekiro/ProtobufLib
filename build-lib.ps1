# build.ps1

$SolutionPath = ".\ProtobufLib.sln"
$OutputDir = ".\output"
$IncludeDir = "$OutputDir\include"
$LibDir = "$OutputDir\lib"

# Create output directories
Remove-Item -Recurse -Force $OutputDir
New-Item -ItemType Directory -Force -Path $OutputDir
New-Item -ItemType Directory -Force -Path $IncludeDir
New-Item -ItemType Directory -Force -Path "$LibDir\x86"
New-Item -ItemType Directory -Force -Path "$LibDir\x64"

# Build configurations
$Configs = @(
    @{Platform="x86"; Config="Release"},
    @{Platform="x64"; Config="Release"}
)

$HeaderFiles = @(
    "src\spriteappearances.h",
    "src\appearances.h"
)

foreach ($cfg in $Configs) {
    # Build using MSBuild
    & "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" `
        $SolutionPath `
        /p:Configuration=$($cfg.Config) `
        /p:Platform=$($cfg.Platform) `
        /t:Rebuild `
        /m

	foreach ($file in $HeaderFiles) {
		Copy-Item $file -Destination $IncludeDir -Force
	}
    
    # Copy libs based on platform
    if ($cfg.Platform -eq "x86") {
        Copy-Item "Release\ProtobufLib.lib" -Destination "$LibDir\x86" -Force
        Copy-Item "Release\ProtobufLib.dll" -Destination "$LibDir\x86" -Force
    } else {
        Copy-Item "x64\Release\ProtobufLib.lib" -Destination "$LibDir\x64" -Force
        Copy-Item "x64\Release\ProtobufLib.dll" -Destination "$LibDir\x64" -Force
    }
}