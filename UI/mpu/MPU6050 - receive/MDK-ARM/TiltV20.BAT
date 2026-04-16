SET PATH=D:\Keil_v5\ARM\ARMCC\Bin;C:\Program Files (x86)\Common Files\Oracle\Java\javapath;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;C:\Users\root001\AppData\Local\Microsoft\WindowsApps
SET CPU_TYPE=STM32L071RBTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=TiltV20
SET CPU_CLOCK=0x00B71B00
"D:\Keil_v5\ARM\ARMCC\Bin\ArmAsm" --Via ".\tiltv20\startup_stm32l071xx._ia"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\tilt_tsk.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\lora_tsk.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\main.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\stm32l0xx_it.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\stm32l0xx_hal_msp.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\sysmanage_tsk.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\stm32l0xx_hal_adc.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\stm32l0xx_hal_adc_ex.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmCC" --Via ".\tiltv20\stm32l0xx_hal_i2c.__i"
"D:\Keil_v5\ARM\ARMCC\Bin\ArmLink" --Via ".\TiltV20\TiltV20.lnp"
