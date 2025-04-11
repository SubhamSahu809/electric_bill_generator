# Electric Billing System

The Electric Billing System is a C-based application designed to manage customer information, calculate electricity usage, generate bills, track payments, and generate monthly reports. It is a comprehensive solution for managing electricity billing operations.

## Features

- **Customer Management**: Add, update, and search customer details.
- **Billing**: Generate bills based on tiered pricing and time-of-use rates.
- **Payment Tracking**: Record payments and view payment history.
- **Bill Comparison**: Compare current bills with previous months.
- **Energy Usage Alerts**: Generate alerts for high energy usage.
- **Bill Projection**: Estimate next month's bill based on usage trends.
- **Monthly Reports**: Generate detailed reports for billing and usage analysis.


## How to Run

1. **Compile the Code**:
   Use a C compiler like `gcc` to compile the source code:
   ```bash
   gcc -o bill bill.c

2. **Run the Program**: 
Execute the compiled program: ./bill

3. **Follow the Menu**: The program provides a menu-driven interface for managing customers, generating bills, and viewing reports.

**Dependencies**
A C compiler (e.g., GCC)
Standard C libraries (stdio.h, stdlib.h, string.h, time.h)

**Usage**
**Adding a Customer**
Enter customer details such as name, address, phone, email, and meter number.
Assign a customer type (Residential, Commercial, or Industrial).

**Generating a Bill**
Input the current meter reading and time-of-use usage.
The system calculates the bill amount based on tiered pricing and tax rates.

**Recording Payments**
Mark bills as paid and specify the payment method (e.g., Cash, Credit Card).

**Generating Reports**
Generate monthly reports summarizing customer activity, billing, and energy usage.

**Example Report**
An example report is generated in the output/ directory. It includes:

-Customer summary
-Billing summary
-Usage by customer type
-Time-of-use analysis
-Top 5 consumers
-Payment methods analysis


**Future Enhancements**
-Add support for exporting reports in CSV or PDF format.
-Implement a graphical user interface (GUI).
-Integrate with online payment systems.

**Author**
-Developed by Subham.
