$ErrorActionPreference = 'Stop'

$CONFIGURATION = $args[0]
$PLATFORM = $args[1]
$PROJECT_SUBDIR = $args[2]
$PROJECT_OUTPUT = $args[3]



$XMLfile = "F:\builder.xml"
[xml]$xml = Get-Content $XMLfile

$BUILD_HOST = $xml.builder.host
$BUILD_USER = $xml.builder.user
$BUILD_PWD = $xml.builder.pwd

$ROOT_PATH = [System.IO.Path]::GetFullPath("$PSScriptRoot\..\")
$PROJECT_PATH = [String]::Format("{0}:{1}", $ROOT_PATH, $PROJECT_SUBDIR)
$TARGET_PATH="/home/$BUILD_USER/MyProject/$env:UserName/$PROJECT_SUBDIR"

$LOGIN="-l $BUILD_USER -pw $BUILD_PASS"
$OUTPATH=[String]::Format("{0}:{1}", $BUILD_HOST, $TARGET_PATH)


 $env:MYLANG='EN'

function ExecuteCommandOBuildnHost([string]$command) 
{
	$fullCommand = @("-batch", "-ssh",  "-l", $BUILD_USER, "-pw", $BUILD_PWD, $BUILD_HOST, $command)
	Write-Host "plink.exe $fullCommand"
	$timing = Measure-Command { &plink.exe $fullCommand | Out-Default }
	if ($timing.TotalSeconds -gt 1) { Write-Host "Done: ${timing}" }
}



function CopyFilesToBuilder()
{
	Write-Host "creating dir"
	ExecuteCommandOBuildnHost "mkdir -p ", "$TARGET_PATH" 
 
	Write-Host "create archive with source files"
	zip -r -q  src.zip .. -x *.obj *.exe *.pch *.pdb *.idb *.ncb *.dll *.vcproj* *.vcxproj* *.sdf *.htm *.ipch
	Write-Host "copy  files"
	pscp.exe -scp -batch "-l"  $BUILD_USER "-pw"  $BUILD_PWD  src.zip $OUTPATH
	Write-Host "unzipping files"
	ExecuteCommandOBuildnHost "unzip -x -q -u -o", "$TARGET_PATH/src.zip", "-d", "$TARGET_PATH"

	erase  src.zip
}

function CopyFilesFromBuilder([string]$buildPath, [string]$localPath)
{
	Write-Host "copy from build in" $localPath
    $fullBuildPath = [string]::Format('{0}:{1}', $BUILD_HOST, $buildPath)
	$command = "-batch -scp -l $BUILD_USER -pw $BUILD_PWD $fullBuildPath $localPath".Split(" ")
	pscp.exe $command
}


function BuildOnBuilder([string]$cmakePath)
{
	ExecuteCommandOBuildnHost("cp", "$TARGET_PATH/Build/build.sh", "$cmakePath/build.sh")
	ExecuteCommandOBuildnHost("chmod +x", "$cmakePath/build.sh")
	ExecuteCommandOBuildnHost("cd ", "$cmakePath;", "./build.sh $CONFIGURATION")
}


Write-Host([String]::Format("Configuration: {0}, platform: {1}, project: {2}, output: {3}, root: {4}", $CONFIGURATION, $PLATFORM, $PROJECT_SUBDIR, $PROJECT_OUTPUT, $ROOT_PATH))
ExecuteCommandOBuildnHost('echo host time: $(date)')
Write-Host "traget path " $TARGET_PATH


CopyFilesToBuilder


BuildOnBuilder("$TARGET_PATH")
BuildOnBuilder("$TARGET_PATH/Tests/TestLog")

$LIB_PATH = [String]::Format("{0}{1}", $ROOT_PATH, "Binaries\CommonLib.a")
CopyFilesFromBuilder "$TARGET_PATH/$PROJECT_OUTPUT" "$LIB_PATH"