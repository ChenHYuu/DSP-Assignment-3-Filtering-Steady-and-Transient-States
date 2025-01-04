CC = gcc
CFLAGS = -lm -lgsl

SINE_GEN = sine_wav_gen.exe
LINEAR_FILTER = Linear_Phase_Filter.exe
MINIMUM_FILTER = Minimum_Phase_Filter.exe

all: $(SINE_GEN) $(LINEAR_FILTER) $(MINIMUM_FILTER) run_sine run_linear run_minimum

$(SINE_GEN): sine_wav_gen.c
	$(CC) -o $@ $< -lm

$(LINEAR_FILTER): Linear_Phase_Filter.c
	$(CC) -o $@ $< $(CFLAGS)

$(MINIMUM_FILTER): Minimum_Phase_Filter.c
	$(CC) -o $@ $< $(CFLAGS)

run_sine: $(SINE_GEN)
	./$(SINE_GEN) 8000 1000 1.0 sincos_fs8000_f1000_L1.wav
	./$(SINE_GEN) 8000 3000 1.0 sincos_fs8000_f3000_L1.wav
	./$(SINE_GEN) 8000 4000 1.0 sincos_fs8000_f4000_L1.wav
	./$(SINE_GEN) 8000 5000 1.0 sincos_fs8000_f5000_L1.wav
	./$(SINE_GEN) 8000 8000 1.0 sincos_fs8000_f8000_L1.wav

run_linear: $(LINEAR_FILTER)

	./$(LINEAR_FILTER) 400 4 b_fn.txt sincos_fs8000_f1000_L1.wav sincos_fs8000_f1000_L1_out_fn4.wav
	./$(LINEAR_FILTER) 400 16 b_fn.txt sincos_fs8000_f1000_L1.wav sincos_fs8000_f1000_L1_out_fn16.wav
	./$(LINEAR_FILTER) 400 64 b_fn.txt sincos_fs8000_f1000_L1.wav sincos_fs8000_f1000_L1_out_fn64.wav
	./$(LINEAR_FILTER) 400 512 b_fn.txt sincos_fs8000_f1000_L1.wav sincos_fs8000_f1000_L1_out_fn512.wav
	./$(LINEAR_FILTER) 400 2048 b_fn.txt sincos_fs8000_f1000_L1.wav sincos_fs8000_f1000_L1_out_fn2048.wav

	./$(LINEAR_FILTER) 400 4 b_fn.txt sincos_fs8000_f3000_L1.wav sincos_fs8000_f3000_L1_out_fn4.wav
	./$(LINEAR_FILTER) 400 16 b_fn.txt sincos_fs8000_f3000_L1.wav sincos_fs8000_f3000_L1_out_fn16.wav
	./$(LINEAR_FILTER) 400 64 b_fn.txt sincos_fs8000_f3000_L1.wav sincos_fs8000_f3000_L1_out_fn64.wav
	./$(LINEAR_FILTER) 400 512 b_fn.txt sincos_fs8000_f3000_L1.wav sincos_fs8000_f3000_L1_out_fn512.wav
	./$(LINEAR_FILTER) 400 2048 b_fn.txt sincos_fs8000_f3000_L1.wav sincos_fs8000_f3000_L1_out_fn2048.wav

	./$(LINEAR_FILTER) 400 4 b_fn.txt sincos_fs8000_f4000_L1.wav sincos_fs8000_f4000_L1_out_fn4.wav
	./$(LINEAR_FILTER) 400 16 b_fn.txt sincos_fs8000_f4000_L1.wav sincos_fs8000_f4000_L1_out_fn16.wav
	./$(LINEAR_FILTER) 400 64 b_fn.txt sincos_fs8000_f4000_L1.wav sincos_fs8000_f4000_L1_out_fn64.wav
	./$(LINEAR_FILTER) 400 512 b_fn.txt sincos_fs8000_f4000_L1.wav sincos_fs8000_f4000_L1_out_fn512.wav
	./$(LINEAR_FILTER) 400 2048 b_fn.txt sincos_fs8000_f4000_L1.wav sincos_fs8000_f4000_L1_out_fn2048.wav

	./$(LINEAR_FILTER) 400 4 b_fn.txt sincos_fs8000_f5000_L1.wav sincos_fs8000_f5000_L1_out_fn4.wav
	./$(LINEAR_FILTER) 400 16 b_fn.txt sincos_fs8000_f5000_L1.wav sincos_fs8000_f5000_L1_out_fn16.wav
	./$(LINEAR_FILTER) 400 64 b_fn.txt sincos_fs8000_f5000_L1.wav sincos_fs8000_f5000_L1_out_fn64.wav
	./$(LINEAR_FILTER) 400 512 b_fn.txt sincos_fs8000_f5000_L1.wav sincos_fs8000_f5000_L1_out_fn512.wav
	./$(LINEAR_FILTER) 400 2048 b_fn.txt sincos_fs8000_f5000_L1.wav sincos_fs8000_f5000_L1_out_fn2048.wav

	./$(LINEAR_FILTER) 400 4 b_fn.txt sincos_fs8000_f8000_L1.wav sincos_fs8000_f8000_L1_out_fn4.wav
	./$(LINEAR_FILTER) 400 16 b_fn.txt sincos_fs8000_f8000_L1.wav sincos_fs8000_f8000_L1_out_fn16.wav
	./$(LINEAR_FILTER) 400 64 b_fn.txt sincos_fs8000_f8000_L1.wav sincos_fs8000_f8000_L1_out_fn64.wav
	./$(LINEAR_FILTER) 400 512 b_fn.txt sincos_fs8000_f8000_L1.wav sincos_fs8000_f8000_L1_out_fn512.wav
	./$(LINEAR_FILTER) 400 2048 b_fn.txt sincos_fs8000_f8000_L1.wav sincos_fs8000_f8000_L1_out_fn2048.wav

run_minimum: $(MINIMUM_FILTER)

	./$(MINIMUM_FILTER) 400 4 b_fn.txt sincos_fs8000_f1000_L1.wav sincos_fs8000_f1000_L1_min_out_fn4.wav
	./$(MINIMUM_FILTER) 400 16 b_fn.txt sincos_fs8000_f1000_L1.wav sincos_fs8000_f1000_L1_min_out_fn16.wav
	./$(MINIMUM_FILTER) 400 64 b_fn.txt sincos_fs8000_f1000_L1.wav sincos_fs8000_f1000_L1_min_out_fn64.wav

	./$(MINIMUM_FILTER) 400 4 b_fn.txt sincos_fs8000_f3000_L1.wav sincos_fs8000_f3000_L1_min_out_fn4.wav
	./$(MINIMUM_FILTER) 400 16 b_fn.txt sincos_fs8000_f3000_L1.wav sincos_fs8000_f3000_L1_min_out_fn16.wav
	./$(MINIMUM_FILTER) 400 64 b_fn.txt sincos_fs8000_f3000_L1.wav sincos_fs8000_f3000_L1_min_out_fn64.wav

	./$(MINIMUM_FILTER) 400 4 b_fn.txt sincos_fs8000_f4000_L1.wav sincos_fs8000_f4000_L1_min_out_fn4.wav
	./$(MINIMUM_FILTER) 400 16 b_fn.txt sincos_fs8000_f4000_L1.wav sincos_fs8000_f4000_L1_min_out_fn16.wav
	./$(MINIMUM_FILTER) 400 64 b_fn.txt sincos_fs8000_f4000_L1.wav sincos_fs8000_f4000_L1_min_out_fn64.wav

	./$(MINIMUM_FILTER) 400 4 b_fn.txt sincos_fs8000_f5000_L1.wav sincos_fs8000_f5000_L1_min_out_fn4.wav
	./$(MINIMUM_FILTER) 400 16 b_fn.txt sincos_fs8000_f5000_L1.wav sincos_fs8000_f5000_L1_min_out_fn16.wav
	./$(MINIMUM_FILTER) 400 64 b_fn.txt sincos_fs8000_f5000_L1.wav sincos_fs8000_f5000_L1_min_out_fn64.wav

	./$(MINIMUM_FILTER) 400 4 b_fn.txt sincos_fs8000_f8000_L1.wav sincos_fs8000_f8000_L1_min_out_fn4.wav
	./$(MINIMUM_FILTER) 400 16 b_fn.txt sincos_fs8000_f8000_L1.wav sincos_fs8000_f8000_L1_min_out_fn16.wav
	./$(MINIMUM_FILTER) 400 64 b_fn.txt sincos_fs8000_f8000_L1.wav sincos_fs8000_f8000_L1_min_out_fn64.wav

clean:
	rm -f $(SINE_GEN) $(LINEAR_FILTER) $(MINIMUM_FILTER) *.wav
