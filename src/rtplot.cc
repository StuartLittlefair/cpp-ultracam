/*

!!begin
!!title   rtplot
!!author  T.R. Marsh
!!created 09 May 2002
!!revised 06 July 2007
!!root    rtplot
!!index   rtplot
!!descr   rtplots plots server files as they come in
!!class   Display
!!class   Observing
!!css     style.css
!!head1   rtplots plots files from server.

!!emph{rtplot} plots ultracam data files as they come in. This is useful
during focus runs, positioning objects etc. It is probably the most useful program
for observing with ULTRACAM. !!emph{rtplot} has many arguments (48 in all!) 
but many will not be prompted for by default and depending upon what options are selected.

Among the options are the possibilities of plotting a file of defects, useful for ensuring 
that a target does not fall on any of them, and also a file of test windows for setting
up the readout format on a target. Fitting options, especially the Moffat profile are useful
for focussing. See below for more details of this.

An irritating feature is that if you switch between prompted and unprompted input,
you may have to alter the order of the command line arguments unless you explicitly name them.
This is a feature I'm afraid.

!!head2 Program call

rtplot [device source] ((url)/(file) first trim [(ncol nrow) twait tmax])/(flist) [pause] nccd [def] (defect transform) [setup]
setwin bias (biasframe) (threshold (photon) naccum) (lowlevel highlevel) ([stack]) xleft xright yleft yright iset (i1 i2)/(p1 p2) 
(profit) ([method symm (beta) fwhm hwidth readout gain sigrej onedsrch (fwhm1d hwidth1d) (fdevice)])

!!head2 Arguments

!!table

!!arg{device}{Plot device. e.g. "/xs" or "1/xs"}

!!arg{source}{Data source, either 'l' for local, 's' for server or 'u' for a list of ucm files. 
'Local' means the usual .xml and .dat files accessed directly. Do not add either .xml or .dat 
to the file name; these are assumed.}

!!arg{url/file}{If source='S', this should be the complete URL of the
file, e.g.  'http://127.0.0.1:8007/run00000012', or just the file part
in which case the program will try to find a default part to add from
the environment variable ULTRACAM_DEFAULT_URL. Failing this it will
add http://127.0.0.1:8007/, i.e. the local host. If source='L', this
should just be a plain file name.}

!!arg{first}{If source='L' or 'S, 'first' sets which exposure to start with (starts at 1). Set = 0 
to always try to get the last file, useful if the program does not keep up with the data and for 
positioning stars.}

!!arg{trim}{If source='L' or 'S', set trim=true to enable trimming of potential junk rows and
columns of each window}

!!arg{ncol}{If trim, then this specifies the number of columns nearest the readouts of each window to be snipped
off as these can be corrupted.}

!!arg{nrow}{If trim, then this specifies the number of rows to snip off the bottom of each window as these 
can be corrupted.}

!!arg{twait}{Time to wait between attempts to find a new exposure. (Only for data from a server)}

!!arg{tmax}{Maximum time to wait before giving up, set = 0 to give up immediately. (Only for data from
a server).}

!!arg{flist}{If source = 'U', this is the name of a list of ULTRACAM files to reduce. These should be arranged in
temporal order to help the reduction move from one exposure to the next successfully.}

!!arg{pause}{Seconds to pause between frames.}

!!arg{nccd}{CCD number to plot, 0 for all}

!!arg{stack}{Stacking direcion when plotting more than on CCD. Either in 'X' or 'Y'}

!!arg{bias}{true/false according to whether you want to subtract a bias frame. You can specify a full-frame 
bias because it will be cropped to match whatever your format is. This is useful for ultracam because of
the different bias levels of the 6 readouts.}

!!arg{biasframe}{If bias, then you need to specify the name of the bias frame.}

!!arg{threshold}{If you are applying a bias to ULTRASPEC L3CCD data, you have the option of 
converting to photon counts (0 or 1 whether above or below a certain threshold).}

!!arg{photon}{The threshold level if threshold = true}

!!arg{naccum}{Accumulate data into the sum of naccum frames before plotting.}

!!arg{def}{true/false according to whether you want to plot a list of CCD defects (see 
!!ref{setdefect.html}{setdefect}). This is supposed to be useful when trying to acquire targets.}

!!arg{defect}{The name of the defect file if def=true. Bad ones are plotted as stars; moderate ones as circles. Moderate
line defects are dashed cf solid lines for disastrous ones.}

!!arg{transform}{The name of a transformation file generated by the script !!ref{transform.html}{transform}. This allows
the defects of one CCD to be over-plotted on another, accounting for their relative orientations. For each CCD this file
has a line listing the scale factor, rotation (anti-clockwise degrees) and translation in x and y to get from a reference
CCD to the CCD in question. Lines starting with '#' or a blank are skipped.}

!!arg{setup}{True if you want to try setting up windows in which a file of test windows (next argument)
is continually re-read for each image plotted.}

!!arg{setwin}{The file of test windows that will be read each image,
allowing you to keep changing it while new images come in (e.g. with
'emacs', remembering to save it). This should start with the binning
factors as a line 'XBIN YBIN' and then a series of lines of the form
'XSTART YSTART NX NY', which define each window in the following manner: 
XSTART is the X value of left-most column in the window (in terms of unbinned pixels),
YSTART is the Y value of the bottom row, and NX and NY are the X and Y dimensions.  All of these are in
unbinned pixels. This file will be read once per image and the windows
overplotted as dashed boxes. They will be in yellow if judged valid,
red if not, and the reason for their not being valid will be printed
to the terminal. Lines starting with '#' or a blank or a tab are
treated as comments. !!emph{setwin} may either be a disk file name
or an http URL on which a server is waiting which will dispatch the
values. The Java clients !!emph{udriver} and !!emph{usdriver} can act as servers
for !!emph{rtplot}. If these are running on the same machine as rtplot, then
'http://localhost' should work.}

!!arg{lowlevel}{A warning will be printed if the median level of the left-hand window of 
the CCD 1 falls BELOW this value. Sometimes this happens after a power on and adds noise to all the CCDs.
2000 seems to be about the right value. Set = 0 to ignore. (ULTRACAM only)}

!!arg{highlevel}{A warning will be printed if the median value of the right-hand window of CCD 1 
is ABOVE this value. 3500 seems about the right value, but remember sky counts will trigger it. 
Set = 0. to ignore. (ULTRACAM only)}

!!arg{flatlevel}{To reduce false high bias warnings, this is a level above which no warning will be printed.
It is a level WITHOUT bias-subtraction so 6000 is reasonable.
}

!!arg{stack}{Stacking direcion when plotting more than on CCD. Either in 'X' or 'Y'}

!!arg{xleft xright}{X range to plot}

!!arg{yleft yright}{Y range to plot}

!!arg{iset}{'A', 'D' or 'P' according to whether you want to set the intensity limits
automatically (= min to max), directly or with percentiles.}

!!arg{ilow ihigh}{If iset='d', ilow and ihigh specify the intensity range to plot}

!!arg{plow phigh}{If iset='p', plow and phigh are percentiles to set the intensity range,
e.g. 10, 99} 

!!arg{profit}{If a single CCD is plotted, this option allows profile fitting.
If you type 'y', you will be asked to select objects with a cursor for
fitting on each frame. If you select two stars then the angle of the line joining them
with respect to the chip rows will be reported, which is useful during drift mode 
setups.}

!!arg{method}{There are two options for profile fitting: 2D Gaussian or a Moffat profile. 
The latter is a much better representation of seeing broadened profiles. The Gaussian can
be symmetric or elliptical. The Moffat profile is of the form h/(1+(r/R)**2)**beta where
r is the distance from the centre of the profile, R is a scale length and beta is a parameter
that you will be prompted for an intial value. As beta gets large, the Moffat profile tends
to Gaussian. The intial value of R is computed from a FWHM value. The routine plots a
line at 2 times the measured FWHM on the image (not yet for the asymmetric gaussian).}

!!arg{symm}{Yes/no for symmetric versus elliptical gaussians in the
case of gaussian fits. I have not fully tested this option.}

!!arg{fwhm}{fwhm is the initial FWHM to use for the profile fits.}

!!arg{hwidth}{The half-width of the region to be used when fitting a
target. Should be larger than the fwhm, but not so large as to include
multiple targets if possible. This is also the region used to compute the
maximum value that the program will report.}

!!arg{readout}{Readout noise, RMS ADU in order for the program to come
back with an uncertainty. NB It is important in this case to carry out
bias subtraction if you want half-way decent error estimates.}

!!arg{gain}{Gain, electrons/ADU, again for uncertainty estimates}

!!arg{sigrej}{The fits can include rejection of poor pixels. This is
the threshold, measured in sigma. Should not be too small as this may
cause rejection of many points which can slow the routine down a fair
bit as well as leading to calamity in some cases.}

!!arg{onedsrch}{Yes if you want a 1D search to be made before each
profile fit. This is tolerant of poor positioning of the start point,
but potentially vulnerable to problems with multiple targets. What
happens is that a box around the cursor position is collapsed in X and
Y and then the peak in each direction is located using
cross-correlation with a gaussian of FWHM=fwhm1D. This new position is
then used to define the fitting region and initial position for the 2D
gaussian fit. For an isolated target, this should probably be enabled.}

!!arg{fwhm1d}{This is the FWHM used in the 1D search. It does not have
to match the FWHM of the target necessarily, and you may want to err on the
large side to ensure that a taget is picked up..}

!!arg{hwidth1d}{The half-width of the region to be used for searching
for a target by collapsing to 1D. The wider this is, the more chance of finding a target
from a sloppy start position, but also the more chance of peaking up
on a spurious target.}

!!arg{fdevice}{Plot device for showing Moffat & symmetrical gaussian fits.
Should be different from the image plot device. e.g. "/xs" or "2/xs" if 
image plot device = "1/xs" otherwise program goes belly up for reasons that
I can't quite track down.}

!!table

!!head2 Fitting

Two fit options are available: 2D gaussian or a Moffat profile. The Moffat profile can
be thought of as a genaralised gaussian since it tends towards it in extreme cases. It
is therefore usually a better one to use. The only caveat is that at the moment the
implementation of the Moffat assumes that it is symmetrical whereas the gaussian can
be elliptical as well as symmetrical. Most of all the Moffat is better usually because
it allows for more extended wings than the gaussian and tghe real profiles seem to have
these. You have the option during fits of plotting a 1D representation of the fit where
the data is plotted versus radius from the fitted position. 

The radii of the green circle plotted is set equal to the fitted FWHM (at least in the
Moffat case)

See also !!ref{plot.html}{plot} for standard plots.

!!end

*/

#include <cstdlib>
#include <climits>
#include <cfloat>
#include <string>
#include <fstream>
#include <map>
#include "cpgplot.h"
#include "trm_subs.h"
#include "trm_format.h"
#include "trm_input.h"
#include "trm_plot.h"
#include "trm_header.h"
#include "trm_mccd.h"
#include "trm_defect.h"
#include "trm_frame.h"
#include "trm_ultracam.h"
#include "trm_signal.h"

// Main program
int main(int argc, char* argv[]){
	
    using Ultracam::File_Open_Error;
    using Ultracam::Input_Error;

    try{

		// Construct Input object
		Subs::Input input(argc, argv, Ultracam::ULTRACAM_ENV, Ultracam::ULTRACAM_DIR);

		// sign-in input variables
		input.sign_in("device",    Subs::Input::LOCAL,  Subs::Input::NOPROMPT);
		input.sign_in("source",    Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("url",       Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("file",      Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("first",     Subs::Input::LOCAL,  Subs::Input::PROMPT);
		input.sign_in("trim",      Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("ncol",      Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("nrow",      Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("twait",     Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("tmax",      Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("flist",     Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("pause",     Subs::Input::LOCAL,  Subs::Input::NOPROMPT);
		input.sign_in("nccd",      Subs::Input::LOCAL,  Subs::Input::PROMPT);
		input.sign_in("def",       Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("defect",    Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("transform", Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("setup",     Subs::Input::LOCAL,  Subs::Input::NOPROMPT);
		input.sign_in("setwin",    Subs::Input::LOCAL,  Subs::Input::PROMPT);
		input.sign_in("bias",      Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("biasframe", Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("threshold", Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("photon",    Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("naccum",    Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("lowlevel",  Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("highlevel", Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("flatlevel", Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("stack",     Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("xleft",     Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("xright",    Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("ylow",      Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("yhigh",     Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("iset",      Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("ilow",      Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("ihigh",     Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("plow",      Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("phigh",     Subs::Input::GLOBAL, Subs::Input::PROMPT);
		input.sign_in("profit",    Subs::Input::LOCAL,  Subs::Input::PROMPT);
		input.sign_in("method",    Subs::Input::LOCAL,  Subs::Input::NOPROMPT);
		input.sign_in("symm",      Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("beta",      Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("fwhm",      Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("hwidth",    Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("readout",   Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("gain",      Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("sigrej",    Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("onedsrch",  Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("fwhm1d",    Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("hwidth1d",  Subs::Input::GLOBAL, Subs::Input::NOPROMPT);
		input.sign_in("fdevice",   Subs::Input::LOCAL,  Subs::Input::NOPROMPT);

		// Get inputs
		std::string device;
		input.get_value("device", device, "1/xs", "plot device");
		char source;
		input.get_value("source", source, 'S', "uUsSlL", "data source: L(ocal), S(erver) or U(cm)?");
		source = toupper(source);
		std::string url;
		if(source == 'S'){
			input.get_value("url", url, "url", "url of file");
		}else if(source == 'L'){
			input.get_value("file", url, "file", "name of local file");
		}
		size_t first, nfile;
		bool trim;
		int ncol, nrow;
		std::vector<std::string> file;
		double twait, tmax;
		Ultracam::Mwindow mwindow;
		Subs::Header header;
		Ultracam::ServerData serverdata;
		Ultracam::Frame data, dvar;

		if(source == 'S' || source == 'L'){

			input.get_value("first", first, size_t(0), size_t(0), size_t(INT_MAX), "first file to access (0 for last)");
			input.get_value("trim", trim, true, "trim junk lower rows from windows?");
			if(trim){
				input.get_value("ncol", ncol, 0, 0, 100, "number of columns to trim from each window");
				input.get_value("nrow", nrow, 0, 0, 100, "number of rows to trim from each window");
			}
			input.get_value("twait", twait, 1., 0., 1000., "time to wait between attempts to find a frame (seconds)");
			input.get_value("tmax", tmax, 2., 0., 1000., "maximum time to wait before giving up trying to find a "
							"frame (seconds)");

			// Add extra stuff to URL if need be.
			if(url.find("http://") == std::string::npos && source == 'S'){
				char *DEFAULT_URL = getenv(Ultracam::ULTRACAM_DEFAULT_URL);
				if(DEFAULT_URL != NULL){
					url = DEFAULT_URL + url;
				}else{
					url = Ultracam::ULTRACAM_LOCAL_URL + url;
				}
			}else if(url.find("http://") == 0 && source == 'L'){
				throw Ultracam::Input_Error("Should not specify the local file as a URL");
			}
      
			// Parse the XML file
			Ultracam::parseXML(source, url, mwindow, header, serverdata, trim, ncol, nrow, twait, tmax);

			// Initialise standard data frame
			data.format(mwindow, header);

			nfile = first;

		}else{

			std::string flist;
			input.get_value("flist", flist, "files.lis", "name of local file list");

			// Read file list
			std::string name;
			std::ifstream istr(flist.c_str());
			while(istr >> name){
				file.push_back(name);
			}
			istr.close();
			if(file.size() == 0) 
				throw Input_Error("No file names loaded");

			// Read first file
			data.read(file[0]);

			nfile = first = 1;

		}

		Subs::Header::Hnode *hnode = data.find("Instrument.instrument");
		bool ultraspec = (hnode->has_data() && hnode->value->get_string() == "ULTRASPEC"); 
    
		// Carry on getting inputs
		double pause;
		input.get_value("pause", pause, 0., 0., 1000., "time to pause after plotting an image (seconds)");
		int nccd;
		if(data.size() > 1)
			input.get_value("nccd", nccd, int(0), int(0), int(data.size()),  "CCD number to plot (0 for all)");
		else
			nccd = 1;
		bool allccds = (nccd == 0);
		if(!allccds) nccd--;
    
		bool def;
		input.get_value("def", def, false, "do you want to overplot CCD defects?");
    
		Ultracam::Mdefect defect;
		std::vector<Ultracam::Transform> transform;
		if(def){
			std::string sdef;
			input.get_value("defect", sdef, "defect", "name of defect file");
			defect.rasc(sdef);
			if(defect.size() != mwindow.size())
				throw Ultracam::Input_Error("Conflicting numbers of CCDs between defect and data file");
	
			std::string strans;
			input.get_value("transform", strans, "transform", "name of CCD orientation file");
			std::ifstream ifstr(strans.c_str());
			char c;
			double scale, angle, dx, dy;
			size_t ncount = 0;
			while(ifstr){
				c = ifstr.peek();
				if(c == '#' || c == ' '){
		
					ifstr.ignore(100000, '\n');
		
				}else{
		
					ifstr >> scale >> angle >> dx >> dy;
					if(ifstr){
						ifstr.ignore(100000, '\n');
						transform.push_back(Ultracam::Transform(scale, angle, dx, dy));
						ncount++;
					}
				}
			}
			ifstr.close();
			if(ncount != data.size())
				throw Ultracam::Input_Error("Transformation file has incorrect number of CCDs = " + Subs::str(ncount) +
											" cf the expected number = " + Subs::str(data.size()));
	
			// Transform all defects onto the coordinates of the CCD to be displayed
			if(!allccds){
				for(size_t nc=0; nc<defect.size(); nc++){
					for(size_t io=0; io<defect[nc].size(); io++){
						defect[nc][io].transform(transform[nc],false);
						defect[nc][io].transform(transform[nccd],true);
					}
				}
			}
		}
    
		bool setup;
		input.get_value("setup", setup, false, "do you want to plot setup windows?");
		std::string setwin;
		if(setup)
			input.get_value("setwin", setwin, "setwin", "name of setup windows file");
    
		bool bias, thresh = false;
		float photon;
		input.get_value("bias", bias, true, "do you want to subtract a bias frame before plotting?");
		Ultracam::Frame bias_frame;
		if(bias){
			std::string sbias;
			input.get_value("biasframe", sbias, "bias", "name of bias frame");
			bias_frame.read(sbias);
			bias_frame.crop(mwindow);
	
			if(ultraspec){ 
				input.get_value("threshold", thresh, true, "do you want to threshold to get 0 or 1 photons/pix?");
				if(thresh)
					input.get_value("photon", photon, 50.f, FLT_MIN, FLT_MAX, "threshold level to count as 1 photon");
			}
		}
    
		int naccum = 1;
		if(ultraspec) 
			input.get_value("naccum", naccum, 1, 1, 10000, "number of frames to accumulate before displaying");
    
		float lowlevel = 0.f, highlevel = 0.f, flatlevel = 10000.f;
		if(!ultraspec){
			input.get_value("lowlevel", lowlevel,   2000.f, 0.f, FLT_MAX, "low red bias warning level");
			input.get_value("highlevel", highlevel, 3500.f, 0.f, FLT_MAX, "high red bias warning level");
			input.get_value("flatlevel", flatlevel, 6000.f, 0.f, FLT_MAX, "flat level cancel high bias warning level");
		}
    
		char stackdirn;
		if(allccds){
			input.get_value("stack", stackdirn, 'X', "xXyY", "stacking direction for image display (X or Y)");
			stackdirn = toupper(stackdirn);
		}
    
		float x1, x2, y1, y2;
		if(allccds){
			x2 = data.nxtot()+0.5;
			y2 = data.nytot()+0.5;
		}else{
			x2 = data[nccd].nxtot()+0.5;
			y2 = data[nccd].nytot()+0.5;
		}
		input.get_value("xleft",  x1, 0.5f, 0.5f, x2, "left X limit of plot");
		input.get_value("xright", x2, x2,   0.5f, x2, "right X limit of plot");
		input.get_value("ylow",   y1, 0.5f, 0.5f, y2, "lower Y limit of plot");
		input.get_value("yhigh",  y2, y2, 0.5f, y2, "upper Y limit of plot");
    
		char iset;
		input.get_value("iset", iset, 'a', "aAdDpP", 
						"set intensity a(utomatically), d(irectly) or with p(ercentiles)?");
		iset = toupper(iset);
		float ilow, ihigh, plow, phigh;
		if(iset == 'D'){
			input.get_value("ilow",   ilow,  0.f, -FLT_MAX, FLT_MAX, "lower intensity limit");
			input.get_value("ihigh",  ihigh, 1000.f, -FLT_MAX, FLT_MAX, "upper intensity limit");
		}else if(iset == 'P'){
			input.get_value("plow",   plow,  1.f, 0.f, 100.f,  "lower intensity limit percentile");
			input.get_value("phigh",  phigh, 99.f, 0.f, 100.f, "upper intensity limit percentile");
			plow  /= 100.;
			phigh /= 100.;
		}
    
		// Profile fit stuff
		bool profit = false;
		if(!allccds)
			input.get_value("profit",  profit, false, "do you want profile fits?");
    
		float fwhm, readout, gain, beta, sigrej, fwhm1d;
		int   hwidth, hwidth1d;
		bool  symm, initial_search;
		std::string fdevice;
		char method='M';
		if(profit){
			input.get_value("method",  method,  'm', "mMgG", "G(aussian) or M(offat) profile?");
			method = toupper(method);
			input.get_value("symm",    symm, true, "force symmetric profiles?");
			if(method == 'M')
				input.get_value("beta",   beta, 4.f, 0.5f, 1000.f, "default beta exponent for Moffat fits");
	
			input.get_value("fwhm",    fwhm,  10.f, 2.f, 1000.f, "initial FWHM for profile fits");
			input.get_value("hwidth",  hwidth, 31, int(fwhm)+1, INT_MAX, 
							"half-width of fit region for profile fits (unbinned pixels)");
			input.get_value("readout", readout,  4.f, 0.f, FLT_MAX, "readout noise for fits (RMS ADU)");
			input.get_value("gain",    gain,  1.f, 0.01f, 100.f, "electrons/ADU for fits");
	
			input.get_value("sigrej",  sigrej, 5.f, 0.f, FLT_MAX, "threshold for masking pixels (in sigma)");
			input.get_value("onedsrch",  initial_search, true, "carry out 1D position tweak before profile fits?");
			if(initial_search){
				input.get_value("fwhm1d",   fwhm1d,  10.f, 2.f, 1000.f, "FWHM for 1D search");
				input.get_value("hwidth1d", hwidth1d, hwidth, int(2.*fwhm1d+1), INT_MAX, "half-width of 1D search region");
			}
			input.get_value("fdevice", fdevice, "", "plot device for profile fits"); 	
		}
    
		// Save defaults now because one often wants to terminate this program early
		input.save();

		signal(SIGINT, signalproc);
    
		std::vector<Ultracam::sky_mask> skymask;

		// Open plot(s)
		Subs::Plot plot(device), fplot;
		if(fdevice != "") fplot.open(fdevice);

		bool firstplot = true;

		Subs::Format aform(5), plform(6), eform(7), posform(8);

		// vector for storage of profile fitting defaults
		std::vector<Ultracam::Ppars> pdefs;
		bool reliable, found_ut, found_exp, found_gps, found_nsat;
		Subs::Time ut_date, gps_time, ttime(16,Subs::Date::May,2002);
		int nsatellite = 0, format = 0;
		float exposure = 0;

		std::cout << std::endl;

		// Display buffer if naccum > 1 
		Ultracam::Frame dbuffer;
		int nstack = 0;
		bool stopped = false;
		for(;;){

			if(global_ctrlc_set){
				std::cout << "ctrl-C trapped inside rtplot" << std::endl;
				std::cout << "Finishing plotting of server data." << std::endl;
				break;
			}

			// In rtplot we read data and carry on regardless of whether it is any good
			// unless we are trying to make profile fits for the blue CCD in which case
			// only good data are allowed through.
			bool blue_is_bad = true;
			do{

				if(source == 'S' || source == 'L'){

					// Ensure we always request last frame, if that is what is wanted
					nfile = first == 0 ? 0 : nfile;

					if(!Ultracam::get_server_frame(source, url, data, serverdata, nfile, twait, tmax)){
						stopped = true;
						break;
					}
		
				}else{
		
					if(nfile > file.size()) {
						stopped = true;
						break;
					}
					data.read(file[nfile-1]);
		
				}

				hnode = data.find("Frame.bad_blue");
				blue_is_bad = hnode->has_data() ? hnode->value->get_bool() : false;
				if(profit && blue_is_bad && !allccds && nccd == 2){
					std::cerr << "Skipped junk blue CCD data in exposure " << nfile << std::endl;
		
					if(first == 0)
						Subs::sleep(pause);
					else
						nfile++;
				}

			}while(profit && !allccds && nccd == 2 && blue_is_bad);

			// really stop
			if(stopped) break;

			// Tests for low bias problem
			if(!ultraspec && lowlevel > 0.f){
				float avered;
				data[0][0].centile(0.5f, avered);
				if(avered < lowlevel){
					std::cerr << "\n\n\aWARNING: Median of lower-left window of red CCD = " << avered << " which is less than" << std::endl;
					std::cerr << "the trigger level of " << lowlevel << " indicative of the low bias problem that sometimes occurs" << std::endl;
					std::cerr << "after a power-on. A stop & restart will probably fix it.\n" << std::endl;
				}
			}

			// Tests for high bias problem
			if(!ultraspec && highlevel > 0.f && data[0].size() > 1){
				float avered;
				data[0][1].centile(0.5f, avered);
				if(avered > highlevel && avered < flatlevel){
					std::cerr << "\n\n\aWARNING: Median of lower-right window of red CCD = " << avered << " which is more than" << std::endl;
					std::cerr << "the trigger level of " << highlevel << " indicative of the high bias problem that sometimes occurs" << std::endl;
					std::cerr << "after a power-on, but could of course also be due to sky counts. A stop & restart will probably fix it.\n" << std::endl;
				}
			}

			// Subtract a bias frame
			if(bias) data -= bias_frame;

			// Apply threshold
			if(thresh) data.step(photon);

			// Get header items, if they exist
			hnode = data.find("UT_date");
			if(hnode->has_data()){
				found_ut = true;
				ut_date = hnode->value->get_time();
				hnode = data.find("Frame.reliable");
				reliable = (hnode->has_data() && hnode->value->get_bool());
			}else{
				found_ut = false;
				reliable = false;
			}

			hnode = data.find("Frame.satellites");
			if(hnode->has_data()){
				found_nsat = true;
				nsatellite = hnode->value->get_int();
			}else{
				found_nsat = false;
			}

			hnode = data.find("Frame.GPS_time");
			if(hnode->has_data()){
				found_gps = true;
				gps_time = hnode->value->get_time();
			}else{
				found_gps = false;
			}

			hnode = data.find("Exposure");
			if(hnode->has_data()){
				found_exp = true;
				exposure = hnode->value->get_float();
			}else{
				found_exp = false;
			}

			std::cout << "Exp " << nfile << ", UTC= ";
			if(found_ut){
				std::cout << ut_date; 
				if(ut_date < ttime){
					std::cout << " (junk)";
				}else if(reliable){
					std::cout << " (ok)";
				}else{
					std::cout << " (nok)";
				}
			}else{
				std::cout << "UNKNOWN";
			}
			std::cout << ", dwell= ";
			if(found_exp){
				std::cout << eform(exposure) << " s";
			}else{
				std::cout << "UNKNOWN";
			}
			std::cout << ", GPS= ";
			if(found_gps){
				std::cout << gps_time;
			}else{
				std::cout << "UNKNOWN";
			}

			hnode = data.find("Frame.format");
			if(hnode->has_data())
			    format = hnode->value->get_int();

			if(format == 1){
			    std::cout << ", sats = ";
			    if(found_nsat){
				std::cout << nsatellite;
			    }else{
				std::cout << "UNKNOWN";
			    }
			}
			    
			nstack++;

			if(nstack < naccum){
				if(nstack == 1)
					dbuffer = data;
				else
					dbuffer += data;
				std::cout << " Frame " << nstack << " of " << naccum << " added into data buffer." << std::endl;

			}else{

				// Retrieve from the display buffer if necessary
				// reset the display buffer for the next one.
				if(naccum > 1) {
					data   += dbuffer;
					std::cout << " Frame " << nstack << " of " << naccum << " added into data buffer." << std::endl;
					nstack  = 0;
					std::cout << std::endl;
					std::cout << "Plotting sum of last " << naccum << " frames" << std::endl;
				}

				plot.focus();
				if(allccds){
					std::cout << std::endl;
					Ultracam::plot_images(data, x1, x2, y1, y2, allccds, stackdirn, iset, ilow, ihigh, plow, phigh, true, "server file", nccd, true);	   	  
				}else{
					Ultracam::plot_images(data, x1, x2, y1, y2, allccds, stackdirn, iset, ilow, ihigh, plow, phigh, true, "server file", nccd, false);	   	  
					hnode = data.find("Frame.bad_blue");
					bool blue_is_bad = hnode->has_data() ? hnode->value->get_bool() : false;
					if(blue_is_bad && nccd == 2){
						std::cout << ", junk blue data; skipped." << std::endl;
					}else{
						std::cout << ", plot range = " << plform(ilow) << " to " << plform(ihigh) << std::endl;
						if(thresh) std::cout << "Mean photons/pixel = " << data[nccd].mean() << std::endl;
					}
				}

				// Plot any CCD defects
				if(def) Ultracam::plot_defects(defect, x1, x2, y1, y2, allccds, stackdirn, nccd);	   	  
	  
				// Setup windows section
				if(setup) Ultracam::plot_setupwins(setwin, data.size(), x1, x2, y1, y2, allccds, stackdirn, nccd, ultraspec);
	  
				if(profit){
	      
					dvar = data;
					dvar.max(0);
					dvar /= gain;
					dvar += readout*readout;
	      
					if(firstplot){
	
						// Cursor selection of targets after the first plot
						char reply = 'X';
						float x, y;
						std::cout << "You must now select targets whose fwhm etc you want to measure. You can select as many as you like." << std::endl;
						while(reply != 'Q'){
							std::cout << "Move the cursor to the target whose fwhm you want to measure. Hit any key to grab, Q to quit" << std::endl;
							if(!cpgcurs(&x, &y, &reply)) throw Ultracam::Ultracam_Error("Cursor error in rtplot");
							reply = toupper(reply);
							if(reply != 'Q'){
			  
								// here we check that the position is a good one
								try{
									float sky, peak;
									double xm=x, ym=y;
									Ultracam::profit_init(data[nccd], dvar[nccd], xm, ym, initial_search, fwhm1d, hwidth1d, hwidth, sky, peak, true);
			      
									// Initial estimate of 'a' from FWHM
									double a = 1./2./Subs::sqr(fwhm/Constants::EFAC);
			      
									// Store defaults
									if(method == 'G'){
										pdefs.push_back(Ultracam::Ppars(sky, xm, ym, peak, a, 0., a, symm));
									}else{
										pdefs.push_back(Ultracam::Ppars(sky, xm, ym, peak, a, 0., a, beta, symm));
									}
									std::cout << "Added point at " << posform(xm) << ", " << posform(ym) << std::endl;
								}
								catch(const Ultracam::Ultracam_Error& err){
									std::cerr << err << std::endl;
									std::cerr << "Point not added." << std::endl;
								}
							}
						}
					}
	      
					// Profile fitting 
	      
					Ultracam::Ppars psave;
					Ultracam::Iprofile iprofile;
					for(size_t ip=0; ip<pdefs.size(); ip++){
		  
						std::cout << "\nTarget number " << ip + 1 << std::endl;
		  
						psave = pdefs[ip];
		  
						try{
		      
							if(method == 'G'){	    
								std::cout << "\nFitting 2D gaussian ...\n" << std::endl;
							}else if(method == 'M'){	    
								std::cout << "\nFitting moffat profile ...\n" << std::endl;
							}
		      
							Ultracam::fit_plot_profile(data[nccd], dvar[nccd], pdefs[ip], initial_search && !firstplot, false, 0., 0., skymask,
													   fwhm1d, hwidth1d, hwidth, fplot, sigrej, iprofile, true);
		      
							// return focus to image plot
							plot.focus();
		      
							cpgsfs(2);
							if(symm){
			  
								// Overplot circle of radius FWHM
								cpgsci(Subs::GREEN);
								cpgcirc(pdefs[ip].x, pdefs[ip].y, iprofile.fwhm);
								cpgpt1(pdefs[ip].x, pdefs[ip].y, 1);
								cpgsci(Subs::WHITE);
			  
							}else{
			  
								// Overplot ellipse
								cpgsci(Subs::GREEN);
								float cosa = cos(Constants::TWOPI*iprofile.angle/360.);
								float sina = sin(Constants::TWOPI*iprofile.angle/360.);
								float xi, yi, x, y;
								xi = iprofile.fwhm_max;
								yi = 0.;
								x  = pdefs[ip].x + cosa*xi - sina*yi;
								y  = pdefs[ip].y + sina*xi + cosa*yi;
								cpgmove(x,y);
								const int NPLOT=200;
								for(int np=0; np<NPLOT; np++){
									xi = iprofile.fwhm_max*cos(Constants::TWOPI*(np+1)/NPLOT);
									yi = iprofile.fwhm_min*sin(Constants::TWOPI*(np+1)/NPLOT);
									x  = pdefs[ip].x + cosa*xi - sina*yi;
									y  = pdefs[ip].y + sina*xi + cosa*yi;
									cpgdraw(x, y);
								}
								cpgpt1(pdefs[ip].x, pdefs[ip].y, 1);
								cpgsci(Subs::WHITE);
			  
							}
		      
						}
						catch(const std::string& err){
							std::cerr << "Target number " << ip + 1 << ", initial position " << psave.x << " " << psave.y << std::endl;
							std::cerr << err << std::endl;
							std::cerr << "Old defaults will be preserved." << std::endl;
							pdefs[ip] = psave;
						}
						plot.focus();
					}	
				}
	  
				// Report angle from horizontal in the case of two stars as this can be useful for setting up drift mode
				if(pdefs.size() == 2){
					double angle_to_horizontal;
					if(pdefs[0].x < pdefs[1].x){
						angle_to_horizontal = 360.*atan2(pdefs[1].y-pdefs[0].y, pdefs[1].x-pdefs[0].x)/Constants::TWOPI;
					}else{
						angle_to_horizontal = 360.*atan2(pdefs[0].y-pdefs[1].y, pdefs[0].x-pdefs[1].x)/Constants::TWOPI;
					}
					std::cout << "Line from left-hand to right-hand star is rotated by " << aform(angle_to_horizontal) << " degrees anti-clockwise wrt chip rows\n\n" << std::endl;
				}
	  
				if(pause > 0.f){
					// Plot display delay
					std::cerr << "Waiting " << pause << " secs before next plotting next exposure.\n" << std::endl;
					Subs::sleep(pause);
				}
				firstplot = false;
			}
			nfile++;
		}
    }
  
    // Handle errors  
    catch(const Input_Error& err){
		std::cerr << "\nUltracam::Input_Error:" << std::endl;
		std::cerr << err << std::endl;
    }
    catch(const File_Open_Error& err){
		std::cerr << "\nUltracam::File_Open_error:" << std::endl;
		std::cerr << err << std::endl;
    }
    catch(const Ultracam::Ultracam_Error& err){
		std::cerr << "\nUltracam::Ultracam_Error:" << std::endl;
		std::cerr << err << std::endl;
    }
    catch(const Subs::Plot::Plot_Error& err){
		std::cerr << "\nSubs::Plot::Plot_Error:" << std::endl;
		std::cerr << err << std::endl;
    }
    catch(const Subs::Subs_Error& err){
		std::cerr << "\nSubs::Subs_Error:" << std::endl;
		std::cerr << err << std::endl;
    }
    catch(const std::string& err){
		std::cerr << "\n" << err << std::endl;
    }
}

