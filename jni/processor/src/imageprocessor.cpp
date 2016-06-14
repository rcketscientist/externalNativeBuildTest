#include <string.h>
#include <android/log.h>

#include "imageprocessor.h"

#define  LOG_TAG    "libimageprocessor"
#define  LOGI(...)  ((void)__android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define  LOGE(...)  ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

#include <jpeglib.h>
#include <assert.h>

void createJpeg(unsigned char** outJpeg, unsigned long* outJpegSize, unsigned char* data, int width, int height, int components, J_COLOR_SPACE, int orientation, int quality);
unsigned char* readJpeg(unsigned char* data, unsigned long size, int width, int height);

JNIEXPORT jbyteArray JNICALL Java_com_anthonymandra_imageprocessor_ImageProcessor_getThumbAsJpgFd
        (JNIEnv *env, jclass type, jint source, jobjectArray exif, jint quality, jbyteArray watermark, jintArray margins, jint waterWidth, jint waterHeight)
{
	return NULL;
}
unsigned char* readJpeg(unsigned char* data, unsigned long size, int width, int height)
{
	// http://sourceforge.net/p/libjpeg-turbo/code/HEAD/tree/trunk/example.c#l109
	// http://stackoverflow.com/questions/5616216/need-help-in-reading-jpeg-file-using-libjpeg

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	int row_stride;		/* physical row width in output buffer */

	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */
	jpeg_mem_src(&cinfo, data, size);
	/* Step 3: read file parameters with jpeg_read_header() */
	(void) jpeg_read_header(&cinfo, TRUE);

	/* Step 4: set parameters for decompression */

	/* In this example, we don't need to change any of the defaults set by
	* jpeg_read_header(), so we do nothing here.
	*/

	/* Step 5: Start decompressor */
	(void) jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* We may need to do some setup of our own at this point before reading
	* the data.  After jpeg_start_decompress() we have the correct scaled
	* output image dimensions available, as well as the output colormap
	* if we asked for color quantization.
	* In this example, we need to make an output work buffer of the right size.
	*/
	/* JSAMPLEs per row in output buffer */
	//AJM: cinfo has incorrect width/height information
//	row_stride = cinfo.output_width * cinfo.output_components;
	row_stride = width * cinfo.output_components;
	JSAMPROW rowData;
	unsigned char* imageData = new unsigned char[height * row_stride];
	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	int row = 0;
	while (cinfo.output_scanline < cinfo.output_height)
	{
		rowData = imageData + (row * row_stride);
		jpeg_read_scanlines(&cinfo, &rowData, 1);
		++row;
	}

	/* Step 7: Finish decompression */
	(void) jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	* with the stdio data source.
	*/

	/* Step 8: Release JPEG decompression object */
	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);
	/* At this point you may want to check to see whether any corrupt-data
	* warnings occurred (test whether jerr.pub.num_warnings is nonzero).
	*/

	/* And we're done! */
	return imageData;
}

//TODO: return a value
void createJpeg(unsigned char** outJpeg, unsigned long int* outJpegSize, unsigned char* data, int width, int height, int components, J_COLOR_SPACE color_space, int orientation, int quality)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	int row_stride;				/* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	jpeg_mem_dest(&cinfo, outJpeg, outJpegSize);

    cinfo.image_width = width; 	/* image width and height, in pixels */
	cinfo.image_height = height;
	cinfo.input_components = components;			/* # of color components per pixel */
	cinfo.in_color_space = color_space; 	    /* colorspace of input image */

	/* Now use the library's routine to set default compression parameters.
	* (You must set at least cinfo.in_color_space before calling this,
	* since the defaults depend on the source color space.)
	*/
	jpeg_set_defaults(&cinfo);
	/* Now you can set any non-default parameters you wish to.
	* Here we just illustrate the use of quality (quantization table) scaling:
	*/
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	/* Step 4: Start compressor */

	/* TRUE ensures that we will write a complete interchange-JPEG file.
	* Pass TRUE unless you are very sure of what you're doing.
	*/
	jpeg_start_compress(&cinfo, TRUE);

	/* Step 5: while (scan lines remain to be written) */
	/*           jpeg_write_scanlines(...); */

	/* Here we use the library's state variable cinfo.next_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	* To keep things simple, we pass one scanline per call; you can pass
	* more if you wish, though.
	*/
//	row_stride = cinfo.image_width * cinfo.in_color_space;	/* JSAMPLEs per row in image_buffer */
	row_stride = cinfo.image_width * components;
	while (cinfo.next_scanline < cinfo.image_height)
	{
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could pass
		 * more than one scanline at a time if that's more convenient.
		 */
		row_pointer[0] = &data[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	/* Step 6: Finish compression */

	jpeg_finish_compress(&cinfo);

	/* Step 7: release JPEG compression object */

	/* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_compress(&cinfo);
//    writeExif(outJpeg, *outJpegSize, orientation);

//	return mem;
}