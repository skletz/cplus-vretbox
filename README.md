# VRETBOX

VRETBOX is a Video Retrieval Evaluation Toolbox developed for the evaluation of content descriptions methods to make collections of videos searchable.

As an example serves the large-scale video collection from TRECVID. TRECVID is a yearly workshop for evaluation of video search approaches. It is a workshop which is part of the Text Retrieval Conference (TREC). The workshop consists of different disciplines and also provides datasets for various tasks. Finally, the evaluated results of all proposed approaches serve as a well-used benchmark in literature.

## Dependencies
To build you need the following libraries:
- [**OpenCV**](http://opencv.org/downloads.html) is tested with OpenCV 3.1
- [**Boost**](http://www.boost.org/users/history/version_1_59_0.html) is tested with Boost 1.59.0
- cpluslogger
- cplusutil
- defuse

### Windows Configuration
 1. Extract  OpenCV to C:/libraries/opencv_3_1
 2. Extract  Boost to C:/libraries/boost_1_59_0
 3. Create a new system variable 
	- Name: **OpenCV_DIR**
	- Value: C:\libraries\opencv_3_1\opencv\build
	- Name: **Boost_DIR**
	- Value: C:\libraries\boost_1_59_0
 4.  Add to system variable: 
	 -Name: Path
	 -Value: ...;**%OpenCV_DIR%\x64\vc14\bin**
 5. Set the VRET_DATA and VRET_EVAL system variable to use these in config files 

### Linux Configuration

## Makefile
```bash
$ make opencv=opt os=linux
# opencv=opt only necessary if opencv is not installed in /usr/local/
```

## Visual Studio 2015 Solution

### Visual Studio Project Properties
 1. Additional include directories (C/C++ -> General)
    - $(OpenCV_DIR)\include
    - $(Boost_DIR)
    - $(PATH-TO)\cplusutil\include
    - $(PATH-TO)\cpluslogger\include
    - $(PATH-TO)\defuse\include
 2. Additional library directories (Linker -> General)
    - $(OpenCV_DIR)\x64\vc14\lib
    - $(OpenCV_DIR)\x64\vc14\bin
    - $(Boost_DIR)\stage\lib\vc140\x64
 2. Additional dependencies (Linker -> Input)
    - opencv_world310d.lib;cpluslogger_d.lib;cplusutil_d.lib;opencv-histlib_d.lib;opencv-pctsig_d.lib;opencv-tfsig_d.lib;defuse_d.lib
 4. Disable security warnings  (C/C++ -> Preprocessor)
	- Add **_CRT_SECURE_NO_WARNINGS** to Preprocessor Definitions
      