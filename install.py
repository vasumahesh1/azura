import sys
import os
import errno
sys.path.insert(0, './Scripts/Common/')
sys.path.insert(0, './Scripts/Build/')
sys.path.insert(0, './Scripts/Install/')

import datetime
import subprocess
import platform
from GetInstallArgs import GetInstallArgs
from GetInstallManager import GetInstallManager
from DownloadFile import DownloadFile

def ZipExtractor(installArgs, currentZipPath, unzipPath):
  command = [installArgs.zipExtractor, 'x', currentZipPath, '-o' + unzipPath, '-aoa', '-bb1']

  res = subprocess.Popen(command, stdout=subprocess.DEVNULL).wait()
  if res:
    return False

  return True

def WindowsInstaller(installArgs, currentLLVM, unzipPath):
  command = [os.path.abspath(currentLLVM), '/S', '/D='+ os.path.abspath(unzipPath)]

  res = subprocess.Popen(command, shell=True).wait()
  if res:
    return False

  return True

def BoostWindowsInstaller(installArgs, currentLLVM, unzipPath):
  command = [os.path.abspath(currentLLVM), '/VERYSILENT', '/DIR='+ os.path.abspath(unzipPath)]

  res = subprocess.Popen(command, shell=True).wait()
  if res:
    return False

  return True

def run():
  installArgs = GetInstallArgs()
  installManager = GetInstallManager()

  installManager.register('Ninja', {
    'saveTo': 'External/$Platform/Tools/Ninja/',
    'onDownload': ZipExtractor,

    'Windows': {
      'url': 'https://github.com/ninja-build/ninja/releases/download/v1.6.0/ninja-win.zip'
    }
  })

  installManager.register('Slang', {
    'saveTo': 'Source/Imports/$Platform/Slang/slang-0.11.16/',
    'onDownload': ZipExtractor,

    'Windows': {
      'url': 'https://github.com/shader-slang/slang/releases/download/v0.11.16/slang-0.11.16-win64.zip',
    }
  })

  installManager.register('CMake', {
    'saveTo': 'External/$Platform/Tools/CMake/',
    'onDownload': ZipExtractor,

    'Windows': {
      'url': 'https://cmake.org/files/v3.11/cmake-3.11.4-win64-x64.zip',
    }
  })

  installManager.register('LLVM', {
    'saveTo': 'External/$Platform/Tools/LLVM/x64/LLVM/',
    
    'Windows': {
      'url': 'http://releases.llvm.org/6.0.1/LLVM-6.0.1-win64.exe',
      'onDownload': WindowsInstaller
    }
  })

  installManager.register('Vulkan', {
    'saveTo': 'Source/Imports/$Platform/Vulkan/1.1.77.0/',
    
    'Windows': {
      'url': 'https://vulkan.lunarg.com/sdk/download/1.1.77.0/windows/VulkanSDK-1.1.77.0-Installer.exe',
      'onDownload': WindowsInstaller
    }
  })

  installManager.register('Boost', {
    'saveTo': 'Source/Imports/$Platform/Boost/boost_1_68_0/',
    
    'Windows': {
      'url': 'https://downloads.sourceforge.net/project/boost/boost-binaries/1.67.0/boost_1_67_0-msvc-14.1-64.exe',
      'onDownload': BoostWindowsInstaller
    }
  })

  installManager.register('Doxygen', {
    'saveTo': 'External/$Platform/Tools/Doxygen/',
    'onDownload': ZipExtractor,
    
    'Windows': {
      'url': 'http://doxygen.nl/files/doxygen-1.8.14.windows.x64.bin.zip',
    }
  })

  if installArgs.only == "all":
    installManager.installAll(installArgs)
  else:
    installManager.install(installArgs.only, installArgs)

run()