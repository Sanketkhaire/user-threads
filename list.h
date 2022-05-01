//linked list node data structure for storing threads created.
typedef struct node{
    mythread *th;
    funcDesc *fD;
    struct node *next;
}node;


//actual linked list structure enclosing head and tail pointer.
struct linked_list{
    node *start;
    node *end;
    int count;
};

typedef struct linked_list thread_ll;

void add_thread_to_ll(thread_ll *list, mythread *t, funcDesc *f);
