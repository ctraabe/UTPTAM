UTPTAM
======

University of Tokyo's modified variant of PTAM.

Installation instructions on Ubuntu 14.04
-----------------------------------------

#### Install prerequisites:

<pre><code>
sudo apt-get update
sudo apt-get install build-essential git cmake
</code></pre>

#### Install TooN

In the directory of your choice:
(The directory may be discarded or kept to pull future updates)
<pre><code>
git clone https://github.com/edrosten/TooN.git
cd TooN
./configure
sudo make install
</code></pre>


#### Install libCVD

In the directory of your choice:
(The directory may be discarded or kept to pull future updates)
<pre><code>
sudo apt-get install liblapack-dev freeglut3-dev
git clone https://github.com/edrosten/libcvd.git
cd libcvd
export CXXFLAGS=-D_REENTRANT
./configure --without-ffmpeg
make -j4
sudo make install
make distclean
</code></pre>

#### Install GVars3 (use the directory of your choice):

In the directory of your choice:
(The directory may be discarded or kept to pull future updates)
<pre><code>
git clone https://github.com/edrosten/gvars.git
cd gvars
./configure --disable-widgets
make -j4
sudo make install
make clean
</code></pre>

#### Install ARToolKit (recommend using \tmp):

Prerequisites:
<pre><code>
sudo apt-get install libv4l-dev libxi-dev libxmu-dev
</code></pre>

Create a symbolic link to videodev.h:
*Note:* This might not be best practice but I found it to be the easiest way to
get everything compiling without modifying a lot of code.
<pre><code>
sudo ln -s /usr/include/libv4l1-videodev.h /usr/include/linux/videodev.h
</code></pre>

<pre><code>
wget http://downloads.sourceforge.net/project/artoolkit/artoolkit/2.72.1/ARToolKit-2.72.1.tgz
tar xzf ARToolKit-2.72.1.tgz
cd ARToolKit
./Configure
</code></pre>
Enter the following at the prompts:
- 1 (Video4Linux)
- n (Do not use x86 assembly for color conversion)
- y (Create debug symbols)
- y (Build gsub libraries with texture rectangle support)

<pre><code>
make -j4
sudo cp -r include/AR /usr/local/include/
sudo cp lib/*.a /usr/local/lib/
</code></pre>

#### Install Flycapture (recommend using \tmp)

Download and untar flycapture from pointgrey research
<pre><code>
sudo apt-get install libglademm-2.4-1c2a libgtkglextmm-x11-1.2-dev
sudo ./install_flycapture.sh
</code></pre>
When prompted, setup flycapture user

#### UTPTAM

In the directory of your choice:
<pre><code>
sudo apt-get install libgstreamer0.10-dev libgstreamer-plugins-base0.10-dev libopencv-dev libboost-signals-dev
git clone https://github.com/ctraabe/UTPTAM.git
cd UTPTAM
mkdir build
cd build
cmake ..
make -j4
</code></pre>

Now configure settings and make links to executables:
<pre><code>
cd ..
cp settings_template.cfg settings.cfg
ln -s build/bin/UTPTAM .
</code></pre>
