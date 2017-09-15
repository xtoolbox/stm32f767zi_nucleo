-- log info to the logEdit
function log(v, ...)
    local r = tostring(v)
    for k,v in pairs({...}) do
        r = r .. "  " .. tostring(v)
    end
    logEdit:append(r)
end

function switch(c)
  local swtbl = {
    casevar = c,
    caseof = function (self, code)
      local f
      if (self.casevar) then
        f = code[self.casevar] or code.default
      else
        f = code.default
      end
      if f then
        if type(f)=="function" then
          return f(self.casevar,self)
        else
          return f
        end
      end
    end
  }
  return swtbl
end

--[[ sample usage for switch
switch("123123"):caseof{
    [1]   = function (x) log(x,"one") end,
    [2]   = function (x) log(x,"two") end,
    [3]   = 12345, -- just return 12345
    ["123123"] = function () end,
    default = function (x) log(x,"default") end,
}
--]]


function parse_ui(dest, controls)
    for k,v in pairs(controls) do
        r = v.findChildren(dest)
        for k1,v1 in pairs(r) do
            dest[v1.objectName] = v1
            log("find: " .. v1.objectName)
        end
    end
end

-- load ui from .ui file
function load_ui(dest, name, export_controls)
    local ld = QUiLoader()
    --dest.layout = ld:load(name).layout
    ld:load(name,dest)
    parse_ui(dest, export_controls)
end

--[[ sample usage for load_ui
    frame = QFrame()
    -- we only care QPushButton, QComboBox and QLineEdit
    load_ui(frame, "test.ui", {QPushButton, QComboBox, QLineEdit})
    -- after load ui, we can use the btn, combobox lineedit now
    frame.btn1.clicked = function() end
    selected = frame.comboBox1.currentIndex
    frame.lineEdit1.text = "123123"
--]]

function makecall(func, ...)
    if func then
    if type(func) == "table" then
        return func[2](func[1], ...)
    else
        return func(...)
    end
    end
end

function table_append(r, v)
    for i=1,#v do
        r[#r+1] = v[i]
    end
    return r
end

function  string2table(str, len)
    local r = {}
    local codec = QTextCodec.codecForLocale()
    local text = codec:fromUnicode(str)
    len = len or #text
    for i=1,len do
        if i <= #text then
            r[#r+1] = text[i]
        else
            r[#r+1] = 0
        end
    end
    return r
end


-- fix bit operation error

function _G.bitand(x,y)
    local x = QUtil.fromUint32(x)
    local y = QUtil.fromUint32(y)
    for i=1,#x do
        x[i] = QUtil.bitand(x[i],y[i])
    end
    return QUtil.toUint32(x)
end

function _G.bitor(x,y)
    local x = QUtil.fromUint32(x)
    local y = QUtil.fromUint32(y)
    for i=1,#x do
        x[i] = QUtil.bitor(x[i],y[i])
    end
    return QUtil.toUint32(x)
end

function _G.bitxor(x,y)
    local x = QUtil.fromUint32(x)
    local y = QUtil.fromUint32(y)
    for i=1,#x do
        x[i] = QUtil.bitxor(x[i],y[i])
    end
    return QUtil.toUint32(x)
end