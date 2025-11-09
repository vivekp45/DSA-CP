#include <iostream>
#include <cmath>
#include <cstring>
#include <queue>
#include <limits>
using namespace std;

#define MAX 100

// ==== Stack ====
struct Stack {
    double arr[MAX];
    int top;
};
void initStack(Stack &s){ s.top = -1; }
bool isEmpty(Stack &s){ return s.top == -1; }
void push(Stack &s,double v){ if(s.top < MAX-1) s.arr[++s.top] = v; }
double pop(Stack &s){ return isEmpty(s)?0:s.arr[s.top--]; }
double peek(Stack &s){ return isEmpty(s)?0:s.arr[s.top]; }

// ==== Linked List for History ====
struct Node {
    char expr[100];
    double result;
    Node* next;
};
Node* head = nullptr;

void addHistory(const char expr[], double res){
    Node* t = new Node;
    strcpy(t->expr, expr);
    t->result = res;
    t->next = nullptr;
    if(!head) head = t;
    else {
        Node* c = head;
        while(c->next) c = c->next;
        c->next = t;
    }
}

void showHistory(){
    Node* c = head;
    int i = 1;
    while(c){
        cout << i++ << ". " << c->expr << " = " << c->result << endl;
        c = c->next;
    }
}

// ==== BST ====
struct Tree {
    double data;
    Tree *left, *right;
};
Tree* insert(Tree* root, double x){
    if(!root){
        root = new Tree;
        root->data = x;
        root->left = root->right = nullptr;
        return root;
    }
    if(x < root->data) root->left = insert(root->left, x);
    else if(x > root->data) root->right = insert(root->right, x);
    return root;
}
void inorder(Tree* root){
    if(!root) return;
    inorder(root->left);
    cout << root->data << " ";
    inorder(root->right);
}

// ==== Sorting + Search ====
void bubbleSort(double a[], int n){
    for(int i=0;i<n-1;i++)
        for(int j=0;j<n-i-1;j++)
            if(a[j] > a[j+1])
                swap(a[j], a[j+1]);
}
int linearSearch(double a[], int n, double key){
    for(int i=0;i<n;i++)
        if(fabs(a[i]-key) < 1e-6)
            return i;
    return -1;
}

// ==== Infix to Postfix and Evaluation ====
int precedence(char op){
    if(op=='+'||op=='-') return 1;
    if(op=='*'||op=='/') return 2;
    if(op=='^') return 3;
    return 0;
}
bool isOperator(char c){
    return (c=='+'||c=='-'||c=='*'||c=='/'||c=='^');
}

void infixToPostfix(char infix[], char postfix[][20], int &n){
    Stack s; initStack(s);
    char token[20]; int i=0,k;
    while(infix[i]){
        if(isspace(infix[i])){ i++; continue; }
        if(isdigit(infix[i]) || infix[i]=='.'){
            k=0;
            while(isdigit(infix[i]) || infix[i]=='.')
                token[k++] = infix[i++];
            token[k]='\0';
            strcpy(postfix[n++], token);
        }
        else if(infix[i]=='('){
            push(s, (int)'('); i++;
        }
        else if(infix[i]==')'){
            while(!isEmpty(s) && (char)peek(s)!='('){
                char op[2] = {(char)pop(s), '\0'};
                strcpy(postfix[n++], op);
            }
            if(!isEmpty(s)) pop(s);
            i++;
        }
        else if(isOperator(infix[i])){
            while(!isEmpty(s) && precedence((char)peek(s)) >= precedence(infix[i])){
                char op[2] = {(char)pop(s), '\0'};
                strcpy(postfix[n++], op);
            }
            push(s, (int)infix[i]); i++;
        } else i++;
    }
    while(!isEmpty(s)){
        char op[2] = {(char)pop(s), '\0'};
        strcpy(postfix[n++], op);
    }
}

double evaluatePostfix(char postfix[][20], int n){
    Stack s; initStack(s);
    for(int i=0;i<n;i++){
        if(isOperator(postfix[i][0]) && strlen(postfix[i])==1){
            double b = pop(s), a = pop(s);
            switch(postfix[i][0]){
                case '+': push(s,a+b); break;
                case '-': push(s,a-b); break;
                case '*': push(s,a*b); break;
                case '/': push(s,a/b); break;
                case '^': push(s,pow(a,b)); break;
            }
        } else push(s, atof(postfix[i]));
    }
    return pop(s);
}

// ==== Expression Solver ====
void expressionSolver(queue<double> &q, Tree* &root, double arr[], int &idx){
    char infix[200];
    char postfix[100][20];
    int n = 0;

    cout << "\nEnter expression: ";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.getline(infix, 200);

    infixToPostfix(infix, postfix, n);
    double res = evaluatePostfix(postfix, n);
    cout << "Result = " << res << endl;

    q.push(res);
    arr[idx++] = res;
    root = insert(root, res);
    addHistory(infix, res);
}

// ==== Polynomial Solver ====
void polynomialSolver(queue<double> &q, Tree* &root, double arr[], int &idx){
    double a,b,c;
    cout << "\nEnter a b c: ";
    cin >> a >> b >> c;
    double D = b*b - 4*a*c;
    char e[60]; snprintf(e, sizeof(e), "%.2fx^2+%.2fx+%.2f", a, b, c);

    if(D > 0){
        double r1 = (-b + sqrt(D)) / (2*a);
        double r2 = (-b - sqrt(D)) / (2*a);
        cout << "Two real roots: " << r1 << ", " << r2 << endl;
        q.push(r1); q.push(r2);
        arr[idx++] = r1; arr[idx++] = r2;
        root = insert(root, r1);
        root = insert(root, r2);
        addHistory(e, r1);
        addHistory(e, r2);
    }
    else if(D == 0){
        double r = -b / (2*a);
        cout << "One real root: " << r << endl;
        q.push(r);
        arr[idx++] = r;
        root = insert(root, r);
        addHistory(e, r);
    }
    else {
        double re = -b / (2*a);
        double im = sqrt(-D) / (2*a);
        cout << "Complex roots: " << re << " ± " << im << "i" << endl;
    }
}

// ==== Equation Solver ====
void equationSolver(queue<double> &q, Tree* &root, double arr[], int &idx){
    double a1,b1,c1,a2,b2,c2;
    cout << "\nEnter a1 b1 c1: ";
    cin >> a1 >> b1 >> c1;
    cout << "Enter a2 b2 c2: ";
    cin >> a2 >> b2 >> c2;

    double D = a1*b2 - a2*b1;
    if(fabs(D) < 1e-12){
        cout << "No unique solution\n";
        return;
    }

    double Dx = c1*b2 - c2*b1;
    double Dy = a1*c2 - a2*c1;
    double x = Dx / D, y = Dy / D;

    cout << "x = " << x << ", y = " << y << endl;
    q.push(x); q.push(y);
    arr[idx++] = x; arr[idx++] = y;
    root = insert(root, x);
    root = insert(root, y);

    char e[80];
    snprintf(e, sizeof(e), "%.1fx+%.1fy=%.1f, %.1fx+%.1fy=%.1f", a1,b1,c1,a2,b2,c2);
    addHistory(e, x);
}

// ==== Graph Demo ====
void graphDemo(){
    int g[3][3] = {{0,1,1},{1,0,1},{1,1,0}};
    cout << "\nGraph Relation Between Solvers (Adjacency Matrix):\n";
    cout << "     Expr Poly Eqn\n";
    for(int i=0;i<3;i++){
        cout << (i==0?"Expr ":"Poly Eqn"[i*5]) << " ";
        for(int j=0;j<3;j++) cout << g[i][j] << "   ";
        cout << endl;
    }
}

// ==== Main ====
int main(){
    queue<double> q;
    double arr[100];
    int idx = 0;
    Tree* root = nullptr;
    int ch;

    do{
        cout << "\n====================\n";
        cout << " ALGEBRAIC SOLVER\n";
        cout << "====================\n";
        cout << "1. Expression Solver\n";
        cout << "2. Polynomial Solver\n";
        cout << "3. Equation Solver\n";
        cout << "4. Show History\n";
        cout << "5. Search Result\n";
        cout << "6. Sort Results\n";
        cout << "7. Show BST Order\n";
        cout << "8. Show Graph\n";
        cout << "9. Exit\n";
        cout << "Enter choice: ";
        cin >> ch;

        switch(ch){
            case 1: expressionSolver(q, root, arr, idx); break;
            case 2: polynomialSolver(q, root, arr, idx); break;
            case 3: equationSolver(q, root, arr, idx); break;
            case 4: showHistory(); break;
            case 5:{
                double key; cout << "Enter value to search: "; cin >> key;
                int pos = linearSearch(arr, idx, key);
                if(pos != -1) cout << "Found at index " << pos << endl;
                else cout << "Not found\n";
                break;
            }
            case 6:
                bubbleSort(arr, idx);
                cout << "Sorted results: ";
                for(int i=0;i<idx;i++) cout << arr[i] << " ";
                cout << endl;
                break;
            case 7:
                cout << "Inorder traversal of BST: ";
                inorder(root); cout << endl;
                break;
            case 8: graphDemo(); break;
            case 9: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice\n";
        }
    } while(ch != 9);

    return 0;
}
