package com.cn.luo.ndk;

import android.graphics.Bitmap;
import android.support.annotation.NonNull;

/**
 * AUTHOR:       Luo
 * VERSION:      V1.0
 * DESCRIPTION:  description
 * CREATE TIME:        2017/6/13 16:36
 * NOTE:
 */
public class StackBlurUtil {
    static {
        System.loadLibrary("StackBlur");
    }

    public static Bitmap convertBitmapBlur(@NonNull Bitmap mBitmap) {
        Bitmap bitmap = mBitmap.copy(mBitmap.getConfig(), true);//做一份Bitmap拷贝，拷贝的图片是可以被修改的。
        int w = bitmap.getWidth();
        int h = bitmap.getHeight();
        int[] pix = new int[w * h];
        bitmap.getPixels(pix, 0, w, 0, 0, w, h);
        bitmap.setPixels(stackBlur(pix, w, h, 20), 0, w, 0, 0, w, h);//调用jni中的C方法处理图片，并修改图片的pixels值实现毛玻璃效果
        if (!mBitmap.isRecycled()) {
            mBitmap.recycle();
        }

        return bitmap;
    }

    public static native int[] stackBlur(int[] pix, int width, int height, int radius);
}
