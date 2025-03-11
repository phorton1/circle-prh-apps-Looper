EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Device:R R?
U 1 1 67D04B55
P 2400 1650
F 0 "R?" H 2470 1696 50  0001 L CNN
F 1 "1K" V 2400 1600 50  0000 L CNN
F 2 "" V 2330 1650 50  0001 C CNN
F 3 "~" H 2400 1650 50  0001 C CNN
	1    2400 1650
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push_DPDT SW?
U 1 1 67D04E81
P 1650 1700
F 0 "SW?" H 1650 2185 50  0001 C CNN
F 1 "SW_Push_DPDT" H 1650 2093 50  0001 C CNN
F 2 "" H 1650 1900 50  0001 C CNN
F 3 "~" H 1650 1900 50  0001 C CNN
	1    1650 1700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 67D05D1A
P 2400 1150
F 0 "R?" H 2470 1196 50  0001 L CNN
F 1 "10K" V 2400 1100 50  0000 L CNN
F 2 "" V 2330 1150 50  0001 C CNN
F 3 "~" H 2400 1150 50  0001 C CNN
	1    2400 1150
	1    0    0    -1  
$EndComp
Text GLabel 1450 1500 0    50   Input ~ 0
RED-IN
Text GLabel 1450 1900 0    50   Input ~ 0
BLK_GROUND
Text GLabel 2750 1400 2    50   Input ~ 0
YELLOW_OUT
Wire Wire Line
	1850 1400 2400 1400
NoConn ~ 1850 1800
Wire Wire Line
	1850 2000 2400 2000
Wire Wire Line
	2400 2000 2400 1800
Wire Wire Line
	2400 1500 2400 1400
Connection ~ 2400 1400
Wire Wire Line
	2400 1400 2750 1400
Wire Wire Line
	2400 1300 2400 1400
Wire Wire Line
	1850 1600 2050 1600
Wire Wire Line
	2050 1600 2050 850 
Wire Wire Line
	2050 850  2400 850 
Wire Wire Line
	2400 850  2400 1000
Text Notes 2650 1150 0    50   ~ 0
Position shown is NO PAD (up)\nRed is connected directly to Yellow\nand Ground has no connection \nso 1K resistor does nothing
Text Notes 2650 1950 0    50   ~ 0
PAD is other position (down)\nRed goes to 10K-1K voltage divider\nand BLK is connected at the bottom.\nYellow is pulled from the middle
Text Notes 1000 3200 0    50   ~ 0
When we last tried Looper2/TE1, I had to stick a toothpick in the pad button to keep it \ndown in order to get a green "active sound" LED from the iRig2HD.\nThis would be the DOWN=PAD position and gave workable levels.\n\nIt should have also given a green "active sound" in the UP=NO PAD position\nas it would have been even "hotter".  I suspect the reason it didn't is that\nthe switch was not making a good contact in the up position, perhaps due\nto the 3D printed housing.\n\nIn any case, I am going to replace the circuit board with a permanent\nPAD voltage divider, and test it, before I put Looper2/TE1 away for posterity\nas I switch over to TE3/Looper3. \n\nThe old iPad barely holds a charge.
$EndSCHEMATC
