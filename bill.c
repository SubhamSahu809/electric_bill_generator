/**
 * Electric Bill Generation System
 * Features:
 * - Customer information management
 * - Usage calculation with tiered pricing
 * - Time-of-use billing
 * - Payment history tracking
 * - Bill comparison with previous months
 * - Energy usage alerts
 * - Estimated bill projection
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 
 #define MAX_CUSTOMERS 100
 #define MAX_HISTORY 12
 #define MAX_NAME_LENGTH 50
 #define MAX_ADDRESS_LENGTH 100
 #define FILENAME "customer_data.bin"
 
 typedef enum {
     RESIDENTIAL,
     COMMERCIAL,
     INDUSTRIAL
 } CustomerType;
 
 typedef struct {
     int day;
     int month;
     int year;
 } Date;
 
 typedef struct {
     float peak_hours;      // 2pm-8pm (higher rate)
     float off_peak_hours;  // 8pm-2pm (lower rate)
 } TimeOfUseUsage;
 
 typedef struct {
     int bill_id;
     Date bill_date;
     Date due_date;
     float meter_reading_start;
     float meter_reading_end;
     float total_usage;
     TimeOfUseUsage tou_usage;
     float amount;
     int is_paid;
     Date payment_date;
     char payment_method[20];
 } BillingInfo;
 
 typedef struct {
     int customer_id;
     char name[MAX_NAME_LENGTH];
     char address[MAX_ADDRESS_LENGTH];
     char phone[15];
     char email[50];
     CustomerType type;
     char meter_number[20];
     BillingInfo billing_history[MAX_HISTORY];
     int bill_count;
     Date connection_date;
     int is_active;
 } Customer;
 
 // Global variables
 Customer customers[MAX_CUSTOMERS];
 int customer_count = 0;
 
 // Rate structure
 typedef struct {
     CustomerType type;
     float base_charge;
     float tier1_rate;  // 0-100 units
     float tier2_rate;  // 101-300 units
     float tier3_rate;  // 301+ units
     float peak_rate;
     float off_peak_rate;
     float tax_rate;
 } RateStructure;
 
 RateStructure rates[] = {
     {RESIDENTIAL, 50.0, 3.5, 7.0, 10.0, 12.0, 5.0, 0.05},
     {COMMERCIAL, 100.0, 5.0, 8.5, 12.0, 15.0, 7.0, 0.07},
     {INDUSTRIAL, 200.0, 6.5, 10.0, 15.0, 18.0, 9.0, 0.09}
 };
 
 // Function prototypes
 void saveData();
 void loadData();
 void addCustomer();
 void displayCustomer(int index);
 void generateBill(int customer_index);
 void displayBill(int customer_index, int bill_index);
 void recordPayment(int customer_index, int bill_index);
 void showPaymentHistory(int customer_index);
 void compareWithPreviousBill(int customer_index);
 void projectNextBill(int customer_index);
 void generateEnergyUsageAlert(int customer_index);
 void generateReport();
 float calculateBillAmount(CustomerType type, float usage, TimeOfUseUsage tou_usage);
 Date getCurrentDate();
 Date addDaysToDate(Date date, int days);
 int findCustomerByMeterNumber(char *meter_number);
 void updateCustomerInfo(int customer_index);
 void showAllCustomers();
 void searchCustomer();
 void showMainMenu();
 
 int main() {
     int choice, customer_index, bill_index;
     char meter_number[20];
     
     loadData();
     
     while (1) {
         showMainMenu();
         printf("Enter your choice: ");
         scanf("%d", &choice);
         getchar(); // Consume newline character
         
         switch (choice) {
             case 1:
                 addCustomer();
                 break;
                 
             case 2:
                 printf("Enter meter number: ");
                 fgets(meter_number, 20, stdin);
                 meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
                 
                 customer_index = findCustomerByMeterNumber(meter_number);
                 if (customer_index != -1) {
                     displayCustomer(customer_index);
                 } else {
                     printf("Customer not found!\n");
                 }
                 break;
                 
             case 3:
                 printf("Enter meter number: ");
                 fgets(meter_number, 20, stdin);
                 meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
                 
                 customer_index = findCustomerByMeterNumber(meter_number);
                 if (customer_index != -1) {
                     generateBill(customer_index);
                 } else {
                     printf("Customer not found!\n");
                 }
                 break;
                 
             case 4:
                 printf("Enter meter number: ");
                 fgets(meter_number, 20, stdin);
                 meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
                 
                 customer_index = findCustomerByMeterNumber(meter_number);
                 if (customer_index != -1) {
                     if (customers[customer_index].bill_count > 0) {
                         displayBill(customer_index, customers[customer_index].bill_count - 1);
                     } else {
                         printf("No bills found for this customer!\n");
                     }
                 } else {
                     printf("Customer not found!\n");
                 }
                 break;
                 
             case 5:
                 printf("Enter meter number: ");
                 fgets(meter_number, 20, stdin);
                 meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
                 
                 customer_index = findCustomerByMeterNumber(meter_number);
                 if (customer_index != -1) {
                     if (customers[customer_index].bill_count > 0) {
                         printf("Enter bill index (0-%d): ", customers[customer_index].bill_count - 1);
                         scanf("%d", &bill_index);
                         getchar(); // Consume newline character
                         
                         if (bill_index >= 0 && bill_index < customers[customer_index].bill_count) {
                             recordPayment(customer_index, bill_index);
                         } else {
                             printf("Invalid bill index!\n");
                         }
                     } else {
                         printf("No bills found for this customer!\n");
                     }
                 } else {
                     printf("Customer not found!\n");
                 }
                 break;
                 
             case 6:
                 printf("Enter meter number: ");
                 fgets(meter_number, 20, stdin);
                 meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
                 
                 customer_index = findCustomerByMeterNumber(meter_number);
                 if (customer_index != -1) {
                     showPaymentHistory(customer_index);
                 } else {
                     printf("Customer not found!\n");
                 }
                 break;
                 
             case 7:
                 printf("Enter meter number: ");
                 fgets(meter_number, 20, stdin);
                 meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
                 
                 customer_index = findCustomerByMeterNumber(meter_number);
                 if (customer_index != -1) {
                     compareWithPreviousBill(customer_index);
                 } else {
                     printf("Customer not found!\n");
                 }
                 break;
                 
             case 8:
                 printf("Enter meter number: ");
                 fgets(meter_number, 20, stdin);
                 meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
                 
                 customer_index = findCustomerByMeterNumber(meter_number);
                 if (customer_index != -1) {
                     projectNextBill(customer_index);
                 } else {
                     printf("Customer not found!\n");
                 }
                 break;
                 
             case 9:
                 printf("Enter meter number: ");
                 fgets(meter_number, 20, stdin);
                 meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
                 
                 customer_index = findCustomerByMeterNumber(meter_number);
                 if (customer_index != -1) {
                     generateEnergyUsageAlert(customer_index);
                 } else {
                     printf("Customer not found!\n");
                 }
                 break;
                 
             case 10:
                 updateCustomerInfo(findCustomerByMeterNumber(meter_number));
                 break;
                 
             case 11:
                 showAllCustomers();
                 break;
                 
             case 12:
                 searchCustomer();
                 break;
                 
             case 13:
                 generateReport();
                 break;
                 
             case 0:
                 saveData();
                 printf("Thank you for using Electric Billing System. Goodbye!\n");
                 exit(0);
                 
             default:
                 printf("Invalid choice! Please try again.\n");
         }
         
         printf("\nPress Enter to continue...");
         getchar();
     }
     
     return 0;
 }
 
 void showMainMenu() {
     printf("\n========== ELECTRIC BILLING SYSTEM ==========\n");
     printf("1. Add New Customer\n");
     printf("2. View Customer Details\n");
     printf("3. Generate New Bill\n");
     printf("4. View Latest Bill\n");
     printf("5. Record Payment\n");
     printf("6. View Payment History\n");
     printf("7. Compare With Previous Bill\n");
     printf("8. Project Next Month's Bill\n");
     printf("9. Generate Energy Usage Alert\n");
     printf("10. Update Customer Information\n");
     printf("11. Show All Customers\n");
     printf("12. Search Customer\n");
     printf("13. Generate Monthly Report\n");
     printf("0. Exit\n");
     printf("============================================\n");
 }
 
 void saveData() {
     FILE *file = fopen(FILENAME, "wb");
     if (file == NULL) {
         printf("Error opening file for writing!\n");
         return;
     }
     
     fwrite(&customer_count, sizeof(int), 1, file);
     fwrite(customers, sizeof(Customer), customer_count, file);
     
     fclose(file);
     printf("Data saved successfully!\n");
 }
 
 void loadData() {
     FILE *file = fopen(FILENAME, "rb");
     if (file == NULL) {
         printf("No existing data found or error opening file!\n");
         return;
     }
     
     fread(&customer_count, sizeof(int), 1, file);
     fread(customers, sizeof(Customer), customer_count, file);
     
     fclose(file);
     printf("Data loaded successfully!\n");
 }
 
 Date getCurrentDate() {
     time_t t = time(NULL);
     struct tm *tm_info = localtime(&t);
     
     Date current_date;
     current_date.day = tm_info->tm_mday;
     current_date.month = tm_info->tm_mon + 1;
     current_date.year = tm_info->tm_year + 1900;
     
     return current_date;
 }
 
 Date addDaysToDate(Date date, int days) {
     struct tm timeinfo = {0};
     timeinfo.tm_year = date.year - 1900;
     timeinfo.tm_mon = date.month - 1;
     timeinfo.tm_mday = date.day + days;
     
     time_t rawtime = mktime(&timeinfo);
     struct tm *tm_info = localtime(&rawtime);
     
     Date new_date;
     new_date.day = tm_info->tm_mday;
     new_date.month = tm_info->tm_mon + 1;
     new_date.year = tm_info->tm_year + 1900;
     
     return new_date;
 }
 
 void addCustomer() {
     if (customer_count >= MAX_CUSTOMERS) {
         printf("Maximum number of customers reached!\n");
         return;
     }
     
     Customer new_customer;
     new_customer.customer_id = customer_count + 1001; // Starting from 1001
     new_customer.bill_count = 0;
     new_customer.is_active = 1;
     new_customer.connection_date = getCurrentDate();
     
     printf("Enter customer name: ");
     fgets(new_customer.name, MAX_NAME_LENGTH, stdin);
     new_customer.name[strcspn(new_customer.name, "\n")] = 0; // Remove newline
     
     printf("Enter address: ");
     fgets(new_customer.address, MAX_ADDRESS_LENGTH, stdin);
     new_customer.address[strcspn(new_customer.address, "\n")] = 0; // Remove newline
     
     printf("Enter phone number: ");
     fgets(new_customer.phone, 15, stdin);
     new_customer.phone[strcspn(new_customer.phone, "\n")] = 0; // Remove newline
     
     printf("Enter email: ");
     fgets(new_customer.email, 50, stdin);
     new_customer.email[strcspn(new_customer.email, "\n")] = 0; // Remove newline
     
     printf("Enter customer type (0-Residential, 1-Commercial, 2-Industrial): ");
     int type;
     scanf("%d", &type);
     getchar(); // Consume newline
     new_customer.type = (CustomerType)type;
     
     printf("Enter meter number: ");
     fgets(new_customer.meter_number, 20, stdin);
     new_customer.meter_number[strcspn(new_customer.meter_number, "\n")] = 0; // Remove newline
     
     customers[customer_count] = new_customer;
     customer_count++;
     
     printf("Customer added successfully! Customer ID: %d\n", new_customer.customer_id);
     saveData();
 }
 
 int findCustomerByMeterNumber(char *meter_number) {
     for (int i = 0; i < customer_count; i++) {
         if (strcmp(customers[i].meter_number, meter_number) == 0) {
             return i;
         }
     }
     return -1;
 }
 
 void displayCustomer(int index) {
     Customer c = customers[index];
     printf("\n------ Customer Details ------\n");
     printf("ID: %d\n", c.customer_id);
     printf("Name: %s\n", c.name);
     printf("Address: %s\n", c.address);
     printf("Phone: %s\n", c.phone);
     printf("Email: %s\n", c.email);
     printf("Meter Number: %s\n", c.meter_number);
     printf("Customer Type: %s\n", c.type == RESIDENTIAL ? "Residential" : (c.type == COMMERCIAL ? "Commercial" : "Industrial"));
     printf("Connection Date: %02d/%02d/%d\n", c.connection_date.day, c.connection_date.month, c.connection_date.year);
     printf("Active Status: %s\n", c.is_active ? "Active" : "Inactive");
     printf("Number of Bills: %d\n", c.bill_count);
     printf("-----------------------------\n");
 }
 
 float calculateBillAmount(CustomerType type, float usage, TimeOfUseUsage tou_usage) {
     RateStructure rate;
     
     // Find the appropriate rate structure
     for (int i = 0; i < 3; i++) {
         if (rates[i].type == type) {
             rate = rates[i];
             break;
         }
     }
     
     float amount = rate.base_charge;
     
     // Calculate based on regular usage tiers
     if (usage <= 100) {
         amount += usage * rate.tier1_rate;
     } else if (usage <= 300) {
         amount += 100 * rate.tier1_rate + (usage - 100) * rate.tier2_rate;
     } else {
         amount += 100 * rate.tier1_rate + 200 * rate.tier2_rate + (usage - 300) * rate.tier3_rate;
     }
     
     // Add time-of-use charges
     amount += tou_usage.peak_hours * rate.peak_rate;
     amount += tou_usage.off_peak_hours * rate.off_peak_rate;
     
     // Add tax
     amount += amount * rate.tax_rate;
     
     return amount;
 }
 
 void generateBill(int customer_index) {
     Customer *c = &customers[customer_index];
     
     if (c->bill_count >= MAX_HISTORY) {
         // Shift bills to make room for new one
         for (int i = 0; i < MAX_HISTORY - 1; i++) {
             c->billing_history[i] = c->billing_history[i + 1];
         }
         c->bill_count--;
     }
     
     int bill_index = c->bill_count;
     BillingInfo *bill = &c->billing_history[bill_index];
     
     bill->bill_id = c->customer_id * 100 + bill_index + 1;
     bill->bill_date = getCurrentDate();
     bill->due_date = addDaysToDate(bill->bill_date, 15); // Due in 15 days
     bill->is_paid = 0;
     
     float previous_reading = 0;
     if (bill_index > 0) {
         previous_reading = c->billing_history[bill_index - 1].meter_reading_end;
     }
     
     bill->meter_reading_start = previous_reading;
     
     printf("Enter current meter reading: ");
     scanf("%f", &bill->meter_reading_end);
     getchar(); // Consume newline
     
     bill->total_usage = bill->meter_reading_end - bill->meter_reading_start;
     
     printf("Enter peak hours usage (2pm-8pm): ");
     scanf("%f", &bill->tou_usage.peak_hours);
     getchar(); // Consume newline
     
     printf("Enter off-peak hours usage (8pm-2pm): ");
     scanf("%f", &bill->tou_usage.off_peak_hours);
     getchar(); // Consume newline
     
     // Calculate bill amount
     bill->amount = calculateBillAmount(c->type, bill->total_usage, bill->tou_usage);
     
     c->bill_count++;
     
     printf("Bill generated successfully!\n");
     displayBill(customer_index, bill_index);
     saveData();
 }
 
 void displayBill(int customer_index, int bill_index) {
     Customer c = customers[customer_index];
     BillingInfo bill = c.billing_history[bill_index];
     
     printf("\n========== ELECTRIC BILL ==========\n");
     printf("Bill ID: %d\n", bill.bill_id);
     printf("Date: %02d/%02d/%d\n", bill.bill_date.day, bill.bill_date.month, bill.bill_date.year);
     printf("Due Date: %02d/%02d/%d\n", bill.due_date.day, bill.due_date.month, bill.due_date.year);
     printf("Customer ID: %d\n", c.customer_id);
     printf("Name: %s\n", c.name);
     printf("Address: %s\n", c.address);
     printf("Meter Number: %s\n", c.meter_number);
     printf("Customer Type: %s\n", c.type == RESIDENTIAL ? "Residential" : (c.type == COMMERCIAL ? "Commercial" : "Industrial"));
     printf("-------------------------------\n");
     printf("Previous Reading: %.2f units\n", bill.meter_reading_start);
     printf("Current Reading: %.2f units\n", bill.meter_reading_end);
     printf("Total Consumption: %.2f units\n", bill.total_usage);
     printf("-------------------------------\n");
     printf("Peak Hours Usage (2pm-8pm): %.2f units\n", bill.tou_usage.peak_hours);
     printf("Off-Peak Hours Usage (8pm-2pm): %.2f units\n", bill.tou_usage.off_peak_hours);
     printf("-------------------------------\n");
     printf("Total Amount Due: $%.2f\n", bill.amount);
     printf("Payment Status: %s\n", bill.is_paid ? "Paid" : "Unpaid");
     
     if (bill.is_paid) {
         printf("Payment Date: %02d/%02d/%d\n", bill.payment_date.day, bill.payment_date.month, bill.payment_date.year);
         printf("Payment Method: %s\n", bill.payment_method);
     }
     
     printf("===============================\n");
 }
 
 void recordPayment(int customer_index, int bill_index) {
     BillingInfo *bill = &customers[customer_index].billing_history[bill_index];
     
     if (bill->is_paid) {
         printf("This bill is already paid!\n");
         return;
     }
     
     bill->is_paid = 1;
     bill->payment_date = getCurrentDate();
     
     printf("Enter payment method (Cash/Credit Card/Bank Transfer): ");
     fgets(bill->payment_method, 20, stdin);
     bill->payment_method[strcspn(bill->payment_method, "\n")] = 0; // Remove newline
     
     printf("Payment recorded successfully!\n");
     saveData();
 }
 
 void showPaymentHistory(int customer_index) {
     Customer c = customers[customer_index];
     
     printf("\n===== Payment History for %s =====\n", c.name);
     
     if (c.bill_count == 0) {
         printf("No payment history found!\n");
         return;
     }
     
     for (int i = 0; i < c.bill_count; i++) {
         BillingInfo bill = c.billing_history[i];
         printf("Bill ID: %d, Date: %02d/%02d/%d, Amount: $%.2f, Status: %s\n",
                bill.bill_id, bill.bill_date.day, bill.bill_date.month, bill.bill_date.year,
                bill.amount, bill.is_paid ? "Paid" : "Unpaid");
         
         if (bill.is_paid) {
             printf("  Payment Date: %02d/%02d/%d, Method: %s\n",
                    bill.payment_date.day, bill.payment_date.month, bill.payment_date.year,
                    bill.payment_method);
         }
     }
     
     printf("===================================\n");
 }
 
 void compareWithPreviousBill(int customer_index) {
     Customer c = customers[customer_index];
     
     if (c.bill_count < 2) {
         printf("Not enough bills for comparison!\n");
         return;
     }
     
     BillingInfo current = c.billing_history[c.bill_count - 1];
     BillingInfo previous = c.billing_history[c.bill_count - 2];
     
     float usage_diff = current.total_usage - previous.total_usage;
     float amount_diff = current.amount - previous.amount;
     float usage_diff_percent = (usage_diff / previous.total_usage) * 100;
     float amount_diff_percent = (amount_diff / previous.amount) * 100;
     
     printf("\n===== Bill Comparison =====\n");
     printf("Current Bill (%02d/%02d/%d): $%.2f, %.2f units\n",
            current.bill_date.day, current.bill_date.month, current.bill_date.year,
            current.amount, current.total_usage);
     
     printf("Previous Bill (%02d/%02d/%d): $%.2f, %.2f units\n",
            previous.bill_date.day, previous.bill_date.month, previous.bill_date.year,
            previous.amount, previous.total_usage);
     
     printf("---------------------------\n");
     printf("Usage Difference: %.2f units (%.2f%%)\n", usage_diff, usage_diff_percent);
     printf("Amount Difference: $%.2f (%.2f%%)\n", amount_diff, amount_diff_percent);
     printf("===========================\n");
     
     if (usage_diff_percent > 20) {
         printf("\nALERT: Your usage has increased by more than 20%% compared to last month!\n");
         printf("Tips to reduce consumption:\n");
         printf("1. Turn off lights when not in use\n");
         printf("2. Use energy-efficient appliances\n");
         printf("3. Reduce air conditioning usage\n");
         printf("4. Check for electrical leakages\n");
     }
 }
 
 void projectNextBill(int customer_index) {
     Customer c = customers[customer_index];
     
     if (c.bill_count == 0) {
         printf("No previous bill found for projection!\n");
         return;
     }
     
     BillingInfo last_bill = c.billing_history[c.bill_count - 1];
     
     // Get average increase if we have multiple bills
     float avg_usage_increase = 0;
     if (c.bill_count > 1) {
         float total_increase = 0;
         for (int i = 1; i < c.bill_count; i++) {
             total_increase += c.billing_history[i].total_usage - c.billing_history[i-1].total_usage;
         }
         avg_usage_increase = total_increase / (c.bill_count - 1);
     }
     
     float projected_usage = last_bill.total_usage + avg_usage_increase;
     
     // Assume same time-of-use distribution
     TimeOfUseUsage projected_tou;
     float tou_ratio = last_bill.total_usage > 0 ? 
                      (last_bill.tou_usage.peak_hours / last_bill.total_usage) : 0.3;
     
     projected_tou.peak_hours = projected_usage * tou_ratio;
     projected_tou.off_peak_hours = projected_usage * (1 - tou_ratio);
     
     float projected_amount = calculateBillAmount(c.type, projected_usage, projected_tou);
     
     printf("\n===== Next Month's Bill Projection =====\n");
     printf("Projected Usage: %.2f units\n", projected_usage);
     printf("Projected Amount: $%.2f\n", projected_amount);
     printf("---------------------------------------\n");
     printf("Last Month's Usage: %.2f units\n", last_bill.total_usage);
     printf("Last Month's Amount: $%.2f\n", last_bill.amount);
     printf("=======================================\n");
     
     // Provide energy-saving tips
     printf("\nEnergy-Saving Tips:\n");
     printf("1. Switch to LED bulbs to save up to 80%% on lighting costs\n");
     printf("2. Use smart power strips to eliminate phantom power usage\n");
     printf("3. Adjust your thermostat by 1-2 degrees to save up to 10%% on heating/cooling\n");
     printf("4. Shift energy-intensive activities to off-peak hours (8pm-2pm)\n");
 }
 
 void generateEnergyUsageAlert(int customer_index) {
     Customer c = customers[customer_index];
     
     if (c.bill_count == 0) {
         printf("No bills found for analysis!\n");
         return;
     }
     
     BillingInfo last_bill = c.billing_history[c.bill_count - 1];
     
     // Calculate average usage from all bills
     float total_usage = 0;
     for (int i = 0; i < c.bill_count; i++) {
         total_usage += c.billing_history[i].total_usage;
     }
     float avg_usage = total_usage / c.bill_count;
     
     printf("\n===== Energy Usage Analysis =====\n");
     printf("Customer: %s\n", c.name);
     printf("Meter Number: %s\n", c.meter_number);
     printf("Last Month's Usage: %.2f units\n", last_bill.total_usage);
     printf("Average Monthly Usage: %.2f units\n", avg_usage);
     
     if (c.bill_count > 1) {
         float monthly_change = ((last_bill.total_usage - c.billing_history[c.bill_count - 2].total_usage) 
                               / c.billing_history[c.bill_count - 2].total_usage) * 100;
         printf("Monthly Change: %.2f%%\n", monthly_change);
     }
     
     printf("-------------------------------\n");
     
     if (last_bill.total_usage > avg_usage * 1.2) {
         printf("ALERT: Your usage is %.2f%% above your average!\n", 
               ((last_bill.total_usage / avg_usage) - 1) * 100);
         
         printf("\nPossible causes of high consumption:\n");
         printf("1. Weather changes (heating/cooling)\n");
         printf("2. New appliances or electronic devices\n");
         printf("3. Increased occupancy\n");
         printf("4. Faulty appliances or electrical leakages\n");
         
         printf("\nRecommended actions:\n");
         printf("1. Schedule an energy audit\n");
         printf("2. Check for appliances left on standby\n");
         printf("3. Inspect for electrical leakages\n");
         printf("4. Consider smart home energy monitoring\n");
     } else if (last_bill.total_usage < avg_usage * 0.8) {
         printf("NOTICE: Your usage is %.2f%% below your average. Good job!\n", 
               (1 - (last_bill.total_usage / avg_usage)) * 100);
     } else {
         printf("Your usage is within normal range.\n");
     }
     
     // Time of use analysis
     float peak_percentage = (last_bill.tou_usage.peak_hours / last_bill.total_usage) * 100;
     printf("\nPeak Hours Usage: %.2f%% of total\n", peak_percentage);
     
     if (peak_percentage > 40) {
         printf("TIP: You can save money by shifting usage to off-peak hours (8pm-2pm).\n");
         printf("Activities to consider shifting:\n");
         printf("- Laundry\n");
         printf("- Dishwashing\n");
         printf("- Electric vehicle charging\n");
         printf("- Heavy machinery operation\n");
     }
     
     printf("===============================\n");
 }
 
 void updateCustomerInfo(int customer_index) {
    if (customer_index == -1) {
        printf("Enter meter number: ");
        char meter_number[20];
        fgets(meter_number, 20, stdin);
        meter_number[strcspn(meter_number, "\n")] = 0; // Remove newline
        
        customer_index = findCustomerByMeterNumber(meter_number);
        
        if (customer_index == -1) {
            printf("Customer not found!\n");
            return;
        }
    }
    
    Customer *c = &customers[customer_index];
    int choice;
    
    printf("\n===== Update Customer Information =====\n");
    printf("1. Update Name\n");
    printf("2. Update Address\n");
    printf("3. Update Phone\n");
    printf("4. Update Email\n");
    printf("5. Update Customer Type\n");
    printf("6. Change Active Status\n");
    printf("0. Back to Main Menu\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume newline
    
    switch (choice) {
        case 1:
            printf("Current Name: %s\n", c->name);
            printf("Enter new name: ");
            fgets(c->name, MAX_NAME_LENGTH, stdin);
            c->name[strcspn(c->name, "\n")] = 0; // Remove newline
            printf("Name updated successfully!\n");
            break;
            
        case 2:
            printf("Current Address: %s\n", c->address);
            printf("Enter new address: ");
            fgets(c->address, MAX_ADDRESS_LENGTH, stdin);
            c->address[strcspn(c->address, "\n")] = 0; // Remove newline
            printf("Address updated successfully!\n");
            break;
            
        case 3:
            printf("Current Phone: %s\n", c->phone);
            printf("Enter new phone: ");
            fgets(c->phone, 15, stdin);
            c->phone[strcspn(c->phone, "\n")] = 0; // Remove newline
            printf("Phone updated successfully!\n");
            break;
            
        case 4:
            printf("Current Email: %s\n", c->email);
            printf("Enter new email: ");
            fgets(c->email, 50, stdin);
            c->email[strcspn(c->email, "\n")] = 0; // Remove newline
            printf("Email updated successfully!\n");
            break;
            
        case 5:
            printf("Current Customer Type: %s\n", 
                   c->type == RESIDENTIAL ? "Residential" : 
                   (c->type == COMMERCIAL ? "Commercial" : "Industrial"));
            printf("Enter new customer type (0-Residential, 1-Commercial, 2-Industrial): ");
            int type;
            scanf("%d", &type);
            getchar(); // Consume newline
            c->type = (CustomerType)type;
            printf("Customer type updated successfully!\n");
            break;
            
        case 6:
            printf("Current Status: %s\n", c->is_active ? "Active" : "Inactive");
            printf("Change status to %s? (1-Yes, 0-No): ", c->is_active ? "Inactive" : "Active");
            int status;
            scanf("%d", &status);
            getchar(); // Consume newline
            if (status == 1) {
                c->is_active = !c->is_active;
                printf("Status updated successfully!\n");
            }
            break;
            
        case 0:
            return;
            
        default:
            printf("Invalid choice!\n");
    }
    
    saveData();
}

void showAllCustomers() {
    if (customer_count == 0) {
        printf("No customers found!\n");
        return;
    }
    
    printf("\n===== All Customers =====\n");
    printf("%-5s %-20s %-15s %-15s %-10s\n", "ID", "Name", "Meter Number", "Type", "Status");
    printf("---------------------------------------------------------------\n");
    
    for (int i = 0; i < customer_count; i++) {
        Customer c = customers[i];
        printf("%-5d %-20s %-15s %-15s %-10s\n", 
               c.customer_id, 
               c.name, 
               c.meter_number, 
               c.type == RESIDENTIAL ? "Residential" : (c.type == COMMERCIAL ? "Commercial" : "Industrial"),
               c.is_active ? "Active" : "Inactive");
    }
    
    printf("---------------------------------------------------------------\n");
    printf("Total Customers: %d\n", customer_count);
}

void searchCustomer() {
    if (customer_count == 0) {
        printf("No customers found!\n");
        return;
    }
    
    int choice;
    printf("\n===== Search Customer =====\n");
    printf("1. Search by Name\n");
    printf("2. Search by Meter Number\n");
    printf("3. Search by Customer ID\n");
    printf("4. Search by Phone\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar(); // Consume newline
    
    char search_term[MAX_NAME_LENGTH];
    int found = 0;
    
    switch (choice) {
        case 1:
            printf("Enter customer name: ");
            fgets(search_term, MAX_NAME_LENGTH, stdin);
            search_term[strcspn(search_term, "\n")] = 0; // Remove newline
            
            printf("\n--- Search Results ---\n");
            printf("%-5s %-20s %-15s %-15s %-10s\n", "ID", "Name", "Meter Number", "Type", "Status");
            printf("---------------------------------------------------------------\n");
            
            for (int i = 0; i < customer_count; i++) {
                if (strstr(customers[i].name, search_term) != NULL) {
                    Customer c = customers[i];
                    printf("%-5d %-20s %-15s %-15s %-10s\n", 
                           c.customer_id, 
                           c.name, 
                           c.meter_number, 
                           c.type == RESIDENTIAL ? "Residential" : (c.type == COMMERCIAL ? "Commercial" : "Industrial"),
                           c.is_active ? "Active" : "Inactive");
                    found++;
                }
            }
            break;
            
        case 2:
            printf("Enter meter number: ");
            fgets(search_term, MAX_NAME_LENGTH, stdin);
            search_term[strcspn(search_term, "\n")] = 0; // Remove newline
            
            printf("\n--- Search Results ---\n");
            printf("%-5s %-20s %-15s %-15s %-10s\n", "ID", "Name", "Meter Number", "Type", "Status");
            printf("---------------------------------------------------------------\n");
            
            for (int i = 0; i < customer_count; i++) {
                if (strstr(customers[i].meter_number, search_term) != NULL) {
                    Customer c = customers[i];
                    printf("%-5d %-20s %-15s %-15s %-10s\n", 
                           c.customer_id, 
                           c.name, 
                           c.meter_number, 
                           c.type == RESIDENTIAL ? "Residential" : (c.type == COMMERCIAL ? "Commercial" : "Industrial"),
                           c.is_active ? "Active" : "Inactive");
                    found++;
                }
            }
            break;
            
        case 3:
            printf("Enter customer ID: ");
            int search_id;
            scanf("%d", &search_id);
            getchar(); // Consume newline
            
            printf("\n--- Search Results ---\n");
            printf("%-5s %-20s %-15s %-15s %-10s\n", "ID", "Name", "Meter Number", "Type", "Status");
            printf("---------------------------------------------------------------\n");
            
            for (int i = 0; i < customer_count; i++) {
                if (customers[i].customer_id == search_id) {
                    Customer c = customers[i];
                    printf("%-5d %-20s %-15s %-15s %-10s\n", 
                           c.customer_id, 
                           c.name, 
                           c.meter_number, 
                           c.type == RESIDENTIAL ? "Residential" : (c.type == COMMERCIAL ? "Commercial" : "Industrial"),
                           c.is_active ? "Active" : "Inactive");
                    found++;
                }
            }
            break;
            
        case 4:
            printf("Enter phone number: ");
            fgets(search_term, MAX_NAME_LENGTH, stdin);
            search_term[strcspn(search_term, "\n")] = 0; // Remove newline
            
            printf("\n--- Search Results ---\n");
            printf("%-5s %-20s %-15s %-15s %-10s\n", "ID", "Name", "Meter Number", "Type", "Status");
            printf("---------------------------------------------------------------\n");
            
            for (int i = 0; i < customer_count; i++) {
                if (strstr(customers[i].phone, search_term) != NULL) {
                    Customer c = customers[i];
                    printf("%-5d %-20s %-15s %-15s %-10s\n", 
                           c.customer_id, 
                           c.name, 
                           c.meter_number, 
                           c.type == RESIDENTIAL ? "Residential" : (c.type == COMMERCIAL ? "Commercial" : "Industrial"),
                           c.is_active ? "Active" : "Inactive");
                    found++;
                }
            }
            break;
            
        default:
            printf("Invalid choice!\n");
            return;
    }
    
    printf("---------------------------------------------------------------\n");
    printf("Total Results: %d\n", found);
    
    if (found > 0) {
        printf("\nDo you want to view details of any customer? (1-Yes, 0-No): ");
        int view;
        scanf("%d", &view);
        getchar(); // Consume newline
        
        if (view == 1) {
            printf("Enter Customer ID: ");
            int id;
            scanf("%d", &id);
            getchar(); // Consume newline
            
            for (int i = 0; i < customer_count; i++) {
                if (customers[i].customer_id == id) {
                    displayCustomer(i);
                    return;
                }
            }
            
            printf("Customer with ID %d not found!\n", id);
        }
    }
}
void generateReport() {
    if (customer_count == 0) {
        printf("No customers found!\n");
        return;
    }
    
    Date current_date = getCurrentDate();
    char report_filename[50];
    sprintf(report_filename, "report_%02d_%02d_%d.txt", current_date.day, current_date.month, current_date.year);
    
    FILE *report_file = fopen(report_filename, "w");
    if (report_file == NULL) {
        printf("Error creating report file!\n");
        return;
    }
    
    // Report header
    fprintf(report_file, "===============================================\n");
    fprintf(report_file, "          ELECTRIC BILLING SYSTEM REPORT       \n");
    fprintf(report_file, "                %02d/%02d/%d                   \n", 
            current_date.day, current_date.month, current_date.year);
    fprintf(report_file, "===============================================\n\n");
    
    // Customer summary
    fprintf(report_file, "CUSTOMER SUMMARY\n");
    fprintf(report_file, "-----------------\n");
    fprintf(report_file, "Total Customers: %d\n", customer_count);
    
    int active_customers = 0;
    int residential = 0, commercial = 0, industrial = 0;
    
    for (int i = 0; i < customer_count; i++) {
        if (customers[i].is_active)
            active_customers++;
        
        switch (customers[i].type) {
            case RESIDENTIAL: residential++; break;
            case COMMERCIAL: commercial++; break;
            case INDUSTRIAL: industrial++; break;
        }
    }
    
    fprintf(report_file, "Active Customers: %d (%.1f%%)\n", 
            active_customers, (float)active_customers / customer_count * 100);
    fprintf(report_file, "Inactive Customers: %d (%.1f%%)\n", 
            customer_count - active_customers, (float)(customer_count - active_customers) / customer_count * 100);
    fprintf(report_file, "Customer Types:\n");
    fprintf(report_file, "  - Residential: %d (%.1f%%)\n", 
            residential, (float)residential / customer_count * 100);
    fprintf(report_file, "  - Commercial: %d (%.1f%%)\n", 
            commercial, (float)commercial / customer_count * 100);
    fprintf(report_file, "  - Industrial: %d (%.1f%%)\n\n", 
            industrial, (float)industrial / customer_count * 100);
    
    // Billing summary for current month
    fprintf(report_file, "BILLING SUMMARY FOR %02d/%d\n", current_date.month, current_date.year);
    fprintf(report_file, "------------------------\n");
    
    int bills_generated = 0;
    int bills_paid = 0;
    float total_billed_amount = 0;
    float total_collected_amount = 0;
    float total_outstanding_amount = 0;
    float total_usage = 0;
    
    for (int i = 0; i < customer_count; i++) {
        for (int j = 0; j < customers[i].bill_count; j++) {
            BillingInfo bill = customers[i].billing_history[j];
            
            // Check if the bill is from current month
            if (bill.bill_date.month == current_date.month && 
                bill.bill_date.year == current_date.year) {
                
                bills_generated++;
                total_billed_amount += bill.amount;
                total_usage += bill.total_usage;
                
                if (bill.is_paid) {
                    bills_paid++;
                    total_collected_amount += bill.amount;
                } else {
                    total_outstanding_amount += bill.amount;
                }
            }
        }
    }
    
    fprintf(report_file, "Bills Generated: %d\n", bills_generated);
    fprintf(report_file, "Bills Paid: %d (%.1f%%)\n", 
            bills_paid, bills_generated > 0 ? (float)bills_paid / bills_generated * 100 : 0);
    fprintf(report_file, "Bills Unpaid: %d (%.1f%%)\n", 
            bills_generated - bills_paid, 
            bills_generated > 0 ? (float)(bills_generated - bills_paid) / bills_generated * 100 : 0);
    fprintf(report_file, "Total Billed Amount: $%.2f\n", total_billed_amount);
    fprintf(report_file, "Total Collected Amount: $%.2f (%.1f%%)\n", 
            total_collected_amount, 
            total_billed_amount > 0 ? total_collected_amount / total_billed_amount * 100 : 0);
    fprintf(report_file, "Total Outstanding Amount: $%.2f (%.1f%%)\n", 
            total_outstanding_amount, 
            total_billed_amount > 0 ? total_outstanding_amount / total_billed_amount * 100 : 0);
    fprintf(report_file, "Total Energy Usage: %.2f units\n\n", total_usage);
    
    // Usage by customer type
    fprintf(report_file, "USAGE BY CUSTOMER TYPE\n");
    fprintf(report_file, "---------------------\n");
    
    float residential_usage = 0, commercial_usage = 0, industrial_usage = 0;
    float residential_amount = 0, commercial_amount = 0, industrial_amount = 0;
    
    for (int i = 0; i < customer_count; i++) {
        for (int j = 0; j < customers[i].bill_count; j++) {
            BillingInfo bill = customers[i].billing_history[j];
            
            // Check if the bill is from current month
            if (bill.bill_date.month == current_date.month && 
                bill.bill_date.year == current_date.year) {
                
                switch (customers[i].type) {
                    case RESIDENTIAL:
                        residential_usage += bill.total_usage;
                        residential_amount += bill.amount;
                        break;
                    case COMMERCIAL:
                        commercial_usage += bill.total_usage;
                        commercial_amount += bill.amount;
                        break;
                    case INDUSTRIAL:
                        industrial_usage += bill.total_usage;
                        industrial_amount += bill.amount;
                        break;
                }
            }
        }
    }
    
    fprintf(report_file, "Residential:\n");
    fprintf(report_file, "  - Usage: %.2f units (%.1f%%)\n", 
            residential_usage, 
            total_usage > 0 ? residential_usage / total_usage * 100 : 0);
    fprintf(report_file, "  - Amount: $%.2f (%.1f%%)\n", 
            residential_amount, 
            total_billed_amount > 0 ? residential_amount / total_billed_amount * 100 : 0);
    
    fprintf(report_file, "Commercial:\n");
    fprintf(report_file, "  - Usage: %.2f units (%.1f%%)\n", 
            commercial_usage, 
            total_usage > 0 ? commercial_usage / total_usage * 100 : 0);
    fprintf(report_file, "  - Amount: $%.2f (%.1f%%)\n", 
            commercial_amount, 
            total_billed_amount > 0 ? commercial_amount / total_billed_amount * 100 : 0);
    
    fprintf(report_file, "Industrial:\n");
    fprintf(report_file, "  - Usage: %.2f units (%.1f%%)\n", 
            industrial_usage, 
            total_usage > 0 ? industrial_usage / total_usage * 100 : 0);
    fprintf(report_file, "  - Amount: $%.2f (%.1f%%)\n\n", 
            industrial_amount, 
            total_billed_amount > 0 ? industrial_amount / total_billed_amount * 100 : 0);
    
    // Time of use analysis
    fprintf(report_file, "TIME OF USE ANALYSIS\n");
    fprintf(report_file, "-------------------\n");
    
    float peak_usage = 0, off_peak_usage = 0;
    
    for (int i = 0; i < customer_count; i++) {
        for (int j = 0; j < customers[i].bill_count; j++) {
            BillingInfo bill = customers[i].billing_history[j];
            
            // Check if the bill is from current month
            if (bill.bill_date.month == current_date.month && 
                bill.bill_date.year == current_date.year) {
                
                peak_usage += bill.tou_usage.peak_hours;
                off_peak_usage += bill.tou_usage.off_peak_hours;
            }
        }
    }
    
    fprintf(report_file, "Peak Hours Usage (2pm-8pm): %.2f units (%.1f%%)\n", 
            peak_usage, 
            total_usage > 0 ? peak_usage / total_usage * 100 : 0);
    fprintf(report_file, "Off-Peak Hours Usage (8pm-2pm): %.2f units (%.1f%%)\n\n", 
            off_peak_usage, 
            total_usage > 0 ? off_peak_usage / total_usage * 100 : 0);
    
    // Top consumers
    fprintf(report_file, "TOP 5 CONSUMERS\n");
    fprintf(report_file, "-------------\n");
    
    typedef struct {
        int customer_index;
        float usage;
        float amount;
    } ConsumerRanking;
    
    ConsumerRanking rankings[MAX_CUSTOMERS] = {0};
    int ranking_count = 0;
    
    for (int i = 0; i < customer_count; i++) {
        float monthly_usage = 0;
        float monthly_amount = 0;
        
        for (int j = 0; j < customers[i].bill_count; j++) {
            BillingInfo bill = customers[i].billing_history[j];
            
            // Check if the bill is from current month
            if (bill.bill_date.month == current_date.month && 
                bill.bill_date.year == current_date.year) {
                
                monthly_usage += bill.total_usage;
                monthly_amount += bill.amount;
            }
        }
        
        if (monthly_usage > 0) {
            rankings[ranking_count].customer_index = i;
            rankings[ranking_count].usage = monthly_usage;
            rankings[ranking_count].amount = monthly_amount;
            ranking_count++;
        }
    }
    
    // Sort rankings by usage
    for (int i = 0; i < ranking_count - 1; i++) {
        for (int j = 0; j < ranking_count - i - 1; j++) {
            if (rankings[j].usage < rankings[j + 1].usage) {
                ConsumerRanking temp = rankings[j];
                rankings[j] = rankings[j + 1];
                rankings[j + 1] = temp;
            }
        }
    }
    
    // Print top 5 (or less if there are fewer customers)
    int top_count = ranking_count < 5 ? ranking_count : 5;
    
    fprintf(report_file, "%-5s %-20s %-15s %-15s %-15s\n", 
            "Rank", "Customer Name", "Meter Number", "Usage (units)", "Amount ($)");
    fprintf(report_file, "----------------------------------------------------------------------\n");
    
    for (int i = 0; i < top_count; i++) {
        int idx = rankings[i].customer_index;
        fprintf(report_file, "%-5d %-20s %-15s %-15.2f %-15.2f\n", 
                i + 1, 
                customers[idx].name, 
                customers[idx].meter_number, 
                rankings[i].usage, 
                rankings[i].amount);
    }
    fprintf(report_file, "\n");
    
    // Payment methods analysis (for paid bills in current month)
    fprintf(report_file, "PAYMENT METHODS ANALYSIS\n");
    fprintf(report_file, "-----------------------\n");
    
    typedef struct {
        char method[20];
        int count;
        float amount;
    } PaymentMethodStats;
    
    PaymentMethodStats payment_stats[10] = {0}; // Assuming max 10 different payment methods
    int payment_methods_count = 0;
    
    for (int i = 0; i < customer_count; i++) {
        for (int j = 0; j < customers[i].bill_count; j++) {
            BillingInfo bill = customers[i].billing_history[j];
            
            // Check if the bill is paid and from current month
            if (bill.is_paid && 
                bill.payment_date.month == current_date.month && 
                bill.payment_date.year == current_date.year) {
                
                // Check if payment method already exists in stats
                int found = 0;
                for (int k = 0; k < payment_methods_count; k++) {
                    if (strcmp(payment_stats[k].method, bill.payment_method) == 0) {
                        payment_stats[k].count++;
                        payment_stats[k].amount += bill.amount;
                        found = 1;
                        break;
                    }
                }
                
                // If payment method not found, add it
                if (!found) {
                    strcpy(payment_stats[payment_methods_count].method, bill.payment_method);
                    payment_stats[payment_methods_count].count = 1;
                    payment_stats[payment_methods_count].amount = bill.amount;
                    payment_methods_count++;
                }
            }
        }
    }
    
    fprintf(report_file, "%-20s %-10s %-15s %-10s\n", 
            "Payment Method", "Count", "Amount ($)", "Percentage");
    fprintf(report_file, "------------------------------------------------------\n");
    
    for (int i = 0; i < payment_methods_count; i++) {
        fprintf(report_file, "%-20s %-10d %-15.2f %-10.1f%%\n", 
                payment_stats[i].method, 
                payment_stats[i].count, 
                payment_stats[i].amount,
                total_collected_amount > 0 ? payment_stats[i].amount / total_collected_amount * 100 : 0);
    }
    
    fprintf(report_file, "\n");
    fprintf(report_file, "===============================================\n");
    fprintf(report_file, "               END OF REPORT                   \n");
    fprintf(report_file, "===============================================\n");
    
    fclose(report_file);
    
    printf("Report generated successfully! Saved as %s\n", report_filename);
}