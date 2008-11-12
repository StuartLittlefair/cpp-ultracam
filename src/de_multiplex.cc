//! \file

#include "trm_subs.h"
#include "trm_frame.h"
#include "trm_ultracam.h"

/**

This function is completely ULTRACAM specific. It de-multiplexes the data stored in the buffer 
sent back by the fileserver. This comes back in the order:

\verbatim
CCD=0, window=0, iy=0, ix=0 
CCD=0, window=1, iy=0, ix=nx-1 
CCD=1, window=0, iy=0, ix=0 
CCD=1, window=1, iy=0, ix=nx-1 
CCD=2, window=0, iy=0, ix=0 
CCD=2, window=1, iy=0, ix=nx-1 

CCD=0, window=0, iy=0, ix=1 
CCD=0, window=1, iy=0, ix=nx-2 
CCD=1, window=0, iy=0, ix=1
CCD=1, window=1, iy=0, ix=nx-2 
CCD=2, window=0, iy=0, ix=1
CCD=2, window=1, iy=0, ix=nx-2 
.
.
.

CCD=0, window=0, iy=1, ix=0 
CCD=0, window=0, iy=1, ix=nx-1 
CCD=1, window=0, iy=1, ix=0
CCD=1, window=0, iy=1, ix=nx-1 
CCD=2, window=0, iy=1, ix=0
CCD=2, window=0, iy=1, ix=nx-1 
\endverbatim

etc until all iy are done, and then onto the next window pair until the end. 
This routine puts all of this into the standard arrays inside the Frame passed to it.
\param buffer a buffer of data returned by the server, without a header
\param data   a data frame to store it into. Needs its format to have been defined
by running parseXML

The routine tries to work out what sort of machine we are one and will swap bytes
if it is thought to be big-endian (as opposed to intel / linux little endian)

*/

void Ultracam::de_multiplex_ultracam(char *buffer, Frame& data){

    // Initialise. 
    // PIX_SHIFT accounts for a problem that was present until May 2007 the cure for which 
    // is to remove the outermost pixel of all windows.
    const int  PIX_SHIFT = data["Instrument.version"]->get_int() < 0 ? 1 : 0;
    const bool TRIM      = data["Trimming.applied"]->get_bool();
    const int  NCOL      = TRIM ? data["Trimming.ncols"]->get_int() + PIX_SHIFT: PIX_SHIFT;
    const int  NROW      = TRIM ? data["Trimming.nrows"]->get_int() : 0;
    const bool LITTLE    = Subs::is_little_endian();
    const bool STRIP     = NCOL > 0 || NROW > 0;

    size_t nwin1=0, nwin2=1;

    // Variables accessed most often are declared 'register' in the hope of speeding things
    register int nccd;
    register const int NCCD = data.size();
    register size_t ip = 0;
    register int iy;
    register Subs::UCHAR cbuff[2];

    // Overscan mode is a special case. Separate it because of rarity and difficulty
    bool normal = (data["Instrument.Readout_Mode_Flag"]->get_int() != ServerData::FULLFRAME_OVERSCAN);

    if(normal){

	register int NX = data[0][nwin1].nx();
	register int ix1=0, ix2 = NX-1;
	iy = 0;

	// Advance buffer pointer if trimming enabled. The factor 4 comes from 2 bytes for
	// each pixel and 2 windows. Should not have been done in overscan mode
	if(STRIP){
	    ip += 4*NCCD*(NX+NCOL)*NROW;
	    ip += 4*NCCD*NCOL;
	}

	for(;;){

	    // Add in pixels from the left of the left window and from the right of the right window for each window
	    // pair and CCD in turn. The pointer to char is cast to a pointer to an unsigned 2-byte integer and then the value 
	    // of the thing pointed at is converted to the internal data type of ULTRACAM, and the pointer is advanced 
	    // appropriately. This loop is the critical one for speed. 

	    for(nccd=0; nccd<NCCD; nccd++){	
		if(LITTLE){
		    data[nccd][nwin1][iy][ix1] = internal_data(*(Subs::UINT2*)(buffer+ip));
		    ip += 2;
		    data[nccd][nwin2][iy][ix2] = internal_data(*(Subs::UINT2*)(buffer+ip));
		    ip += 2;
		}else{
		    cbuff[1] = buffer[ip++];
		    cbuff[0] = buffer[ip++];
		    data[nccd][nwin1][iy][ix1] = internal_data(*(Subs::UINT2*)cbuff);
		    cbuff[1] = buffer[ip++];
		    cbuff[0] = buffer[ip++];
		    data[nccd][nwin2][iy][ix2] = internal_data(*(Subs::UINT2*)cbuff);
		}
	    }
      
	    // Update pointers for next round. 
	    ix1++;
	    ix2--;
	    if(ix1 == NX){
		ix1 = 0;
		ix2 = NX-1;
		iy++;
		if(iy == data[0][nwin1].ny()){
		    nwin2 += 2;
	  
		    // Finished
		    if(nwin2 > data[0].size()) break;
	  
		    nwin1 += 2;
		    iy     = 0;
		    NX     = data[0][nwin1].nx();
		    ix2    = NX-1;
	  
		    // skip lower rows 
		    if(STRIP) ip += 4*NCCD*(NX+NCOL)*NROW;
	  
		}
	
		// skip columns on left of left window, right of right window
		if(STRIP) ip += 4*NCCD*NCOL;
	    }
	}

    }else{

	// Overscan mode is a bit of a bugger. 24 column on left of left window and right
	// of right window, plus 4 on right of left window and left of right window
	// plus another 8 rows at the top. Very specific implementation here to split
	// between 6 windows with the two parts of the overscan combined into single strips
	// which appear on the right of the main windows and an extra part at the top. This
	// way the mapping of real pixel to image pixel is preserved so object positions stay
	// the same.
    
	register int ix;

	for(iy=0; iy<1032; iy++){
	    for(ix=0; ix<540; ix++){
		for(nccd=0; nccd<NCCD; nccd++){
		    if(ix < 24){
	    
			// left and right overscan windows
			if(LITTLE){
			    data[nccd][2][iy][ix]   = internal_data(*(Subs::UINT2*)(buffer+ip));	 
			    ip += 2;
			    data[nccd][3][iy][27-ix] = internal_data(*(Subs::UINT2*)(buffer+ip));	  
			    ip += 2;
			}else{
			    cbuff[1] = buffer[ip++];
			    cbuff[0] = buffer[ip++];
			    data[nccd][2][iy][ix]   = internal_data(*(Subs::UINT2*)cbuff);
			    cbuff[1] = buffer[ip++];
			    cbuff[0] = buffer[ip++];
			    data[nccd][3][iy][27-ix] = internal_data(*(Subs::UINT2*)cbuff);
			}

		    }else if(ix < 536){
	    
			if(iy < 1024){
			    // left and right data windows
			    if(LITTLE){
				data[nccd][0][iy][ix-24]  = internal_data(*(Subs::UINT2*)(buffer+ip));	  
				ip += 2;
				data[nccd][1][iy][535-ix] = internal_data(*(Subs::UINT2*)(buffer+ip));	  
				ip += 2;
			    }else{
				cbuff[1] = buffer[ip++];
				cbuff[0] = buffer[ip++];
				data[nccd][0][iy][ix-24]  = internal_data(*(Subs::UINT2*)cbuff);
				cbuff[1] = buffer[ip++];
				cbuff[0] = buffer[ip++];
				data[nccd][1][iy][535-ix] = internal_data(*(Subs::UINT2*)cbuff); 
			    }

			}else{

			    // top left and right overscan windows
			    if(LITTLE){
				data[nccd][4][iy-1024][ix-24]  = internal_data(*(Subs::UINT2*)(buffer+ip));	  
				ip += 2;
				data[nccd][5][iy-1024][535-ix] = internal_data(*(Subs::UINT2*)(buffer+ip));	  
				ip += 2;
			    }else{
				cbuff[1] = buffer[ip++];
				cbuff[0] = buffer[ip++];
				data[nccd][4][iy-1024][ix-24]  = internal_data(*(Subs::UINT2*)cbuff);
				cbuff[1] = buffer[ip++];
				cbuff[0] = buffer[ip++];
				data[nccd][5][iy-1024][535-ix] = internal_data(*(Subs::UINT2*)cbuff);  
			    }
			}
	    
		    }else{

			// left and right overscan windows again
			if(LITTLE){
			    data[nccd][2][iy][ix-512] = internal_data(*(Subs::UINT2*)(buffer+ip));	  
			    ip += 2;
			    data[nccd][3][iy][539-ix] = internal_data(*(Subs::UINT2*)(buffer+ip));	  
			    ip += 2;
			}else{
			    cbuff[1] = buffer[ip++];
			    cbuff[0] = buffer[ip++];
			    data[nccd][2][iy][ix-512] = internal_data(*(Subs::UINT2*)cbuff);
			    cbuff[1] = buffer[ip++];
			    cbuff[0] = buffer[ip++];
			    data[nccd][3][iy][539-ix] = internal_data(*(Subs::UINT2*)cbuff);  
			}
		    }
		}
	    }
	}
    }
}


/**

This function is ULTRASPEC specific. It de-multiplexes the data stored in the buffer 
sent back by the fileserver. This comes back in the order:

\verbatim
window=0, iy=0, ix=0 
window=0, iy=0, ix=1 
window=0, iy=0, ix=2
.
.
.
window=0, iy=1, ix=0
.
.
.
window=1, iy=1, ix=0
.
.
.
\endverbatim

There are two possible read out modes however, and in one of them the read out order is reversed in the X direction, so
that it starts with nx-1. This function swaps this case so that the images will appear the same on the screen.

*/

void Ultracam::de_multiplex_ultraspec(char *buffer, Frame& data){

    // Initialise
    const bool TRIM   = data["Trimming.applied"]->get_bool();
    const int  NCOL   = TRIM ? data["Trimming.ncols"]->get_int() : 0;
    const int  NROW   = TRIM ? data["Trimming.nrows"]->get_int() : 0;
    const bool LITTLE = Subs::is_little_endian();

    // Variables accessed most often are declared 'register' in the hope of speeding things
    register size_t ip = 0;
    register int iy;
    register Subs::UCHAR cbuff[2];

    // Flag the output being used. This is what indicates reversal or not.
    bool normal = (data["Instrument.Output"]->get_int() == 0);

    if(normal){

	register size_t nwin = 0;
	register int NX = data[0][0].nx();
	register int ix=0;
	iy = 0;

	// Advance buffer pointer if trimming enabled. The factor 2 comes from 2 bytes for each pixel.
	if(TRIM){
	    ip += 2*(NX+NCOL)*NROW;
	    ip += 2*NCOL;
	}

	for(;;){

	    // 'normal' mode we assume that the first pixel read out is the left-most 
	
	    if(LITTLE){
		data[0][nwin][iy][ix] = internal_data(*(Subs::UINT2*)(buffer+ip));
		ip += 2;
	    }else{
		cbuff[1] = buffer[ip++];
		cbuff[0] = buffer[ip++];
		data[0][nwin][iy][ix] = internal_data(*(Subs::UINT2*)cbuff);
	    }

	    // Update pointers for next round. 
	    ix++;
	    if(ix == NX){
		ix = 0;
		iy++;
		if(iy == data[0][nwin].ny()){

		    nwin++;

		    // Finished
		    if(nwin >= data[0].size()) break;
	  	  
		    iy     = 0;
		    NX     = data[0][nwin].nx();
	  
		    // skip lower rows 
		    if(TRIM) ip += 2*(NX+NCOL)*NROW;
	  
		}
	
		// skip columns on left of left window
		if(TRIM) ip += 2*NCOL;
	    }
	}
    
    }else{
      
	register size_t nwin = 0;
	register int NX = data[0][nwin].nx();
	register int ix = NX-1;
	iy = 0;

	// Advance buffer pointer if trimming enabled. The factor 2 comes from 2 bytes for each pixel.
	if(TRIM){
	    ip += 2*(NX+NCOL)*NROW;
	    ip += 2*NCOL;
	}
      
	for(;;){
	  
	    // 'abnormal' mode we assume that the first pixel read out is the right-most 
	    if(LITTLE){
		data[0][nwin][iy][ix] = internal_data(*(Subs::UINT2*)(buffer+ip));
		ip += 2;
	    }else{
		cbuff[1] = buffer[ip++];
		cbuff[0] = buffer[ip++];
		data[0][nwin][iy][ix] = internal_data(*(Subs::UINT2*)cbuff);
	    }

	    // Update pointers for next round. 
	    ix--;
	    if(ix < 0){
		iy++;
		if(iy == data[0][nwin].ny()){

		    nwin++;

		    // Finished
		    if(nwin >= data[0].size()) break;
	  	  
		    iy     = 0;
		    NX     = data[0][nwin].nx();
	  
		    // skip lower rows 
		    if(TRIM) ip += 2*(NX+NCOL)*NROW;
	  
		}

		ix = NX-1;
	
		// skip columns on left of left window
		if(TRIM) ip += 2*NCOL;
	    }
	}
    }
}

