# LearnOpenGL-Qt
[![GitHub actions](https://github.com/ABBAPOH/learnopengl-qt/actions/workflows/main.yml/badge.svg)](https://github.com/ABBAPOH/learnopengl-qt/actions/workflows/main.yml?query=event%3Apush)

An Qt-based implementation of the examples from the [learnopengl.com](https://learnopengl.com)
website.

The original examples from the website can be found
[here](https://github.com/JoeyDeVries/LearnOpenGL/tree/master).

## Building From Source

### Install Qt

You can install Qt from the online installer from the Qt [website](https://www.qt.io/download-dev).

Alternatively, you can use the `install-qt.sh` script from the `scripts` directory.
```
$ ./scripts/install-qt.sh -d $HOME/Qt --version 6.7.0 --target desktop qtbase
```
This script requqres Bash-like Shell.

#### Ubuntu / Debian

```
$ sudo apt install qt5-default
```

#### macOS
Qt is available in [Brew](https://formulae.brew.sh/formula/qt)
or [macports](https://ports.macports.org/port/qt6/).
```
$ brew install qt
```
or
```
$ sudo port install qt6
```

#### Windows

Use the online installer from the Qt [website](https://www.qt.io/download-dev)

### Install Qbs

#### Ubuntu / Debian

```
$ sudo apt install qbs
```

#### macOS
Qbs is available in [Brew](https://formulae.brew.sh/formula/qbs)
or [macports](https://ports.macports.org/port/qbs/).
```
$ brew install qbs
```
or
```
$ sudo port install qbs
```

#### Windows
Qbs is available in [Chocolatey](https://community.chocolatey.org/packages/qbs#versionhistory)
```
$ choco install qbs
```

### Configuring Qbs
```
$ qbs setup-toolchains --detect
$ qbs setup-qt $(which qmake) qt
$ qbs config defaultProfile qt
```

### Buiding with Qbs
From the source directory, run:
```
$ qbs resolve
$ qbs build
```

### Using IDE
Or you can simply open the `project.qbs` file in QtCreator or the source directory in VScode.
For VScode you might want to use the
[Qbs plugin](https://marketplace.visualstudio.com/items?itemName=qbs-community.qbs-tools).
