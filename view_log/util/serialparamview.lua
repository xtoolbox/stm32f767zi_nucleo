-- serialparamview.lua
-- 创建一个串口参数设置界面。self.serial为参数所对应的串口实例
-- 可以通过self.serial.readyReady信号来读取串口数据，或是通过self.onDataReady信号
-- 向串口写数据，self.serial:write({1,2,3,4,5,6,7,8,9,10})
--[[
   serialParamView = SerialParamView()
   serialParamView.serial.readyRead = function()
       data = serialParamView.serial:readAll()
       -- process serial data here
       ...
       -- write data back
       serialParamView.serial:write(data)
   end
--]]
class "SerialParamView"(QFrame)

SerialParamView.SIMPLE = 0
SerialParamView.VERTICAL = 1
SerialParamView.HORIZONTAL = 2
function SerialParamView:__init(mode, lineStatus, name)
    QFrame.__init(self)
    self.windowTitle = tr("Serial Port Viewer")
    self.portList = QComboBox();
    ports = enumCOM() --QSerialPort.enumPort()
    table.foreach(ports, function (k,v) self.portList:addItem(v.portName, v)  end)
    self.serial = QSerialPort(self)
    self.mode = mode or SerialParamView.SIMPLE
    self.lineStatus = lineStatus or false
    self.groupName = name or tr("Serial setting")

    self.baudList = QComboBox{ minW = 70, QSerialPort.ValidBaudRate(), editable = true }
    self.baudList.lineEdit.inputMask = "00000000"
    self.parityList = QComboBox{ QSerialPort.ValidParity() }
    self.dataBitsList = QComboBox{ QSerialPort.ValidDataBits() }
    self.stopBitsList = QComboBox{ QSerialPort.ValidStopBits() }
    self.flowList = QComboBox{ QSerialPort.ValidFlow() }
    self.settingString = QLineEdit(self.serial.settingString){
        readonly = true,
        minw = 100,
    }

    self.serial.flowControl = QSerialPort.FLOW_OFF
    self.baudList.currentIndex = self.serial.baudRate
    self.parityList.currentIndex = self.serial.parity
    self.dataBitsList.currentIndex = self.serial.dataBits
    self.stopBitsList.currentIndex = self.serial.stopBits
    self.flowList.currentIndex = self.serial.flowControl

    --self.baudList.currentIndexChanged = {self.serial,self.serial.setBaudRate}
    self.baudList.editTextChanged = function(text)
        idx = self.baudList:findText(text)
        if idx > 0 and idx < QSerialPort.BAUDLAST then
            self.serial:setBaudRate(idx)
        else
            bd = tonumber(text)
            if bd then
                self.serial:setBaudRate(bd)
            end
        end
        logEdit:append(self.serial.settingString)
    end

    self.parityList.currentIndexChanged = {self.serial,self.serial.setParity}
    self.dataBitsList.currentIndexChanged = {self.serial,self.serial.setDataBits}
    self.stopBitsList.currentIndexChanged = {self.serial,self.serial.setStopBits}
    self.flowList.currentIndexChanged = {self.serial,self.serial.setFlowControl}
    self.serial.settingChange = {self.settingString, self.settingString.setText}
    self.btnOpen = QPushButton(tr("Open"))
    self.btnRefresh = QPushButton(tr("Refresh"))

    self.lsdtr = QCheckBox("DTR"){toggled = {self.serial,self.serial.setDtr }}
    self.lsrts = QCheckBox("RTS"){toggled = {self.serial,self.serial.setRts }}
    self.lsdcd = QCheckBox("DCD"){enabled = false}
    self.lsdsr = QCheckBox("DSR"){enabled = false}
    self.lscts = QCheckBox("CTS"){enabled = false}
    self.lsri = QCheckBox("RI"){enabled = false}

    if self.mode == SerialParamView.HORIZONTAL then
        if self.lineStatus then
        self.settingGroup = QGroupBox(self.groupName){
        layout =
            QVBoxLayout{
                QHBoxLayout{
                    QVBoxLayout{QLabel(tr("Port:")), self.portList},
                    QVBoxLayout{QLabel(tr("Baud:")), self.baudList},
                    QVBoxLayout{QLabel(tr("Parity:")), self.parityList},
                    QVBoxLayout{QLabel(tr("Data Bits:")), self.dataBitsList},
                    QVBoxLayout{QLabel(tr("Stop Bits:")), self.stopBitsList},
                    QVBoxLayout{QLabel(tr("Flow:")), self.flowList},
                    QVBoxLayout{QLabel(), self.btnOpen, },
                },
                QHBoxLayout{
                    QLabel(tr("Line status:")),
                    --self.settingString,
                    self.lsdtr,
                    self.lsrts,
                    self.lsdcd,
                    self.lsdsr,
                    self.lscts,
                    self.lsri,
                    self.btnRefresh,
                }
            }
        }
        else
        self.settingGroup = QGroupBox(self.groupName){
        layout =
            QVBoxLayout{
                QHBoxLayout{
                    QVBoxLayout{QLabel(tr("Port:")), self.portList},
                    QVBoxLayout{QLabel(tr("Baud:")), self.baudList},
                    QVBoxLayout{QLabel(tr("Parity:")), self.parityList},
                    QVBoxLayout{QLabel(tr("Data Bits:")), self.dataBitsList},
                    QVBoxLayout{QLabel(tr("Stop Bits:")), self.stopBitsList},
                    QVBoxLayout{QLabel(tr("Flow:")), self.flowList},
                    QVBoxLayout{self.btnRefresh, self.btnOpen, },
                }
            }
        }
        end
    elseif self.mode == SerialParamView.VERTICAL then
        if self.lineStatus then
        self.settingGroup = QGroupBox(self.groupName){
        layout =
            QHBoxLayout{
                QFormLayout{
                    {QLabel(tr("Port:")), self.portList},
                    {QLabel(tr("Baud:")), self.baudList},
                    {QLabel(tr("Parity:")), self.parityList},
                    {QLabel(tr("Data Bits:")), self.dataBitsList},
                    {QLabel(tr("Stop Bits:")), self.stopBitsList},
                    {QLabel(tr("Flow:")), self.flowList},
                    {self.btnRefresh, self.btnOpen, },
                },
                QVBoxLayout{
                    QLabel(tr("Line status:")),
                    --self.settingString,
                    self.lsdtr,
                    self.lsrts,
                    self.lsdcd,
                    self.lsdsr,
                    self.lscts,
                    self.lsri,
                    QLabel(),
                    strech = "0,0,0,0,0,0,0,1"
                }
            }
        }
        else
        self.settingGroup = QGroupBox(self.groupName){
        layout =
            QHBoxLayout{
                QFormLayout{
                    {QLabel(tr("Port:")), self.portList},
                    {QLabel(tr("Baud:")), self.baudList},
                    {QLabel(tr("Parity:")), self.parityList},
                    {QLabel(tr("Data Bits:")), self.dataBitsList},
                    {QLabel(tr("Stop Bits:")), self.stopBitsList},
                    {QLabel(tr("Flow:")), self.flowList},
                    {self.btnRefresh, self.btnOpen, },
                }
            }
        }
        end
    else
        self.settingGroup = QGroupBox(self.groupName){
            layout = QHBoxLayout{
                QLabel(tr("Port:")), self.portList,
                QLabel(tr("Baud:")), self.baudList,
                self.btnOpen, self.btnRefresh,
            }
        }
    end
--[[
    self.settingGroup = QGroupBox(tr("Serial setting")){
        layout = QHBoxLayout{
            QLabel(tr("Port:")), self.portList,
            QLabel(tr("Baud:")), self.baudList,
            self.btnOpen, self.btnRefresh,
        }
    }

    self.settingGroup = QGroupBox(tr("Serial setting")){
        layout =
            QVBoxLayout{
                QHBoxLayout{
                    QVBoxLayout{QLabel(tr("Port:")), self.portList},
                    QVBoxLayout{QLabel(tr("Baud:")), self.baudList},
                    QVBoxLayout{QLabel(tr("Parity:")), self.parityList},
                    QVBoxLayout{QLabel(tr("Data Bits:")), self.dataBitsList},
                    QVBoxLayout{QLabel(tr("Stop Bits:")), self.stopBitsList},
                    QVBoxLayout{QLabel(tr("Flow:")), self.flowList},
                    QVBoxLayout{QLabel(), self.btnOpen, },
                },
                QHBoxLayout{
                    QLabel(tr("Line status:")),
                    --self.settingString,
                    self.lsdtr,
                    self.lsrts,
                    self.lsdcd,
                    self.lsdsr,
                    self.lscts,
                    self.lsri,
                    self.btnRefresh,
                }
            }
    }
--]]

    function update_ls(ls)
        self.lsdcd.checked = bit_and(ls, QSerialPort.LS_DCD) ~= 0
        self.lsdsr.checked = bit_and(ls, QSerialPort.LS_DSR) ~= 0
        self.lscts.checked = bit_and(ls, QSerialPort.LS_CTS) ~= 0
        self.lsri.checked = bit_and(ls, QSerialPort.LS_RI) ~= 0
    end


    self.btnOpen.clicked =
    function ()
        if self.serial.isOpen then
            self.serial:close()
            self.btnOpen.text = tr("Open")
        else
            i = self.portList.currentIndex
            portInfo = self.portList:itemData(i)
            local name = portInfo.portName
            if name then
            logEdit:append("open: " .. name .. " with setting: " .. self.serial.settingString)
            self.serial.portName = name
            res = self.serial:open()
            if res then
                self.btnOpen.text = tr("Close")
                update_ls(self.serial.lineStatus)
                logEdit:append("Success ...")
            else
                logEdit:append("Fail:  " .. self.serial.errorString)
            end
            end
        end
    end

    self.btnRefresh.clicked = function()
        self.portList:clear()
        ports = enumCOM() --QSerialPort.enumPort()
        table.foreach(ports, function (k,v) self.portList:addItem(v.portName, v)  end)
    end

    self.serial.readyRead = function()
        x = self.serial:readAll()
        if self.onDataReady then
            if type(self.onDataReady) == "table" then
                self.onDataReady[2](self.onDataReady[1],x)
            else
                self.onDataReady(x)
            end
        end
    end

    self.layout = QHBoxLayout{
        self.settingGroup,
    }
    self.serial.lineChanged = function(ls)
        update_ls(ls)
    end

    self.serial.connected = function(info)
        logEdit:append(info.portName .. "   connected")
    end

    self.serial.disconnected = function(info)
        logEdit:append(info.portName .. "   disconnected")
    end
end

function enumCOM()
    return QSerialPort.enumPort()
--[[
    local proc = QProcess()
    local r = ""
    proc.readyReadStandardOutput = function()
        r = r .. toString(proc:readAllStandardOutput())
    end
    proc:start("cmd",{"/c","mode"})
    proc:waitForFinished()
    local port = {}
    for w in string.gmatch(r, "(COM%d+)") do
        port[#port+1]  = {portName = w}
    end
    return port
--]]
end