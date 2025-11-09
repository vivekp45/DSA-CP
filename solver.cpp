#include <iostream>
#include <cmath>
#include <cstring>
#include <queue>
#include <limits>
using namespace std;

#define MAX 100

struct Stack { double arr[MAX]; int top; };
void initStack(Stack &s){ s.top=-1; }
bool isEmpty(Stack &s){ return s.top==-1; }
void push(Stack &s,double v){ if(s.top<MAX-1) s.arr[++s.top]=v; }
double pop(Stack &s){ return isEmpty(s)?0:s.arr[s.top--]; }
double peek(Stack &s){ return isEmpty(s)?0:s.arr[s.top]; }

struct Node { char expr[100]; double result; Node* next; };
Node* head=nullptr;
void addHistory(const char e[],double r){
    Node* t=new Node; strcpy(t->expr,e); t->result=r; t->next=nullptr;
    if(!head) head=t; else { Node* c=head; while(c->next)c=c->next; c->next=t; }
}
void showHistory(){
    Node* c=head; int i=1;
    while(c){ cout<<i++<<". "<<c->expr<<" = "<<c->result<<endl; c=c->next; }
}

struct Tree{ double d; Tree *l,*r; };
Tree* insert(Tree* r,double x){
    if(!r){ r=new Tree{x,nullptr,nullptr}; return r; }
    if(x<r->d) r->l=insert(r->l,x);
    else if(x>r->d) r->r=insert(r->r,x);
    return r;
}
void inorder(Tree* r){ if(!r) return; inorder(r->l); cout<<r->d<<" "; inorder(r->r); }

void bubbleSort(double a[],int n){
    for(int i=0;i<n-1;i++)
        for(int j=0;j<n-i-1;j++)
            if(a[j]>a[j+1]) swap(a[j],a[j+1]);
}
int linearSearch(double a[],int n,double key){
    for(int i=0;i<n;i++) if(fabs(a[i]-key)<1e-6) return i;
    return -1;
}

int prec(char op){ if(op=='+'||op=='-')return 1; if(op=='*'||op=='/')return 2; if(op=='^')return 3; return 0; }
bool isOp(char c){ return c=='+'||c=='-'||c=='*'||c=='/'||c=='^'; }

void infixToPostfix(char in[],char post[][20],int &n){
    Stack s; initStack(s);
    char tok[20]; int i=0,k;
    while(in[i]){
        if(isspace(in[i])){ i++; continue; }
        if(isdigit(in[i])||in[i]=='.'){
            k=0; while(isdigit(in[i])||in[i]=='.') tok[k++]=in[i++]; tok[k]='\0';
            strcpy(post[n++],tok);
        } else if(in[i]=='('){ push(s,(int)'('); i++; }
        else if(in[i]==')'){
            while(!isEmpty(s) && (char)peek(s)!='('){
                char op[2]={char(pop(s)),'\0'}; strcpy(post[n++],op);
            }
            if(!isEmpty(s)) pop(s);
            i++;
        } else if(isOp(in[i])){
            while(!isEmpty(s) && prec((char)peek(s))>=prec(in[i])){
                char op[2]={char(pop(s)),'\0'}; strcpy(post[n++],op);
            }
            push(s,(int)in[i]); i++;
        } else i++;
    }
    while(!isEmpty(s)){ char op[2]={char(pop(s)),'\0'}; strcpy(post[n++],op); }
}

double evalPostfix(char post[][20],int n){
    Stack s; initStack(s);
    for(int i=0;i<n;i++){
        if(isOp(post[i][0]) && strlen(post[i])==1){
            double b=pop(s),a=pop(s);
            switch(post[i][0]){
                case '+': push(s,a+b); break;
                case '-': push(s,a-b); break;
                case '*': push(s,a*b); break;
                case '/': push(s,a/b); break;
                case '^': push(s,pow(a,b)); break;
            }
        } else push(s,atof(post[i]));
    }
    return pop(s);
}

void expressionSolver(queue<double>&q,Tree*&r,double a[],int &idx){
    char in[200]; char post[100][20]; int n=0;
    cout<<"Enter expression: "; cin.ignore(numeric_limits<streamsize>::max(),'\n'); cin.getline(in,200);
    infixToPostfix(in,post,n);
    double res=evalPostfix(post,n);
    cout<<"Result = "<<res<<endl;
    q.push(res); a[idx++]=res; r=insert(r,res); addHistory(in,res);
}

void polynomialSolver(queue<double>&q,Tree*&r,double a[],int &idx){
    double A,B,C; cout<<"Enter a b c: "; cin>>A>>B>>C;
    double D=B*B-4*A*C;
    char e[60]; snprintf(e,sizeof(e),"%.2fx^2+%.2fx+%.2f",A,B,C);
    if(D>=0){
        double r1=(-B+sqrt(D))/(2*A),r2=(-B-sqrt(D))/(2*A);
        cout<<"Roots: "<<r1<<", "<<r2<<endl;
        q.push(r1); q.push(r2); a[idx++]=r1; a[idx++]=r2;
        r=insert(r,r1); r=insert(r,r2);
        addHistory(e,r1); addHistory(e,r2);
    } else {
        double re=-B/(2*A), im=sqrt(-D)/(2*A);
        cout<<"Complex: "<<re<<" ± "<<im<<"i\n";
    }
}

void equationSolver(queue<double>&q,Tree*&r,double a[],int &idx){
    double a1,b1,c1,a2,b2,c2;
    cout<<"Enter a1 b1 c1: ";cin>>a1>>b1>>c1;
    cout<<"Enter a2 b2 c2: ";cin>>a2>>b2>>c2;
    double D=a1*b2-a2*b1;
    if(fabs(D)<1e-12){ cout<<"No unique solution\n"; return; }
    double Dx=c1*b2-c2*b1, Dy=a1*c2-a2*c1;
    double x=Dx/D, y=Dy/D;
    cout<<"x="<<x<<", y="<<y<<endl;
    q.push(x); q.push(y); a[idx++]=x; a[idx++]=y;
    r=insert(r,x); r=insert(r,y);
    char e[80]; snprintf(e,sizeof(e),"%.1fx+%.1fy=%.1f, %.1fx+%.1fy=%.1f",a1,b1,c1,a2,b2,c2);
    addHistory(e,x);
}

void graphDemo(){
    int g[3][3]={{0,1,1},{1,0,1},{1,1,0}};
    cout<<"\nGraph Relation (Adjacency Matrix)\n  Expr Poly Eqn\n";
    for(int i=0;i<3;i++){
        cout<<(i==0?"Expr ":i==1?"Poly ":"Eqn  ");
        for(int j=0;j<3;j++) cout<<g[i][j]<<"  ";
        cout<<endl;
    }
}

int main(){
    queue<double> q; double arr[100]; int idx=0; Tree* root=nullptr; int ch;
    do{
        cout<<"\n=== ALGEBRAIC SOLVER ===\n";
        cout<<"1.Expr 2.Poly 3.Eqn 4.History 5.Search 6.Sort 7.BST 8.Graph 9.Exit\n";
        cout<<"Choice: "; cin>>ch;
        switch(ch){
            case 1: expressionSolver(q,root,arr,idx); break;
            case 2: polynomialSolver(q,root,arr,idx); break;
            case 3: equationSolver(q,root,arr,idx); break;
            case 4: showHistory(); break;
            case 5:{ double k; cout<<"Enter value: ";cin>>k;
                     int p=linearSearch(arr,idx,k);
                     cout<<(p!=-1?"Found":"Not found")<<endl; break;}
            case 6: bubbleSort(arr,idx); cout<<"Sorted: "; for(int i=0;i<idx;i++)cout<<arr[i]<<" "; cout<<endl; break;
            case 7: cout<<"BST in-order: "; inorder(root); cout<<endl; break;
            case 8: graphDemo(); break;
            case 9: cout<<"Exit\n"; break;
            default: cout<<"Invalid\n";
        }
    }while(ch!=9);
    return 0;
}

