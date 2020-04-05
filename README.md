# T3 
Server Client TicTacToe

Imgui for GUI<br />
GLFW for window managment<br />
OpenGl for grapichs. (GLEW to load opengl function pointers)<br />
GLM for math operations<br />



## Visual Studio Setup

------ C/C++ -----<br />
Additional include directories:<br />
$(SolutionDir)dependencies\include\IMGUI<br />
$(SolutionDir)dependencies\include<br />
$(SolutionDir)src\ <br />


----- Linker -----<br />
Additional library directories: //Using visual studio 2019, change glfw for wanted version<br />
$(SolutionDir)dependencies\lib\GLEW<br />
$(SolutionDir)dependencies\lib\GLFW\lib-vc2019<br />

Additional dependencies:<br />
glfw3.lib<br />
opengl32.lib<br />
glew32s.lib<br />

SubSystem: //main() if Console, winMain if Windows<br />
Console (/SUBSYSTEM:CONSOLE)<br />
Windows (/SUBSYSTEM:WINDOWS)<br />
