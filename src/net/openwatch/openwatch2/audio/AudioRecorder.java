package net.openwatch.openwatch2.audio;

import java.io.File;
import java.io.IOException;

import android.media.MediaRecorder;
import android.util.Log;

public class AudioRecorder {
	
	public static final String TAG = "AudioRecorder";
	
	public static MediaRecorder audio_recorder;
	
	public static void startRecording(File output_file) {
        audio_recorder = new MediaRecorder();
        audio_recorder.setAudioSource(MediaRecorder.AudioSource.MIC);
        audio_recorder.setOutputFormat(MediaRecorder.OutputFormat.MPEG_4);
        audio_recorder.setOutputFile(output_file.getAbsolutePath());
        audio_recorder.setAudioEncoder(MediaRecorder.AudioEncoder.AAC);

        try {
            audio_recorder.prepare();
        } catch (IOException e) {
            Log.e(TAG, "prepare() failed");
        }

        audio_recorder.start();
    }
	
	public static void stopRecording() {
		audio_recorder.stop();
		audio_recorder.release();
		audio_recorder = null;
    }

}