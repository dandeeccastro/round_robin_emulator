# Round Robin Emulator
> An emulator for the Round Robin algorithm for process allocation and management 

## Installation and Usage 
- Clone the repo and `cd` into it
- To run with default configuration (generating a random process list), just `make run`
  - Alternatively, you can create a CSV file called `process_table.csv`, where you can add your own processes into the simulation
  - The fields expected by the CSV are `ARRIVAL TIME,DURATION,PRIORITY,`
    - The last comma is **necessary for parsing**, don't forget it!

