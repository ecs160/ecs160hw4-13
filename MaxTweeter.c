#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct Map { //linked list nodes
    char *name;
    int count;
    struct Map* next;
};

int countC(char *input){ //count how many comma separated fields there are
    int ret = -1;
    if(input){
        ret++;
        for(int i = 0; i < strlen(input); i++){
            if(input[i] == ','){
                ret++;
            }
        }
    }
    return ret;
}

char* omitQuote(char *input){ //if string has quotes around the outside omit them
    char * ret = NULL;
    if(input && input[0] != '\0')
        input = strtok(input, "\n\r");
    if(input != NULL && strlen(input) > 1 && input[0] == '\"' && input[strlen(input)-1]=='\"'){
        ret = calloc(strlen(input)-1,sizeof(char)); //deallocated
        if(strlen(input) > 2){
            strncpy(ret, input+1, strlen(input)-2);
        }
    } else {
        if(input){
            ret = calloc(strlen(input)+1, sizeof(char));
            strcpy(ret, input);
        }
    }
    return ret;
}

char* getColumn(char* input, int index){ //strtok that can deal with empty fields
    char* ret = NULL;
    if(input)
        input = strtok(input, "\n\r");
    if(input){
        int length = (int)strlen(input);
        int count = 0;
        int start = -1, end = -1;
        int i;
        for(i = 0; i < length; i++){
            if(count == index && start == -1){
                start = i;
            }
            if(input[i] == ','){
                if(start != -1 && end == -1){
                    end = i;
                }
                count++;
            }
        }
        if(end != start){
            if(end == -1){
                end = length;
            }
            char *temp = calloc(end-start+1,sizeof(char));//deallocated
            strncpy(temp, input+start, end - start);
            ret = temp;
        }
    }
    return ret;
}

int main(int argc, char** argv)
{
    struct Map * head = NULL;
    //printf("Input file: %s\n", argv[1]);
    FILE* stream = fopen(argv[1], "r");
    if (!stream){ //file unaccessible
        printf("Invalid Input Format\n");
        exit(1);
    }
    char line[1024], test[1025];
    fgets(test, 1025, stream);
    if(strlen(test) > 1023){ //line longer than 1024 including \0
        printf("Invalid Input Format\n");
        exit(2);
    } else {
        strcpy(line, test);
    }
    bool *quotes;
    bool first = false;
    bool sec = false;
    int fc = -1;
    char *s = NULL;
    if(line[0] == '\r'){ //no header on the first line
        printf("Invalid Input Format\n");
        exit(3);
    }
    int fields = countC(line) + 1;
    quotes = malloc(sizeof(bool)*(fields));//deallocated
    int k = 0;
    for(k = 0; k < fields; k++){ //find name collumn and check quotations
        s = getColumn(line, k);
        quotes[k] = false;
        if(s){//} && s[0] == '\"'){
            int length = (int)strlen(s);
            if((length == 1 && s[0] == '\"') || (s[length-1] == '\"' ^ s[0] == '\"')){
                printf("Invalid Input Format\n");//quotation unmatched
                exit(4);
            } else if (s[0] == '\"'){
                quotes[k] = true; //storing quotation values
            }
        }
        char*hold = omitQuote(s);
        if(hold && strcmp(hold, "name")==0){
            if (!first){
                first = true;
                fc = k;
            } else {
                sec = true;
            }
        }
        free(hold);
        free(s);
    }
    if(!first || sec){ //no name column
        printf("Invalid Input Format\n");
        exit(5);
    }
    //printf("name column index: %d\n",fc);//debug
    int lines = 1;
    while(fgets(test,1025,stream)){//iterate through csv
        lines++;
        if(lines>20000){//file contains more than 20000 lines
            printf("Invalid Input Format\n");
            exit(6);
        }
        if(strlen(test) > 1023){//line exceeds 1024 character limit
            printf("Invalid Input Format\n");
            exit(7);
        } else {
            strcpy(line, test);
        }
        if(countC(line) != fields - 1){//line does not have enough fields
            if(line[0]=='\n' || line[0]=='\r')
                continue;
            printf("Invalid Input Format\n");
            exit(8);
        }
        s = getColumn(line, fc);
        if(s){//name has unmatched quotations
            int length = (int)strlen(s);
            if((length == 1 && s[0] == '\"') || (s[length-1] == '\"' ^ s[0] == '\"')){
                printf("Invalid Input Format\n");//quotation unmatched
                exit(9);
            }
        }
        if((quotes[fc] ^ (s && (s[0] == '\"') && (s[strlen(s)-1] == '\"')))){
            printf("Invalid Input Format\n");//quotation does not match header name
            exit(10);
        }

        struct Map * itr = head;
        bool fl = false;
        while(itr){//counting and setting up flag to insert new name into linked list
            if(s && itr->name){
                if(strcmp(itr->name,s)==0){
                    fl = true;
                    itr->count++;
                }
            } else {
                if(!s && itr->name[0]=='\0'){
                    fl = true;
                    itr->count++;
                }
            }
            itr = itr->next;
        }
        if (!fl){//inserting new name
            struct Map * temp = (struct Map*)malloc(sizeof(struct Map));//deallocated
            if(s){
                temp->name = malloc(sizeof(char)*(strlen(s)+1));
                strcpy(temp->name,s);
            } else {
                temp->name = malloc(sizeof(char));
                temp->name[0] = '\0';
            }
            temp->count = 1;
            temp->next = head;
            head = temp;
        }
        free(s);
    }
    struct Map* tem = head;
//    printf("%s\n","List unsorted"); Debug purpose
//    while(tem){
//        printf("%s %i\n",tem->name, tem->count);
//        tem = tem->next;
//    }
//    printf("\n%s\n","List sorted");
    tem = head;
    struct Map* ohead = NULL, *otail = NULL;
    while(tem){//sorts the original linked list and store as a new list
        struct Map* tm = ohead;
        bool t = true;
        otail = NULL;
        while(tm && t){
            if(tm->count < tem->count){
                struct Map * temp = (struct Map*)malloc(sizeof(struct Map));//deallocated
                temp->name = malloc(sizeof(char)*(strlen(tem->name)+1));
                strcpy(temp->name,tem->name);
                temp->count = tem->count;
                temp->next = tm;
                if(!otail){
                    ohead = temp;
                } else {
                    otail->next = temp;
                }
                t = false;
            }
            otail = tm;
            tm = tm->next;
        }
        if(t){
            struct Map * temp = (struct Map*)malloc(sizeof(struct Map));//deallocated
            temp->name = malloc(sizeof(char)*(strlen(tem->name)+1));
            strcpy(temp->name,tem->name);
            temp->count = tem->count;
            temp->next = tm;
            if(ohead){
                otail->next = temp;
            } else {
                ohead = temp;
            }
            
        }
        tem = tem->next;
    }
    tem = ohead;
    int j = 0;
    while(tem && j <10){ //outputing top ten
        char *hold = omitQuote(tem->name);
        j++;
        printf("%s %d\n",hold,tem->count);
        tem = tem->next;
        free(hold);
    }
    free(quotes); //memory deallocation from here to end
    struct Map *temp = head;
    while(temp){
        temp = temp->next;
        free(head->name);
        free(head);
        head = temp;
    }
    temp = ohead;
    while(temp){
        temp = temp->next;
        free(ohead->name);
        free(ohead);
        ohead = temp;
    }
}

