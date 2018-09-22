import os
import sys
import time
import datetime
import platform

cacheFile = open('.cicache', "w")
time.ctime()
cacheFile.write('-- AUTO GENERATED FILE --\n')
cacheFile.write('-------------------------\n')
cacheFile.write('use trigger_ci_cache.py to timestamp this file.')
cacheFile.write('\n')
cacheFile.write('\n')
cacheFile.write('Last Requested On:\n')
cacheFile.write(time.strftime('%H:%M %p %Z on %b %d, %Y'))
cacheFile.write('\n')
cacheFile.write('Machine: ' + platform.node())
cacheFile.close()