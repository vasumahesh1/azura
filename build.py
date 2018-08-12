import sys
import os
sys.path.insert(0, './Scripts/Build/')

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
	if (platform == 'Windows'):
		ext = '.exe'

	executableMap['ninja'] = config['ninja'] + '/ninja' + ext
	executableMap['cmake'] = config['cmake'] + '/cmake' + ext
	executableMap['clang-tidy'] = config['llvm'] + '/clang-tidy' + ext

def printConfig(item):
	for key, value in item.iteritems():
		print("Using %s:" % key, value)

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

	processEnv['PATH'] = hostExternalConfig['ninja'] + ";" + hostExternalConfig['msvcpath'] + "/bin/Hostx64/x64/;" + hostExternalConfig['windows10sdkbin'] + "/x64/;" + processEnv['PATH'];
	processEnv['LIB'] = hostExternalConfig['msvcpath'] + '/lib/x64/;' + hostExternalConfig['windows10sdklib'] + '/ucrt/x64/;' + hostExternalConfig['windows10sdklib'] + '/um/x64/;'
	processEnv['INCLUDE'] = hostExternalConfig['msvcpath'] + '/include/;' + hostExternalConfig['windows10sdkinc'] + '/um/;' + hostExternalConfig['windows10sdkinc'] + '/shared/;' + hostExternalConfig['windows10sdkinc'] + '/ucrt/;';

def configureLinux():
	global hostExternalConfig
	global executableMap
	global processEnv

	processEnv['PATH'] = hostExternalConfig['clang'] + ":"
	processEnv['LIB'] = hostExternalConfig['clang'] + '/lib/:'
	processEnv['INCLUDE'] = hostExternalConfig['clang'] + '/include/:'

def run():
	global hostExternalConfig
	global executableMap
	global processEnv

	hostOS = platform.system()

	az_log.banner("Azura Build System")
	az_log.trace("Host OS: %s" % hostOS)

	if (buildArgs.v):
		print("Version: " + VERSION)
		return

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

	if (buildArgs.cmakeConfigFile):
		az_log.warn('Found a Custom CMake Config')
		cmakeConfig = ConfigParser.ConfigParser()
		cmakeConfig.read(buildArgs.cmakeConfigFile)
		cmakeDefines = getRawConfigMap(cmakeConfig, 'Defines')
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

run()