#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <gsl/gsl_poly.h>
#include <gsl/gsl_fft_complex.h>
#include <string.h>

#define FS 48000.0f
#define PI 3.141592653589793f

typedef struct _wav {
    int fs;
    char header[44];
    size_t length;
    short *LChannel;
    short *RChannel;
} wav;

int wav_read_fn(char *fn, wav *p_wav) {
    short temp = 0;
    size_t i = 0;

    FILE *fp = fopen(fn, "rb");
    if (fp == NULL) {
        fprintf(stderr, "cannot read %s\n", fn);
        return 0;
    }
    fread(p_wav->header, sizeof(char), 44, fp);
    while (!feof(fp)) {
        fread(&temp, sizeof(short), 1, fp);
        i++;
    }
    p_wav->length = i / 2;
    p_wav->LChannel = (short *)calloc(p_wav->length, sizeof(short));
    if (p_wav->LChannel == NULL) {
        fprintf(stderr, "cannot allocate memory for LChannel in wav_read_fn\n");
        fclose(fp);
        return 0;
    }
    p_wav->RChannel = (short *)calloc(p_wav->length, sizeof(short));
    if (p_wav->RChannel == NULL) {
        fprintf(stderr, "cannot allocate memory for RChannel in wav_read_fn\n");
        fclose(fp);
        return 0;
    }
    fseek(fp, 44, SEEK_SET);
    for (i = 0; i < p_wav->length; i++) {
        fread(p_wav->LChannel + i, sizeof(short), 1, fp);
        fread(p_wav->RChannel + i, sizeof(short), 1, fp);
    }
    fclose(fp);
    return 1;
}

int wav_save_fn(char *fn, wav *p_wav) {
    FILE *fp = fopen(fn, "wb");
    size_t i;
    if (fp == NULL) {
        fprintf(stderr, "cannot save %s\n", fn);
        return 0;
    }
    fwrite(p_wav->header, sizeof(char), 44, fp);
    for (i = 0; i < p_wav->length; i++) {
        fwrite(p_wav->LChannel + i, sizeof(short), 1, fp);
        fwrite(p_wav->RChannel + i, sizeof(short), 1, fp);
    }
    fclose(fp);
    return 1;
}

int wav_init(size_t length, wav *p_wav) {
    p_wav->length = length;
    p_wav->LChannel = (short *)calloc(p_wav->length, sizeof(short));
    if (p_wav->LChannel == NULL) {
        fprintf(stderr, "cannot allocate memory for LChannel in wav_read_fn\n");
        return 0;
    }
    p_wav->RChannel = (short *)calloc(p_wav->length, sizeof(short));
    if (p_wav->RChannel == NULL) {
        fprintf(stderr, "cannot allocate memory for RChannel in wav_read_fn\n");
        return 0;
    }
    return 1;
}

void wav_free(wav *p_wav) {
    free(p_wav->LChannel);
    free(p_wav->RChannel);
}

float hamming(int N, int n) {
    return 0.54 - 0.46 * cosf(2 * PI * ((float)(n)) / ((float)N));
}

void generate_minimum_phase_lowpass_filter(int m, double fc, double *filter) {
    int N = 2 * m + 1;
    double complex *spectrum = (double complex *)calloc(N, sizeof(double complex));
    double complex *log_spectrum = (double complex *)calloc(N, sizeof(double complex));

    if (!spectrum || !log_spectrum) {
        fprintf(stderr, "Memory allocation failed in generate_minimum_phase_lowpass_filter\n");
        exit(1);
    }

    for (int n = 0; n < N; n++) {
        double freq = (double)n / N;
        if (freq < fc / FS) {
            spectrum[n] = 1.0 + 0.0 * I;
        } else {
            spectrum[n] = 0.0 + 0.0 * I;
        }
    }

    for (int n = 0; n < N; n++) {
        double magnitude = cabs(spectrum[n]);
        log_spectrum[n] = log(magnitude + 1e-10) + 0.0 * I;
    }

    gsl_fft_complex_wavetable *wavetable = gsl_fft_complex_wavetable_alloc(N);
    gsl_fft_complex_workspace *workspace = gsl_fft_complex_workspace_alloc(N);
    gsl_fft_complex_inverse((double *)log_spectrum, 1, N, wavetable, workspace);

    for (int n = 1; n < N / 2; n++) {
        log_spectrum[n] *= 2.0;
    }
    for (int n = N / 2; n < N; n++) {
        log_spectrum[n] = 0.0 + 0.0 * I;
    }

    gsl_fft_complex_forward((double *)log_spectrum, 1, N, wavetable, workspace);
    for (int n = 0; n < N; n++) {
        spectrum[n] = cexp(log_spectrum[n]);
    }

    gsl_fft_complex_inverse((double *)spectrum, 1, N, wavetable, workspace);
    for (int n = 0; n < N; n++) {
        filter[n] = creal(spectrum[n]);
    }

    gsl_fft_complex_wavetable_free(wavetable);
    gsl_fft_complex_workspace_free(workspace);
    free(spectrum);
    free(log_spectrum);
}

void apply_filter(const double *filter, int filter_len, const short *input, short *output, size_t length) {
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

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <cutoff frequency> <order> <coef file> <input file> <output file>\n", argv[0]);
        return 1;
    }

    int fc = atoi(argv[1]);
    int m = atoi(argv[2]);
    const char *b_fn = argv[3];
    const char *in_fn = argv[4];
    const char *out_fn = argv[5];

    wav wavin, wavout;
    double filter[2 * m + 1];

    if (wav_read_fn((char *)in_fn, &wavin) == 0) {
        fprintf(stderr, "Cannot read wav file %s\n", in_fn);
        exit(1);
    }

    generate_minimum_phase_lowpass_filter(m, fc, filter);


    if (wav_init(wavin.length, &wavout) == 0) {
        exit(1);
    }

    apply_filter(filter, 2 * m + 1, wavin.LChannel, wavout.LChannel, wavin.length);
    apply_filter(filter, 2 * m + 1, wavin.RChannel, wavout.RChannel, wavin.length);
    memcpy(wavout.header, wavin.header, 44);

    if (wav_save_fn((char *)out_fn, &wavout) == 0) {
        fprintf(stderr, "Cannot save %s\n", out_fn);
        exit(1);
    }

    wav_free(&wavin);
    wav_free(&wavout);

    return 0;
}