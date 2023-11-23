#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <regex.h>

typedef struct {
    int e;
    int i;
    int v;
    int c;
    int l;
    int n;
    int h;
    int s;
    int f;
    int o;
}options;

void save(int argc){
	char* temp[argc];
	int temp_count = 0;
	char* temp_fp[argc];
	int temp_fpcount = 0;
}

void input(int argc, char* argv[], char** files, int *count_files, char** temp, int temp_count, char** temp_fp, int temp_fpcount);
void option(int argc, char* argv[], options *opts, char** temp, int *temp_count, char** temp_fp, int *temp_fpcount);
char* pattern(char* line, char* expression, regmatch_t* pmatch, options opts);
char* pattern_file(char* line, char* filename, options* opts, char** temp, int temp_count, char** temp_fp, int temp_fpcount, int line_number, int* match_count, int files_count);
void file_handl(FILE* fp, options* opts, char** temp, int temp_count, char** temp_fp, int temp_fpcount, char* filename, int* match_count, int files_count);
void output(char* line, char* filename, int line_num, int files_count, options opts);

void option(int argc, char* argv[], options *opts, char** temp, int *temp_count, char** temp_fp, int *temp_fpcount){
	int opt_ind;
	static struct option long_op[] = {{0,0,0,0}};
	int opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_op, &opt_ind);
	for(; opt!=-1; opt = getopt_long(argc, argv, "e:ivclnhsf:o", long_op, &opt_ind)){
		switch(opt){
			case 'e':
			  opts->e = 1;
              temp[*temp_count] = optarg;
              *temp_count += 1;
			  break;
			case 'i':
			  opts->i = 1;
			  break;
			case 'v':
			  opts->v = 1;
			  break;
			case 'c':
			  opts->c = 1;
			  break;
			case 'l':
			  opts->l = 1;
			  break;
			case 'n':
			  opts->n = 1;
			  break;
			case 'h':
			  opts->h = 1;
			  break;
			case 's':
			  opts->s = 1;
			  break;
            case 'f':
			  opts->f = 1;
              temp_fp[*temp_fpcount] = optarg;
              *temp_fpcount += 1;
			  break;
            case 'o':
			  opts->o = 1;
			  break;
			default:
			  printf("error");
			  exit(-1);
			  break;
		}
	}
	for(int i = 0; i < *temp_fpcount; i++) {
		FILE* fp = fopen(temp_fp[i], "r");
		if(fp == NULL) {
			fprintf(stderr, "error");
			exit(-1);
		}
		else {
			fclose(fp);
		} 
	}
}

void input(int argc, char* argv[], char** files, int *count_files, char** temp, int temp_count, char** temp_fp, int temp_fpcount) {
	for(int i = 1; i < argc; i++) {
		int cheak = 0;
		if(argv[i][0] != '-') {
			for(int j  = 0; j < temp_count; j++){
				if(argv[i] == temp[j]){
					cheak = 1;
					break; 
				}
			}
			for(int j  = 0; j < temp_fpcount; j++){
				if(argv[i] == temp_fp[j]){
					cheak = 1;
					break; 
				}
			}
			if(cheak == 0){
				files[*count_files] = argv[i];
			    *count_files += 1;
			}
		}
	}
}

char* pattern(char* line, char* expression, regmatch_t* pmatch, options opts){
	char* result = NULL;
	if(line[0] != 0 && line[0] != '\n' || opts.f){
		regex_t reg_exp;
		int err = 0;
		char mess[64];
		int len = strlen(line);
		if(opts.i){
			err = regcomp(&reg_exp, expression, REG_EXTENDED | REG_ICASE);
		}
		else{
			err = regcomp(&reg_exp, expression, REG_EXTENDED);
		}
		if(err != 0){
			regerror(err, &reg_exp, mess, 64);
			printf("%s\n", mess);
			exit(-1);
		}
		if(err = regexec(&reg_exp, line, len, pmatch, 0) == 0) {
			result = line;
		} else if(err != REG_NOMATCH) {
			regerror(err, &reg_exp, mess, 64);
			exit(-1);
		}
		regfree(&reg_exp);
	}
	return result;
}

char* pattern_file(char* line, char* filename, options* opts, char** temp, int temp_count, char** temp_fp, int temp_fpcount, int line_number, int* match_count, int files_count) {
	char* res = NULL;
	char* tmp_file = NULL;
	size_t len;
	regmatch_t pmatch[strlen(line)];
	for(int i = 0; i < temp_count; i++){
		res = pattern(line, temp[i], pmatch, *opts);
		if((res != NULL && opts->o) || res != NULL && opts->v){
			break;
		}
	}
	if(res != NULL){
		for(int i = 0; i < temp_fpcount; i++){
			FILE* fp = fopen(temp_fp[i], "r");
			int c;
			while(c = getline(&tmp_file, &len, fp) != EOF){
				res = pattern(line, tmp_file, pmatch, *opts);
				if((res != NULL && opts->o) || res != NULL && opts->v){
					break;
				}
			}
			free(tmp_file);
			fclose(fp);
		}
	}
	return res;
}

void file_handl(FILE* fp, options* opts, char** temp, int temp_count, char** temp_fp, int temp_fpcount, char* filename, int* match_count, int files_count){
	size_t len;
	int line_num = 1;
	int endline;
	char* line_fp = NULL;
	while(endline = getline(&line_fp, &len, fp) != EOF) {
		int check = 0;
		char* match_part = pattern_file(line_fp, filename, opts, temp, temp_count, temp_fp, temp_fpcount, line_num, match_count, files_count);
		if(match_part == NULL){
			check = 0;
		}
		else{
			check = 1;
		}
		if((check == 1 && opts->v == 0) ||(check == 0 && opts->v == 1)){
			*match_count += 1;
			if(opts->c == 0 && opts->l == 0){
				output(line_fp, filename, line_num, files_count, *opts);
			}
		}
		line_num += 1; 
	}
	free(line_fp);
	if(opts->c == 1){
		if(opts->l == 1){
			if(*match_count > 0){
				*match_count = 1;
			}
			else{
				*match_count = 0;
			}
		}
		if(files_count = 1 || opts->h == 1){
			printf("%d\n", *match_count);
		}
		else{
			printf("%s:%d\n", filename, *match_count);
		} 
	}
	if(opts->l == 1){
		if(*match_count != 0){
			printf("%s\n", filename);
		}
	}
}

void output(char* line, char* filename, int line_num, int files_count, options opts) {
	if(files_count == 1 || opts.h == 1) {
		if(opts.n == 1) {
			printf("%d:%s", line_num, line);
		}
		else {
			printf("%s", line);
		}
	} else {
		if(opts.n == 1) {
			printf("%s:%d:%s", filename, line_num, line);
		}
		else {
			printf("%s:%s", filename, line);
		}
	}
	if(line[strlen(line) - 1] != '\n') printf("\n");
}

int main(int argc, char* argv[]) {
	options opts = {0};
	char *files[argc], *temp[argc], *temp_fp[argc];
	int count_fp = 0, temp_count = 0, temp_fpcount = 0, match_count = 0;
	option(argc, argv, &opts, temp, &temp_count, temp_fp, &temp_fpcount);
	input(argc, argv, files, &count_fp, temp, temp_count, temp_fp, temp_fpcount);
	//printf("%d", temp_count);
	//printf("%d", temp_fpcount);
	//printf("%d", count_fp);
	FILE* fp = NULL;
	for(int i = 0; i < count_fp; i++) {
		fp = fopen(files[i], "r");
		//printf("%s", files[i]);
		if(fp) {
			file_handl(fp, &opts, temp, temp_count, temp_fp, temp_fpcount, *files, &match_count, count_fp);
			fclose(fp);
			match_count = 0;
		}
		else {
			printf("error");
			exit(-1);
		}
	}
	return 0; 
}