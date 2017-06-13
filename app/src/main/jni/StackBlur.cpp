#include <jni.h>
#include <stdio.h>
#include <malloc.h>
#include <android/log.h>

#define MAX(a, b)((a>b)?(a):(b))
#define MIN(a, b)((a<b)?(a):(b))
#define ABS(x) ((x)>=0 ? (x):(-(x)))

#define TAG "stackBlur" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型

extern "C" {
JNIEXPORT jintArray  JNICALL
Java_com_cn_luo_ndk_StackBlurUtil_stackBlur(JNIEnv *env, jobject obj, jintArray array1, jint w,
                                            jint h,
                                            jint radius);
};

JNIEXPORT jintArray  JNICALL
Java_com_cn_luo_ndk_StackBlurUtil_stackBlur(JNIEnv *env, jobject obj, jintArray array1, jint w,
                                            jint h,
                                            jint radius) {
    const jsize length = env->GetArrayLength(array1);
    jintArray newArray = env->NewIntArray(length);

    jint *pix;
    pix = env->GetIntArrayElements(array1, NULL);
    jint *narr = env->GetIntArrayElements(newArray, NULL);

    jint wm = w - 1;
    jint hm = h - 1;
    jint wh = w * h;
    jint div = radius + radius + 1;

    jint *r = (jint *) malloc(wh * sizeof(jint));
    jint *g = (jint *) malloc(wh * sizeof(jint));
    jint *b = (jint *) malloc(wh * sizeof(jint));
    jint rsum, gsum, bsum, x, y, i, p, yp, yi, yw;

    jint *vmin = (jint *) malloc(MAX(w, h) * sizeof(jint));

    jint divsum = (div + 1) >> 1;
    divsum *= divsum;
    jint *dv = (jint *) malloc(256 * divsum * sizeof(jint));
    for (i = 0; i < 256 * divsum; i++) {
        dv[i] = (i / divsum);
    }

    yw = yi = 0;

    jint(*stack)[3] = (jint(*)[3]) malloc(div * 3 * sizeof(jint));
    jint stackpointer;
    jint stackstart;
    jint *sir;
    jint rbs;
    jint r1 = radius + 1;
    jint routsum, goutsum, boutsum;
    jint rinsum, ginsum, binsum;
    for (y = 0; y < h; y++) {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        for (i = -radius; i <= radius; i++) {
            p = pix[yi + (MIN(wm, MAX(i, 0)))];
            sir = stack[i + radius];
            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);

            rbs = r1 - ABS(i);
            rsum += sir[0] * rbs;
            gsum += sir[1] * rbs;
            bsum += sir[2] * rbs;
            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }
        }
        stackpointer = radius;

        for (x = 0; x < w; x++) {

            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];

            if (y == 0) {
                vmin[x] = MIN(x + radius + 1, wm);
            }
            p = pix[yw + vmin[x]];

            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[(stackpointer) % div];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];

            yi++;
        }
        yw += w;
    }
    for (x = 0; x < w; x++) {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        yp = -radius * w;
        for (i = -radius; i <= radius; i++) {
            yi = MAX(0, yp) + x;

            sir = stack[i + radius];

            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];

            rbs = r1 - ABS(i);

            rsum += r[yi] * rbs;
            gsum += g[yi] * rbs;
            bsum += b[yi] * rbs;

            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }

            if (i < hm) {
                yp += w;
            }
        }
        yi = x;
        stackpointer = radius;
        for (y = 0; y < h; y++) {
            // Preserve alpha channel: ( 0xff000000 & pix[yi] )
            pix[yi] = (0xff000000 & pix[yi]) | (dv[rsum] << 16) | (dv[gsum] << 8) | dv[bsum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];

            if (x == 0) {
                vmin[y] = MIN(y + r1, hm) * w;
            }
            p = x + vmin[y];

            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[stackpointer];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];

            yi += w;
        }
    }

    free(r);
    free(g);
    free(b);
    free(vmin);
    free(dv);
    free(stack);

    for (jint x = 0; x < length; x++) {
        narr[x] = pix[x];
    }
    env->ReleaseIntArrayElements(newArray, narr, NULL);
    env->ReleaseIntArrayElements(array1, pix, NULL);
    return newArray;
}

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    void *venv;
    if (vm->GetEnv((void **) &venv, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    return JNI_VERSION_1_4;
}