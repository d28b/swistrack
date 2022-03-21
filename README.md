# SwisTrack 4.2

## Documentation

A brief documentation of SwisTrack is available on the [wiki page](https://github.com/d28b/swistrack/wiki).

## Compiling SwisTrack on Ubuntu/Debian/Raspian

To compile and run SwisTrack, open a terminal, and type the following commands:

```sh
# Download the SwisTrack source code
git clone https://github.com/d28b/swistrack.git

# Install the compiler and libraries
sudo apt install build-essential
sudo apt install libglib2.0-dev libwxgtk3.0-gtk3-dev libopencv-dev

# Compile SwisTrack
cd swistrack
make

# Run SwisTrack
cd SwisTrackEnvironment
./SwisTrack
```

Note that compiling may take a few minutes.

## Compiling SwisTrack on other Linux systems

Install the following dependencies using your package manager:

- GLib 2.0
- wxWidgets (e.g. wxGTK)
- OpenCV 4

Then proceed using `make`.

## Compiling SwisTrack on Mac OS

Install the following dependencies, e.g. using [brew](https://brew.sh):

- GLib 2.0
- wxWidgets (e.g. wxGTK)
- OpenCV 4

Then proceed using:

```sh
make osxbundle
```

## Compiling SwisTrack on Windows

To build SwisTrack for Windows, proceed as follows:

- Install Visual Studio 2022. The free version is enough.
- Download and install OpenCV to C:\OpenCV (or any other folder).
- Download and install wxWidgets to C:\wxWidgets-3.1.5 (or any other folder).
- Add the path of OpenCV and wxWidgets to the environment variables:

```sh
setx -m OPENCV_DIR C:\OpenCV\Build\x64\vc15
setx -m WXWIN C:\wxWidgets-3.1.5
```

- Open the wxWidgets Visual Studio project (C:\wxWidgets-3.1.5\build\msw\wx_vc16.sln) and compile the debug and release versions for x86 (and Win32 if desired). This takes several minutes.
- Open the SwisTrack Visual Studio project (swistrack\Code\MSVisualStudio\SwisTrack.sln) and compile the x86 debug or release version. This takes several minutes.
- Copy the OpenCV DLLs opencv_world455.dll (release) or opencv_world455d.dll (debug) into the SwisTrackEnvironment folder.
