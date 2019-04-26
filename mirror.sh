#!/bin/bash
# Simple script to mirror the Spectrum3D project.

mkdir www pkg

# Download Website Recursively
cd ./www
#wget --recursive --page-requisites --html-extension --convert-links --domains spectrum3d.sourceforge.net http://spectrum3d.sourceforge.net

# Download Latest Package and Archive
cd ..
wget -O "Spectrum3D_latest.tar.gz" https://sourceforge.net/projects/spectrum3d/files/spectrum3d-2.0.1.tar.gz/download
rm -rf "./pkg/*"; cd ./pkg
tar --strip-components=1 -xvf "../Spectrum3D_latest.tar.gz"

cd ..
rm "Spectrum3D_latest.tar.gz"
