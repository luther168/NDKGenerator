package com.cn.luo.ndk;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ImageView;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Bitmap srcBitmap = BitmapFactory.decodeResource(getResources(), R.mipmap.blur_img);
        Bitmap blurredBitmap = StackBlurUtil.convertBitmapBlur(srcBitmap);
        ImageView img = (ImageView) findViewById(R.id.img);
        img.setImageBitmap(blurredBitmap);
    }
}
