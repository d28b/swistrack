# SwisTrack 4

## Documentation

The documentation of SwisTrack (for both SwisTrack users and developers) can be found in the [SwisTrack wikibook](http://en.wikibooks.org/wiki/Swistrack).

## Running SwisTrack on Ubuntu/Debian/Raspian

To compile and run SwisTrack, open a terminal, and type the following

```sh
git clone https://github.com/d28b/swistrack.git
sudo apt install build-essential
sudo apt install libglib2.0-dev libwxgtk3.0-gtk3-dev libopencv-dev
cd swistrack
make
cd SwisTrackEnvironment
./SwisTrack
```

This downloads the latest version of SwisTrack (`git clone ...`), installs the compiler and the dependencies (`apt install ...`), compiles (`make`) and runs SwisTrack. Note that compiling may take a few minutes.

## Running SwisTrack on other Linux systems

Install the following dependencies using your package manager:

- GLib 2.0
- wxWidgets (e.g. wxGTK)
- OpenCV 4

Then proceed using `make`.

## Running SwisTrack on Mac

Install the following dependencies, e.g. using [brew](https://brew.sh):

- GLib 2.0
- wxWidgets (e.g. wxGTK)
- OpenCV 4

Then proceed using:

```sh
make osxbundle
```

## Running SwisTrack on Windows

Compile the Visual Studio project.
