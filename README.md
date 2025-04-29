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
Ladder logic programs consist of "rungs" connecting two vertical "rails," mimicking electrical relay circuits. Each rung contains conditions (e.g., contacts) and actions (e.g., coils). The instructions in `ladder.h` are defined in the `LADDER_INSTRUCTIONS` enum, which assigns unique codes to each instruction, corresponding to functions executed by the PLC. The following sections detail each instruction, grouped by category, with their instruction codes, descriptions, and symbols.  
  
## Basic Logic Instructions  

### Instruction Summary Table  
The following table summarizes the instructions, their codes, categories, and symbols.  
  
| Instruction | Code | Category | Symbol | Description |  
|-------------|------|----------|--------|-------------|  
| NOP | 0 | Specialized | None | No operation, placeholder |  
| CONN | 1 | Specialized | Vertical lines | Connects rungs or branches |  
| NEG | 2 | Specialized | Circle before contact | Inverts logic state |  
| NO | 3 | Basic Logic | `--| |--` | Normally Open contact |  
| NC | 4 | Basic Logic | `--|/|--` | Normally Closed contact |  
| RE | 5 | Specialized | Contact with upward arrow | Rising edge detection |  
| FE | 6 | Specialized | Contact with downward arrow | Falling edge detection |  
| COIL | 7 | Basic Logic | `--( )--` | Output coil |  
| COILL | 8 | Basic Logic | `--(L)--` | Latched coil |  
| COILU | 9 | Basic Logic | `--(U)--` | Unlatched coil |  
| TON | 10 | Timer | Box labeled "TON" | Timer On Delay |  
| TOFF | 11 | Timer | Box labeled "TOFF" | Timer Off Delay |  
| TP | 12 | Timer | Box labeled "TP" | Timer Pulse |  
| CTU | 13 | Counter | Box labeled "CTU" | Count Up |  
| CTD | 14 | Counter | Box labeled "CTD" | Count Down |  
| MOVE | 15 | Specialized | Box labeled "MOVE" | Moves data |  
| SUB | 16 | Arithmetic | Box labeled "-" | Subtraction |  
| ADD | 17 | Arithmetic | Box labeled "+" | Addition |  
| MUL | 18 | Arithmetic | Box labeled "*" | Multiplication |  
| DIV | 19 | Arithmetic | Box labeled "/" | Division |  
| MOD | 20 | Arithmetic | Box labeled "%" | Modulus |  
| SHL | 21 | Bitwise | Box labeled "SHL" | Shift Left |  
| SHR | 22 | Bitwise | Box labeled "SHR" | Shift Right |  
| ROL | 23 | Bitwise | Box labeled "ROL" | Rotate Left |  
| ROR | 24 | Bitwise | Box labeled "ROR" | Rotate Right |  
| AND | 25 | Bitwise | Box labeled "AND" | Bitwise AND |  
| OR | 26 | Bitwise | Box labeled "OR" | Bitwise OR |  
| XOR | 27 | Bitwise | Box labeled "XOR" | Bitwise XOR |  
| NOT | 28 | Bitwise | Box labeled "NOT" | Bitwise NOT |  
| EQ | 29 | Comparison | Box labeled "=" | Equal |  
| GT | 30 | Comparison | Box labeled ">" | Greater Than |  
| GE | 31 | Comparison | Box labeled ">=" | Greater Than or Equal |  
| LT | 32 | Comparison | Box labeled "<" | Less Than |  
| LE | 33 | Comparison | Box labeled "<=" | Less Than or Equal |  
| NE | 34 | Comparison | Box labeled "<>" | Not Equal |  
| FOREIGN | 35 | Specialized | Box labeled "FOREIGN" | External function call |  
| TMOVE | 36 | Specialized | Box labeled "TMOVE" | Table data move |  
| INV | 37 | Specialized | None | Invalid instruction |

## Basic Instructions
Basic instructions manage Input/Output functions
  
### NO (Normally Open Contact)  
- **Instruction Code:** `LADDER_INS_NO` (3)  
- **Description:** The Normally Open contact allows logic flow when its associated input is true, acting like a switch that closes when activated. It is used to check if an input condition (e.g., a sensor) is met.  
- **Symbol:** `--| |--`  
- **Example Use:** Checking if a pushbutton is pressed to activate a motor.  
  
### NC (Normally Closed Contact)  
- **Instruction Code:** `LADDER_INS_NC` (4)  
- **Description:** The Normally Closed contact allows logic flow when its associated input is false, acting like a switch that opens when activated. It is used to proceed when a condition is not met.  
- **Symbol:** `--|/|--`  
- **Example Use:** Ensuring a safety switch is not triggered before starting a process.  
  
### COIL (Output Coil)  
- **Instruction Code:** `LADDER_INS_COIL` (7)  
- **Description:** The Coil instruction sets an output to true when the rung's logic is true, controlling devices like motors or lights. It represents the action taken when conditions are satisfied.  
- **Symbol:** `--( )--`  
- **Example Use:** Turning on a conveyor belt when all conditions are met.  
  
### COILL (Coil Latch)  
- **Instruction Code:** `LADDER_INS_COILL` (8)  
- **Description:** The Coil Latch sets an output to true and maintains it (latches) even if the input conditions change, until unlatched. It is used for memory functions in control systems.  
- **Symbol:** `--(L)--`  
- **Example Use:** Keeping a pump running after a start button is released.  
  
### COILU (Coil Unlatch)  
- **Instruction Code:** `LADDER_INS_COILU` (9)  
- **Description:** The Coil Unlatch resets a latched output to false, releasing the latched state. It is paired with `COILL` for set/reset operations.  
- **Symbol:** `--(U)--`  
- **Example Use:** Stopping a latched pump when a stop button is pressed.  
  
## Timer Instructions  
Timer instructions manage time-based operations, delaying or pulsing outputs.  
  
### TON (Timer On Delay)  
- **Instruction Code:** `LADDER_INS_TON` (10)  
- **Description:** The Timer On Delay starts timing when its input is true. After a preset time, the output becomes true. It is used to delay actions, such as waiting before starting a motor.  
- **Symbol:** A rectangular box labeled "TON" with input, preset time, and output connections.  
- **Example Use:** Delaying a fan activation for 10 seconds after a heater starts.  
  
### TOFF (Timer Off Delay)  
- **Instruction Code:** `LADDER_INS_TOFF` (11)  
- **Description:** The Timer Off Delay starts timing when its input goes false. After a preset time, the output becomes false. It is used to extend the duration of an output.  
- **Symbol:** A rectangular box labeled "TOFF" with input, preset time, and output connections.  
- **Example Use:** Keeping a light on for 5 seconds after a switch is turned off.  
  
### TP (Timer Pulse)  
- **Instruction Code:** `LADDER_INS_TP` (12)  
- **Description:** The Timer Pulse generates a pulse of a specified duration when triggered by a true input. It is used for creating timed signals, such as a brief alarm.  
- **Symbol:** A rectangular box labeled "TP" with input, preset time, and output connections.  
- **Example Use:** Activating a buzzer for 2 seconds when a fault is detected.  
  
## Counter Instructions  
Counter instructions track the number of events or occurrences.  
  
### CTU (Count Up)  
- **Instruction Code:** `LADDER_INS_CTU` (13)  
- **Description:** The Count Up counter increments its count each time the input transitions from false to true, up to a preset value. It is used to count items or events.  
- **Symbol:** A rectangular box labeled "CTU" with input, reset, preset, and output connections.  
- **Example Use:** Counting the number of boxes on a conveyor belt.  
  
### CTD (Count Down)  
- **Instruction Code:** `LADDER_INS_CTD` (14)  
- **Description:** The Count Down counter decrements its count each time the input transitions from false to true, down to zero. It is used for countdown tasks.  
- **Symbol:** A rectangular box labeled "CTD" with input, reset, preset, and output connections.  
- **Example Use:** Tracking remaining items in a batch process.  
  
## Arithmetic Instructions  
These instructions perform mathematical operations on data.  
  
### ADD (Addition)  
- **Instruction Code:** `LADDER_INS_ADD` (17)  
- **Description:** Adds two values and stores the result in a specified location. It is used for calculations like summing sensor readings.  
- **Symbol:** A rectangular box labeled "+" with two inputs and one output.  
- **Example Use:** Adding the outputs of two flow sensors.  
  
### SUB (Subtraction)  
- **Instruction Code:** `LADDER_INS_SUB` (16)  
- **Description:** Subtracts one value from another and stores the result. It is used for differential calculations.  
- **Symbol:** A rectangular box labeled "-" with two inputs and one output.  
- **Example Use:** Calculating the difference in pressure between two points.  
  
### MUL (Multiplication)  
- **Instruction Code:** `LADDER_INS_MUL` (18)  
- **Description:** Multiplies two values and stores the result. It is used for scaling or area calculations.  
- **Symbol:** A rectangular box labeled "*" with two inputs and one output.  
- **Example Use:** Calculating power by multiplying voltage and current.  
  
### DIV (Division)  
- **Instruction Code:** `LADDER_INS_DIV` (19)  
- **Description:** Divides one value by another and stores the result. It is used for ratio calculations.  
- **Symbol:** A rectangular box labeled "/" with two inputs and one output.  
- **Example Use:** Determining speed by dividing distance by time.  
  
### MOD (Modulus)  
- **Instruction Code:** `LADDER_INS_MOD` (20)  
- **Description:** Computes the remainder of dividing one value by another. It is used for cyclic operations or remainder checks.  
- **Symbol:** A rectangular box labeled "%" with two inputs and one output.  
- **Example Use:** Checking if a counter value is even or odd.  
  
## Bitwise Operation Instructions  
These instructions manipulate bits within data values.  
  
### SHL (Shift Left)  
- **Instruction Code:** `LADDER_INS_SHL` (21)  
- **Description:** Shifts the bits of a value to the left by a specified number of positions, filling with zeros. It is used for bit manipulation or scaling.  
- **Symbol:** A rectangular box labeled "SHL" with input, shift amount, and output.  
- **Example Use:** Doubling a binary value by shifting left.  
  
### SHR (Shift Right)  
- **Instruction Code:** `LADDER_INS_SHR` (22)  
- **Description:** Shifts the bits of a value to the right by a specified number of positions, filling with zeros or sign bits. It is used for division or bit extraction.  
- **Symbol:** A rectangular box labeled "SHR" with input, shift amount, and output.  
- **Example Use:** Halving a binary value by shifting right.  
  
### ROL (Rotate Left)  
- **Instruction Code:** `LADDER_INS_ROL` (23)  
- **Description:** Rotates the bits of a value to the left, with the most significant bit wrapping around to the least significant position. It is used for circular shifts.  
- **Symbol:** A rectangular box labeled "ROL" with input, rotate amount, and output.  
- **Example Use:** Rotating status flags in a control word.  
  
### ROR (Rotate Right)  
- **Instruction Code:** `LADDER_INS_ROR` (24)  
- **Description:** Rotates the bits of a value to the right, with the least significant bit wrapping around to the most significant position. It is used for circular shifts.  
- **Symbol:** A rectangular box labeled "ROR" with input, rotate amount, and output.  
- **Example Use:** Cycling through a set of control bits.  
  
### AND (Bitwise AND)  
- **Instruction Code:** `LADDER_INS_AND` (25)  
- **Description:** Performs a bitwise AND operation on two values, setting output bits to 1 only where both inputs are 1. It is used for masking or condition checks.  
- **Symbol:** A rectangular box labeled "AND" with two inputs and one output.  
- **Example Use:** Masking specific bits in a status register.  
  
### OR (Bitwise OR)  
- **Instruction Code:** `LADDER_INS_OR` (26)  
- **Description:** Performs a bitwise OR operation on two values, setting output bits to 1 where either input is 1. It is used for combining conditions.  
- **Symbol:** A rectangular box labeled "OR" with two inputs and one output.  
- **Example Use:** Combining multiple fault flags.  
  
### XOR (Bitwise XOR)  
- **Instruction Code:** `LADDER_INS_XOR` (27)  
- **Description:** Performs a bitwise XOR operation on two values, setting output bits to 1 where inputs differ. It is used for toggling or comparison.  
- **Symbol:** A rectangular box labeled "XOR" with two inputs and one output.  
- **Example Use:** Detecting changes in bit patterns.  
  
### NOT (Bitwise NOT)  
- **Instruction Code:** `LADDER_INS_NOT` (28)  
- **Description:** Performs a bitwise NOT operation, inverting all bits of a value. It is used for logical negation.  
- **Symbol:** A rectangular box labeled "NOT" with one input and one output.  
- **Example Use:** Inverting a control signal.  
  
## Comparison Instructions  
These instructions compare values and produce boolean outputs.  
  
### EQ (Equal)  
- **Instruction Code:** `LADDER_INS_EQ` (29)  
- **Description:** Checks if two values are equal, outputting true if they are. It is used for equality checks in control logic.  
- **Symbol:** A rectangular box labeled "=" with two inputs and one output.  
- **Example Use:** Verifying if a temperature reading matches a setpoint.  
  
### GT (Greater Than)  
- **Instruction Code:** `LADDER_INS_GT` (30)  
- **Description:** Checks if one value is greater than another, outputting true if so. It is used for threshold checks.  
- **Symbol:** A rectangular box labeled ">" with two inputs and one output.  
- **Example Use:** Checking if pressure exceeds a safe limit.  
  
### GE (Greater Than or Equal)  
- **Instruction Code:** `LADDER_INS_GE` (31)  
- **Description:** Checks if one value is greater than or equal to another, outputting true if so. It is used for inclusive threshold checks.  
- **Symbol:** A rectangular box labeled ">=" with two inputs and one output.  
- **Example Use:** Ensuring a tank level is at or above a minimum.  
  
### LT (Less Than)  
- **Instruction Code:** `LADDER_INS_LT` (32)  
- **Description:** Checks if one value is less than another, outputting true if so. It is used for lower-bound checks.  
- **Symbol:** A rectangular box labeled "<" with two inputs and one output.  
- **Example Use:** Detecting if a speed is below a target.  
  
### LE (Less Than or Equal)  
- **Instruction Code:** `LADDER_INS_LE` (33)  
- **Description:** Checks if one value is less than or equal to another, outputting true if so. It is used for inclusive lower-bound checks.  
- **Symbol:** A rectangular box labeled "<=" with two inputs and one output.  
- **Example Use:** Verifying a flow rate is at or below a maximum.  
  
### NE (Not Equal)  
- **Instruction Code:** `LADDER_INS_NE` (34)  
- **Description:** Checks if two values are not equal, outputting true if they differ. It is used for change detection.  
- **Symbol:** A rectangular box labeled "<>" or "≠" with two inputs and one output.  
- **Example Use:** Detecting a change in a sensor value.  
  
## Specialized Instructions  
These instructions provide additional functionality, some of which may be specific to ladderlib.  
  
### NOP (No Operation)  
- **Instruction Code:** `LADDER_INS_NOP` (0)  
- **Description:** Performs no operation, acting as a placeholder or for timing purposes. It does not affect the program state.  
- **Symbol:** Typically not represented, or a blank space in the diagram.  
- **Example Use:** Reserving space for future instructions.  
  
### CONN (Connection)  
- **Instruction Code:** `LADDER_INS_CONN` (1)  
- **Description:** Likely used to connect different parts of the ladder diagram, such as branching or joining rungs. It facilitates complex logic structures.  
- **Symbol:** Vertical lines or a specific connector symbol, such as a junction point.  
- **Example Use:** Linking parallel branches in a rung.  
  
### NEG (Negate)  
- **Instruction Code:** `LADDER_INS_NEG` (2)  
- **Description:** Inverts the logic state of an input, turning true to false or vice versa. It is used to reverse conditions.  
- **Symbol:** A small circle before a contact or coil, indicating inversion.  
- **Example Use:** Inverting a stop signal to enable a process.  
  
### RE (Rising Edge)  
- **Instruction Code:** `LADDER_INS_RE` (5)  
- **Description:** Detects a rising edge, triggering when an input transitions from false to true. It is used for event-driven actions.  
- **Symbol:** A contact with an upward arrow, indicating rising edge detection.  
- **Example Use:** Starting a counter when a button is pressed.  
  
### FE (Falling Edge)  
- **Instruction Code:** `LADDER_INS_FE` (6)  
- **Description:** Detects a falling edge, triggering when an input transitions from true to false. It is used for event-driven actions.  
- **Symbol:** A contact with a downward arrow, indicating falling edge detection.  
- **Example Use:** Resetting a system when a sensor deactivates.  
  
### MOVE (Move Data)  
- **Instruction Code:** `LADDER_INS_MOVE` (15)  
- **Description:** Copies data from one memory location to another without modification. It is used for data transfer.  
- **Symbol:** A rectangular box labeled "MOVE" with source and destination parameters.  
- **Example Use:** Transferring a sensor value to a display register.  
  
### TMOVE (Table Move)  
- **Instruction Code:** `LADDER_INS_TMOVE` (36)  
- **Description:** Likely used for moving data within tables or arrays, possibly for batch processing or data manipulation. The exact functionality may depend a lot on ladderlib's implementation.  
- **Symbol:** A rectangular box labeled "TMOVE" with table and destination parameters.  
- **Example Use:** Moving a set of values to a new memory block.  
  
### FOREIGN (Foreign Function)  
- **Instruction Code:** `LADDER_INS_FOREIGN` (35)  
- **Description:** Allows calling external or user-defined functions not natively supported by ladder logic. It enables integration with custom code.  
- **Symbol:** A rectangular box labeled "FOREIGN" or with a specific function identifier.  
- **Example Use:** Calling a custom algorithm for complex calculations.  
  
### INV (Invalid)  
- **Instruction Code:** `LADDER_INS_INV` (37)  
- **Description:** Marks the beginning of invalid instruction codes, not intended for use in programs. It serves as a boundary for valid instructions.  
- **Symbol:** Not represented in diagrams, as it is not a functional instruction.  
- **Example Use:** None, as it is not used in programming.  
  
### MULTI (Multi-cell Instruction)  
- **Instruction Code:** `LADDER_INS_MULTI` (255, 0xff)  
- **Description:** Indicates that the cell is part of a larger instruction spanning multiple cells, used for complex operations requiring additional data.  
- **Symbol:** Varies depending on the specific multi-cell instruction, often a series of connected boxes.  
- **Example Use:** Implementing a multi-step operation like a sequence control.  
  
## LADDER_STATE  
Defines 6 possible states for program execution:  
- `STOPPED`: Program is halted.  
- `RUNNING`: Program is actively executing.  
- `ERROR`: An error has occurred.  
- `EXIT_TSK`: Task is exiting.  
- `NULLFN`: Null function encountered.  
- `INV`: Invalid state.  
  
These states manage the lifecycle of the ladder logic program, ensuring proper control and error handling.  
  
## LADDER_INS_ERROR  
Provides 8 error codes for instruction execution:  
- `OK`: Successful execution.  
- `GETPREVVAL`: Error retrieving previous value.  
- `GETDATAVAL`: Error getting data value.  
- `SETDATAVAL`: Error setting data value.  
- `NOFOREIGN`: No foreign function found.  
- `NOTABLE`: Table not found.  
- `OUTOFRANGE`: Value out of range.  
- `FAIL`: General failure.  
  
These codes help developers diagnose and handle issues during program execution.  
  
## LADDER_TYPE  
Defines 15 data types and 4 basetime options:  
- **Types**: `NONE`, `M` (Memory Register), `Q` (Output), `I` (Input), `Cd` (Counter Done), `Cr` (Counter Running), `Td` (Timer Done), `Tr` (Timer Running), `IW` (Input Word), `QW` (Output Word), `C` (Counter), `T` (Timer), `D` (Data Register), `CSTR` (Constant String), `REAL` (Floating Point).  
- **Basetime Options**: `BASETIME_MS`, `BASETIME_10MS`, `BASETIME_100MS`, `BASETIME_SEC`, `BASETIME_MIN`.  
  
These types support various data representations, crucial for defining inputs, outputs, and internal variables.  
  
# Structures  
Ladderlib uses structures to organize ladder logic programs, hardware interactions, and execution management. Below is a detailed breakdown of key structures.  
  
### ladder_cell_s  
Represents an individual cell in a ladder logic diagram, akin to a contact or coil in a relay circuit.  
  
| **Field** | **Description** |  
|-----------------|----------------------------------------------|  
| `state` | Current state of the cell (e.g., on/off). |  
| `vertical_bar` | Indicates vertical connection presence. |  
| `code` | Instruction code from `LADDER_INSTRUCTIONS`. |  
| `data_qty` | Number of data elements associated. |  
| `data` | Array of `ladder_value_s` for cell data. |  
  
### ladder_network_s  
Represents a network or rung, a horizontal line in the ladder diagram containing multiple cells.  
  
| **Field** | **Description** |  
|------------|------------------------------------------|  
| `enable` | Flag to enable/disable the network. |  
| `cells` | Array of `ladder_cell_s` for the network. |  
  
### ladder_timer_s  
Manages timer functions for time-based operations.  
  
| **Field** | **Description** |  
|-----------------|-------------------------------------|  
| `time_stamp` | Timestamp for timer operations. |  
| `acc` | Accumulator for timer value. |  
  
### ladder_s  
The main structure for the ladder logic program, encapsulating overall state and configuration.  
  
| **Field** | **Description** |  
|-------------|----------------------------------------------|  
| `state` | Current state from `LADDER_STATE`. |  
| `last` | Tracks last executed instruction, network, etc. |  
| `quantity` | Quantities of memory, inputs, outputs, etc. |  
  
### ladder_hw_s  
Handles hardware interactions, providing function pointers for I/O and timing.  This Functions are external and hardware dependant
  
| **Field** | **Description** |  
|-----------------------------|------------------------------------------|  
| `_read_inputs_local` | Reads local hardware inputs. |  
| `_write_outputs_local` | Writes to local outputs. |  
| `_read_inputs_remote` | Reads remote inputs. |  
| `_write_outputs_remote` | Writes to remote outputs. |  
| `_millis` | Returns current time in milliseconds. |  
| `_delay` | Delays execution for a specified time. |  
  
### ladder_manage_s  
Manages execution events through callbacks.  This functions are optionals.
  
| **Field** | **Description** |  
|----------------------|------------------------------------------|  
| `_on_scan_end` | Called at the end of a scan cycle. |  
| `_on_instruction` | Called per instruction execution. |  
| `_on_task_before` | Called before task execution. |  
| `_on_task_after` | Called after task execution. |  
| `_on_panic` | Handles panic situations. |  
| `_on_end_task` | Called at task completion. |  
  
### Other Structures  
Additional structures include:  
- `ladder_instructions_ioc_s`: Defines input/output/cell configurations.  
- `ladder_value_s`: Holds data with type and union (u32, i32, cstr, real).  
- `ladder_memory_s`: Manages memory areas (M, I, Q, etc.).  
- `ladder_prev_scan_vals_s`: Stores previous scan values.  
- `ladder_registers_s`: Manages registers (IW, QW, C, D, R).  
- `ladder_scan_internals_s`: Tracks scan timing information.  
- `ladder_foreign_function_s`: Defines custom functions with ID, name, and execution logic.  
- `ladder_foreign_s`: Manages a collection of foreign functions.  
- `ladder_ctx_s`: The main context, combining all components for program execution.  
  
## Functions  
The API provides functions for initializing, running, and managing ladder logic programs. Below is a detailed description of each.  
  
### ladder_ctx_init  
Initializes the ladder context, setting up structures and resources.  
  
| **Parameter** | **Description** |  
|---------------------|----------------------------------------------|  
| `net_columns_qty` | Number of columns in networks (max 32). |  
| `net_rows_qty` | Number of rows in networks. |  
| `networks_qty` | Number of networks. |  
| `qty_m`, `qty_i`, etc. | Quantities for memory, inputs, outputs, etc. |  
  
**Returns**: Pointer to `ladder_ctx_s`.  
  
### ladder_ctx_deinit  
Deinitializes the context, freeing allocated resources.  
  
| **Parameter** | **Description** |  
|----------------|----------------------------------|  
| `ladder_ctx` | Pointer to `ladder_ctx_s`. |  
  
### ladder_task  
Executes the ladder logic task, processing networks and cells.  
  
| **Parameter** | **Description** |  
|----------------|----------------------------------|  
| `ladderctx` | Pointer to `ladder_ctx_s`. |  
  
### ladder_clear_program  
Clears the current ladder logic program, resetting the context.  
  
| **Parameter** | **Description** |  
|----------------|----------------------------------|  
| `ladder_ctx` | Pointer to `ladder_ctx_s`. |  
  
### ladder_add_foreign  
Adds custom (foreign) functions to the context for extended functionality.  
  
| **Parameter** | **Descrip## Instruction Summary Table  

The following table summarizes the instructions, their codes, categories, and symbols.  
  

| Instruction | Code | Category | Symbol | Description |  
|-------------|------|----------|--------|-------------|  
| NOP | 0 | Specialized | None | No operation, placeholder |  
| CONN | 1 | Specialized | Vertical lines | Connects rungs or branches |  
| NEG | 2 | Specialized | Circle before contact | Inverts logic state |  
| NO | 3 | Basic Logic | `--|  |--` | Normally Open contact |  
| NC | 4 | Basic Logic | `--|/|--` | Normally Closed contact |  
| RE | 5 | Specialized | Contact with upward arrow | Rising edge detection |  
| FE | 6 | Specialized | Contact with downward arrow | Falling edge detection |  
| COIL | 7 | Basic Logic |  `--( )--`  | Output coil |  
| COILL | 8 | Basic Logic |  `--(L)--`  | Latched coil |  
| COILU | 9 | Basic Logic |  `--(U)--`  | Unlatched coil |  
| TON | 10 | Timer | Box labeled "TON" | Timer On Delay |  
| TOFF | 11 | Timer | Box labeled "TOFF" | Timer Off Delay |  
| TP | 12 | Timer | Box labeled "TP" | Timer Pulse |  
| CTU | 13 | Counter | Box labeled "CTU" | Count Up |  
| CTD | 14 | Counter | Box labeled "CTD" | Count Down |  
| MOVE | 15 | Specialized | Box labeled "MOVE" | Moves data |  
| SUB | 16 | Arithmetic | Box labeled "-" | Subtraction |  
| ADD | 17 | Arithmetic | Box labeled "+" | Addition |  
| MUL | 18 | Arithmetic | Box labeled "*" | Multiplication |  
| DIV | 19 | Arithmetic | Box labeled "/" | Division |  
| MOD | 20 | Arithmetic | Box labeled "%" | Modulus |  
| SHL | 21 | Bitwise | Box labeled "SHL" | Shift Left |  
| SHR | 22 | Bitwise | Box labeled "SHR" | Shift Right |  
| ROL | 23 | Bitwise | Box labeled "ROL" | Rotate Left |  
| ROR | 24 | Bitwise | Box labeled "ROR" | Rotate Right |  
| AND | 25 | Bitwise | Box labeled "AND" | Bitwise AND |  
| OR | 26 | Bitwise | Box labeled "OR" | Bitwise OR |  
| XOR | 27 | Bitwise | Box labeled "XOR" | Bitwise XOR |  
| NOT | 28 | Bitwise | Box labeled "NOT" | Bitwise NOT |  
| EQ | 29 | Comparison | Box labeled "=" | Equal |  
| GT | 30 | Comparison | Box labeled ">" | Greater Than |  
| GE | 31 | Comparison | Box labeled ">=" | Greater Than or Equal |  
| LT | 32 | Comparison | Box labeled "<" | Less Than |  
| LE | 33 | Comparison | Box labeled "<=" | Less Than or Equal |  
| NE | 34 | Comparison | Box labeled "<>" | Not Equal |  
| FOREIGN | 35 | Specialized | Box labeled "FOREIGN" | External function call |  
| TMOVE | 36 | Specialized | Box labeled "TMOVE" | Table data move |  
| INV | 37 | Specialized | None | Invalid instruction |tion** |  
|----------------|------------------------------------------|  
| `ladder_ctx` | Pointer to `ladder_ctx_s`. |  
| `fn_init` | Initialization function for foreign functions. |  
| `init_data` | Data for initialization. |  
| `qty` | Number of foreign functions to add. |  
  
### ladder_fn_cell  
Assigns a function to a specific cell in the ladder logic.  
  
| **Parameter** | **Description** |  
|----------------|------------------------------------------|  
| `ladder_ctx` | Pointer to `ladder_ctx_s`. |  
| `network` | Network index. |  
| `row` | Row index within the network. |  
| `column` | Column index within the network. |  
| `function` | Function code to assign. |  
| `foreign_id` | ID of foreign function, if applicable. |  
  
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
