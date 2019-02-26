inlfile=$1/src/debugger.inl
echo 'const char* debuggee_code = R"_(' >${inlfile}
cat $1/debuggee.lua >>${inlfile}
echo ')_";' >>${inlfile}
echo "" >>${inlfile}
echo 'const char* debugger_code = R"_(' >>${inlfile}
cat $1/debugger.lua >>${inlfile}
echo ')_";' >>${inlfile}
