/*
Author : ssp3edy 
Date : 2019-June-06
*/

#include <iostream>
#include <string.h>
#include <malloc.h>
#include <fstream>
#include <string>
#include "compression.h"

//Internal and Leaf nodes
#define LEAF 0
#define INTERNAL 1

typedef struct node 
{
    char x;
    int freq;
    char* code;
    int type;
    struct node* next;
    struct node* left;
    struct node* right;

} node;

//Head and root nodes  in the linked list.
node *HEAD, *ROOT;


void printll();
void makeTree();
void genCode(node *p,char* code);
void insert(node *p,node *m);
void addSymbol(char c);
void writeHeader(FILE *f);
void writeBit(int b,FILE *f);
void writeCode(char ch,FILE *f);
char *getCode(char ch);

node* newNode(char c)
{
    node *temp;
    temp = new node;
    temp->x = c;
    temp->type = LEAF;
    temp->freq = 1;
    temp->next = NULL;
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

int main(int argc, char** argv)
{
    FILE *fp, *fp1;
    char ch;
    int t;
    HEAD = NULL;
    ROOT = NULL;

    if(argc <= 2)
    {
        printf("\n***Automated File Compression***\n");
        printf("\nCreating new compressed file...............\n");
        argv[2]=(char *)malloc(sizeof(char)*(strlen(argv[1])+strlen(compressed_extension)+1));
		strcpy(argv[2],argv[1]);
		strcat(argv[2],compressed_extension);
		argc++;
    }
    else 
        return 0;
    
    fp = fopen(argv[1],"rb");
    if(fp == NULL)
    {
        printf("Error, Input file does not exists, Check the file name\n");
        return -1;
    }

    printf("Initiating the compression sequence.................\n");
    printf("Reading input file %s\n",argv[1]);
    while(fread(&ch,sizeof(char),1,fp)!=0)
        addSymbol(ch);
    fclose(fp);

    printf("Constructing Huffman-Tree....................\n");
    makeTree();
    printf("Assigning codewords..........................\n");
    //Pre order traversal of the of the huffman code.
    genCode(ROOT,"\0");

    printf("Compressing the file.........................\n");
    fp=fopen(argv[1],"r");
    if(fp==NULL)
    {
        printf("\n[!]Input file cannot be opened.\n");
        return -1;
    }
    fp1=fopen(argv[2],"wb");
    if(fp1==NULL)
    {
        printf("\n[!]Output file cannot be opened.\n");
        return -2;
    }

    printf("\nReading input file %s.......................",argv[1]);
    printf("\nWriting file %s........................",argv[2]);
    printf("\nWriting File Header..............................");
        writeHeader(fp1);
    printf("\nWriting compressed content............................");
    //writing corersponding codes into the new file fp1
    while(fread(&ch,sizeof(char),1,fp)!=0)
        writeCode(ch,fp1);
    fclose(fp);
    fclose(fp1);

    printf("\n***Done***\n");
    return 0;

}


void writeHeader(FILE *f)
{
    //mapping of codes to actual words
    codeTable record;
    node *p;
    int temp =0, i=0;
    p = HEAD;
    //Determine the uniwue symbols and padding of bits
    while(p!=NULL)
    {
        temp+=(strlen(p->code)) * (p->freq);		//temp stores padding
        if(strlen(p->code) > MAX) printf("\n[!] Codewords are longer than usual.");	//TODO: Solve this case
        temp%=8;
        i++;
        p=p->next;
    }

    if(i==256)
        N=0;	//if 256 diff bit combinations exist, then alias 256 as 0
    else 
        N=i;
    
    fwrite(&N,sizeof(unsigned char),1,f);	//read these many structures while reading
    printf("\nN=%u",i);


    p=HEAD;
    //Start from the HEAD and wirte all character with its corresponding codes
    while(p!=NULL)	
    {
        record.x=p->x;
        strcpy(record.code,p->code);
        fwrite(&record,sizeof(codeTable),1,f);
        p=p->next;
    }

    padding=8-(char)temp;	//int to char & padding = 8-bitsExtra
    fwrite(&padding,sizeof(char),1,f);
    printf("\nPadding=%d",padding);
    //do actual padding
    for(i=0;i<padding;i++)
        writeBit(0,f);

}

void writeCode(char ch,FILE *f)
{
char *code;
//getting code of each character form the file
code=getCode(ch);
//printf("\n%s\n",code);
	while(*code!='\0')
	{
        //Write into file 1 if the code is one
		if(*code=='1')
			writeBit(1,f); //write bit 1 into file f
        //Write 0 bit in file the corrensponding code is 0
		else
			writeBit(0,f);
	code++;
	}
	return;
}

//Function to wite the bit value to the file 
void writeBit(int b,FILE *f)
{//My Logic: Maintain static buffer, if it is full, write into file 
	static char byte;
	static int cnt;
	char temp;
	//printf("\nSetting %dth bit = %d of %d ",cnt,b,byte);
	if(b==1)
	{	temp=1;
		temp=temp<<(7-cnt);		//right shift bits
		byte=byte | temp;
	}
	cnt++;
	
	if(cnt==8)	//buffer full
	{
//		printf("[%s]",bitsInChar(byte));
		fwrite(&byte,sizeof(char),1,f);
		cnt=0; byte=0;	//reset buffer
		return;// buffer written to file
	}
	return;
}

//getting correaponding code from the linked list, whose character is given
char *getCode(char ch)
{
node *p=HEAD;
	while(p!=NULL)
	{
        //LOGIC: check if the passed character matches with the linked list, if yes return its corresponding code
	    if(p->x==ch)
		  return p->code;
	    p=p->next;
	}
	return NULL; //not found
}

//Inserting a node according to its freq in the linked list
void insert(node *p,node *m)
{ // insert p in list as per its freq., start from m to right,
// we cant place node smaller than m since we dont have ptr to node left to m
if(m->next==NULL)
{  m->next=p; return;}
	while(m->next->freq < p->freq)
	{  m=m->next;
	  if(m->next==NULL)
	    { m->next=p; return; }
	}
  p->next=m->next;
  m->next=p;
}

//Adding the symbols to the linked list
void addSymbol(char c)
{// Insert symbols into linked list if its new, otherwise freq++
node *p,*q,*m;
int t;

if(HEAD==NULL)
{	HEAD=newNode(c);
	return;
}
	p=HEAD; q=NULL;
if(p->x==c) //item found in HEAD
{
	p->freq+=1;
	if(p->next==NULL)
		return;
	if(p->freq > p->next->freq)
	{
		HEAD=p->next;
		p->next=NULL;
		insert(p,HEAD);
	}
	return;
}

while(p->next!=NULL && p->x!=c)
{
	q=p; p=p->next;
}

if(p->x==c)
{
	p->freq+=1;
        if(p->next==NULL)
		return;	
	if(p->freq > p->next->freq)
	{
		m=p->next;
		q->next=p->next;
		p->next=NULL;
		insert(p,HEAD);
	}
}
else  //p->next==NULL , all list traversed c is not found, insert it at beginning
{	
	q=newNode(c);
	q->next=HEAD;  //first because freq is minimum
	HEAD=q;
}
}

//Generating huffman tree
void makeTree()
{
    node  *p,*q;
    p=HEAD;
	while(p!=NULL)
	{
		q=newNode('@');
		q->type=INTERNAL;	//internal node
		q->left=p;		//join left subtree/node
		q->freq=p->freq;
		if(p->next!=NULL)
		{
			p=p->next;
			q->right=p;	//join right subtree /node
			q->freq+=p->freq;
		}
		p=p->next;	//consider next node frm list
		if(p==NULL)	//list ends
			break;
		//insert new subtree rooted at q into list starting from p
		//if q smaller than p
		if(q->freq <= p->freq)
		{//place it before p
			q->next=p;
			p=q;
		}
		else
			insert(q,p);	//find appropriate position
	}//while
	ROOT=q; //q created at last iteration is ROOT of h-tree
}

//Genreating Huffman codes of the characters
void genCode(node *p,char* code)
{
char *lcode,*rcode;
static node *s;
static int flag;
if(p!=NULL)
{
//sort linked list as it was
	if(p->type==LEAF)   //leaf node
	{	if(flag==0) //first leaf node
		{flag=1; HEAD=p;}
		else	//other leaf nodes
		{ s->next=p;}		//sorting LL
		p->next=NULL;
		s=p;
	}

//assign code
	p->code=code;	//assign code to current node
//	printf("[%c|%d|%s|%d]",p->x,p->freq,p->code,p->type);
	lcode=(char *)malloc(strlen(code)+2);
	rcode=(char *)malloc(strlen(code)+2);
	sprintf(lcode,"%s0",code);
	sprintf(rcode,"%s1",code);
//recursive DFS
	genCode(p->left,lcode);		//left child has 0 appended to current node's code
	genCode(p->right,rcode);
}
}

