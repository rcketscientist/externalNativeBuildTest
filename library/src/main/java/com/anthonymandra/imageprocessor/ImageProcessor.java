package com.anthonymandra.imageprocessor;

import android.graphics.BitmapRegionDecoder;
import android.support.annotation.Nullable;
import android.util.Log;

import java.io.File;

public class ImageProcessor
{
	private static final String TAG = "ImageProcessor";

	static
	{
		loadLibrary();
	}

	public static void loadLibrary()
	{
		try
		{
			System.loadLibrary("imageprocessor");
		}
		catch (UnsatisfiedLinkError ule)
		{
			Log.e("JNI", ule.toString());
		}
	}

	private static native byte[] getThumbAsJpgFd
		(int source, String[] exif, int quality, @Nullable byte[] watermark, @Nullable int[] margins, int waterWidth, int waterHeight);

	/**
	 * Extracts a raw thumbnail as jpeg.
	 * This is a convenience method with a null watermark for {@link #getThumb(byte[], String[], byte[], int[], int, int)}
	 * @param source source file descriptor
	 * @param exif array to store exif fields
     * @return byte array representing the jpeg and a populated exif array
     */
	public static byte[] getThumb(int source, String[] exif)
	{
		return getThumbAsJpgFd(source, exif, 100, null, null, 0, 0);
	}

	/**
	 * Extracts a raw thumbnail and inserts a watermark.
	 * @param source source file descriptor
	 * @param exif array to store exif fields
	 * @param watermark watermark image
	 * @param margins margins defining location of watermark
	 * @param waterWidth width of the watermark
	 * @param waterHeight height of the watermark
     * @return byte array representing the jpeg and a populated exif array
     */
	public static byte[] getThumb(int source, String[] exif, @Nullable byte[] watermark, @Nullable int[] margins, int waterWidth, int waterHeight)
	{
		return getThumbAsJpgFd(source, exif, 100, watermark, margins, waterWidth, waterHeight);
	}
}
