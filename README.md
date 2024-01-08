# Analog to Digital Conversion with Sensor Data Display on LCD

## Overview

This project revolves around using the TM4C123GH6PM microcontroller to implement Analog to Digital Conversion (ADC) and interface with sensors. The primary goal is to convert analog signals from a potentiometer, photosensor, and temperature sensor into a digital format, displaying the results on an LCD. The project explores practical applications of ADCs, GPIO configurations, and efficient use of interrupts for user interactions.

The project's solution involves configuring the microcontroller's ADC module, GPIO ports, and LCD display. The provided code initializes necessary peripherals, making use of interrupts for user input through pushbuttons. By strategically employing the Nested Vectored Interrupt Controller (NVIC), the system ensures responsiveness, particularly when handling user interactions like toggling between different sensors and temperature units. This approach enhances real-time responsiveness, ensuring smooth transitions between tasks and optimal user experience.

The switch statement in the main function adeptly manages the display of sensor data on the LCD, providing users with the ability to toggle between different sensors and temperature units seamlessly.

## Products Used

- **TM4C123GH6PM Microcontroller**: Utilized for ADC, GPIO, and LCD interfacing operations.
- **Potentiometer, Photosensor, and Temperature Sensor**: Built-in sensors generating analog signals, converted to digital using the ADC module.
- **LCD Display**: Used to visually present the converted sensor data.

## Learning Opportunities

- **Analog to Digital Conversion**: Understand the basics of ADCs and their practical applications.
- **GPIO Configuration**: Learn to configure GPIO ports for sensor enabling, pushbutton handling, and interfacing with devices like LCDs.
- **Interrupt Handling**: Explore the use of interrupts for efficient user input management, contributing to system responsiveness.

## Usefulness for Other Projects

This project serves as a foundational example for implementing ADC in embedded systems. The provided functions for LCD interfacing can be reused in projects involving data display. The modular structure allows easy adaptation for different sensor types and additional features.

## Note

This project was developed as part of a school assignment. The LCD functions were provided beforehand, emphasizing the focus on ADC and GPIO configurations.
