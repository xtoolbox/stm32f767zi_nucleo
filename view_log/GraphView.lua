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
    self.lImage = QImage(60,self.rH,4)
    self.btnLoad = QPushButton("Loa&d")
    self.scroll = QScrollArea()
    self.frame = QFrame(self)
    self.leftFrame = QFrame(self)
    self.frame.minW = w or self.rW
    self.frame.minH = h or self.rH
    self.scroll.widget = self.frame

    self.leftFrame.minW = self.lWidth
   
    self.layout = QVBoxLayout{
      QHBoxLayout{ self.btnLoad, QLabel(""), stretch = "0,1"},
      QHBoxLayout{ self.leftFrame , self.scroll,}
    }
    self.rightPatiner = QPainter()
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
            s1 = s1 + math.random(300,1000)
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

function GraphView:update_graph(objs)
    if not objs then return end
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
   
    local ratio = (self.image.width-60)/(objs.max - objs.min)
    ratio = ratio

    local d = (objs.max - objs.min) / 20
    local text_pos = vpos
    for i=0, 20 do
        p2:setPen(QColor(self.lineColor) )
        p2:drawLine(self.l_offset + i*d*ratio, 0, self.l_offset + i*d*ratio, self.image.height)
        p2:setPen(QColor(self.textColor) )
        p2:drawText(self.l_offset + i*d*ratio + 1, text_pos, string.format( "%.1f",(d*i)/(216*1000) ))
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