# opcua

## Pre Condition

- gcc 9
- cmake min v3.16

## Build Procedure

### 1st Build
Run build.sh from command line to complile the Server or Client or both using below params
	a) ./build.sh server : Build Server only
	b) ./build.sh client : Build Client only
	c) ./build.sh all    : Build Server and Client both
	
With this command,
	- cmake will install the build environmnt and create a direcetory "build" in the project root directory for keeping all buildable outcomes.
	- Generated excecutable will be install in "bin" folder in the project root directory.
	
### Incremental build

After 1st build, for any change, incremental build can be run by below command from command line
	cmake --build build -v
	
Note: Whenever new option to build, always run the build.sh
	
	
	

	
