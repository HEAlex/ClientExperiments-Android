#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <android/log.h>

#include <math.h>

#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/mathematics.h>
#include <libavutil/samplefmt.h>

#define LOG_TAG "FFVideoEncoder"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

AVCodec *codec;
AVCodecContext *c= NULL;
AVFrame *frame;
AVPacket pkt;
int i, out_size, x, y, outbuf_size;
FILE *f;
uint8_t *outbuf;
int had_output=0;

void Java_net_openwatch_openwatch2_video_FFVideoEncoder_internalInitializeEncoder(JNIEnv * env, jobject this, jstring filename, jint width, jint height){

	// Convert Java types
	const jbyte *native_filename;
	native_filename = (*env)->GetStringUTFChars(env, filename, NULL);

	int native_height = (int) height;
	int native_width = (int) width;

	avcodec_register_all();

	int codec_id = CODEC_ID_MPEG1VIDEO;

	LOGI("Encode video file %s", native_filename);
	/* find the mpeg1 video encoder */
	codec = avcodec_find_encoder(codec_id);
	if (!codec) {
		//fprintf(stderr, "codec not found\n");
		LOGI("codec not found");
		exit(1);
	}

	c = avcodec_alloc_context3(codec);
	frame= avcodec_alloc_frame();

	LOGI("alloc AVFrame and AVCodecContext");

	/* put sample parameters */
	c->bit_rate = 400000;
	/* resolution must be a multiple of two */
	c->width = native_width;
	c->height = native_height;
	/* frames per second */
	c->time_base= (AVRational){1,25};
	c->gop_size = 10; /* emit one intra frame every ten frames */
	c->max_b_frames=1;
	c->pix_fmt = PIX_FMT_YUV420P;

	LOGI("AVCodecContext initialize");

	//if(codec_id == CODEC_ID_H264)
		//av_opt_set(c->priv_data, "preset", "slow", 0);

	/* open it */
	if (avcodec_open2(c, codec, NULL) < 0) {
		//fprintf(stderr, "could not open codec\n");
		LOGE("could not open codec");
		exit(1);
	}

	LOGI("open avcodec");

	f = fopen(native_filename, "wb");
	if (!f) {
		//fprintf(stderr, "could not open %s\n", filename);
		LOGE("could not open %s", native_filename);
		exit(1);
	}

	LOGI("open file");

	/* alloc image and output buffer */
	outbuf_size = 100000 + 12*c->width*c->height;
	outbuf = malloc(outbuf_size);

	LOGI("malloc outbuf");

	/* the image can be allocated by any means and av_image_alloc() is
	 * just the most convenient way if av_malloc() is to be used */
	av_image_alloc(frame->data, frame->linesize,
				   c->width, c->height, c->pix_fmt, 1);

	LOGI("malloc av_image");

}

void Java_net_openwatch_openwatch2_video_FFVideoEncoder_encodeFrame(JNIEnv * env, jobject this, jbyteArray frame_data){
	//LOGI("Encode frame");
	// Convert Java types
	int frame_data_length = (*env)->GetArrayLength(env, frame_data);
	jboolean is_copy;
	jbyte *native_frame_data = (*env)->GetByteArrayElements(env, frame_data, &is_copy);

	//LOGI("Get native frame: is_copy: %d", is_copy);

	/* encode 1 second of video */
		fflush(stdout);
		/* prepare a dummy image */
		/* Y */
		for(y=0;y<c->height;y++) {
			for(x=0;x<c->width;x++) {
				frame->data[0][y * frame->linesize[0] + x] = native_frame_data[0];
				native_frame_data++;
			}
		}

		/* Cb and Cr */
		for(y=0;y<c->height/2;y++) {
			for(x=0;x<c->width/2;x++) {
				frame->data[2][y * frame->linesize[2] + x] = native_frame_data[0];
				frame->data[1][y * frame->linesize[1] + x] = native_frame_data[1];
				native_frame_data+=2;
			}
		}

		/* encode the image */
		out_size = avcodec_encode_video(c, outbuf, outbuf_size, frame);
		had_output |= out_size;
		//printf("encoding frame %3d (size=%5d)\n", i, out_size);
		fwrite(outbuf, 1, out_size, f);

		(*env)->ReleaseByteArrayElements(env, frame_data, native_frame_data, 0);
}

void Java_net_openwatch_openwatch2_video_FFVideoEncoder_finalizeEncoder(JNIEnv * env, jobject this){
	LOGI("finalize encoder");

	/* get the delayed frames */
	for(; out_size || !had_output; i++) {
		//fflush(stdout);

		out_size = avcodec_encode_video(c, outbuf, outbuf_size, NULL);
		had_output |= out_size;
		//printf("write frame %3d (size=%5d)\n", i, out_size);
		LOGI("write frame %3d (size=%5d)", i, out_size);
		fwrite(outbuf, 1, out_size, f);
	}

	/* add sequence end code to have a real mpeg file */
	outbuf[0] = 0x00;
	outbuf[1] = 0x00;
	outbuf[2] = 0x01;
	outbuf[3] = 0xb7;
	fwrite(outbuf, 1, 4, f);
	fclose(f);
	free(outbuf);

	avcodec_close(c);
	av_free(c);
	av_free(frame->data[0]);
	av_free(frame);
	//printf("\n");

}

void Java_net_openwatch_openwatch2_video_FFVideoEncoder_testFFMPEG(JNIEnv * env, jclass clazz, jstring filename){

	AVCodec *codec;
	AVCodecContext *c= NULL;
	int i, out_size, x, y, outbuf_size;
	FILE *f;
	AVFrame *picture;
	uint8_t *outbuf;
	int had_output=0;

	const jbyte *native_filename;
	native_filename = (*env)->GetStringUTFChars(env, filename, NULL);

	avcodec_register_all();

	//printf("Encode video file %s\n", native_filename);
	LOGI("Test method called");
	int codec_id = CODEC_ID_H264;
	//int codec_id =  CODEC_ID_MPEG4;
	//int codec_id = CODEC_ID_MPEG1VIDEO;
	/* find the mpeg1 video encoder */
	codec = avcodec_find_encoder(codec_id);
	if (!codec) {
		LOGE("codec not found");
		//fprintf(stderr, "codec not found\n");
		exit(1);
	}

	c = avcodec_alloc_context3(codec);
	LOGI("avcodec alloc context3");
	picture= avcodec_alloc_frame();

	/* put sample parameters */
	c->bit_rate = 400000;
	/* resolution must be a multiple of two */
	c->width = 352;
	c->height = 288;
	/* frames per second */
	c->time_base= (AVRational){1,25};
	c->gop_size = 10; /* emit one intra frame every ten frames */
	c->max_b_frames=1;
	c->pix_fmt = PIX_FMT_YUV420P;

	if(codec_id == CODEC_ID_H264)
		av_opt_set(c->priv_data, "preset", "slow", 0);

	/* open it */
	if (avcodec_open2(c, codec, NULL) < 0) {
		LOGE("could not open codec");
		//fprintf(stderr, "could not open codec\n");
		exit(1);
	}

	f = fopen(native_filename, "wb");
	if (!f) {
		LOGE("Could not open file");
		//fprintf(stderr, "could not open %s\n", filename);
		exit(1);
	}

	/* alloc image and output buffer */
	outbuf_size = 100000 + 12*c->width*c->height;
	outbuf = malloc(outbuf_size);

	/* the image can be allocated by any means and av_image_alloc() is
	 * just the most convenient way if av_malloc() is to be used */
	av_image_alloc(picture->data, picture->linesize,
				   c->width, c->height, c->pix_fmt, 1);

	/* encode 1 second of video */
	for(i=0;i<25;i++) {
		fflush(stdout);
		/* prepare a dummy image */
		/* Y */
		for(y=0;y<c->height;y++) {
			for(x=0;x<c->width;x++) {
				picture->data[0][y * picture->linesize[0] + x] = x + y + i * 3;
			}
		}

		/* Cb and Cr */
		for(y=0;y<c->height/2;y++) {
			for(x=0;x<c->width/2;x++) {
				picture->data[1][y * picture->linesize[1] + x] = 128 + y + i * 2;
				picture->data[2][y * picture->linesize[2] + x] = 64 + x + i * 5;
			}
		}

		/* encode the image */
		out_size = avcodec_encode_video(c, outbuf, outbuf_size, picture);
		had_output |= out_size;
		//printf("encoding frame %3d (size=%5d)\n", i, out_size);
		fwrite(outbuf, 1, out_size, f);
	}

	/* get the delayed frames */
	for(; out_size || !had_output; i++) {
		fflush(stdout);

		out_size = avcodec_encode_video(c, outbuf, outbuf_size, NULL);
		had_output |= out_size;
		//printf("write frame %3d (size=%5d)\n", i, out_size);
		fwrite(outbuf, 1, out_size, f);
	}

	/* add sequence end code to have a real mpeg file */
	outbuf[0] = 0x00;
	outbuf[1] = 0x00;
	outbuf[2] = 0x01;
	outbuf[3] = 0xb7;
	fwrite(outbuf, 1, 4, f);
	fclose(f);
	free(outbuf);

	avcodec_close(c);
	av_free(c);
	av_free(picture->data[0]);
	av_free(picture);
	//printf("\n");
	LOGI("example success!");

}


