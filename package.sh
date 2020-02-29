#!/usr/bin/env bash
# Determine paths
linuxDir=$(pwd)/cmake-build-debug
windowsDir=$(pwd)/cmake-build-debug-windows
addonSrcDir=$(pwd)/X4ConverterBlenderAddon
addonTmpDir=$(pwd)/X4ConverterBlenderAddonBuild
addonReleaseDir=$(pwd)/X4ConverterBlenderAddonRelease
# Build Linux
echo "Starting Linux build" &&
mkdir -p $linuxDir && cd $linuxDir &&
cmake .. -DCMAKE_BUILD_TYPE=Debug &&
cmake --build . -- -j6 &&
# Build Windows
echo "Starting Windows build" &&
mkdir -p $windowsDir && cd $windowsDir &&
cmake ..  -DCMAKE_TOOLCHAIN_FILE=../polly-0.10.4/linux-mingw-w64-gnuxx11.cmake -DCMAKE_BUILD_TYPE=Debug &&
cmake --build . -- -j6 &&
echo "Packaging Addon" &&
# Package the Blender Addon
cd $addonTmpDir &&
# Figure out what filename is safe
name=X4ConverterBlenderAddon-Release
if [[ -e "$name"1.zip || -L "$name"1.zip ]] ; then
    i=2
    while [[ -e $name-$i.zip || -L $name-$i.zip ]] ; do
        (( i++ ))
    done
    name=$name-$i
fi
echo "Release name: "$name &&
# Make a temporary directory
tmpDir=$addonTmpDir/$name
mkdir -p "$tmpDir" &&
# Copy in the addon itself
cp "$addonSrcDir"/* "$tmpDir" &&
# Copy in the executables
cp "$linuxDir"/X4ConverterApp/X4ConverterApp "$tmpDir" &&
cp "$windowsDir"/X4ConverterApp/X4ConverterApp.exe "$tmpDir" &&
# Finally, zip the whole thing up & copy it to the release Directory
zip -r $name.zip "$tmpDir" &&
mv "$tmpDir"/"$name".zip "$addonReleaseDir" &&
echo "Release can be found at: $addonReleaseDir/$name.zip"