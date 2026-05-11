#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RECORDS 100

// clientData structure definition
struct clientData {
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
};

// Prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchRecord(FILE *fPtr);
void transferFunds(FILE *fPtr);      // NEW
void bankTotalAssets(FILE *fPtr);    // NEW

int main(int argc, char *argv[]) {
    FILE *cfPtr;
    unsigned int choice;

    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL) {
        printf("Database not found. Creating a blank 'credit.dat' file...\n");
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL) {
            printf("Error: Could not create file.\n");
            exit(-1);
        }
        struct clientData blankClient = {0, "", "", 0.0};
        for (int i = 0; i < MAX_RECORDS; i++) {
            fwrite(&blankClient, sizeof(struct clientData), 1, cfPtr);
        }
        rewind(cfPtr);
    }

    // Changed condition to 8 to allow for more options
    while ((choice = enterChoice()) != 8) {
        switch (choice) {
            case 1: textFile(cfPtr); break;
            case 2: updateRecord(cfPtr); break;
            case 3: newRecord(cfPtr); break;
            case 4: deleteRecord(cfPtr); break;
            case 5: searchRecord(cfPtr); break;
            case 6: transferFunds(cfPtr); break;   // NEW
            case 7: bankTotalAssets(cfPtr); break; // NEW
            default: puts("Incorrect choice"); break;
        }
    }

    fclose(cfPtr);
    return 0;
}

// 1. Create formatted text file for printing
void textFile(FILE *readPtr) {
    FILE *writePtr;
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL) {
        puts("File 'accounts.txt' could not be opened.");
    } else {
        rewind(readPtr);
        fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
        while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1) {
            if (client.acctNum != 0) {
                fprintf(writePtr, "%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
            }
        }
        fclose(writePtr);
        printf("Exported to 'accounts.txt' successfully.\n");
    }
}

// 2. Update balance in record
void updateRecord(FILE *fPtr) {
    unsigned int account;
    double transaction;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - %d): ", MAX_RECORDS);
    scanf("%u", &account);

    if (account > 0 && account <= MAX_RECORDS) {
        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&client, sizeof(struct clientData), 1, fPtr);

        if (client.acctNum == 0) {
            printf("Account #%d has no information.\n", account);
        } else {
            printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
            printf("Enter charge (+) or payment (-): ");
            scanf("%lf", &transaction);
            client.balance += transaction;
            printf("Updated balance: %10.2f\n", client.balance);

            fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
        }
    }
}

// 3. Create and insert record
void newRecord(FILE *fPtr) {
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter new account number (1 - %d): ", MAX_RECORDS);
    scanf("%u", &accountNum);

    if (accountNum > 0 && accountNum <= MAX_RECORDS) {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&client, sizeof(struct clientData), 1, fPtr);

        if (client.acctNum != 0) {
            printf("Account #%d already contains information.\n", client.acctNum);
        } else {
            printf("Enter lastname, firstname, balance: ");
            scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance);
            client.acctNum = accountNum;

            fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
            fwrite(&client, sizeof(struct clientData), 1, fPtr);
            printf("Record created.\n");
        }
    }
}

// 4. Delete an existing record
void deleteRecord(FILE *fPtr) {
    struct clientData client;
    struct clientData blankClient = {0, "", "", 0.0};
    unsigned int accountNum;

    printf("Enter account number to delete (1 - %d): ", MAX_RECORDS);
    scanf("%u", &accountNum);

    if (accountNum > 0 && accountNum <= MAX_RECORDS) {
        fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&client, sizeof(struct clientData), 1, fPtr);

        if (client.acctNum == 0) {
            printf("Account %d does not exist.\n", accountNum);
        } else {
            fseek(fPtr, (accountNum - 1) * sizeof(struct clientData), SEEK_SET);
            fwrite(&blankClient, sizeof(struct clientData), 1, fPtr);
            printf("Account #%u deleted.\n", accountNum);
        }
    }
}

// 5. Search for a record
void searchRecord(FILE *fPtr) {
    unsigned int account;
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account number (1 - %d): ", MAX_RECORDS);
    scanf("%u", &account);

    if (account > 0 && account <= MAX_RECORDS) {
        fseek(fPtr, (account - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&client, sizeof(struct clientData), 1, fPtr);

        if (client.acctNum == 0) {
            printf("Account #%u is empty.\n", account);
        } else {
            printf("\n%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");
            printf("%-6d%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);
        }
    }
}

// 6. Transfer Funds (NEW)
void transferFunds(FILE *fPtr) {
    unsigned int fromAcc, toAcc;
    double amount;
    struct clientData source = {0}, dest = {0};

    printf("Enter source account number: "); scanf("%u", &fromAcc);
    printf("Enter destination account number: "); scanf("%u", &toAcc);
    printf("Enter amount to transfer: "); scanf("%lf", &amount);

    if (fromAcc > 0 && fromAcc <= MAX_RECORDS && toAcc > 0 && toAcc <= MAX_RECORDS) {
        // Read source account
        fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&source, sizeof(struct clientData), 1, fPtr);
        // Read destination account
        fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
        fread(&dest, sizeof(struct clientData), 1, fPtr);

        if (source.acctNum != 0 && dest.acctNum != 0) {
            if (source.balance >= amount) {
                source.balance -= amount;
                dest.balance += amount;

                // Update source in file
                fseek(fPtr, (fromAcc - 1) * sizeof(struct clientData), SEEK_SET);
                fwrite(&source, sizeof(struct clientData), 1, fPtr);
                // Update destination in file
                fseek(fPtr, (toAcc - 1) * sizeof(struct clientData), SEEK_SET);
                fwrite(&dest, sizeof(struct clientData), 1, fPtr);

                printf("Transfer successful! $%.2f moved from #%u to #%u.\n", amount, fromAcc, toAcc);
            } else {
                printf("Error: Insufficient funds in account #%u.\n", fromAcc);
            }
        } else {
            printf("Error: One or both accounts do not exist.\n");
        }
    }
}

// 7. Calculate total bank assets (NEW)
void bankTotalAssets(FILE *fPtr) {
    struct clientData client = {0};
    double total = 0.0;
    int count = 0;

    rewind(fPtr);
    while (fread(&client, sizeof(struct clientData), 1, fPtr) == 1) {
        if (client.acctNum != 0) {
            total += client.balance;
            count++;
        }
    }
    printf("\n--- BANK SUMMARY ---\n");
    printf("Total Active Accounts: %d\n", count);
    printf("Total Bank Balance:   $%.2f\n", total);
    printf("--------------------\n");
}

// Updated Choice Menu
unsigned int enterChoice(void) {
    unsigned int menuChoice;
    printf("\n1 - Export formatted text file\n"
           "2 - Update an account balance\n"
           "3 - Add a new account\n"
           "4 - Delete an account\n"
           "5 - Search for an account\n"
           "6 - Transfer funds between accounts\n"
           "7 - View total bank assets\n"
           "8 - End program\n? ");
    scanf("%u", &menuChoice);
    return menuChoice;
}