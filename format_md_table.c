#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_FIELD 10
#define MAX_LINE  100

#define debug_line() printf("%d\n", __LINE__);

int is_zh_ch(char p)
{
    if( (p >> 8) == 0) {
	return 0;
    }

    return 1;
}

size_t cal_len(char* str)
{
    char *cur = str;
    size_t zh, en;

    for(zh=0, en=0; *cur != '\0'; cur++)
    {
	is_zh_ch(*cur) ? zh++ : en++;
    }

    return zh/3*2+en;
}

char* align_text(char* str, int s_cnt)
{
    int space = s_cnt - cal_len(str);
    // printf("str = %s\n", str);
    char * dest = calloc(1, strlen(str)+space+1);
    for(int i=0; i<strlen(str)+space; i++)
	dest[i]=' ';
    // [this is test] -> [ this is test            ]
    memcpy(dest+1, str, strlen(str));

    return dest;
}

int main(int argc, char* argv[])
{
    FILE* fp_s;
    char* t_line = NULL;
    char* t_word[MAX_LINE][MAX_FIELD] = {{NULL}};
    char* t_temp[MAX_FIELD] = {NULL};
    int max_line[MAX_FIELD] = {0};
    char buffer[4096] = {0};
    int max_row, max_col;
    char *temp;

    /* Delete all of tab & space */
    char cmd[512]={0};
    sprintf(cmd, "%s%s", "sed -i s/[[:space:]]//g ", argv[1]);
    // printf("cmd=%s\n", cmd);
    system(cmd);

    fp_s = fopen(argv[1], "r");
    int row = 0;
    int col = 0;
    do {
	memset(buffer, 0, sizeof(buffer));
	t_line = fgets(buffer, sizeof(buffer), fp_s);
	if(t_line == NULL)
	    break;
	buffer[strlen(buffer)-1] = '\0'; // Delete \n
	//printf("line = %s\n", buffer);

	col = 0;
	temp = strtok(buffer, "|");
	t_word[row][col] = calloc(1, strlen(temp));
	memcpy(t_word[row][col], temp, strlen(temp));
	t_word[row][col][strlen(temp)] = '\0';
	size_t len = cal_len(t_word[row][col]);
	if (max_line[col] < len)
	    max_line[col] = len;
	printf("t_word[%d][%d](%ld) = %s\n", row, col, len, t_word[row][col]);
	col++;
	while(NULL != temp)
	{
	    temp = strtok(NULL, "|");
	    if(!temp)
		break;
	    t_word[row][col] = calloc(1, strlen(temp));
	    memcpy(t_word[row][col], temp, strlen(temp));
	    t_word[row][col][strlen(temp)] = '\0';
	    len = cal_len(t_word[row][col]);
	    if (max_line[col] < len)
		max_line[col] = len;
	    printf("t_word[%d][%d](%ld) = %s\n", row, col, len, t_word[row][col]);
	    col++;
	}
	row++;
	if(max_col < col)
	    max_col = col;
    } while (t_line);
    if(max_row < row)
	max_row = row;

    printf("max_row=%d\n max_col=%d\n", max_row, max_col);

    for(int col=0; col < max_col; col++)
	max_line[col]+=2;

    FILE* fp_d = fopen(argv[2], "w");

    for(int row=0; row < max_row; row++)
    {
	fprintf(fp_d, "|");
	for(int col=0; col < max_col; col++)
	{
	    temp = align_text(t_word[row][col], max_line[col]);
	    //printf("temp = [%s]\n", temp);
	    free(t_word[row][col]);
	    fprintf(fp_d, "%s|", temp);
	    free(temp);
	}
	fprintf(fp_d, "\n");
    }

    fclose(fp_d);
    fclose(fp_s);

    return 0;
}
