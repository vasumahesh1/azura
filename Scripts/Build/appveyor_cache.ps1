$testRoot = Test-Path -Path C:\AppveyorCache\
$testNinja = Test-Path -Path C:\AppveyorCache\Ninja\
$testVulkan = Test-Path -Path C:\AppveyorCache\VulkanSDK\
$testBoost = Test-Path -Path C:\AppveyorCache\Boost\

if($testRoot -and $testNinja -and $testVulkan -and $testBoost) {
    echo "Cache already found ..."
    exit 0
}

echo "Preparing Cache Directories ..."
mkdir C:\AppveyorCache\ -ea 0 > $null
mkdir C:\AppveyorCache\Ninja\ -ea 0 > $null
mkdir C:\AppveyorCache\VulkanSDK\ -ea 0 > $null

# Download Ninja
echo "Downloading Ninja ..."
appveyor DownloadFile $env:NINJA_URL -FileName ninja.zip > $null
7z x ninja.zip -oC:\AppveyorCache\Ninja\ > $null

# Download Boost and Build it
echo "Downloading Boost ..."
appveyor DownloadFile $env:BOOST_URL -FileName boost.zip > $null
7z x boost.zip -oC:\AppveyorCache\Boost\ > $null
cd C:\AppveyorCache\Boost\boost_1_68_0\

echo "Building Boost ..."
cmd /c "boostrap.bat && exit" > $null
cmd /c ".\b2 && exit" > $null

# Download and setup Vulkan
echo "Downloading VulkanSDK ..."
cd ..
appveyor DownloadFile $env:VULKAN_URL -FileName Vulkan.exe > $null

echo "Installing VulkanSDK ..."
cmd /c "Vulkan.exe /S && exit" > $null
Copy-Item -Path "C:\VulkanSDK\*" -Destination "C:\AppveyorCache\VulkanSDK\" -Recurse -Force > $null


echo "Dependencies Downloaded."
exit 0