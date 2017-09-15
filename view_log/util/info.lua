-- info.lua   ����: lxyppc
-- ��lua����ʵ�ֵ�XToolbox�ĺ��װ����ĵ�
-- ����һ��������̬������صı�����������������൱�������ռ�
dynamicHelp = {}
-- �õ�������content�е�ֵ
function dynamicHelp.pick_value(content)
    table.sort(content)
    return content
end
-- �õ�������content�е�Key
function dynamicHelp.pick_key(content)
    local res = {}
    for k,v in pairs(content) do
        if k:sub(1,2) ~= "__" then
            res[#res + 1] = k
        end
    end
    return dynamicHelp.pick_value(res)
end

dynamicHelp.ext_class = {
"gl"        ,
"glu"       ,
"table"     ,
"string"    ,
"io"        ,
"os"        ,
}

function dynamicHelp.is_ext_class(name)
	for i = 1, #dynamicHelp.ext_class do
		if name == dynamicHelp.ext_class[i] then
			return true
		end
	end
	return false
end

-- �õ�����ע�������
function dynamicHelp.register_classes()
    local classes = class_names() -- class_names ��luabindע��ĺ��������ڵõ����е�����
	for i = 1, #dynamicHelp.ext_class do
		classes[#classes + 1] = dynamicHelp.ext_class[i]
	end
    table.sort(classes)    
    return classes
end
dynamicHelp.classFilter = QLineEdit()  -- ��classFilter�е��ַ�����������
dynamicHelp.nameFilter = QLineEdit()  -- ��nameFilter�е��ַ�����������
dynamicHelp.tab = QTabWidget()  -- ����һ��TabWidget�����������ĸ��ֳ�Ա
dynamicHelp.baseClass = QLabel(tr("Base class:").." ") -- ������ʾ����ı�ǩ
-- ����������ҳ�����ؼ��ʵı�ǩ
dynamicHelp.online_help = QLabel(tr("Search Help:") .." "){ openExternalLinks = true }
dynamicHelp.reg_classes = dynamicHelp.register_classes() -- ����������ע�����
-- ����һ�������б�����reg_classes��ʼ��
dynamicHelp.classList = QListWidget{ dynamicHelp.reg_classes }
dynamicHelp.currentClassName = ""
-- TabWidget�и������ݵĲ�����
dynamicHelp.member_lists = {
    -- �����б�����          ��ʾ��tab�ϵ�����   class_info�ж�Ӧ����        ��������ĺ���
  { list = QListWidget(), name = tr("methods"), field = "methods",        pick = dynamicHelp.pick_key},
  { list = QListWidget(), name = tr("attributes"),    field = "attributes",     pick = dynamicHelp.pick_value},
  { list = QListWidget(), name = tr("static"), field = "static_methods", pick = dynamicHelp.pick_key},
  { list = QListWidget(), name = tr("constants"),    field = "constants",      pick = dynamicHelp.pick_key},
}
-- �������߰�������
function dynamicHelp.upadte_online(text)
    local site = "doc.qt.nokia.com"
    if dynamicHelp.currentClassName == "gl" then
        if dynamicHelp.tab.currentIndex == 2 then
            text = "gl" .. text
        end
        site = "opengl.org"
    end
    dynamicHelp.online_help.text = tr("Search Help:").."  " ..
        [[<a href="http://www.google.com.hk/search?q=]] 
        .. text .. [[%20site:]] .. site .. [[&btnI=i">]]
        .. text .. "</a>"
end
-- ��ʼ�����е��б������뵽tab��
for k,v in pairs(dynamicHelp.member_lists) do
    dynamicHelp.tab:addTab(v.list,tr(v.name))
    v.list.currentTextChanged = dynamicHelp.upadte_online
end
-- ��̬������߲��֣�����һ��������������һ�������б�
dynamicHelp.frm1 = QFrame{
    layout = QVBoxLayout{ -- ��ֱ����
        QHBoxLayout{
            QLabel(tr("Class Name Filter:")),
            dynamicHelp.classFilter,
        },
        dynamicHelp.classList,
    }
}
-- �ұ߲��֣������ǻ��������м��ǳ�Ա�б����������߰���
dynamicHelp.frm2 = QFrame{
    layout = QVBoxLayout{  -- ��ֱ����
        dynamicHelp.baseClass,
        QHBoxLayout{
            QLabel(tr("Name Filter:")),
            dynamicHelp.nameFilter,
        },
        dynamicHelp.tab,
        dynamicHelp.online_help,
    }
}
-- ��������������ʱ����ת����ע��Ļ��ദ
dynamicHelp.baseClass.linkActivated = function(link)
    dynamicHelp.classList:clear()
    dynamicHelp.classList:addItems(dynamicHelp.reg_classes)
    local res = dynamicHelp.classList:findItems(link,0) --��������Ҫ�����
    if res and #res > 0 then -- ����У������������б�ĵ�ǰ��
        dynamicHelp.classList.currentRow = res[1].row
    end
end
-- �������б��еĵ�ǰ��ı�ʱ�����³�Ա�б��е�����
dynamicHelp.classList.currentTextChanged = function(text)
    dynamicHelp.currentClassName = text
    local info = class_info(_G[text]) --�õ�ѡ�е�������Ӧ����Ϣ
    local bases = dynamicHelp.pick_value(info.bases) -- ��ȡ������Ϣ
    local r = tr("Base class:") .. " "
    for k,v in pairs(bases) do -- Ϊ����������������
        local ref = [[<a href="]] .. v ..[[">]] .. v .. "</a>"
        r = r .. ref .. ", "
    end
    if dynamicHelp.is_ext_class(text) then
        info = {}
        info.static_methods = {}
        info.constants = {}
        info.attributes = {}
        info.methods = {}
        for k,v in pairs(_G[text]) do
            if type(v) == "function" then
                info.static_methods[k] = v
            elseif type(v) == "number" then
                info.constants[k] = v
            end
        end
    end
    --����baseClass��openExternalLinksΪfalse����������೬����ʱ�ᴥ��linkActivated�ź�
    dynamicHelp.baseClass.text = r
    for k,v in pairs(dynamicHelp.member_lists) do -- ����������
        v.list:clear() -- ����Ӧ���б����
        local items = v.pick(info[v.field]) -- ���ø���info����Ӧ����õ��������б�
        -- ��Tab�и�������ֺ�����ϳ�Ա������
        dynamicHelp.tab:setTabText(k-1, v.name .. "(" .. #items .. ")")
        v.items = items
        v.list:addItems(items)
    end
end
-- ���������е��ı��ı�ʱ��ֻ��ʾƥ��ɹ������� 
dynamicHelp.classFilter.textChanged = function(str)
    local r = {}
    for k,v in pairs(dynamicHelp.reg_classes) do
        if string.match(v:upper(),str:upper()) then --ƥ����Դ�Сд
            r[#r+1] = v
        end
    end
    dynamicHelp.classList:clear()
    dynamicHelp.classList:addItems(r) -- ��ƥ��ɹ������������б�
end
-- ���������е��ı��ı�ʱ��ֻ��ʾƥ��ɹ��ĳ�Ա��
dynamicHelp.nameFilter.textChanged = function(str)
    -- ���ݵ�ǰtab���������õ�list�е�Ԫ�ز�ƥ��
    local list = dynamicHelp.member_lists[dynamicHelp.tab.currentIndex+1]
    local r = {}
    for k,v in pairs(list.items) do
        if string.match(v:upper(),str:upper()) then --ƥ����Դ�Сд
            r[#r+1] = v
        end
    end
    list.list:clear()
    list.list:addItems(r) -- ��ƥ��ɹ������������б�
end
-- ��splitter��Ϊ��̬������������
dynamicHelp.splitter = QSplitter{
    childrenCollapsible = false, --��ֹ��ȫ����Splitter�е�����
    dynamicHelp.frm1, --��/�ϲ���
    dynamicHelp.frm2, --��/�²���
}
-- ��splitter���뵽һ��DockWidget��
dynamicHelp.dock = QDockWidget(tr("Dynamic Help")){dynamicHelp.splitter}
-- ��dock����main window�У����ҷ��ڵײ���Qt::BottomDockWidgetArea = 8
mainWindow:addDockWidget(8, dynamicHelp.dock)
-- ��dock����ı�ʱ������splitter�Ĳ���
dynamicHelp.dock.dockLocationChanged = function(area)
    -- area 1,2 ��ʾdock�����ң�4,8��ʾdock������
    -- orientation 1��ʾˮƽ���֣�2��ʾ��ֱ����
    dynamicHelp.splitter.orientation = area>2 and 1 or 2 -- area > 2��ʾdock���ϻ�����
end
-- ��menuBar�����һ����ΪHelp�Ĳ˵��������������dock�Ŀ���
mainWindow.menuBar{
    QMenu(tr("&Help")){
        dynamicHelp.dock.toggleViewAction
    }
}
--mdiArea:addSubWindow(splitter):show()











