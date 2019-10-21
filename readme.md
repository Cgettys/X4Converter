# Caution: This is very much WIP. Please keep backups of your work and save often!
# Build:
Install git if you don't have it.
Start by cloning or downloading this repository.
## Windows:
Note that these instructions are less reliable than the Linux build; I develop on Linux, and while I am happy to provide support & will eventually provide builds for Windows, it is not my native environment.
One way to do this might be Windows Subsystem for Linux (WSL); WSL2 should be installed on up to date Windows 10 PCs by default I believe. However, I have not tried this.

The way I have gotten to work in the past is as follows:
Install cmake & Mingw-64. It might be possible to use Visual Studio's compiler since cmake does support it, but I have not tried it.
Open Git bash or Bash Shell; you could even use powershell if you know a little bit about Linux commands and their Windows counterparts, as the bash script is currently a change directory and 2 cmake commands.
```
build.sh
```
## Linux:
Note, this is targeted at Ubuntu, but if you're running something else, I assume you know enough to adapt it. Might be some other stuff I'm missing, but the below should get your basic dependencies, and the rest are handled through CMake and Hunter.
```bash
sudo apt install build-essential cmake
build.sh
```
# Notes
To run the tests, you must set the environment variable X4_UNPACKED_ROOT to the base directory of an unpacked X4 install (as made by Egosoft's catalog tool).

# Unsupported features


Not supported by converter:

* interpolationtype = 7
* Tens, Cont, Bias
* EaseIn, EaseOut
* Deriv, DerivInX DerivInY DerivInZ, DerivOutX DerivOutY DerivOutZ
* 
