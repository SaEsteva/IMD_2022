cmd_/home/rpisesteva/Documents/IMD/Char_devices/devtmpfs/Module.symvers := sed 's/\.ko$$/\.o/' /home/rpisesteva/Documents/IMD/Char_devices/devtmpfs/modules.order | scripts/mod/modpost -m -a  -o /home/rpisesteva/Documents/IMD/Char_devices/devtmpfs/Module.symvers -e -i Module.symvers   -T -