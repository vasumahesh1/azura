import argparse

def GetInstallArgs():
  parser = argparse.ArgumentParser()
  parser.add_argument("--only", help="Use to specify a certain key to install", default="all", required=False)
  parser.add_argument("--cachePath", help="Cache Folder to store", default="./Cache/", required=False)
  parser.add_argument("--zipExtractor", help="Path to 7z", default="7z", required=False)
  parser.parse_args()

  return parser.parse_args()
