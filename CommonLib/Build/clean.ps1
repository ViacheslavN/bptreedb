$ErrorActionPreference = 'Stop'


$PROJECT_SUBDIR = $args[0]


$XMLfile = "F:\builder.xml"
[xml]$xml = Get-Content $XMLfile

$BUILD_HOST = $xml.builder.host
$BUILD_USER = $xml.builder.user
$BUILD_PWD = $xml.builder.pwd


$TARGET_PATH="/home/$BUILD_USER/MyProject/$env:UserName/$PROJECT_SUBDIR"



function ExecuteCommandOBuildnHost([string]$command) {
	$fullCommand = @("-batch", "-ssh",  "-l", $BUILD_USER, "-pw", $BUILD_PWD, $BUILD_HOST, $command)
	Write-Host "plink.exe $fullCommand"
	$timing = Measure-Command { &plink.exe $fullCommand | Out-Default }
	if ($timing.TotalSeconds -gt 1) { Write-Host "Done: ${timing}" }
}

Write-Host "cleaning traget path " $TARGET_PATH

ExecuteCommandOBuildnHost("rm -fdr ", "$TARGET_PATH")
