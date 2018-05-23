# Sabrina
Sabrina stand for (in french) "Sabrina Aide à Bien Rédiger les Idées de Narations et d'Aventures.". 
It's a small program to keep notes on rich fictions universes.

## Warning

Sabrina is in an early stage of developpement and may be a bit buggy, use with care.

## Get and compile the program

It's recommended to keep the build and source folder apart when you compile the program, begin by creating a main directory for Sabrina:

```
mkdir Sabrina
cd Sabrina
```

Then create a source and a build directory:

```
mkdir src
mkdir build
```

Get the program from github:

```
cd src
git clone https://github.com/french-paragon/Sabrina.git
cd Sabrina
git submodule init --recursive
git submodule update --recursive
```

Then go and build the program:

```
cd ../../build
mkdir build_sabrina_release
cd build_sabrina_release
cmake ../../src/Sabrina -DCMAKE_BUILD_TYPE=Release
```

Note: you may need a recent version of Qt5 to build without errors. Personally, on ubuntu, I use the one from [Stephan Binner's ppa](https://launchpad.net/~beineri/+archive/ubuntu/opt-qt-5.10.1-xenial).

You can then run the program:

```
cd ./prog 
./Sabrina
```

It's not recommended to install it for the moment.

Alternatively, the CMakeLists.txt can be opened with QtCreator (tested) or Kdevelop (untested).


## Report issues

Use this github repository to report issues on the program.
