class "FilePicker" (QFrame)
function FilePicker:__init(filter, maxCount, cfgFile, isSave)
    QFrame.__init(self)
    self.filter = filter or "All(*)"
    self.path = QLineEdit() 
    self.btnLoad = QPushButton("...")
    self.btnRecent = QPushButton(tr("Recent"))
    self.maxCount = maxCount or 10
    self.cfgFile = cfgFile or "recent_list.ini"
	self.maxH = 40
    self.isSave = isSave

    if QFile.exists(self.cfgFile) then
        self.recent_file_list = dofile(self.cfgFile) or {}
    else
        self:save_recent_list()
    end

    self:load_recent_list()

    self.btnLoad.clicked = function()
        local file
        if self.isSave then
        file = QCommonDlg.getSaveFileName("Save File", self.path.text, self.filter)
        else
        file = QCommonDlg.getOpenFileName("Open File", self.path.text, self.filter)
        end
            if file ~= "" and (QFile.exists(file) or self.isSave) then
                local findStr = false
                for i=1,#self.recent_file_list do
                    if file == self.recent_file_list[i] then
                        findStr = true
                    end
                    if findStr then self.recent_file_list[i] = self.recent_file_list[i+1] end
                end
                self.recent_file_list[#self.recent_file_list + 1] = file
                self:save_recent_list()
                self:load_recent_list()
				if self.getFile then self.getFile(file) end
            end
        end
   self.layout = QHBoxLayout{
      self.path,
      self.btnLoad,
      self.btnRecent,
   }
end

function FilePicker:load_recent_list()
    self.btnRecent.menu = QMenu()
    local start = 1
    if #self.recent_file_list > self.maxCount then
        start = #self.recent_file_list - self.maxCount + 1
    end
    for i=start,#self.recent_file_list do
        local str = self.recent_file_list[i]
        local act = QAction(str)
        act.triggered = function()
            self.path.text = str
			if self.getFile then self.getFile(str) end
        end
        self.btnRecent.menu:addAction(act)
        self.path.text = str
    end
end

function FilePicker:save_recent_list()
    self.recent_file_list = self.recent_file_list or {}
    local file = io.open(self.cfgFile, "w+")
    if file then
        file:write("return {", "\n")
        local start = 1
        if #self.recent_file_list > self.maxCount then
            start = #self.recent_file_list - self.maxCount + 1
        end
        for i=start,#self.recent_file_list do
            file:write("[[", self.recent_file_list[i] ,"]],\n")
        end
        file:write("}", "\n")
        file:close()
    end
end

--dlg = QDialog()
--dlg.layout = QHBoxLayout{ FilePicker("lua(*.lua);;All(*)", 3) }
--dlg:exec()