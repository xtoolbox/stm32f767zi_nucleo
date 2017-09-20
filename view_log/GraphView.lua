require("util")

class "GraphView"(QFrame)

function GraphView:__init(w,h)
    QFrame.__init(self)
    --self.minW = w or 400
    --self.minH = h or 300
    self.rW = w or 1600
    self.rH = h or 300
    self.lWidth = 60
    self.image = QImage(self.rW,self.rH,4)
    self.lImage = QImage(self.lWidth,self.rH,4)
    self.btnLoad = QPushButton("Loa&d")
    self.scroll = QScrollArea()
    self.frame = QFrame(self)
    self.leftFrame = QFrame(self)
    self.frame.minW = w or self.rW
    self.frame.minH = h or self.rH
    self.scroll.widget = self.frame

    self.leftFrame.minW = self.lWidth
   
    self.frame.cursor = QCursor(17)
    
    self.layout = QVBoxLayout{
      QHBoxLayout{ self.btnLoad, QLabel("   Use Ctrl + [Mouse Scroll] to Zoom in/Zoom out"), stretch = "0,1"},
      QHBoxLayout{ self.leftFrame , self.scroll,}
    }
    self.rightPatiner = QPainter()--self.image)
    self.rightPatiner:begin(self.image)
    self.rightPatiner:setPen(QColor("lightgray") )
    self.rightPatiner:setBrush(QBrush(QColor("lightgray")))
    self.rightPatiner:drawRect(0,0,self.rW,self.rH)
   
    self.leftPainter = QPainter()--self.lImage)
    self.leftPainter:begin(self.lImage)
    self.leftPainter:setPen(QColor("lightgray") )
    self.leftPainter:setBrush(QBrush(QColor("lightgray")))
    self.leftPainter:drawRect(0,0,self.lWidth,self.rH)

    self.frame.eventFilter = QPaintEvent.filter(
    function(obj, evt)
        local pt = QPainter()
        pt:begin(obj)
        pt:drawImage(0,0,self.image)
        pt:done()
    end
    )
    self.frame.eventFilter = QMouseEvent.filter(
    function(obj,evt)
        --self.frame.cursor = QCursor(18)
        if evt.type == 2  and evt.button == 1 then   -- left button press
            self.mouse_drag = true
            self.mouse_org_x = evt.x
            self.offset_org_x = self.l_offset
            self.frame.cursor = QCursor(18)
        elseif evt.type == 3 and evt.button == 1  then -- left button release
            self.mouse_drag = false
            self.frame.cursor = QCursor(17)
        elseif evt.type == 5 and self.mouse_drag then    -- mouse move
            self.l_offset = self.offset_org_x + (evt.x - self.mouse_org_x)
            --log(self.l_offset)
            self:update_graph(self.objs)
            self.frame:update()
        end
    end
    )
    
    self.frame.eventFilter = QWheelEvent.filter(
    function(obj, evt)
        if evt.modifiers == 0x04000000 then
        --log(evt.delta)
        self:change_ratio(evt.delta, evt.x)
        return true
        end
    end
    )
    
    self.leftFrame.eventFilter = QPaintEvent.filter(
    function(obj, evt)
        local pt = QPainter()
        pt:begin(obj)
        pt:drawImage(0,0,self.lImage)
        pt:done()
    end
    )

   
    self.btnLoad.clicked = function()
        self.idx = self.idx or 0
        self.idx = self.idx + 1
        logEdit:clear()
        dlg = QDialog(self)
        dlg.lua = QLuaEdit()
        dlg.lua.plainText = "return \n"
        dlg.OK = QPushButton("OK")
        dlg.layout = QVBoxLayout{
        dlg.lua,
        dlg.OK,
        }
        dlg.OK.clicked = function() dlg:accept() end
        
        local objs = {}
        if dlg:exec() == 1 then
            objs = loadstring(dlg.lua.plainText)()
        else
        
        objs = {
          { name = "test1" .. self.idx, 
            {start = 1122, stop = 1124, desc = "xx1"},
            {start = 1190, stop = 1214, desc = "xx2"},
          },
          { name = "test2" .. self.idx, 
            {start = 1100, stop = 1114, desc = "xx1"},
            {start = 1130, stop = 1154, desc = "xx2"},
          },
          { name = "test3" .. self.idx, 
            {start = 1112, stop = 1124, desc = "xx1"},
            {start = 1250, stop = 1154, desc = "xx2"},
          },
          { name = "test4" .. self.idx, 
            {start = 1129, stop = 1179, desc = "xx1"},
            {start = 1254,              desc = "xx2"},
          },
        }
        math.randomseed(QTime.currentTime().msec)
        objs = generate_test_data()
        end
        prepocess(objs)
        self.cur_ratio = nil
        self.org_ratio = nil
        self.l_offset = nil
        log(objs.min, objs.max)
        self:update_graph(objs)
        --self.objs = objs
        self:update()
        self.frame:update()
    end
    self.backColor = "white"
    self.textColor = "black"
    self.lineColor = "darkgray"
    self.eleColor = {
      "darkred",
      "darkblue",
      "darkgreen",
      "blue",
      "orange",
      "pink",
    }
    self.l_offset = 10
    self.h_offset = 10
    self.v_dist = 30
    self.v_offset = 40
    self.v_ratio = 0.4
end

function generate_test_data(ele_cnt, rec_cnt)
    local r = {}
    ele_cnt = ele_cnt or math.random(5,10)
    rec_cnt = rec_cnt or math.random(20,30)
    while #r < ele_cnt do
        local obj = {}
        obj.name = "Ele " .. #r
        local s1 = math.random(20,1000)
        while #obj < rec_cnt do
            local i = #obj+1
            obj[i] = {}
            obj[i].start = s1 + math.random(10,100)
            obj[i].stop = obj[i].start + math.random(100,300)
            obj[i].desc = "desc " .. i
            s1 = s1 + math.random(1000,2000)
        end
        r[#r+1] = obj
    end
   
   
    return r
end

function prepocess(objs)
   for i=1, #objs do
      log("-----------")
      local obj = objs[i]
      for j=1, #obj do
         objs.min = objs.min or obj[j].start
         objs.min = objs.min > obj[j].start and obj[j].start or objs.min
         obj[j].stop = obj[j].stop or (obj[j].start + 20)
         objs.max = objs.max or obj[j].stop
         objs.max = objs.max < obj[j].stop and obj[j].stop or objs.max
         log("  ", obj[j].start, obj[j].stop)
      end
   end
   
   for i=1, #objs do
      local obj = objs[i]
      for j=1, #obj do
         obj[j].start = obj[j].start - objs.min
         obj[j].stop = obj[j].stop - objs.min
      end
   end
end

function GraphView:change_ratio(dir,pos)
    local ratio = (self.image.width-60)/(self.objs.max - self.objs.min)
    self.org_ratio = self.org_ratio or ratio
    self.cur_ratio = self.cur_ratio or self.org_ratio
    local last_ratio = self.cur_ratio
    if dir > 0 then
        self.cur_ratio = self.cur_ratio * math.sqrt(2)
    else
        self.cur_ratio = self.cur_ratio / math.sqrt(2)
    end
    if self.cur_ratio < ratio/2 then
        self.cur_ratio = ratio/2
    end
    if self.cur_ratio > ratio*50 then
        self.cur_ratio = ratio*50
    end
    --local pos = self.scroll.horizontalScrollBar.value
    --log(pos)
    --self.l_offset = self.h_offset*(self.cur_ratio/self.org_ratio) + pos - pos*(self.cur_ratio/self.org_ratio)
    --[[
      off1 + r1 * x = p1
      off2 + r2 * x = p2
      here: p1 == p2
      so we have: off2 = p1 - ((p1 - off1) * ( r2/r1))
    --]]
    last_ratio = self.cur_ratio  / last_ratio
    self.l_offset = pos - (pos - self.l_offset)* last_ratio
    
    self:update_graph(self.objs)
    self.frame:update()
end

function time_str(tick)
    local t = tick / (216*1000)
    if true then
    --return string.format("%.1f %.1f", tick,t)
    end
    if t > 1 then
        return string.format("%.1f ms", t)
    end
    t = tick / 216
    return string.format("%.1f us", t)
end

function GraphView:update_graph(objs)
    if not objs then return end
    self.objs = objs
    local p1 = self.leftPainter--QPainter(self.lImage)
    p1:setPen(QColor(self.backColor) )
    p1:setBrush(QBrush(QColor(self.backColor)))
    p1:drawRect(0,0,self.lImage.width,self.lImage.height)
    local p2 = self.rightPatiner--QPainter(self.image)
    p2:setPen(QColor(self.backColor) )
    p2:setBrush(QBrush(QColor(self.backColor)))
    p2:drawRect(0,0,self.image.width,self.image.height)
    local vpos = self.v_offset
    p1:setPen(QColor(self.textColor) )
    for i=1,#objs do
        local obj = objs[i]
        p1:drawText(3, vpos, obj.name)
        vpos = vpos + self.v_dist
        --log(i, obj.name)
    end
    
    self.l_offset = self.l_offset or self.h_offset
    local ratio = (self.image.width-60)/(objs.max - objs.min)
    ratio = self.cur_ratio or ratio
    local grid_size = self.grid_size or ((self.image.width-60)/20)

    local d = (objs.max - objs.min) / 20
    local text_pos = vpos
    local grid_cnt = 20
    d = grid_size / ratio
    grid_cnt = math.ceil((objs.max - objs.min) / d)
    --log("grid_cnt" , grid_cnt, d)
    for i=0, grid_cnt do
        p2:setPen(QColor(self.lineColor) )
        p2:drawLine(self.l_offset + i*d*ratio, 0, self.l_offset + i*d*ratio, self.image.height)
        p2:setPen(QColor(self.textColor) )
        p2:drawText(self.l_offset + i*d*ratio + 1, text_pos, time_str((d*i)) )
        text_pos = text_pos == vpos and vpos+10 or vpos
    end
    
    vpos = self.v_offset
    for i=1,#objs do
        local obj = objs[i]
        p2:setPen(QColor(self.lineColor) )
        p2:drawLine(0, vpos, self.image.width, vpos)
        for j=1,#obj do
            local v = obj[j]
            p2:setPen(QColor(self.eleColor[i]) )
            p2:setBrush(QBrush(QColor(self.eleColor[i])))
            p2:drawRect(v.start*ratio+self.l_offset, vpos-self.v_dist*self.v_ratio, (v.stop-v.start)*ratio, self.v_dist*self.v_ratio )
            if v.desc then
                p2:setPen(QColor(self.textColor) )
                p2:drawText( v.start*ratio+self.l_offset, vpos-self.v_dist*self.v_ratio, v.desc  )
            end
        end
        
        vpos = vpos + self.v_dist
        --log(i, obj.name)
    end

    
    --self.lImage:save("test.jpg")
    --self:update()
end