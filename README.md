# README: Process Injection Tool

## Overview

This CPP File (`ProcessInjection.cpp`) is designed to perform process injection. Process injection involves injecting code into the virtual address space of another process, allowing the injected code to run within the context of that process. This is a basic example I created at home to inject shellcode into the Notepad process. It's a simple approach to process injection, intended as a learning exercise. The shellcode used in this example spawns the Calculator app.


## Usage

1. **Preparation**: 
    - Obtain the `process_injector.cpp` file and compile it to a exe.
    - Ensure that the target process (the process you want to inject into) is running.
    - Obtain the PID of the target process (I use Notepad)
    ```powershell
    get-process -Name "notepad" | Select-Object Id
    ```


2. **Running the Executable**:
    - Open a command prompt.
    - Navigate to the directory where `ProcessInjection.exe` is located.

3. **Basic Command Syntax**:

    ```powershell
    ProcessInjection.exe <PID>
    ```

    **Parameters**:
    - `<PID>`: The Process ID of the target process.

4. **Example Usage**:


5. **Monitoring and Logging**:

    - I installed Sysmon to monitor the associated Event IDs. I noticed a Sysmon Event ID 1, which highlighted the process injection.

⚠️ **Disclaimer:** This project is for educational purposes only. It is designed to help individuals understand how malware operates so they can better defend against it.
