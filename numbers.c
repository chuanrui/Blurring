#include <stdio.h>     
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
//create variable to store current number 
float curr_num;
//create an array to store five numbers
float last_five[5];
//index to decide the position of input number, count is the total times of inputs, 
//start is to check if user start to input
int ind=0, count=0, start = 0; 
//variables storing max, min and average value 
float max, min, average;
//create mutex lock
pthread_mutex_t lock;
/*
 * helper function, calculate those required value
 */
void process(float num){
    int i;
    //start lock
    pthread_mutex_lock(&lock);
    //first input
    if(ind==0){
        last_five[ind] = num;
        max=num;
        min=num;
        average=num;
        ind++;
    }
    //more than 5 inputs
    else if(ind==5){
        for(i=4;i>0;i--){
            last_five[i] = last_five[i-1];
        }
        last_five[0] = num;
        if (max<num) max = num;
        if (min>num) min = num;
        average = (average*count+num)/(count+1);
    }
    else{
        for(i=ind;i>0;i--){
            last_five[i] = last_five[i-1];
        }
        last_five[0] = num;
        ind++;
        if (max<num) max = num;
        if (min>num) min = num;
        average = (average*count+num)/(count+1);
    }
    count++;
    start = 1;
    //release lock
    pthread_mutex_unlock(&lock); 
}
/*
 * function in thread, print the current values
 */
void* display(void* unused){
    while(1){
        sleep(10);
        //start lock
        pthread_mutex_lock(&lock);
        //if user has not input
        if(start==0){
            printf("No input yet\n");
        }
        //if user has made input
        else{
            int i;
            printf("Max value so far is %f\n",max);
            printf("Min value so far is %f\n",min);
            printf("Average value so far is %f\n",average);
            printf("The last five inputs are\n");
            for(i=0;i<ind;i++){
                printf("%f ", last_five[i]);
            }
            printf("\nPlease input a number\n");
        }
        //release lock
        pthread_mutex_unlock(&lock); 
    }
    return unused;
}
/*
 * function that check if the input is a number
 */ 
int check_num(char* input){
    char* copy_input = malloc(strlen(input)*sizeof(char)+1);
    strcpy(copy_input, input);
    char* delim1 = {" \n"};
    char* delim2 = {" .0123456789\n"};
    char* token = strtok(copy_input, delim1);
    //1.empty string or only has space
    if(token == NULL){
        free(copy_input);
        return 0;
    }
    token = strtok(NULL, delim1);
    //2.has space between numbers
    if(token != NULL){
        free(copy_input);
        return 0;
    }
    //3.has other char like 'a' or '+' or...
    token = strtok(copy_input, delim2);
    if(token != NULL){
        free(copy_input);
        return 0;
    }
    free(copy_input);
    return 1;
}
/*
 *function in thread, ask for input and calculate new values 
 */
void ask_input(){
    char* input = malloc(100*sizeof(char)+1);
    while(1){
        printf("Please input a number\n");
        fgets(input, 100, stdin);
        if(check_num(input)==1){
            curr_num = atof(input);
            process(curr_num);
        }
        else{
            printf("Incorrect format, please make sure it is a number\n");
        }
    }
    free(input);
}
int main(){
    pthread_t t;
    pthread_mutex_init(&lock, NULL);
    //just to meet the format request of pthread_create
    int useless = 1;
    void* unused;
    pthread_create (&t, NULL, &display, &useless);
    ask_input();
    //just to mset the format request of pthread_join
    pthread_join (t, &unused);
    pthread_mutex_destroy(&lock);
    return 0;
}