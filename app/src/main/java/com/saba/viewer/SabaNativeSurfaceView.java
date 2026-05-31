package com.saba.viewer;

import android.content.Context;
import android.content.res.Configuration;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.SurfaceHolder;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class SabaNativeSurfaceView extends GLSurfaceView {

    private static final String TAG = "SabaNativeSurfaceView";
    private final SabaNativeRenderer mRenderer;

    public SabaNativeSurfaceView(Context context) {
        super(context);

        setEGLContextClientVersion(3);

        setEGLConfigChooser(8, 8, 8, 8, 24, 8);

        getHolder().setFormat(PixelFormat.RGBA_8888);

        mRenderer = new SabaNativeRenderer();
        setRenderer(mRenderer);

        setRenderMode(RENDERMODE_CONTINUOUSLY);

        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int action = event.getActionMasked();
        int pointerCount = event.getPointerCount();
        float[] x = new float[pointerCount];
        float[] y = new float[pointerCount];
        int[] ids = new int[pointerCount];

        for (int i = 0; i < pointerCount; i++) {
            x[i] = event.getX(i);
            y[i] = event.getY(i);
            ids[i] = event.getPointerId(i);
        }

        nativeOnTouchEvent(action, pointerCount, ids, x, y);
        return true;
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        nativeOnSurfaceDestroyed();
        super.surfaceDestroyed(holder);
    }

    public void onResume() {
        super.onResume();
        nativeOnResume();
    }

    public void onPause() {
        super.onPause();
        nativeOnPause();
    }

    public void onDestroy() {
        queueEvent(new Runnable() {
            @Override
            public void run() {
                nativeOnDestroy();
            }
        });
    }

    public void onConfigurationChanged(Configuration newConfig) {
        nativeOnConfigurationChanged(newConfig.orientation);
    }

    private static class SabaNativeRenderer implements GLSurfaceView.Renderer {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            nativeOnSurfaceCreated();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            nativeOnSurfaceChanged(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            nativeOnDrawFrame();
        }
    }

    private static native void nativeOnSurfaceCreated();
    private static native void nativeOnSurfaceChanged(int width, int height);
    private static native void nativeOnDrawFrame();
    private static native void nativeOnTouchEvent(int action, int pointerCount, int[] ids, float[] x, float[] y);
    private static native void nativeOnSurfaceDestroyed();
    private static native void nativeOnResume();
    private static native void nativeOnPause();
    private static native void nativeOnDestroy();
    private static native void nativeOnConfigurationChanged(int orientation);

    static {
        System.loadLibrary("saba_native");
    }
}