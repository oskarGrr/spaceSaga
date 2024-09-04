My own take on the classic space invaders game using the raylib library.

To build: uses CMake and vcpkg (in manifest mode).
Simply run either mingwBuild.bat to use the Mingw Makefiles CMake generator, or
run msvcBuild.bat to use the default CMake generator (visual studio on windows).
I havent tried to build this on linux yet. It should theoretically work, but you would
of course have to use to shell scripts instead of batch files if you wanted to use msvcBuild.bat or mingwBuild.bat.
