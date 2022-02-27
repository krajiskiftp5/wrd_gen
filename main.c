//GPL v3

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define list_of_wordlists LoW
//define buffer_size bs

int bs;
int number_of_wordlists = 0;
int  *lenght_of_wordlists;
char ***list_of_wordlists;

int loop(char *base, char *cmd);
int find_wordlist_id(char *alias);
int loop_wordlist(int wordlist_id, char *base, char *cmd);
int get_file_length(char *file_name);
char **load_file(char *file_name, int *len, char *alias);
char ***load_all_wordlists(int *len, int *low);
char *crop_string(char *in, int b, int e);
void print_lines(char **lines, int len);

int main(int argc, char *argv[]){
	LoW = load_all_wordlists(&number_of_wordlists,lenght_of_wordlists);	
    bs = atoi(argv[1]);

	for(int i = 2; i < argc; i++){
		loop("",argv[i]);	
	}
	return 0;
}

int loop_wordlist(int wordlist_id, char *base, char *cmd){
	if(wordlist_id == -1)
		return -1;
	int len = lenght_of_wordlists[wordlist_id] - 1;
	if(strlen(cmd) < 3){
		char **wl = list_of_wordlists[wordlist_id]+1;	
		int residue = len % bs;
			
		for(int i = 0; i < len/bs;i++){
			char *new, *new_base;
			int bl = strlen(base) + 1, nl, nb, length = 0;
			
            new_base = (char *)malloc(bl * sizeof(char));
			strcpy(new_base,"");

			for(int j = 0;j < bs;j++){
				nb = strlen(new_base) + 1;
				nl = strlen(wl[bs*i+j]) + 1;
				char *nnb = (char *)realloc(new_base,(nb + bl + nl)*sizeof(char));
				
                if(!nnb){
					fprintf(stderr,"Realloc failed\n");
					exit(0);
				}
					
				new_base = nnb;
				strncat(new_base,base,bl);
				strncat(new_base,wl[bs*i+j],nl);
				strcat(new_base,"\n");
				length += 1 + bl + nl;
			}

			printf("%s",new_base);
			free(new_base);
		}
		
		residue += (len / bs) * bs; 
		for(int i = (len / bs) * bs; i < residue; i++){
			int lenlen = strlen(base)+strlen(wl[i])+1;
			char *final = (char *)malloc(lenlen * sizeof(char));			
			strcpy(final, "");	
			strncat(final, base, strlen(base));
			strncat(final, wl[i], strlen(wl[i]));
			printf("%s\n", final);
			free(final);
		}	
	
		return 0;
	}
	for(int i = 1; i <= len;	i++){
		char *new_base, *wl = list_of_wordlists[wordlist_id][i];
		int new_len = strlen(wl), base_len = strlen(base);

		new_base = (char *)malloc((new_len+base_len+1)*sizeof(char));
		strcpy(new_base,"");
		strncat(new_base,base,base_len);
		strncat(new_base,wl, new_len);
		
		loop(new_base, cmd);
		free(new_base);	
	}
	return 0;
}

int loop(char *base, char *cmd){
	int s = -1, e = -1, c = 0;
	int wordlist_id, commas[10] = {0};

	for(int i = 0; i < strlen(cmd); i++){
		if(cmd[i] == '[' && s == -1) s = i + 1;
		if(cmd[i] == ']' && e == -1) e = i;
	}

    if(s == -1 || e == -1){
        fprintf(stderr, "Incorrectly written command\n");
        exit(1);
    }

	for(int i = s; i <= e; i++)
		if(cmd[i] == ',')
			commas[++c] = i;
	commas[++c] = e;
	
    for(int i = 0; i < c; i++){
		wordlist_id = find_wordlist_id(crop_string(cmd,commas[i]+1,commas[i+1]));	      loop_wordlist(wordlist_id, base, crop_string(cmd,e+1,strlen(cmd)));
	}	

	return 0;
}

int find_wordlist_id(char *alias){
	for(int i = 0; i < number_of_wordlists; i++){
		if(strcmp(alias, list_of_wordlists[i][0]) == 0)
			return i;
	}
	return -1;
}

char **load_file(char *file_name, int *len, char *alias){
	int cl = 100; //chunk lenght
	int noc = 1; //number of chunks

	FILE *file = fopen(file_name, "r");
	
	if(!file){
	    fprintf(stderr, "Failed to load file named: %s\n", file_name);
        exit(1);
    }
	
	char **lines = (char **)malloc(cl * sizeof(char *));
	char buf[50];
	int i = 1;
	lines[0] = alias;

	while(fgets(buf, 50, file)){
		buf[strlen(buf)-1] = '\0';	

		int string_lenght = strlen(buf);
		char *str = (char *)malloc((string_lenght + 1) * sizeof(char));
			
		strcpy(str,buf);
		lines[i++] = str;
		
		if(i == cl * noc){
			char **nl = (char **)realloc(lines, (++noc) * cl * sizeof(char *));
			if(!nl){
                fprintf(stderr, "Realloc failed");
                exit(1);
            }  
			lines = nl;
		}
	}
	*len = i;
	return lines;
}

void print_lines(char **lines, int len){
	for(int i = 0;i < len; i++){
		printf("%s\n", lines[i]);
	}
}

char *crop_string(char *in, int b, int e){
	char *new = (char *)malloc((strlen(in) + 1) * sizeof(char));
	strcpy(new,"");	strncat(new, in, e); return new + b;	
}

int get_file_length(char *file_name){
	FILE *wordlist_file = fopen(file_name, "r");
	int count_of_words = 0;

	char ch;
	while((ch=fgetc(wordlist_file))!=EOF)if(ch=='\n')count_of_words++;

	fclose(wordlist_file);
	return count_of_words;
}

char ***load_all_wordlists(int *len, int *low){
	int number_of_aliases, number_of_words;
	char ***list_of_wordlists, **aliases;

	aliases = load_file("alias.txt", &number_of_aliases,"als") + 1;
	number_of_aliases = (number_of_aliases - 1) / 2;

	list_of_wordlists = (char ***)malloc(number_of_aliases * sizeof(char**));		  lenght_of_wordlists = (int*)malloc(number_of_aliases * sizeof(int));

	for(int i = 0; i < number_of_aliases; i++){
		LoW[i]=load_file(aliases[2*i+1],&number_of_words,aliases[2*i]);	
		lenght_of_wordlists[i] = number_of_words;
	}
	
	low = lenght_of_wordlists;	
	*len = number_of_aliases;
	return list_of_wordlists;
}


