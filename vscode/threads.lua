--此文件只能包含函数，可能有不同的luastate dofile这个文件，只执行某一个函数


local thread_name = ...

function dump_table(t)
    for k,v in pairs(t) do
        print(k,v)
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

if thread_name == 'vscode' then
    function vscode_on_connection(conn)
        print('[vscode]' ..conn:tohostport().. ' ' ..(conn:connected() and 'true' or 'false'))
    end

    local vscode_on_message_parser = create_on_message_parser()
    function vscode_on_message(conn,buf,netq)
        vscode_on_message_parser(conn,buf,netq)
    end
elseif thread_name == 'stdio' then


elseif thread_name =='runtime' then

    function runtime_on_connection(conn)
        print('[runtime]' ..conn:tohostport().. ' ' ..(conn:connected() and 'true' or 'false'))
    end

    local runtime_on_message_parser = create_on_message_parser()
    function runtime_on_message(conn,buf,netq)
        runtime_on_message_parser(conn,buf,netq)
    end

end

