### LIMINAL ENGINE
Liminal engine is a 3d graphical engine. This project was created as a proof of concept and was partially used in the development of some unfinished games I worked on

## Dependencies
**GLEW:** Glew is used as a wrangler for this project <br />
**GLFW:** Although Window.h provides some functionality and logic related to opening a window, this is mainly handled using GLFW<br />
**stb_image:** Textures are not currently supported, however this library is included to read png image data since this was an important feature in other versions of this engine<br />
<br />
While it may be possible to compile this project on other operating systems / platforms, it has only ever been developed on windows and I have not worked to provide support for other platforms

## Main.cpp
This project was intended to be used for other programs and is not useful on its own. To showcase the capabilities of the project, main.cpp implements a **very** simple sandbox program.
A gif of this program running can be found on my website: https://rpovey.com/ (This avoids you having to download the required dependencies and having to figure out how to compile this project yourself)

## Notes / misc
The goal of this project was for it to be written in pure c++. Although some external libraries were used, I feel that since all logic and processing uses c++ this goal is achieved<br />

This project was not developed using git, rather it was finished and then put into a repository. This is mainly because it was built from an old version of the engine with some parts changed and some left the same<br />
This explains the lack of a version history and the overall strange layout of this repo