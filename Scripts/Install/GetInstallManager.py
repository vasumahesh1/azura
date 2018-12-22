import argparse
import collections
import os
import platform
import subprocess
import copy

from DownloadFile import DownloadFile
from MakeDirectoryRecursive import MakeDirectoryRecursive
import az_log

def ZipExtractor(installArgs, currentZipPath, unzipPath):
  command = [installArgs.zipExtractor, 'x', currentZipPath, '-o' + unzipPath, '-aoa', '-bb1']

  res = subprocess.Popen(command, stdout=subprocess.DEVNULL).wait()
  if res:
    return False

  return True

def LLVMWindowsInstaller(installArgs, currentLLVM, unzipPath):
  command = [os.path.abspath(currentLLVM), '/S', '/D='+ os.path.abspath(unzipPath)]

  res = subprocess.Popen(command, shell=True).wait()
  if res:
    return False

  return True


class InstallManager(object):
    dependencyMap = None

    def __init__(self):
      self.dependencyMap = {}

    def register(self, event, eventData):
      self.dependencyMap[event] = eventData

    def installAll(self, installArgs):
      for key in self.dependencyMap:
        self.install(key, installArgs)

    def install(self, event, installArgs):
      az_log.info('Installing: ' + event)
      hostOS = platform.system()

      dependencyData = copy.deepcopy(self.dependencyMap[event])

      if not dependencyData:
        az_log.fail('Failed find dependency: ' + event)
        return

      if dependencyData[hostOS]:
        for key in dependencyData[hostOS]:
          dependencyData[key] = dependencyData[hostOS][key]

      fileExtension = dependencyData['url'].split('.')[-1]
      fileName = event.lower() + '_' + hostOS.lower() + '.' + fileExtension

      temporaryPath = os.path.join(installArgs.cachePath, fileName)

      if os.path.exists(temporaryPath):
        az_log.warn('Skipping download as required file was found in cache (located at: ' + installArgs.cachePath + ')')
        res = True
      else:
        res = DownloadFile(dependencyData['url'], temporaryPath)

      if not res:
        if os.path.exists(temporaryPath):
          os.remove(temporaryPath)
        az_log.fail('Failed to download dependency: ' + event)
        return


      modifiedSavePath = dependencyData['saveTo'].replace('$Platform', hostOS)

      if not os.path.exists(modifiedSavePath):
        MakeDirectoryRecursive(modifiedSavePath)

      res = dependencyData['onDownload'](installArgs, temporaryPath, modifiedSavePath)

      if not res:
        az_log.fail('Failed to execute onDownload script for dependency: ' + event)
        return

      az_log.success('Done: ' + event)
      az_log.empty()


def GetInstallManager():
  # installables = {
  #   # "Vulkan": {
  #   #   "url": "",
  #   #   "downloadAs": "vulkan.exe",
  #   #   "saveTo": None, # Vulkan has a different installation procedure
  #   #   "onDownload": lambda downloadedPath: True
  #   # }
  #   "Ninja": 
  # }

  manager = InstallManager()

  manager.register('Ninja', {
    'saveTo': 'External/$Platform/Tools/Ninja/',
    'onDownload': ZipExtractor,

    'Windows': {
      'url': 'https://github.com/ninja-build/ninja/releases/download/v1.6.0/ninja-win.zip'
    }
  })

  manager.register('Slang', {
    'saveTo': 'Source/Imports/$Platform/Slang/slang-0.11.16/',
    'onDownload': ZipExtractor,

    'Windows': {
      'url': 'https://github.com/shader-slang/slang/releases/download/v0.11.16/slang-0.11.16-win64.zip',
    }
  })

  manager.register('CMake', {
    'saveTo': 'External/$Platform/Tools/CMake/',
    'onDownload': ZipExtractor,

    'Windows': {
      'url': 'https://cmake.org/files/v3.11/cmake-3.11.4-win64-x64.zip',
    }
  })

  manager.register('LLVM', {
    'saveTo': 'External/$Platform/Tools/LLVM/x64/LLVM/',
    
    'Windows': {
      'url': 'http://releases.llvm.org/6.0.1/LLVM-6.0.1-win64.exe',
      'onDownload': LLVMWindowsInstaller
    }
  })

  manager.register('Vulkan', {
    'saveTo': 'Source/Imports/$Platform/Vulkan/1.1.77.0/',
    
    'Windows': {
      'url': 'https://vulkan.lunarg.com/sdk/download/1.1.77.0/windows/VulkanSDK-1.1.77.0-Installer.exe',
      'onDownload': LLVMWindowsInstaller
    }
  })

  manager.register('Doxygen', {
    'saveTo': 'External/$Platform/Tools/Doxygen/',
    'onDownload': ZipExtractor,
    
    'Windows': {
      'url': 'http://doxygen.nl/files/doxygen-1.8.14.windows.x64.bin.zip',
    }
  })

  return manager
