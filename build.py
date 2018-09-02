import sys
import os
sys.path.insert(0, './Scripts/Build/')

import datetime
import subprocess
import platform
import ConfigParser
import az_log
import az_build_args

VERSION = "1.0.0"
buildArgs = az_build_args.buildArgs

processEnv = os.environ.copy()
hostExternalConfig = {}
executableMap = {}

currentBuildArch = "64"

def getRawConfigMap(config, section):
    resultMap = {}

    if (not config.has_section(section)):
      return resultMap

    options = config.options(section)
    for option in options:
        try:
            resultMap[option] = config.get(section, option)
        except:
            print("exception on %s!" % option)
            resultMap[option] = None
    return resultMap

def getConfigMap(config, section):
    resultMap = {}
    relativePath = "External/" + section + "/"

    if (not config.has_section(section)):
      return resultMap

    options = config.options(section)
    for option in options:
        try:
            finalPath = config.get(section, option) if os.path.isabs(config.get(section, option)) else os.path.abspath(relativePath + config.get(section, option))
            resultMap[option] = finalPath
        except:
            print("exception on %s!" % option)
            resultMap[option] = None
    return resultMap

def getDefines(defineMap):
  arr = []
  for key, value in defineMap.iteritems():
    arr.append('-D' + key.upper() + '=' + value)

  return arr

def buildExecutableMap(config, platform):
  global executableMap

  ext = ''
  sep = '/'
  if (platform == 'Windows'):
    ext = '.exe'
    sep = '\\'

  executableMap['ninja'] = config['ninja'] + sep + 'ninja' + ext
  executableMap['cmake'] = config['cmake'] + sep + 'cmake' + ext
  executableMap['ctest'] = config['cmake'] + sep + 'ctest' + ext
  executableMap['clang-tidy'] = config['llvm'] + sep + 'clang-tidy' + ext

def printConfig(item):
  for key, value in item.iteritems():
    message = "Using %32s = " % key.upper()
    message = message + ("%s" % value)
    print(message)

def executeCommand(command, override=False):
  global processEnv

  if (override):
    res = subprocess.Popen(command, env=processEnv).wait()
    if res:
      raise Exception('Error Executing one or more build commands')
    return

  if (buildArgs.debug):
    print(command)
  else:
    res = subprocess.Popen(command, env=processEnv).wait()

    if res:
      raise Exception('Error Executing one or more build commands')

def configureWindows():
  global hostExternalConfig
  global executableMap
  global processEnv
  global currentBuildArch
  global buildArgs

  processEnv['INCLUDE'] = hostExternalConfig['msvcpath'] + '/include/;' + hostExternalConfig['windows10sdkinc'] + '/um/;' + hostExternalConfig['windows10sdkinc'] + '/shared/;' + hostExternalConfig['windows10sdkinc'] + '/ucrt/;';
  processEnv['PATH'] = hostExternalConfig['ninja'] + ";" + hostExternalConfig['msvcpath'] + "/bin/Hostx64/x64/;" + hostExternalConfig['windows10sdkbin'] + "/x64/;" + processEnv['PATH'];
  processEnv['LIB'] = hostExternalConfig['msvcpath'] + '/lib/x64/;' + hostExternalConfig['windows10sdklib'] + '/ucrt/x64/;' + hostExternalConfig['windows10sdklib'] + '/um/x64/;'

  if (currentBuildArch == "64"):
    buildArgs.projectGenerator = buildArgs.projectGenerator + ' Win64'

def configureLinux():
  global hostExternalConfig
  global executableMap
  global processEnv

  processEnv['PATH'] = hostExternalConfig['clang'] + ":"
  processEnv['LIB'] = hostExternalConfig['clang'] + '/lib/:'
  processEnv['INCLUDE'] = hostExternalConfig['clang'] + '/include/:'

def printTimeDelta(seconds):
    seconds = int(seconds)
    days, seconds = divmod(seconds, 86400)
    hours, seconds = divmod(seconds, 3600)
    minutes, seconds = divmod(seconds, 60)

    if days > 0:
        return '%dd %dh %dm %ds' % (days, hours, minutes, seconds)
    elif hours > 0:
        return '%dh %dm %ds' % (hours, minutes, seconds)
    elif minutes > 0:
        return '%dm %ds' % (minutes, seconds)
    else:
        return '%ds' % (seconds)

def setCompileSettings():
  global currentBuildArch

  if (buildArgs.target == 'Win64'):
    return

  if (buildArgs.target == 'Win32'):
    currentBuildArch = "32"

def addTargetCmakeArgs(cmakeArgs):
  if (buildArgs.target == 'Win64'):
    cmakeArgs.append("-DBUILD_TARGET=Win64")
    cmakeArgs.append("-DBUILD_ARCH=64")
    cmakeArgs.append("-DBUILD_PLATFORM=Windows")
    return cmakeArgs

  if (buildArgs.target == 'Win32'):
    cmakeArgs.append("-DBUILD_TARGET=Win32")
    cmakeArgs.append("-DBUILD_ARCH=32")
    cmakeArgs.append("-DBUILD_PLATFORM=Windows")
    return cmakeArgs

  return cmakeArgs

def run():
  global hostExternalConfig
  global executableMap
  global processEnv

  buildStartTime = datetime.datetime.now()

  hostOS = platform.system()

  az_log.banner("Azura Build System")
  az_log.trace("Host OS: %s" % hostOS)

  if (buildArgs.v):
    print("Version: " + VERSION)
    return

  setCompileSettings()

  externalConfig = ConfigParser.ConfigParser()
  externalConfig.read(buildArgs.configFile)
  hostExternalConfig = getConfigMap(externalConfig, hostOS)
  
  az_log.empty()
  az_log.info('> Build Config:')
  printConfig(hostExternalConfig)

  az_log.empty()
  
  az_log.info('> Executable Config:')
  buildExecutableMap(hostExternalConfig, hostOS)
  printConfig(executableMap)

  cmakeDefines = {}
  if (buildArgs.cmakeConfigFile):
    az_log.empty()
    az_log.warn('> Custom CMake Override Config')
    cmakeConfig = ConfigParser.ConfigParser()
    cmakeConfig.read(buildArgs.cmakeConfigFile)
    cmakeDefines = getRawConfigMap(cmakeConfig, 'Defines')
    printConfig(cmakeDefines)
    az_log.flush();

  cmakeCommand = [executableMap['cmake']]

  if (buildArgs.debug):
    print('Building %s Environment' % hostOS)

  if (hostOS == 'Windows'):
    configureWindows()
  elif (hostOS == 'Linux'):
    configureLinux()

  az_log.empty()
  az_log.banner("Configuring Build Files")

  cmakeArgs = []
  cmakeArgs.append('-G' + buildArgs.generator)
  cmakeArgs.append("-B%s" % buildArgs.buildPath)
  cmakeArgs.append("-H.")
  cmakeArgs.append("-DCMAKE_BUILD_TYPE=" + buildArgs.build.upper())
  cmakeArgs.append("-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")
  cmakeArgs.append("-DCMAKE_CXX_CLANG_TIDY=" + executableMap['clang-tidy'] + ";-extra-arg=-std=c++17")

  cmakeArgs = addTargetCmakeArgs(cmakeArgs)

  if (buildArgs.cmakeConfigFile):
    cmakeArgs = cmakeArgs + getDefines(cmakeDefines)

  if (buildArgs.verbose):
    cmakeArgs.append("--trace")

  finalCommand = cmakeCommand + cmakeArgs
  executeCommand(finalCommand)

  if (buildArgs.projectFiles):
    az_log.empty()
    az_log.banner("Creating Project Files at: " + buildArgs.projectPath)
    print("Using Generator: " + buildArgs.projectGenerator)

    cmakeArgs = []
    cmakeArgs.append('-G' + buildArgs.projectGenerator)
    cmakeArgs.append("-B%s" % buildArgs.projectPath)
    cmakeArgs.append("-H.")
    cmakeArgs.append("-DCMAKE_BUILD_TYPE=" + buildArgs.build.upper())

    cmakeArgs = addTargetCmakeArgs(cmakeArgs)

    if (buildArgs.cmakeConfigFile):
      cmakeArgs = cmakeArgs + getDefines(cmakeDefines)

    if (buildArgs.verbose):
      cmakeArgs.append("--trace")

    finalCommand = cmakeCommand + cmakeArgs
    executeCommand(finalCommand)


  os.chdir('./' + buildArgs.buildPath)

  ninjaBuildArgs = [executableMap['ninja'], buildArgs.project]

  if (buildArgs.clean):
    ninjaBuildArgs.append('-tclean')

  if (buildArgs.verbose):
    ninjaBuildArgs.append("-v")

  az_log.empty()
  az_log.banner("Building Projects")

  executeCommand(ninjaBuildArgs)

  if (buildArgs.includeTests):
    az_log.empty()
    az_log.banner("Running Tests")

    ctestArgs = [executableMap['ctest']]
    ctestArgs.append('-DGTEST_COLOR=1')
    ctestArgs.append('--verbose')

    executeCommand(ctestArgs)


  buildEndTime = datetime.datetime.now()
  delta = printTimeDelta((buildEndTime - buildStartTime).total_seconds())

  buildTimings = 'Started At: %s' % buildStartTime
  buildTimings += '  -->  '
  buildTimings += 'Completed At: %s' % buildEndTime

  az_log.bannerLarge('Build Finished', buildTimings, 'Time Taken: %s' % delta)

run()