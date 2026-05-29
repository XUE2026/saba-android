package com.saba.viewer;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class NativeGLSurfaceView extends GLSurfaceView {

    private MainActivity mainActivity;
    private ScaleGestureDetector scaleDetector;
    private float density;

    public NativeGLSurfaceView(Context context) {
        super(context);
        init(context);
    }

    public NativeGLSurfaceView(Context context, android.util.AttributeSet attrs) {
        super(context, attrs);
        init(context);
    }

    private void init(Context context) {
        setEGLContextClientVersion(3);
        setPreserveEGLContextOnPause(true);
        setRenderer(new Renderer());

        density = context.getResources().getDisplayMetrics().density;
        scaleDetector = new ScaleGestureDetector(context, new ScaleListener());

        setRenderMode(RENDERMODE_CONTINUOUSLY);
    }

    public void setMainActivity(MainActivity activity) {
        this.mainActivity = activity;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        scaleDetector.onTouchEvent(event);

        int pointerIndex = event.getActionIndex();
        int pointerId = event.getPointerId(pointerIndex);
        float x = event.getX(pointerIndex) / density;
        float y = event.getY(pointerIndex) / density;
        int action = event.getActionMasked();

        switch (action) {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_POINTER_DOWN:
                queueEvent(() -> MainActivity.nativeTouchEvent(pointerId, x, y, 0));
                break;
            case MotionEvent.ACTION_MOVE:
                for (int i = 0; i < event.getPointerCount(); i++) {
                    int pid = event.getPointerId(i);
                    float px = event.getX(i) / density;
                    float py = event.getY(i) / density;
                    queueEvent(() -> MainActivity.nativeTouchEvent(pid, px, py, 1));
                }
                break;
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_POINTER_UP:
                queueEvent(() -> MainActivity.nativeTouchEvent(pointerId, x, y, 2));
                break;
        }
        return true;
    }

    private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            float scaleFactor = detector.getScaleFactor();
            queueEvent(() -> {
                float centerX = detector.getFocusX() / density;
                float centerY = detector.getFocusY() / density;
            });
            return true;
        }
    }

    private class Renderer implements GLSurfaceView.Renderer {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            String assetPath = getContext().getCacheDir().getAbsolutePath();
            MainActivity.nativeInit(assetPath);
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            MainActivity.nativeResize(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            MainActivity.nativeStep();
        }
    }
}