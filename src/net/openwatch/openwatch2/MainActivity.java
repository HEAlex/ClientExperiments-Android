package net.openwatch.openwatch2;

import java.io.File;
import java.io.IOException;
import java.util.Date;

import net.openwatch.openwatch2.audio.AudioHardwareRecorder;
import net.openwatch.openwatch2.audio.AudioSoftwareRecorder;
import net.openwatch.openwatch2.audio.AudioStreamer;
import net.openwatch.openwatch2.audio.FFAudioEncoder;
import net.openwatch.openwatch2.constants.OWConstants;
import net.openwatch.openwatch2.file.FileUtils;
import net.openwatch.openwatch2.video.DualVideoRecorder;
import net.openwatch.openwatch2.video.VideoHardwareRecorder;
import net.openwatch.openwatch2.video.VideoSoftwareRecorder;
import net.openwatch.openwatch2.video.FFVideoEncoder;
import android.os.Build;
import android.os.Bundle;
import android.provider.SyncStateContract.Constants;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.view.Menu;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class MainActivity extends Activity {
	
	private Button record_hw_video_btn;
	private Button record_sw_video_btn;
	
	private AudioSoftwareRecorder audio_software_recorder = new AudioSoftwareRecorder();

	@SuppressLint("NewApi")
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		if (Build.VERSION.SDK_INT >= 11) {
			this.getActionBar().setDisplayShowTitleEnabled(false);
			this.getActionBar().setTitle("OW Tech Demo");
		}

		record_hw_video_btn = (Button) findViewById(R.id.record_hw_video_btn);
		record_hw_video_btn.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				if (VideoHardwareRecorder.is_recording) {
					VideoHardwareRecorder.stopRecording();
					//AudioHardwareRecorder.stopRecording();
					record_sw_video_btn.setEnabled(true);
					((Button) v).setText("Start HW Recording Video");
				} else {
					String video_filename = String.valueOf(new Date().getTime()) + "_AV.mp4";
					File video_output_file = new File(
							FileUtils.getExternalStorage(MainActivity.this,
									OWConstants.recording_directory), video_filename);

					VideoHardwareRecorder.startRecording(
							(SurfaceView) MainActivity.this
									.findViewById(R.id.camera_surface_view),
							video_output_file);
					
					// See if an audio recording can take place simultaneously
					// nope. Throws IllegalStateException
					// Even when a different hardware encoder is used
					// i.e: VideoRecorder using AAC audio hardware and AudioRecorder using AMR hardware
					/*
					String audio_filename = String.valueOf(new Date().getTime()) + "_A.3gpp";
					File audio_output_file = new File(FileUtils.getExternalStorage(MainActivity.this, OWConstants.recording_directory), audio_filename);
					AudioRecorder.startRecording(audio_output_file);
					*/
					record_sw_video_btn.setEnabled(false);
					((Button) v).setText("Stop HW Recording Video");
				}

			}

		});
		
		record_sw_video_btn = (Button) findViewById(R.id.record_sw_video_btn);
		record_sw_video_btn.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				if(VideoSoftwareRecorder.is_recording){
					VideoSoftwareRecorder.stopRecording();
					record_hw_video_btn.setEnabled(true);
					((Button) v).setText("Start SW Video Recording");
				}
				else{
					String video_filename = String.valueOf(new Date().getTime()) + ".mpg";
					File video_output_file = new File(
							FileUtils.getExternalStorage(MainActivity.this,
									OWConstants.recording_directory), video_filename);

					VideoSoftwareRecorder.startRecording(
							(SurfaceView) MainActivity.this
									.findViewById(R.id.camera_surface_view),
							video_output_file);
					
					record_hw_video_btn.setEnabled(false);
					((Button) v).setText("Stop SW Video Recording");
				}
				
			}
			
		});
		
		Button test_btn = (Button) findViewById(R.id.test_btn);
		test_btn.setOnClickListener(new OnClickListener(){

			@Override
			public void onClick(View v) {
				if(audio_software_recorder.is_recording){
					//audio_software_recorder.stopRecording();
					//DualVideoRecorder.stopRecording();
					
					((Button)v).setText("Start SW Audio Recording");
					
				} else {
					String output_filename = "/sdcard/ffmpeg_testing/" + String.valueOf(new Date().getTime()) + ".mp3";
					
					File test_file = new File(output_filename);
					if(!test_file.exists()){
						try {
							test_file.createNewFile();
						} catch (IOException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
					FFAudioEncoder.testFFMPEG(output_filename);
					/*
					DualVideoRecorder.startRecording((SurfaceView) MainActivity.this
							.findViewById(R.id.camera_surface_view), output_dir);*/
					
					//audio_software_recorder.startRecording(new File(output_dir));
					((Button)v).setText("Stop SW Audio Recording");
					/*
					FFEncoder ffencoder = new FFEncoder();
					ffencoder.initializeEncoder(test_filename, 320, 240);
					ffencoder.encodeFrame(new byte[]{});
					ffencoder.finalizeEncoder();
					*/
				}
			}
			
		});
		
		
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

	@Override
	public void onPause() {
		super.onPause();

		/*
		 * // Release the MediaRecorder if (AudioStreamer.recorder != null) {
		 * AudioStreamer.recorder.release(); AudioStreamer.recorder = null; }
		 */
	}
}
