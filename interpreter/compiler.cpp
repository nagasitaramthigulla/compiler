#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include"utils.h"

struct symbol {
	char name;
	int base, size, isConst, value;
};

int linecount = 0;

struct symbolTable {
	int count;
	symbol **symbols;
	int consts,memory;
};

struct instruction {
	int opcode;
	int param_count;
	int *params;
};

struct lables {
	char *str;
	int value;
};

void printerror()
{
	printf("error in line %d", linecount);
	_getch();
	exit(0);
}

int getint(char *s)
{
	int val = 0;
	for (int i = 2; s[i] != ']'; i++)
		val = val * 10 + s[i] - '0';
	return val;
}

int isarray(char *s)
{
	int i;
	if ((s[0]<'A' || s[0]>'Z')&& (s[1] != '['))
		return 0;
	for (i = 2; s[i] >= '0'&&s[i] <= '9'; i++);
	if (s[i] != ']')
		return 0;
	return 1;
}

symbol *create_symbol(char **tokens,int isConst,int *base)
{
	symbol *sym = (symbol *)malloc(sizeof(symbol));
	if (isConst)
	{
		sym->isConst = 1;
		sym->name = tokens[1][0];
		sym->size = 1;
		if (strlen(tokens[2]) != 1 || tokens[2][0] != '=')
			printerror();
		if (!isnum(tokens[3]))
			printerror();
		sym->value = atoi(tokens[3]);
	}
	else
	{
		sym->isConst = 0;
		if (strlen(tokens[1]) == 1) {
			sym->name = tokens[1][0];
			sym->size = 1;
		}
		else if (isarray(tokens[1])) {
			sym->name = tokens[1][0];
			sym->size = getint(tokens[1]);
		}
		else {
			printerror();
		}
	}
	sym->base = (*base);
	(*base) += sym->size;
	return sym;
}

symbolTable * getSymbols(FILE *fp)
{
	symbolTable *table = (symbolTable*)malloc(sizeof(symbolTable));
	table->count = table->consts = 0;
	table->symbols = (symbol**)malloc(sizeof(symbol*) * 10);
	char *line=(char*)malloc(sizeof(char )*200);
	line[0] = '\0';
	char start[] = "START:",data[]="DATA",constant[]="CONST";
	int base = 0;
	while (!feof(fp)) {
		linecount++;
		getline(fp, &line);
		int counttokens;
		char **tokens = strtok(line, ' ',&counttokens);
		if (strcmp(data, tokens[0]) && counttokens == 2)
		{
			table->symbols[table->count++] = create_symbol(tokens, 0,&base);
		}
		else if (strcmp(constant, tokens[0]) && counttokens == 4)
		{
			table->consts++;
			table->symbols[table->count++] = create_symbol(tokens, 1,&base);
		}
		else if (strcmp(start, tokens[0]) && counttokens == 1)
		{
			break;
		}
		else {
			printerror();
		}
	}
	table->memory = base;
	return table;
}

void printsymboltable(symbolTable *table)
{
	printf("%-10s%-6s%-6svalue\n", "name", "base", "size");
	for (int i = 0; i < table->count; i++)
	{
		printf("%c%9s%-6d%-6d", table->symbols[i]->name, "", table->symbols[i]->base, table->symbols[i]->size);
		if (table->symbols[i]->isConst)
			printf("%d", table->symbols[i]->value);
		printf("\n");
	}
}

int hashstring(void *s,int buckets)
{
	char *str = (char *)s;
	int val = 0;
	int p = 3;
	int mod = (int)(1e9 + 7);
	for (int i = 0; str[i]; i++)
	{
		val = (val) % mod;
		val = val * p + str[i];
	}
	return val % buckets;
}

struct opcode {
	char str[20];
	int code;
};

struct labeltabel {
	lables *lbl;
	int count;
};

struct instructionTable {
	int count;
	instruction **ins;
	labeltabel *label;
};

int islabel(char *s)
{
	int i;
	for (i = 0; s[i] && s[i] != ':'; i++)
		if (s[i]<'A' || s[i]>'Z')
			return 0;
	if (s[i] == ':')
		return 1;
	return 0;
}

opcode opcodes[15] = { {"MOV",1},{"ADD",3},{"SUB",4},{"MUL",5},{"JMP",6},{"IF",7},{"EQ",8},{"LT",9},{"GT",10},{"LTEQ",11},{"GTEQ",12},{"PRINT",13},{"READ",14},{"ENDIF",15},{"ELSE",16} };

int findopcodes(char *s)
{
	for (int i = 0; i < 15; i++)
	{
		if (strcmp(s, opcodes[i].str))
			return opcodes[i].code;
	}
	return -1;
}

symbol *findsymbol(symbolTable *table,char c)
{
	for (int i = 0; i < table->count; i++)
		if (table->symbols[i]->name == c)
			return table->symbols[i];
	return 0;
}

instructionTable *loadinstructions(FILE *fp,symbolTable *symtab)
{
	instructionTable *table = (instructionTable *)malloc(sizeof(instructionTable));
	table->count = 0;
	table->ins = (instruction**)malloc(sizeof(instruction*) * 10);
	char *s = (char *)malloc(sizeof(char) * 200);
	int stk[100], top = 0;
	int opc,*a,ifc;
	char **token;
	symbol *sym;
	labeltabel *label = (labeltabel *)malloc(sizeof(labeltabel));
	label->count = 0;
	label->lbl = (lables *)malloc(sizeof(lables) * 10);
	char end[] = "END";
	while (!feof(fp))
	{
		linecount++;
		getline(fp, &s);
		int counttokens;
		char **tokens = strtok(s, ' ', &counttokens);
		instruction *ins =  (instruction*)malloc(sizeof(instruction)*10);
		if (( opc=findopcodes(tokens[0]))!=0)
		{
			switch (opc) {
			case 1:
				if (tokens[1][0] >= 'A'&&tokens[1][0] <= 'H'&&tokens[1][1] == 'X')
				{
					ins->opcode = 2;
					ins->param_count = 2;
					ins->params = (int *)malloc(sizeof(int) * 2);
					ins->params[0] = tokens[1][0] - 'A';
					ins->params[1] = tokens[2][0] - 'A';
					sym = findsymbol(symtab, tokens[2][0]);
					ins->params[1] = sym->base;
					if (isarray(tokens[2]))
					{
						ins->params[1] += getint(tokens[2]);
					}
					table->ins[table->count++] = ins;
				}
				else if (tokens[2][0] >= 'A'&&tokens[2][0] <= 'H'&&tokens[2][1] == 'X')
				{
					ins->opcode = 1;
					ins->param_count = 2;
					ins->params = (int *)malloc(sizeof(int) * 2);
					ins->params[0] = tokens[1][0] - 'A';
					ins->params[1] = tokens[2][0] - 'A';
					sym = findsymbol(symtab, tokens[1][0]);
					ins->params[0] = sym->base;
					if (isarray(tokens[1]))
					{
						ins->params[0] += getint(tokens[1]);
					}
					table->ins[table->count++] = ins;
				}
				else
					printerror();
				break;
			case 3:
			case 4:
			case 5:
				ins->opcode = opc;
				ins->param_count = 3;
				ins->params = (int *)malloc(sizeof(int) * 3);
				for (int i = 0; i < 3; i++)
					ins->params[i] = tokens[i + 1][0] - 'A';
				table->ins[table->count++] = ins;
				break;
			case 6:
				ins->opcode = opc;
				int z, i;
				ins->param_count = 1;
				ins->params = (int *)malloc(sizeof(int));
				token = strtok(tokens[1], ':', &z);
				for (i = 0; i < label->count; i++)
				{
					if (strcmp(token[0], label->lbl[i].str))
					{
						ins->params[0] = label->lbl[i].value;
						break;
					}
				}
				table->ins[table->count++] = ins;
				break;
			case 7:
				stk[top++] = table->count + 1;
				ins->opcode = opc;
				ins->param_count = 4;
				ins->params = (int *)malloc(sizeof(int) * 4);
				ins->params[0] = tokens[1][0] - 'A';
				ins->params[2] = tokens[3][0] - 'A';
				ifc = findopcodes(tokens[2]);
				if (ifc >= 8 && ifc <= 12)
				{
					ins->params[1] = ifc;
				}
				else
					printerror();
				table->ins[table->count++] = ins;
				break;
			case 13:
			case 14:
				ins->opcode = opc;
				ins->param_count = 1;
				ins->params = (int *)malloc(sizeof(int));
				ins->params[0] = tokens[1][0] - 'A';
				table->ins[table->count++] = ins;
				break;
			case 15:
				if (top)
				{
					int x = stk[--top];
					table->ins[x - 1]->params[table->ins[x - 1]->param_count - 1] = table->count + 2;
				}
				else
					printerror();
				break;
			case 16:
				if (top)
				{
					int x = stk[--top];
					table->ins[x - 1]->params[table->ins[x - 1]->param_count - 1] = table->count + 2;
					stk[top++] = table->count + 1;
					ins->opcode = 6;
					ins->params = (int *)malloc(sizeof(int));
					ins->param_count = 1;
					table->ins[table->count++] = ins;
				}
				else
				{
					printerror();
				}
				break;
			default:
				if (islabel(tokens[0])) {
					int q;
					char **ts = strtok(tokens[0],':',&q);
					label->count++;
					label->lbl[label->count - 1].value = table->count + 1;
					label->lbl[label->count - 1].str=(char *) malloc(sizeof(char )*20);
					strcpy(ts[0],&(label->lbl[label->count-1].str));
					if (label->count % 10 == 0)
						label->lbl = (lables *)realloc(label->lbl, sizeof(lables)*(label->count + 10));
				}
				else if (strcmp(tokens[0], end))
				{
					goto FLAG;
				}
			}
		}
	}
	FLAG:
	table->label = label;
	return table;
}

void runinstructions(instructionTable *inst, symbolTable *symt)
{
	int *ar = (int *)malloc(sizeof(int)*symt->memory);
	int reg[8] = { 0 };
	for (int i = 0; i < symt->count; i++)
	{
		if (symt->symbols[i]->isConst)
		{
			ar[symt->symbols[i]->base] = symt->symbols[i]->value;
		}
	}
	for (int i = 0; i < inst->count; i++)
	{
		instruction *ins = inst->ins[i];
		switch (ins->opcode)
		{
		case 1:
			ar[ins->params[0]] = reg[ins->params[1]];
			break;
		case 2:
			reg[ins->params[0]] = ar[ins->params[1]];
			break;
		case 3:

			reg[ins->params[0]] = reg[ins->params[1]] + reg[ins->params[2]];
			break;
		case 4:
			reg[ins->params[0]] = reg[ins->params[1]] - reg[ins->params[2]];
			break;
		case 5:
			reg[ins->params[0]] = reg[ins->params[1]] * reg[ins->params[2]];
			break;
		case 6:
			i = ins->params[0] - 2;
			break;
		case 7:
			if (ins->params[1] == 8 && reg[ins->params[0]] == reg[ins->params[2]]);
			else if (ins->params[1] == 9 && reg[ins->params[0]] < reg[ins->params[2]]);
			else if (ins->params[1] == 10 && reg[ins->params[0]] > reg[ins->params[2]]);
			else if (ins->params[1] == 11 && reg[ins->params[0]] <= reg[ins->params[2]]);
			else if (ins->params[1] == 12 && reg[ins->params[0]] >= reg[ins->params[2]]);
			else
				i = ins->params[3] - 2;
			break;
		case 13:
			printf("%d\n", reg[ins->params[0]]);
			break;
		case 14:
			scanf("%d", &reg[ins->params[0]]);
			break;
		}
	}
	_getch();
}

void printinstructiontable(instructionTable *table)
{
	printf("\n\n");
	int i = 0;
	printf("label PC");
	labeltabel *l = table->label;
	for (i = 0; i < l->count; i++)
		printf("%s  %d\n", l->lbl[i].str, l->lbl[i].value);
	printf("\n\n");
	printf("ln opcode params\n");
	for (int i = 0; i < table->count; i++)
	{
		printf("%2d %6d ", i + 1, table->ins[i]->opcode);
		for (int j = 0; j < table->ins[i]->param_count; j++)
			printf("%3d", table->ins[i]->params[j]);
		printf("\n");
	}
	_getch();
}

void savetofile(FILE *f, instructionTable *inst, symbolTable *symt)
{
	fprintf(f,"%d %d\n", symt->memory, symt->consts);

	for (int i = 0; i < symt->count; i++)
	{
		if (symt->symbols[i]->isConst)
		{
			fprintf(f, "%d %d\n", symt->symbols[i]->base, symt->symbols[i]->value);
		}
	}
	fprintf(f,"%d\n", inst->count);
	for (int i = 0; i < inst->count; i++)
	{
		fprintf(f,"%d %d", inst->ins[i]->opcode, inst->ins[i]->param_count);
		for (int j = 0; j < inst->ins[i]->param_count; j++)
			fprintf(f," %d", inst->ins[i]->params[j]);
		fprintf(f,"\n");
	}
}

void loadinstructions(FILE *f, instructionTable **insta, symbolTable **symta)
{
	instructionTable *inst = (instructionTable *)malloc(sizeof(instructionTable));
	symbolTable *symt = (symbolTable *)malloc(sizeof(symbolTable));
	*insta = inst;
	*symta = symt;
	fscanf(f, "%d%d", &symt->memory, &symt->consts);
	symt->count = symt->consts;
	symt->symbols = (symbol **)malloc(sizeof(symbol*)*symt->count);
	for (int i = 0;i< symt->count; i++)
	{
		symbol *sym = (symbol *)malloc(sizeof(symbol));
		fscanf(f,"%d%d", &sym->base, &sym->value);
		symt->symbols[i] = sym;
	}
	fscanf(f, "%d", &inst->count);
	inst->ins = (instruction **)malloc(sizeof(instruction*)* inst->count);
	for (int i = 0; i < inst->count; i++)
	{
		instruction *ins = (instruction *)malloc(sizeof(instruction));
		fscanf(f, "%d%d", &ins->opcode, &ins->param_count);
		ins->params = (int *)malloc(sizeof(int)*ins->param_count);
		for (int j = 0; j < ins->param_count; j++)
			fscanf(f, "%d", &ins->params[j]);
		inst->ins[i] = ins;
	}
}

int main()
{
	symbolTable* symtable = 0;
	instructionTable *instable = 0;
	FILE *fp;
	while (1)
	{
		printf("1. compile file\n 2. execute\n3. save intermediate code to file\n4. load instermediate\n0. exit\n");
		int c;
		scanf("%d", &c);
		char *filename = (char *)malloc(sizeof(char) * 200);
		switch (c)
		{
		case 0:
			goto FLAG;
			break;
		case 1:
			fp = 0;
			while (!fp) {
				system("cls");
				printf("enter file name:");
				getline(stdin, &filename);
				fp = fopen(filename, "r");
			}
			symtable = getSymbols(fp);
			printsymboltable(symtable);
			instable = loadinstructions(fp, symtable);
			printinstructiontable(instable);
			fclose(fp);
			break;
		case 2:
			runinstructions(instable, symtable);
			break;
		case 3:
			fp = NULL;
			while (fp == NULL) {
				printf("enter file name:");
				scanf("%s", filename);
				fp = fopen(filename, "w");
			}
			savetofile(fp,instable, symtable);
			fclose(fp);
			break;
		case 4:
			fp = NULL;
			while (fp == NULL)
			{
				printf("enter file name:");
				scanf("%s", filename);
				fp = fopen(filename, "r");
			}
			loadinstructions(fp, &instable, &symtable);
			break;
		}
	}
FLAG:
	_getch();
	return 0;
}