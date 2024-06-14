# Nucleo/blackpill Remora-STM32F4xx-W5500 test branch. 
------------------------------------------

Stepgen, blink and digital io are the only modules supported at the moment. 

Default config that loads on startup is an led blink module on PC13

Configs are loaded via tftp , refer to the remora documents for this process
https://remora-docs.readthedocs.io/en/latest/firmware/ethernet-config.html

Board will not start to blink until ethernet connection is established. 

Dont know how to use serial monitor. 

# W5500 connection


 - PA12 SPI CS
 - PB13 SCK
 - PB14  MISO
 - PB15 MOSI


# Boards
- Nucleo : working , connects to linuxcnc needs more testing
- Blackpill : working , connects to linuxcnc needs more testing
- BTT Octopus 446 : working , connects to linuxcnc needs more testing 



# TO DO and known issues

- Encoder modules need to be ported. Still a WIP, but I do not know where to point the signals.
- UART console output needs to be added.
- Add a platformio.ini define for SPI CS pin

------------------------------------------

![Logo](/readme_images/logo_sm.jpg)
# Remora-STM32F4xx-W5500
Port of Remora to uFlexiNet+FlexiHAL

# Installation instructions
Start by flashing the Remora UF2 firmware to your FlexiHAL.

This firmware uses a basically unchanged version of the Remora-eth-0.3.0 ethernet component that is located here:

https://github.com/Expatria-Technologies/Remora/tree/main/LinuxCNC/Components/Remora-eth

All testing is done with the Flexi-Pi LinuxCNC image from here:

https://github.com/Expatria-Technologies/remora-flexi-hal

From your linuxcnc home folder, clone the Remora repo and move to the Remora-eth component folder.
```
git clone https://github.com/Expatria-Technologies/Remora.git
cd Remora/LinuxCNC/Components/Remora-eth
```

Compile the component using halcompile
```
sudo halcompile --install remora-eth.c
```

Then upload your configuration file to the FlexiHAL using the upload_config.py script that should be in your configuration folder:
```
python3 upload_config.py FlexiHAL-config.txt
```
A basic configuration to get started is located here: 

https://github.com/Expatria-Technologies/linuxcnc_configs/tree/main/flexi-hal-uFlexiNet-XYZ

To use the RS485 interface on the FlexiHAL you can set up a virtual serial port and use socat to bridge between userspace Modbus components and Remora.  If the RS485 component is included in the board config txt, it will listen on port 27183 for UDP packets and forward them to the RS485 port:

```
socat pty, link=/tmp/virtualcom0, raw udp:10.10.10.10:27183 &
```

All credit to Scotta and Terje IO as this project draws heavily from Remora (especially the RP2040 and STM32-W5500 ports) and also GRBLHAL (especially for the SPI DMA and networking stack)

<img src="/readme_images/Board_installed.jpg" width="800">
