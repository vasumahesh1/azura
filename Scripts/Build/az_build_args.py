import argparse

parser = argparse.ArgumentParser()
parser.add_argument("--project", help="Project to build", required=False, default="all")
parser.add_argument("--target", help="Project to build on", required=True)
parser.add_argument("--generator", help="CMake Generator", default="Ninja", required=False)
parser.add_argument("--configFile", help="Config File Override", default="./External/Config.ini", required=False)
parser.add_argument("--cmakeConfigFile", help="CMake Config File Override", default="", required=False)
parser.add_argument("--projectGenerator", help="CMake Project Files Generator", default="Visual Studio 15 2017", required=False)
parser.add_argument("--buildPath", help="Build Path", default="Build/", required=False)
parser.add_argument("--projectPath", help="Projects Path", default="BuildProjects/", required=False)
parser.add_argument("--verbose", help="Verbose Mode", action="store_true", required=False)
parser.add_argument("--debug", help="Debug Commands Mode", action="store_true", required=False)
parser.add_argument("--clean", help="Clean the project", action="store_true", required=False)
parser.add_argument("--build", help="Build Release or Debug, defaults to Debug Mode", default="Debug", required=False)
parser.add_argument("--projectFiles", help="Flag to tell the build system to generate project files", action="store_true")
parser.add_argument("--includeTests", help="Flag to run tests as well", action="store_true")
parser.add_argument("--testRegex", help="Regex to Filter Test groups", default="", required=False)
parser.add_argument("-v", help="Version", action="store_true")
parser.parse_args()

buildArgs = parser.parse_args()

def shouldRun():
	if (buildArgs.v):
		return False

	return True