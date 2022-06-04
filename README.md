# xc : cmake wrapper for xmake

It's designed for CLion but it could work with other IDE using cmake, it's actually not tested / supported

# prerequisites
- **xmake** in the path
- **cmake** in the path

# usage
- Replace cmake by the path to **xc** executable in the toolchain options \
  you can also create a a new toolchain using **xc**
 
- Set the build directory to xmake build directory in cmake options (global or project) \
  Example: *build\windows\x64\debug* for debug build type