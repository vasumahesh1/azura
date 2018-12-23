import requests
import az_log

def DownloadFile(url, saveFile):
  print('Downloading: ' + url)
  # 20 min timeout
  res = requests.get(url, stream=True, timeout=1200)
  res.raise_for_status()

  with open(saveFile, 'wb') as downloadFileStream:
    print('Downloading File (can take a while) ... ')
    for chunk in res.iter_content(1024):
        downloadFileStream.write(chunk)

    print('Saved to: ' + saveFile)
    return True

  az_log.error('Failed to download file from URL: ' + url)
  return False