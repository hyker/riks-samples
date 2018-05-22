#!/usr/bin/python

from __future__ import print_function
from future.standard_library import install_aliases
install_aliases()

import os, sys, zipfile, platform as pf
from urllib.request import urlopen
from urllib.error import HTTPError
from io import BytesIO

def download(platform, tag, destination = "."):
    try:
        response = urlopen("https://cdn.hykr.io/res/riks/{0}/{1}/latest/libriks-{0}-{1}.zip".format(platform, tag))
    except HTTPError as err:
    	try:
        	urlopen("https://cdn.hykr.io/res/riks/{0}".format(platform))
        	raise Exception("{0} is not a valid tag for this platform.".format(tag))
    	except HTTPError as err:
	    	try:
	        	urlopen("https://cdn.hykr.io/res/riks")
        		raise Exception("{0} is not a valid platform.".format(platform))
	    	except HTTPError as err:
	        	raise Exception("{0}: {1}".format(err.code, err.reason))

    # Check destination
    if not os.path.isdir(destination):
        os.makedirs(destination)

    zipdata = BytesIO()
    total = int(response.headers["content-length"])
    size = 0
    blockSize = 1024
    lastPercent = 0

    while True:
        block = response.read(blockSize)
        if not block:
            break
        zipdata.write(block)
        size += len(block)
        percent = int(round(100 * size / total))
        if percent > lastPercent:
            hash = ((60 * percent) // 100)
            print("[{}{}] {}%".format('#' * hash, ' ' * (60 - hash), percent) + '\r', end='')
            lastPercent = percent

    with zipfile.ZipFile(zipdata) as z:
        z.extractall(destination)

def main():
    download(sys.argv[2], sys.argv[1])

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print(str(e))
