import argparse
import collections
import os
import platform
import subprocess
import copy

from DownloadFile import DownloadFile
from MakeDirectoryRecursive import MakeDirectoryRecursive
import az_log


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

      if not dependencyData[hostOS]:
        az_log.warn('Skipping dependency as config for OS not found: ' + event.lower())
        res = True
        return

      # Mirror hostOS data into dependencyData
      if dependencyData[hostOS]:
        for key in dependencyData[hostOS]:
          dependencyData[key] = dependencyData[hostOS][key]

      if 'url' in dependencyData:
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

        res = dependencyData['onDownload'](installArgs, temporaryPath, modifiedSavePath, installArgs.cachePath, event.lower())

        if not res:
          az_log.fail('Failed to execute onDownload script for dependency: ' + event)
          return

        print('Installed to: ' + os.path.abspath(modifiedSavePath))

        az_log.success('Done: ' + event)
        az_log.empty()

      elif 'shell' in dependencyData:
        res = subprocess.Popen(dependencyData['shell'].split(' ')).wait()

        if res:
          raise Exception('Error Executing install command: ' + dependencyData['shell'])

        az_log.success('Done: ' + event)
        az_log.empty()


def GetInstallManager():
  manager = InstallManager()
  return manager
