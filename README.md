# Ladderlib
  
Ladderlib is a C library designed for implementing ladder logic programming, a method widely used in Programmable Logic Controllers (PLCs) for industrial automation. It provides a robust framework to define, execute, and manage ladder logic programs, supporting hardware interactions and custom function extensions. This library is particularly suited for embedded systems.  
  
**Project Information:**  
- **Repository:** [Ladderlib GitHub](https://github.com/hiperiondev/ladderlib)  
- **Version:** 2.0.0  
- **License:** MIT License  
- **Copyright:** 2025 Emiliano Gonzalez (egonzalez.hiperion@gmail.com)  
  
## Overview  
Ladderlib enables developers to create ladder logic programs, which are graphical representations of control logic resembling electrical relay circuits. These programs are executed cyclically by PLCs to automate industrial processes, such as controlling machinery in manufacturing or chemical processing. The library  supports defining logic through cells and networks, managing execution states, handling hardware inputs/outputs, and integrating custom functions for enhanced flexibility.  
  
Key features include:  
- **Instruction Set**: 39 instructions for defining logic operations.  
- **State Management**: 6 states to control program execution.  
- **Error Handling**: 8 error codes for robust operation.  
- **Hardware Integration**: Function pointers for reading inputs and writing outputs.  
- **Custom Extensions**: Support for adding user-defined functions.  
  
The library is likely optimized for embedded systems.
 
## Introduction to Ladder Logic  
Ladder logic programs consist of "rungs" connecting two vertical "rails," mimicking electrical relay circuits. Each rung contains conditions (e.g., contacts) and actions (e.g., coils)

## Basic Logic Instructions  

### Instruction Summary Table  
The following table summarizes the instructions, their codes, categories, and symbols.  
  
| Instruction | Code | Category | Symbol | Description |  
|-------------|------|----------|--------|-------------|  
| NOP | 0 | Specialized | None | No operation, placeholder |  
| CONN | 1 | Specialized | Vertical lines | Connects rungs or branches |  
| NEG | 2 | Specialized |  <img src="images/png/NEG.png" alt="Image"> | Inverts logic state |  
| NO | 3 | Basic Logic | <img src="images/png/NO.png" alt="Image"> | Normally Open contact |  
| NC | 4 | Basic Logic |  <img src="images/png/NC.png" alt="Image"> | Normally Closed contact |  
| RE | 5 | Specialized |  <img src="images/png/RE.png" alt="Image"> | Rising edge detection |  
| FE | 6 | Specialized |  <img src="images/png/FE.png" alt="Image"> | Falling edge detection |  
| COIL | 7 | Basic Logic |  <img src="images/png/COIL.png" alt="Image"> | Output coil |  
| COILL | 8 | Basic Logic |  <img src="images/png/COILL.png" alt="Image"> | Latched coil |  
| COILU | 9 | Basic Logic |  <img src="images/png/COILU.png" alt="Image"> | Unlatched coil |  
| TON | 10 | Timer |  <img src="images/png/TON.png" alt="Image"> | Timer On Delay |  
| TOFF | 11 | Timer |  <img src="images/png/TOF.png" alt="Image"> | Timer Off Delay |  
| TP | 12 | Timer |  <img src="images/png/TP.png" alt="Image"> | Timer Pulse |  
| CTU | 13 | Counter |  <img src="images/png/CTU.png" alt="Image"> | Count Up |  
| CTD | 14 | Counter |  <img src="images/png/CTD.png" alt="Image"> | Count Down |  
| MOVE | 15 | Specialized | <img src="images/png/MOV.png" alt="Image"> | Moves data |  
| SUB | 16 | Arithmetic |  <img src="images/png/SUB.png" alt="Image"> | Subtraction |  
| ADD | 17 | Arithmetic | <img src="images/png/ADD.png" alt="Image"> | Addition |  
| MUL | 18 | Arithmetic | <img src="images/png/MUL.png" alt="Image"> | Multiplication |  
| DIV | 19 | Arithmetic | <img src="images/png/DIV.png" alt="Image">| Division |  
| MOD | 20 | Arithmetic | <img src="images/png/MOD.png" alt="Image"> | Modulus |  
| SHL | 21 | Bitwise | <img src="images/png/SHL.png" alt="Image"> | Shift Left |  
| SHR | 22 | Bitwise | <img src="images/png/SHR.png" alt="Image"> | Shift Right |  
| ROL | 23 | Bitwise | <img src="images/png/ROL.png" alt="Image"> | Rotate Left |  
| ROR | 24 | Bitwise | <img src="images/png/ROR.png" alt="Image"> | Rotate Right |  
| AND | 25 | Bitwise | <img src="images/png/AND.png" alt="Image"> | Bitwise AND |  
| OR | 26 | Bitwise | <img src="images/png/OR.png" alt="Image"> | Bitwise OR |  
| XOR | 27 | Bitwise | <img src="images/png/XOR.png" alt="Image"> | Bitwise XOR |  
| NOT | 28 | Bitwise | <img src="images/png/NOT.png" alt="Image"> | Bitwise NOT |  
| EQ | 29 | Comparison | <img src="images/png/EQ.png" alt="Image"> | Equal |  
| GT | 30 | Comparison | <img src="images/png/GT.png" alt="Image"> | Greater Than |  
| GE | 31 | Comparison | <img src="images/png/GE.png" alt="Image"> | Greater Than or Equal |  
| LT | 32 | Comparison | <img src="images/png/LT.png" alt="Image"> | Less Than |  
| LE | 33 | Comparison | <img src="images/png/LE.png" alt="Image"> | Less Than or Equal |  
| NE | 34 | Comparison | <img src="images/png/NE.png" alt="Image"> | Not Equal |  
| FOREIGN | 35 | Specialized | Box labeled "FOREIGN" | External function call |  
| TMOVE | 36 | Specialized | <img src="images/png/TMOV.png" alt="Image"> | Table data move |  
| INV | 37 | Specialized | None | Invalid instruction |

## Basic Instructions
Basic instructions manage Input/Output functions
  
### NO (Normally Open Contact)  
- **Instruction Code:** `LADDER_INS_NO` (3)  
- **Description:** The Normally Open contact allows logic flow when its associated input is true, acting like a switch that closes when activated. It is used to check if an input condition (e.g., a sensor) is met.  
- **Example Use:** Checking if a pushbutton is pressed to activate a motor.  
  
### NC (Normally Closed Contact)  
- **Instruction Code:** `LADDER_INS_NC` (4)  
- **Description:** The Normally Closed contact allows logic flow when its associated input is false, acting like a switch that opens when activated. It is used to proceed when a condition is not met.  
- **Example Use:** Ensuring a safety switch is not triggered before starting a process.  
  
### COIL (Output Coil)  
- **Instruction Code:** `LADDER_INS_COIL` (7)  
- **Description:** The Coil instruction sets an output to true when the rung's logic is true, controlling devices like motors or lights. It represents the action taken when conditions are satisfied.  
- **Example Use:** Turning on a conveyor belt when all conditions are met.  
  
### COILL (Coil Latch)  
- **Instruction Code:** `LADDER_INS_COILL` (8)  
- **Description:** The Coil Latch sets an output to true and maintains it (latches) even if the input conditions change, until unlatched. It is used for memory functions in control systems.  
- **Example Use:** Keeping a pump running after a start button is released.  
  
### COILU (Coil Unlatch)  
- **Instruction Code:** `LADDER_INS_COILU` (9)  
- **Description:** The Coil Unlatch resets a latched output to false, releasing the latched state. It is paired with `COILL` for set/reset operations.  
- **Example Use:** Stopping a latched pump when a stop button is pressed.  
  
## Timer Instructions  
Timer instructions manage time-based operations, delaying or pulsing outputs.  
  
### TON (Timer On Delay)  
- **Instruction Code:** `LADDER_INS_TON` (10)  
- **Description:** The Timer On Delay starts timing when its input is true. After a preset time, the output becomes true. It is used to delay actions, such as waiting before starting a motor.  
- **Example Use:** Delaying a fan activation for 10 seconds after a heater starts.  
  
### TOFF (Timer Off Delay)  
- **Instruction Code:** `LADDER_INS_TOFF` (11)  
- **Description:** The Timer Off Delay starts timing when its input goes false. After a preset time, the output becomes false. It is used to extend the duration of an output.  
- **Example Use:** Keeping a light on for 5 seconds after a switch is turned off.  
  
### TP (Timer Pulse)  
- **Instruction Code:** `LADDER_INS_TP` (12)  
- **Description:** The Timer Pulse generates a pulse of a specified duration when triggered by a true input. It is used for creating timed signals, such as a brief alarm.  
- **Example Use:** Activating a buzzer for 2 seconds when a fault is detected.  
  
## Counter Instructions  
Counter instructions track the number of events or occurrences.  
  
### CTU (Count Up)  
- **Instruction Code:** `LADDER_INS_CTU` (13)  
- **Description:** The Count Up counter increments its count each time the input transitions from false to true, up to a preset value. It is used to count items or events.  
- **Example Use:** Counting the number of boxes on a conveyor belt.  
  
### CTD (Count Down)  
- **Instruction Code:** `LADDER_INS_CTD` (14)  
- **Description:** The Count Down counter decrements its count each time the input transitions from false to true, down to zero. It is used for countdown tasks.  
- **Example Use:** Tracking remaining items in a batch process.  
  
## Arithmetic Instructions  
These instructions perform mathematical operations on data.  
  
### ADD (Addition)  
- **Instruction Code:** `LADDER_INS_ADD` (17)  
- **Description:** Adds two values and stores the result in a specified location. It is used for calculations like summing sensor readings.  
- **Example Use:** Adding the outputs of two flow sensors.  
  
### SUB (Subtraction)  
- **Instruction Code:** `LADDER_INS_SUB` (16)  
- **Description:** Subtracts one value from another and stores the result. It is used for differential calculations.  
- **Example Use:** Calculating the difference in pressure between two points.  
  
### MUL (Multiplication)  
- **Instruction Code:** `LADDER_INS_MUL` (18)  
- **Description:** Multiplies two values and stores the result. It is used for scaling or area calculations.  
- **Example Use:** Calculating power by multiplying voltage and current.  
  
### DIV (Division)  
- **Instruction Code:** `LADDER_INS_DIV` (19)  
- **Description:** Divides one value by another and stores the result. It is used for ratio calculations.  
- **Example Use:** Determining speed by dividing distance by time.  
  
### MOD (Modulus)  
- **Instruction Code:** `LADDER_INS_MOD` (20)  
- **Description:** Computes the remainder of dividing one value by another. It is used for cyclic operations or remainder checks.  
- **Example Use:** Checking if a counter value is even or odd.  
  
## Bitwise Operation Instructions  
These instructions manipulate bits within data values.  
  
### SHL (Shift Left)  
- **Instruction Code:** `LADDER_INS_SHL` (21)  
- **Description:** Shifts the bits of a value to the left by a specified number of positions, filling with zeros. It is used for bit manipulation or scaling.  
- **Example Use:** Doubling a binary value by shifting left.  
  
### SHR (Shift Right)  
- **Instruction Code:** `LADDER_INS_SHR` (22)  
- **Description:** Shifts the bits of a value to the right by a specified number of positions, filling with zeros or sign bits. It is used for division or bit extraction.  
- **Example Use:** Halving a binary value by shifting right.  
  
### ROL (Rotate Left)  
- **Instruction Code:** `LADDER_INS_ROL` (23)  
- **Description:** Rotates the bits of a value to the left, with the most significant bit wrapping around to the least significant position. It is used for circular shifts.  
- **Example Use:** Rotating status flags in a control word.  
  
### ROR (Rotate Right)  
- **Instruction Code:** `LADDER_INS_ROR` (24)  
- **Description:** Rotates the bits of a value to the right, with the least significant bit wrapping around to the most significant position. It is used for circular shifts.  
- **Example Use:** Cycling through a set of control bits.  
  
### AND (Bitwise AND)  
- **Instruction Code:** `LADDER_INS_AND` (25)  
- **Description:** Performs a bitwise AND operation on two values, setting output bits to 1 only where both inputs are 1. It is used for masking or condition checks.  
- **Example Use:** Masking specific bits in a status register.  
  
### OR (Bitwise OR)  
- **Instruction Code:** `LADDER_INS_OR` (26)  
- **Description:** Performs a bitwise OR operation on two values, setting output bits to 1 where either input is 1. It is used for combining conditions.  
- **Example Use:** Combining multiple fault flags.  
  
### XOR (Bitwise XOR)  
- **Instruction Code:** `LADDER_INS_XOR` (27)  
- **Description:** Performs a bitwise XOR operation on two values, setting output bits to 1 where inputs differ. It is used for toggling or comparison.  
- **Example Use:** Detecting changes in bit patterns.  
  
### NOT (Bitwise NOT)  
- **Instruction Code:** `LADDER_INS_NOT` (28)  
- **Description:** Performs a bitwise NOT operation, inverting all bits of a value. It is used for logical negation.  
- **Example Use:** Inverting a control signal.  
  
## Comparison Instructions  
These instructions compare values and produce boolean outputs.  
  
### EQ (Equal)  
- **Instruction Code:** `LADDER_INS_EQ` (29)  
- **Description:** Checks if two values are equal, outputting true if they are. It is used for equality checks in control logic.  
- **Example Use:** Verifying if a temperature reading matches a setpoint.  
  
### GT (Greater Than)  
- **Instruction Code:** `LADDER_INS_GT` (30)  
- **Description:** Checks if one value is greater than another, outputting true if so. It is used for threshold checks.  
- **Example Use:** Checking if pressure exceeds a safe limit.  
  
### GE (Greater Than or Equal)  
- **Instruction Code:** `LADDER_INS_GE` (31)  
- **Description:** Checks if one value is greater than or equal to another, outputting true if so. It is used for inclusive threshold checks.  
- **Example Use:** Ensuring a tank level is at or above a minimum.  
  
### LT (Less Than)  
- **Instruction Code:** `LADDER_INS_LT` (32)  
- **Description:** Checks if one value is less than another, outputting true if so. It is used for lower-bound checks.  
- **Example Use:** Detecting if a speed is below a target.  
  
### LE (Less Than or Equal)  
- **Instruction Code:** `LADDER_INS_LE` (33)  
- **Description:** Checks if one value is less than or equal to another, outputting true if so. It is used for inclusive lower-bound checks.  
- **Example Use:** Verifying a flow rate is at or below a maximum.  
  
### NE (Not Equal)  
- **Instruction Code:** `LADDER_INS_NE` (34)  
- **Description:** Checks if two values are not equal, outputting true if they differ. It is used for change detection.  
- **Example Use:** Detecting a change in a sensor value.  
  
## Specialized Instructions  
These instructions provide additional functionality, some of which may be specific to ladderlib.  
  
### NOP (No Operation)  
- **Instruction Code:** `LADDER_INS_NOP` (0)  
- **Description:** Performs no operation, acting as a placeholder or for timing purposes. It does not affect the program state.  
- **Example Use:** Reserving space for future instructions.  
  
### CONN (Connection)  
- **Instruction Code:** `LADDER_INS_CONN` (1)  
- **Description:** Likely used to connect different parts of the ladder diagram, such as branching or joining rungs. It facilitates complex logic structures.  
- **Example Use:** Linking parallel branches in a rung.  
  
### NEG (Negate)  
- **Instruction Code:** `LADDER_INS_NEG` (2)  
- **Description:** Inverts the logic state of an input, turning true to false or vice versa. It is used to reverse conditions.  
- **Example Use:** Inverting a stop signal to enable a process.  
  
### RE (Rising Edge)  
- **Instruction Code:** `LADDER_INS_RE` (5)  
- **Description:** Detects a rising edge, triggering when an input transitions from false to true. It is used for event-driven actions.  
- **Example Use:** Starting a counter when a button is pressed.  
  
### FE (Falling Edge)  
- **Instruction Code:** `LADDER_INS_FE` (6)  
- **Description:** Detects a falling edge, triggering when an input transitions from true to false. It is used for event-driven actions.  
- **Example Use:** Resetting a system when a sensor deactivates.  
  
### MOVE (Move Data)  
- **Instruction Code:** `LADDER_INS_MOVE` (15)  
- **Description:** Copies data from one memory location to another without modification. It is used for data transfer.  
- **Example Use:** Transferring a sensor value to a display register.  
  
### TMOVE (Table Move)  
- **Instruction Code:** `LADDER_INS_TMOVE` (36)  
- **Description:** Likely used for moving data within tables or arrays, possibly for batch processing or data manipulation. The exact functionality may depend a lot on ladderlib's implementation.  
- **Example Use:** Moving a set of values to a new memory block.  
  
### FOREIGN (Foreign Function)  
- **Instruction Code:** `LADDER_INS_FOREIGN` (35)  
- **Description:** Allows calling external or user-defined functions not natively supported by ladder logic. It enables integration with custom code.  
- **Example Use:** Calling a custom algorithm for complex calculations.  
  
### INV (Invalid)  
- **Instruction Code:** `LADDER_INS_INV` (37)  
- **Description:** Marks the beginning of invalid instruction codes, not intended for use in programs. It serves as a boundary for valid instructions.  
- **Example Use:** None, as it is not used in programming.  
  
### MULTI (Multi-cell Instruction)  
- **Instruction Code:** `LADDER_INS_MULTI` (255, 0xff)  
- **Description:** Indicates that the cell is part of a larger instruction spanning multiple cells, used for complex operations requiring additional data.  
- **Example Use:** Implementing a multi-step operation like a sequence control.

## API Documentation  
  
This section provides detailed documentation for the core API functions of LadderLib, extracted from the header files `ladder.h`, `ladder_instructions.h`, and `ladder_internals.h`.  
  
### ladder_init  
  
Initializes the ladder context with specified parameters for networks, memory, inputs, outputs, timers, counters, and blocks.  
  
```c  
ladder_ctx_t* ladder_init(uint32_t net_columns_qty, uint32_t net_rows_qty, uint32_t networks_qty, uint32_t qty_m, uint32_t qty_i, uint32_t qty_q, uint32_t qty_t, uint32_t qty_c, uint32_t qty_b)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `net_columns_qty` | Number of columns in networks (max 32). Defines the width of each network grid. |  
| `net_rows_qty` | Number of rows in networks. Defines the height of each network grid. |  
| `networks_qty` | Number of networks. Specifies how many ladder networks the context will manage. |  
| `qty_m` | Quantity of memory bits. Number of general-purpose memory bits available. |  
| `qty_i` | Quantity of input bits. Number of input bits for interfacing with external inputs. |  
| `qty_q` | Quantity of output bits. Number of output bits for controlling external devices. |  
| `qty_t` | Quantity of timers. Number of timer instances available for timing operations. |  
| `qty_c` | Quantity of counters. Number of counter instances available for counting events. |  
| `qty_b` | Quantity of blocks. Number of functional blocks available for advanced operations. |  
  
**Returns**: Pointer to the initialized `ladder_ctx_t` structure, or `NULL` if initialization fails.  
  
### ladder_deinit  
  
Deinitializes the ladder context, freeing all associated resources.  
  
```c  
void ladder_deinit(ladder_ctx_t* ladder_ctx)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context to be deinitialized. |  
  
**Returns**: None.  
  
### ladder_add_network  
  
Adds a new network to the ladder context with a specified ID.  
  
```c  
ladder_network_t* ladder_add_network(ladder_ctx_t* ladder_ctx, uint32_t id)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `id` | Unique identifier for the network. |  
  
**Returns**: Pointer to the newly created `ladder_network_t` structure, or `NULL` if the operation fails.  
  
### ladder_execute  
  
Executes the ladder program, processing all networks and instructions.  
  
```c  
void ladder_execute(ladder_ctx_t* ladder_ctx)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context to execute. |  
  
**Returns**: None.  
  
### ladder_add_read_fn  
  
Registers a callback function for reading input values from hardware.  
  
```c  
void ladder_add_read_fn(ladder_ctx_t* ladder_ctx, _io_read read_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `read_fn` | Pointer to the read callback function. |  
  
**Returns**: None.  
  
### ladder_add_write_fn  
  
Registers a callback function for writing output values to hardware.  
  
```c  
void ladder_add_write_fn(ladder_ctx_t* ladder_ctx, _io_write write_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `write_fn` | Pointer to the write callback function. |  
  
**Returns**: None.  
  
### ladder_add_instruction  
  
Adds an instruction to a specific position in a network.  
  
```c  
bool ladder_add_instruction(ladder_network_t* network, uint32_t row, uint32_t column, ladder_instruction_e instruction, uint32_t id)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `network` | Pointer to the network where the instruction will be added. |  
| `row` | Row index in the network grid (0-based). |  
| `column` | Column index in the network grid (0-based). |  
| `instruction` | Type of instruction (e.g., `LADDER_INSTRUCTION_LD`). See `ladder_instruction_e` enum. |  
| `id` | Identifier for the instruction operand (e.g., input, output, timer ID). |  
  
**Returns**: `true` if the instruction was added successfully, `false` otherwise.  
  
### ladder_add_init_fn  
  
Registers a callback function for initializing or deinitializing I/O operations.  
  
```c  
void ladder_add_init_fn(ladder_ctx_t* ladder_ctx, _io_init init_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `init_fn` | Pointer to the init/deinit callback function. |  
  
**Returns**: None.  
  
### ladder_add_scan_end_fn  
  
Registers a callback function to be called at the end of each scan cycle.  
  
```c  
void ladder_add_scan_end_fn(ladder_ctx_t* ladder_ctx, _on_scan_end scan_end_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `scan_end_fn` | Pointer to the scan end callback function. |  
  
**Returns**: None.  
  
### ladder_add_instruction_fn  
  
Registers a callback function to be called for each instruction execution.  
  
```c  
void ladder_add_instruction_fn(ladder_ctx_t* ladder_ctx, _on_instruction instruction_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `instruction_fn` | Pointer to the instruction callback function. |  
  
**Returns**: None.  
  
### ladder_add_task_before_fn  
  
Registers a callback function to be called before each task cycle scan.  
  
```c  
void ladder_add_task_before_fn(ladder_ctx_t* ladder_ctx, _on_task_before task_before_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `task_before_fn` | Pointer to the task before callback function. |  
  
**Returns**: None.  
  
### ladder_add_task_after_fn  
  
Registers a callback function to be called after each task cycle scan.  
  
```c  
void ladder_add_task_after_fn(ladder_ctx_t* ladder_ctx, _on_task_after task_after_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `task_after_fn` | Pointer to the task after callback function. |  
  
**Returns**: None.  
  
### ladder_add_panic_fn  
  
Registers a callback function for panic situations.  
  
```c  
void ladder_add_panic_fn(ladder_ctx_t* ladder_ctx, _on_panic panic_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `panic_fn` | Pointer to the panic callback function. |  
  
**Returns**: None.  
  
### ladder_add_end_task_fn  
  
Registers a callback function to be called when a task ends.  
  
```c  
void ladder_add_end_task_fn(ladder_ctx_t* ladder_ctx, _on_end_task end_task_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `end_task_fn` | Pointer to the end task callback function. |  
  
**Returns**: None.  
  
### ladder_add_delay_fn  
  
Registers a callback function for implementing delays.  
  
```c  
void ladder_add_delay_fn(ladder_ctx_t* ladder_ctx, _delay delay_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `delay_fn` | Pointer to the delay callback function. |  
  
**Returns**: None.  
  
### ladder_add_millis_fn  
  
Registers a callback function for retrieving the current time in milliseconds.  
  
```c  
void ladder_add_millis_fn(ladder_ctx_t* ladder_ctx, _millis millis_fn)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `millis_fn` | Pointer to the millis callback function. |  
  
**Returns**: None.  
  
## Utility Functions  
  
This section documents utility functions from the header files `ladder_print.h`, `ladder_program_json.h`, and `ladder_program_check.h`.  
  
### ladder_print_program  
  
Prints the entire ladder program to the console for debugging purposes.  
  
```c  
void ladder_print_program(ladder_ctx_t* ladder_ctx)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: None.  
  
### ladder_print_network  
  
Prints a specific network within the ladder program.  
  
```c  
void ladder_print_network(ladder_network_t* network)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `network` | Pointer to the network to print. |  
  
**Returns**: None.  
  
### ladder_program_to_json  
  
Serializes the ladder program to a JSON string.  
  
```c  
char* ladder_program_to_json(ladder_ctx_t* ladder_ctx)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: Pointer to a dynamically allocated JSON string representing the ladder program. Caller must free this memory.  
  
### ladder_program_check  
  
Checks the integrity and validity of the ladder program.  
  
```c  
bool ladder_program_check(ladder_ctx_t* ladder_ctx)  
```  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: `true` if the program is valid, `false` otherwise.  

## Enums and Structures  
  
This section documents all enums and structures from the header files.  
  
### Enums  
  
#### ladder_instruction_e  
  
Defines the types of instructions supported by LadderLib.  
  
```c  
typedef enum {  
LADDER_INSTRUCTION_NOP, // No operation  
LADDER_INSTRUCTION_LD, // Load  
LADDER_INSTRUCTION_ST, // Store  
LADDER_INSTRUCTION_AND, // Logical AND  
LADDER_INSTRUCTION_OR, // Logical OR  
LADDER_INSTRUCTION_NOT, // Logical NOT  
LADDER_INSTRUCTION_TON, // Timer On Delay  
LADDER_INSTRUCTION_TOF, // Timer Off Delay  
LADDER_INSTRUCTION_CTU, // Counter Up  
LADDER_INSTRUCTION_CTD, // Counter Down  
LADDER_INSTRUCTION_END_NETWORK // End of network  
} ladder_instruction_e;  
```  
  
- **LADDER_INSTRUCTION_NOP**: Performs no operation, used as a placeholder.  
- **LADDER_INSTRUCTION_LD**: Loads a value from an input, memory, or other source.  
- **LADDER_INSTRUCTION_ST**: Stores a value to an output or memory.  
- **LADDER_INSTRUCTION_AND**: Performs a logical AND operation between operands.  
- **LADDER_INSTRUCTION_OR**: Performs a logical OR operation between operands.  
- **LADDER_INSTRUCTION_NOT**: Inverts the logical state of an operand.  
- **LADDER_INSTRUCTION_TON**: Implements a timer that activates after a delay.  
- **LADDER_INSTRUCTION_TOF**: Implements a timer that deactivates after a delay.  
- **LADDER_INSTRUCTION_CTU**: Increments a counter on each activation.  
- **LADDER_INSTRUCTION_CTD**: Decrements a counter on each activation.  
- **LADDER_INSTRUCTION_END_NETWORK**: Marks the end of a network’s instruction set.  
  
### Structures  
  
#### ladder_instruction_s  
  
Represents a single instruction within a network.  
  
```c  
typedef struct {  
ladder_instruction_e instruction;  
uint32_t id;  
bool value;  
} ladder_instruction_s;  
```  
  
- **instruction**: Type of instruction (e.g., `LADDER_INSTRUCTION_LD`).  
- **id**: Identifier for the operand (e.g., input, output, timer ID).  
- **value**: Current value of the instruction (true/false).  
  
#### ladder_network_s  
  
Represents a ladder network, which is a grid of instructions.  
  
```c  
typedef struct {  
uint32_t id;  
uint32_t rows_qty;  
uint32_t columns_qty;  
ladder_instruction_s** instructions;  
} ladder_network_s;  
```  
  
- **id**: Unique identifier for the network.  
- **rows_qty**: Number of rows in the network grid.  
- **columns_qty**: Number of columns in the network grid.  
- **instructions**: 2D array of pointers to `ladder_instruction_s` structures.  
  
#### ladder_ctx_s  
  
Represents the main ladder context, managing all networks and resources.  
  
```c  
typedef struct {  
uint32_t net_columns_qty;  
uint32_t net_rows_qty;  
uint32_t networks_qty;  
ladder_network_s** networks;  
uint32_t qty_m;  
uint32_t qty_i;  
uint32_t qty_q;  
uint32_t qty_t;  
uint32_t qty_c;  
uint32_t qty_b;  
bool* m;  
bool* i;  
bool* q;  
void* t;  
void* c;  
void* b;  
_io_read read_fn;  
_io_write write_fn;  
_io_init init_fn;  
_on_scan_end scan_end_fn;  
_on_instruction instruction_fn;  
_on_task_before task_before_fn;  
_on_task_after task_after_fn;  
_on_panic panic_fn;  
_on_end_task end_task_fn;  
_delay delay_fn;  
_millis millis_fn;  
} ladder_ctx_s;  
```  
  
- **net_columns_qty**: Number of columns in each network (max 32).  
- **net_rows_qty**: Number of rows in each network.  
- **networks_qty**: Total number of networks.  
- **networks**: Array of pointers to `ladder_network_s` structures.  
- **qty_m**: Number of memory bits.  
- **qty_i**: Number of input bits.  
- **qty_q**: Number of output bits.  
- **qty_t**: Number of timers.  
- **qty_c**: Number of counters.  
- **qty_b**: Number of blocks.  
- **m**: Array of memory bits.  
- **i**: Array of input bits.  
- **q**: Array of output bits.  
- **t**: Array of timer instances (opaque type).  
- **c**: Array of counter instances (opaque type).  
- **b**: Array of block instances (opaque type).  
- **read_fn**: Callback for reading inputs.  
- **write_fn**: Callback for writing outputs.  
- **init_fn**: Callback for I/O initialization/deinitialization.  
- **scan_end_fn**: Callback for end of scan cycle.  
- **instruction_fn**: Callback for each instruction.  
- **task_before_fn**: Callback before task cycle.  
- **task_after_fn**: Callback after task cycle.  
- **panic_fn**: Callback for panic situations.  
- **end_task_fn**: Callback for task completion.  
- **delay_fn**: Callback for delays.  
- **millis_fn**: Callback for current time.  
  
## Callback Prototypes  
  
This section lists all callback prototypes used by LadderLib, with their exact function signatures and purposes.  
  
### _io_read  
  
```c  
void (*_io_read)(ladder_ctx_t* ladder_ctx, uint32_t id)  
```  
  
**Description**: Reads hardware values and updates the input bits in the ladder context.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `id` | Identifier of the input to read. |  
  
**Returns**: None.  
  
### _io_write  
  
```c  
void (*_io_write)(ladder_ctx_t* ladder_ctx, uint32_t id)  
```  
  
**Description**: Writes hardware values based on the output bits in the ladder context.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `id` | Identifier of the output to write. |  
  
**Returns**: None.  
  
### _io_init  
  
```c  
bool (*_io_init)(ladder_ctx_t* ladder_ctx, uint32_t id, bool init_deinit)  
```  
  
**Description**: Initializes or deinitializes an I/O function.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
| `id` | Identifier of the I/O function. |  
| `init_deinit` | `true` to initialize, `false` to deinitialize. |  
  
**Returns**: `true` if successful, `false` otherwise.  
  
### _on_scan_end  
  
```c  
bool (*_on_scan_end)(ladder_ctx_t* ladder_ctx)  
```  
  
**Description**: Called at the end of each scan cycle for custom processing.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: `true` to continue execution, `false` to stop.  
  
### _on_instruction  
  
```c  
bool (*_on_instruction)(ladder_ctx_t* ladder_ctx)  
```  
  
**Description**: Called for each instruction executed during a scan.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: `true` to continue execution, `false` to stop.  
  
### _on_task_before  
  
```c  
bool (*_on_task_before)(ladder_ctx_t* ladder_ctx)  
```  
  
**Description**: Called before each task cycle scan begins.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: `true` to proceed with the scan, `false` to skip.  
  
### _on_task_after  
  
```c  
bool (*_on_task_after)(ladder_ctx_t* ladder_ctx)  
```  
  
**Description**: Called after each task cycle scan completes.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: `true` to continue, `false` to stop.  
  
### _on_panic  
  
```c  
void (*_on_panic)(ladder_ctx_t* ladder_ctx)  
```  
  
**Description**: Handles panic situations, such as critical errors.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: None.  
  
### _on_end_task  
  
```c  
void (*_on_end_task)(ladder_ctx_t* ladder_ctx)  
```  
  
**Description**: Called when a task ends, for cleanup or final actions.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `ladder_ctx` | Pointer to the ladder context. |  
  
**Returns**: None.  
  
### _delay  
  
```c  
void (*_delay)(long msec)  
```  
  
**Description**: Implements a delay in milliseconds.  
  
**Parameters:**  
  
| **Parameter** | **Description** |  
|---------------|-----------------|  
| `msec` | Duration of the delay in milliseconds. |  
  
**Returns**: None.  
  
### _millis  
  
```c  
uint64_t (*_millis)(void)  
```  
  
**Description**: Returns the time elapsed since system start in milliseconds.  
  
**Parameters**: None.  
  
**Returns**: Number of milliseconds as a 64-bit unsigned integer.
  
## Basic Usage  
To use Ladderlib, follow these steps:  
1. **Initialize Context**: Call `ladder_ctx_init` with appropriate quantities for networks, memory areas, inputs, outputs, counters, timers, data, and registers.  
2. **Define Program**: Populate the `ladder_s` structure by defining networks (`ladder_network_s`) and cells (`ladder_cell_s`), assigning instructions from `LADDER_INSTRUCTIONS` and configuring data or timers as needed.  
3. **Set Hardware Callbacks**: Provide functions for hardware interactions in `ladder_hw_s`, such as `_read_inputs_local` and `_write_outputs_local`, to interface with physical devices.  
4. **Set Event Callbacks**: Optionally, assign callbacks in `ladder_manage_s` for events like scan end (`_on_scan_end`) or panic handling (`_on_panic`).  
5. **Run Task**: Execute the ladder logic by calling `ladder_task` periodically, typically in a loop, to process the program and interact with hardware.  
6. **Monitor and Handle Errors**: Check `LADDER_STATE` and `LADDER_INS_ERROR` to monitor execution status and handle errors.  
7. **Deinitialize**: Call `ladder_ctx_deinit` to free resources when the program is no longer needed.  
  
### Example Workflow  
Below is a simplified example of how to structure a Ladderlib program:  
  
```c  
 // TODO: EXAMPLE
```  
  
This example initializes a context, sets hardware callbacks, defines a simple program, runs the task, and deinitializes the context. Actual implementations will vary based on specific hardware and logic requirements.  
  
## Limitations and Assumptions  
```c  
// TODO: DOC  
```


