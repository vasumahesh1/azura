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

def run():
  installArgs = GetInstallArgs()
  installManager = GetInstallManager()

  if installArgs.only == "all":
    installManager.installAll(installArgs)
  else:
    installManager.install(installArgs.only, installArgs)

run()