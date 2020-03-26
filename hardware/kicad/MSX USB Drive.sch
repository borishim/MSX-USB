EESchema Schematic File Version 4
LIBS:MSX USB Drive-cache
EELAYER 29 0
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
L Connector_Generic:Conn_02x25_Odd_Even J1
U 1 1 5D239E16
P 2000 3600
F 0 "J1" H 2050 5017 50  0000 C CNN
F 1 "Conn_02x25_Odd_Even" H 2050 4926 50  0000 C CNN
F 2 "" H 2000 3600 50  0001 C CNN
F 3 "~" H 2000 3600 50  0001 C CNN
	1    2000 3600
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS688 U1
U 1 1 5D24214F
P 3950 3500
F 0 "U1" H 4494 3546 50  0000 L CNN
F 1 "74LS688" H 4494 3455 50  0000 L CNN
F 2 "" H 3950 3500 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS688" H 3950 3500 50  0001 C CNN
	1    3950 3500
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x08_Odd_Even J2
U 1 1 5D243AE7
P 5850 3400
F 0 "J2" H 5900 3917 50  0000 C CNN
F 1 "Conn_02x08_Odd_Even" H 5900 3826 50  0000 C CNN
F 2 "" H 5850 3400 50  0001 C CNN
F 3 "~" H 5850 3400 50  0001 C CNN
	1    5850 3400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 4700 1700 4700
Wire Wire Line
	1700 4700 1700 4600
Wire Wire Line
	1700 4600 1800 4600
Connection ~ 1700 4700
Wire Wire Line
	1700 4700 1800 4700
$Comp
L power:VCC #PWR0101
U 1 1 5D24690A
P 1550 4700
F 0 "#PWR0101" H 1550 4550 50  0001 C CNN
F 1 "VCC" V 1568 4827 50  0000 L CNN
F 2 "" H 1550 4700 50  0001 C CNN
F 3 "" H 1550 4700 50  0001 C CNN
	1    1550 4700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1550 4500 1700 4500
Wire Wire Line
	1700 4500 1700 4400
Wire Wire Line
	1700 4400 1800 4400
Connection ~ 1700 4500
Wire Wire Line
	1700 4500 1800 4500
$Comp
L power:GND #PWR0102
U 1 1 5D2482CD
P 1550 4500
F 0 "#PWR0102" H 1550 4250 50  0001 C CNN
F 1 "GND" V 1555 4372 50  0000 R CNN
F 2 "" H 1550 4500 50  0001 C CNN
F 3 "" H 1550 4500 50  0001 C CNN
	1    1550 4500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0103
U 1 1 5D24A39B
P 3950 4700
F 0 "#PWR0103" H 3950 4450 50  0001 C CNN
F 1 "GND" H 3955 4527 50  0000 C CNN
F 2 "" H 3950 4700 50  0001 C CNN
F 3 "" H 3950 4700 50  0001 C CNN
	1    3950 4700
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0104
U 1 1 5D24A8BC
P 3950 2300
F 0 "#PWR0104" H 3950 2150 50  0001 C CNN
F 1 "VCC" H 3967 2473 50  0000 C CNN
F 2 "" H 3950 2300 50  0001 C CNN
F 3 "" H 3950 2300 50  0001 C CNN
	1    3950 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 5D24B659
P 3300 5350
F 0 "C1" H 3415 5396 50  0000 L CNN
F 1 "0.1 uF" H 3415 5305 50  0000 L CNN
F 2 "" H 3338 5200 50  0001 C CNN
F 3 "~" H 3300 5350 50  0001 C CNN
	1    3300 5350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0105
U 1 1 5D24BE8F
P 3300 5500
F 0 "#PWR0105" H 3300 5250 50  0001 C CNN
F 1 "GND" H 3305 5327 50  0000 C CNN
F 2 "" H 3300 5500 50  0001 C CNN
F 3 "" H 3300 5500 50  0001 C CNN
	1    3300 5500
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR0106
U 1 1 5D24C5DA
P 3300 5200
F 0 "#PWR0106" H 3300 5050 50  0001 C CNN
F 1 "VCC" H 3317 5373 50  0000 C CNN
F 2 "" H 3300 5200 50  0001 C CNN
F 3 "" H 3300 5200 50  0001 C CNN
	1    3300 5200
	1    0    0    -1  
$EndComp
Text GLabel 3450 3200 0    50   Input ~ 0
IORQ_
Text GLabel 1800 2900 0    50   Input ~ 0
IORQ_
Text GLabel 6150 3400 2    50   Input ~ 0
A0
Text GLabel 6150 3200 2    50   Input ~ 0
RD_
Text GLabel 6150 3100 2    50   Input ~ 0
WR_
Text GLabel 6150 3300 2    50   Input ~ 0
CS_
$Comp
L power:GND #PWR0107
U 1 1 5D2519F2
P 6150 3800
F 0 "#PWR0107" H 6150 3550 50  0001 C CNN
F 1 "GND" V 6155 3672 50  0000 R CNN
F 2 "" H 6150 3800 50  0001 C CNN
F 3 "" H 6150 3800 50  0001 C CNN
	1    6150 3800
	0    -1   -1   0   
$EndComp
$Comp
L power:VCC #PWR0108
U 1 1 5D252457
P 6150 3600
F 0 "#PWR0108" H 6150 3450 50  0001 C CNN
F 1 "VCC" V 6167 3728 50  0000 L CNN
F 2 "" H 6150 3600 50  0001 C CNN
F 3 "" H 6150 3600 50  0001 C CNN
	1    6150 3600
	0    1    1    0   
$EndComp
Text GLabel 4450 2600 2    50   Input ~ 0
CS_
Text GLabel 2300 3700 2    50   Input ~ 0
A0
Text GLabel 2300 3000 2    50   Input ~ 0
RD_
Text GLabel 1800 3000 0    50   Input ~ 0
WR_
Text GLabel 3450 2600 0    50   Input ~ 0
A1
Text GLabel 3450 2700 0    50   Input ~ 0
A2
Text GLabel 3450 2800 0    50   Input ~ 0
A3
Text GLabel 3450 2900 0    50   Input ~ 0
A5
Text GLabel 3450 3000 0    50   Input ~ 0
A6
Text GLabel 3450 3100 0    50   Input ~ 0
A7
Text GLabel 3450 3300 0    50   Input ~ 0
A4
$Comp
L power:VCC #PWR0109
U 1 1 5D259292
P 3450 4200
F 0 "#PWR0109" H 3450 4050 50  0001 C CNN
F 1 "VCC" V 3468 4327 50  0000 L CNN
F 2 "" H 3450 4200 50  0001 C CNN
F 3 "" H 3450 4200 50  0001 C CNN
	1    3450 4200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3450 3800 3450 3700
Wire Wire Line
	3450 3700 3450 3600
Connection ~ 3450 3700
Wire Wire Line
	3450 3600 3450 3500
Connection ~ 3450 3600
$Comp
L power:GND #PWR0110
U 1 1 5D25B3BF
P 3450 3500
F 0 "#PWR0110" H 3450 3250 50  0001 C CNN
F 1 "GND" V 3455 3372 50  0000 R CNN
F 2 "" H 3450 3500 50  0001 C CNN
F 3 "" H 3450 3500 50  0001 C CNN
	1    3450 3500
	0    1    1    0   
$EndComp
Connection ~ 3450 3500
Text GLabel 1800 3700 0    50   Input ~ 0
A1
Text GLabel 2300 3800 2    50   Input ~ 0
A2
Text GLabel 2300 3900 2    50   Input ~ 0
A4
Text GLabel 1800 3800 0    50   Input ~ 0
A3
Text GLabel 1800 3900 0    50   Input ~ 0
A5
Text GLabel 1800 3400 0    50   Input ~ 0
A7
Text GLabel 2300 3400 2    50   Input ~ 0
A6
Text GLabel 5650 3800 0    50   Input ~ 0
D0
Text GLabel 5650 3700 0    50   Input ~ 0
D1
Text GLabel 5650 3600 0    50   Input ~ 0
D2
Text GLabel 5650 3500 0    50   Input ~ 0
D3
Text GLabel 5650 3400 0    50   Input ~ 0
D4
Text GLabel 5650 3300 0    50   Input ~ 0
D5
Text GLabel 5650 3200 0    50   Input ~ 0
D6
Text GLabel 5650 3100 0    50   Input ~ 0
D7
Text GLabel 2300 4000 2    50   Input ~ 0
D0
Text GLabel 2300 4100 2    50   Input ~ 0
D2
Text GLabel 2300 4200 2    50   Input ~ 0
D4
Text GLabel 2300 4300 2    50   Input ~ 0
D6
Text GLabel 1800 4000 0    50   Input ~ 0
D1
Text GLabel 1800 4100 0    50   Input ~ 0
D3
Text GLabel 1800 4200 0    50   Input ~ 0
D5
Text GLabel 1800 4300 0    50   Input ~ 0
D7
NoConn ~ 2300 2400
NoConn ~ 2300 2500
NoConn ~ 2300 2600
NoConn ~ 2300 2700
NoConn ~ 2300 2900
NoConn ~ 2300 3100
NoConn ~ 2300 3200
NoConn ~ 2300 3300
NoConn ~ 2300 3500
NoConn ~ 2300 3600
NoConn ~ 1800 3600
NoConn ~ 1800 3500
NoConn ~ 1800 3300
NoConn ~ 1800 3200
NoConn ~ 1800 3100
NoConn ~ 1800 2800
NoConn ~ 1800 2700
NoConn ~ 1800 2600
NoConn ~ 1800 2500
NoConn ~ 1800 2400
NoConn ~ 2300 4400
NoConn ~ 2300 4700
NoConn ~ 2300 4800
NoConn ~ 1800 4800
NoConn ~ 1650 5000
Wire Wire Line
	6150 3700 6150 3800
Connection ~ 6150 3800
NoConn ~ 6150 3500
Wire Wire Line
	3450 3900 3450 3800
Connection ~ 3450 3800
Wire Wire Line
	3450 4000 3450 3900
Connection ~ 3450 3900
Wire Wire Line
	3450 4100 3450 4000
Connection ~ 3450 4000
$Comp
L power:GND #PWR01
U 1 1 5D4D6C52
P 3450 4400
F 0 "#PWR01" H 3450 4150 50  0001 C CNN
F 1 "GND" V 3455 4272 50  0000 R CNN
F 2 "" H 3450 4400 50  0001 C CNN
F 3 "" H 3450 4400 50  0001 C CNN
	1    3450 4400
	0    1    1    0   
$EndComp
Wire Wire Line
	2300 4600 2400 4600
Wire Wire Line
	2400 4600 2400 4500
Wire Wire Line
	2400 4500 2300 4500
Text GLabel 2300 2800 2    50   Input ~ 0
BUSDIR_
Text GLabel 5700 4550 2    50   Input ~ 0
BUSDIR_
Text GLabel 5100 4450 0    50   Input ~ 0
CS_
Text GLabel 5100 4650 0    50   Input ~ 0
RD_
$Comp
L 74xx:74LS32 U2
U 3 1 5D4EE21D
P 5400 4550
F 0 "U2" H 5400 4875 50  0000 C CNN
F 1 "74LS32" H 5400 4784 50  0000 C CNN
F 2 "" H 5400 4550 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS32" H 5400 4550 50  0001 C CNN
	3    5400 4550
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74LS32 U2
U 5 1 5D504037
P 5450 5450
F 0 "U2" H 5450 5775 50  0000 C CNN
F 1 "74LS32" H 5450 5684 50  0000 C CNN
F 2 "" H 5450 5450 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS32" H 5450 5450 50  0001 C CNN
	5    5450 5450
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 5D5138BF
P 5450 4950
F 0 "#PWR?" H 5450 4800 50  0001 C CNN
F 1 "VCC" H 5467 5123 50  0000 C CNN
F 2 "" H 5450 4950 50  0001 C CNN
F 3 "" H 5450 4950 50  0001 C CNN
	1    5450 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5D514275
P 5450 5950
F 0 "#PWR?" H 5450 5700 50  0001 C CNN
F 1 "GND" H 5455 5777 50  0000 C CNN
F 2 "" H 5450 5950 50  0001 C CNN
F 3 "" H 5450 5950 50  0001 C CNN
	1    5450 5950
	1    0    0    -1  
$EndComp
Text GLabel 10100 1950 2    50   Input ~ 0
D0
Text GLabel 10100 2050 2    50   Input ~ 0
D1
Text GLabel 10100 2150 2    50   Input ~ 0
D2
Text GLabel 10100 2250 2    50   Input ~ 0
D3
Text GLabel 10100 2350 2    50   Input ~ 0
D4
Text GLabel 10100 2450 2    50   Input ~ 0
D5
Text GLabel 10100 2550 2    50   Input ~ 0
D6
Text GLabel 10100 2650 2    50   Input ~ 0
D7
Text GLabel 8900 1950 0    50   Input ~ 0
A0
Text GLabel 8900 2050 0    50   Input ~ 0
A1
Text GLabel 8900 2150 0    50   Input ~ 0
A2
Text GLabel 8900 2250 0    50   Input ~ 0
A3
Text GLabel 8900 2350 0    50   Input ~ 0
A4
Text GLabel 8900 2450 0    50   Input ~ 0
A5
Text GLabel 8900 2550 0    50   Input ~ 0
A6
Text GLabel 8900 2650 0    50   Input ~ 0
A7
Text GLabel 8900 2750 0    50   Input ~ 0
A8
Text GLabel 8900 2850 0    50   Input ~ 0
A9
Text GLabel 8900 2950 0    50   Input ~ 0
A10
Text GLabel 8900 3050 0    50   Input ~ 0
A11
Text GLabel 8900 3150 0    50   Input ~ 0
A12
$Comp
L 74xx_IEEE:74LS670 U?
U 1 1 5DB8E9EF
P 7750 3100
F 0 "U?" H 7750 3866 50  0000 C CNN
F 1 "74LS670" H 7750 3775 50  0000 C CNN
F 2 "" H 7750 3100 50  0001 C CNN
F 3 "" H 7750 3100 50  0001 C CNN
	1    7750 3100
	1    0    0    -1  
$EndComp
$Comp
L Memory_Flash:SST39SF040 U?
U 1 1 5DB86845
P 9500 3150
F 0 "U?" H 9500 4631 50  0000 L CNN
F 1 "SST39SF040" H 9500 4540 50  0000 L CNN
F 2 "" H 9500 3450 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/25022B.pdf" H 9500 3450 50  0001 C CNN
	1    9500 3150
	1    0    0    -1  
$EndComp
$Comp
L 74xx_IEEE:74LS670 U?
U 1 1 5DB9E92E
P 7750 4450
F 0 "U?" H 7750 5216 50  0000 C CNN
F 1 "74LS670" H 7750 5125 50  0000 C CNN
F 2 "" H 7750 4450 50  0001 C CNN
F 3 "" H 7750 4450 50  0001 C CNN
	1    7750 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 3250 8900 3250
Wire Wire Line
	8300 3350 8900 3350
Wire Wire Line
	8300 3450 8900 3450
Wire Wire Line
	8300 3550 8900 3550
Wire Wire Line
	8300 4600 8300 3650
Wire Wire Line
	8300 3650 8900 3650
Wire Wire Line
	8300 4700 8400 4700
Wire Wire Line
	8400 4700 8400 3750
Wire Wire Line
	8400 3750 8900 3750
$Comp
L power:GND #PWR?
U 1 1 5DBA29A0
P 9500 4350
F 0 "#PWR?" H 9500 4100 50  0001 C CNN
F 1 "GND" H 9505 4177 50  0000 C CNN
F 2 "" H 9500 4350 50  0001 C CNN
F 3 "" H 9500 4350 50  0001 C CNN
	1    9500 4350
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR?
U 1 1 5DBA455B
P 9500 1450
F 0 "#PWR?" H 9500 1300 50  0001 C CNN
F 1 "VCC" H 9517 1623 50  0000 C CNN
F 2 "" H 9500 1450 50  0001 C CNN
F 3 "" H 9500 1450 50  0001 C CNN
	1    9500 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9500 1850 9500 1450
Text GLabel 2300 2500 2    50   Input ~ 0
SLTSL_
Text GLabel 6200 1750 0    50   Input ~ 0
WR_
Text GLabel 6200 1950 0    50   Input ~ 0
SLTSL_
Wire Wire Line
	6800 1850 6900 1850
Wire Wire Line
	6900 1850 6900 3000
Wire Wire Line
	6900 3000 7200 3000
Wire Wire Line
	7200 4350 6900 4350
Wire Wire Line
	6900 4350 6900 3000
Connection ~ 6900 3000
Text GLabel 7200 2600 0    50   Input ~ 0
A15
Text GLabel 7200 3950 0    50   Input ~ 0
A15
Text GLabel 7200 2700 0    50   Input ~ 0
A13
Text GLabel 7200 4050 0    50   Input ~ 0
A13
Text GLabel 7200 2800 0    50   Input ~ 0
A15
Text GLabel 7200 2900 0    50   Input ~ 0
A13
Text GLabel 7200 4150 0    50   Input ~ 0
A15
Text GLabel 7200 4250 0    50   Input ~ 0
A13
Text GLabel 7200 3250 0    50   Input ~ 0
D0
Text GLabel 7200 3350 0    50   Input ~ 0
D1
Text GLabel 7200 3450 0    50   Input ~ 0
D2
Text GLabel 7200 3550 0    50   Input ~ 0
D3
Text GLabel 7200 4600 0    50   Input ~ 0
D4
Text GLabel 7200 4700 0    50   Input ~ 0
D5
Text GLabel 7200 4800 0    50   Input ~ 0
D6
Text GLabel 7200 4900 0    50   Input ~ 0
D7
$Comp
L power:GND #PWR?
U 1 1 5DBB7136
P 7200 3100
F 0 "#PWR?" H 7200 2850 50  0001 C CNN
F 1 "GND" H 7205 2927 50  0000 C CNN
F 2 "" H 7200 3100 50  0001 C CNN
F 3 "" H 7200 3100 50  0001 C CNN
	1    7200 3100
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5DBB82A9
P 7200 4450
F 0 "#PWR?" H 7200 4200 50  0001 C CNN
F 1 "GND" H 7205 4277 50  0000 C CNN
F 2 "" H 7200 4450 50  0001 C CNN
F 3 "" H 7200 4450 50  0001 C CNN
	1    7200 4450
	0    1    1    0   
$EndComp
$Comp
L 74xx:74LS32 U?
U 2 1 5DBB94CC
P 6500 1850
F 0 "U?" H 6500 2175 50  0000 C CNN
F 1 "74LS32" H 6500 2084 50  0000 C CNN
F 2 "" H 6500 1850 50  0001 C CNN
F 3 "http://www.ti.com/lit/gpn/sn74LS32" H 6500 1850 50  0001 C CNN
	2    6500 1850
	1    0    0    -1  
$EndComp
Text GLabel 2300 2900 2    50   Input ~ 0
MERQ_
Text GLabel 8900 4250 0    50   Input ~ 0
SLTSL_
Text GLabel 8900 4150 0    50   Input ~ 0
RD_
Text GLabel 8900 3950 0    50   Input ~ 0
WR_
$EndSCHEMATC
