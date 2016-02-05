#!/usr/bin/python
# coding=utf-8

import sys, os, platform, subprocess

try:
	this_dir = os.path.dirname(os.path.realpath(__file__))
	os.chdir(this_dir)

	if platform.system() == "Windows":
		command = "cs-make.exe"
	elif platform.system() == "Linux":
		command = "make"
	else: # 他のOS
		command = "make"

	print command
	print
	p = subprocess.Popen(command, shell=True) # コマンドの実行
	ret = p.wait()
	print
	if ret: sys.exit(1)

except KeyboardInterrupt:
	sys.exit(1)
finally:
	if platform.system() == "Windows": raw_input("press enter to exit")

