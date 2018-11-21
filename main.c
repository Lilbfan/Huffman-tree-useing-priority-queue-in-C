#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct node *nodePointer;
typedef struct node
{
 char character;
 int frequency;
 nodePointer leftChild;
 nodePointer rightChild;
}node;

void push( nodePointer* heap, const int n)
{
    // heap[n] is the thing you want to push into
    // If tmp > its parent, change it with his parent
    // EX. Chang B C A to A B C ( Pull A to the front )
    // Store A in the other side
    // Replace A with C, and than replace C with B ( B C A -> B B C )
    // Replace B with A ( B B C -> A B C )
    // and so is 4 elements, 5 elements, and more, if you want to pull one element forntward or backword.
    int i=n;
    nodePointer tmp = heap[n];
    while ( (i>1) && ( tmp->frequency < heap[i/2]->frequency) )
    {
        heap[i] = heap[i/2];
        i /= 2;
    }
    heap[i] = tmp;
}

nodePointer pop( nodePointer* heap, const int n)
{
    // Use the same method above
    int parent, child;
    nodePointer item, temp;
    item = heap[1];     // Save the key you want to pop out
    temp = heap[n];     // This is used to fill the hole made form poping out
    parent = 1;
    child = 2;
    while (child <= n)
    {
        // If the rightchild is smaller go right
        if ( (child < n) && ( heap[child]->frequency > heap[child+1]->frequency) )
            child++;
        // Move to the next lower level if frequency of current node smaller than tmp
        if ( temp->frequency <= heap[child]->frequency )
            break;
        heap[parent] = heap[child];
        parent = child;
        // Goto leftchild
        child *= 2;
    }
    // Fill the hole made form poping out
    heap[parent] = temp;
    return item;
}

int* encode( nodePointer current, int pre, int* top, char c )
{
    int* tmp;
    // If it is leaf and the data is what you want, return a int to let previous node know
    if( current->character==c )
    {
        tmp=malloc( sizeof(int) );
        *tmp=pre;
        (*top)=1;
        return tmp;
    }
    // If it is leaf but the data is not what you want, return NULL
    if( current->character!=-1 )
        return NULL;
    // Visite right child
    tmp=encode( current->leftChild, 0, top, c );
    // If current node is root, directly return what you receive
    if( pre==-1 )
    {
        // If going left can't find it than go right 
        if( tmp!=NULL )
            return tmp;
        return encode( current->rightChild, 1, top, c );
        // If none of each side can find it, return NULL
        // But if going right can't find it, it will return NULL, too
    }
    // If going left can't find it than go right 
    if( tmp==NULL )
    {
        // If going left can't find it, return NULL
        tmp=encode( current->rightChild, 1, top, c );
        if( tmp==NULL )
            return NULL;
    }
    // Add direction ( 0 or 1 ) to the code
    // Code length plus one
    ++(*top);
    tmp=realloc( tmp, *top );
    // Push code back to insert new one to the first one
    for( int i=(*top)-1; i>0; i-- )
        tmp[i]=tmp[i-1];
    tmp[0]=pre;
    //tmp[(*top)-1]=pre;
    return tmp;
}

int main()
{
    const int MAX_QUEUE_SIZE=1000;              // I don't know what this do, but teacher ask us to do
    int i=0, queueSize=0;                       // i is tmp int, and queueSize is length of queue
    char str[MAX_QUEUE_SIZE];                   // The string need to be encode
    nodePointer priorityQueue[MAX_QUEUE_SIZE];  // Actually it is not a priorty queue, it just a statistical array
    nodePointer heap[MAX_QUEUE_SIZE];           // This will be the priorty queue, and then become a huffman tree
    nodePointer entryOfHeap=NULL;             // As its name, root of huffman tree
    FILE *in, *out;                             // Input and output files

    if( ( in=fopen( "./test.txt", "r+" ))==NULL )
    {
        fprintf( stderr, "Input text open error.\n" );
        exit(0);
    }

    // Store context into string
    if( fgets( str, MAX_QUEUE_SIZE, in)==NULL )
    {
        fprintf( stderr, "Read file error.\n" );
        fclose(in);
        exit(1);
    }

    // Change '\n' into '\0'
    for( i=0; i<strlen(str); i++ )
        if( str[i]=='\n' )
        {
            str[i]='\0';
            break;
        }

    // Check chars in string one by one
    for( int j=0; j<strlen(str); j++ )
    {
        // Seem '\t' as space
        if( str[j]=='\t' )
            str[j]=' ';
        // If the char has been stored, add frequency
        for( i=1; i<=queueSize && i<MAX_QUEUE_SIZE ;i++ )
        {
            if( priorityQueue[i]->character==str[j] )
            {
                priorityQueue[i]->frequency++;
                break;
            }
        }
        // If the char hasn't been stored, create a new node and store it
        if( i==queueSize+1 )
        {
            priorityQueue[queueSize+1]=(nodePointer)malloc( sizeof(node) );
            priorityQueue[queueSize+1]->character=str[j];
            priorityQueue[queueSize+1]->frequency=1;
            priorityQueue[queueSize+1]->leftChild=NULL;
            priorityQueue[queueSize+1]->rightChild=NULL;
            queueSize++;
        }
    }

    // Close the input file
    fclose(in);

    // Copy array heap from array priorityQueue
    memcpy( &heap, &priorityQueue, sizeof(priorityQueue) );
    
    // Push it one by one to make it a priority queue
    for( i=1; i<=queueSize && i<MAX_QUEUE_SIZE ;i++ )
        push( heap, i );

    // Pop two node out and create a new node to be their parent, and push the parent back
    for( i=queueSize; i>1; i-- )
    {
        // Pop two node out
        nodePointer tmp1 = pop( heap, i );
        nodePointer tmp2 = pop( heap, i-1 );
        // create a new node to be their parent
        heap[i-1] = (nodePointer)malloc( sizeof(node) );
        // -1 to make a distinction between leaf and none leaf
        heap[i-1]->character = -1;
        // Its frequency is the summation of its children
        heap[i-1]->frequency = tmp1->frequency + tmp2->frequency;
        // The bigger one go right
        if( tmp1->frequency > tmp2->frequency )
        {
            heap[i-1]->leftChild=tmp2;
            heap[i-1]->rightChild=tmp1;
        }
        else
        {
            heap[i-1]->leftChild=tmp1;
            heap[i-1]->rightChild=tmp2;
        }
        // Push parent back
        push( heap, i-1 );
    }

    // At the end the priority queue will only left one node, and that is the root of huffman
    entryOfHeap = heap[1];

    if( ( out=fopen( "./code.txt", "w+" ))==NULL )
    {
        fprintf( stderr, "Output text open error.\n" );
        exit(2);
    }

    // Output how man leaf it has, what is stored in leaves ( include character ,frequency ), and codes of leaves
    fprintf( out, "%d\n", queueSize);
    for( i=1; i<=queueSize && i<MAX_QUEUE_SIZE ;i++ )
    {
        // Change output of ' ' into "space" 
        if( priorityQueue[i]->character==' ' )
        {
            fprintf( out, "space %d ", priorityQueue[i]->frequency );
            int top=0, *a=NULL;
            a = encode( entryOfHeap, -1, &top, priorityQueue[i]->character );
            if( a==NULL )
            {
                fprintf( stderr, "Encode error\n" );
                exit(3);
            }
            else
            {
                for( int j=0; j<top; j++ )
                    fprintf( out, "%d", a[j] );
                fprintf( out, "\n" );
            }
        }
        else
        {
            fprintf( out, "%c %d ", priorityQueue[i]->character, priorityQueue[i]->frequency );
            int top=0, *a=NULL;
            a = encode( entryOfHeap, -1, &top, priorityQueue[i]->character );
            if( a==NULL )
            {
                fprintf( stderr, "Encode error\n" );
                exit(3);
            }
            else
            {
                for( int j=0; j<top; j++ )
                    fprintf( out, "%d", a[j] );
                fprintf( out, "\n" );
            }
        }
    }

    // Encode the str, inputed string
    int wordCount=0, total[MAX_QUEUE_SIZE];
    for( i=0; i<strlen(str); i++ )
    {
        int top=0, *a=NULL;
        a = encode( entryOfHeap, -1, &top, str[i] );
        for( int j=0; j<top; j++ )
            total[wordCount+j]=a[j];
        wordCount+=top;
        free(a);
    }

    // Print how many digits in the encoded string
    fprintf( out, "%d\n", wordCount);
    
    // Print the encoded string
    for( i=0; i<wordCount; i++ )
        fprintf( out, "%d", total[i]);
    fprintf( out, "\n");

    fclose(out);
    return 0;
}
