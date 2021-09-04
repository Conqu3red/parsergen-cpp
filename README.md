


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
To build the library, git clone the repository and then use make to build `fmt` then `parsergen-cpp`.
```sh
make fmt
make parsergen-cpp
```
Include the generated `libparsergen.a` in your compilation command using `-L. -lparsergen`.
Alternatively you can download the static library and header files from the releases page.
The executable `parsergen-cpp.exe` can be used to generate a C++ file from a grammar file by doing:
```
parsergen-cpp somefile.gram -o somefile.cpp
```

```sh
# other make rules:
make libparsergen # libparsergen.a
make json # json parser example
make calc # basic calculator example
make test # tests
make metagrammar # recompile metagrammar
```


Refer to the wiki for details and tutorials on how to use.

Copyright © 2021 [Conqu3red](https://github.com/Conqu3red).