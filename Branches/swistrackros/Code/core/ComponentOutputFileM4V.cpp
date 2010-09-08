#include "ComponentOutputFileM4V.h"
#define THISCLASS ComponentOutputFileM4V

#ifdef USE_XVID
#include "DisplayEditor.h"

THISCLASS::ComponentOutputFileM4V(SwisTrackCore *stc):
		Component(stc, wxT("OutputFileM4V")),
		mM4VHandle(0), mM4VBuffer(0), mFrameRate(15), mInputChannel(cInputChannel_None),
		mDisplayOutput(wxT("Output"), wxT("M4V File: Unprocessed Frame")) {

	// Data structure relations
	mCategory = &(mCore->mCategoryOutput);
	AddDisplay(&mDisplayOutput);
	AddDataStructureRead(&(mCore->mDataStructureImageGray));
	AddDataStructureRead(&(mCore->mDataStructureImageColor));
	AddDataStructureRead(&(mCore->mDataStructureImageBinary));

	// Read the XML configuration file
	Initialize();
}

THISCLASS::~ComponentOutputFileM4V() {
}

void THISCLASS::OnStart() {
	wxString filename_string = GetConfigurationString(wxT("FileName"), wxT(""));
	mFileName = mCore->GetRunFileName(filename_string);
	mFrameRate = GetConfigurationInt(wxT("FrameRate"), 15);

	wxString keepinmemory = GetConfigurationString(wxT("WriteMode"), wxT(""));
	if (keepinmemory == wxT("raw")) {
		mKeepInMemory = cKeepInMemory_Raw;
	} else if (keepinmemory == wxT("compressed")) {
		mKeepInMemory = cKeepInMemory_Compressed;
	} else {
		mKeepInMemory = cKeepInMemory_None;
	}

	OnReloadConfiguration();
}

void THISCLASS::OnReloadConfiguration() {
	wxString inputchannel = GetConfigurationString(wxT("InputChannel"), wxT(""));
	if (inputchannel == wxT("color")) {
		mInputChannel = cInputChannel_Color;
	} else if (inputchannel == wxT("grayscale")) {
		mInputChannel = cInputChannel_Grayscale;
	} else if (inputchannel == wxT("binary")) {
		mInputChannel = cInputChannel_Binary;
	} else {
		mInputChannel = cInputChannel_None;
	}
}

void THISCLASS::OnStep() {
	// Get the input image
	IplImage* inputimage = 0;
	if (mInputChannel == cInputChannel_Grayscale) {
		inputimage = mCore->mDataStructureImageGray.mImage;
	} else if (mInputChannel == cInputChannel_Color) {
		inputimage = mCore->mDataStructureImageColor.mImage;
	} else if (mInputChannel == cInputChannel_Binary) {
		inputimage = mCore->mDataStructureImageBinary.mImage;
	} else {
		AddError(wxT("No input channel selected."));
		return;
	}

	// Do nothing if no image is available
	if (! inputimage) {
		AddError(wxT("No image on selected input."));
		return;
	}

	if (mKeepInMemory == cKeepInMemory_None) {
		// Open the output file if necessary
		if (! mM4VHandle) {
			M4VOpen(inputimage);
		}

		// Write the frame
		M4VWriteFrame(inputimage);
	} else if (mKeepInMemory == cKeepInMemory_Raw) {
	} else if (mKeepInMemory == cKeepInMemory_Compressed) {
	}

	// Set the display
	DisplayEditor de(&mDisplayOutput);
	if (de.IsActive()) {
		de.SetMainImage(inputimage);
	}
}

void THISCLASS::OnStepCleanup() {
}

void THISCLASS::OnStop() {
	M4VClose();
}

/* Initialize encoder for first use, pass all needed parameters to the codec */
void THISCLASS::M4VOpen(IplImage* image) {
	xvid_enc_plugin_t plugins[7];
	xvid_gbl_init_t xvid_gbl_init;
	xvid_enc_create_t xvid_enc_create;

	// Open the output file
	mFile.Open(mFileName.GetFullPath(), wxFile::write);
	if (mFile.IsOpened() == false) {
		AddError(wxT("Error while opening the output file."));
		return;
	}

	// Set version
	memset(&xvid_gbl_init, 0, sizeof(xvid_gbl_init));
	xvid_gbl_init.version = XVID_VERSION;

#ifdef ARCH_IS_IA64
	xvid_gbl_init.cpu_flags = XVID_CPU_FORCE | XVID_CPU_ASM;
#else
	xvid_gbl_init.cpu_flags = 0;
#endif

	// Initialize XviD core
	xvid_global(0, XVID_GBL_INIT, &xvid_gbl_init, 0);

	// Version again
	memset(&xvid_enc_create, 0, sizeof(xvid_enc_create));
	xvid_enc_create.version = XVID_VERSION;

	// Width and Height of input frames
	xvid_enc_create.width = image->width - (image->width & 1);
	xvid_enc_create.height = image->height - (image->height & 1);
	xvid_enc_create.profile = XVID_PROFILE_AS_L4;

	// Allocate memory for the compressed frame
	mM4VBuffer = new unsigned char[image->width * image->height * 6];
	if (! mM4VBuffer) {
		AddError(wxT("Failed to allocate memory for the compressed frame."));
		return;
	}

	// Init zones
	//xvid_enc_create.zones = mM4VZones;
	//xvid_enc_create.num_zones = 0;
	xvid_enc_create.zones = 0;
	xvid_enc_create.num_zones = 0;

	// Init plugins
	xvid_enc_create.plugins = plugins;
	xvid_enc_create.num_plugins = 0;

	// No fancy thread tests
	xvid_enc_create.num_threads = 0;

	// Frame rate
	int framerate_increment = 1000;
	xvid_enc_create.fincr = framerate_increment;
	xvid_enc_create.fbase = (int) floor(framerate_increment * mFrameRate + 0.5);

	// Maximum key frame interval
	xvid_enc_create.max_key_interval = (int)floor(mFrameRate + 0.5) * 10;

	// Bframes settings
	xvid_enc_create.max_bframes = 0;
	xvid_enc_create.bquant_ratio = 150;
	xvid_enc_create.bquant_offset = 100;

	// Dropping ratio frame
	xvid_enc_create.frame_drop_ratio = 0;

	// Global encoder options
	xvid_enc_create.global = 0;

	// I use a small value here, since will not encode whole movies, but short clips
	printf("XVID open\n");
	int error = xvid_encore(0, XVID_ENC_CREATE, &xvid_enc_create, 0);
	if (error) {
		AddError(wxT("Error while opening the encoder."));
		return;
	}

	// Retrieve the encoder instance from the structure
	mM4VHandle = xvid_enc_create.handle;
}

void THISCLASS::M4VWriteFrame(IplImage* image) {
	xvid_enc_frame_t xvid_enc_frame;
	xvid_enc_stats_t xvid_enc_stats;

	// If we could not open the encoder, just do nothing
	if (! mM4VHandle) {
		return;
	}

	// Version for the frame and the stats
	memset(&xvid_enc_frame, 0, sizeof(xvid_enc_frame));
	xvid_enc_frame.version = XVID_VERSION;
	memset(&xvid_enc_stats, 0, sizeof(xvid_enc_stats));
	xvid_enc_stats.version = XVID_VERSION;

	// Bind output buffer
	xvid_enc_frame.bitstream = mM4VBuffer;
	xvid_enc_frame.length = -1;

	// Set up core's general features
	xvid_enc_frame.vop_flags = 0;

	// Initialize input image fields
	xvid_enc_frame.input.plane[0] = image->imageData;
	xvid_enc_frame.input.stride[0] = image->widthStep;
	xvid_enc_frame.input.csp = XVID_CSP_BGR;

	if (image->nChannels == 1) {
		xvid_enc_frame.vop_flags |= XVID_VOP_GREYSCALE;
	}

	// Frame type -- let core decide for us
	xvid_enc_frame.type = XVID_TYPE_AUTO;

	// Force the right quantizer -- it is internally managed by RC plugins
	xvid_enc_frame.quant = 0;

	// Set up motion estimation flags
	xvid_enc_frame.motion = 0;

	//if (ARG_TURBO) {
	//	xvid_enc_frame.motion |= XVID_ME_FASTREFINE16 | XVID_ME_FASTREFINE8 |
	//							 XVID_ME_SKIP_DELTASEARCH | XVID_ME_FAST_MODEINTERPOLATE |
	//							 XVID_ME_BFRAME_EARLYSTOP;
	//}

	xvid_enc_frame.vop_flags |= XVID_VOP_MODEDECISION_RD;
	//xvid_enc_frame.motion |= XVID_ME_HALFPELREFINE16_RD;
	//xvid_enc_frame.motion |= XVID_ME_HALFPELREFINE8_RD;
	//xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE16_RD;
	//xvid_enc_frame.motion |= XVID_ME_QUARTERPELREFINE8_RD;
	//xvid_enc_frame.motion |= XVID_ME_CHECKPREDICTION_RD;
	//xvid_enc_frame.motion |= XVID_ME_EXTSEARCH_RD;

	// We don't use special matrices
	xvid_enc_frame.quant_intra_matrix = 0;
	xvid_enc_frame.quant_inter_matrix = 0;

	// Encode the frame
	printf("XVID write frame\n");
	int size = xvid_encore(mM4VHandle, XVID_ENC_ENCODE, &xvid_enc_frame, &xvid_enc_stats);
	if (size < 0) {
		AddError(wxT("Error while encoding a frame."));
	}

	// Write the frame
	mFile.Write(mM4VBuffer, size);
}

void THISCLASS::M4VClose() {
	mFile.Close();
	printf("XVID close\n");
	if (mM4VHandle) {
		xvid_encore(mM4VHandle, XVID_ENC_DESTROY, 0, 0);
		mM4VHandle = 0;
	}
}

#endif // USE_XVID
