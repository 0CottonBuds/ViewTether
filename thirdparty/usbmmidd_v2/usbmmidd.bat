@cd /d "%~dp0"

@goto %PROCESSOR_ARCHITECTURE%
@exit

:AMD64
@cmd /c deviceinstaller64.exe install usbmmidd.inf usbmmidd
@goto end

:x86
@cmd /c deviceinstaller.exe install usbmmidd.inf usbmmidd
:end

@pause
