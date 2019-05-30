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

def ZipExtractor(installArgs, currentZipPath, unzipPath, cachePath, moduleName):
  command = [installArgs.zipExtractor, 'x', currentZipPath, '-o' + unzipPath, '-aoa', '-bb1']

  res = subprocess.Popen(command, stdout=subprocess.DEVNULL).wait()
  if res:
    return False

  return True

def TarGZipExtractor(installArgs, currentZipPath, unzipPath, cachePath, moduleName):
  extractPath = os.path.join(cachePath, moduleName + '.tar')
  
  if not os.path.exists(extractPath):
    command1 = [installArgs.zipExtractor, 'e', currentZipPath, '-o' + extractPath]
    res = subprocess.Popen(command1, stdout=subprocess.DEVNULL).wait()

    if res:
      return False
  
  command2 = [installArgs.zipExtractor, 'x', extractPath, '-o' + unzipPath, '-aoa', '-bb1']
  res = subprocess.Popen(command2, stdout=subprocess.DEVNULL).wait()

  if res:
    return False

  return True

def WindowsInstaller(installArgs, currentExecutable, installPath, cachePath, moduleName):
  command = [os.path.abspath(currentExecutable), '/S', '/D='+ os.path.abspath(installPath)]

  res = subprocess.Popen(command, shell=True).wait()
  if res:
    return False

  return True

def VerySilentWindowsInstaller(installArgs, currentExecutable, installPath, cachePath, moduleName):
  command = [os.path.abspath(currentExecutable), '/VERYSILENT', '/DIR='+ os.path.abspath(installPath)]

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
    },

    'Darwin': {
      'url': 'https://github.com/ninja-build/ninja/releases/download/v1.6.0/ninja-mac.zip'
    }
  })

  installManager.register('Slang', {
    'saveTo': 'Source/Imports/$Platform/Slang/slang-0.11.16/',
    'onDownload': ZipExtractor,

    'Windows': {
      'url': 'https://github.com/shader-slang/slang/releases/download/v0.11.16/slang-0.11.16-win64.zip',
    },

    'Darwin': {
      'url': 'https://github.com/shader-slang/slang/releases/download/v0.11.16/slang-0.11.16-linux-x86_64.zip'
    }
  })

  installManager.register('CMake', {
    'Windows': {
      'saveTo': 'External/$Platform/Tools/CMake/',
      'onDownload': ZipExtractor,
      'url': 'https://cmake.org/files/v3.11/cmake-3.11.4-win64-x64.zip',
    },

    'Darwin': {
      'shell': 'brew install cmake'
    }
  })

  installManager.register('LLVM', {
    'Windows': {
      'saveTo': 'External/$Platform/Tools/LLVM/x64/LLVM/',
      'url': 'http://releases.llvm.org/6.0.1/LLVM-6.0.1-win64.exe',
      'onDownload': WindowsInstaller
    },

    'Darwin': {
      'shell': 'brew install llvm@6'
    }
  })

  installManager.register('Vulkan', {
    'saveTo': 'Source/Imports/$Platform/Vulkan/1.1.77.0/',
    
    'Windows': {
      'url': 'https://vulkan.lunarg.com/sdk/download/1.1.77.0/windows/VulkanSDK-1.1.77.0-Installer.exe',
      'onDownload': WindowsInstaller
    },

    'Darwin': {
      'url': 'https://vulkan.lunarg.com/sdk/download/1.1.77.0/mac/vulkansdk-macos-1.1.77.0.tar.gz',
      'onDownload': TarGZipExtractor,
    }
  })

  installManager.register('Boost', {
    'Windows': {
      'saveTo': 'Source/Imports/$Platform/Boost/boost_1_67_0/',
      'url': 'https://downloads.sourceforge.net/project/boost/boost-binaries/1.67.0/boost_1_67_0-msvc-14.1-64.exe',
      'onDownload': VerySilentWindowsInstaller
    },

    'Darwin': {
      'shell': 'brew install boost@1.69'
    }
  })

  installManager.register('Doxygen', {
    'saveTo': 'External/$Platform/Tools/Doxygen/',
    
    'Windows': {
      'url': 'http://doxygen.nl/files/doxygen-1.8.14.windows.x64.bin.zip',
      'onDownload': ZipExtractor,
    },

    'Darwin': {
      'shell': 'brew install doxygen'
    }
  })

  if installArgs.only == "all":
    installManager.installAll(installArgs)
  else:
    installManager.install(installArgs.only, installArgs)

run()