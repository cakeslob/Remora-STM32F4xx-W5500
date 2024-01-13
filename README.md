# Nucleo/blackpill Remora-STM32F4xx-W5500 test branch. 
------------------------------------------

Attempts to get boards to work with a vanilla config, then work on breaking out the pins and adding other modules.

Stepgen, blink and digital io are the only modules supported at the moment. 

Default config that loads on startup is an led blink module on PA5

Configs are loaded via tftp , refer to the remora documents for this process
https://remora-docs.readthedocs.io/en/latest/firmware/ethernet-config.html

Board will not start to blink until ethernet connection is established. 

Dont know how to use serial monitor. 

# W5500 connection


 - PA15 SPI CS
 - PB3 SCK
 - PB4  MISO
 - PB5 MOSI


# Boards
- Nucleo : working , connects to linuxcnc needs more testing
- Blackpill : working , connects to linuxcnc needs more testing
- BTT Octopus 446 : working , connects to linuxcnc needs more testing 



# TO DO and known issues

- Need to fix config loading. Configuration can only be loaded once. When loading the second time, something goes wrong, the memory gets locked/protected and the board returns to default config.
- PWM and Encoder modules need to be ported. Still a WIP, but I do not know where to point the signals.
- UART console output needs to be added.
- Add a platformio.ini define for SPI CS pin

------------------------------------------

![Logo](/readme_images/logo_sm.jpg)
# Remora-STM32F4xx-W5500
Port of Remora to uFlexiNet+FlexiHAL

All credit to Scotta and Terje IO as this project draws heavily from Remora (especially the RP2040 and STM32-W5500 ports) and also GRBLHAL (especially for the SPI DMA and networking stack)

<img src="/readme_images/Board_installed.jpg" width="800">
