![O_V_C](Images\O_V_C++.jpg)
# Description
Generating or Scanning matrix barcodes with my own encryption and decryption algorithm.
# Project Structure
Containing three cpp files (Generator.cpp, Scanner.cpp, Main.cpp), this project was structured using OOP model.
# How To Use
## 1. Generator
The Generator uses console input for creating the QR code.
![Generator input](Images\Generator_input.jpg)
Output:
![Generator output](Images\Generator_output.jpg)
## 2. Scanner
 The Scanner is the second part of project, it reads the QR code from the camera using OpenCV. It has to be created by the Generator. 
![Maria Barcode](Images\Maria_Barcode.png)
## Decrypting the QR code
#### Every QR code has the following partition:
![Partition](Images\Partition.png)
- Datasets
- Data type
- Permanent corner
- Validation
### Datasets
![Dataset](Images\Dataset.jpg)
Datasets are transformed in binary type. It starts from left corner resumming every row.  
### Data type
![Data Type](Images\Data_type.jpg)
There are two types of data established: alphanumeric and number.
### Validation
![Validation](Images\Validation.jpg)
This process counts the number of "1" from the binary dataset (in this case, it is 18) and then fills in the section starting from the right-down corner with numbers powers of 2, the sum of those being equal with the number of "1".