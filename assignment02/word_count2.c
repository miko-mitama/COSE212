#include <stdio.h>
#include <stdlib.h> // malloc
#include <string.h> // strdup, strcmp

#define SORT_BY_WORD	0 // 단어 순 정렬
#define SORT_BY_FREQ	1 // 빈도 순 정렬

// User structure type definition
// 단어 구조체
typedef struct {
	char	*word;		// 단어
	int		freq;		// 빈도
} tWord;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tWord		*dataPtr;
	struct node	*link; // 단어순 리스트를 위한 포인터
	struct node	*link2; // 빈도순 리스트를 위한 포인터
} NODE;

typedef struct
{
	int		count;
	NODE	*head; // 단어순 리스트의 첫번째 노드에 대한 포인터
	NODE	*head2; // 빈도순 리스트의 첫번째 노드에 대한 포인터
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations
int compare_by_freq( const void *n1, const void *n2);

// Allocates dynamic memory for a list head node and returns its address to caller
// return	head node pointer
// 			NULL if overflow
LIST *createList(void){
	LIST *list=(LIST*)malloc(sizeof(LIST));
	if (list==NULL) return NULL;
	list->count=0;
	list->head=NULL;
	list->head2=NULL;
	return list;

}
//  단어 리스트에 할당된 메모리를 해제 (head node, data node, word data)
void destroyList( LIST *pList){
	NODE *now=pList->head;
	NODE *temp;
	while (now!=NULL){
		temp=now;
		now=now->link;
		free(temp->dataPtr->word);
		free(temp->dataPtr);
		free(temp);
	}
	free(pList);
}
	

// internal search function
// searches list and passes back address of node containing target and its logical predecessor
// for addNode function
// return	1 found
// 			0 not found
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu){
	*pPre=NULL;
	*pLoc=pList->head;
	while (*pLoc != NULL && strcmp((*pLoc)->dataPtr->word, pArgu->word) < 0) {
		*pPre = *pLoc;
		*pLoc = (*pLoc)->link;
	}
	if (*pLoc != NULL && strcmp((*pLoc)->dataPtr->word, pArgu->word) == 0)
		return 1; 
	return 0;
}	
static int _search_by_freq( LIST *pList, NODE **pPre, NODE **pLoc, tWord *pArgu){
	*pPre=NULL;
	*pLoc=pList->head2;
	while (*pLoc != NULL && compare_by_freq(pArgu, (*pLoc)->dataPtr) > 0){
		*pPre = *pLoc;
		*pLoc = (*pLoc)->link2;
	}
	 return (*pLoc != NULL && compare_by_freq(pArgu, (*pLoc)->dataPtr) == 0);
}	

// internal insert function
// inserts data into a new node
// for addNode function
// return	1 if successful
// 			0 if memory overflow
static int _insert( LIST *pList, NODE *pPre, tWord *dataInPtr){
	NODE *new=(NODE*)malloc(sizeof(NODE));
	if (!new){
		return 0;
	}
	new->dataPtr=dataInPtr;
	new->link=NULL;
	new->link2=NULL;
	if (pPre==NULL){
		new->link=pList->head;
		pList->head=new;
	}else{
		new->link=pPre->link;
		pPre->link=new;
	}
	return 1;
}
// Inserts data into list
// return	0 if overflow
//			1 if successful
//			2 if duplicated key (이미 저장된 단어는 빈도 증가)
int addNode( LIST *pList, tWord *dataInPtr){
	NODE *pPre, *pLoc;
	int found=_search(pList,&pPre,&pLoc,dataInPtr);
	if(found==1){
		pLoc->dataPtr->freq++;
		return 2;
	}else{
		
		if (!_insert(pList,pPre,dataInPtr)){
			return 0;
		}else{
			pList->count++;
			return 1;
		}
	}
}
// internal function
// for connect_by_frequency function
// connects node into a frequency list
static void _link_by_freq( LIST *pList, NODE *pPre, NODE *pLoc){
	if (pPre==NULL){
		pLoc->link2=pList->head2;
		pList->head2=pLoc;
	}else{
		pLoc->link2=pPre->link2;
		pPre->link2=pLoc;
	}
}

// 단어순 리스트를 순회하며 빈도순 리스트로 연결
void connect_by_frequency( LIST *list){
	list->head2=NULL;
	NODE *pNow= list->head;
	while(pNow!=NULL){
		NODE *pPre2= NULL;
		NODE *pLoc2=NULL;
		_search_by_freq(list,&pPre2,&pLoc2,pNow->dataPtr);
		_link_by_freq(list,pPre2,pNow);
		pNow=pNow->link;
	}
}
		
	

// 사전을 화면에 출력 ("단어\t빈도" 형식)
void print_dic( LIST *pList){
	NODE *p=pList->head;
	while(p!=NULL){
		printf("%s\t%d\n",p->dataPtr->word,p->dataPtr->freq);
		p=p->link;
	}
}
void print_dic_by_freq( LIST *pList){
	NODE *q=pList->head2;
	while (q!=NULL){
		printf("%s\t%d\n",q->dataPtr->word,q->dataPtr->freq);
		q=q->link2;
	}
}

// 단어 구조체를 위한 메모리를 할당하고 word, freq 초기화
// for addNode function
// return	할당된 단어 구조체에 대한 pointer
//			NULL if overflow
tWord *createWord( char *word){
	tWord *wordd=(tWord*)malloc(sizeof(tWord));
	if (wordd==NULL){
		return NULL;
	}else{
		wordd->word=strdup(word);
		wordd->freq=1;
		return wordd;
	}
}

//  단어 구조체에 할당된 메모리를 해제
// for destroyList function
void destroyWord( tWord *pNode){
	free(pNode->word);
	free(pNode);
}

////////////////////////////////////////////////////////////////////////////////
// compares two words in word structures
// for _search function
// 정렬 기준 : 단어
int compare_by_word( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	return strcmp( p1->word, p2->word);
}
////////////////////////////////////////////////////////////////////////////////
// for _search_by_freq function
// 정렬 기준 : 빈도 내림차순(1순위), 단어(2순위)
int compare_by_freq( const void *n1, const void *n2)
{
	tWord *p1 = (tWord *)n1;
	tWord *p2 = (tWord *)n2;
	
	int ret = (int) p2->freq - p1->freq;
	
	if (ret != 0) return ret;
	
	return strcmp( p1->word, p2->word);
}


////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	int option;
	FILE *fp;
	char word[100];
	tWord *pWord;
	int ret;
	
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

	// creates an empty list
	list = createList();
	
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}

	if ((fp = fopen( argv[2], "r")) == NULL) 
	{
		fprintf( stderr, "cannot open file : %s\n", argv[2]);
		return 2;
	}
	
	while(fscanf( fp, "%s", word) != EOF)
	{
		pWord = createWord( word);
		
		// 사전(단어순 리스트) 업데이트
		// 이미 저장된 단어는 빈도 증가
		ret = addNode( list, pWord);
		
		if (ret == 0 || ret == 2) // failure or duplicated
		{
			destroyWord( pWord);
		}
	}
	
	fclose( fp);

	if (option == SORT_BY_WORD) {
		
		// 단어순 리스트를 화면에 출력
		print_dic( list);
	}
	else { // SORT_BY_FREQ
	
		// 빈도순 리스트 연결
		connect_by_frequency( list);
		
		// 빈도순 리스트를 화면에 출력
		print_dic_by_freq( list);
	}
	
	// 단어 리스트 메모리 해제
	destroyList( list);
	
	return 0;
}

