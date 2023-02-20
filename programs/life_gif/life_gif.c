#include "bmp.h"
#include "gif.h"
#include "../../modules/life.c"
#include <math.h>
int main(int argc,char* argv[]){
    //reading file procedure
    char* file=malloc(50*sizeof(char));
    if(file==NULL){
        printf("memory not allocated\n");
        return -1;
    }
    char* gif_name=malloc(50*sizeof(char));
    if(gif_name==NULL){
        printf("memory not allocated\n");
        return -1;
    }
    if(file==NULL){
        printf("memory not allocated\n");
        return -1;
    }
    if(argc<11){
        printf("wrong input\n"); //we didnt get the enough command line argument
        return -1;
    }
    //we copy the name of the file to the variable
    strcpy(file,argv[1]); 
    //we save the dimensions
    //πρεπει να ισχυει left+right>0
    //και top+bottom>0
    int top=atoi(argv[2]);
    int left=atoi(argv[3]);
    int bottom= atoi(argv[4]);
    int right=atoi(argv[5]);
    if(left+right<=0||bottom+top<=0){
        printf("Couldnt make the gifs outline\n");
        printf("wrong cordinates(top/left/bottom/right) input");
        return -1;
    }
    //the frames of the gif
    int frames=atoi(argv[6]);
    if(frames<1){
        printf("wrong frames input\n");
        return -1;
    }
    //zoom
    float zoom=atoi(argv[7]);
    if(zoom<1){
        printf("this program doesnt use zoom < 1 \n");
        return -1;
    }
    //speed
    int saved_speed=atoi(argv[8]); 
    if(saved_speed<1){
        printf("speed must be higher or equal to 1\n");
        return -1;
    }
    int speed=1;
    //delay of the frames
    //ενώ το δέχεται δεν έχει νοημα το αρνητικό delay 
    //οποτε βγαζω μυνημα λαθους
    int delay=atoi(argv[9]);
    if(delay<0){
        printf("delay should be positive or zero\n");
        return -1;
    }
    //name of the gif
    strcpy(gif_name,argv[10]);

    int height=(top+bottom)*zoom;
    int width=(left+right)*zoom;

    //cell_size
    //το έχω κανει comment το cell_size 
    //γιατι εξουδετερώνεται απο το -1 που θα βαζαμε στο
    //bm_fillrect(..,..,..-1,..-1)
    //int cell_size=1;

    //for present the animation close to the center
    // a random offset to show "more" of the animation
    //αμα θελετε να μην επηρεάζει το bm_fillrect απλα βαλτε 0
    int offset=left+20;

    //for saving the state if there is a loop
    List list_loop=list_create(free);   
    list_insert_next(list_loop,LIST_BOF,NULL);
    ListNode loop=list_first(list_loop);

    //creating the list for the states
    //βαζω ενα τυχαιο αριθμο για steps
    int steps=30;
    LifeState state=life_create_from_rle(file);
    List list_of_states=life_evolve_many(state,steps,&loop);
    
    //καντε uncomment την επομενη εντολη για να δειτε αναλυτικα το καθε state 
    //print_list(list_of_states);

    // uncomment bellow to show the last state of the loop

    // LifeState loop_value=list_node_value(list_loop,loop);
    // if(loop_value!=NULL){
    //     printf("The last LifeState from the loop:\n");
    //     print_state(loop_value);
    // }

    // Δημιουργία ενός GIF και ενός bitmap στη μνήμη
	GIF* gif = gif_create(width, height);
	Bitmap* bitmap = bm_create(height, width);
    gif->default_delay = delay;
    for(int i=0;i<frames;i++){
        //Διατρέχω όλα τα states για κάθε frame 
        for(ListNode node=list_first(list_of_states);
            node!=LIST_EOF;
            node=list_next(list_of_states,node)){
                if(speed==1){
                        // Σε κάθε frame, πρώτα μαυρίζουμε ολόκληρο το bitmap
                        bm_set_color(bitmap, bm_atoi("black")); //black
                        bm_clear(bitmap);

                        LifeState current_state=list_node_value(list_of_states,node);
                        int size=map_size(current_state);
                        MapNode node=map_first(current_state);
                        for(int i=0;i<size;i++){
                            LifeCell* cell=map_node_key(current_state,node);
                            // Και μετά ζωγραφίζουμε ένα άσπρο τετράγωνο με αρχή το
                            // σημείο (i,i) και τέλος το (i+cell_size, i+cell_size)
                            bm_set_color(bitmap, bm_atoi("white")); //white
                            bm_fillrect(bitmap,offset+cell->y,offset+cell->x,offset+(cell->y),offset+(cell->x));
                            node=map_next(current_state,node);
                        }
                        speed=saved_speed;
                }
                else{
                    speed--;
                }
                // Τέλος προσθέτουμε το bitmap σαν frame στο GIF (τα περιεχόμενα αντιγράφονται)
                gif_add_frame(gif, bitmap);
            }
        }
    // Αποθήκευση σε αρχείο
	gif_save(gif, gif_name);
    free(file);
    free(gif_name);
    list_destroy(list_loop);
    list_destroy(list_of_states);
    return 0;
}