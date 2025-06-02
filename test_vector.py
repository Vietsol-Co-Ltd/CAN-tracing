import wmi

c = wmi.WMI()

for device in c.Win32_PnPEntity():
    if device.Manufacturer and "Vector Informatik GmbH" in device.Manufacturer:
        print(device.Name)