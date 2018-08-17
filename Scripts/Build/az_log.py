import sys

class bcolors:
	HEADER = '\033[95m'
	OKBLUE = '\033[94m'
	OKGREEN = '\033[92m'
	WARNING = '\033[93m'
	FAIL = '\033[91m'
	ENDC = '\033[0m'
	BOLD = '\033[1m'
	UNDERLINE = '\033[4m'

def repeat_to_length(string_to_expand, length):
   return (string_to_expand * ((length/len(string_to_expand))+1))[:length]

def header(message):
	print(bcolors.HEADER + message + bcolors.ENDC)
	sys.stdout.flush()

def info(message):
	print(bcolors.OKBLUE + message + bcolors.ENDC)
	sys.stdout.flush()

def success(message):
	print(bcolors.OKGREEN + message + bcolors.ENDC)
	sys.stdout.flush()

def fail(message):
	print(bcolors.FAIL + message + bcolors.ENDC)
	sys.stdout.flush()

def trace(message):
	print(message)
	sys.stdout.flush()

def warn(message):
	print(bcolors.WARNING + message + bcolors.ENDC)
	sys.stdout.flush()

def empty():
	print('')
	sys.stdout.flush()

def banner(message):
	header("--------" + repeat_to_length("-", len(message)))
	header("--- %s ---" % message)
	header("--------" + repeat_to_length("-", len(message)))

def bannerLarge(message1, message2, message3):
	sizes = [len(message1), len(message2), len(message3)]
	size = max(sizes)
	size += 10

	header(repeat_to_length("=", size))
	header((": %s" + repeat_to_length(" ", size - len(message1) - 3) + ":")  % message1)
	header((": " + repeat_to_length(" ", size -  3) + ":"))
	header((": %s" + repeat_to_length(" ", size - len(message2) - 3) + ":") % message2)
	header((": %s" + repeat_to_length(" ", size - len(message3) - 3) + ":") % message3)
	header(repeat_to_length("=", size))