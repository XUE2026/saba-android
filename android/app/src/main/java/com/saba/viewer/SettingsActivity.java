package com.saba.viewer;

import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Spinner;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

public class SettingsActivity extends AppCompatActivity {

    private Spinner filterSpinner;
    private SeekBar filterIntensitySeekBar;
    private TextView filterIntensityValue;
    private Spinner orientationSpinner;
    private SeekBar environmentBrightness;
    private SeekBar environmentContrast;
    private SeekBar toonIntensity;
    private Button applyButton;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_settings);

        filterSpinner = findViewById(R.id.filter_spinner);
        filterIntensitySeekBar = findViewById(R.id.filter_intensity_seekbar);
        filterIntensityValue = findViewById(R.id.filter_intensity_value);
        orientationSpinner = findViewById(R.id.orientation_spinner);
        environmentBrightness = findViewById(R.id.environment_brightness);
        environmentContrast = findViewById(R.id.environment_contrast);
        toonIntensity = findViewById(R.id.toon_intensity);
        applyButton = findViewById(R.id.apply_button);

        ArrayAdapter<CharSequence> filterAdapter = ArrayAdapter.createFromResource(this,
            R.array.filter_options, android.R.layout.simple_spinner_item);
        filterAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        filterSpinner.setAdapter(filterAdapter);

        ArrayAdapter<CharSequence> orientationAdapter = ArrayAdapter.createFromResource(this,
            R.array.orientation_options, android.R.layout.simple_spinner_item);
        orientationAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        orientationSpinner.setAdapter(orientationAdapter);

        filterIntensitySeekBar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                filterIntensityValue.setText(String.format("%.2f", progress / 100.0f));
            }
            @Override public void onStartTrackingTouch(SeekBar seekBar) {}
            @Override public void onStopTrackingTouch(SeekBar seekBar) {}
        });

        applyButton.setOnClickListener(v -> applySettings());
    }

    private void applySettings() {
        int filterIndex = filterSpinner.getSelectedItemPosition();
        float filterIntensity = filterIntensitySeekBar.getProgress() / 100.0f;
        int orientation = orientationSpinner.getSelectedItemPosition();

        if (NativeGLSurfaceView.getStaticInstance() != null) {
            NativeGLSurfaceView.getStaticInstance().queueEvent(() -> {
                MainActivity.nativeSetFilter(filterIndex);
                MainActivity.nativeSetFilterParam(0, filterIntensity);
                MainActivity.nativeSetScreenOrientation(orientation);
            });
        }
        finish();
    }
}