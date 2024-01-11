#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define FNAME_LEN 32
#define LNAME_LEN 64
#define ADDR_LEN 128
#define PESEL_LEN 12

typedef struct {
    int account_number;

    struct {
        char first_name[FNAME_LEN];
        char last_name[LNAME_LEN];
        char address[ADDR_LEN];
        char PESEL[PESEL_LEN];
    } personal_info;

    struct {
        // money is stored in grosze
        long long int grosze;
        long long int savings_grosze;
    } balance;
} UserAccount;

void ignore_chars() {
    char c = getchar();
    while (c != '\n' && c != EOF && !feof(stdin))
        c = getchar();
}

bool confirm_action() {
    char choice;
    printf("Are you sure? (y/n): ");
    while (scanf(" %c", &choice) != 1 || (choice != 'y' && choice != 'n')) {
        ignore_chars();
        printf("Invalid input. Please enter 'y' or 'n': ");
    }

    return (choice == 'y');
}

void initialize_customer_struct(UserAccount *ua) {
	memset(ua,0,sizeof(UserAccount));
}

void print_account(const UserAccount *ua) {
    assert(ua != NULL);

    printf("Account number: %d\n", ua->account_number);
    printf("    First name: %s\n", ua->personal_info.first_name);
    printf("     Last name: %s\n", ua->personal_info.last_name);
    printf("       Address: %s\n", ua->personal_info.address);
    printf("         PESEL: %s\n", ua->personal_info.PESEL);
    printf("       Balance:\n");
    long long int zlote = ua->balance.grosze / 100;
    long long int grosze = ua->balance.grosze % 100;
    printf("\t\tZlote: %lld\n", zlote);
    printf("\t\tGrosze: %lld\n", grosze);
    printf("       Savings balance:\n");
    long long int zlote_savings = ua->balance.savings_grosze / 100;
    long long int grosze_savings = ua->balance.savings_grosze % 100;
    printf("\t\tZlote: %lld\n", zlote_savings);
    printf("\t\tGrosze: %lld\n", grosze_savings);

    printf("----------------------------------------------------\n");
}

void print_all_accounts(FILE *in) {
    assert(in != NULL);

    UserAccount account = {0};
    rewind(in);
    while (fread(&account, sizeof(UserAccount), 1, in) == 1) {
        print_account(&account);
    }
}

// cyfry rowno 11
int is_valid_pesel(const char *pesel) {
    size_t len = strlen(pesel);

    for (size_t i = 0; i < len; i++) {
        if (!isdigit(pesel[i])) {
            return 0;
        }
    }

    return 1;
}

UserAccount create_new_account(int account_number) {
    UserAccount new_account;
    initialize_customer_struct(&new_account);

    new_account.account_number = account_number;

    printf("Enter first name: ");
    while (scanf("%32s%*[^\n]", new_account.personal_info.first_name) != 1 ||
           strlen(new_account.personal_info.first_name) >= 32) {
        ignore_chars();
        printf("Invalid input. Please enter a valid first name: ");
    }

    printf("Enter last name: ");
    while (scanf("%64s%*[^\n]", new_account.personal_info.last_name) != 1 ||
           strlen(new_account.personal_info.last_name) >= 64) {
        ignore_chars();
        printf("Invalid input. Please enter a valid last name: ");
    }

    printf("Enter address: ");
    while (scanf("%128s%*[^\n]", new_account.personal_info.address) != 1 ||
           strlen(new_account.personal_info.address) >= 128) {
        ignore_chars();
        printf("Invalid input. Please enter a valid address: ");
    }

    printf("Enter PESEL: ");
    while (scanf("%12s%*[^\n]", new_account.personal_info.PESEL) != 1 ||
           strlen(new_account.personal_info.PESEL) != 11 || !is_valid_pesel(new_account.personal_info.PESEL)) {
        ignore_chars();
        printf("Invalid input. Please enter a valid PESEL: ");
    }


    new_account.balance.grosze = 0;
    new_account.balance.savings_grosze = 0;

    return new_account;
}

void search_account(FILE *in) {
    assert(in != NULL);

    int search_option;
    printf("Search account by:\n");
    printf("1. Account number\n");
    printf("2. First name\n");
    printf("3. Last name\n");
    printf("4. PESEL\n");
    printf("Enter your choice (1-4): ");
    while (scanf("%d", &search_option) != 1 || (search_option < 1 || search_option > 4)) {
        ignore_chars();
        printf("Invalid input. Please enter a valid choice (1-4): ");
    }

    UserAccount search_key = {0};
    switch (search_option) {
        case 1:
            printf("Enter the account number: ");
            while (scanf("%d", &search_key.account_number) != 1) {
                ignore_chars();
                printf("Invalid input. Please enter a valid account number: ");
            }
            break;
        case 2:
            printf("Enter the first name: ");
            while (scanf("%32s%*[^\n]", search_key.personal_info.first_name) != 1) {
                ignore_chars();
                printf("Invalid input. Please enter a valid first name: ");
            }
            break;
        case 3:
            printf("Enter the last name: ");
            while (scanf("%64s%*[^\n]", search_key.personal_info.last_name) != 1) {
                ignore_chars();
                printf("Invalid input. Please enter a valid last name: ");
            }
            break;
        case 4:
            printf("Enter the PESEL: ");
            while (scanf("%12s%*[^\n]", search_key.personal_info.PESEL) != 1 ||
                   !is_valid_pesel(search_key.personal_info.PESEL)) {
                ignore_chars();
                printf("Invalid input. Please enter a valid PESEL: ");
            }
            break;
    }

    UserAccount account = {0};
    int found = 0;
    rewind(in);
    while (fread(&account, sizeof(UserAccount), 1, in) == 1) {
        int match = 0;
        switch (search_option) {
            case 1:
                match = (account.account_number == search_key.account_number);
                break;
            case 2:
                match = (strcmp(account.personal_info.first_name, search_key.personal_info.first_name) == 0);
                break;
            case 3:
                match = (strcmp(account.personal_info.last_name, search_key.personal_info.last_name) == 0);
                break;
            case 4:
                match = (strcmp(account.personal_info.PESEL, search_key.personal_info.PESEL) == 0);
                break;
        }

        if (match) {
            print_account(&account);
            found = 1;
        }
    }

    if (!found) {
        printf("No matching account found.\n");
    }
}

void transfer_money(FILE *file) {
    assert(file != NULL);

    int sender_account_number;
    printf("Enter the account number of the sender: ");
    while (scanf("%d", &sender_account_number) != 1) {
        ignore_chars();
        printf("Invalid input. Please enter a valid account number: ");
    }

    UserAccount sender_account = {0};
    int sender_found = 0;
    rewind(file);
    while (fread(&sender_account, sizeof(UserAccount), 1, file) == 1) {
        if (sender_account.account_number == sender_account_number) {
            sender_found = 1;
            break;
        }
    }

    int sender_pos = 0;
    if (sender_found)
        sender_pos = ftell(file) - sizeof(UserAccount);

    if (!sender_found) {
        printf("Sender account not found.\n");
        return;
    }

    int transfer_option;
    printf("Transfer from:\n");
    printf("1. Regular account\n");
    printf("2. Savings account\n");
    printf("Enter your choice (1-2): ");
    while (scanf("%1d%*[^\n]", &transfer_option) != 1 || (transfer_option < 1 || transfer_option > 2)) {
        ignore_chars();
        printf("Invalid input. Please enter a valid choice (1-2): ");
    }

    int receiver_account_number;
    printf("Enter the account number of the receiver: ");
    while (scanf("%d", &receiver_account_number) != 1) {
        ignore_chars();
        printf("Invalid input. Please enter a valid account number: ");
    }

    UserAccount receiver_account = {0};
    int receiver_found = 0;
    rewind(file);
    while (fread(&receiver_account, sizeof(UserAccount), 1, file) == 1) {
        if (receiver_account.account_number == receiver_account_number) {
            receiver_found = 1;
            break;
        }
    }

    int receiver_pos = 0;
    if (receiver_found)
        receiver_pos = ftell(file) - sizeof(UserAccount);

    if (!receiver_found) {
        printf("Receiver account not found.\n");
        return;
    }

    int transfer_option_receiver;
    printf("Transfer to:\n");
    printf("1. Regular account\n");
    printf("2. Savings account\n");
    printf("Enter your choice (1-2): ");
    while (scanf("%1d%*[^\n]", &transfer_option_receiver) != 1 ||
           (transfer_option_receiver < 1 || transfer_option_receiver > 2)) {
        ignore_chars();
        printf("Invalid input. Please enter a valid choice (1-2): ");
    }

    printf("Enter transfer amount (in zlote.grosze): ");
    long long int zlote;
    long long int grosze;
    while (scanf("%lld%*[.,]%lld", &zlote, &grosze) != 2 || zlote < 0 || grosze < 0) {
        ignore_chars();
        printf("Invalid input. Please enter a valid balance (in zlote.grosze): ");
    }

    long long int amount = zlote * 100 + grosze;

    if (!confirm_action()) {
        printf("Transfer canceled.\n");
        return;
    }

    if (transfer_option == 1 && transfer_option_receiver == 1) {
        if (sender_account.balance.grosze < amount) {
            printf("Insufficient funds in the sender's account.\n");
            return;
        }
        sender_account.balance.grosze -= amount;
        receiver_account.balance.grosze += amount;
    } else if (transfer_option == 2 && transfer_option_receiver == 1) {
        if (sender_account.balance.savings_grosze < amount) {
            printf("Insufficient funds in the sender's account.\n");
            return;
        }
        sender_account.balance.savings_grosze -= amount;
        receiver_account.balance.grosze += amount;

    } else if (transfer_option == 1 && transfer_option_receiver == 2) {
        if (sender_account.balance.grosze < amount) {
            printf("Insufficient funds in the sender's account.\n");
            return;
        }
        sender_account.balance.grosze -= amount;
        receiver_account.balance.savings_grosze += amount;
    } else if (transfer_option == 2 && transfer_option_receiver == 2) {
        if (sender_account.balance.savings_grosze < amount) {
            printf("Insufficient funds in the sender's account.\n");
            return;
        }
        sender_account.balance.savings_grosze -= amount;
        receiver_account.balance.savings_grosze += amount;
    }


    fseek(file, sender_pos, SEEK_SET);
    fwrite(&sender_account, sizeof(UserAccount), 1, file);

    fseek(file, receiver_pos, SEEK_SET);
    fwrite(&receiver_account, sizeof(UserAccount), 1, file);

    printf("Transfer successful.\n");
}

void deposit(FILE *file) {
    assert(file != NULL);

    int sender_account_number;
    printf("Enter the account number of the account you want to deposit the money to: ");
    while (scanf("%d", &sender_account_number) != 1) {
        ignore_chars();
        printf("Invalid input. Please enter a valid account number: ");
    }

    UserAccount sender_account = {0};
    int sender_found = 0;
    rewind(file);
    while (fread(&sender_account, sizeof(UserAccount), 1, file) == 1) {
        if (sender_account.account_number == sender_account_number) {
            sender_found = 1;
            break;
        }
    }

    if (!sender_found) {
        printf("Sender account not found.\n");
        return;
    }

    int transfer_option;
    printf("Deposit to:\n");
    printf("1. Regular account\n");
    printf("2. Savings account\n");
    printf("Enter your choice (1-2): ");
    while (scanf("%1d%*[^\n]", &transfer_option) != 1 || (transfer_option < 1 || transfer_option > 2)) {
        ignore_chars();
        printf("Invalid input. Please enter a valid choice (1-2): ");
    }

    printf("Enter deposit amount (in zlote.grosze): ");
    long long int zlote;
    long long int grosze;
    while (scanf("%lld%*[.,]%lld", &zlote, &grosze) != 2 || zlote < 0 || grosze < 0) {
        ignore_chars();
        printf("Invalid input. Please enter a valid amount (in zlote.grosze): ");
    }

    if (!confirm_action()) {
        printf("Transfer canceled.\n");
        return;
    }

    long long int amount = zlote * 100 + grosze;

    if (transfer_option == 1) {
        sender_account.balance.grosze += amount;
    } else if (transfer_option == 2) {
        sender_account.balance.savings_grosze += amount;
    }

    fseek(file, -sizeof(UserAccount), SEEK_CUR);
    fwrite(&sender_account, sizeof(UserAccount), 1, file);

    printf("Deposit successful.\n");
}

void withdrawal(FILE *file) {
    assert(file != NULL);

    int sender_account_number;
    printf("Enter the account number of the account you want to withdraw from: ");
    while (scanf("%d", &sender_account_number) != 1) {
        ignore_chars();
        printf("Invalid input. Please enter a valid account number: ");
    }

    UserAccount sender_account = {0};
    int sender_found = 0;
    rewind(file);
    while (fread(&sender_account, sizeof(UserAccount), 1, file) == 1) {
        if (sender_account.account_number == sender_account_number) {
            sender_found = 1;
            break;
        }
    }

    if (!sender_found) {
        printf("Account not found.\n");
        return;
    }

    char transfer_option;
    printf("Withdraw from:\n");
    printf("1. Regular account\n");
    printf("2. Savings account\n");
    printf("Enter your choice (1-2): ");
    while (scanf("%c%*[^\n]", &transfer_option) != 1 || (transfer_option != '1' && transfer_option != '2')) {
        ignore_chars();
        printf("Invalid input. Please enter a valid choice (1-2): ");
    }

    printf("Enter withdrawal amount (in zlote.grosze): ");
    long long int zlote;
    long long int grosze;
    while (scanf("%lld%*[.,]%lld", &zlote, &grosze) != 2 || zlote < 0 || grosze < 0) {
        ignore_chars();
        printf("Invalid input. Please enter a valid amount (in zlote.grosze): ");
    }

    if (!confirm_action()) {
        printf("Transfer canceled.\n");
        return;
    }

    long long int amount = zlote * 100 + grosze;


    if (transfer_option == '1') {
        if (sender_account.balance.grosze < amount) {
            printf("Insufficient funds on the account.\n");
            return;
        }
        sender_account.balance.grosze -= amount;
    } else if (transfer_option == '2') {
        if (sender_account.balance.savings_grosze < amount) {
            printf("Insufficient funds on the account.\n");
            return;
        }
        sender_account.balance.savings_grosze -= amount;
    }


    fseek(file, -sizeof(UserAccount), SEEK_CUR);
    fwrite(&sender_account, sizeof(UserAccount), 1, file);

    printf("Withdrawal successful.\n");
}


int main() {
    FILE *file1 = fopen("bank.db","a+");
    fclose(file1);
    FILE *file = fopen("bank.db", "r+b");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return 1;
    }

    int choice;
    do {
        printf("Bank Account System\n");
        printf("--------------------\n");
        printf("1. Create new account\n");
        printf("2. Search account\n");
        printf("3. Transfer money\n");
        printf("4. Print all accounts\n");
        printf("5. Deposit\n");
        printf("6. Withdrawal\n");
        printf("0. Exit\n");
        printf("Enter your choice (0-6): ");
        while (scanf("%d", &choice) != 1 || (choice < 0 || choice > 6)) {
            ignore_chars();
            printf("Invalid input. Please enter a valid choice (0-6): ");
        }

        switch (choice) {
            case 0:
                printf("Exiting...\n");
                break;
            case 1: {
                int max_account_number = 0;
                {
                    rewind(file);
                    //UserAccount tmp;
                    /*
                    while (fread(&tmp, sizeof(UserAccount), 1, file) == 1) {
                        if (tmp.account_number > max_account_number)
                            max_account_number = tmp.account_number;
                    }
                    */
                    if(fseek(file, 0L, SEEK_END))
                    {
			perror("Error: ");
			exit(2);
		    }
		    long int res = ftell(file);
		    max_account_number = res/sizeof(UserAccount);
                }
                UserAccount new_account = create_new_account(max_account_number + 1);
                fseek(file, 0, SEEK_END);
                fwrite(&new_account, sizeof(UserAccount), 1, file);
                printf("Account created successfully.\n");
                break;
            }
            case 2:
                search_account(file);
                break;
            case 3:
                transfer_money(file);
                break;
            case 4:
                print_all_accounts(file);
                break;
            case 5:
                deposit(file);
                break;
            case 6:
                withdrawal(file);
                break;
        }

        printf("\n");
    } while (choice != 0);

    fclose(file);
    return 0;
}




