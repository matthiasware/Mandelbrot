#ifndef IMGWRITE_H_
#define IMGWRITE_H_
	int write_png(char const *filename,
				  int w, int h, int comp,
				  const void *data,
		      	  int stride_in_bytes);
#endif