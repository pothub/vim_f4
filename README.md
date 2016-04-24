Build and Test Environment based on Ubuntu 16.04 LTS for the My F4-Mini board

# 1. How to Setup the Environment
	sudo apt-get update
	sudo apt install gcc-arm-none-eabi
	sudo apt-get install python-serial
	sudo apt-get install cu
# 2. Usage
## 2.1 Build Project
	cd ~/main/
	make t
## 2.2 Flush Project
Change to STM32 uart write mode

Press the RESET while pressing the SW0

If write and view uart debug

	make f

If only write

	make ff

