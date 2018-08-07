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
	print bcolors.HEADER + message + bcolors.ENDC

def info(message):
	print bcolors.OKBLUE + message + bcolors.ENDC

def success(message):
	print bcolors.OKGREEN + message + bcolors.ENDC

def fail(message):
	print bcolors.FAIL + message + bcolors.ENDC

def trace(message):
	print message

def warning(message):
	print bcolors.WARNING + message + bcolors.ENDC

def empty():
	print ''

def banner(message):
	header("--------" + repeat_to_length("-", len(message)))
	header("--- %s ---" % message)
	header("--------" + repeat_to_length("-", len(message)))