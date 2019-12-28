### barrel2019
This application is used to maintain water pressure in a given range.  

System can operate in automatic and manual modes.

- automatic mode
    - the system maintains the pressure in a given range automatically.
    - when pressure reaches low threshold - pump will switch ON automatically.
    - when pressure reaches high threshold - pump will switch OFF automatically.
    - START (or 'select') button will start the pump manually. The pump will stop automatically once the high threshold will be reached.
    - STOP (or 'select') button will stop the pump manually. The pump will start automatically once the low threshold will be reached.
- manual mode
    - If low threshold equals high threshold, that means you are in the manual mode.
    - In this mode, the system doesn't check the current pressure. **Be careful**.
    - Only you switch the pump ON and OFF. **Be careful**.
    - START (or 'select') button will start the pump manually. No any sensors used to automatically stop the pump. 
    - STOP (or 'select') button will stop the pump manually. No any sensors used to automatically start the pump.

The system starts in **automatic** mode with thresholds **1.8** / **3.8** Pa

### Connections
- **A0**, **D4..D10** used by LCD + buttons shield
    - A0 - buttons: UP / DOWN / LEFT / RIGHT / SELECT
    - D4..D9 - LED shield
    - D10 - LED shield back light control (must be input)
- **A1** used for Pressure sensor taken from Chrysler 5.7 (4.7) engine oil pressure sensor
    - [05149062AA](https://www.rockauto.com/en/parts/mopar,5149062AB,oil+filter+adapter,14022)
    - or [PS317](https://www.rockauto.com/en/partsearch/?partnum=PS317)
    - or [PS317T](https://www.rockauto.com/en/partsearch/?partnum=PS317T)


- **A2** will be used for buttons AUTO / MANUAL / START / STOP
- **D11** used to control the pump relay
- **D12** used to read empty-barrel switch to prevent pump idle spinning without water
- **D2** - green LED will inform about automatic mode 
- **D3** - yellow LED will inform about manual mode 


- **A3..A5** - free
- **D0..D1** - free
- **D13** - free

### Links
- project based on [barrel2018](https://github.com/alexr007/barrel2018)
- Arduino official [site](https://www.arduino.cc/en/main/software)
