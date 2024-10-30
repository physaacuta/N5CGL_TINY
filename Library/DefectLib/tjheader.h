/*
* Copyright (C)2009-2015, 2017 D. R. Commander.  All Rights Reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* - Redistributions of source code must retain the above copyright notice,
*   this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above copyright notice,
*   this list of conditions and the following disclaimer in the documentation
*   and/or other materials provided with the distribution.
* - Neither the name of the libjpeg-turbo Project nor the names of its
*   contributors may be used to endorse or promote products derived from this
*   software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS",
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*/


typedef void *tjhandle;


enum TJSAMP {
	/**
	* 4:4:4 chrominance subsampling (no chrominance subsampling).  The JPEG or
	* YUV image will contain one chrominance component for every pixel in the
	* source image.
	*/
	TJSAMP_444 = 0,
	/**
	* 4:2:2 chrominance subsampling.  The JPEG or YUV image will contain one
	* chrominance component for every 2x1 block of pixels in the source image.
	*/
	TJSAMP_422,
	/**
	* 4:2:0 chrominance subsampling.  The JPEG or YUV image will contain one
	* chrominance component for every 2x2 block of pixels in the source image.
	*/
	TJSAMP_420,
	/**
	* Grayscale.  The JPEG or YUV image will contain no chrominance components.
	*/
	TJSAMP_GRAY,
	/**
	* 4:4:0 chrominance subsampling.  The JPEG or YUV image will contain one
	* chrominance component for every 1x2 block of pixels in the source image.
	*
	* @note 4:4:0 subsampling is not fully accelerated in libjpeg-turbo.
	*/
	TJSAMP_440,
	/**
	* 4:1:1 chrominance subsampling.  The JPEG or YUV image will contain one
	* chrominance component for every 4x1 block of pixels in the source image.
	* JPEG images compressed with 4:1:1 subsampling will be almost exactly the
	* same size as those compressed with 4:2:0 subsampling, and in the
	* aggregate, both subsampling methods produce approximately the same
	* perceptual quality.  However, 4:1:1 is better able to reproduce sharp
	* horizontal features.
	*
	* @note 4:1:1 subsampling is not fully accelerated in libjpeg-turbo.
	*/
	TJSAMP_411
};


/**
* Pixel formats
*/
enum TJPF {
	/**
	* RGB pixel format.  The red, green, and blue components in the image are
	* stored in 3-byte pixels in the order R, G, B from lowest to highest byte
	* address within each pixel.
	*/
	TJPF_RGB = 0,
	/**
	* BGR pixel format.  The red, green, and blue components in the image are
	* stored in 3-byte pixels in the order B, G, R from lowest to highest byte
	* address within each pixel.
	*/
	TJPF_BGR,
	/**
	* RGBX pixel format.  The red, green, and blue components in the image are
	* stored in 4-byte pixels in the order R, G, B from lowest to highest byte
	* address within each pixel.  The X component is ignored when compressing
	* and undefined when decompressing.
	*/
	TJPF_RGBX,
	/**
	* BGRX pixel format.  The red, green, and blue components in the image are
	* stored in 4-byte pixels in the order B, G, R from lowest to highest byte
	* address within each pixel.  The X component is ignored when compressing
	* and undefined when decompressing.
	*/
	TJPF_BGRX,
	/**
	* XBGR pixel format.  The red, green, and blue components in the image are
	* stored in 4-byte pixels in the order R, G, B from highest to lowest byte
	* address within each pixel.  The X component is ignored when compressing
	* and undefined when decompressing.
	*/
	TJPF_XBGR,
	/**
	* XRGB pixel format.  The red, green, and blue components in the image are
	* stored in 4-byte pixels in the order B, G, R from highest to lowest byte
	* address within each pixel.  The X component is ignored when compressing
	* and undefined when decompressing.
	*/
	TJPF_XRGB,
	/**
	* Grayscale pixel format.  Each 1-byte pixel represents a luminance
	* (brightness) level from 0 to 255.
	*/
	TJPF_GRAY,
	/**
	* RGBA pixel format.  This is the same as @ref TJPF_RGBX, except that when
	* decompressing, the X component is guaranteed to be 0xFF, which can be
	* interpreted as an opaque alpha channel.
	*/
	TJPF_RGBA,
	/**
	* BGRA pixel format.  This is the same as @ref TJPF_BGRX, except that when
	* decompressing, the X component is guaranteed to be 0xFF, which can be
	* interpreted as an opaque alpha channel.
	*/
	TJPF_BGRA,
	/**
	* ABGR pixel format.  This is the same as @ref TJPF_XBGR, except that when
	* decompressing, the X component is guaranteed to be 0xFF, which can be
	* interpreted as an opaque alpha channel.
	*/
	TJPF_ABGR,
	/**
	* ARGB pixel format.  This is the same as @ref TJPF_XRGB, except that when
	* decompressing, the X component is guaranteed to be 0xFF, which can be
	* interpreted as an opaque alpha channel.
	*/
	TJPF_ARGB,
	/**
	* CMYK pixel format.  Unlike RGB, which is an additive color model used
	* primarily for display, CMYK (Cyan/Magenta/Yellow/Key) is a subtractive
	* color model used primarily for printing.  In the CMYK color model, the
	* value of each color component typically corresponds to an amount of cyan,
	* magenta, yellow, or black ink that is applied to a white background.  In
	* order to convert between CMYK and RGB, it is necessary to use a color
	* management system (CMS.)  A CMS will attempt to map colors within the
	* printer's gamut to perceptually similar colors in the display's gamut and
	* vice versa, but the mapping is typically not 1:1 or reversible, nor can it
	* be defined with a simple formula.  Thus, such a conversion is out of scope
	* for a codec library.  However, the TurboJPEG API allows for compressing
	* CMYK pixels into a YCCK JPEG image (see #TJCS_YCCK) and decompressing YCCK
	* JPEG images into CMYK pixels.
	*/
	TJPF_CMYK,
	/**
	* Unknown pixel format.  Currently this is only used by #tjLoadImage().
	*/
	TJPF_UNKNOWN = -1
};
/**
* The uncompressed source/destination image is stored in bottom-up (Windows,
* OpenGL) order, not top-down (X11) order.
*/
#define TJFLAG_BOTTOMUP  2
/**
* When decompressing an image that was compressed using chrominance
* subsampling, use the fastest chrominance upsampling algorithm available in
* the underlying codec.  The default is to use smooth upsampling, which
* creates a smooth transition between neighboring chrominance components in
* order to reduce upsampling artifacts in the decompressed image.
*/
#define TJFLAG_FASTUPSAMPLE  256
/**
* Disable buffer (re)allocation.  If passed to one of the JPEG compression or
* transform functions, this flag will cause those functions to generate an
* error if the JPEG image buffer is invalid or too small rather than
* attempting to allocate or reallocate that buffer.  This reproduces the
* behavior of earlier versions of TurboJPEG.
*/
#define TJFLAG_NOREALLOC  1024
/**
* Use the fastest DCT/IDCT algorithm available in the underlying codec.  The
* default if this flag is not specified is implementation-specific.  For
* example, the implementation of TurboJPEG for libjpeg[-turbo] uses the fast
* algorithm by default when compressing, because this has been shown to have
* only a very slight effect on accuracy, but it uses the accurate algorithm
* when decompressing, because this has been shown to have a larger effect.
*/
#define TJFLAG_FASTDCT  2048
/**
* Use the most accurate DCT/IDCT algorithm available in the underlying codec.
* The default if this flag is not specified is implementation-specific.  For
* example, the implementation of TurboJPEG for libjpeg[-turbo] uses the fast
* algorithm by default when compressing, because this has been shown to have
* only a very slight effect on accuracy, but it uses the accurate algorithm
* when decompressing, because this has been shown to have a larger effect.
*/
#define TJFLAG_ACCURATEDCT  4096
/**
* Immediately discontinue the current compression/decompression/transform
* operation if the underlying codec throws a warning (non-fatal error).  The
* default behavior is to allow the operation to complete unless a fatal error
* is encountered.
*/
#define TJFLAG_STOPONWARNING  8192
/**
* Use progressive entropy coding in JPEG images generated by the compression
* and transform functions.  Progressive entropy coding will generally improve
* compression relative to baseline entropy coding (the default), but it will
* reduce compression and decompression performance considerably.
*/
#define TJFLAG_PROGRESSIVE  16384

static tjhandle(*p_tjInitCompress)(void);
static int(*p_tjDestroy)(tjhandle handle);
static int(*p_tjCompress2)(tjhandle handle, const unsigned char *srcBuf,
	int width, int pitch, int height, int pixelFormat,
	unsigned char **jpegBuf, unsigned long *jpegSize,
	int jpegSubsamp, int jpegQual, int flags);


tjhandle (*p_tjInitDecompress)(void);
int (*p_tjDecompressHeader3)(tjhandle handle,
	const unsigned char *jpegBuf,
	unsigned long jpegSize, int *width,
	int *height, int *jpegSubsamp,
	int *jpegColorspace);
int (*p_tjDecompress2)(tjhandle handle, const unsigned char *jpegBuf,
	unsigned long jpegSize, unsigned char *dstBuf,
	int width, int pitch, int height, int pixelFormat,
	int flags);

