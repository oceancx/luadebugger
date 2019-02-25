--this file is only used debuggee thread
cjson = require 'cjson'

function create_on_message_parser()
    local parsed_len = -1
    local readstate = 1
    local LINE_ENDING = nil
    return function(conn,buf,netq)
        while buf:readablesize() > 0 do 
            local preview = buf:preview(buf:readablesize())
            if readstate == 1 then
                local s,e = preview:find("\n")
                if s then
                    if not LINE_ENDING then
                        local s,e = preview:find("\r\n")
                        LINE_ENDING = s and "\r\n" or "\n"
                        set_line_ending_in_c(LINE_ENDING)
                    end
                    local line = buf:readstring(e)
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
                    local line = buf:readstring(e)
                    readstate = readstate+1
                else 
                    break
                end
            elseif readstate == 3 then
                if buf:readablesize() >= parsed_len then
                local js  = buf:readstring(parsed_len) 
                readstate = 1
                netq:push_back(0,js)
                else 
                    break
                end
            else
                break
            end 
        end
    end
end

function debuggee_on_connection(conn,netq)
    --print('[debuggee]' ..conn:tohostport().. ' ' ..(conn:connected() and 'connected' or 'disconnected'))
    netq:push_back(0,cjson.encode({ type='debuggee', event='connection_state', connected = conn:connected(), addr = conn:tohostport()  }))
end

local debuggee_on_message_parser = create_on_message_parser()
function debuggee_on_message(conn,buf,netq)
    debuggee_on_message_parser(conn,buf,netq)
end