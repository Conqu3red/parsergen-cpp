


# Parsergen C++
<p>
  <img alt="Version" src="https://img.shields.io/badge/version-1.0.0-blue.svg?cacheSeconds=2592000" />
  <a href="https://github.com/Conqu3red/parsergen-cpp/blob/master/LICENSE" target="_blank">
    <img alt="License: MIT" src="https://img.shields.io/github/license/Conqu3red/parsergen-cpp" />
  </a>
</p>

> A C++ library for writing Lexers and PEG Parsers.

Parsergen C++ provides a toolset for effectively tokenizing strings and generating PEG parsers from grammar definitions.

# Installation
To build the library, git clone the repository and then use make to build.
```bat
make fmt
make libparsergen
```
Include the generated `libparsergen.a` in your compilation command using `-L. -lparsergen`.
Alternatively you can download the static library and header files from the releases page.

Refer to the wiki for details and tutorials on how to use.

Copyright © 2021 [Conqu3red](https://github.com/Conqu3red).