#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SORT_BY_WORD	0 
#define SORT_BY_FREQ	1 

typedef struct {
	char	*word;		
	int		freq;		
} tWord;

typedef struct {
	int		len;		
	int		capacity;	
	tWord	*data;		
} tWordDic;

int compare_by_word(const void *n1, const void *n2);
int binary_search(const void *key, const void *base, size_t nmemb, size_t size,
                  int (*compare)(const void *, const void *), int *found);

void word_count( FILE *fp, tWordDic *dic){
	char *lines= malloc(sizeof(char)*1000);
	while (fgets(lines,1000,fp)!=NULL){
		char *wordd=strtok(lines," \n\t");
		while (wordd!=NULL){
			
			if (dic->capacity==dic->len){
				dic->capacity=dic->capacity+1000;
				dic->data=realloc(dic->data,sizeof(tWord)*dic->capacity);
			}
			if (dic->len==0){
				dic->data[0].word=strdup(wordd);
				dic->data[0].freq=1;
				dic->len++;
			}else if(dic->len>0){
				int found;
				int index = binary_search(&wordd,dic->data,dic->len,sizeof(tWord),compare_by_word,&found);
				if (found){
					dic->data[index].freq++;
				}else{
					if (index==dic->len){
						dic->data[dic->len].word=strdup(wordd);
						dic->data[dic->len].freq=1;
						dic->len++;
					}else{
						memmove(&dic->data[index+1],&dic->data[index],sizeof(tWord)*(dic->len-index));
						dic->data[index].word=strdup(wordd);
						dic->data[index].freq=1;
						dic->len++;
					}	
				}
			}
			wordd=strtok(NULL," \n\t");
		}
	}
	free(lines);
}


void print_dic( tWordDic *dic){
	for (int i=0;i<dic->len;i++){
		printf("%s\t%d\n",dic->data[i].word,dic->data[i].freq);
	}
}
void destroy_dic(tWordDic *dic){
	for (int i=0;i<dic->len;i++){
		free(dic->data[i].word);
	}
	free(dic->data);
	free(dic);
}

int compare_by_word( const void *n1, const void *n2){
	return strcmp(*(char**)n1,((tWord*)n2)->word);
}

int compare_by_freq( const void *n1, const void *n2){
	if ((((tWord*)n1)->freq)!=((tWord*)n2)->freq){
		return (((tWord*)n2)->freq-((tWord*)n1)->freq);
	}else{
		return compare_by_word(n1,n2);
	}
}

int binary_search( const void *key, const void *base, size_t nmemb, size_t size, 
	int (*compare)(const void *, const void *), int *found){
	int min=0, max=nmemb-1;
	*found=0;
	while(min<=max){
		int mid=(min+max)/2;
		int value=compare(key,(char*)base+(mid*size));
		if (value==0){
			*found=1;
			return mid;
		}else if (value>0){
			min=mid+1;
		}else if (value<0){
			max=mid-1;
		}
	}
	return min;
}
	
tWordDic *create_dic(void){
	tWordDic *dic = (tWordDic *)malloc( sizeof(tWordDic));
	
	dic->len = 0;
	dic->capacity = 1000;
	dic->data = (tWord *)malloc(dic->capacity * sizeof(tWord));

	return dic;
}

int main(int argc, char **argv){
	tWordDic *dic;
	int option;
	FILE *fp;
	
	if (argc != 3)
	{
		fprintf( stderr, "Usage: %s option FILE\n\n", argv[0]);
		fprintf( stderr, "option\n\t-w\t\tsort by word\n\t-f\t\tsort by frequency\n");
		return 1;
	}
	
	if (strcmp( argv[1], "-w") == 0) option = SORT_BY_WORD;
	else if (strcmp( argv[1], "-f") == 0) option = SORT_BY_FREQ;
	else {
		fprintf( stderr, "unknown option : %s\n", argv[1]);
		return 1;
	}
	
	dic = create_dic();

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 1;
	}

	word_count( fp, dic);

	fclose( fp);


	if (option == SORT_BY_FREQ) {
		qsort( dic->data, dic->len, sizeof(tWord), compare_by_freq);
	}
		
	print_dic( dic);

	destroy_dic( dic);
	
	return 0;
}

