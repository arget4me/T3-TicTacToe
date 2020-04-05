# T3 
Server Client TicTacToe

Imgui for GUI
GLFW for window managment
OpenGl for grapichs. (GLEW to load opengl function pointers)
GLM for math operations



## Visual Studio Setup

#C/C++
Additional include directories:
$(SolutionDir)dependencies\include\IMGUI
$(SolutionDir)dependencies\include
$(SolutionDir)src\


#Linker
Additional library directories: //Using visual studio 2019, change glfw for wanted version
$(SolutionDir)dependencies\lib\GLEW
$(SolutionDir)dependencies\lib\GLFW\lib-vc2019

Additional dependencies:
glfw3.lib
opengl32.lib
glew32s.lib

SubSystem: //main() if Console, winMain if Windows
Console (/SUBSYSTEM:CONSOLE)
Windows (/SUBSYSTEM:WINDOWS)