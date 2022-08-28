# genomus-core

> :warning: **`genomus-core` is currently a work in progress** and it is not ready for its integration and use in the GenoMus project.

The [`GenoMus`](https://github.com/lopezmontes/GenoMus) project is conformed by a theoretical proposal and its implementation in the form of (currently) a prototype of a generative model for semi-automatic music composition. `genomus-core` attempts to implement the core functionalities of the `GenoMus` model in the form of a modern C++ library, with the goal of improving performance and providing a well-formed and maintainable code base. 


_Refer to the original [`GenoMus`](https://github.com/lopezmontes/GenoMus) repository for model specifications and documentation._

## Version 0.1.0

`genomus-core` is currently at version 0.1.0 and includes the following features:

 - Full implementation of the generative grammar
 - Partial implementation of the original function library
 - Genotype creation from random germinal vectors
 - Text parsing functionalitites
 - GenoMus expressions interpreter (CLI)
 - Benchmarking tool for grammar evaluation speed tests
 - TDD tool: GTest

## Project Setup

### Compilation

This project uses the CMake toolkit for an "easy" compilation process. TO compile and run the different executables you will just need to have a C++ compiler and the CMake toolkit installed.

> :information_source: _It is expected that precompiled executables for the different platforms will be available in future versions._

### Dev setup

VSCode is the recommended editor to read and manipulate this code base, as configuration files for an easy setup are included in this repo.
 


