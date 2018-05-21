#!/usr/bin/python

from __future__ import print_function
from future.standard_library import install_aliases
install_aliases()

import os, sys, zipfile, platform as pf
from urllib.request import urlopen
from urllib.error import HTTPError
from io import BytesIO

configurations = {
    "linux": {
        "x86":    "x86",
        "x86_64": "x64",
    },
    "osx": {
        "x86":    "x86",
        "x86_64": "x64"
    },
    "ios": {
    },
    "vs2015": {
        "X86":    "x86",
        "AMD64":  "x64"
    },
    "vs2017": {
        "X86":    "x86",
        "AMD64":  "x64"
    },
    "uwp": {
        "X86":    "x86",
        "AMD64":  "x64"
    }
}

def defaultTag(platform):
    uname = pf.machine()
    if uname not in configurations[platform]:
        raise Exception("Platform is not supported.")
    
    return configurations[platform][uname]
    
def defaultPlatform():
    if pf.system() == "Darwin":  return "osx"
    if pf.system() == "Linux":   return "linux"
    else: raise Exception("Could not automatically determine platform.")

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
    platform = sys.argv[2] if len(sys.argv) > 2 else defaultPlatform()
    tag = sys.argv[1] if len(sys.argv) > 1 else defaultTag(platform)
    download(platform, tag)

if __name__ == '__main__':
    try:
        main()
    except Exception as e:
        print(str(e))
