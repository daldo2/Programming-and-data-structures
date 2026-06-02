#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_STR 30
#define STR_SCAN "29"
#define MAX_PESEL 12
#define PESEL_SCAN "11"
#define MAX_REG 10
#define REG_SCAN "9"
#define MAX_BUF 100
#define INITIAL_BALANCE 1000.0

typedef struct {
    int account_number;
    char reg_number[MAX_REG];
    float price;
} InsuranceRecord;

typedef struct {
    int number;
    char name[MAX_STR];
    char surname[MAX_STR];
    char address[MAX_STR];
    char pesel[MAX_PESEL];
    float balance;
} Account;

char* get_input(char* buffer, int size, const char* label) {
    printf("%s: ", label);

	scanf(" %"STR_SCAN"[^\n]", buffer);
	while(getchar() != '\n'){
		while(getchar() != '\n');
		printf("Too long, please repeat\n");
		printf("%s: ", label);
		scanf(" %"STR_SCAN"[^\n]", buffer);
	}
	
    return buffer;
}

char* get_pesel() {
    static char pesel[MAX_PESEL];
    while (1) {
        printf("PESEL: ");
        scanf(" %" PESEL_SCAN "s", pesel);
		while(getchar() != '\n'){
			while(getchar() != '\n');
			printf("Invalid! Must be 11 digits.\n");
			printf("PESEL: ");
			scanf(" %" PESEL_SCAN "s", pesel);
		}
        if (strlen(pesel) == MAX_PESEL-1) {
            int flag = 1;
            for (int i = 0; i < MAX_PESEL-1; i++) {
                if (pesel[i] < '0' || pesel[i] > '9') {
                    flag = 0;
                    break;
                }
            }
            if (flag) return pesel;
        }
        printf("Invalid! Must be 11 digits.\n");
    }
}

int confirm() {
    char c;
    printf("Are you sure? (y/n): ");
    scanf(" %c", &c);
    getchar();
    return (c == 'y' || c == 'Y');
}

int get_account_count() {
    FILE* f = fopen("data.bin", "rb");
    if (!f) return 0;
    int count = 0;
    fread(&count, sizeof(int), 1, f);
    fclose(f);
    return count;
}

void update_account_count(int new_count) {
    FILE* f = fopen("data.bin", "rb+");
    if (!f) f = fopen("data.bin", "wb");
    rewind(f);
    fwrite(&new_count, sizeof(int), 1, f);
    fclose(f);
}

long access_account(int acc_num, Account* a) {
    FILE* f = fopen("data.bin", "rb");
    if (!f) return -1;
    int count;
    fread(&count, sizeof(int), 1, f);
    for (int i = 0; i < count; i++) {
        long offset = ftell(f);
        fread(a, sizeof(Account), 1, f);
        if (a->number == acc_num) {
            fclose(f);
            return offset;
        }
    }
    fclose(f);
    return -1;
}

void save_account_at(long offset, Account* a) {
    FILE* f = fopen("data.bin", "rb+");
    if (!f) return;
    fseek(f, offset, SEEK_SET);
    fwrite(a, sizeof(Account), 1, f);
    fclose(f);
}

int adjust_balance(int acc_num, float amount) {
    Account a;
    long off = access_account(acc_num, &a);
    if (off == -1) return 0;
    if (a.balance + amount < 0) return -1;
    a.balance += amount;
    save_account_at(off, &a);
    return 1;
}

void append_insurance_record(InsuranceRecord* ins) {
    int count = 0;
    FILE* f = fopen("insurances.bin", "rb");
    if (f) {
        fread(&count, sizeof(int), 1, f);
        fclose(f);
    }
    f = fopen("insurances.bin", "rb+");
    if (!f) {
        f = fopen("insurances.bin", "wb");
        fwrite(&count, sizeof(int), 1, f);
    }
    fseek(f, 0, SEEK_END);
    fwrite(ins, sizeof(InsuranceRecord), 1, f);
    rewind(f);
    count++;
    fwrite(&count, sizeof(int), 1, f);
    fclose(f);
}

int ignore_case(const char* record_str, const char* search_term) {
    char lower_record[MAX_BUF];
    char lower_search[MAX_BUF];
    int i;
    for (i = 0; record_str[i] && i < (MAX_BUF - 1); i++) {
        lower_record[i] = (char)tolower((unsigned char)record_str[i]);
    }
    lower_record[i] = '\0';
    for (i = 0; search_term[i] && i < (MAX_BUF - 1); i++) {
        lower_search[i] = (char)tolower((unsigned char)search_term[i]);
    }
    lower_search[i] = '\0';
    return strcmp(lower_record, lower_search) == 0;
}

int check_match(Account* a, int choice, const char* search) {
    if (choice == 1) return a->number == atoi(search);
    if (choice == 2) return ignore_case(a->name, search);
    if (choice == 3) return ignore_case(a->surname, search);
    if (choice == 4) return ignore_case(a->address, search);
    if (choice == 5) return ignore_case(a->pesel, search);
    return 0;
}

void print_account(Account* a) {
    printf("Number: %d\nName: %s\nSurname: %s\nAddress: %s\nPESEL: %s\nBalance: %.2f\n", 
            a->number, a->name, a->surname, a->address, a->pesel, a->balance);
    FILE* f = fopen("insurances.bin", "rb");
    if (f) {
        int ins_count;
        fread(&ins_count, sizeof(int), 1, f);
        InsuranceRecord ins;
        int found = 0;
        for (int i = 0; i < ins_count; i++) {
            fread(&ins, sizeof(InsuranceRecord), 1, f);
            if (ins.account_number == a->number) {
                if (!found) printf("Insurances:\n");
                printf(" - %s: %.2f\n", ins.reg_number, ins.price);
                found = 1;
            }
        }
        fclose(f);
    }
    printf("---\n");
}

void search_accounts() {
    int choice;
    char search[MAX_STR];
    printf("Search by: 1.Number 2.Name 3.Surname 4.Address 5.PESEL\nChoice: ");
    scanf("%d", &choice);
    get_input(search, MAX_STR, "Search term");
    FILE* f = fopen("data.bin", "rb");
    if (!f) return;
    int count, found = 0;
    fread(&count, sizeof(int), 1, f);
    Account a;
    for (int i = 0; i < count; i++) {
        fread(&a, sizeof(Account), 1, f);
        if (check_match(&a, choice, search)) {
            print_account(&a);
            found = 1;
        }
    }
    if (!found) printf("No matches found for '%s'.\n", search);
    fclose(f);
}

void list_accounts() {
    FILE* f = fopen("data.bin", "rb");
    if (!f) return;
    int count;
    fread(&count, sizeof(int), 1, f);
    Account a;
    for (int i = 0; i < count; i++) {
        fread(&a, sizeof(Account), 1, f);
        print_account(&a);
    }
    fclose(f);
}

int execute_transfer(int from, int to, float amt) {
    if (from == to) {
        printf("Error: Cannot transfer to same account.\n");
        return 0;
    }
    Account a_from, a_to;
    if (access_account(from, &a_from) == -1 || access_account(to, &a_to) == -1) {
        printf("Error: Missing account.\n");
        return 0;
    }
    if (a_from.balance < amt) {
        printf("Error: Low funds.\n");
        return -1;
    }
    if (!confirm()) return 0;
    adjust_balance(from, -amt);
    adjust_balance(to, amt);
    return 1;
}

void transfer() {
    int from, to;
    float amt;
    printf("From account: "); scanf("%d", &from);
    printf("To account: "); scanf("%d", &to);
    printf("Amount: "); scanf("%f", &amt);
		while(amt < 0){
			while(getchar() != '\n');
			printf("Must be possitive\n");
			printf("Amount: ");
			scanf("%f", &amt);
		}
    if (execute_transfer(from, to, amt)) {
        printf("Transaction successful.\n");
    }
}

int execute_insurance_logic(int acc_num, float price, const char* reg) {
    Account a = {0};
    if (access_account(acc_num, &a) == -1) return 0;
    if (a.balance < price) return -1;
    if (!confirm()) return 0;
    InsuranceRecord ins = {0};
    ins.account_number = acc_num;
    ins.price = price;
    strncpy(ins.reg_number, reg, MAX_REG-1);
    ins.reg_number[MAX_REG-1] = '\0';
    adjust_balance(acc_num, -price);
    append_insurance_record(&ins);
    return 1;
}

void add_insurance() {
    int acc_num;
    float price;
    char reg[MAX_REG];
    printf("Account number: "); scanf("%d", &acc_num);
    printf("Insurance Price: "); scanf("%f", &price);
    while(price < 0){
			while(getchar() != '\n');
			printf("Must be possitive\n");
			printf("Insurance Price:  ");
			scanf("%f", &price);
		}
    printf("Registration number: "); 
    scanf(" %" REG_SCAN "s", reg);
    
    
    int status = execute_insurance_logic(acc_num, price, reg);
    if (status == 1) printf("Deducted %.2f from account %d.\n", price, acc_num);
    else if (status == -1) printf("not enough funds!\n");
    else if (status == 0) printf("Failed or cancelled.\n");
}

void create_account() {
    Account a = {0};
    int count = get_account_count();
    get_input(a.name, MAX_STR, "Name");
    get_input(a.surname, MAX_STR, "Surname");
    get_input(a.address, MAX_STR, "Address");
    strcpy(a.pesel, get_pesel());
    a.number = count + 1;
    a.balance = INITIAL_BALANCE;
    if (!confirm()) return;
    FILE* f = fopen("data.bin", "ab");
    if (count == 0) {
        fclose(f);
        f = fopen("data.bin", "wb");
        int initial_count = 0;
        fwrite(&initial_count, sizeof(int), 1, f);
    }
    fseek(f, 0, SEEK_END);
    fwrite(&a, sizeof(Account), 1, f);
    fclose(f);
    update_account_count(count + 1);
    printf("Account added.\n");
}

void handle_balance_operation(int choice) {
    int acc; 
    float sum; 
    printf("Acc num: "); 
    scanf("%d", &acc); 
    printf("Sum: "); 
    scanf("%f", &sum);
    while(sum < 0){
			while(getchar() != '\n');
			printf("Must be possitive\n");
			printf("Sum: ");
			scanf("%f", &sum); 
	}
    if (choice == 5) sum = -sum;
    if (!confirm()) return;
    int result = adjust_balance(acc, sum);
    if (result == 1) printf("Success.\n");
    else if (result == 0) printf("Not found.\n");
    else printf("Insufficient funds.\n");
}

void menu() {
    printf("\n1.New 2.List 3.Search 4.Deposit 5.Withdraw 6.Transfer 7.Insurance 8.Exit\nChoice: ");
}

int main() {
    int choice;
    while (1) {
        menu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            continue;
        }
        if (choice == 1) create_account();
        else if (choice == 2) list_accounts();
        else if (choice == 3) search_accounts();
        else if (choice == 4 || choice == 5) handle_balance_operation(choice);
        else if (choice == 6) transfer();
        else if (choice == 7) add_insurance();
        else if (choice == 8) break;
    }
    return 0;
}
