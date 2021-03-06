/*

!!begin
!!title   Analyses noise in Ultracam frames
!!author  T.R. Marsh
!!created 14 May 2002
!!descr   Analyses noise in Ultracam frames
!!css     style.css
!!root    ncal
!!index   ncal
!!class   Programs
!!class   Testing
!!head1   ncal - analyses noise in Ultracam frames

This program performs a noise analysis similar to pamela's !!emph{noise}. i.e. it
makes local estimates of the noise using the mean of the absolute value of the
differences of the values of each pixel and the mean of its 8 surrounding neighbours.
The measn is taken over all pixels in boxes over the images. The idea here is not to take overall
RMS values which are sensitive to image structure. To relate the mean of the absolute value to the
variance a correction factor = sqrt(4*Pi/9) is applied. This accounts for the uncertainty in the
mean of the 8 surrounding pixels and is based upon an assumption of gaussian white noise.

!!head2 Invocation

ncal device list nccd nwin rms gain grain!!break

!!head2 Arguments

!!table

!!arg{device}{Plot device}

!!arg{list}{List of files to be analysed}

!!arg{window}{A windows file (e.g. as generated by !!ref{setwin.html}{setwin} defining
the regions to be evaluated. Each CCD is treated separately. Since one expects each half
of the chips to behave differently in ULTRACAM, it is probably wise to avoid crossing the
border.}

!!arg{nccd}{CCD number to analyse}

!!arg{xbox}{Size of box to average over (X dimension, binned pixels)}

!!arg{ybox}{Size of box to average over (Y dimension, binned pixels)}

!!arg{rms}{RMS counts readout to plot as a model}

!!arg{gain}{electrons/count factor}

!!arg{grain}{flat-field noise in percent}

!!table

!!end

*/

#include <cstdlib>
#include <cfloat>
#include <string>
#include <map>
#include <fstream>
#include <errno.h>
#include "cpgplot.h"
#include "trm/constants.h"
#include "trm/subs.h"
#include "trm/input.h"
#include "trm/plot.h"
#include "trm/frame.h"
#include "trm/ultracam.h"

int main(int argc, char* argv[]){

  using Ultracam::Input_Error;

  try{

    // Construct Input object
    Subs::Input input(argc, argv, Ultracam::ULTRACAM_ENV, Ultracam::ULTRACAM_DIR);

    // Sign-in input variables
    input.sign_in("device",    Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
    input.sign_in("list",      Subs::Input::LOCAL,  Subs::Input::PROMPT);
    input.sign_in("window",    Subs::Input::LOCAL,  Subs::Input::PROMPT);
    input.sign_in("nccd",      Subs::Input::GLOBAL, Subs::Input::PROMPT);
    input.sign_in("xbox",      Subs::Input::LOCAL,  Subs::Input::PROMPT);
    input.sign_in("ybox",      Subs::Input::LOCAL,  Subs::Input::PROMPT);
    input.sign_in("rms",       Subs::Input::LOCAL,  Subs::Input::PROMPT);
    input.sign_in("gain",      Subs::Input::LOCAL,  Subs::Input::PROMPT);
    input.sign_in("grain",     Subs::Input::LOCAL,  Subs::Input::PROMPT);

    // Get inputs
    std::string device;
    input.get_value("device", device, "/xs", "plot device");
    std::string list_name;
    input.get_value("list", list_name, "list", "file list for noise analysis");

    // Read file list
    std::string name;
    std::vector<std::string> flist;
    std::ifstream istr(list_name.c_str());
    while(istr >> name){
      flist.push_back(name);
    }
    istr.close();
    size_t nfile = flist.size();
    if(nfile == 0) throw Input_Error("No file names loaded");

    std::string swindow;
    input.get_value("window", swindow, "window", "window over which histogram will be computed");
    Ultracam::Mwindow mwindow(swindow);

    // Get rest of parameters
    int nccd;
    input.get_value("nccd", nccd, int(1), int(1), int(10), "CCD number to analyse");
    nccd--;
    int xbox;
    input.get_value("xbox", xbox, 1, 1, 1024, "X size of boxes to average over");
    int ybox;
    input.get_value("ybox", ybox, 1, 1, 1024, "Y size of boxes to average over");
    float rms;
    input.get_value("rms", rms, 0.f, 0.f, FLT_MAX, "RMS readout noise in ADU");
    float gain;
    input.get_value("gain", gain, 1.f, 1.e-10f, 10000.f, "gain factor, electrons/ADU");
    float grain;
    input.get_value("grain", grain, 0.f, 0.f, 100.f, "flat field 'grain' noise, percent");

    Ultracam::Frame frame;
    float cval;
    double mean, sum1, sum2;
    int num_pix;
    // Factor to account for the way the scatter is estimated using absolute deviations
    // rather than RMS and also to allow for the variance of the 8 surrounding pixels.
    const float SCALE = sqrt(4.*Constants::PI/9.);

    // Set up plot
    Subs::Plot plot(device);
    cpgsci(Subs::BLUE);
    const float x1 = -1.;
    const float x2 = 5.;
    cpgsch(1.2);
    cpgscf(2);
    cpgenv(x1, x2, 0., 3., 0, 30);
    cpgsci(Subs::RED);
    cpglab("Mean value", "RMS fluctuation", "ULTRACAM noise analysis");
    cpgsci(Subs::WHITE);

    for(size_t nf=0; nf<nfile; nf++){

      frame.read(flist[nf]);
      if(frame.size() != mwindow.size())
    throw Ultracam::Input_Error("Data frame = " + flist[nf] + " and window file have differing numbers of CCDs");

      // window frame
      frame.window(mwindow);

      // loop over all windows
      for(size_t iw=0; iw<frame[nccd].size(); iw++){

    const Ultracam::Windata &win = frame[nccd][iw];

    // loop over every pixel of every box
    for(int iyb=0; iyb<(win.ny()-2)/ybox; iyb++){
      for(int ixb=0; ixb<(win.nx()-2)/xbox; ixb++){
        sum1 = 0.;
        sum2 = 0.;
        num_pix = 0;
        for(int iy=1+ybox*iyb; iy<=ybox*(iyb+1); iy++){
          for(int ix=1+xbox*ixb; ix<=xbox*(ixb+1); ix++){
        cval  = win[iy][ix];

        // Form the mean of the 8 surrounding pixels.
        mean  = 0.;
        mean += win[iy-1][ix-1];
        mean += win[iy-1][ix];
        mean += win[iy-1][ix+1];
        mean += win[iy][ix-1];
        mean += win[iy][ix+1];
        mean += win[iy+1][ix-1];
        mean += win[iy+1][ix];
        mean += win[iy+1][ix+1];
        mean /= 8.;
        sum1 += mean;
        sum2 += fabs(cval-mean);
        num_pix++;
          }
        }
        if(num_pix){
          sum1 /= num_pix;
          sum2 /= num_pix;
          sum2 *= SCALE;
          float x = log10(std::max(sum1,0.11));
          float y = log10(std::max(sum2,1.));
          cpgpt1(x,y,1);
        }
      }
    }
      }
    }

    // Plot model
    const int MAXPLOT = 200;
    float xp[MAXPLOT], yp[MAXPLOT];
    float dn;
    for(int i=0; i<MAXPLOT; i++){
      xp[i] = x1 + (x2-x1)*i/float(MAXPLOT-1);
      dn    = pow(10.f, xp[i]);
      yp[i] = 0.5*log10(Subs::sqr(rms) + dn/gain + Subs::sqr(grain*dn/100.));
    }
    cpgsci(Subs::RED);
    cpgline(MAXPLOT, xp, yp);

  }

  catch(const Ultracam::Input_Error& err){
    std::cerr << "Ultracam::Input_Error exception:" << std::endl;
    std::cerr << err << std::endl;
  }
  catch(const Ultracam::Ultracam_Error& err){
    std::cerr << "Ultracam::Ultracam_Error exception:" << std::endl;
    std::cerr << err << std::endl;
  }
  catch(const Subs::Subs_Error& err){
    std::cerr << "Subs::Subs_Error exception:" << std::endl;
    std::cerr << err << std::endl;
  }
  catch(const std::string& err){
    std::cerr << err << std::endl;
  }
}



