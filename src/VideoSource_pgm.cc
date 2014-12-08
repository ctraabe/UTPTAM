#include "VideoSource_pgm.h"
#include <cvd/colourspace_convert.h>
#include <cvd/image_io.h>
#include <gvars3/instances.h>

#include <cvd/exceptions.h>

namespace PTAMM {

using namespace CVD;
using namespace std;
using namespace GVars3;

    #define VIDEO_W 640
    #define VIDEO_H 480

VideoSource_pgm::VideoSource_pgm(const std::string &sName)
{
    mFormatString = sName;

    mnFrameRate = GV3::get<int>("VideoSource1.Framerate", 60);
    
    mirSize = ImageRef(VIDEO_W, VIDEO_H);

    mimRGB.resize(mirSize);
    
}

bool VideoSource_pgm::GetAndFillFrameBWandRGB(Image<byte> &imBW, Image<Rgb<byte> > &imRGB)
{

    static int cnt=0;
    
    imRGB.resize(mirSize);
    imBW.resize(mirSize);
    
    char tname[256];

    sprintf(tname,mFormatString.c_str(),cnt);

    ++cnt;
    
    try {
	std::cout << "loading " << tname << std::endl;
	img_load(mimBW, tname);
    } catch (Exceptions::All e) {
	return false;
    }

    std::cout << "conv image: " << mimBW.size() << std::endl;
    convert_image(mimBW,mimRGB);
    
    imBW.copy_from(mimBW);
    imRGB.copy_from(mimRGB);

    mRateLimiter.Limit(mnFrameRate);
    
    return true;
}

}
