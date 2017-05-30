# Typeshooting
Type x Shooting program by OpenCV through glut over C++

<a href="http://www.youtube.com/watch?feature=player_embedded&v=-AFYTWe-Fbs
" target="_blank"><img src="http://img.youtube.com/vi/-AFYTWe-Fbs/0.jpg" 
alt="movie on youtube" width=40% border="10" /></a>  
[Type shooting on youtube](https://www.youtube.com/watch?v=-AFYTWe-Fbs)  

The prototype progranm for "SD exercise" in 3rd semester of System Design Engineering by Professor Yakoh. 
OpenGL is handled with glut.
Developed in Visual Studio 2013.

## About
- bin
- glut-3.7.6-bin
- header
	- sdglib.h : library 
- resource
	- EnglishDictionary(2).txt: Words used for typing. Put this file in the same directory where ```.exe``` file is in.
- source
	- typeshooting.cpp


## Preparation
Preparation to make graphics work.  

* glut
	- Must be installed.
	- Please follow the instruction in <http://qiita.com/masarkey/items/fdc97c6ff2561aea4ad9> (in Japanese)
	- I tried the Plan A in the website.
		- copy ```glut.h``` in C:\Program Files (x86)\Windows Kits\8.1\Include\um\gl
		- copy ```glut32.lib``` in C:\Program Files (x86)\Windows Kits\8.1\Lib\winv6.3\um\x86
		- copy ```glut32.dll``` in...
			- when 64bit Windows C:\Windows\SysWOW64
			- when 32bit Windows C:\Windows\System32
	- You can use glut files in ```glut-3.7.6-bin``` 
* sdglib.h
	- coded by professors
	- sample program of 2-D graphics of glut
