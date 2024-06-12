CC = nvc++
CFLAGS = -fopenmp -mp=gpu

all: mmgpu

mmgpu: magic_matrix_gpu.cpp
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f mmgpu
