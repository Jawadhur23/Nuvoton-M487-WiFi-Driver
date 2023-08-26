# Nuvoton-M487-WiFi-Driver
This Project contains a UART-Driver for Implementing as a WiFi Driver in the Smart-Gate Project. In this, I Have Implemented the Ring Buffer Concept for Storing of Data and Respond to the Data

At the Initial Stage, I represented in the code Jawadhur whether data is able to write the Data and Read Data Correctly.
When we write the Data Jawadhur it will Validate and Read the Data It will be Carried out by the CPU Handler in the Ring Buffer of 256 Bits
The 256 Bytes Ring Buffer The data will be written in the Ring Buffer Once the Data is Read we can Overwrite in the Ring Buffer


![image](https://github.com/Jawadhur23/Nuvoton-M487-WiFi-Driver/assets/140182146/a1ce0065-4a32-4f5e-ae6a-cf079e2a4547)


![image](https://github.com/Jawadhur23/Nuvoton-M487-WiFi-Driver/assets/140182146/49c5ae26-728a-4fe1-8606-24423ce172a0)


At the Next Stage, I represented a Particular Key 
Let us Imagine that the Particular Key I Configured, The key is 'A' the Ring Buffer will validate the correct Data when a Data is written in Ring Buffer if it is wrong it will not accept the data
When we Press the Key 'A' it will Accept the Data In the Ring Buffer and the Data is Read Correctly!!.


![Screenshot (38)](https://github.com/Jawadhur23/Nuvoton-M487-WiFi-Driver/assets/140182146/df6dbf2f-45fd-4e3c-8910-f7b181ce98fa)

