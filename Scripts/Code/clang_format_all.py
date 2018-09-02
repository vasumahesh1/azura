import os
import subprocess

from os import walk

def validClangFormatFiles(file):
  global os
  filename, ext = os.path.splitext(file)

  if "Imports" in file:
    return False

  if ext == ".cpp":
    return True

  if ext == ".h":
    return True

selectedFiles = []
for (dirpath, dirnames, filenames) in walk("./Source/"):
  if "Imports" in dirpath:
    continue

  filenames = filter(validClangFormatFiles, filenames)

  currentList = []
  for st in filenames:
    currentList.append(dirpath + '/' + st)

  selectedFiles.extend(currentList)

command = 'clang-format -i ' + ' '.join(selectedFiles)

res = subprocess.Popen(command).wait()

if res:
  raise Exception('Error Executing clang-format')