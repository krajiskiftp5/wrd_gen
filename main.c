#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define list_of_wls LoW
#define crop_string cstr

int number_of_wls = 0;
int  *lenght_of_wls;
char ***list_of_wls;

int loop(char *base, char *cmd);
int find_wl_id(char *alias);
int loop_wl(int wl_id, char *base, char *cmd, char *mod);
char **load_file(char *file_name, int *len, char *alias);
char ***load_all_wls(int *len, int *low);
char *cstr(char *in, int b, int e);
int join(char *base, char *new, char *cmd, char *mod);
int decide(char *base, char *to_mod, char *cmd);
void all_caps(char *string);

int main(int argc, char *argv[]){
	FILE *save = fopen("command_history.txt", "a");
	LoW = load_all_wls(&number_of_wls,lenght_of_wls);	

	for(int i = 1; i < argc; i++){
		fprintf(save, "%s\n", argv[i]);
		loop("",argv[i]);	
	}
	fclose(save);
	return 0;
}

void rot_13(char *string){
	for(int i = 0; i < strlen(string); i++){
		string[i] += 13;
		if(isalpha(string[i])==0)string[i]-=26;
	}
}
	
void all_caps(char *string){
	for(int i = 0; i < strlen(string); i++)	
		if(isalpha(string[i])) string[i] -= 32;
}

int join(char *base, char *new, char *cmd, char *mod){
	int s = strlen(base) + strlen(new) + 1;
	char *out = (char*)malloc(s * sizeof(char));
	char *to_mod = (char *)malloc((strlen(new)+1)*sizeof(char));
	strcpy(to_mod, new);
	
	decide(base, to_mod, cmd);	
	if(strchr(mod,'c')!=NULL){
		to_mod[0] -= 32;
		decide(base, to_mod, cmd);
	}
	if(strchr(mod,'u')!=NULL){
		all_caps(to_mod);
		decide(base, to_mod, cmd);
	}
	if(strchr(mod,'r')!=NULL){
		rot_13(to_mod);
		decide(base, to_mod, cmd);
	}
}

int decide(char *base, char *to_mod, char *cmd){
	int s = strlen(base) + strlen(to_mod) + 1;
	char *out = (char*)malloc(s * sizeof(char));
	sprintf(out, "%s%s", base, to_mod);
	if(strlen(cmd) < 2) printf("%s\n", out);
	else loop(out,cmd);
}	

int loop_wl(int wl_id, char *base, char *cmd, char *mod){
	int len = lenght_of_wls[wl_id] - 1;
	for(int i = 1; i <= len; i++){
		char *new_base, *wl = list_of_wls[wl_id][i];
		int new_len = strlen(wl), base_len = strlen(base);
		join(base, wl, cmd, mod);
	}
	return 0;
}

int loop(char *base, char *cmd){
	int s = -1, e = -1, c = 0;
	int wl_id, commas[10] = {0};

	for(int i = 0; i < strlen(cmd); i++){
		if(cmd[i] == '[' && s == -1) s = i + 1;
		if(cmd[i] == ']' && e == -1) e = i;
	}

    	if(s == -1 || e == -1){
        	fprintf(stderr, "Incorrectly written command\n");
        	exit(1);
    	}

	for(int i = s; i <= e; i++)if(cmd[i] == ',')commas[++c] = i;
	commas[++c] = e;

	char *cm = cmd;	
    	for(int i = 0; i < c; i++){
		char *mod = strchr(cm+1,':');
		if(mod == NULL) s = commas[i+1];
		else{
			s = strlen(cmd)-strlen(mod);
			if(s > commas[i+1]) s = commas[i+1]; 
		}
		cm = mod + 1;
		wl_id = find_wl_id(cstr(cmd,commas[i]+1,s));
        	if(wl_id == -1)	return -1;
		loop_wl(wl_id,base,cstr(cmd,e+1,strlen(cmd)),cstr(cmd,s+1,commas[i+1]));
	}
	return 0;
}

int find_wl_id(char *alias){
	for(int i = 0; i < number_of_wls; i++)
		if(strcmp(alias, list_of_wls[i][0]) == 0) return i;
	fprintf(stderr, "Alias: %s not found. \n", alias);
	return -1;
}

char **load_file(char *file_name, int *len, char *alias){
	int i = 1, cl = 100; /*chunk lenght*/ int noc = 1; /*number of chunks*/
	FILE *file = fopen(file_name, "r");
	
	if(!file){
	    	fprintf(stderr, "Failed to load file named: %s\n", file_name);
        	exit(1);
    	}
	
	char **lines = (char **)malloc(cl * sizeof(char *)); char buf[50];
	lines[0] = alias;

	while(fgets(buf, 50, file)){
		buf[strlen(buf)-1] = '\0';	

		int string_lenght = strlen(buf);
		char *str = (char *)malloc((string_lenght + 1) * sizeof(char));
			
		strcpy(str,buf);
		lines[i++] = str;
		
		if(i == cl * noc){
			char **nl=(char**)realloc(lines,(++noc)*cl*sizeof(char*));
			lines = nl;
		}
	}
	*len = i;
	return lines;
}

char *cstr(char *in, int b, int e){
	char *new = (char *)malloc((strlen(in) + 1) * sizeof(char));
	strcpy(new,"");	strncat(new, in, e); return new + b;	
}

char ***load_all_wls(int *len, int *low){
	int number_of_aliases, number_of_words;
	char ***list_of_wls, **aliases;

	aliases = load_file("alias.txt", &number_of_aliases,"als") + 1;
	number_of_aliases = (number_of_aliases - 1) / 2;

	list_of_wls = (char ***)malloc(number_of_aliases * sizeof(char**));
    	lenght_of_wls = (int*)malloc(number_of_aliases * sizeof(int));

	for(int i = 0; i < number_of_aliases; i++){
		LoW[i]=load_file(aliases[2*i+1],&number_of_words,aliases[2*i]);	
		lenght_of_wls[i] = number_of_words;
	}
	
	low = lenght_of_wls;	
	*len = number_of_aliases;
	return list_of_wls;
}


