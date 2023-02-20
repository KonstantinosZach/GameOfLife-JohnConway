#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADTMap.h>
#include <ADTVector.h>
#include <ADTList.h>
// Το LifeState το χειρίζομαι ακριβώς όπως μια map δομή
// για αυτό και το έχω κάνει typedef όπως έχει γίνει και το MAP
typedef struct map* LifeState;
typedef struct {
	int x, y;
} LifeCell;
//creates a struct allocating new memory
//for the key of the map
LifeCell* create_struct (LifeCell* cell){
    LifeCell* cell_new=malloc(sizeof(LifeCell));
    cell_new->x=cell->x;
    cell_new->y=cell->y;
    return cell_new;
}
//creates a int allocating new memory
//for the value of the map
int* create_int(int value){
    int* number=malloc(sizeof(int));
    if(number==NULL){
        printf("memory not allocated\n");
        exit(0);
    }
    *number=value;
    return number;
}
// κάνει compare τα κλειδιά του map(δηλαδη ενός state)
// είναι Strict weak ordering
int compare_structs(Pointer a,Pointer b){
    LifeCell* cell_a=a;
    LifeCell* cell_b=b;
    // το συγκεκριμένο if είναι από το post του Χατζηκοκολακη @216
    // άμα το δεύτερο struct έχει μεγαλύτερα cordinates (χ ή y)
    if((cell_a->x < cell_b->x) || (cell_a->x == cell_b->x && cell_a->y < cell_b->y)){
        return -1;
    }
    else if((cell_a->x)-(cell_b->x)==0){
        // αν είναι και το δύο ίδια
        if((cell_a->y)-(cell_b->y)==0){
            return 0;
        }
        // αν το πρώτο έχει μεγαλύτερο y
        return 1;
    }
    else{
        // αν το πρώτο έχει μεγαλύτερο χ
        return 1;
    }
}

// this function makes an empty state(map) because it represents a state filled with empty cells
// στο map εμφανίζονται μόνο τα alive cells
LifeState life_create(){
    LifeState state=map_create(compare_structs,free,free);
    return state;

}

// functions that creates a state from a file
LifeState life_create_from_rle(char* file){
    FILE* ptr = fopen(file,"r");
    if(ptr==NULL){
        printf("The file you assigned does not exist\n");
		exit(0);
    }
    Vector input=vector_create(0,free); 
    char* buf=malloc(10*sizeof(char));
    int extra=0,number=0;
    //we save the input to a vector for better access
    // στο vector η πληροφορία αποθήκευται χωρίς αριθμούς
    // δηλαδη το 3ο! γίνεται οοο!
    while(fscanf(ptr,"%c",buf)==1){
        // αμα συναντησουμε αλλαγη γραμμης απλα την αγνοουμε 
        // και πηγαινουμε στην αρχη της λουπας
        if(strcmp("\n",buf)==0){
            continue;
        }
        // εδώ θα μπει άμα έχουμε συναντήσει αριθμό
        if(extra==1){
            // άμα εχουμε μετά και αλλο αριθμο ενημερώνουμε το number
            // δηλαδη το 32o! --> θα διαβασει πρωτα το 3 και μετα το 2 (3-->3*10+2-->32)
            if((strcmp("o",buf)!=0)&&(strcmp("b",buf)!=0)&&(strcmp("$",buf)!=0)&&(strcmp("!",buf)!=0)){
                number=number*10+atoi(buf);
            }
            else{
                //οσο είναι το number τοσους χαρακτηρες βαζουμε
                for(int i=0;i<number;i++){
                    vector_insert_last(input,strdup(buf));
                    extra=0;
                }
            }
        }
        else{
            // αμα συναντησουμε αριθμό
            if((strcmp("o",buf)!=0)&&(strcmp("b",buf)!=0)&&(strcmp("$",buf)!=0)&&(strcmp("!",buf)!=0)){
                number=atoi(buf);
                // ενημερόνουμε το extra για να ξερουμε οτι βρηκαμε αριθμό
                extra=1;
            }
            else{
                // αμα συναντησουμε ο ή b απλα το βαζουμε στο vector στο τέλος
                vector_insert_last(input,strdup(buf));
            }
        }
    }
    LifeState state=map_create(compare_structs,free,free);
    int size=vector_size(input);
    VectorNode node=vector_first(input);
    // τα lines και rows ειναι τα cordinates των cells
    int lines=0,rows=0;
    for(int i=0;i<size;i++){
        char* value=vector_node_value(input,node);
        if((strcmp(value,"$")!=0)&&(strcmp(value,"!")!=0)){
            // αμα συναντησουμε ο το προσθετουμε στο LifeState
            // και ενημερώνουμε το rows (+1) είτε βρουμε ο είτε b
            // απλα το b δεν το βαζουμε στο LifeState
            if(strcmp(value,"o")==0){
                LifeCell* cell=malloc(sizeof(LifeCell));
                cell->x=lines;
                cell->y=rows;
                map_insert(state,create_struct(cell),create_int(1));
                rows++;
                free(cell);
            }
            else{
                rows++;
            }
        }
        // αμα συναντήσουμε $(δεν μας νοιαζει τοσο το !)
        // απλα αυξουμε το lines και μηδενιζουμε το rows
        else{
                lines++;
                rows=0;
        }    
        node=vector_next(input,node);
    }
    // αμα θέλετε να δείτε το input πως έχει αποθηκευτεί καντε uncomment την κατω εντολή
    // απλα η υλοποιηση της βρισκεται στο τελος του αρχείου οποτε να την κάνετε copy στην αρχή
    // για να μην βγαλει error
    //print_input(input);
    vector_destroy(input);
    free(buf);
    return state;

}

// function that copies the state to a file
// input x=0,y=1/x=1,y=0 output bo$o!
void life_save_to_rle(LifeState state, char* file){
    FILE* ptr = fopen(file,"a+");
    if(ptr==NULL){
        printf("The file you assigned does not exist\n");
		exit(0);
    }
//put new line to be readable if the file has already some context
//  fputs("\n",ptr);
// θελουμε να παρουμε τις μικροτερες συντενταγμενες για να ξερουμε απο που θα ξεκινήσουμε
// το μικροτερο χ ειναι το χ του πρωτου node
    LifeCell* key_lines=map_node_key(state,map_first(state));
    int lines=key_lines->x;
// το μικροτερο y πρεπει να το ψαξουμε
    int min_rows=0;
    for(MapNode node=map_first(state);
    node!=MAP_EOF;
    node=map_next(state,node)){
        LifeCell* key_rows=map_node_key(state,node);
        if(min_rows>key_rows->y){
            min_rows=key_rows->y;
        }
    }
    int rows=min_rows;
    int next_node=0;
    for(MapNode node=map_first(state);
        node!=MAP_EOF;
        node=map_next(state,node)){
        
        LifeCell* key=map_node_key(state,node);
        // ουσιαστικα αυτη η επαναληψη γινεται μεχρι να βρουμε το πρωτο (ο)
        // δηλαδη οταν το lines==key->x και rows==key->y
        // και επειδη δεν γνωριζουμε τι υπαρχει πριν απο αυτο(δηλαδη πόσα b) αρχιζουμε να ψαχνουμε απο το min_rows
        while(next_node==0){
            // αμα βρισκομαστε στην ιδια γραμμη με αυτη που ξεκινήσαμε την επαναληψη
            if(lines==key->x){
                // αμα βρουμε στην τιμη rows καποιο κλειδι τοτε βαζουμε ο
                if(rows==key->y){
                    fputs("o",ptr);
                    // βρηκαμε το ο
                    // παμε στο επομενο node
                    next_node=1;
                    rows++;
                }
                // αλλιως b και αυξανουμε το rows
                else{
                    fputs("b",ptr);
                    rows++;
                }
            }
            // αλλιως αμα πχ σε εμας το lines ειναι 0 και το κλειδι ειναι (χ=1,y=5)
            // πρεπει να αλλαξουμε γραμμη και να βαλουμε $
            else{
                lines++;
                rows=min_rows;
                fputs("$",ptr);
            }
        }
        next_node=0;
    }
    fputs("!",ptr);
}

//function that returns the state of a specific cell(true=1,false=0)
bool life_get_cell(LifeState state, LifeCell cell){
    LifeCell* cell_to_find=malloc(sizeof(LifeCell));
    cell_to_find->x=cell.x;
    cell_to_find->y=cell.y;
    MapNode node=map_find_node(state,cell_to_find);
    if(node==MAP_EOF){
        free(cell_to_find);
        return false;
    }
    else{
        int* value=map_node_value(state,node);
        // εδω απλα καλυπτουμε και την  περιπτωση 
        // που για καποιο λογο εχει περισσεψει value==0
        if(*value==1){
            free(cell_to_find);
            return true;
        }
        else{
            free(cell_to_find);
            return false;
        }
    }
    
}
// function that set the state of a specific cell according to the value
void life_set_cell(LifeState state, LifeCell cell, bool value){
    LifeCell* cell_to_change=malloc(sizeof(LifeCell));
    cell_to_change->x=cell.x;
    cell_to_change->y=cell.y;
    if(value){
        map_insert(state,create_struct(cell_to_change),create_int(1));
    }
    else{
        map_insert(state,create_struct(cell_to_change),create_int(0));   
    }
}

//generates the next state according to the current state
LifeState life_evolve(LifeState state){
    LifeState new_state=life_create();
    // βρισκουμε τις ελαχιστες και μεγιστες συντεταγμενες
    // με αυτες γνωριζουμε ποια cells πρεπει να εξετάσουμε
    int max_lines=0,max_rows=0;
    int min_lines=0,min_rows=0;
    for(MapNode node=map_first(state);
    node!=MAP_EOF;
    node=map_next(state,node)){
        LifeCell* cordinates=map_node_key(state,node);
        if(max_lines<cordinates->x){
            max_lines=cordinates->x;
        }
        if(max_rows<cordinates->y){
            max_rows=cordinates->y;
        }
        if(min_lines>cordinates->x){
            min_lines=cordinates->x;
        }
        if(min_rows>cordinates->y){
            min_rows=cordinates->y;
        }
    }
    MapNode node;
    LifeCell* cordinates=malloc(sizeof(LifeCell));
    for(int lines=min_lines-1;lines<=max_lines+1;lines++){
        for(int rows=min_rows-1;rows<=max_rows+1;rows++){
            int counter=0;
            LifeCell neighbor;
            // στην αρχη εξεταζουμε το cell για τους γειτονες τους
            for(int i=-1;i<2;i++){
                neighbor.x=lines-1;
                neighbor.y=rows-i;
                if(life_get_cell(state,neighbor)){
                    counter++;
                }
            }
            for(int i=-1;i<2;i++){
                neighbor.x=lines+1;
                neighbor.y=rows-i;
                if(life_get_cell(state,neighbor)){
                    counter++;
                }
            }
            for(int i=-1;i<=1;i=i+2){
                neighbor.x=lines;
                neighbor.y=rows+i;
                if(life_get_cell(state,neighbor)){
                    counter++;
                }
            }
            // και αφου εχουμε παρει τον αριθμο των ζωντανων γειτόνων (counter)
            // του cell που εξεταζουμε αυτη τη στιγμη
            // εφαρμοζουμε τους κανονες
            cordinates->x=lines;
            cordinates->y=rows;
            node=map_find_node(state,cordinates);
            // αμα ειναι dead cell//
            if(node==NULL){
                if(counter==3){
                    map_insert(new_state,create_struct(cordinates),create_int(1));
                }
            }
            // αμα ειναι alive cell//

            // Τα alives cells που γινονται dead cells απλα τα αγνοουμε και δεν τα βαζουμε
            // στη νεα κατασταση
            else{
                int* value=map_node_value(state,node);
                if(*value==1){
                    if(counter==3||counter==2){
                        map_insert(new_state,create_struct(cordinates),create_int(1));
                    }
                }
                else{
                    if(counter==3){
                        map_insert(new_state,create_struct(cordinates),create_int(1));
                    }
                }
            }
            counter=0;
        }
    }
    return new_state;

}
// destroy the memory that was allocated for the state
void life_destroy(LifeState state){
    map_destroy(state);
}
// επιστεφει το αριθμο των alive cells
int LifeState_size(LifeState state){
    return map_size(state);
}
// this function checks if we have loop
bool loop_check(List list_of_states,LifeState new_state){

    for(ListNode node=list_first(list_of_states);
		node!=LIST_EOF;
		node=list_next(list_of_states,node)){
        LifeState list_state=list_node_value(list_of_states,node);
        if(LifeState_size(list_state)==LifeState_size(new_state)){
            //first node of the new_state
            MapNode map_cell=map_first(new_state);
            //first node of the list_state
            MapNode list_cell=map_first(list_state);

            //first lifecell of the new_state
            LifeCell* map_cell_key=map_node_key(new_state,map_cell);
            //first lifecell of the list_state
            LifeCell* list_cell_key=map_node_key(list_state,list_cell);
            int count=1;
            //εχουμε παρει 2 states(το new_state και καποιο state απο την λιστα)
            //και συγκρινουμε τα nodes των 2 states με την βοηθεια της compare_structs
            while(compare_structs(map_cell_key,list_cell_key)==0 && count<LifeState_size(new_state)){
                map_cell=map_next(new_state,map_cell);
                list_cell=map_next(list_state,list_cell);

                map_cell_key=map_node_key(new_state,map_cell);
                list_cell_key=map_node_key(list_state,list_cell);

                count++;
            }
            //αμα η προηγουμη λουπα εγινε τόσες φορες οσος ειναι και ο αριθμος των nodes τοτε τα
            //δυο states ειναι ιδια
            if(count==LifeState_size(new_state)){
                return true;
            }
            
        }
    }
    return false;

}
// this function makes a list of multiple(<=steps) evolved states
// it stops if we find a loop 
List life_evolve_many(LifeState state, int steps, ListNode* loop){
    List list_of_states=list_create(free);
    LifeState new_state=life_create();
    int i=0;
    bool loop_exist=false;
    //original state
    list_insert_next(list_of_states,LIST_BOF,state);
    ListNode node=list_first(list_of_states);
    while(i<steps){
        new_state=life_evolve(state);
        //check for the loop
        if(loop_check(list_of_states,new_state)){
            loop_exist=true;
        }
        if(loop_exist){
            break;
        }
        //next state
        list_insert_next(list_of_states,node,new_state);
        node=list_next(list_of_states,node);
        //save the new state
        state=new_state;
        i++;
    }
    // αμα εχουμε βρει loop επιστρεφουμε το επομενο state απο το τελευταιο της λουπας
    if(loop_exist){
        new_state=life_evolve(state);
        List list_loop=list_create(free);
        list_insert_next(list_loop,LIST_BOF,new_state);
        *loop=list_first(list_loop);
        //if you want the LifeState of the ListNode loop in main() do this:
        //LifeState loop_value=list_node_value(list_loop,loop);
        //print_state(loop_value);
    }
    return list_of_states;

}
// other functions
// prints the state based on the cordinates
void print_state(LifeState state){
    int size=map_size(state);
    MapNode node=map_first(state);
    for(int i=0;i<size;i++){
        LifeCell* cell=map_node_key(state,node);
        printf("x=%d y=%d   ",cell->x,cell->y);
        int* value=map_node_value(state,node);
        printf("value=%d\n",*value);
        node=map_next(state,node);

    }
}

//prints the input
//can be used in the function LifeState life_create_from_rle(char* file);
void print_input(Vector input){
    int size=vector_size(input);
    VectorNode node=vector_first(input);
    for(int i=0;i<size;i++){
        char* value=vector_node_value(input,node);
        printf("%s ",value);
        node=vector_next(input,node);       
    }
    printf("\n");
}
// prints all the LifeStates of the list
void print_list(List list_of_states){
    int i=0;
    for(ListNode node=list_first(list_of_states);
		node!=LIST_EOF;
		node=list_next(list_of_states,node)){
			LifeState current_state=list_node_value(list_of_states,node);
            printf("%d state:\n",i);
            print_state(current_state);
            i++;
	}
}