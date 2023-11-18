#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

typedef struct {
	int b;
	int e;
	int n;
	int s;
	int v;
	int t;
}options;

void option(int argc, char* argv[], options *opts);
void output(options opts, FILE* fp, int* number, char tmp[]);
void input(int argc, char* argv[], char** files, int *count_files);

void option(int argc, char* argv[], options *opts){
	int opt_ind;
	static struct option long_op[] = {{"number-nonblank", 0, 0, 'b'},
	                                  {0,0,0,0}};
	int opt = getopt_long(argc, argv, "+benvstET", long_op, &opt_ind);
	for(; opt!=-1; opt = getopt_long(argc, argv, "+benvstET", long_op, &opt_ind)){
		switch(opt){
			case 'b':
			  opts->b = 1;
			  break;
			case 'e':
			  opts->e = 1;
			  opts->v = 1;
			  break;
			case 'n':
			  opts->n = 1;
			  break;
			case 'v':
			  opts->v = 1;
			  break;
			case 's':
			  opts->s = 1;
			  break;
			case 't':
			  opts->t = 1;
			  opts->v = 1;
			  break;
			case 'T':
			  opts->t = 1;
			  break;
			case 'E':
			  opts->e = 1;
			  break;
			default:
			  printf("err");
			  exit(-1);
			  break;
		}
	}
}

void input(int argc, char* argv[], char** files, int *count_files) {
	for(int i = 1; i < argc; i++) {
		if(argv[i][0] != '-') {
			files[*count_files] = argv[i];
			*count_files += 1;
		}
	}
}

void output(options opts, FILE* fp, int* number, char tmp[]) {
	int count = 0;
	int count_s = 0;
	char r;
	while ((r=getc(fp))!= EOF) {
		tmp[0] = tmp[1];
		tmp[1] = r;
		if(opts.s){
			if(tmp[0] == 0 && tmp[1] == '\n' && count == 0)
			    count_s = 1;
			else if(tmp[0] == '\n' && tmp[1] == '\n' && count_s == 0)
			    count_s = 1;
		    else if(tmp[1] != '\n')
		        count_s = 0;
		    else if(tmp[0] == '\n' && tmp[1] == '\n' && count_s == 1)
			    continue;
		}
		if(opts.b) {
			if((count == 0 && *number == 1 && r != '\n') || (r != '\n' && tmp[0] == '\n')) {
			    printf("%6i\t", *number);
			    *number += 1;
			}
		} else if(opts.n){
			if((count == 0 && *number == 1) || (tmp[0] == '\n')) {
			    printf("%6i\t", *number);
			    *number += 1;
			}
		}
		if(opts.v){
			if(r >= 0 && r < 32 && r!= 9 && r != 10 || r == 127) {
				if(r == 127) {
					printf("^?");
					continue;
				}
				else {
					printf("^%c", (r+64));
					continue;
				}	
			}
		}
		if(opts.e){
			if(r == '\n') printf("$");
		}
		if(opts.t){
			if(r == '\t') {
				printf("^|");
				continue;
			}
		}
		printf("%c", r);
	}
}

int main(int argc, char* argv[]) {
	FILE* fp;
	int number = 1;
	char tmp[2] = {0, 0};
	options optt = {0};
	int count_fp = 0;
	char* files[argc];
	option(argc, argv, &optt);
	input(argc, argv, files, &count_fp);
	for(int i = 0;  i < count_fp; i++) {
		fp = fopen(files[i], "r");
		if(fp == NULL)
		    printf("error");
		else {
		    output(optt, fp, &number, tmp);
			fclose(fp);
		}
	}
	return 0;
}