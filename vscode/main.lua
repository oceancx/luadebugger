


local WORK_CWD
local PORT
local event_loop
local runtime_netq = net_thread_queue_create()
local vscode_handler 
local runtime_handler 
local runtime_client

function luadbg_is_stdio_mode()
    return PORT==0  
end

-- Content-Length: 312

-- {"command":"initialize","arguments":{"clientID":"vscode","clientName":"Visual Studio Code","adapterID":"cxlua","pathFormat":"path","linesStartAt1":true,"columnsStartAt1":true,"supportsVariableType":true,"supportsVariablePaging":true,"supportsRunInTerminalRequest":true,"locale":"zh-cn"},"type":"request","seq":1}

log_trace = function(...)
    if luadbg_is_stdio_mode() then
        local path = get_default_cwd()..'dbg.log'
        local file = io.open(path,'a+')
        local args = {...}
        for i,v in ipairs(args) do
            file:write(v..'\t')
        end
        file:write('\n')
        file:close()
    else
        print(...)
    end
end

function create_on_message_parser()
    local parsed_len = -1
    local readstate = 1
    local LINE_ENDING = nil
    return function(conn,buf,netq)
        while buf:readable_size() > 0 do
            local preview = buf:Preview(buf:readable_size())
            if readstate == 1 then
                local s,e = preview:find("\n")
                if s then
                    if not LINE_ENDING then
                        local s,e = preview:find("\r\n")
                        LINE_ENDING = s and "\r\n" or "\n"
                        set_line_ending_in_c(LINE_ENDING)
                    end
                    local line = buf:ReadAsString(e)
                    local match = line:gmatch("Content%-Length: (%d*)")()
                    if tonumber(match) then
                        parsed_len = tonumber(match)
                        readstate = readstate + 1
                    else
                        break
                    end
                else
                    break
                end
            elseif readstate == 2 then
                local s,e = preview:find("\n")
                if s then
                    local line = buf:ReadAsString(e)
                    readstate = readstate+1
                else
                    break
                end
            elseif readstate == 3 then
                if buf:readable_size() >= parsed_len then
                    netq:push_back(0,buf,parsed_len)
                    buf:Consume(parsed_len)
                    readstate = 1
                else
                    break
                end
            else
                break
            end
        end
    end
end

local runtime_parser = create_on_message_parser()
function vscode_on_attach_cmd(ip, port)
    if not runtime_handler then
        runtime_client = ez_tcp_client_create(event_loop, ip, port, 'Runtime')
        runtime_client:set_on_connection(function(conn)
            log_trace('[runtime]' ..conn:tohostport().. ' ' ..(conn:connected() and 'true' or 'false'))
            if conn:connected() then
                runtime_handler = conn
            else
                runtime_handler = nil
                if luadbg_is_stdio_mode() then
                    os.exit()
                end
            end
        end)
        runtime_client:set_on_message(function(conn, buf, ts)
            log_trace('runtime_client on message')
            runtime_parser(conn,buf,runtime_netq)
        end)

        runtime_client:Connect()
    end
    while not runtime_handler do
        sleepms(10)
    end
    log_trace('connected ')
end



local message_seq = 1
function vscode_final_send(js)
    log_trace('vscode_final_send', js)
    local buf = {}
    table.insert(buf,"Content-Length: "..js:len())
    table.insert(buf,"")
    table.insert(buf,js)
    local sent = table.concat(buf,get_line_ending_in_c())
    if luadbg_is_stdio_mode() then
        print(sent)
    else
        if not vscode_handler then return end
        vscode_handler:send(sent)
    end
end

function vscode_send_response(req)
    local resp = {}
    resp.type = 'response'
    resp.command = req.command
    resp.request_seq = req.seq
    resp.success = true
    resp.seq = message_seq
    message_seq = message_seq + 1        
    if req.body then
        resp.body = req.body
    end

    vscode_final_send(cjson.encode(resp))
end

function vscode_send_event(ev)
    local event = {}
    event.type = 'event'
    event.event = ev
    event.seq = message_seq
    message_seq = message_seq + 1

    vscode_final_send(cjson.encode(event))
end


function runtime_final_send(js)
    log_trace('runtime_final_send', js)

    if not runtime_handler then return end
    local buf = {}
    table.insert(buf,"Content-Length: "..js:len())
    table.insert(buf,"")
    table.insert(buf,js)
    local sent = table.concat(buf,get_line_ending_in_c())
    runtime_handler:send(sent)
end

function dispatch_runtime_message(js)
    log_trace(string.format("\nDA <= RT:\n%s\n", js))
    vscode_final_send(js)
end


function runtime_send_response(req)
    local resp = {}
    resp.type = 'response'
    resp.command = req.command
    resp.request_seq = req.seq
    resp.success = true
    resp.body = req.body
    resp.seq = message_seq
    message_seq = message_seq + 1        

    runtime_final_send(cjson.encode(resp))
end

function runtime_send_event(ev)
    local event = {}
    event.type = 'event'
    event.event = ev
    event.seq = message_seq
    message_seq = message_seq + 1

    runtime_final_send(cjson.encode(event))
end


function dispatch_vscode_message(js)
    log_trace(string.format("\nDA <= VS:\n%s\n", js))

    local req = cjson.decode(js)
     if req.type == 'request' then
        local cmd = req.command
        if cmd == "initialize" then
            req.body = {
                supportsConfigurationDoneRequest = true,
                supportsEvaluateForHovers = true,
                supportsStepBack = true
            }
            vscode_send_response(req)
            vscode_send_event('initialized')
        elseif cmd == "launch" then
            local arguments = req.arguments
            local ip = arguments.ip
            local port = math.tointeger(arguments.port)
            local cmd = arguments.launchcmd

            vscode_on_launch_cmd('start '..cmd,ip,port)
            runtime_final_send(cjson.encode(req))
        elseif cmd == "attach" then
            local arguments = req.arguments
            local ip = arguments.ip
            local port = math.tointeger(arguments.port)
            local cwd = arguments.cwd
            vscode_on_attach_cmd(ip,port)
            runtime_final_send(cjson.encode(req))
        elseif cmd == "disconnect" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "restart" then
            vscode_send_response(req)
        elseif cmd == "setBreakpoints" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "setFunctionBreakpoints" then
            vscode_send_response(req)
        elseif cmd == "setExceptionBreakpoints" then
            vscode_send_response(req)
        elseif cmd == "configurationDone" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "continue" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "next" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepIn" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepOut" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepOver" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepBack" then
            
        elseif cmd == "reverseContinue" then
            
        elseif cmd == "restartFrame" then
            
        elseif cmd == "goto" then
            
        elseif cmd == "pause" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stackTrace" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "scopes" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "variables" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "setVariable" then

        elseif cmd == "source" then
            
        elseif cmd == "threads" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "evaluate" then
            runtime_final_send(cjson.encode(req))
        elseif cmd == "stepInTargets" then
        elseif cmd == "gotoTargets" then
        elseif cmd == "completions" then
        elseif cmd == "exceptionInfo" then
        elseif cmd == "loadedSources" then
            vscode_send_response(req)
        end
    end
end

local parsed_len = -1
local readstate = 1
function stdio_on_message(buf,netq, runtime_netq)
    while buf:readable_size() > 0 do
        local preview = buf:Preview(buf:readable_size())
        if readstate == 1 then
            local s,e = preview:find("\n")
            if s then
                if not LINE_ENDING then
                    local s,e = preview:find("\r\n")
                    LINE_ENDING = s and "\r\n" or "\n"
                    set_line_ending_in_c(LINE_ENDING)
                end
                local line = buf:ReadAsString(e)
                local match = line:gmatch("Content%-Length: (%d*)")()
                if tonumber(match) then
                    parsed_len = tonumber(match)
                    readstate = readstate + 1
                else
                    break
                end
            else
                break
            end
        elseif readstate == 2 then
            local s,e = preview:find("\n")
            if s then
                local line = buf:ReadAsString(e)
                readstate = readstate+1
            else
                break
            end
        elseif readstate == 3 then
            if buf:readable_size() >= parsed_len then
                local js  = buf:ReadAsString(parsed_len)
                readstate = 1
                dispatch_vscode_message(js)
            else
                break
            end
        else
            break
        end
    end
end

-- function DA_in_break()
--     local vs_netq = fetch_vscode_netq()
--     while not vs_netq:empty(0) do
--         log_trace('DA_in_break vs_netq')
--         local msg = vs_netq:front_as_string(0)
--         vs_netq:pop_front(0)
--         dispatch_vscode_message(msg)
--     end
--     local rt_netq = fetch_runtime_netq()
--     while not rt_netq:empty(0) do
--         log_trace('DA_in_break rt_netq')
--         local msg = rt_netq:front_as_string(0)
--         rt_netq:pop_front(0)
--         dispatch_runtime_message(msg)
--     end
-- end

local stdio_parser = create_on_message_parser()
local stdco = coroutine.create(function()
    local buf = ezio_buffer_create()
    while true do
        local c = io.read(1)
        if not c then break end
        buf:WriteByte(string.byte(c))
        stdio_parser(nil,buf,vscode_netq,true)
    end
end)

function cx_server_on_connect(conn)
    log_trace('[vscode]' ..conn:tohostport().. ' ' ..(conn:connected() and 'true' or 'false'))
    if conn:connected() then
        vscode_handler = conn
    else
        vscode_handler = nil
    end
end

do
    WORK_CWD = command_arg_opt_str('cwd',get_default_cwd())
    PORT =  command_arg_opt_int('port',0)
    log_trace(WORK_CWD, PORT)

    event_loop = ez_event_loop_create()
    if luadbg_is_stdio_mode() then
        start_stdio_read_thread()
    else
        cx_server = ez_tcp_server_create(event_loop, PORT,'VscodeServer')
        cx_server:set_on_connection(cx_server_on_connect)        
        local vscode_on_message_parser = create_on_message_parser()
        cx_server:set_on_message(function(conn, buf, ts)
            vscode_on_message_parser(conn,buf,vscode_netq)
        end)       
        cx_server:Start()
    end
    
    event_loop:RunTaskEvery(function()    
        while not vscode_netq:empty(0) do
            local msg = vscode_netq:front_as_string(0)
            vscode_netq:pop_front(0)
            dispatch_vscode_message(msg)
        end

        while not runtime_netq:empty(0) do
            local msg = runtime_netq:front_as_string(0)
            runtime_netq:pop_front(0)
            dispatch_runtime_message(msg)
        end
    end,10)

    event_loop:Run()
    
    
    -- if luadbg_is_stdio_mode() then
    --     local buf = ezio_buffer_create()
    --     while true do
    --         local c = io.read(1)
    --         if not c or not is_debugger_adapter_run() then break end
    --         buf:WriteByte(string.byte(c))
    --         stdio_on_message(buf,fetch_vscode_netq(),fetch_runtime_netq())
    --     end
    -- else
    --     while is_debugger_adapter_run() do
    --         local vs_netq = fetch_vscode_netq()
    --         while not vs_netq:empty(0) do
    --             local msg = vs_netq:front_as_string(0)
    --             vs_netq:pop_front(0)
    --             dispatch_vscode_message(msg)
    --         end
    --         local rt_netq = fetch_runtime_netq()
    --         while not rt_netq:empty(0) do
    --             local msg = rt_netq:front_as_string(0)
    --             rt_netq:pop_front(0)
    --             dispatch_runtime_message(msg)
    --         end
    --     end
    -- end

    
end