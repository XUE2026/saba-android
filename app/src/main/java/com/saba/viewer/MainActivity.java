package com.saba.viewer;

import android.app.NativeActivity;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;

public class MainActivity extends NativeActivity {

    private SabaNativeSurfaceView mNativeSurfaceView;
    private boolean mImmersiveModeEnabled = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        mNativeSurfaceView = new SabaNativeSurfaceView(this);
        setContentView(mNativeSurfaceView);

        enableImmersiveMode();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mNativeSurfaceView.onResume();
        enableImmersiveMode();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mNativeSurfaceView.onPause();
    }

    @Override
    protected void onDestroy() {
        mNativeSurfaceView.onDestroy();
        super.onDestroy();
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus && mImmersiveModeEnabled) {
            enableImmersiveMode();
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        mNativeSurfaceView.onConfigurationChanged(newConfig);
    }

    private void enableImmersiveMode() {
        if (mImmersiveModeEnabled) {
            mNativeSurfaceView.setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_FULLSCREEN
            );
        }
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return mNativeSurfaceView.onTouchEvent(event);
    }
}