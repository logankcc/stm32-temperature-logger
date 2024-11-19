# STM32 Temperature Logger
This repository contains all the source code required to program an STM32 NUCLEO-F446RE microcontroller to interact with a TMP100 temperature sensor and a 24FC256 EEPROM using C++. The source code is located in the `Project` directory. Auto-generated HAL files, created by STM32CubeIDE, are located in the `Core` and `Drivers` directories.

The entry point for this project is the `project_main` function, implemented in `Project/Src/project_main.cpp`, and called in `Core/Src/main.c`. This structure ensures complete separation between the auto-generated HAL files and the embedded program. As a result, STM32CubeIDE can update the auto-generated files without impacting the program.

## Program Logic
- **Step 1: Configuration**  
    - Select the TMP100 **Configuration Register** by writing `0x01` to the **Pointer Register**.  
    - Write `0x21` to the **Configuration Register** during setup to enable **Shut-Down Mode** and set **10-bit Resolution (0.25°C)**.
    - If configuration fails, turn off the on-board LED to notify of failure and terminate the program.

- **Step 2: Trigger Conversion**  
    - Select the TMP100  **Configuration Register** by writing `0x01` to the **Pointer Register**.  
    - Set the **OS/ALERT** bit (bit-7) to `1` to initiate a single temperature conversion.  
    - Wait **60 ms** for the conversion to complete.

- **Step 3: Read Temperature Data**  
    - Select the TMP100  **Temperature Register** by writing `0x00` to the **Pointer Register**.  
    - Read **2 bytes** of temperature data from the register.

- **Step 4: Write Data to EEPROM**
    - Select a **16-bit memory address** (`0x0000` to `0x7FFF`) by sending **2 bytes** to the **24FC256**.  
    - Write the **2 bytes** of temperature data to the selected address.  
    - Increment the next memory address locally for the subsequent write.

- **Step 5: Repeat Periodically**  
    - Repeat Steps 2 to 4 every **10 minutes**.

## Requirements

### Hardware Requirements

#### TMP100 Temperature Sensor
- **Power Supply**:
    - Requires a power supply voltage between **2.7 V and 5.5 V**.
    - A **0.1 μF bypass capacitor** should be placed as close as possible to the **Vcc** and **GND** pins for noise filtering.
- **I2C Communication**:
    - Uses **SDA** and **SCL** lines for I2C communication.
    - The **SDA** and **SCL** lines must have **5 kΩ pull-up resistors** to Vcc.
- **Address Selection**:
    - The ADD0 and ADD1 pins must be connected to **GND**, **Vcc**, or left **floating** to configure the I2C address.
    - The address must not conflict with the **24FC256**.
- **Operating Temperature**:
    - Must operate within the temperature range of **-55°C to 125°C**.

#### 24FC256 EEPROM
- **Power Supply**:
    - Requires a power supply voltage between **1.7 V and 5.5 V**.
    - A **0.1 μF bypass capacitor** should be placed as close as possible to the **Vcc** and **GND** pins for noise filtering.
- **I2C Communication**:
    - Uses **SDA** and **SCL** lines for I2C communication.
    - The **SDA** and **SCL** lines must have **10 kΩ pull-up resistors** to Vcc when operating at **100 kHz**.
- **Address Selection**:
    - The ADD0, ADD1, and ADD2 pins must be connected to **GND** to support all package types.
    - The address must not conflict with the **TMP100**.
- **Operating Temperature**:
    - Must operate within the temperature range of **-40°C to 85°C**.

#### Microcontroller
- **Power Supply**:
    - Must supply a voltage between **2.7 V and 5.5 V**.
- **I2C Communication**:
    - Requires **SDA** and **SCL** lines for I2C communication.
    - Must support a standard-mode I2C clock frequency of **100 kHz**.

### Functional Software Requirements
- Read temperature data from the TMP100 temperature sensor every 10 minutes.
- Write the temperature data to the 24FC256 EEPROM.
- Handle I2C communication errors to ensure proper operation.

### Non-Functional Software Requirements
- The code must be sustainable and reusable.
- The codebase must be easy to maintain.

## Design Decisions
- **100 kHz Clock Speed**
   - Ensures compatibility with standard-mode I2C devices and provides sufficient speed for periodic temperature readings.

- **TMP100 in Shut-Down Mode**
   - Reduces power consumption by keeping the sensor off except when taking a measurement.

- **0.25°C Temperature Resolution**  
   - The temperature data is stored with a resolution of 0.25°C, which provides the necessary granularity without requiring additional computational complexity.

- **Big-Endian Format**
   - Ensures consistent byte ordering, simplifying data interpretation across platforms.

- **Store Raw Data**
   - Eliminates the need for floating-point operations, simplifying data storage and processing.

- **No UNIX Timestamps**
   - Saves memory by avoiding 4-byte timestamps, which would triple the size of each data point.

- **Blocking I2C Function Calls**
   - Simplifies implementation and ensures reliable communication without requiring interrupts.

## Known Issues
- **Memory Wrap-Around**  
    - The 24FC256 EEPROM has 32,768 valid addresses (`0x0000` to `0x7FFF`). On the 113th day of operation (assuming one reading every 10 minutes and 2 bytes per reading), the chip will run out of memory and the program will overwrite data starting at address `0x0000`.

- **No UNIX Timestamps**  
    - There is no way to determine when a temperature reading was taken, as timestamps are not stored with the data.

- **Error Propagation**  
    - Errors are propagated to the initial caller. I2C errors during operation cannot be identified as they are only logged via UART during debugging.

- **Resolution Configuration**  
    - The temperature reading resolution is configurable by storing the resolution bits `R1` and `R0` as a data member of the `TMP100` class. However, the method `TMP100::convertRawTemperatureDataToCelsius` assumes the resolution of a passed temperature reading based on the current bit settings. For example, if a 10-bit measurement is passed while the resolution is configured for 9 bits, the Celsius conversion will be incorrect.
