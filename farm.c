#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_ENTRIES 100
#define FILENAME "farm_diary.csv"

// Structure to hold a single diary entry
typedef struct {
    char date[11];      // Format: YYYY-MM-DD
    char type[20];      // "sowing", "expense", "harvest"
    char description[100];
    double amount;      // For expenses (-) or harvest income (+)
    double quantity;    // Area for sowing, weight for harvest
    char unit[20];      // "Acre", "kg", etc.
} DiaryEntry;

// --- Function Prototypes ---
void addEntry();
void viewDiary();
void showSummary();
void saveEntryToFile(const DiaryEntry* entry);
int loadEntriesFromFile(DiaryEntry entries[]);
void printMenu();

// --- Main Function ---
int main() {
    int choice;
    while (1) {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            // Clear invalid input buffer
            while (getchar() != '\n');
            printf("\nInvalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1:
                addEntry();
                break;
            case 2:
                viewDiary();
                break;
            case 3:
                showSummary();
                break;
            case 4:
                printf("Exiting the Farm Diary. Goodbye!\n");
                return 0;
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
    }
    return 0;
}

// --- Function Definitions ---

// Prints the main menu
void printMenu() {
    printf("\n--- C-Based Farm Diary ---\n");
    printf("1. Add New Entry\n");
    printf("2. View All Entries\n");
    printf("3. Show Profit/Loss Summary\n");
    printf("4. Exit\n");
    printf("--------------------------\n");
    printf("Enter your choice: ");
}

// Handles the logic for adding a new entry
void addEntry() {
    DiaryEntry newEntry = {0}; // Initialize with zeros/nulls
    int typeChoice;

    printf("\n--- Add New Entry ---\n");
    printf("Select Entry Type:\n");
    printf("1. Sowing / Planting\n");
    printf("2. Expense\n");
    printf("3. Harvest / Yield\n");
    printf("Enter choice: ");
    scanf("%d", &typeChoice);

    // Get current date
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(newEntry.date, "%d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    printf("Date automatically set to: %s\n", newEntry.date);

    // Clear input buffer before reading strings
    while (getchar() != '\n');

    switch (typeChoice) {
        case 1: // Sowing
            strcpy(newEntry.type, "sowing");
            printf("Enter Crop Name: ");
            fgets(newEntry.description, sizeof(newEntry.description), stdin);
            newEntry.description[strcspn(newEntry.description, "\n")] = 0; // Remove newline
            printf("Enter Area: ");
            scanf("%lf", &newEntry.quantity);
            printf("Enter Unit (e.g., Acre, Bigha): ");
            scanf("%s", newEntry.unit);
            newEntry.amount = 0; // No monetary value for sowing
            break;
        case 2: // Expense
            strcpy(newEntry.type, "expense");
            printf("Enter Expense Description (e.g., Fertilizer): ");
            fgets(newEntry.description, sizeof(newEntry.description), stdin);
            newEntry.description[strcspn(newEntry.description, "\n")] = 0;
            printf("Enter Amount (BDT): ");
            scanf("%lf", &newEntry.amount);
            newEntry.quantity = 0;
            strcpy(newEntry.unit, "N/A");
            break;
        case 3: // Harvest
            strcpy(newEntry.type, "harvest");
            printf("Enter Crop Name: ");
            fgets(newEntry.description, sizeof(newEntry.description), stdin);
            newEntry.description[strcspn(newEntry.description, "\n")] = 0;
            printf("Enter Quantity: ");
            scanf("%lf", &newEntry.quantity);
            printf("Enter Unit (e.g., kg, Ton): ");
            scanf("%s", newEntry.unit);
            printf("Enter Total Sale Price (BDT): ");
            scanf("%lf", &newEntry.amount);
            break;
        default:
            printf("Invalid type choice.\n");
            return;
    }

    saveEntryToFile(&newEntry);
    printf("Entry saved successfully!\n");
}

// Displays all entries from the file
void viewDiary() {
    DiaryEntry entries[MAX_ENTRIES];
    int count = loadEntriesFromFile(entries);

    printf("\n--- My Farm Diary Log ---\n");
    if (count == 0) {
        printf("The diary is empty.\n");
        return;
    }

    // Print entries in reverse order to show newest first
    for (int i = count - 1; i >= 0; i--) {
        printf("----------------------------------------\n");
        printf("Date: %s\n", entries[i].date);
        if (strcmp(entries[i].type, "sowing") == 0) {
            printf("Event: Planted %s\n", entries[i].description);
            printf("Details: %.2f %s\n", entries[i].quantity, entries[i].unit);
        } else if (strcmp(entries[i].type, "expense") == 0) {
            printf("Event: Expense for %s\n", entries[i].description);
            printf("Amount: -%.2f BDT\n", entries[i].amount);
        } else if (strcmp(entries[i].type, "harvest") == 0) {
            printf("Event: Harvested %s\n", entries[i].description);
            printf("Details: %.2f %s\n", entries[i].quantity, entries[i].unit);
            printf("Income: +%.2f BDT\n", entries[i].amount);
        }
    }
    printf("----------------------------------------\n");
}

// Calculates and displays the financial summary
void showSummary() {
    DiaryEntry entries[MAX_ENTRIES];
    int count = loadEntriesFromFile(entries);
    double totalIncome = 0;
    double totalExpenses = 0;

    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i].type, "harvest") == 0) {
            totalIncome += entries[i].amount;
        } else if (strcmp(entries[i].type, "expense") == 0) {
            totalExpenses += entries[i].amount;
        }
    }

    double netProfit = totalIncome - totalExpenses;

    printf("\n--- Profit/Loss Summary ---\n");
    printf("Total Income:   %.2f BDT\n", totalIncome);
    printf("Total Expenses: %.2f BDT\n", totalExpenses);
    printf("---------------------------\n");
    printf("Net Profit:     %.2f BDT\n", netProfit);
    printf("---------------------------\n");
}

// Appends a single entry to the CSV file
void saveEntryToFile(const DiaryEntry* entry) {
    FILE *file = fopen(FILENAME, "a"); // "a" for append mode
    if (file == NULL) {
        printf("Error: Could not open file %s for writing.\n", FILENAME);
        return;
    }
    fprintf(file, "%s,%s,\"%s\",%.2f,%.2f,%s\n",
            entry->date,
            entry->type,
            entry->description,
            entry->amount,
            entry->quantity,
            entry->unit);
    fclose(file);
}

// Loads all entries from the CSV file into an array of structs
int loadEntriesFromFile(DiaryEntry entries[]) {
    FILE *file = fopen(FILENAME, "r");
    if (file == NULL) {
        // If file doesn't exist, it's not an error, just means no entries yet.
        return 0;
    }

    int count = 0;
    char line[256];
    while (fgets(line, sizeof(line), file) && count < MAX_ENTRIES) {
        // Using strtok to parse CSV. This is a simple parser.
        char* token;
        token = strtok(line, ","); if(token) strcpy(entries[count].date, token);
        token = strtok(NULL, ","); if(token) strcpy(entries[count].type, token);
        
        // Handling description that might be quoted
        char* desc_start = strchr(line, '\"');
        if (desc_start) {
            char* desc_end = strrchr(desc_start + 1, '\"');
            if (desc_end) {
                *desc_end = '\0';
                strcpy(entries[count].description, desc_start + 1);
                // Move strtok past the description
                token = strtok(desc_end + 2, ",");
            }
        } else {
             token = strtok(NULL, ","); if(token) strcpy(entries[count].description, token);
             token = strtok(NULL, ",");
        }
        
        if(token) entries[count].amount = atof(token);
        token = strtok(NULL, ","); if(token) entries[count].quantity = atof(token);
        token = strtok(NULL, "\n"); if(token) strcpy(entries[count].unit, token);
        
        count++;
    }

    fclose(file);
    return count;
}
