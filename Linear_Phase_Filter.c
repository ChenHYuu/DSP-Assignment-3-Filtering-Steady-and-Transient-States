/* Linear_Phase_Filter [Usage]: ./Linear_Phase_Filter.exe fc M b_fn.txt in_fn.wav out.fn.wav */
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <string.h>

#define FS 48000.0f
// #define FL 400.0f
// #define FH 4000.0f
// #define M 1024
#define PI 3.141592653589793f


typedef struct _wav {
	int fs;
	char header[44];
	size_t length;
	short *LChannel;
	short *RChannel;
} wav;

int wav_read_fn(char *fn, wav *p_wav)
{
	//char header[44];
	short temp = 0;
	size_t i = 0;

	FILE *fp = fopen(fn, "rb");
	if(fp==NULL) {
		fprintf(stderr, "cannot read %s\n", fn);
		return 0;
	}
	fread(p_wav->header, sizeof(char), 44, fp);
	while( !feof(fp) ) {
		fread(&temp, sizeof(short), 1, fp);
		i++;
	}
	p_wav->length = i / 2;
	p_wav->LChannel = (short *) calloc(p_wav->length, sizeof(short));
	if( p_wav->LChannel==NULL ) {
		fprintf(stderr, "cannot allocate memory for LChannel in wav_read_fn\n");
		fclose(fp);
		return 0;
	}
	p_wav->RChannel = (short *) calloc(p_wav->length, sizeof(short));
	if( p_wav->RChannel==NULL ) {
		fprintf(stderr, "cannot allocate memory for RChannel in wav_read_fn\n");
		fclose(fp);
		return 0;
	}
	fseek(fp, 44, SEEK_SET);
	for(i=0;i<p_wav->length;i++) {
		fread(p_wav->LChannel+i, sizeof(short), 1, fp);
		fread(p_wav->RChannel+i, sizeof(short), 1, fp);
	}
	fclose(fp);
	return 1;
}

int wav_save_fn(char *fn, wav *p_wav)
{
	FILE *fp = fopen(fn, "wb");
	size_t i;
	if(fp==NULL) {
		fprintf(stderr, "cannot save %s\n", fn);
		return 0;
	}
	fwrite(p_wav->header, sizeof(char), 44, fp);
	for(i=0;i<p_wav->length;i++) {
		fwrite(p_wav->LChannel+i, sizeof(short), 1, fp);
		fwrite(p_wav->RChannel+i, sizeof(short), 1, fp);
	}
	fclose(fp);
	return 1;
}

int wav_init(size_t length, wav *p_wav)
{
	p_wav->length = length;
	p_wav->LChannel = (short *) calloc(p_wav->length, sizeof(short));
	if( p_wav->LChannel==NULL ) {
		fprintf(stderr, "cannot allocate memory for LChannel in wav_read_fn\n");
		return 0;
	}
	p_wav->RChannel = (short *) calloc(p_wav->length, sizeof(short));
	if( p_wav->RChannel==NULL ) {
		fprintf(stderr, "cannot allocate memory for RChannel in wav_read_fn\n");
		return 0;
	}
	return 1;
}

void wav_free(wav *p_wav)
{
	free(p_wav->LChannel);
	free(p_wav->RChannel);
}

/* hamming: for n=0,1,2,...N, length of N+1 */
float hamming(int N, int n)
{
	return 0.54 - 0.46 * cosf(2*PI*((float)(n))/((float)N));
}

/* low-pass filter coef: n=0,1,2...,2M */
float low_pass(int m, int n, int fc)
{
	float wc = 2*PI*fc/FS;
	if(n==m) {// L'Hopital's Rule
		return wc/PI;
	}
	else {
		return sinf(wc*((float)(n-m)))/PI/((float)(n-m)) * hamming(2*m+1, n);
	}
}

// float band_pass(int m, int n, int fc)
// {
// 	float wh = 2*PI*FH/FS;
//     float wl = 2*PI*fc/FS;
// 	if(n==m) {// L'Hopital's Rule
// 		return 2.0*(wh/PI - wl/PI);
// 	}
// 	else {
// 		return 2.0*(sinf(wh*((float)(n-m)))-sinf(wl*((float)(n-m))))/PI/((float)(n-m)) * hamming(2*m+1, n);
// 	}
// }
void apply_filter(const float *filter, int filter_len, const short *input, short *output, size_t length) {
    #pragma omp parallel for
    for (size_t n = 0; n < length; n++) {
        float y = 0.0f;
        for (int k = 0; k < filter_len; k++) {
            if (n >= k) {
                y += filter[k] * input[n - k];
            }
        }
        output[n] = (short)(roundf(y));
    }
}

int main(int argc, char **argv)
{
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <cutoff frequency> <order> <poles/zeros> <in_fn.wav> <out_fn.wav>\n", argv[0]);
        return 1;
    }
    int fc = atoi(argv[1]);
    int m = atoi(argv[2]);
    const char * b_fn = argv[3];
    const char * in_fn = argv[4];
    const char * out_fn = argv[5];

    wav wavin;
	wav wavout;
	// char fn_in[1024] = {"Ascience-Fast-Piano-Add-Tones.wav"};
	// char fn_out[1024] = {"out.wav"};
	float h_L[2*m+1];
    float h_R[2*m+1];
	int n = 0;
	float y = 0;
	int k;

	// read wav
	if( wav_read_fn((char *)in_fn, &wavin) == 0 ) {
		fprintf(stderr, "cannot read wav file %s\n", in_fn);
		exit(1);
	}


	// construct low-pass filter
	for(n=0;n<(2*m+1);n++) {
		h_L[n] = low_pass(m, n, fc);
        h_R[n] = low_pass(m, n, fc);
	}

    /*
	for(n=0;n<(2*M+1);n++) {
		fprintf(stdout, "%.15f\n", h[n]);
	}
    */

	// filtering (convolution)
	if( wav_init(wavin.length, &wavout)==0 ) {
		exit(1);
	}

	apply_filter(h_L, 2 * m + 1, wavin.LChannel, wavout.LChannel, wavin.length);
    apply_filter(h_R, 2 * m + 1, wavin.RChannel, wavout.RChannel, wavin.length);
	memcpy(wavout.header, wavin.header, 44);


	// save wav
	if( wav_save_fn((char *)out_fn, &wavout)==0) {
		fprintf(stderr, "cannot save %s\n", out_fn);
		exit(1);

	}
	wav_free(&wavin);
	wav_free(&wavout);
}