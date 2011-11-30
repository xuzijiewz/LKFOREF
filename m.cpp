#include"stdio.h"
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
/*
The program is to build enhancement function. If it need long time to comput, just click any key to stop, 
then it can run the program to continue the compute.
If it is a new compute, it need input follow three parameters:
  1. how many enhancement function.
  2. the weight of the weightest input.
  3. the type of enhancement function.(0 ro 1)

the format of output file:
1. weight of input,
2. the number of enhancement function,
3. the type of enhancement function,

(the enhancement functions is outputted one by one follow)
(first line) function codes
(second line) the minimum fixed words of different weight input    (if type 0, second line will be different with third line)
(third line)  the minimum weight output of different weight input
*/

struct node{    
    node *p;            //parent node
    node *fn;            //first chile node of a node
    node *ln;            //last chile node of a node
    node *lb;            //left node    
    node *rb;            //right node
    unsigned int lc;     //streng function code
    int id,pid, deep, maxhw, checked,maxdeep,streng_type,input_weight;
    int hw[16];   //min weight output with i weight input 
    int min_hw[16];   //the min weight, include input and output
};

node  * new_node(){
    node *n=new node();
    n->p=NULL;
    n->fn=NULL;
    n->ln=NULL;    
    n->lb=NULL;
    n->rb=NULL;
    n->maxhw=0;
    n->lc=0;
    n->deep=1;
    n->id=1;
    n->pid=-1;
    n->checked=0;    
    for(int i=0;i<16;i++){
        n->hw[i]=100;
        n->min_hw[i]=100;
    } //hw[i]=100 mean there is no i weight input
    return n;
}

unsigned int streng_fun(unsigned int test_num,unsigned int f){
    //use f change test_num
    unsigned int re=0x0;
    for(int i=0;i<16;i++)if(((f>>i)%2)>0) re=(re ^ ((test_num>>i) |(test_num<<(16-i)))) &0xffff;
    return re;    
}
int check(node *s){
    int minhw=1024;    
    node *p=s;
    int bhw, ihw,hw;
    unsigned int fv=0, hv;
    for(unsigned int itn=1;itn<0x10000;itn++){
    //itn is streng function's input
        int hw=0;
        for(int i1=0;i1<16;i1++) hw=hw+(itn>>i1)%2;
        if(hw<(s->input_weight) &&((s->streng_type==0)||((s->streng_type==1)&&(hw%2==0)))){
        //if itn hammng weight less than the target
            fv=0;
            //if streng function type is 0,then put itn to fv
            if(s->streng_type==0) fv=itn;
            p=s;
            ihw=0;
            
            do{
            //putet every streng function result to fv                
                hv=streng_fun(itn,p->lc);
                fv=fv | hv;
                for(int i=0;i<16;i++) ihw=ihw+((hv>>i)&0x1);
                p=p->p;   //move to parent node
            }while(p!=NULL);
            //check the result weight 
            bhw=0;
            for(int i=0;i<16;i++) bhw=bhw+((fv>>i)&0x1);
            
            if(ihw<s->min_hw[hw]) s->min_hw[hw]=ihw;
            //save th min weight
            if(bhw<(s->hw[hw])) s->hw[hw]=bhw;
            if(bhw<minhw) minhw=bhw;
        }
    }
    return minhw;
}
void write_nodes(char * fin,node *s){
    FILE *fn=fopen(fin,"w");
    //to the first node of top node
    while(s->p!=NULL) s=s->p;
    while(s->lb!=NULL) s=s->lb;    
    bool arrive_end=false;
    while(!arrive_end){
        fprintf(fn,"%x  %d  %d  %d  %d  %d\n", s->lc,s->deep,s->id,s->pid,s->maxhw, s->checked);
        fprintf(fn,"%d  %d  %d\n",s->maxdeep,s->input_weight,s->streng_type);        
        for(int i=0;i<16;i++) fprintf(fn,"%d  ", s->hw[i]);
        fprintf(fn,"\n");
        for(int i=0;i<16;i++) fprintf(fn,"%d  ", s->min_hw[i]);
        fprintf(fn,"\n");
        if(s->rb!=NULL) s=s->rb;
        else{
            //there is no more node in the current deep, find the node that has child node
            while(s->lb!=NULL && s->fn==NULL) s=s->lb;
            if(s->fn!=NULL){            //s has child node
                s=s->fn;
                while(s->lb!=NULL) s=s->lb; //to the first node
            }
            else{arrive_end=true; }//there is no node that has child node, it reach end.
        }
    }
    fclose(fn);
}

node * add_new_node(node * cn){
    node *n;
    n=cn;
    if(cn->pid>-1){
        //if cn is not top node, find the unchecked up-deep node 
        n=n->p;
        while(n->checked==1 && n->rb!=NULL) n=n->rb;
        //if all up-deep node had been checked, to the leftest node
        //else set the unchecked node to cn
        if(n->checked==1) while(cn->lb!=NULL) cn=cn->lb;
        else cn=n;
    }
    else{
        //cn is top node, goto the leftest node
        while(cn->lb!=NULL) cn=cn->lb;
    }

    //create a new node and set the new node as child node of cn
    n=new_node();
    cn->fn=n;
    cn->ln=n;
    n->input_weight=cn->input_weight;
    n->streng_type=cn->streng_type;
    n->maxdeep=cn->maxdeep;
    n->deep=cn->deep+1;
    n->p=cn;
    n->pid=cn->id;
    if(cn->lb!=NULL) cn=cn->lb;
    //fnd the node that has child node
    while(cn->lb!=NULL && cn->ln==NULL) cn=cn->lb;
    if(cn!=n->p){
        //cn is not the parent node of n
        cn=cn->ln;
        n->id=cn->id+1;
        n->lb=cn;
        cn->rb=n;
    }
    else{
        //n is the first node in current node
        n->id=1;
    }
    n->maxhw=cn->maxhw;
    return n;
}
int ihe(unsigned int ii, unsigned int i){
    //if the ii-th bit of i is 1, compute the height of ii.
    int hw=0;
    if(((i>>ii)%2)>0){for(int j=0;j<4;j++) hw=hw+(ii>>j)%2;}
    return hw;
}
void fun(node *s){
    int curdeep=s->deep; 
    int left_node_sfhw=256;     //the streng function code hamming weight 
    if(s->lb!=NULL){
        left_node_sfhw=0;
        for(int ii=0;ii<16;ii++) left_node_sfhw=left_node_sfhw+ihe(ii,s->lb->lc) ;
    }    
    for(unsigned int i=0x1;i<0x10000;i++){
        //set the streng function code, and check the code
        s->lc=i;        
        for(int ii=0;ii<16;ii++){ 
            s->hw[ii]=100; 
            s->min_hw[ii]=100;
        }        
        int re=check(s);//check the code
        int cur_node_sfhw=0;//current streng function code hamming weight
        for(unsigned int ii=0;ii<16;ii++) cur_node_sfhw=cur_node_sfhw+ihe(ii,i);
        if(re>s->maxhw || (re==s->maxhw && cur_node_sfhw<=left_node_sfhw)){
            //if current code is better
            //or same min weight and better streng function code hamming weight
            if((re>s->maxhw || (re==s->maxhw && cur_node_sfhw<left_node_sfhw))&& s->lb!=NULL){ 
                //if better code or better code hamming weight, delete the nodes in same deep.
                node *p=s->lb;
                while(p->lb!=NULL){
                    p=p->lb;
                    free(p->rb);
                }
                if(p!=NULL){
                    free(p);
                }
                if(s->p!=NULL){//s is not top node
                    p=s->p;
                    while(p!=NULL){
                        p->ln=NULL;
                        p->fn=NULL;
                        p=p->lb;
                    }
                    s->p->fn=s;
                    s->p->ln=s;
                }
                s->id=1;
                s->lb=NULL;
            }            
            left_node_sfhw=cur_node_sfhw;  
            s->maxhw=re;
            //pad a new node        
            s->rb=new_node();
            s->rb->lb=s;
            s->rb->deep=s->deep;
            s->rb->maxhw=re;
            s->rb->p=s->p;
            s->rb->id=s->id+1;
            s->rb->maxdeep=s->maxdeep;
            s->rb->input_weight=s->input_weight;
            s->rb->streng_type=s->streng_type;
            if(s->pid>-1) s->rb->pid=s->rb->p->id;
            if(s->p!=NULL) s->p->ln=s->rb;
            printf("xxx %x   %d  %d  %d  %d\n ",s->lc,re,s->id, s->deep,cur_node_sfhw);
            s=s->rb;            
        }
    }
    //mark the node had been checked
    if(s->p!=NULL) s->p->checked=1;
    printf("one node had been checked\n");
    //delete the last node
    if(s->lb!=NULL){
        s=s->lb;
        if(s->p!=NULL){
            if( s->p==s->rb->p){s->p->ln=s;}
            else{
                s->rb->p->fn=NULL;
                s->rb->p->ln=NULL;
            }
        }
        free(s->rb);
        s->rb=NULL;
    }
    write_nodes("out.txt",s);//save the tree to a file        
    if((s->maxdeep==1) || _kbhit()){ return;} //reach the preinstal deep or press key to stop
    else{
        if(s->deep==s->maxdeep){//if reach the preinstal deep, check weather there is unchecked node in up deep
            node *t=s->p;
            while(t->checked==1 && t->rb!=NULL) t=t->rb;
            if(t->checked==1)return;
        } 
    }
    fun(add_new_node(s)); //pad a new node
}
bool read_node(FILE *fn,node *n){     
    fscanf(fn,"%x  %d  %d  %d  %d  %d ", &n->lc,&n->deep,&n->id,&n->pid,&n->maxhw, &n->checked);
    fscanf(fn,"%d %d %d \n",&n->maxdeep,&n->input_weight,&n->streng_type);
    for(int i=0;i<16;i++) fscanf(fn,"%d", &n->hw[i]);
    for(int i=0;i<16;i++) fscanf(fn,"%d", &n->min_hw[i]);
    return !feof(fn);
}
node *read_node_tree(){ //input node from file, build the tree that had done.
    FILE *fn;
    fn=fopen("out.txt","r");
    node *cn=NULL;    //current node
    node *n=NULL;     //temp node
    if(fn==NULL){   //new compute
        n= new_node();   
        printf(" input deepth:");
        scanf("%d", &n->maxdeep); //input maxdeep
        printf("\n input the max input weight:");
        scanf("%d", &n->input_weight); //input the max input weight
        printf("\n input streng function type\n 0 for the function input had fix bit, 1 for the function input had not fix bit:");
        scanf("%d", &n->streng_type); //input streng function type
        
        return n;
    }
    cn=new_node();
    n=cn;
    while(read_node(fn,n)){    //read node        
        if(n->deep==cn->deep){  //same deep
            n->lb=cn;
            cn->rb=n;
            cn=n;
            if(cn->pid>-1){ //not top node, find the parent node
                n=cn->lb->p;
                while(n->id!=cn->pid) n=n->rb;
                if(n->fn==NULL)n->fn=cn;  //check first child node
                n->ln=cn;    //set last child node
                cn->p=n;
            }            
        }
        else{  //next deep
            while(cn->id!=n->pid) cn=cn->lb; //to the parent node of temp node
            cn->fn=n;
            cn->ln=n;
            n->p=cn;
            cn=n;            
        }
        n=new_node();
    }        

    fclose(fn);
    cn=add_new_node(cn);  //pad a new node to the tree
    return(cn); 
}
void write_result(node *s){
    while(s->deep!=s->maxdeep){
        while(s->rb!=NULL)s=s->rb;
        while(s->fn==NULL&&s->lb!=NULL) s=s->lb;
        s=s->fn;
    }
    while(s->lb!=NULL)s=s->lb;
    
    FILE *fn=fopen("result.txt","w");
    fprintf(fn,"input weight: %d\ndeep: %d\nstreng type:%d\n", s->input_weight,s->maxdeep,s->streng_type);
    do{
        node *n=s;
        fprintf(fn,"streng function code:  ");
        do{
            fprintf(fn,"%x  ", n->lc);
            if(n->p==NULL) n=NULL;
            else n=n->p;
        }while(n!=NULL);
        
        fprintf(fn,"\nthe munber of the word will be fixed: ");
        for(int i=0;i<16;i++){
            if(s->hw[i]>16) fprintf(fn,"0\t");//if hw[i]>16, there is not input that weight i.
            else  fprintf(fn,"%d\t",s->hw[i]);
        }
        fprintf(fn,"\nthe min weight: ");
        for(int i=0;i<16;i++) fprintf(fn,"%d\t",s->min_hw[i]);
        if(s->rb==NULL)s=NULL;
        else{
            s=s->rb;
            fprintf(fn,"\n\n");
        }
    }while(s!=NULL);    
    fclose(fn);
}
void main(){
    node *s=read_node_tree() ;      
    printf("begin ....\n");
    if(s!=NULL) fun(s);
    //output
    write_result(s); 
    printf("done");
    getchar();
    getchar();
}
